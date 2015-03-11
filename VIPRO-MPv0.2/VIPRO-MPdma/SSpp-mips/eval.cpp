#include <iostream>
#include <stdio.h>
#include <errno.h>
#include "eval.hpp"
#include "sim.hpp"

namespace std { extern "C" int errno; }



eval::eval(sim *S)
{
	Sim = S;
	
	tok_map_initialized = FALSE;
	eval_error = ERR_NOERR;	
	
	//confirmar veracidade disso
	err_value.type = et_int;
	err_value.value.as_int = 0;
}

/* builds the first token map */
void
eval::init_tok_map(void)
{
  int i;

  for (i=0; i<256; i++)
    tok_map[i] = tok_invalid;

  /* identifier characters */
  for (i='a'; i<='z'; i++)
    tok_map[i] = tok_ident;
  for (i='A'; i<='Z'; i++)
    tok_map[i] = tok_ident;
  tok_map[(int)'_'] = tok_ident;
  tok_map[(int)'$'] = tok_ident;

  /* numeric characters */
  for (i='0'; i<='9'; i++)
    tok_map[i] = tok_const;
  tok_map[(int)'.'] = tok_const;

  /* operator characters */
  tok_map[(int)'+'] = tok_plus;
  tok_map[(int)'-'] = tok_minus;
  tok_map[(int)'*'] = tok_mult;
  tok_map[(int)'/'] = tok_div;
  tok_map[(int)'('] = tok_oparen;
  tok_map[(int)')'] = tok_cparen;

  /* whitespace characers */
  tok_map[(int)' '] = tok_whitespace;
  tok_map[(int)'\t'] = tok_whitespace;
}

/* get next token from the expression string */
enum eval_token_t		/* token parsed */
eval::get_next_token(struct eval_state_t *es)	/* expression evaluator */
{
  int allow_hex;
  enum eval_token_t tok;
  char *ptok_buf, last_char;

  /* initialize the token map, if needed */
  if (!tok_map_initialized)
    {
      init_tok_map();
      tok_map_initialized = TRUE;
    }

  /* use the peek'ed token, if available, tok_buf should still be valid */
  if (es->peek_tok != tok_invalid)
    {
      tok = es->peek_tok;
      es->peek_tok = tok_invalid;
      return tok;
    }

  /* set up the token string space */
  ptok_buf = es->tok_buf;
  *ptok_buf = '\0';

  /* skip whitespace */
  while (*es->p && tok_map[(int)*es->p] == tok_whitespace)
    es->p++;

  /* end of token stream? */
  if (*es->p == '\0')
    return tok_eof;

  *ptok_buf++ = *es->p;
  tok = tok_map[(int)*es->p++];
  switch (tok)
    {
    case tok_ident:
      /* parse off next identifier */
      while (*es->p
	     && (tok_map[(int)*es->p] == tok_ident
		 || tok_map[(int)*es->p] == tok_const))
	{
	  *ptok_buf++ = *es->p++;
	}
      break;
    case tok_const:
      /* parse off next numeric literal */
      last_char = '\0';
      allow_hex = FALSE;
      while (*es->p &&
	     (tok_map[(int)*es->p] == tok_const
	      || (*es->p == '-' && last_char == 'e')
	      || (*es->p == '+' && last_char == 'e')
	      || tolower(*es->p) == 'e'
	      || tolower(*es->p) == 'x'
	      || (tolower(*es->p) == 'a' && allow_hex)
	      || (tolower(*es->p) == 'b' && allow_hex)
	      || (tolower(*es->p) == 'c' && allow_hex)
	      || (tolower(*es->p) == 'd' && allow_hex)
	      || (tolower(*es->p) == 'e' && allow_hex)
	      || (tolower(*es->p) == 'f' && allow_hex)))
	{
	  last_char = tolower(*es->p);
	  if (*es->p == 'x' || *es->p == 'X')
	    allow_hex = TRUE;
	  *ptok_buf++ = *es->p++;
	}
      break;
    case tok_plus:
    case tok_minus:
    case tok_mult:
    case tok_div:
    case tok_oparen:
    case tok_cparen:
      /* just pass on the token */
      break;
    default:
      tok = tok_invalid;
      break;
    }

  /* terminate the token string buffer */
  *ptok_buf = '\0';

  return tok;
}

/* peek ahead at the next token from the expression stream, currently
   only the next token can be peek'ed at */
enum eval_token_t		 /* next token in expression */
eval::peek_next_token(struct eval_state_t *es) /* expression evalutor */
{
  /* if there is no peek ahead token, get one */
  if (es->peek_tok == tok_invalid)
    {
      es->lastp = es->p;
      es->peek_tok = get_next_token(es);
    }

  /* return peek ahead token */
  return es->peek_tok;
}


/* determine necessary arithmetic conversion on T1 <op> T2 */
enum eval_type_t			/* type of expression result */
eval::result_type(enum eval_type_t t1,	/* left operand type */
	    enum eval_type_t t2)	/* right operand type */
{
  /* sanity check, symbols should not show up in arithmetic exprs */
  if (t1 == et_symbol || t2 == et_symbol)
    Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");

  /* using C rules, i.e., A6.5 */
  if (t1 == et_double || t2 == et_double)
    return et_double;
  else if (t1 == et_float || t2 == et_float)
    return et_float;
#ifdef HOST_HAS_QUAD
  else if (t1 == et_quad || t2 == et_quad)
    return et_quad;
  else if (t1 == et_squad || t2 == et_squad)
    return et_squad;
#endif /* HOST_HAS_QUAD */
  else if (t1 == et_addr || t2 == et_addr)
    return et_addr;
  else if (t1 == et_uint || t2 == et_uint)
    return et_uint;
  else
    return et_int;
}

/*
 * expression value arithmetic conversions
 */

/* eval_value_t (any numeric type) -> double */
double
eval::eval_as_double(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return val.value.as_double;
    case et_float:
      return (double)val.value.as_float;
#ifdef HOST_HAS_QUAD
    case et_quad:
#ifdef _MSC_VER /* FIXME: MSC does not implement quad_t to double conversion */
      return (double)(squad_t)val.value.as_quad;
#else /* !_MSC_VER */
      return (double)val.value.as_quad;
#endif /* _MSC_VER */
    case et_squad:
      return (double)val.value.as_squad;
#endif /* HOST_HAS_QUAD */
    case et_addr:
#if defined(_MSC_VER) && defined(TARGET_ALPHA)
      /* FIXME: MSC does not implement quad_t to double conversion */
      return (double)(squad_t)val.value.as_addr;
#else
      return (double)val.value.as_addr;
#endif
    case et_uint:
      return (double)val.value.as_uint;
    case et_int:
      return (double)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

/* eval_value_t (any numeric type) -> float */
float
eval::eval_as_float(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (float)val.value.as_double;
    case et_float:
      return val.value.as_float;
#ifdef HOST_HAS_QUAD
    case et_quad:
#ifdef _MSC_VER /* FIXME: MSC does not implement quad_t to double conversion */
      return (float)(squad_t)val.value.as_quad;
#else /* !_MSC_VER */
      return (float)val.value.as_quad;
#endif /* _MSC_VER */
    case et_squad:
      return (float)val.value.as_squad;
#endif /* HOST_HAS_QUAD */
    case et_addr:
#if defined(_MSC_VER) && defined(TARGET_ALPHA)
      /* FIXME: MSC does not implement quad_t to double conversion */
      return (float)(squad_t)val.value.as_addr;
#else
      return (float)val.value.as_addr;
#endif
    case et_uint:
      return (float)val.value.as_uint;
    case et_int:
      return (float)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

#ifdef HOST_HAS_QUAD
/* eval_value_t (any numeric type) -> quad_t */
quad_t
eval::eval_as_quad(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (quad_t)val.value.as_double;
    case et_float:
      return (quad_t)val.value.as_float;
    case et_quad:
      return val.value.as_quad;
    case et_squad:
      return (quad_t)val.value.as_squad;
    case et_addr:
      return (quad_t)val.value.as_addr;
    case et_uint:
      return (quad_t)val.value.as_uint;
    case et_int:
      return (quad_t)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

/* eval_value_t (any numeric type) -> squad_t */
squad_t
eval::eval_as_squad(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (squad_t)val.value.as_double;
    case et_float:
      return (squad_t)val.value.as_float;
    case et_quad:
      return (squad_t)val.value.as_quad;
    case et_squad:
      return val.value.as_squad;
    case et_addr:
      return (squad_t)val.value.as_addr;
    case et_uint:
      return (squad_t)val.value.as_uint;
    case et_int:
      return (squad_t)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}
#endif /* HOST_HAS_QUAD */

/* eval_value_t (any numeric type) -> unsigned int */
md_addr_t
eval::eval_as_addr(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (md_addr_t)val.value.as_double;
    case et_float:
      return (md_addr_t)val.value.as_float;
#ifdef HOST_HAS_QUAD
    case et_quad:
      return (md_addr_t)val.value.as_quad;
    case et_squad:
      return (md_addr_t)val.value.as_squad;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      return val.value.as_addr;
    case et_uint:
      return (md_addr_t)val.value.as_uint;
    case et_int:
      return (md_addr_t)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

/* eval_value_t (any numeric type) -> unsigned int */
unsigned int
eval::eval_as_uint(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (unsigned int)val.value.as_double;
    case et_float:
      return (unsigned int)val.value.as_float;
#ifdef HOST_HAS_QUAD
    case et_quad:
      return (unsigned int)val.value.as_quad;
    case et_squad:
      return (unsigned int)val.value.as_squad;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      return (unsigned int)val.value.as_addr;
    case et_uint:
      return val.value.as_uint;
    case et_int:
      return (unsigned int)val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

/* eval_value_t (any numeric type) -> int */
int
eval::eval_as_int(struct eval_value_t val)
{
  switch (val.type)
    {
    case et_double:
      return (int)val.value.as_double;
    case et_float:
      return (int)val.value.as_float;
#ifdef HOST_HAS_QUAD
    case et_quad:
      return (int)val.value.as_quad;
    case et_squad:
      return (int)val.value.as_squad;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      return (int)val.value.as_addr;
    case et_uint:
      return (int)val.value.as_uint;
    case et_int:
      return val.value.as_int;
    case et_symbol:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "symbol used in expression");
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "illegal arithmetic expression conversion");
    }
}

/*
 * arithmetic intrinsics operations, used during expression evaluation
 */

/* compute <val1> + <val2> */
struct eval_value_t
eval::f_add(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = eval_as_double(val1) + eval_as_double(val2);
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = eval_as_float(val1) + eval_as_float(val2);
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val.type = et_quad;
      val.value.as_quad = eval_as_quad(val1) + eval_as_quad(val2);
      break;
    case et_squad:
      val.type = et_squad;
      val.value.as_squad = eval_as_squad(val1) + eval_as_squad(val2);
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = eval_as_addr(val1) + eval_as_addr(val2);
      break;
    case et_uint:
      val.type = et_uint;
      val.value.as_uint = eval_as_uint(val1) + eval_as_uint(val2);
      break;
    case et_int:
      val.type = et_int;
      val.value.as_int = eval_as_int(val1) + eval_as_int(val2);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* compute <val1> - <val2> */
struct eval_value_t
eval::f_sub(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = eval_as_double(val1) - eval_as_double(val2);
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = eval_as_float(val1) - eval_as_float(val2);
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val.type = et_quad;
      val.value.as_quad = eval_as_quad(val1) - eval_as_quad(val2);
      break;
    case et_squad:
      val.type = et_squad;
      val.value.as_squad = eval_as_squad(val1) - eval_as_squad(val2);
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = eval_as_addr(val1) - eval_as_addr(val2);
      break;
    case et_uint:
      val.type = et_uint;
      val.value.as_uint = eval_as_uint(val1) - eval_as_uint(val2);
      break;
    case et_int:
      val.type = et_int;
      val.value.as_int = eval_as_int(val1) - eval_as_int(val2);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* compute <val1> * <val2> */
struct eval_value_t
eval::f_mult(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = eval_as_double(val1) * eval_as_double(val2);
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = eval_as_float(val1) * eval_as_float(val2);
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val.type = et_quad;
      val.value.as_quad = eval_as_quad(val1) * eval_as_quad(val2);
      break;
    case et_squad:
      val.type = et_squad;
      val.value.as_squad = eval_as_squad(val1) * eval_as_squad(val2);
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = eval_as_addr(val1) * eval_as_addr(val2);
      break;
    case et_uint:
      val.type = et_uint;
      val.value.as_uint = eval_as_uint(val1) * eval_as_uint(val2);
      break;
    case et_int:
      val.type = et_int;
      val.value.as_int = eval_as_int(val1) * eval_as_int(val2);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* compute <val1> / <val2> */
struct eval_value_t
eval::f_div(struct eval_value_t val1, struct eval_value_t val2)
{
  enum eval_type_t et;
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol || val2.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* get result type, and perform operation in that type */
  et = result_type(val1.type, val2.type);
  switch (et)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = eval_as_double(val1) / eval_as_double(val2);
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = eval_as_float(val1) / eval_as_float(val2);
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val.type = et_quad;
      val.value.as_quad = eval_as_quad(val1) / eval_as_quad(val2);
      break;
    case et_squad:
      val.type = et_squad;
      val.value.as_squad = eval_as_squad(val1) / eval_as_squad(val2);
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = eval_as_addr(val1) / eval_as_addr(val2);
      break;
    case et_uint:
      val.type = et_uint;
      val.value.as_uint = eval_as_uint(val1) / eval_as_uint(val2);
      break;
    case et_int:
      val.type = et_int;
      val.value.as_int = eval_as_int(val1) / eval_as_int(val2);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* compute - <val1> */
struct eval_value_t
eval::f_neg(struct eval_value_t val1)
{
  struct eval_value_t val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return err_value;
    }

  /* result type is the same as the operand type */
  switch (val1.type)
    {
    case et_double:
      val.type = et_double;
      val.value.as_double = - val1.value.as_double;
      break;
    case et_float:
      val.type = et_float;
      val.value.as_float = - val1.value.as_float;
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val.type = et_squad;
      val.value.as_quad = - (squad_t)val1.value.as_quad;
      break;
    case et_squad:
      val.type = et_squad;
      val.value.as_squad = - val1.value.as_squad;
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val.type = et_addr;
      val.value.as_addr = - val1.value.as_addr;
      break;
    case et_uint:
      if ((unsigned int)val1.value.as_uint > 2147483648U)
	{
	  /* promote type */
#ifdef HOST_HAS_QUAD
	  val.type = et_squad;
	  val.value.as_squad = - ((squad_t)val1.value.as_uint);
#else /* !HOST_HAS_QUAD */
	  val.type = et_double;
	  val.value.as_double = - ((double)val1.value.as_uint);
#endif /* HOST_HAS_QUAD */
	}
      else
	{
	  /* don't promote type */
	  val.type = et_int;
	  val.value.as_int = - ((int)val1.value.as_uint);
	}
      break;
    case et_int:
      if ((unsigned int)val1.value.as_int == 0x80000000U)
	{
	  /* promote type */
	  val.type = et_uint;
	  val.value.as_uint = 2147483648U;
	}
      else
	{
	  /* don't promote type */
	  val.type = et_int;
	  val.value.as_int = - val1.value.as_int;
	}
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* compute val1 == 0 */
int
eval::f_eq_zero(struct eval_value_t val1)
{
  int val;

  /* symbols are not allowed in arithmetic expressions */
  if (val1.type == et_symbol)
    {
      eval_error = ERR_BADEXPR;
      return FALSE;
    }

  switch (val1.type)
    {
    case et_double:
      val = val1.value.as_double == 0.0;
      break;
    case et_float:
      val = val1.value.as_float == 0.0;
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      val = val1.value.as_quad == 0;
      break;
    case et_squad:
      val = val1.value.as_squad == 0;
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
      val = val1.value.as_addr == 0;
      break;
    case et_uint:
      val = val1.value.as_uint == 0;
      break;
    case et_int:
      val = val1.value.as_int == 0;
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }

  return val;
}

/* evaluate the value of the numeric literal constant in ES->TOK_BUF,
   eval_err is set to a value other than ERR_NOERR if the constant cannot
   be parsed and converted to an expression value */
struct eval_value_t		/* value of the literal constant */
eval::constant(struct eval_state_t *es)	/* expression evaluator */
{
  struct eval_value_t val;
  int int_val;
  unsigned int uint_val;
  double double_val;
  char *endp;
#ifdef HOST_HAS_QUAD
  squad_t squad_val;
  quad_t quad_val;
#endif /* HOST_HAS_QUAD */

#if !defined(__CYGWIN32__)
//  extern int errno;
#endif
#if 0 /* no longer needed... */
#if defined(sparc) && !defined(__svr4__)
  extern long strtol(char *, char **, int);
  extern double strtod(char *, char **);
#endif /* sparc */
#endif

  /*
   * attempt multiple conversions, from least to most precise, using
   * the value returned when the conversion is successful
   */

  /* attempt integer conversion */
  errno = 0;
  int_val = strtol(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_int;
      val.value.as_int = int_val;
      return val;
    }

  /* else, not an integer, attempt unsigned int conversion */
  errno = 0;
  uint_val = strtoul(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_uint;
      val.value.as_uint = uint_val;
      return val;
    }

#ifdef HOST_HAS_QUAD
  /* else, not an int/uint, attempt squad_t conversion */
  errno = 0;
  squad_val = Sim->MISC->myatosq(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_squad;
      val.value.as_squad = squad_val;
      return val;
    }

  /* else, not an squad_t, attempt quad_t conversion */
  errno = 0;
  quad_val = Sim->MISC->myatoq(es->tok_buf, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_quad;
      val.value.as_quad = quad_val;
      return val;
    }
#endif /* HOST_HAS_QUAD */

  /* else, not any type of integer, attempt double conversion (NOTE: no
     reliable float conversion is available on all machines) */
  errno = 0;
  double_val = strtod(es->tok_buf, &endp);
  if (!errno && !*endp)
    {
      /* good conversion */
      val.type = et_double;
      val.value.as_double = double_val;
      return val;
    }

  /* else, not a double value, therefore, could not convert constant,
     declare an error */
  eval_error = ERR_BADCONST;
  return err_value;
}

/* evaluate an expression factor, eval_err will indicate it any
   expression evaluation occurs */
struct eval_value_t		/* value of factor */
eval::factor(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_oparen:
      (void)get_next_token(es);
      val = expr(es);
      if (eval_error)
	return err_value;

      tok = peek_next_token(es);
      if (tok != tok_cparen)
	{
	  eval_error = ERR_UPAREN;
	  return err_value;
	}
      (void)get_next_token(es);
      break;

    case tok_minus:
      /* negation operator */
      (void)get_next_token(es);
      val = factor(es);
      if (eval_error)
	return err_value;
      val = f_neg(val);
      break;

    case tok_ident:
      (void)get_next_token(es);
      /* evaluate the identifier in TOK_BUF */
      //val = es->f_eval_ident(es);
      val = Sim->STATS->stat_eval_ident(es);
      if (eval_error)
	return err_value;
      break;

    case tok_const:
      (void)get_next_token(es);
      val = constant(es);
      if (eval_error)
	return err_value;
      break;

    default:
      eval_error = ERR_NOTERM;
      return err_value;
    }

  return val;
}

/* evaluate an expression term, eval_err will indicate it any
   expression evaluation occurs */
struct eval_value_t		/* value to expression term */
eval::term(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val, val1;

  val = factor(es);
  if (eval_error)
    return err_value;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_mult:
      (void)get_next_token(es);
      val = f_mult(val, term(es));
      if (eval_error)
	return err_value;
      break;

    case tok_div:
      (void)get_next_token(es);
      val1 = term(es);
      if (eval_error)
	return err_value;
      if (f_eq_zero(val1))
	{
	  eval_error = ERR_DIV0;
	  return err_value;
	}
      val = f_div(val, val1);
      break;

    default:;
    }

  return val;
}

/* evaluate an expression, eval_err will indicate it any expression
   evaluation occurs */
struct eval_value_t		/* value of the expression */
eval::expr(struct eval_state_t *es)		/* expression evaluator */
{
  enum eval_token_t tok;
  struct eval_value_t val;

  val = term(es);
  if (eval_error)
    return err_value;

  tok = peek_next_token(es);
  switch (tok)
    {
    case tok_plus:
      (void)get_next_token(es);
      val = f_add(val, expr(es));
      if (eval_error)
	return err_value;
      break;

    case tok_minus:
      (void)get_next_token(es);
      val = f_sub(val, expr(es));
      if (eval_error)
	return err_value;
      break;

    default:;
    }

  return val;
}

/* create an evaluator */
eval_state_t *			/* expression evaluator */
eval::eval_new(/*eval_ident_t f_eval_ident,*/	/* user ident evaluator */
	        void *user_ptr)		/* user ptr passed to ident fn */
{
  struct eval_state_t *es;

  es = (struct eval_state_t*) calloc(1, sizeof(struct eval_state_t));
  if (!es)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

//  es->f_eval_ident = f_eval_ident;
  es->user_ptr = user_ptr;

  return es;
}

/* delete an evaluator */
void
eval::eval_delete(struct eval_state_t *es)	/* evaluator to delete */
{
  free(es);
}

/* evaluate an expression, if an error occurs during evaluation, the
   global variable eval_error will be set to a value other than ERR_NOERR */
struct eval_value_t			/* value of the expression */
eval::eval_expr(struct eval_state_t *es,	/* expression evaluator */
	  char *p,			/* ptr to expression string */
	  char **endp)			/* returns ptr to 1st unused char */
{
  struct eval_value_t val;

  /* initialize the evaluator state */
  eval_error = ERR_NOERR;
  es->p = p;
  *es->tok_buf = '\0';
  es->peek_tok = tok_invalid;

  /* evaluate the expression */
  val = expr(es);

  /* return a pointer to the first character not used in the expression */
  if (endp)
    {
      if (es->peek_tok != tok_invalid)
	{
	  /* did not consume peek'ed token, so return last p */
	  *endp = es->lastp;
	}
      else
	*endp = es->p;
    }

  return val;
}

/* print an expression value */
void
eval::eval_print(FILE *stream,		/* output stream */
	   struct eval_value_t val)	/* expression value to print */
{
  switch (val.type)
    {
    case et_double:
      fprintf(stream, "%f [double]", val.value.as_double);
      break;
    case et_float:
      fprintf(stream, "%f [float]", (double)val.value.as_float);
      break;
#ifdef HOST_HAS_QUAD
    case et_quad:
      Sim->MISC->myfprintf(stream, "%lu [quad_t]", val.value.as_quad);
      break;
    case et_squad:
      Sim->MISC->myfprintf(stream, "%ld [squad_t]", val.value.as_squad);
      break;
#endif /* HOST_HAS_QUAD */
    case et_addr:
     Sim->MISC-> myfprintf(stream, "0x%p [md_addr_t]", val.value.as_addr);
      break;
    case et_uint:
      fprintf(stream, "%u [uint]", val.value.as_uint);
      break;
    case et_int:
      fprintf(stream, "%d [int]", val.value.as_int);
      break;
    case et_symbol:
      fprintf(stream, "\"%s\" [symbol]", val.value.as_symbol);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus expression type");
    }
}







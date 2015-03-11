#ifndef EVAL_H
#define EVAL_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"





/* an identifier evaluator, when an evaluator is instantiated, the user
   must supply a function of this type that returns the value of identifiers
   encountered in expressions */

// typedef struct eval_value_t  		  /* value of the identifier */
// (*eval_ident_t)(struct eval_state_t *es);      /* ident string in es->tok_buf */

/* expression tokens */
enum eval_token_t {
  tok_ident,		/* user-valued identifiers */
  tok_const,		/* numeric literals */
  tok_plus,		/* `+' */
  tok_minus,		/* `-' */
  tok_mult,		/* `*' */
  tok_div,		/* `/' */
  tok_oparen,		/* `(' */
  tok_cparen,		/* `)' */
  tok_eof,		/* end of file */
  tok_whitespace,	/* ` ', `\t', `\n' */
  tok_invalid		/* unrecognized token */
};

/* an evaluator state record */
struct eval_state_t {
  char *p;			/* ptr to next char to consume from expr */
  char *lastp;			/* save space for token peeks */
  //eval_ident_t f_eval_ident;	/* identifier evaluator */
  void *user_ptr;		/* user-supplied argument pointer */
  char tok_buf[512];		/* text of last token returned */
  enum eval_token_t peek_tok;	/* peek buffer, for one token look-ahead */
};

/* evaluation errors */
enum eval_err_t {
  ERR_NOERR,			/* no error */
  ERR_UPAREN,			/* unmatched parenthesis */
  ERR_NOTERM,			/* expression term is missing */
  ERR_DIV0,			/* divide by zero */
  ERR_BADCONST,			/* badly formed constant */
  ERR_BADEXPR,			/* badly formed constant */
  ERR_UNDEFVAR,			/* variable is undefined */
  ERR_EXTRA,			/* extra characters at end of expression */
  ERR_NUM
};

/* expression evaluation error, this must be a global */
//extern enum eval_err_t eval_error /* = ERR_NOERR */;

/* enum eval_err_t -> error description string map */
//extern char *eval_err_str[ERR_NUM];

/* expression value types */
enum eval_type_t {
  et_int,			/* signed integer result */
  et_uint,			/* unsigned integer result */
  et_addr,			/* address value */
#ifdef HOST_HAS_QUAD
  et_quad,			/* unsigned quadword length integer result */
  et_squad,			/* signed quadword length integer result */
#endif /* HOST_HAS_QUAD */
  et_float,			/* single-precision floating point value */
  et_double,			/* double-precision floating point value */
  et_symbol,			/* non-numeric result (!allowed in exprs)*/
  et_NUM
};

/* non-zero if type is an integral type */
#ifdef HOST_HAS_QUAD
#define EVAL_INTEGRAL(TYPE)						\
  ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr		\
   || (TYPE) == et_quad || (TYPE) == et_squad)
#else /* !HOST_HAS_QUAD */
#define EVAL_INTEGRAL(TYPE)						\
  ((TYPE) == et_int || (TYPE) == et_uint || (TYPE) == et_addr)
#endif /* HOST_HAS_QUAD */


#if defined(sparc) && !defined(__svr4__)
#define strtoul strtol
#endif /* sparc */
/* enum eval_type_t -> expression type description string map */
//extern char *eval_type_str[et_NUM];


/* expression type strings */
char *eval_type_str[et_NUM] = {
  /* et_int */		"int",
  /* et_uint */		"unsigned int",
  /* et_addr */		"md_addr_t",
#ifdef HOST_HAS_QUAD
  /* et_quad */		"quad_t",
  /* et_squad */	"squad_t",
#endif /* HOST_HAS_QUAD */
  /* et_float */	"float",
  /* et_double */	"double",
  /* et_symbol */	"symbol"
};


/* an expression value */
struct eval_value_t {
  enum eval_type_t type;		/* type of expression value */
  union {
    int as_int;				/* value for type == et_int */
    unsigned int as_uint;		/* value for type == et_uint */
    md_addr_t as_addr;			/* value for type == et_addr */
#ifdef HOST_HAS_QUAD
    quad_t as_quad;			/* value for type == ec_quad */
    squad_t as_squad;			/* value for type == ec_squad */
#endif /* HOST_HAS_QUAD */
    float as_float;			/* value for type == et_float */
    double as_double;			/* value for type == et_double */
    char *as_symbol;			/* value for type == et_symbol */
  } value;
};


/* Declaracoes do eval.c                                              */
/* Essa declaracao esta aki porque os processadores nao vao altera-la */
/* enum eval_err_t -> error description string map */
char *eval_err_str[ERR_NUM] = {
  /* ERR_NOERR */	"!! no error!!",
  /* ERR_UPAREN */	"unmatched parenthesis",
  /* ERR_NOTERM */	"expression term is missing",
  /* ERR_DIV0 */	"divide by zero",
  /* ERR_BADCONST */	"badly formed constant",
  /* ERR_BADEXPR */	"badly formed expression",
  /* ERR_UNDEFVAR */	"variable is undefined",
  /* ERR_EXTRA */	"extra characters at end of expression"
};



/* forward declarations */
struct eval_state_t;
struct eval_value_t;

class sim;

class eval
{
	private:
	
		sim *Sim;
	
		/* *first* token character -> enum eval_token_t map */
		enum eval_token_t tok_map[256];
		int tok_map_initialized;

		/* funcoes privadas --------------------------------------------*/

		/* builds the first token map */
		void
		init_tok_map(void);
		
		/* get next token from the expression string */
		enum eval_token_t		/* token parsed */
		get_next_token(struct eval_state_t *es);

		/* peek ahead at the next token from the expression stream, currently
		   only the next token can be peek'ed at */
		enum eval_token_t		 /* next token in expression */
		peek_next_token(struct eval_state_t *es); /* expression evalutor */

		/* determine necessary arithmetic conversion on T1 <op> T2 */
		enum eval_type_t			/* type of expression result */
		result_type(enum eval_type_t t1,	/* left operand type */
			    enum eval_type_t t2);	/* right operand type */

		struct eval_value_t
		f_add(struct eval_value_t val1, struct eval_value_t val2);

		struct eval_value_t
		f_sub(struct eval_value_t val1, struct eval_value_t val2);
		
		struct eval_value_t
		f_mult(struct eval_value_t val1, struct eval_value_t val2);

		struct eval_value_t
		f_div(struct eval_value_t val1, struct eval_value_t val2);

		struct eval_value_t
		f_neg(struct eval_value_t val1);

		int
		f_eq_zero(struct eval_value_t val1);

		struct eval_value_t		
		constant(struct eval_state_t *es);
	
		struct eval_value_t		
		factor(struct eval_state_t *es);
		
		struct eval_value_t		
		term(struct eval_state_t *es);

		struct eval_value_t
		expr(struct eval_state_t *es);


	public:
	
		eval(sim*);
	
		/* expression evaluation error, this must be a global */
		enum eval_err_t eval_error;

		/* default expression error value, eval_err is also set */
		struct eval_value_t err_value;
	
		/*
		 * expression value arithmetic conversions
		 */

		/* eval_value_t (any numeric type) -> double */
		double eval_as_double(struct eval_value_t val);

		/* eval_value_t (any numeric type) -> float */
		float eval_as_float(struct eval_value_t val);

		//#ifdef HOST_HAS_QUAD
		/* eval_value_t (any numeric type) -> quad_t */
		quad_t eval_as_quad(struct eval_value_t val);

		/* eval_value_t (any numeric type) -> squad_t */
		squad_t eval_as_squad(struct eval_value_t val);
		//#endif /* HOST_HAS_QUAD */

		/* eval_value_t (any numeric type) -> md_addr_t */
		md_addr_t eval_as_addr(struct eval_value_t val);

		/* eval_value_t (any numeric type) -> unsigned int */
		unsigned int eval_as_uint(struct eval_value_t val);

		/* eval_value_t (any numeric type) -> int */
		int eval_as_int(struct eval_value_t val);

		/* create an evaluator */
		struct eval_state_t *			/* expression evaluator */
		eval_new(/*eval_ident_t f_eval_ident,*/	/* user ident evaluator */
			 void *user_ptr);		/* user ptr passed to ident fn */

		/* delete an evaluator */
		void
		eval_delete(struct eval_state_t *es);	/* evaluator to delete */

		/* evaluate an expression, if an error occurs during evaluation, the
		   global variable eval_error will be set to a value other than ERR_NOERR */
		struct eval_value_t			/* value of the expression */
		eval_expr(struct eval_state_t *es,	/* expression evaluator */
			  char *p,			/* ptr to expression string */
			  char **endp);			/* returns ptr to 1st unused char */

		/* print an expression value */
		void
		eval_print(FILE *stream,		/* output stream */
			   struct eval_value_t val);	/* expression value to print */
	
};


#endif


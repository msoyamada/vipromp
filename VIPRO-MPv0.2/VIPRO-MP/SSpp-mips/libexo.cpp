#include <iostream>    
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "libexo.hpp"
#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "sim.hpp"

namespace std { extern "C" int errno; };

/* (f)lex external defs */
extern "C"  int yylex(void);
extern "C"  int yy_nextchar(void);
extern "C" char *yytext;
extern "C" FILE *yyin;

extern "C" void yy_setstream(FILE *);
libexo::libexo(sim *S)
{
	Sim = S;
}

/* return the value of an escape sequence, ESCAPE is a pointer to the first
   character following '\', sets NEXT to first character after escape */
/* A2.5.2 */
/*/*static*/int
libexo::intern_escape(char *esc, char **next)
{
  int c, value, empty, count;

  switch (c = *esc++) {
  case 'x':
    /* \xhh hex value */
    value = 0;
    empty = TRUE;
    while (1)
      {
        c = *esc++;
        if (!(c >= 'a' && c <= 'f')
            && !(c >= 'A' && c <= 'F')
            && !(c >= '0' && c <= '9'))
          {
            esc--;
            break;
          }
        value *=16;
        if (c >= 'a' && c <= 'f')
          value += c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
          value += c - 'A' + 10;
        if (c >= '0' && c <= '9')
          value += c - '0';
        empty = FALSE;
      }
    if (empty)
      Sim->fatal("\\x used with no trailing hex digits");
    break;

  case '0': case '1': case '2': case '3':
  case '4': case '5': case '6': case '7':
    /* \ooo octal value */
    value = 0;
    count = 0;
    while ((c <= '7') && (c >= '0') && (count++ < 3))
      {
        value = (value * 8) + (c - '0');
        c = *esc++;
      }
    esc--;
    break;

  case '\\':
  case '\'':
  case '"':
    value = c;
    break;

  case 'n':
    value = '\n';
    break;

  case 't':
    value = '\t';
    break;

  case 'r':
    value = '\r';
    break;

  case 'f':
    value = '\f';
    break;

  case 'b':
    value = '\b';
    break;

  case 'a':
    value = '\a';
    break;

  case 'v':
    value = '\v';
    break;

  case '?':
    value = c;
    break;

  case '(':
  case '{':
  case '[':
  case '%':
    value = c;
    Sim->warn("non-ANSI escape sequence `\\%c'", c);
    break;

  default:
    Sim->fatal("unknown escape, '\\' followed by char %x (`%c')", (int)c, c);
  }

  if (*next)
    *next = esc;

  return value;
}

/* return the value of an character literal sequence */
/* A2.5.2 */
/*static*/int
libexo::intern_char(char *s, char **next)
{
  unsigned char value;

  if (s[0] != '\'' || s[strlen(s)-1] != '\'')
    Sim->panic("mal-formed string constant");

  if (s[1] != '\\')
    {
      value = (unsigned)s[1];
      if (s[2] != '\'')
	Sim->panic("mal-formed string constant");
      if (next)
	*next = s + 2;
    }
  else
    {
      /* escaped char constant */
      value = intern_escape(s+2, next);
    }

  /* map to a signed char value */
  value = (signed int)((unsigned char)((unsigned char)((unsigned int)value)));

  if (UCHAR_MAX < value)
    Sim->fatal("character constant out of range");

  return value;
}

/*static*/void
libexo::print_char(unsigned char c, FILE *stream)
{
  switch (c)
    {
    case '\n':
      fprintf(stream, "\\n");
      break;

    case '\\':
      fprintf(stream, "\\\\");
      break;

    case '\'':
      fprintf(stream, "\\'");
      break;

    case '\t':
      fprintf(stream, "\\t");
      break;

    case '\r':
      fprintf(stream, "\\r");
      break;

    case '\f':
      fprintf(stream, "\\f");
      break;

    case '\b':
      fprintf(stream, "\\b");
      break;

    case '\a':
      fprintf(stream, "\\a");
      break;

    case '\v':
      fprintf(stream, "\\v");
      break;

    default:
      if (isprint(c))
	fprintf(stream, "%c", c);
      else
	fprintf(stream, "\\x%02x", c);
    }
}

/* expand all escapes in string STR, return pointer to allocation w/ result */
/*static*/char *
libexo::intern_string(char *str)
{
  char *s, *istr;

  /* resulting string cannot be longer than STR */
  s = istr = (char*) malloc(strlen(str)+1);

  if (!str || !*str || *str != '\"') /* " */
    Sim->panic("mal-formed string constant");

  /* skip `"' */ /* " */
  str++;

  while (*str)
    {
      if (*str == '\\')
        *s++ = intern_escape(str+1, &str);
      else
        {
          /* A2.6 */
          if (*str == '\n')
            Sim->warn("ANSI C forbids newline in character constant");
          /* A2.6 */
          if (*str == '"' && str[1] != '\0')
            Sim->panic("encountered `\"' embedded in string constant");

          if (*str != '\"') /* " */
            *s++ = *str;
          str++;
        }
    }
  *s = '\0';
  return istr;
}

/*static*/void
libexo::print_string(unsigned char *s, FILE *stream)
{
  while (*s)
    {
      print_char(*s, stream);
      s++;
    }
}


/* hash a string */
/*static*/unsigned long
libexo::hash_str(char *s)
{
  unsigned h = 0;
  while (*s)
    h = (h << 1) + *s++;
  return (h % TOKEN_HASH_SIZE);
}

/* intern token TOKEN_STR */
struct exo_token_t *
libexo::exo_intern(char *token_str)		/* string to intern */
{
  int index;
  struct exo_token_t *ent;

  index = hash_str(token_str);

  for (ent=token_hash[index]; ent != NULL; ent=ent->next)
    {
      if (!strcmp(token_str, ent->str))
	{
	  /* got a match, return token entry */
	  return ent;
	}
    }

  /* not found, create a new entry */
  ent = (struct exo_token_t *)calloc(1, sizeof(struct exo_token_t));
  if (!ent)
    Sim->fatal("out of virtual memory");

  ent->str = Sim->MISC->mystrdup(token_str);
  ent->token = token_id++;
  ent->next = token_hash[index];
  token_hash[index] = ent;

  return ent;
}

/* intern token TOKEN_STR as value TOKEN */
struct exo_token_t *
libexo::exo_intern_as(char *token_str,		/* string to intern */
	      int token)		/* internment value */
{
  struct exo_token_t *ent;

#if 0
  if (token_id > token)
    Sim->fatal("token value is already in use");
#endif

  ent = exo_intern(token_str);
  /* overide the default value */
  ent->token = token;

#if 0
  if (ent->token != token)
    Sim->fatal("symbol `%s' was previously interned", token_str);
#endif

  return ent;
}

/* allocate an EXO node, fill in its type */
/*static*/struct exo_term_t *
libexo::exo_alloc(enum exo_class_t ec)
{
  struct exo_term_t *exo;

  exo = (struct exo_term_t *)calloc(1, sizeof(struct exo_term_t));
  if (!exo)
    Sim->fatal("out of virtual memory");
  exo->next = NULL;
  exo->ec = ec;

  return exo;
}


/*
 * create a new EXO term, usage:
 *
 *	exo_new(ec_integer, (exo_integer_t)<int>);
 *	exo_new(ec_address, (exo_integer_t)<int>);
 *	exo_new(ec_float, (exo_float_t)<float>);
 *	exo_new(ec_char, (int)<char>);
 *      exo_new(ec_string, "<string>");
 *      exo_new(ec_list, <list_ent>..., NULL);
 *      exo_new(ec_array, <size>, <array_ent>..., NULL);
 *	exo_new(ec_token, "<token>"); 
*/
struct exo_term_t *
libexo::exo_new(enum exo_class_t ec, ...)
{
  struct exo_term_t *exo;
  va_list v;
  va_start(v, ec);

  exo = exo_alloc(ec);
  switch (ec)
    {
    case ec_integer:
      exo->as_integer.val = va_arg(v, exo_integer_t);
      break;

    case ec_address:
      exo->as_address.val = va_arg(v, exo_address_t);
      break;

    case ec_float:
      exo->as_float_exo.val = va_arg(v, exo_float_t);
      break;

    case ec_char:
      exo->as_char.val = va_arg(v, int);
      break;

    case ec_string:
      {
	char *str;

	str = va_arg(v, char *);
	exo->as_string.str = (unsigned char *)Sim->MISC->mystrdup(str);
      }
      break;

    case ec_list:
      {
	struct exo_term_t *ent;

	exo->as_list.head = NULL;
	do {
	  ent = va_arg(v, struct exo_term_t *);
	  exo->as_list.head = exo_chain(exo->as_list.head, ent);
	} while (ent != NULL);
      }
      break;

    case ec_array:
      {
	int i;
	struct exo_term_t *ent;

	exo->as_array.size = va_arg(v, int);
	exo->as_array.array = (struct exo_term_t **)
	  calloc(exo->as_array.size, sizeof(struct exo_term_t *));
	if (!exo->as_array.array)
	  Sim->fatal("out of virtual memory");
	i = 0;
	do {
	  ent = va_arg(v, struct exo_term_t *);
	  if (ent != NULL)
	    {
	      if (i == exo->as_array.size)
		Sim->fatal("array constructor overflow");
	      SET_EXO_ARR(exo, i, ent);
	    }
	  i++;
	} while (ent != NULL);
      }
      break;

    case ec_token:
      {
	char *str;

	str = va_arg(v, char *);
	exo->as_token.ent = exo_intern(str);
      }
      break;

    case ec_blob:
      {
	unsigned size;
	unsigned char *data;

	size = va_arg(v, unsigned);
	data = va_arg(v, unsigned char *);

	exo->as_blob.size = size;
	exo->as_blob.data = (unsigned char*) malloc(size);
	if (data != NULL)
	  memcpy(exo->as_blob.data, data, size);
	else
	  memset(exo->as_blob.data, 0, size);
      }
      break;

    case ec_null:
      break;

    default:
      Sim->panic("bogus EXO class");
    }

  va_end(v);
  return exo;
}

/* release an EXO term */
void
libexo::exo_delete(struct exo_term_t *exo)
{
  exo->next = NULL;

  switch (exo->ec)
    {
    case ec_integer:
      /* no extra storage */
      exo->as_integer.val = 0;
      break;

    case ec_address:
      /* no extra storage */
      exo->as_address.val = 0;
      break;

    case ec_float:
      /* no extra storage */
      exo->as_float_exo.val = 0.0;
      break;

    case ec_char:
      /* no extra storage */
      exo->as_char.val = '\0';
      break;

    case ec_string:
      free(exo->as_string.str);
      exo->as_string.str = NULL;
      break;

    case ec_list:
      {
	struct exo_term_t *ent, *next_ent;

	for (ent=exo->as_list.head; ent != NULL; ent = next_ent)
	  {
	    next_ent = ent->next;
	    exo_delete(ent);
	  }
	exo->as_list.head = NULL;
      }
      break;

    case ec_array:
      {
	int i;

	for (i=0; i < exo->as_array.size; i++)
	  {
	    if (exo->as_array.array[i] != NULL)
	      exo_delete(exo->as_array.array[i]);
	  }
	free(exo->as_array.array);
	exo->as_array.array = NULL;
	exo->as_array.size = 0;
      }
      break;

    case ec_token:
      /* no extra storage */
      exo->as_token.ent = NULL;
      break;

    case ec_blob:
      /* free the blob data */
      free(exo->as_blob.data);
      exo->as_blob.data = NULL;
      break;

    case ec_null:
      /* no extra storage */
      break;

    default:
      Sim->panic("bogus EXO class");
    }
  exo->ec = (enum exo_class_t)0;

  /* release the node */
  free(exo);
}

/* chain two EXO lists together, FORE is attached on the end of AFT */
struct exo_term_t *
libexo::exo_chain(struct exo_term_t *fore, struct exo_term_t *aft)
{
  struct exo_term_t *exo, *prev;

  if (!fore && !aft)
    return NULL;

  if (!fore)
    return aft;

  /* find the tail of FORE */
  for (prev=NULL,exo=fore; exo != NULL; prev=exo,exo=exo->next)
    /* nada */;
  assert(prev);

  /* link onto the tail of FORE */
  prev->next = aft;

  return fore;
}

/* copy an EXO node */
struct exo_term_t *
libexo::exo_copy(struct exo_term_t *exo)
{
  struct exo_term_t *new_exo;

  /* NULL copy */
  if (!exo)
    return NULL;

  new_exo = exo_alloc(exo->ec);
  *new_exo = *exo;

  /* the next link is always blown away on a copy */
  new_exo->next = NULL;

  switch (new_exo->ec)
    {
    case ec_integer:
    case ec_address:
    case ec_float:
    case ec_char:
    case ec_string:
    case ec_list:
    case ec_token:
      /* no internal parts to copy */
      break;

    case ec_array:
      {
	int i;

	/* copy the array */
	new_exo->as_array.array = (struct exo_term_t **)
	  calloc(new_exo->as_array.size, sizeof(struct exo_term_t *));

	for (i=0; i<new_exo->as_array.size; i++)
	  {
	    SET_EXO_ARR(new_exo, i, EXO_ARR(exo, i));
	  }
      }
      break;

    case ec_blob:
      new_exo->as_blob.data = (unsigned char*) malloc(new_exo->as_array.size);
      memcpy(new_exo->as_blob.data, exo->as_blob.data, new_exo->as_array.size);
      break;

    default:
      Sim->panic("bogus EXO class");
    }

  return new_exo;
}

/* deep copy an EXO structure */
struct exo_term_t *
libexo::exo_deepcopy(struct exo_term_t *exo)
{
  struct exo_term_t *new_exo;

  /* NULL copy */
  if (!exo)
    return NULL;

  new_exo = exo_copy(exo);
  switch (new_exo->ec)
    {
    case ec_integer:
    case ec_address:
    case ec_float:
    case ec_char:
    case ec_token:
      /* exo_copy() == exo_deepcopy() for these node classes */
      break;

    case ec_string:
      /* copy the referenced string */
      new_exo->as_string.str =
	(unsigned char *)Sim->MISC->mystrdup((char *)exo->as_string.str);
      break;

    case ec_list:
      /* copy all list elements */
      {
	struct exo_term_t *elt, *new_elt, *new_list;

	new_list = NULL;
	for (elt=new_exo->as_list.head; elt != NULL; elt=elt->next)
	  {
	    new_elt = exo_deepcopy(elt);
	    new_list = exo_chain(new_list, new_elt);
	  }
	new_exo->as_list.head = new_list;
      }
      break;

    case ec_array:
      /* copy all array elements */
      {
	int i;

	for (i=0; i<new_exo->as_array.size; i++)
	  {
	    SET_EXO_ARR(new_exo, i, exo_deepcopy(EXO_ARR(exo, i)));
	  }
      }
      break;

    case ec_blob:
      new_exo->as_blob.data = (unsigned char *) malloc(new_exo->as_array.size);
      memcpy(new_exo->as_blob.data, exo->as_blob.data, new_exo->as_array.size);
      break;

    default:
      Sim->panic("bogus EXO class");
    }

  return new_exo;
}

/* print an EXO term */
void
libexo::exo_print(struct exo_term_t *exo, FILE *stream)
{
  if (!stream)
    stream = stderr;

  switch (exo->ec)
    {
    case ec_integer:
      if (sizeof(exo_integer_t) == 4)
	Sim->MISC->myfprintf(stream, "%u", exo->as_integer.val);
      else
	Sim->MISC->myfprintf(stream, "%lu", exo->as_integer.val);
      break;

    case ec_address:
      if (sizeof(exo_address_t) == 4)
	Sim->MISC->myfprintf(stream, "0x%x", exo->as_integer.val);
      else
	Sim->MISC->myfprintf(stream, "0x%lx", exo->as_integer.val);
      break;

    case ec_float:
      fprintf(stream, "%f", exo->as_float_exo.val);
      break;

    case ec_char:
      fprintf(stream, "'");
      print_char(exo->as_char.val, stream);
      fprintf(stream, "'");
      break;

    case ec_string:
      fprintf(stream, "\"");
      print_string(exo->as_string.str, stream);
      fprintf(stream, "\"");
      break;

    case ec_list:
      {
	struct exo_term_t *ent;

	fprintf(stream, "(");
	for (ent=exo->as_list.head; ent != NULL; ent=ent->next)
	  {
	    exo_print(ent, stream);
	    if (ent->next)
	      fprintf(stream, ", ");
	  }
	fprintf(stream, ")");
      }
      break;

    case ec_array:
      {
	int i, last;

	/* search for last first non-NULL entry */
	for (last=exo->as_array.size-1; last >= 0; last--)
	  {
	    if (EXO_ARR(exo, last) != NULL)
	      break;
	  }
	/* LAST == index of last non-NULL array entry */

	fprintf(stream, "{%d}[", exo->as_array.size);
	for (i=0; i<exo->as_array.size && i <= last; i++)
	  {
	    if (exo->as_array.array[i] != NULL)
	      exo_print(exo->as_array.array[i], stream);
	    else
	      fprintf(stream, " ");
	    if (i != exo->as_array.size-1 && i != last)
	      fprintf(stream, ", ");
	  }
	fprintf(stream, "]");
      }
      break;

    case ec_token:
      fprintf(stream, "%s", exo->as_token.ent->str);
      break;

    case ec_blob:
      {
	int i, cr;

	fprintf(stream, "{%d}<\n", exo->as_blob.size);
	for (i=0; i < exo->as_blob.size; i++)
	  {
	    cr = FALSE;
	    if (i != 0 && (i % 38) == 0)
	      {
		fprintf(stream, "\n");
		cr = TRUE;
	      }
	    fprintf(stream, "%02x", exo->as_blob.data[i]);
	  }
	if (!cr)
	  fprintf(stream, "\n");
	fprintf(stream, ">");
      }
      break;

    default:
      Sim->panic("bogus EXO class");
    }
}


/*static*/void
libexo::exo_err(char *err)
{
  extern int line;

  fprintf(stderr, "EXO parse error: line %d: %s\n", line, err);
  exit(1);
}

/* read one EXO term from STREAM */
struct exo_term_t *
libexo::exo_read(FILE *stream)
{
  int tok;
  char tok_buf[1024], *endp;
  struct exo_term_t *ent = NULL;
//  extern int errno;
 // extern "C" void yy_setstream(FILE *);

  /* make sure we have a valid stream */
  if (!stream)
    stream = stdin;
  yy_setstream(stream);

  /* make local copies of everything, allows arbitrary recursion */
  tok = yylex();
  strcpy(tok_buf, yytext);

  switch (tok)
    {
    case lex_integer:
      {
	exo_integer_t int_val;

	/* attempt integer conversion */
	errno = 0;
#ifdef HOST_HAS_QUAD
	int_val = Sim->MISC->myatoq(tok_buf, &endp, /* parse base */10);
#else /* !HOST_HAS_QUAD */
	int_val = strtoul(tok_buf, &endp, /* parse base */10);
#endif /* HOST_HAS_QUAD */
	if (!errno && !*endp)
	  {
	    /* good conversion */
	    ent = exo_new(ec_integer, int_val);
	  }
	else
	  exo_err("cannot parse integer literal");
      }
      break;

    case lex_address:
      {
	exo_address_t addr_val;

	/* attempt address conversion */
	errno = 0;
#ifdef HOST_HAS_QUAD
	addr_val = Sim->MISC->myatoq(tok_buf, &endp, /* parse base */16);
#else /* !HOST_HAS_QUAD */
	addr_val = strtoul(tok_buf, &endp, /* parse base */16);
#endif /* HOST_HAS_QUAD */
	if (!errno && !*endp)
	  {
	    /* good conversion */
	    ent = exo_new(ec_address, addr_val);
	  }
	else
	  exo_err("cannot parse address literal");
      }
      break;

    case lex_float:
      {
	exo_float_t float_val;

	/* attempt double conversion */
	errno = 0;
	float_val = strtod(tok_buf, &endp);
	if (!errno && !*endp)
	  {
	    /* good conversion */
	    ent = exo_new(ec_float, float_val);
	  }
	else
	  exo_err("cannot parse floating point literal");
      }
      break;

    case lex_char:
      {
	int c;

	c = intern_char(tok_buf, &endp);
	if (!endp)
	  exo_err("cannot convert character literal");
	ent = exo_new(ec_char, c);
      }
      break;

    case lex_string:
      {
	char *s;

	s = intern_string(tok_buf);
	ent = exo_new(ec_string, s);
	free(s);
      }
      break;

    case lex_token:
      ent = exo_new(ec_token, tok_buf);
      break;

    case lex_byte:
      exo_err("unexpected blob byte encountered");
      break;

    case '(':
      {
	struct exo_term_t *elt;

	ent = exo_new(ec_list, NULL);

	if (yy_nextchar() != ')')
	  {
	    /* not an empty list */
	    do {
	      elt = exo_read(stream);
	      if (!elt)
		exo_err("unexpected end-of-file");
	      ent->as_list.head =
		exo_chain(ent->as_list.head, elt);

	      /* consume optional commas */
	      if (yy_nextchar() == ',')
		yylex();
	    } while (yy_nextchar() != ')');
	  }

	/* read tail delimiter */
	tok = yylex();
	if (tok != ')')
	  exo_err("expected ')'");
      }
      break;

    case ')':
      exo_err("unexpected ')' encountered");
      break;

    case '<':
      exo_err("unexpected '<' encountered");
      break;

    case '>':
      exo_err("unexpected '>' encountered");
      break;

    case '{':
      {
	int cnt, size;
	struct exo_term_t *elt;

	/* get the size */
	elt = exo_read(stream);
	if (!elt || elt->ec != ec_integer)
	  exo_err("badly formed array size");

	/* record the size of the array/blob */
	size = (int)elt->as_integer.val;

	/* done with the EXO integer */
	exo_delete(elt);

	/* read the array delimiters */
	tok = yylex();
	if (tok != '}')
	  exo_err("expected '}'");

	tok = yylex();
	switch (tok)
	  {
	  case '[': /* array definition */
	    /* allocate an array definition */
	    ent = exo_new(ec_array, size, NULL);

	    /* read until array is full or tail delimiter encountered */
	    if (yy_nextchar() != ']')
	      {
		/* not an empty array */
		cnt = 0;
		do {
		  if (cnt == ent->as_array.size)
		    exo_err("too many initializers for array");

		  /* NULL element? */
		  if (yy_nextchar() == ',')
		    {
		      elt = NULL;
		    }
		  else
		    {
		      elt = exo_read(stream);
		      if (!elt)
			exo_err("unexpected end-of-file");
		    }
		  SET_EXO_ARR(ent, cnt, elt);
		  cnt++;

		  /* consume optional commas */
		  if (yy_nextchar() == ',')
		    yylex();
		} while (yy_nextchar() != ']');
	      }

	    /* read tail delimiter */
	    tok = yylex();
	    if (tok != ']')
	      exo_err("expected ']'");
	    break;

	  case '<': /* blob definition */
	    /* allocate an array definition */
	    ent = exo_new(ec_blob, size, /* zero contents */NULL);

	    /* read until blob is full */
	    if (yy_nextchar() != '>')
	      {
		unsigned int byte_val;

		/* not an empty array */
		cnt = 0;
		for (;;) {
		  /* read next blob byte */
		  tok = yylex();

		  if (tok == lex_byte)
		    {
		      if (cnt == ent->as_blob.size)
			exo_err("too many initializers for blob");

		      /* attempt hex conversion */
		      errno = 0;
		      byte_val = strtoul(yytext, &endp, /* parse base */16);
		      if (errno != 0 || *endp != '\0')
			exo_err("cannot parse blob byte literal");
		      if (byte_val > 255)
			Sim->panic("bogus byte value");
		      ent->as_blob.data[cnt] = byte_val;
		      cnt++;
		    }
		  else if (tok == '>')
		    break;
		  else
		    exo_err("unexpected character in blob");
		}
	      }

#if 0 /* zero tail is OK... */
	    if (cnt != ent->as_blob.size)
	      exo_err("not enough initializers for blob");
#endif
	    break;

	  default:
	    exo_err("expected '[' or '<'");
	  }
      }
      break;

    case '}':
      exo_err("unexpected '}' encountered");
      break;

    case ',':
      exo_err("unexpected ',' encountered");
      break;

    case '[':
      {
	int i, cnt;
	struct exo_term_t *list, *elt, *next_elt;

	/* compute the array size */
	list = NULL;
	if (yy_nextchar() == ']')
	  exo_err("unsized array has no initializers");

	cnt = 0;
	do {
	  /* NULL element? */
	  if (yy_nextchar() == ',')
	    {
	      elt = exo_new(ec_null);
	    }
	  else
	    {
	      elt = exo_read(stream);
	      if (!elt)
		exo_err("unexpected end-of-file");
	    }
	  cnt++;
	  list = exo_chain(list, elt);

	  /* consume optional commas */
	  if (yy_nextchar() == ',')
	    yylex();
	} while (yy_nextchar() != ']');

	/* read tail delimiter */
	tok = yylex();
	if (tok != ']')
	  exo_err("expected ']'");

	/* create the array */
	assert(cnt > 0);
	ent = exo_new(ec_array, cnt, NULL);

	/* fill up the array */
	for (i=0,elt=list; i<cnt; i++,elt=next_elt)
	  {
	    assert(elt != NULL);
	    next_elt = elt->next;
	    if (elt->ec == ec_null)
	      {
		SET_EXO_ARR(ent, cnt, NULL);
		exo_delete(ent);
	      }
	    else
	      {
		SET_EXO_ARR(ent, cnt, elt);
		elt->next = NULL;
	      }
	  }
      }
      break;

    case ']':
      exo_err("unexpected ']' encountered");
      break;

    case lex_eof:
      /* nothing to read */
      ent = NULL;
      break;

    default:
      Sim->panic("bogus token");
    }

  return ent;
}

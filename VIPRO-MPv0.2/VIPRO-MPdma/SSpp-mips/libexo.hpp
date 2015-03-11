
#ifndef EXO_H
#define EXO_H

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"



/* bogus token value */
#define TOKEN_BOGON		0

/*static*/int token_id = TOKEN_BOGON + 1;

#define TOKEN_HASH_SIZE		1024
struct exo_token_t *token_hash[TOKEN_HASH_SIZE];


/* EXO file format versions */
#define EXO_FMT_MAJOR		1
#define EXO_FMT_MINOR		0



/* EXO term classes, keep this in sync with EXO_CLASS_STR */
enum exo_class_t {
  ec_integer,			/* EXO int value */
  ec_address,			/* EXO address value */
  ec_float,			/* EXO FP value */
  ec_char,			/* EXO character value */
  ec_string,			/* EXO string value */
  ec_list,			/* EXO list */
  ec_array,			/* EXO array */
  ec_token,			/* EXO token value */
  ec_blob,			/* EXO blob (Binary Large OBject) */
  ec_null,			/* used internally */
  ec_NUM
};

/* EXO term classes print strings */
char *exo_class_str[ec_NUM] = {
  "integer",
  "address",
  "float",
  "char",
  "string"
  "list",
  "array",
  "token",
  "blob"
};

/* EXO token table entry */
struct exo_token_t {
  struct exo_token_t *next;	/* next element in a hash buck chain */
  char *str;			/* token string */
  int token;			/* token value */
};

struct exo_term_t {
  struct exo_term_t *next;	/* next element, when in a list */
  enum exo_class_t ec;		/* term node class */
  union {
    struct as_integer_t {
      exo_integer_t val;		/* integer value */
    } as_integer;
    struct as_address_t {
      exo_address_t val;		/* address value */
    } as_address;
    struct as_float_t {
      exo_float_t val;			/* floating point value */
    } as_float_exo;
    struct as_char_t {
      char val;				/* character value */
    } as_char;
    struct as_string_t {
      unsigned char *str;		/* string value */
    } as_string;
    struct as_list_t {
      struct exo_term_t *head;		/* list head pointer */
    } as_list;
    struct as_array_t {
      int size;				/* size of the array */
      struct exo_term_t **array;	/* list head pointer */
    } as_array;
    struct as_token_t {
      struct exo_token_t *ent;		/* token table entry */
    } as_token;
    struct as_blob_t {
      int size;				/* byte size of object */
      unsigned char *data;		/* pointer to blob data */
    } as_blob;
  } variant;
};
/* short-cut accessors */
#define as_integer	variant.as_integer
#define as_address	variant.as_address
#define as_float_exo	variant.as_float_exo
#define as_char		variant.as_char
#define as_string	variant.as_string
#define as_list		variant.as_list
#define as_array	variant.as_array
#define as_token	variant.as_token
#define as_blob		variant.as_blob

/* EXO array accessor, may be used as an L-value or R-value */
/* EXO array accessor, may be used as an L-value or R-value */
#define EXO_ARR(E,N)							\
  ((E)->ec != ec_array							\
   ? (Sim->fatal("not an array"), *(struct exo_term_t **)(NULL))		\
   : ((N) >= (E)->as_array.size						\
      ? (Sim->fatal("array bounds error"), *(struct exo_term_t **)(NULL))	\
      : (E)->as_array.array[(N)]))
#define SET_EXO_ARR(E,N,V)						\
  ((E)->ec != ec_array							\
   ? (void)Sim->fatal("not an array")					\
   : ((N) >= (E)->as_array.size						\
      ? (void)Sim->fatal("array bounds error")				\
      : (void)((E)->as_array.array[(N)] = (V))))


/* lexor components */
enum lex_t {
  lex_integer = 256,
  lex_address,
  lex_float,
  lex_char,
  lex_string,
  lex_token,
  lex_byte,
  lex_eof
};


class sim;

class libexo
{
	private:
	
		sim *Sim;
	
		int
		intern_escape(char *esc, char **next);

		int
		intern_char(char *s, char **next);

		void
		print_char(unsigned char c, FILE *stream);

		char *
		intern_string(char *str);

		void
		print_string(unsigned char *s, FILE *stream);

		unsigned long
		hash_str(char *s);

		struct exo_term_t *
		exo_alloc(enum exo_class_t ec);

		void
		exo_err(char *err);

	public:
		libexo(sim*);
		
		/* intern token TOKEN_STR */
		struct exo_token_t *
		exo_intern(char *token_str);		/* string to intern */

		/* intern token TOKEN_STR as value TOKEN */
		struct exo_token_t *
		exo_intern_as(char *token_str,		/* string to intern */
			      int token);		/* internment value */

		/*
		 * create a new EXO term, usage:
		 *
		 *	exo_new(ec_integer, (exo_integer_t)<int>);
		 *	exo_new(ec_address, (exo_address_t)<int>);
		 *	exo_new(ec_float, (exo_float_t)<float>);
		 *	exo_new(ec_char, (int)<char>);
		 *      exo_new(ec_string, "<string>");
		 *      exo_new(ec_list, <list_ent>..., NULL);
		 *      exo_new(ec_array, <size>, <array_ent>..., NULL);
		 *	exo_new(ec_token, "<token>");
		 *	exo_new(ec_blob, <size>, <data_ptr>);
		 */
		struct exo_term_t *
		exo_new(enum exo_class_t ec, ...);

		/* release an EXO term */
		void
		exo_delete(struct exo_term_t *exo);

		/* chain two EXO lists together, FORE is attached on the end of AFT */
		struct exo_term_t *
		exo_chain(struct exo_term_t *fore, struct exo_term_t *aft);

		/* copy an EXO node */
		struct exo_term_t *
		exo_copy(struct exo_term_t *exo);

		/* deep copy an EXO structure */
		struct exo_term_t *
		exo_deepcopy(struct exo_term_t *exo);

		/* print an EXO term */
		void
		exo_print(struct exo_term_t *exo, FILE *stream);

		/* read one EXO term from STREAM */
		struct exo_term_t *
		exo_read(FILE *stream);

};

#endif /* EXO_H */

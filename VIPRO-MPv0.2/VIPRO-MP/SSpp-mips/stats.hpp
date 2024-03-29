#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "host.hpp"
#include "machine.hpp"
#include "eval.hpp"
#include "misc.hpp"


/*
 * The stats package is a uniform module for handling statistical variables,
 * including counters, distributions, and expressions.  The user must first
 * create a stats database using stat_new(), then statical counters are added
 * to the database using the *_reg_*() functions.  Interfaces are included to
 * allocate and manipulate distributions (histograms) and general expression
 * of other statistical variables constants.  Statistical variables can be
 * located by name using stat_find_stat().  And, statistics can be print in
 * a highly standardized and stylized fashion using stat_print_stats().
 */

/* MSO: (03/06/2008) Changed the enum to msc_int, cause they conflict with the systemc types
   Note: changed in the rest of the code */


/* stat variable classes */
enum stat_class_t {
  msc_int = 0,			/* integer stat */
  msc_uint,			/* unsigned integer stat */
#ifdef HOST_HAS_QUAD
  msc_quad,			/* quadword integer stat */
  msc_squad,			/* signed quadword integer stat */
#endif /* HOST_HAS_QUAD */
  msc_float,			/* single-precision FP stat */
  msc_double,			/* double-precision FP stat */
  msc_dist,			/* array distribution stat */
  msc_sdist,			/* sparse array distribution stat */
  msc_formula,			/* stat expression formula */
  msc_NUM
};

/* sparse array distributions are implemented with a hash table */
#define HTAB_SZ			1024
#define HTAB_HASH(I)		((((I) >> 8) ^ (I)) & (HTAB_SZ - 1))

/* hash table bucket definition */
struct bucket_t {
  struct bucket_t *next;	/* pointer to the next bucket */
  md_addr_t index;		/* bucket index - as large as an addr */
  unsigned int count;		/* bucket count */
};

/* forward declaration */
struct stat_stat_t;

/* enable distribution components:  index, count, probability, cumulative */
#define PF_COUNT		0x0001
#define PF_PDF			0x0002
#define PF_CDF			0x0004
#define PF_ALL			(PF_COUNT|PF_PDF|PF_CDF)

/* user-defined print function, if this option is selected, a function of this
   form is called for each bucket in the distribution, in ascending index
   order */
typedef void
(*print_fn_t)(struct stat_stat_t *stat,	/* the stat variable being printed */
	      md_addr_t index,		/* entry index to print */
	      int count,		/* entry count */
	      double sum,		/* cumulative sum */
	      double total);		/* total count for distribution */


/* statistical variable definition */
struct stat_stat_t {
  struct stat_stat_t *next;	/* pointer to next stat in database list */
  char *name;			/* stat name */
  char *desc;			/* stat description */
  char *format;			/* stat output print format */
  enum stat_class_t sc;		/* stat class */
  union stat_variant_t {
    /* sc == sc_int */
    struct stat_for_int_t {
      int *var;			/* integer stat variable */
      int init_val;		/* initial integer value */
    } for_int;
    /* sc == sc_uint */
    struct stat_for_uint_t {
      unsigned int *var;	/* unsigned integer stat variable */
      unsigned int init_val;	/* initial unsigned integer value */
    } for_uint;
#ifdef HOST_HAS_QUAD
    /* sc == sc_quad */
    struct stat_for_quad_t {
      quad_t *var;		/* quadword integer stat variable */
      quad_t init_val;		/* quadword integer value */
    } for_quad;
    /* sc == sc_squad */
    struct stat_for_squad_t {
      squad_t *var;		/* signed quadword integer stat variable */
      squad_t init_val;		/* signed quadword integer value */
    } for_squad;
#endif /* HOST_HAS_QUAD */
    /* sc == sc_float */
    struct stat_for_float_t {
      float *var;		/* float stat variable */
      float init_val;		/* initial float value */
    } for_float;
    /* sc == sc_double */
    struct stat_for_double_t {
      double *var;		/* double stat variable */
      double init_val;		/* initial double value */
    } for_double;
    /* sc == sc_dist */
    struct stat_for_dist_t {
      unsigned int init_val;	/* initial dist value */
      unsigned int *arr;	/* non-sparse array pointer */
      unsigned int arr_sz;	/* array size */
      unsigned int bucket_sz;	/* array bucket size */
      int pf;			/* printables */
      char **imap;		/* index -> string map */
      print_fn_t print_fn;	/* optional user-specified print fn */
      unsigned int overflows;	/* total overflows in stat_add_samples() */
    } for_dist;
    /* sc == sc_sdist */
    struct stat_for_sdist_t {
      unsigned int init_val;	/* initial dist value */
      struct bucket_t **sarr;	/* sparse array pointer */
      int pf;			/* printables */
      print_fn_t print_fn;	/* optional user-specified print fn */
    } for_sdist;
    /* sc == sc_formula */
    struct stat_for_formula_t {
      char *formula;		/* stat formula, see eval.h for format */
    } for_formula;
  } variant;
};

/* statistical database */
struct stat_sdb_t {
  struct stat_stat_t *stats;		/* list of stats in database */
  struct eval_state_t *evaluator;	/* an expression evaluator */
};


class sim;

class stats
{
	private:
		
		sim *Sim;	
		
		void
		add_stat(struct stat_sdb_t *sdb,	
			 struct stat_stat_t *stat);	

		void
		print_dist(struct stat_stat_t *stat,	/* stat variable */
			   FILE *fd);			/* output stream */

		void
		print_sdist(struct stat_stat_t *stat,	/* stat variable */
			    FILE *fd);			/* output stream */
			 
	public:
		stats(sim*);
	
		/* evaluate a stat as an expression */
		struct eval_value_t
		stat_eval_ident(struct eval_state_t *es);/* expression stat to evaluate */

		/* create a new stats database */
		struct stat_sdb_t *stat_new(void);

		/* delete a stats database */
		void
		stat_delete(struct stat_sdb_t *sdb);	/* stats database */

		/* register an integer statistical variable */
		struct stat_stat_t *
		stat_reg_int(struct stat_sdb_t *sdb,	/* stat database */
			     char *name,		/* stat variable name */
			     char *desc,		/* stat variable description */
			     int *var,			/* stat variable */
			     int init_val,		/* stat variable initial value */
			     char *format);		/* optional variable output format */

		/* register an unsigned integer statistical variable */
		struct stat_stat_t *
		stat_reg_uint(struct stat_sdb_t *sdb,	/* stat database */
			      char *name,		/* stat variable name */
			      char *desc,		/* stat variable description */
			      unsigned int *var,	/* stat variable */
			      unsigned int init_val,	/* stat variable initial value */
			      char *format);		/* optional variable output format */
			      
		//#ifdef HOST_HAS_QUAD	      
		/* register a quadword integer statistical variable */
		struct stat_stat_t *
		stat_reg_quad(struct stat_sdb_t *sdb,	/* stat database */
			      char *name,		/* stat variable name */
			      char *desc,		/* stat variable description */
			      quad_t *var,		/* stat variable */
			      quad_t init_val,		/* stat variable initial value */
			      char *format);		/* optional variable output format */

		/* register a signed quadword integer statistical variable */
		struct stat_stat_t *
		stat_reg_squad(struct stat_sdb_t *sdb,	/* stat database */
			       char *name,		/* stat variable name */
			       char *desc,		/* stat variable description */
			       squad_t *var,		/* stat variable */
			       squad_t init_val,	/* stat variable initial value */
			       char *format);		/* optional variable output format */
		//#endif /* HOST_HAS_QUAD */

		/* register a float statistical variable */
		struct stat_stat_t *
		stat_reg_float(struct stat_sdb_t *sdb,	/* stat database */
			       char *name,		/* stat variable name */
			       char *desc,		/* stat variable description */
			       float *var,		/* stat variable */
			       float init_val,		/* stat variable initial value */
			       char *format);		/* optional variable output format */

		/* register a double statistical variable */
		struct stat_stat_t *
		stat_reg_double(struct stat_sdb_t *sdb,	/* stat database */
				char *name,		/* stat variable name */
				char *desc,		/* stat variable description */
				double *var,		/* stat variable */
				double init_val,	/* stat variable initial value */
				char *format);		/* optional variable output format */

		/* create an array distribution (w/ fixed size buckets) in stat database SDB,
		   the array distribution has ARR_SZ buckets with BUCKET_SZ indicies in each
		   bucked, PF specifies the distribution components to print for optional
		   format FORMAT; the indicies may be optionally replaced with the strings
		   from IMAP, or the entire distribution can be printed with the optional
		   user-specified print function PRINT_FN */
		struct stat_stat_t *
		stat_reg_dist(struct stat_sdb_t *sdb,	/* stat database */
			      char *name,		/* stat variable name */
			      char *desc,		/* stat variable description */
			      unsigned int init_val,	/* dist initial value */
			      unsigned int arr_sz,	/* array size */
			      unsigned int bucket_sz,	/* array bucket size */
			      int pf,			/* print format, use PF_* defs */
			      char *format,		/* optional variable output format */
			      char **imap,		/* optional index -> string map */
			      print_fn_t print_fn);	/* optional user print function */

		/* create a sparse array distribution in stat database SDB, while the sparse
		   array consumes more memory per bucket than an array distribution, it can
		   efficiently map any number of indicies from 0 to 2^32-1, PF specifies the
		   distribution components to print for optional format FORMAT; the indicies
		   may be optionally replaced with the strings from IMAP, or the entire
		   distribution can be printed with the optional user-specified print function
		   PRINT_FN */
		struct stat_stat_t *
		stat_reg_sdist(struct stat_sdb_t *sdb,	/* stat database */
			       char *name,		/* stat variable name */
			       char *desc,		/* stat variable description */
			       unsigned int init_val,	/* dist initial value */
			       int pf,			/* print format, use PF_* defs */
			       char *format,		/* optional variable output format */
			       print_fn_t print_fn);	/* optional user print function */

		/* add NSAMPLES to array or sparse array distribution STAT */
		void
		stat_add_samples(struct stat_stat_t *stat,/* stat database */
				 md_addr_t index,	/* distribution index of samples */
				 int nsamples);		/* number of samples to add to dist */

		/* add a single sample to array or sparse array distribution STAT */
		void
		stat_add_sample(struct stat_stat_t *stat,/* stat variable */
				md_addr_t index);	/* index of sample */

		/* register a double statistical formula, the formula is evaluated when the
		   statistic is printed, the formula expression may reference any registered
		   statistical variable and, in addition, the standard operators '(', ')', '+',
		   '-', '*', and '/', and literal (i.e., C-format decimal, hexidecimal, and
		   octal) constants are also supported; NOTE: all terms are immediately
		   converted to double values and the result is a double value, see eval.h
		   for more information on formulas */
		struct stat_stat_t *
		stat_reg_formula(struct stat_sdb_t *sdb,/* stat database */
				 char *name,		/* stat variable name */
				 char *desc,		/* stat variable description */
				 char *formula,		/* formula expression */
				 char *format);		/* optional variable output format */

		/* print the value of stat variable STAT */
		void
		stat_print_stat(struct stat_sdb_t *sdb,	/* stat database */
				struct stat_stat_t *stat,/* stat variable */
				FILE *fd);		/* output stream */

		/* print the value of all stat variables in stat database SDB */
		void
		stat_print_stats(struct stat_sdb_t *sdb,/* stat database */
				 FILE *fd);		/* output stream */


		/* find a stat variable, returns NULL if it is not found */
		struct stat_stat_t *
		stat_find_stat(struct stat_sdb_t *sdb,	/* stat database */
			       char *stat_name);	/* stat name */

};

#endif


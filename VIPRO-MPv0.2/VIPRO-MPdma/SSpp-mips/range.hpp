#ifndef RANGE_H
#define RANGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "symbol.hpp"
#include "loader.hpp"


enum range_ptype_t {
  pt_addr = 0,			/* address position */
  pt_inst,			/* instruction count position */
  pt_cycle,			/* cycle count position */
  pt_NUM
};

struct range_pos_t {
  enum range_ptype_t ptype;	/* type of position */
  counter_t pos;		/* position */
};

/* an execution range */
struct range_range_t {
  struct range_pos_t start;
  struct range_pos_t end;
};

class sim;

class range
{
	private:
		sim *Sim;
		
	public:
		range(sim*);
	
		/* parse execution position *PSTR to *POS */
		char *						/* error string, or NULL */
		range_parse_pos(char *pstr,			/* execution position string */
				struct range_pos_t *pos);	/* position return buffer */

		/* print execution position *POS */
		void
		range_print_pos(struct range_pos_t *pos,	/* execution position */
				FILE *stream);			/* output stream */

		/* parse execution range *RSTR to *RANGE */
		char *						/* error string, or NULL */
		range_parse_range(char *rstr,			/* execution range string */
				  struct range_range_t *range);	/* range return buffer */

		/* print execution range *RANGE */
		void
		range_print_range(struct range_range_t *range,	/* execution range */
				  FILE *stream);		/* output stream */

		/* determine if inputs match execution position */
		int						/* relation to position */
		range_cmp_pos(struct range_pos_t *pos,		/* execution position */
			      counter_t val);			/* position value */

		/* determine if inputs are in range */
		int						/* relation to range */
		range_cmp_range(struct range_range_t *range,	/* execution range */
				counter_t val);			/* position value */


		/* determine if inputs are in range, passes all possible info needed */
		int						/* relation to range */
		range_cmp_range1(struct range_range_t *range,	/* execution range */
				 md_addr_t addr,		/* address value */
				 counter_t icount,		/* instruction count */
				 counter_t cycle);		/* cycle count */
	
};

#endif


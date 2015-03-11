#ifndef PTRACE_H
#define PTRACE_H

#include <stdio.h>
#include <stdlib.h>

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "range.hpp"


/*
 * pipeline events:
 *
 *	+ <iseq> <pc> <addr> <inst>	- new instruction def
 *	- <iseq>			- instruction squashed or retired
 *	@ <cycle>			- new cycle def
 *	* <iseq> <stage> <events>	- instruction stage transition
 *
 */

/*
	[IF]   [DA]   [EX]   [WB]   [CT]
         aa     dd     jj     ll     nn
         bb     ee     kk     mm     oo
         cc                          pp
 */

/* pipeline stages */
#define PST_IFETCH		"IF"
#define PST_DISPATCH		"DA"
#define PST_EXECUTE		"EX"
#define PST_WRITEBACK		"WB"
#define PST_COMMIT		"CT"

/* pipeline events */
#define PEV_CACHEMISS		0x00000001	/* I/D-cache miss */
#define PEV_TLBMISS		0x00000002	/* I/D-tlb miss */
#define PEV_MPOCCURED		0x00000004	/* mis-pred branch occurred */
#define PEV_MPDETECT		0x00000008	/* mis-pred branch detected */
#define PEV_AGEN		0x00000010	/* address generation */




class sim;

class ptrace
{
	private:
		sim *Sim;
	
	public:
	
		ptrace(sim*);
	
		/* main interfaces, with fast checks */
		void ptrace_newinst(unsigned int A,	/* instruction sequence number */
						 md_inst_t B,	/* new instruction */
						 md_addr_t C,		/* program counter of instruction */
						 md_addr_t D);	/* address referenced, if load/store */						
		 
		void ptrace_newuop(unsigned int A,	/* instruction sequence number */
						char *B,		/* new uop description */
						md_addr_t C,		/* program counter of instruction */
						md_addr_t D);	/* address referenced, if load/store */						
		 
		void ptrace_endinst(unsigned int A);						
		  
		void ptrace_newcycle(tick_t A);						
		  
		void ptrace_newstage(unsigned int A,	/* instruction sequence number */
				     char *B,		/* pipeline stage entered */
				     unsigned int C);/* pipeline events while in stage */						

		/* NOTE: pipetracing is a one-shot switch, since turning on a trace more than
		   once will mess up the pipetrace viewer */
		void ptrace_check_active(md_addr_t PC, counter_t ICNT, counter_t CYCLE);				
	  
		/*
		void ptrace_active(A,I,C)						
		  (ptrace_outfd != NULL	&& !range_cmp_range(&ptrace_range, (A), (I), (C)))
			*/
	
		/* pipetrace file */
		FILE *ptrace_outfd;

		/* pipetracing is active */
		int ptrace_active;

		/* pipetracing range */
		struct range_range_t ptrace_range;

		/* one-shot switch for pipetracing */
		int ptrace_oneshot;
	
		/* funcoes publicas --------------------------------------------*/
	
		/* open pipeline trace */
		void
		ptrace_open(char *range,		/* trace range */
			    char *fname);		/* output filename */

		/* close pipeline trace */
		void
		ptrace_close(void);

		/* declare a new instruction */
		void
		__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
				 md_inst_t inst,	/* new instruction */
				 md_addr_t pc,		/* program counter of instruction */
				 md_addr_t addr);	/* address referenced, if load/store */

		/* declare a new uop */
		void
		__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
				char *uop_desc,		/* new uop description */
				md_addr_t pc,		/* program counter of instruction */
				md_addr_t addr);	/* address referenced, if load/store */

		/* declare instruction retirement or squash */
		void
		__ptrace_endinst(unsigned int iseq);	/* instruction sequence number */

		/* declare a new cycle */
		void
		__ptrace_newcycle(tick_t cycle);	/* new cycle */

		/* indicate instruction transition to a new pipeline stage */
		void
		__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
				  char *pstage,		/* pipeline stage entered */
				  unsigned int pevents);/* pipeline events while in stage */

};

#endif


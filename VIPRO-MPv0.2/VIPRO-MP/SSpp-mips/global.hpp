#ifndef GLOBAL_H
#define GLOBAL_H


#include <stdlib.h>
#include <setjmp.h>
#include "host.hpp"


/* boolean value defs */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


class global
{
	public:
	
		global();
		
		/* execution instruction counter */
		counter_t sim_num_insn;

		#if 0 /* not portable... :-( */
		/* total simulator (data) memory usage */
		unsigned int sim_mem_usage = 0;
		#endif

		/* execution start/end times */
		time_t sim_start_time;
		time_t sim_end_time;
		int sim_elapsed_time;

		/* byte/word swapping required to execute target executable on this host */
		int sim_swap_bytes;
		int sim_swap_words;

		/* exit when this becomes non-zero */
		int sim_exit_now;

		/* longjmp here when simulation is completed */
		jmp_buf sim_exit_buf;

		/* set to non-zero when simulator should dump statistics */
		int sim_dump_stats;

		/* options database */
		struct opt_odb_t *sim_odb;

		/* stats database */
		struct stat_sdb_t *sim_sdb;

		/* EIO interfaces */
		char *sim_eio_fname;
		char *sim_chkpt_fname;
		FILE *sim_eio_fd;

		/* redirected program/simulator output file names */
		char *sim_simout;
		char *sim_progout;
		FILE *sim_progfd;

		/* track first argument orphan, this is the program to execute */
		int exec_index;

		/* dump help information */
		int help_me;

		/* random number generator seed */
		int rand_seed;

		/* initialize and quit immediately */
		int init_quit;
		
		int running;
		
		/* do arquivo misc.hpp */		
		int verbose;
};

#endif



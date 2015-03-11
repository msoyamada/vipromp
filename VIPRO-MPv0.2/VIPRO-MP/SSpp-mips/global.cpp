#include <stdio.h>
#include "global.hpp"


global::global()
{
		/* execution instruction counter */
		sim_num_insn = 0;

		/* exit when this becomes non-zero */
		sim_exit_now = FALSE;

		/* set to non-zero when simulator should dump statistics */
		sim_dump_stats = FALSE;

		/* EIO interfaces */
		sim_eio_fname = NULL;
		sim_chkpt_fname = NULL;
		sim_eio_fd = NULL;

		/* redirected program/simulator output file names */
		sim_simout = NULL;
		sim_progout = NULL;
		sim_progfd = NULL;

		/* track first argument orphan, this is the program to execute */
		exec_index = -1;
		running = FALSE;
		
		verbose = FALSE;
}




#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef BFD_LOADER
#include <bfd.h>
#endif /* BFD_LOADER */

/*
#include "host.hpp"
#include "misc.hpp"
#include "endian.hpp"
#include "options.hpp"
#include "stats.hpp"
#include "loader.hpp"
#include "sim.hpp"
*/
#include "machine.hpp"
#include "version.hpp"
#include "simplescalar.hpp"

//#include "memoryshared.hpp"

/* MSO 06/06/2008 Including SystemC */
#include <systemc.h>

using namespace std;

void simplescalar::free_objs()
{
	SIM->free_objs();
	free(SIM);
}

#ifndef _MSC_VER
/* simulator scheduling priority */
int nice_priority;
#endif

/* default simulator scheduling priority */
#define NICE_DEFAULT_VALUE		0

#if 0
/* stats signal handler */
/*static*/  void
simplescalar::signal_sim_stats(int sigtype)
{
  SIM->GLOBAL->sim_dump_stats = TRUE;
}

/* exit signal handler */
/*static*/ void
simplescalar::signal_exit_now(int sigtype)
{
  SIM->GLOBAL->sim_exit_now = TRUE;
}
#endif

#if 0
/*static*/ int
simplescalar::orphan_fn(int i, int argc, char **argv)
{
  SIM->GLOBAL->exec_index = i;
  return /* done */FALSE;
}

#endif

void simplescalar::refresh()
{
	for (;;)
	{
		if (SIM == NULL) halt(); // finish the simulation
		SIM->sim_cycle++;
		wait();
	}
}

/*static*/ void
simplescalar::banner(FILE *fd, int argc, char **argv)
{
  char *s;

  fprintf(fd,
	  "%s: SimpleScalar/%s Tool Set version %d.%d of %s.\n"
	  "Copyright (c) 1994-1998 by Todd M. Austin.  All Rights Reserved.\n"
	  "\n",
	  ((s = strrchr(argv[0], '/')) ? s+1 : argv[0]),
	  VER_TARGET, VER_MAJOR, VER_MINOR, VER_UPDATE);
}

void
simplescalar::usage(FILE *fd, int argc, char **argv)
{
  fprintf(fd, "Usage: %s {-options} executable {arguments}\n", argv[0]);
  SIM->OPTIONS->opt_print_help(SIM->GLOBAL->sim_odb, fd);
}


/* print all simulator stats */
void
simplescalar::sim_print_stats(FILE *fd)		/* output stream */
{
#if 0 /* not portable... :-( */
  extern char etext, *sbrk(int);
#endif

  if (!SIM->GLOBAL->running)
    return;

  /* get stats time */
  SIM->GLOBAL->sim_end_time = time((time_t *)NULL);
  SIM->GLOBAL->sim_elapsed_time = MAX(SIM->GLOBAL->sim_end_time - SIM->GLOBAL->sim_start_time, 1);

#if 0 /* not portable... :-( */
  /* compute simulator memory usage */
  sim_mem_usage = (sbrk(0) - &etext) / 1024;
#endif

  /* print simulation stats */
  fprintf(fd, "\nProcessor: %s\nsim: ** simulation statistics **\n", basename());
  SIM->STATS->stat_print_stats(SIM->GLOBAL->sim_sdb, fd);
  SIM->sim_aux_stats(fd);
  fprintf(fd, "\n");


}

/* print stats, uninitialize simulator components, and exit w/ exitcode */
/*static*/ void
simplescalar::exit_now(int exit_code)
{



  /* print simulation stats */
  if (myfd == NULL)
     sim_print_stats(stdout);
  else
     sim_print_stats(myfd);

  /* un-initialize the simulator */
  SIM->sim_uninit();
  SIM->free_objs();
  free(SIM);
  SIM= NULL;

  /* MSO:(06/06/2008) now exit from the SystemC simulation */
   cout <<  "** ExitNow: SystemC simulation time *** "  << sc_simulation_time() << endl;
   //cout <<  "** ExitNow: SystemC simulation time stamp time *** "  << sc_time_stamp() << endl;
   halt();
  /* MSO: halt= stop this processor (or, execute nops ?!?!),
     Ok, in a MPSoC probably we will turnoff or decrease the frequency of this processor */

  /* all done! */
//  exit(exit_code);
}

int
simplescalar::execute(int argc, char **argv, char **envp)
{
  char *s;
  int i, exit_code;

#if 0
#ifndef _MSC_VER
  /* catch SIGUSR1 and dump intermediate stats */
  signal(SIGUSR1, signal_sim_stats);

  /* catch SIGUSR2 and dump final stats and exit */
  signal(SIGUSR2, signal_exit_now);
#endif /* _MSC_VER */
#endif

  /* register an error handler */
  /*fatal_hook(sim_print_stats);*/

  /* set up a non-local exit point */
  if ((exit_code = setjmp(SIM->GLOBAL->sim_exit_buf)) != 0)
    {
      /* special handling as longjmp cannot pass 0 */
      exit_now(exit_code-1);
      return 0;
    }

  /* register global options */
  SIM->GLOBAL->sim_odb = SIM->OPTIONS->opt_new();
  SIM->OPTIONS->opt_reg_flag(SIM->GLOBAL->sim_odb, "-h", "print help message",
	       &SIM->GLOBAL->help_me, /* default */FALSE, /* !print */FALSE, NULL);
  SIM->OPTIONS->opt_reg_flag(SIM->GLOBAL->sim_odb, "-v", "verbose operation",
	       &SIM->GLOBAL->verbose, /* default */FALSE, /* !print */FALSE, NULL);

//#ifdef DEBUG
//  opt_reg_flag(sim_odb, "-d", "enable debug message",
//	       &debugging, /* default */FALSE, /* !print */FALSE, NULL);
//#endif /* DEBUG */
 // opt_reg_flag(sim_odb, "-i", "start in Dlite debugger",
//	       &dlite_active, /* default */FALSE, /* !print */FALSE, NULL);

  SIM->OPTIONS->opt_reg_int(SIM->GLOBAL->sim_odb, "-seed",
	      "random number generator seed (0 for timer seed)",
	      &SIM->GLOBAL->rand_seed, /* default */1, /* print */TRUE, NULL);
  SIM->OPTIONS->opt_reg_flag(SIM->GLOBAL->sim_odb, "-q", "initialize and terminate immediately",
	       &SIM->GLOBAL->init_quit, /* default */FALSE, /* !print */FALSE, NULL);
  SIM->OPTIONS->opt_reg_string(SIM->GLOBAL->sim_odb, "-chkpt", "restore EIO trace execution from <fname>",
		 &SIM->GLOBAL->sim_chkpt_fname, /* default */NULL, /* !print */FALSE, NULL);

  /* stdio redirection options */
  SIM->OPTIONS->opt_reg_string(SIM->GLOBAL->sim_odb, "-redir:sim",
		 "redirect simulator output to file (non-interactive only)",
		 &SIM->GLOBAL->sim_simout,
		 /* default */NULL, /* !print */FALSE, NULL);
  SIM->OPTIONS->opt_reg_string(SIM->GLOBAL->sim_odb, "-redir:prog",
		 "redirect simulated program output to file",
		 &SIM->GLOBAL->sim_progout, /* default */NULL, /* !print */FALSE, NULL);

#ifndef _MSC_VER
  /* scheduling priority option */
  SIM->OPTIONS->opt_reg_int(SIM->GLOBAL->sim_odb, "-nice",
	      "simulator scheduling priority", &nice_priority,
	      /* default */NICE_DEFAULT_VALUE, /* print */TRUE, NULL);
#endif

  /* FIXME: add stats intervals and max insts... */

  /* register all simulator-specific options */
  SIM->sim_reg_options(SIM->GLOBAL->sim_odb);


  /* parse simulator options */
  SIM->GLOBAL->exec_index = -1;
  SIM->OPTIONS->opt_process_options(SIM->GLOBAL->sim_odb, argc, argv);

  /* redirect I/O? */
  if (SIM->GLOBAL->sim_simout != NULL)
    {
      /* send simulator non-interactive output (STDERR) to file SIM_SIMOUT */
      fflush(stderr);
      if (!freopen(SIM->GLOBAL->sim_simout, "w", stderr))
	SIM->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "unable to redirect simulator output to file `%s'", SIM->GLOBAL->sim_simout);
    }

  if (SIM->GLOBAL->sim_progout != NULL)
    {
      /* redirect simulated program output to file SIM_PROGOUT */
      SIM->GLOBAL->sim_progfd = fopen(SIM->GLOBAL->sim_progout, "w");
      if (!SIM->GLOBAL->sim_progfd)
	SIM->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "unable to redirect program output to file `%s'", SIM->GLOBAL->sim_progout);
    }

  /* need at least two argv values to run */
  if (argc < 2)
    {
      banner(stderr, argc, argv);
      usage(stderr, argc, argv);
      exit(1);
    }

  /* opening banner */
  banner(stderr, argc, argv);

  if (SIM->GLOBAL->help_me)
    {
      /* print help message and exit */
      usage(stderr, argc, argv);
      exit(1);
    }

  /* seed the random number generator */
  if (SIM->GLOBAL->rand_seed == 0)
    {
      /* seed with the timer value, true random */
      SIM->MISC->mysrand(time((time_t *)NULL));
    }
  else
    {
      /* seed with default or user-specified random number generator seed */
      SIM->MISC->mysrand(SIM->GLOBAL->rand_seed);
    }

  /* exec_index is set in orphan_fn() */
  if (SIM->GLOBAL->exec_index == -1)
    {
      /* executable was not found */
      fprintf(stderr, "error: no executable specified\n");
      usage(stderr, argc, argv);
      exit(1);
    }
  /* else, exec_index points to simulated program arguments */

  /* check simulator-specific options */
  SIM->sim_check_options(SIM->GLOBAL->sim_odb, argc, argv);

#ifndef _MSC_VER
  /* set simulator scheduling priority */
  if (nice(0) != nice_priority)
    {
      if (nice(nice_priority - nice(0)) < 0)
	SIM->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not renice simulator process");
    }
#endif



  /* default architected value... */
  SIM->GLOBAL->sim_num_insn = 0;



#ifdef BFD_LOADER
  /* initialize the bfd library */
  bfd_init();
#endif /* BFD_LOADER */


  /* initialize the instruction decoder */
  SIM->MACHINE->md_init_decoder();

  /* initialize all simulation modules */
  SIM->sim_init();

  /* initialize architected state */
  SIM->sim_load_prog(argv[SIM->GLOBAL->exec_index], argc - SIM->GLOBAL->exec_index,
					  argv + SIM->GLOBAL->exec_index, envp);

  /* register all simulator stats */
  SIM->GLOBAL->sim_sdb = SIM->STATS->stat_new();

  SIM->sim_reg_stats(SIM->GLOBAL->sim_sdb);

#if 0 /* not portable... :-( */
  stat_reg_uint(sim_sdb, "sim_mem_usage",
		"total simulator (data) memory usage",
		&sim_mem_usage, sim_mem_usage, "%11dk");
#endif

  /* record start of execution time, used in rate stats */
  SIM->GLOBAL->sim_start_time = time((time_t *)NULL);

  /* emit the command line for later reuse */
  fprintf(stderr, "sim: command line: ");
  for (i=0; i < argc; i++)
    fprintf(stderr, "%s ", argv[i]);
  fprintf(stderr, "\n");

  /* output simulation conditions */
  s = ctime(&SIM->GLOBAL->sim_start_time);
  if (s[strlen(s)-1] == '\n')
    s[strlen(s)-1] = '\0';
  fprintf(stderr, "\nsim: simulation started @ %s, options follow:\n", s);
  SIM->OPTIONS->opt_print_options(SIM->GLOBAL->sim_odb, stderr, /* short */TRUE, /* notes */TRUE);
  SIM->sim_aux_config(stderr);
  fprintf(stderr, "\n");

  /* omit option dump time from rate stats */
  SIM->GLOBAL->sim_start_time = time((time_t *)NULL);

  if (SIM->GLOBAL->init_quit)
    exit_now(0);

  SIM->GLOBAL->running = TRUE;
  SIM->sim_main();

  /* simulation finished early */
//  exit_now(0);

  return 0;
}

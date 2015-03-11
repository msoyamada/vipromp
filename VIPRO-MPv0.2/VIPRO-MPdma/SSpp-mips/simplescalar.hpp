#ifndef MAIN_SS_H
#define MAIN_SS_H


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

#include "sim.hpp"
//#include "../sharedmem/memoryshared.hpp"
#include "../bus/simple_bus_blocking_if.hpp"
//#include "timer_if.hpp"

#include <systemc.h>

class simplescalar
	 : public sc_module
{

	public:
		sc_in_clk CLK;
		sc_in<bool> reset;   //handle it?

		// MSG
		sc_port<simple_bus_blocking_if> bus_port;

		unsigned int m_unique_priority;

		SC_HAS_PROCESS(simplescalar);

		//simplescalar();
		simplescalar(sc_module_name name_, unsigned int unique_priority)
		: sc_module(name_)
		, m_unique_priority(unique_priority)
		{
			SC_CTHREAD(cycle,   CLK.pos());
			SC_CTHREAD(refresh, CLK.neg());
			SIM = new sim(this);
			myfd = NULL;
		}


		void refresh();

		void init(int _argc, char **_argv, char **_envp){
			argc = _argc;
			argv = _argv;
			envp = _envp;
		}

		void setFd(FILE *_fd){
			myfd= _fd;
		}

		int execute(int argc, char **argv, char **envp);

		void sim_print_stats(FILE *fd);
		void banner(FILE *fd, int argc, char **argv);
		void usage(FILE *fd, int argc, char **argv);
		void exit_now(int exit_code);
		void free_objs();

		void cycle() {
			execute(argc, argv, envp);
		}

		// implementation of timer_if
		/*void setInterruptMode(void)
		{
			if (SIM != NULL)
				SIM->interrupt = true;
		} */
		sim *SIM;

	private:

		int argc;
		char **argv;
		char **envp;

		FILE *myfd;

};

#endif


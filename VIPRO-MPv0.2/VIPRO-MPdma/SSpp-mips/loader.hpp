#ifndef LOADER_H
#define LOADER_H

#include <stdio.h>
#include <stdlib.h>

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "regs.hpp"
#include "memory.hpp"


/*
 * program segment ranges, valid after calling ld_load_prog()
 */


#ifdef BFD_LOADER
#include <bfd.h>
#else /* !BFD_LOADER */
#include "target-pisa/ecoff.h"
#endif /* BFD_LOADER */

/* amount of tail padding added to all loaded text segments */
#define TEXT_TAIL_PADDING 128

class sim;


class loader
{
	private:
	

		sim *Sim;
	
	public:
	
		loader::loader(sim *);

		/* program text (code) segment base */
		md_addr_t ld_text_base;

		/* program text (code) size in bytes */
		unsigned int ld_text_size;

		/* program initialized data segment base */
		md_addr_t ld_data_base;

		/* program initialized ".data" and uninitialized ".bss" size in bytes */
		unsigned int ld_data_size;

		/* top of the data segment */
		md_addr_t ld_brk_point;

		/* program stack segment base (highest address in stack) */
		md_addr_t ld_stack_base;

		/* program initial stack size */
		unsigned int ld_stack_size;

		/* lowest address accessed on the stack */
		md_addr_t ld_stack_min;

		/* program file name */
		char *ld_prog_fname;

		/* program entry point (initial PC) */
		md_addr_t ld_prog_entry;

		/* program environment base address address */
		md_addr_t ld_environ_base;

		/* target executable endian-ness, non-zero if big endian */
		int ld_target_big_endian;

		/* register simulator-specific statistics */
		void
		ld_reg_stats(struct stat_sdb_t *sdb);	/* stats data base */

		/* load program text and initialized data into simulated virtual memory
		   space and initialize program segment range variables */
		void
		ld_load_prog(char *fname,		/* program to load */
			     int argc, char **argv,	/* simulated program cmd line args */
			     char **envp,		/* simulated program environment */
			     struct regs_t *regs,	/* registers to initialize for load */
			     struct mem_t *mem,		/* memory space to load prog into */
			     int zero_bss_segs);	/* zero uninit data segment? */

};

#endif



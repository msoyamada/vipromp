
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <signal.h>

#include "sim.hpp"

// includes adicionados para linkar os defines
#include "ptrace.hpp"
#include "machine.hpp"
#include "cache.hpp"
#include "regs.hpp"
#include "simplescalar.hpp"

#include "../bus/simple_bus_tools.hpp"

#include <systemc.h>

using namespace std;

#undef GAMBA_MEM
#define GAMBA_MEM MEMORY



/* MSG */
md_fault_type sim::memshared_access(mem_cmd cmd,
		md_addr_t addr,
		void *host,
		int nbytes)
{
	md_fault_type status = md_fault_none;

	tick_t cycle_temp = sim_cycle;
	tick_t cycle_wait_bus_temp = 0;
	num_bus_access++;

	if (cmd == Read)
	{
		if (this->Simplescalar->bus_port->read_ss(addr, host, nbytes, Simplescalar->m_unique_priority, false) == SIMPLE_BUS_ERROR)
			status = md_fault_access;
	}
	else
	{
		if (this->Simplescalar->bus_port->write_ss(addr, host, nbytes,  Simplescalar->m_unique_priority, false) == SIMPLE_BUS_ERROR)
			status = md_fault_access;
	}

	// difference
	cycle_wait_bus_temp = sim_cycle - cycle_temp;

	// the bus access consumes "x" cycles (get_latency)
	// x >= 3
	int cycle_wait = this->Simplescalar->bus_port->get_latency(addr);

	if (cycle_wait_bus_temp > cycle_wait)
		cycle_bus_busy += cycle_wait_bus_temp - cycle_wait;

	cycle_wait_bus += cycle_wait_bus_temp;

	return status;

}

/*
 * this method is for read private memory
 */
md_fault_type sim::mem_read(md_addr_t addr,	/* target address to access - word - unsigned int - 32bits*/
			    int ht,		/* host memory address to access */
			    int nbytes,
			    void *local){	/* this help the dma for private X menshared access*/
	int *host = (int *)malloc(sizeof(int));
	if(local == NULL){								//read of private memory
		if(MEMORY->mem_access(mem, Read, addr, host, nbytes) == md_fault_none){	//from private memory
			return MEMORY->mem_access(mem, Write, ht, host, nbytes);
		}
	}else{
		return MEMORY->mem_access(mem, Read, addr, local, nbytes);		//this help the dma
											//for read private X menshared
											//memory
	}
	return md_fault_none;
}

/*
 * this method is for write private memory
 */
md_fault_type sim::mem_write(md_addr_t addr,	/* target address to access - word - unsigned int - 32bits*/
			    int ht,		/* host memory address to access */
			    int nbytes,
			    void *local){	/* this help the dma for private X menshared access*/
	int *host = (int *)malloc(sizeof(int));
	if(local == NULL){
		if(MEMORY->mem_access(mem, Read, ht, host, nbytes) == md_fault_none){
			return MEMORY->mem_access(mem, Write, addr, host, nbytes);
		}
	}else{
		return MEMORY->mem_access(mem, Write, addr, local, nbytes);	//this help the dma
										//for read private X menshared
										//memory
	}
	return md_fault_none;
}


md_addr_t sim::IACOMPRESS(md_addr_t A)
{
	if (compress_icache_addrs)
		return (((A - LOADER->ld_text_base) >> 1) + LOADER->ld_text_base);
	else
		return A;
}


int sim::ISCOMPRESS(int SZ)
{
	if (compress_icache_addrs)
		return SZ >> 1;
	else
		return SZ;
}


sim::sim(simplescalar *SS)
{

	Simplescalar = SS;

	CACHE 	= new cache	(this);
	SYSC 	= new syscalle	(this);
	MISC 	= new misc	(this);
	MEMORY  = new memory	(this);
	LOADER 	= new loader	(this);
	ENDIAN	= new endian	(this);
	SYMBOL	= new symbol	(this);
	EVAL 	= new eval	(this);
	OPTIONS = new options	(this);
	STATS 	= new stats	(this);
	EIOobj	= new eio	(this);
	RANGE	= new range	(this);
	PTRACE 	= new ptrace	(this);
	REGS 	= new regs	(this);
	POWER 	= new power	(this);
	MACHINE = new machine	(this, MISC);
	BPRED	= new bpred	(this);
	RESOURCE = new resource	(this);
	LIBEXO	= new libexo	(this);
	GLOBAL 	= new global();


	interrupt = false;

	cycle_wait_bus = 0;
	num_bus_access = 0;
	cycle_bus_busy = 0;

	mem = NULL;
	ptrace_nelt = 0;
	bimod_nelt = 1;

	bimod_config[0] = 2048;
	//  { /* bimod tbl size */2048 };

	/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
	twolev_nelt = 4;

	twolev_config[0] = 1;
	twolev_config[1] = 1024;
	twolev_config[2] = 8;
	twolev_config[3] = FALSE;
	// { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE};

	/* combining predictor config (<meta_table_size> */
	comb_nelt = 1;

	comb_config[0] = 1024;
	// { /* meta_table_size */1024 };


	/* return address stack (RAS) size */
	ras_size = 8;

	/* BTB predictor config (<num_sets> <associativity>) */
	btb_nelt = 2;

	btb_config[0] = 512;
	btb_config[1] = 4;
	//  { /* nsets */512, /* assoc */4 };


	/* issue instructions down wrong execution paths */
	ruu_include_spec = TRUE;

	/* register update unit (RUU) size */
	RUU_size = 8;

	/* load/store queue (LSQ) size */
	LSQ_size = 4;

	/* memory access latency (<first_chunk> <inter_chunk>) */
	mem_nelt = 2;
	mem_lat[0] = 18;
	mem_lat[1] = 2;
	//{ /* lat to first chunk */18, /* lat between remaining chunks */2 };

	/* options for Wattch */
	data_width = 64;

	/* counters added for Wattch */
	rename_access=0;
	bpred_access=0;
	window_access=0;
	lsq_access=0;
	regfile_access=0;
	icache_access=0;
	dcache_access=0;
	dcache2_access=0;
	alu_access=0;
	ialu_access=0;
	falu_access=0;
	resultbus_access=0;

	window_preg_access=0;
	window_selection_access=0;
	window_wakeup_access=0;
	lsq_store_data_access=0;
	lsq_load_data_access=0;
	lsq_preg_access=0;
	lsq_wakeup_access=0;

	window_total_pop_count_cycle=0;
	window_num_pop_count_cycle=0;
	lsq_total_pop_count_cycle=0;
	lsq_num_pop_count_cycle=0;
	regfile_total_pop_count_cycle=0;
	regfile_num_pop_count_cycle=0;
	resultbus_total_pop_count_cycle=0;
	resultbus_num_pop_count_cycle=0;

	pcstat_nelt = 0;

	/*
	 * simulator stats
	 */

	/* total number of instructions executed */
	sim_total_insn = 0;

	/* total number of memory references committed */
	sim_num_refs = 0;

	/* total number of memory references executed */
	sim_total_refs = 0;

	/* total number of loads committed */
	sim_num_loads = 0;

	/* total number of loads executed */
	sim_total_loads = 0;

	/* total number of branches committed */
	sim_num_branches = 0;

	/* total number of branches executed */
	sim_total_branches = 0;

	/* cycle counter */
	sim_cycle = 0;


	/*
	 * simulator state variables
	 */

	/* instruction sequence counter, used to assign unique id's to insts */
	inst_seq = 0;

	/* pipetrace instruction sequence counter */
	ptrace_seq = 0;

	/* speculation mode, non-zero when mis-speculating, i.e., executing
	   instructions down the wrong path, thus state recovery will eventually have
	   to occur that resets processor register and memory state back to the last
	   precise state */
	spec_mode = FALSE;

	/* cycles until fetch issue resumes */
	ruu_fetch_issue_delay = 0;

	/* perfect prediction enabled */
	pred_perfect = FALSE;


	/* functional unit resource pool */
	fu_pool = NULL;


	RSLINK_NULL.next = NULL;
	RSLINK_NULL.rs = NULL;
	RSLINK_NULL.tag = 0;


	// RSLINK_NULL_DATA;

	/* a NULL create vector entry */
	static struct CV_link CVLINK_NULL = { NULL, 0 };


	/* speculative memory hash table bucket free list */
	bucket_free_list = NULL;



	/* the last operation that ruu_dispatch() attempted to dispatch, for
	   implementing in-order issue */
	last_op.next = NULL;
	last_op.rs = NULL;
	last_op.tag = 0;
	// = RSLINK_NULL_DATA

	last_inst_missed = FALSE;
	last_inst_tmissed = FALSE;

}

//--------------------------------------------------------------------------terminou o construtor


void sim::free_objs()
{
	free(CACHE);
	free(SYSC);
	free(MISC);
	free(MEMORY);
	free(LOADER);
	free(ENDIAN);
	free(SYMBOL);
	free(EVAL);
	free(OPTIONS);
	free(STATS );
	free(EIOobj);
	free(RANGE);
	free(PTRACE);
	free(REGS);
	//	free(POWER);
	free(BPRED);
	free(RESOURCE);
	free(LIBEXO);
	free(GLOBAL);
	//	free(MACHINE);
}


/* memory access latency, assumed to not cross a page boundary */
/*static*/ unsigned int			/* total latency of access */
sim::mem_access_latency(int blk_sz)		/* block size accessed */
{
	int chunks = (blk_sz + (mem_bus_width - 1)) / mem_bus_width;

	assert(chunks > 0);

	return (/* first chunk latency */mem_lat[0] +
			(/* remainder chunk latency */mem_lat[1] * (chunks - 1)));
}


/*
 * cache miss handlers
 */

/* l1 data cache l1 block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::dl1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
		md_addr_t baddr,		/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{
	unsigned int lat;

	if (cache_dl2)
	{
		/* access next level of data cache hierarchy */
		lat = CACHE->cache_access(cache_dl2, cmd, baddr, NULL, bsize,
				/* now */now, /* pudata */NULL, /* repl addr */NULL);

		/* Wattch -- Dcache2 access */
		dcache2_access++;

		if (cmd == Read)
			return lat;
		else
		{
			/* FIXME: unlimited write buffers */
			return 0;
		}
	}
	else
	{
		/* access main memory */
		if (cmd == Read)
			return mem_access_latency(bsize);
		else
		{
			/* FIXME: unlimited write buffers */
			return 0;
		}
	}
}

/* l2 data cache block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::dl2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
		md_addr_t baddr,		/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{

	/* Wattch -- main memory access -- Wattch-FIXME (offchip) */

	/* this is a miss to the lowest level, so access main memory */
	if (cmd == Read)
		return mem_access_latency(bsize);
	else
	{
		/* FIXME: unlimited write buffers */
		return 0;
	}
}

/* l1 inst cache l1 block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::il1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
		md_addr_t baddr,		/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{
	unsigned int lat;

	if (cache_il2)
	{
		/* access next level of inst cache hierarchy */
		lat = CACHE->cache_access(cache_il2, cmd, baddr, NULL, bsize,
				/* now */now, /* pudata */NULL, /* repl addr */NULL);

		/* Wattch -- Dcache2 access */
		dcache2_access++;

		if (cmd == Read)
			return lat;
		else
			panic("writes to instruction memory not supported");
	}
	else
	{
		/* access main memory */
		if (cmd == Read)
			return mem_access_latency(bsize);
		else
			panic("writes to instruction memory not supported");
	}
}

/* l2 inst cache block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::il2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
		md_addr_t baddr,		/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{
	/* Wattch -- main memory access -- Wattch-FIXME (offchip) */

	/* this is a miss to the lowest level, so access main memory */
	if (cmd == Read)
		return mem_access_latency(bsize);
	else
		panic("writes to instruction memory not supported");
}


/*
 * TLB miss handlers
 */

/* inst cache block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::itlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,		/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{
	md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert(phy_page_ptr);

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}

/* data cache block miss handler function */
/*static*/ unsigned int			/* latency of block access */
sim::dtlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
		md_addr_t baddr,	/* block address to access */
		int bsize,		/* size of block to access */
		struct cache_blk_t *blk,	/* ptr to block in upper level */
		tick_t now)		/* time of access */
{
	md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert(phy_page_ptr);

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}


/* register simulator-specific options */
void
sim::sim_reg_options(struct opt_odb_t *odb)
{
	OPTIONS->opt_reg_header(odb,
			"sim-outorder: This simulator implements a very detailed out-of-order issue\n"
			"superscalar processor with a two-level memory system and speculative\n"
			"execution support.  This simulator is a performance simulator, tracking the\n"
			"latency of all pipeline operations.\n"
	);

	/* instruction limit */

	OPTIONS->opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
			&max_insts, /* default */0,
			/* print */TRUE, /* format */NULL);

	/* trace options */

	OPTIONS->opt_reg_int(odb, "-fastfwd", "number of insts skipped before timing starts",
			&fastfwd_count, /* default */0,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_string_list(odb, "-ptrace",
			"generate pipetrace, i.e., <fname|stdout|stderr> <range>",
			ptrace_opts, /* arr_sz */2, &ptrace_nelt, /* default */NULL,
			/* !print */FALSE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_note(odb,
			"  Pipetrace range arguments are formatted as follows:\n"
			"\n"
			"    {{@|#}<start>}:{{@|#|+}<end>}\n"
			"\n"
			"  Both ends of the range are optional, if neither are specified, the entire\n"
			"  execution is traced.  Ranges that start with a `@' designate an address\n"
			"  range to be traced, those that start with an `#' designate a cycle count\n"
			"  range.  All other range values represent an instruction count range.  The\n"
			"  second argument, if specified with a `+', indicates a value relative\n"
			"  to the first argument, e.g., 1000:+100 == 1000:1100.  Program symbols may\n"
			"  be used in all contexts.\n"
			"\n"
			"    Examples:   -ptrace FOO.trc #0:#1000\n"
			"                -ptrace BAR.trc @2000:\n"
			"                -ptrace BLAH.trc :1500\n"
			"                -ptrace UXXE.trc :\n"
			"                -ptrace FOOBAR.trc @main:+278\n"
	);

	/* ifetch options */

	OPTIONS->opt_reg_int(odb, "-fetch:ifqsize", "instruction fetch queue size (in insts)",
			&ruu_ifq_size, /* default */4,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-fetch:mplat", "extra branch mis-prediction latency",
			&ruu_branch_penalty, /* default */3,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-fetch:speed",
			"speed of front-end of machine relative to execution core",
			&fetch_speed, /* default */1,
			/* print */TRUE, /* format */NULL);

	/* branch predictor options */

	OPTIONS->opt_reg_note(odb,
			"  Branch predictor configuration examples for 2-level predictor:\n"
			"    Configurations:   N, M, W, X\n"
			"      N   # entries in first level (# of shift register(s))\n"
			"      W   width of shift register(s)\n"
			"      M   # entries in 2nd level (# of counters, or other FSM)\n"
			"      X   (yes-1/no-0) xor history and address for 2nd level index\n"
			"    Sample predictors:\n"
			"      GAg     : 1, W, 2^W, 0\n"
			"      GAp     : 1, W, M (M > 2^W), 0\n"
			"      PAg     : N, W, 2^W, 0\n"
			"      PAp     : N, W, M (M == 2^(N+W)), 0\n"
			"      gshare  : 1, W, 2^W, 1\n"
			"  Predictor `comb' combines a bimodal and a 2-level predictor.\n"
	);

	OPTIONS->opt_reg_string(odb, "-bpred",
			"branch predictor type {nottaken|taken|perfect|bimod|2lev|comb}",
			&pred_type, /* default */"bimod",
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int_list(odb, "-bpred:bimod",
			"bimodal predictor config (<table size>)",
			bimod_config, bimod_nelt, &bimod_nelt,
			/* default */bimod_config,
			/* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_int_list(odb, "-bpred:2lev",
			"2-level predictor config "
			"(<l1size> <l2size> <hist_size> <xor>)",
			twolev_config, twolev_nelt, &twolev_nelt,
			/* default */twolev_config,
			/* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_int_list(odb, "-bpred:comb",
			"combining predictor config (<meta_table_size>)",
			comb_config, comb_nelt, &comb_nelt,
			/* default */comb_config,
			/* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_int(odb, "-bpred:ras",
			"return address stack size (0 for no return stack)",
			&ras_size, /* default */ras_size,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int_list(odb, "-bpred:btb",
			"BTB config (<num_sets> <associativity>)",
			btb_config, btb_nelt, &btb_nelt,
			/* default */btb_config,
			/* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_string(odb, "-bpred:spec_update",
			"speculative predictors update in {ID|WB} (default non-spec)",
			&bpred_spec_opt, /* default */NULL,
			/* print */TRUE, /* format */NULL);

	/* decode options */

	OPTIONS->opt_reg_int(odb, "-decode:width",
			"instruction decode B/W (insts/cycle)",
			&ruu_decode_width, /* default */4,
			/* print */TRUE, /* format */NULL);

	/* issue options */

	OPTIONS->opt_reg_int(odb, "-issue:width",
			"instruction issue B/W (insts/cycle)",
			&ruu_issue_width, /* default */4,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_flag(odb, "-issue:inorder", "run pipeline with in-order issue",
			&ruu_inorder_issue, /* default */FALSE,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_flag(odb, "-issue:wrongpath",
			"issue instructions down wrong execution paths",
			&ruu_include_spec, /* default */TRUE,
			/* print */TRUE, /* format */NULL);

	/* commit options */

	OPTIONS->opt_reg_int(odb, "-commit:width",
			"instruction commit B/W (insts/cycle)",
			&ruu_commit_width, /* default */4,
			/* print */TRUE, /* format */NULL);

	/* register scheduler options */

	OPTIONS->opt_reg_int(odb, "-ruu:size",
			"register update unit (RUU) size",
			&RUU_size, /* default */16,
			/* print */TRUE, /* format */NULL);

	/* memory scheduler options  */

	OPTIONS->opt_reg_int(odb, "-lsq:size",
			"load/store queue (LSQ) size",
			&LSQ_size, /* default */8,
			/* print */TRUE, /* format */NULL);

	/* cache options */

	OPTIONS->opt_reg_string(odb, "-cache:dl1",
			"l1 data cache config, i.e., {<config>|none}",
			&cache_dl1_opt, "dl1:128:32:4:l",
			/* print */TRUE, NULL);

	OPTIONS->opt_reg_note(odb,
			"  The cache config parameter <config> has the following format:\n"
			"\n"
			"    <name>:<nsets>:<bsize>:<assoc>:<repl>\n"
			"\n"
			"    <name>   - name of the cache being defined\n"
			"    <nsets>  - number of sets in the cache\n"
			"    <bsize>  - block size of the cache\n"
			"    <assoc>  - associativity of the cache\n"
			"    <repl>   - block replacement strategy, 'l'-LRU, 'f'-FIFO, 'r'-random\n"
			"\n"
			"    Examples:   -cache:dl1 dl1:4096:32:1:l\n"
			"                -dtlb dtlb:128:4096:32:r\n"
	);

	OPTIONS->opt_reg_int(odb, "-cache:dl1lat",
			"l1 data cache hit latency (in cycles)",
			&cache_dl1_lat, /* default */1,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_string(odb, "-cache:dl2",
			"l2 data cache config, i.e., {<config>|none}",
			&cache_dl2_opt, "ul2:1024:64:4:l",
			/* print */TRUE, NULL);

	OPTIONS->opt_reg_int(odb, "-cache:dl2lat",
			"l2 data cache hit latency (in cycles)",
			&cache_dl2_lat, /* default */6,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_string(odb, "-cache:il1",
			"l1 inst cache config, i.e., {<config>|dl1|dl2|none}",
			&cache_il1_opt, "il1:512:32:1:l",
			/* print */TRUE, NULL);

	OPTIONS->opt_reg_note(odb,
			"  Cache levels can be unified by pointing a level of the instruction cache\n"
			"  hierarchy at the data cache hiearchy using the \"dl1\" and \"dl2\" cache\n"
			"  configuration arguments.  Most sensible combinations are supported, e.g.,\n"
			"\n"
			"    A unified l2 cache (il2 is pointed at dl2):\n"
			"      -cache:il1 il1:128:64:1:l -cache:il2 dl2\n"
			"      -cache:dl1 dl1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
			"\n"
			"    Or, a fully unified cache hierarchy (il1 pointed at dl1):\n"
			"      -cache:il1 dl1\n"
			"      -cache:dl1 ul1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
	);

	OPTIONS->opt_reg_int(odb, "-cache:il1lat",
			"l1 instruction cache hit latency (in cycles)",
			&cache_il1_lat, /* default */1,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_string(odb, "-cache:il2",
			"l2 instruction cache config, i.e., {<config>|dl2|none}",
			&cache_il2_opt, "dl2",
			/* print */TRUE, NULL);

	OPTIONS->opt_reg_int(odb, "-cache:il2lat",
			"l2 instruction cache hit latency (in cycles)",
			&cache_il2_lat, /* default */6,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_flag(odb, "-cache:flush", "flush caches on system calls",
			&flush_on_syscalls, /* default */FALSE, /* print */TRUE, NULL);

	OPTIONS->opt_reg_flag(odb, "-cache:icompress",
			"convert 64-bit inst addresses to 32-bit inst equivalents",
			&compress_icache_addrs, /* default */FALSE,
			/* print */TRUE, NULL);

	/* mem options */
	OPTIONS->opt_reg_int_list(odb, "-mem:lat",
			"memory access latency (<first_chunk> <inter_chunk>)",
			mem_lat, mem_nelt, &mem_nelt, mem_lat,
			/* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	OPTIONS->opt_reg_int(odb, "-mem:width", "memory access bus width (in bytes)",
			&mem_bus_width, /* default */8,
			/* print */TRUE, /* format */NULL);

	/* TLB options */

	OPTIONS->opt_reg_string(odb, "-tlb:itlb",
			"instruction TLB config, i.e., {<config>|none}",
			&itlb_opt, "itlb:16:4096:4:l", /* print */TRUE, NULL);

	OPTIONS->opt_reg_string(odb, "-tlb:dtlb",
			"data TLB config, i.e., {<config>|none}",
			&dtlb_opt, "dtlb:32:4096:4:l", /* print */TRUE, NULL);

	OPTIONS->opt_reg_int(odb, "-tlb:lat",
			"inst/data TLB miss latency (in cycles)",
			&tlb_miss_lat, /* default */30,
			/* print */TRUE, /* format */NULL);

	/* resource configuration */

	OPTIONS->opt_reg_int(odb, "-res:ialu",
			"total number of integer ALU's available",
			&res_ialu, /* default */fu_config[FU_IALU_INDEX].quantity,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-res:imult",
			"total number of integer multiplier/dividers available",
			&res_imult, /* default */fu_config[FU_IMULT_INDEX].quantity,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-res:memport",
			"total number of memory system ports available (to CPU)",
			&res_memport, /* default */fu_config[FU_MEMPORT_INDEX].quantity,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-res:fpalu",
			"total number of floating point ALU's available",
			&res_fpalu, /* default */fu_config[FU_FPALU_INDEX].quantity,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_int(odb, "-res:fpmult",
			"total number of floating point multiplier/dividers available",
			&res_fpmult, /* default */fu_config[FU_FPMULT_INDEX].quantity,
			/* print */TRUE, /* format */NULL);

	OPTIONS->opt_reg_string_list(odb, "-pcstat",
			"profile stat(s) against text addr's (mult uses ok)",
			pcstat_vars, MAX_PCSTAT_VARS, &pcstat_nelt, NULL,
			/* !print */FALSE, /* format */NULL, /* accrue */TRUE);

	OPTIONS->opt_reg_flag(odb, "-bugcompat",
			"operate in backward-compatible bugs mode (for testing only)",
			&bugcompat_mode, /* default */FALSE, /* print */TRUE, NULL);
}

/* check simulator-specific option values */
void
sim::sim_check_options(struct opt_odb_t *odb,        /* options database */
		int argc, char **argv)        /* command line arguments */
{
	char name[128], c;
	int nsets, bsize, assoc;

	if (fastfwd_count < 0 || fastfwd_count >= 2147483647)
		fatal("bad fast forward count: %d", fastfwd_count);

	if (ruu_ifq_size < 1 || (ruu_ifq_size & (ruu_ifq_size - 1)) != 0)
		fatal("inst fetch queue size must be positive > 0 and a power of two");

	if (ruu_branch_penalty < 1)
		fatal("mis-prediction penalty must be at least 1 cycle");

	if (fetch_speed < 1)
		fatal("front-end speed must be positive and non-zero");

	if (!MISC->mystricmp(pred_type, "perfect"))
	{
		/* perfect predictor */
		pred = NULL;
		pred_perfect = TRUE;
	}
	else if (!MISC->mystricmp(pred_type, "taken"))
	{
		/* static predictor, not taken */
		pred = BPRED->bpred_create(BPredTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else if (!MISC->mystricmp(pred_type, "nottaken"))
	{
		/* static predictor, taken */
		pred = BPRED->bpred_create(BPredNotTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else if (!MISC->mystricmp(pred_type, "bimod"))
	{
		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		if (bimod_nelt != 1)
			fatal("bad bimod predictor config (<table_size>)");
		if (btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		pred = BPRED->bpred_create(BPred2bit,
				/* bimod table size */bimod_config[0],
				/* 2lev l1 size */0,
				/* 2lev l2 size */0,
				/* meta table size */0,
				/* history reg size */0,
				/* history xor address */0,
				/* btb sets */btb_config[0],
				/* btb assoc */btb_config[1],
				/* ret-addr stack size */ras_size);
	}
	else if (!MISC->mystricmp(pred_type, "2lev"))
	{
		/* 2-level adaptive predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		pred = BPRED->bpred_create(BPred2Level,
				/* bimod table size */0,
				/* 2lev l1 size */twolev_config[0],
				/* 2lev l2 size */twolev_config[1],
				/* meta table size */0,
				/* history reg size */twolev_config[2],
				/* history xor address */twolev_config[3],
				/* btb sets */btb_config[0],
				/* btb assoc */btb_config[1],
				/* ret-addr stack size */ras_size);
	}
	else if (!MISC->mystricmp(pred_type, "comb"))
	{
		/* combining predictor, bpred_create() checks args */
		if (twolev_nelt != 4)
			fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if (bimod_nelt != 1)
			fatal("bad bimod predictor config (<table_size>)");
		if (comb_nelt != 1)
			fatal("bad combining predictor config (<meta_table_size>)");
		if (btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		pred = BPRED->bpred_create(BPredComb,
				/* bimod table size */bimod_config[0],
				/* l1 size */twolev_config[0],
				/* l2 size */twolev_config[1],
				/* meta table size */comb_config[0],
				/* history reg size */twolev_config[2],
				/* history xor address */twolev_config[3],
				/* btb sets */btb_config[0],
				/* btb assoc */btb_config[1],
				/* ret-addr stack size */ras_size);
	}
	else
		fatal("cannot parse predictor type `%s'", pred_type);

	if (!bpred_spec_opt)
		bpred_spec_update = spec_CT;
	else if (!MISC->mystricmp(bpred_spec_opt, "ID"))
		bpred_spec_update = spec_ID;
	else if (!MISC->mystricmp(bpred_spec_opt, "WB"))
		bpred_spec_update = spec_WB;
	else
		fatal("bad speculative update stage specifier, use {ID|WB}");

	if (ruu_decode_width < 1 || (ruu_decode_width & (ruu_decode_width-1)) != 0)
		fatal("issue width must be positive non-zero and a power of two");

	if (ruu_issue_width < 1 || (ruu_issue_width & (ruu_issue_width-1)) != 0)
		fatal("issue width must be positive non-zero and a power of two");

	if (ruu_commit_width < 1)
		fatal("commit width must be positive non-zero");

	if (RUU_size < 2 || (RUU_size & (RUU_size-1)) != 0)
		fatal("RUU size must be a positive number > 1 and a power of two");

	if (LSQ_size < 2 || (LSQ_size & (LSQ_size-1)) != 0)
		fatal("LSQ size must be a positive number > 1 and a power of two");

	/* use a level 1 D-cache? */
	if (!MISC->mystricmp(cache_dl1_opt, "none"))
	{
		cache_dl1 = NULL;

		/* the level 2 D-cache cannot be defined */
		if (strcmp(cache_dl2_opt, "none"))
			fatal("the l1 data cache must defined if the l2 cache is defined");
		cache_dl2 = NULL;
	}
	else /* dl1 is defined */
	{
		if (sscanf(cache_dl1_opt, "%[^:]:%d:%d:%d:%c",
				name, &nsets, &bsize, &assoc, &c) != 5)
			fatal("bad l1 D-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");


		cache_dl1 = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */0, assoc, CACHE->cache_char2policy(c),
				/*dl1_access_fn,*/ /* hit lat */cache_dl1_lat, (pointer_fn_sim) dl1_access_fn_i);


		/* is the level 2 D-cache defined? */
		if (!MISC->mystricmp(cache_dl2_opt, "none"))
			cache_dl2 = NULL;
		else
		{
			if (sscanf(cache_dl2_opt, "%[^:]:%d:%d:%d:%c",
					name, &nsets, &bsize, &assoc, &c) != 5)
				fatal("bad l2 D-cache parms: "
						"<name>:<nsets>:<bsize>:<assoc>:<repl>");
			cache_dl2 = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
					/* usize */0, assoc, CACHE->cache_char2policy(c),
					/*dl2_access_fn,*/ /* hit lat */cache_dl2_lat, (pointer_fn_sim) dl2_access_fn_i);

		}
	}

	/* use a level 1 I-cache? */
	if (!MISC->mystricmp(cache_il1_opt, "none"))
	{
		cache_il1 = NULL;

		/* the level 2 I-cache cannot be defined */
		if (strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else if (!MISC->mystricmp(cache_il1_opt, "dl1"))
	{
		if (!cache_dl1)
			fatal("I-cache l1 cannot access D-cache l1 as it's undefined");
		cache_il1 = cache_dl1;

		/* the level 2 I-cache cannot be defined */
		if (strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else if (!MISC->mystricmp(cache_il1_opt, "dl2"))
	{
		if (!cache_dl2)
			fatal("I-cache l1 cannot access D-cache l2 as it's undefined");
		cache_il1 = cache_dl2;

		/* the level 2 I-cache cannot be defined */
		if (strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
	}
	else /* il1 is defined */
	{
		if (sscanf(cache_il1_opt, "%[^:]:%d:%d:%d:%c",
				name, &nsets, &bsize, &assoc, &c) != 5)
			fatal("bad l1 I-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
		cache_il1 = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */0, assoc, CACHE->cache_char2policy(c),
				/*il1_access_fn,*/ /* hit lat */cache_il1_lat, (pointer_fn_sim) il1_access_fn_i );

		/* is the level 2 D-cache defined? */
		if (!MISC->mystricmp(cache_il2_opt, "none"))
			cache_il2 = NULL;
		else if (!MISC->mystricmp(cache_il2_opt, "dl2"))
		{
			if (!cache_dl2)
				fatal("I-cache l2 cannot access D-cache l2 as it's undefined");
			cache_il2 = cache_dl2;
		}
		else
		{
			if (sscanf(cache_il2_opt, "%[^:]:%d:%d:%d:%c",
					name, &nsets, &bsize, &assoc, &c) != 5)
				fatal("bad l2 I-cache parms: "
						"<name>:<nsets>:<bsize>:<assoc>:<repl>");
			cache_il2 = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
					/* usize */0, assoc, CACHE->cache_char2policy(c),
					/*il2_access_fn,*/ /* hit lat */cache_il2_lat, (pointer_fn_sim) il2_access_fn_i);
		}
	}

	/* use an I-TLB? */
	if (!MISC->mystricmp(itlb_opt, "none"))
		itlb = NULL;
	else
	{
		if (sscanf(itlb_opt, "%[^:]:%d:%d:%d:%c",
				name, &nsets, &bsize, &assoc, &c) != 5)
			fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		itlb = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */sizeof(md_addr_t), assoc,
				CACHE->cache_char2policy(c), /*itlb_access_fn,*/
				/* hit latency */1, (pointer_fn_sim) itlb_access_fn_i);
	}

	/* use a D-TLB? */
	if (!MISC->mystricmp(dtlb_opt, "none"))
		dtlb = NULL;
	else
	{
		if (sscanf(dtlb_opt, "%[^:]:%d:%d:%d:%c",
				name, &nsets, &bsize, &assoc, &c) != 5)
			fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		dtlb = CACHE->cache_create(name, nsets, bsize, /* balloc */FALSE,
				/* usize */sizeof(md_addr_t), assoc,
				CACHE->cache_char2policy(c), /*dtlb_access_fn,*/
				/* hit latency */1, (pointer_fn_sim) dtlb_access_fn_i);
	}

	if (cache_dl1_lat < 1)
		fatal("l1 data cache latency must be greater than zero");

	if (cache_dl2_lat < 1)
		fatal("l2 data cache latency must be greater than zero");

	if (cache_il1_lat < 1)
		fatal("l1 instruction cache latency must be greater than zero");

	if (cache_il2_lat < 1)
		fatal("l2 instruction cache latency must be greater than zero");

	if (mem_nelt != 2)
		fatal("bad memory access latency (<first_chunk> <inter_chunk>)");

	if (mem_lat[0] < 1 || mem_lat[1] < 1)
		fatal("all memory access latencies must be greater than zero");

	if (mem_bus_width < 1 || (mem_bus_width & (mem_bus_width-1)) != 0)
		fatal("memory bus width must be positive non-zero and a power of two");

	if (tlb_miss_lat < 1)
		fatal("TLB miss latency must be greater than zero");

	if (res_ialu < 1)
		fatal("number of integer ALU's must be greater than zero");
	if (res_ialu > MAX_INSTS_PER_CLASS)
		fatal("number of integer ALU's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_IALU_INDEX].quantity = res_ialu;

	if (res_imult < 1)
		fatal("number of integer multiplier/dividers must be greater than zero");
	if (res_imult > MAX_INSTS_PER_CLASS)
		fatal("number of integer mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_IMULT_INDEX].quantity = res_imult;

	if (res_memport < 1)
		fatal("number of memory system ports must be greater than zero");
	if (res_memport > MAX_INSTS_PER_CLASS)
		fatal("number of memory system ports must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_MEMPORT_INDEX].quantity = res_memport;

	if (res_fpalu < 1)
		fatal("number of floating point ALU's must be greater than zero");
	if (res_fpalu > MAX_INSTS_PER_CLASS)
		fatal("number of floating point ALU's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPALU_INDEX].quantity = res_fpalu;

	if (res_fpmult < 1)
		fatal("number of floating point multiplier/dividers must be > zero");
	if (res_fpmult > MAX_INSTS_PER_CLASS)
		fatal("number of FP mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPMULT_INDEX].quantity = res_fpmult;
}

/* print simulator-specific configuration information */
void
sim::sim_aux_config(FILE *stream)            /* output stream */
{
	/* nada */
}

/* register simulator-specific statistics */
void
sim::sim_reg_stats(struct stat_sdb_t *sdb)   /* stats database */
{
	int i;

	/* register baseline stats */
	stat_reg_counter(sdb, "sim_num_insn",
			"total number of instructions committed",
			&GLOBAL->sim_num_insn, GLOBAL->sim_num_insn, NULL);
	stat_reg_counter(sdb, "sim_num_refs",
			"total number of loads and stores committed",
			&sim_num_refs, 0, NULL);
	stat_reg_counter(sdb, "sim_num_loads",
			"total number of loads committed",
			&sim_num_loads, 0, NULL);
	STATS->stat_reg_formula(sdb, "sim_num_stores",
			"total number of stores committed",
			"sim_num_refs - sim_num_loads", NULL);
	stat_reg_counter(sdb, "sim_num_branches",
			"total number of branches committed",
			&sim_num_branches, /* initial value */0, /* format */NULL);
	STATS->stat_reg_int(sdb, "sim_elapsed_time",
			"total simulation time in seconds",
			&GLOBAL->sim_elapsed_time, 0, NULL);
	STATS->stat_reg_formula(sdb, "sim_inst_rate",
			"simulation speed (in insts/sec)",
			"sim_num_insn / sim_elapsed_time", NULL);

	stat_reg_counter(sdb, "sim_total_insn",
			"total number of instructions executed",
			&sim_total_insn, 0, NULL);
	stat_reg_counter(sdb, "sim_total_refs",
			"total number of loads and stores executed",
			&sim_total_refs, 0, NULL);
	stat_reg_counter(sdb, "sim_total_loads",
			"total number of loads executed",
			&sim_total_loads, 0, NULL);
	STATS->stat_reg_formula(sdb, "sim_total_stores",
			"total number of stores executed",
			"sim_total_refs - sim_total_loads", NULL);
	stat_reg_counter(sdb, "sim_total_branches",
			"total number of branches executed",
			&sim_total_branches, /* initial value */0, /* format */NULL);

	/* register performance stats */
	stat_reg_counter(sdb, "sim_cycle",
			"total simulation time in cycles",
			&sim_cycle, /* initial value */0, /* format */NULL);

	/*MSG: cycle waiting for bus and number access */
	stat_reg_counter(sdb, "num_bus_access",
			"total number of access bus", &num_bus_access, 0, NULL);
	stat_reg_counter(sdb, "cycle_wait_bus",
			"total cycle waiting for bus", &cycle_wait_bus, 0, NULL);
	stat_reg_counter(sdb, "cycle_bus_busy",
			"total cycle waiting (wasted) for bus busy", &cycle_bus_busy, 0, NULL);
	/*---------------------------------------------*/

	STATS->stat_reg_formula(sdb, "sim_IPC",
			"instructions per cycle",
			"sim_num_insn / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "sim_CPI",
			"cycles per instruction",
			"sim_cycle / sim_num_insn", /* format */NULL);
	STATS->stat_reg_formula(sdb, "sim_exec_BW",
			"total instructions (mis-spec + committed) per cycle",
			"sim_total_insn / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "sim_IPB",
			"instruction per branch",
			"sim_num_insn / sim_num_branches", /* format */NULL);

	/* occupancy stats */
	stat_reg_counter(sdb, "IFQ_count", "cumulative IFQ occupancy",
			&IFQ_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, "IFQ_fcount", "cumulative IFQ full count",
			&IFQ_fcount, /* initial value */0, /* format */NULL);
	STATS->stat_reg_formula(sdb, "ifq_occupancy", "avg IFQ occupancy (insn's)",
			"IFQ_count / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ifq_rate", "avg IFQ dispatch rate (insn/cycle)",
			"sim_total_insn / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ifq_latency", "avg IFQ occupant latency (cycle's)",
			"ifq_occupancy / ifq_rate", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ifq_full", "fraction of time (cycle's) IFQ was full",
			"IFQ_fcount / sim_cycle", /* format */NULL);

	stat_reg_counter(sdb, "RUU_count", "cumulative RUU occupancy",
			&RUU_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, "RUU_fcount", "cumulative RUU full count",
			&RUU_fcount, /* initial value */0, /* format */NULL);
	STATS->stat_reg_formula(sdb, "ruu_occupancy", "avg RUU occupancy (insn's)",
			"RUU_count / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ruu_rate", "avg RUU dispatch rate (insn/cycle)",
			"sim_total_insn / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ruu_latency", "avg RUU occupant latency (cycle's)",
			"ruu_occupancy / ruu_rate", /* format */NULL);
	STATS->stat_reg_formula(sdb, "ruu_full", "fraction of time (cycle's) RUU was full",
			"RUU_fcount / sim_cycle", /* format */NULL);

	stat_reg_counter(sdb, "LSQ_count", "cumulative LSQ occupancy",
			&LSQ_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, "LSQ_fcount", "cumulative LSQ full count",
			&LSQ_fcount, /* initial value */0, /* format */NULL);
	STATS->stat_reg_formula(sdb, "lsq_occupancy", "avg LSQ occupancy (insn's)",
			"LSQ_count / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "lsq_rate", "avg LSQ dispatch rate (insn/cycle)",
			"sim_total_insn / sim_cycle", /* format */NULL);
	STATS->stat_reg_formula(sdb, "lsq_latency", "avg LSQ occupant latency (cycle's)",
			"lsq_occupancy / lsq_rate", /* format */NULL);
	STATS->stat_reg_formula(sdb, "lsq_full", "fraction of time (cycle's) LSQ was full",
			"LSQ_fcount / sim_cycle", /* format */NULL);

	/* register predictor stats */
	if (pred)
		BPRED->bpred_reg_stats(pred, sdb);

	/* register cache stats */
	if (cache_il1
			&& (cache_il1 != cache_dl1 && cache_il1 != cache_dl2))
		CACHE->cache_reg_stats(cache_il1, sdb);
	if (cache_il2
			&& (cache_il2 != cache_dl1 && cache_il2 != cache_dl2))
		CACHE->cache_reg_stats(cache_il2, sdb);
	if (cache_dl1)
		CACHE->cache_reg_stats(cache_dl1, sdb);
	if (cache_dl2)
		CACHE->cache_reg_stats(cache_dl2, sdb);
	if (itlb)
		CACHE->cache_reg_stats(itlb, sdb);
	if (dtlb)
		CACHE->cache_reg_stats(dtlb, sdb);

	/* register power stats */
	POWER->power_reg_stats(sdb);

	/* debug variable(s) */
	stat_reg_counter(sdb, "sim_invalid_addrs",
			"total non-speculative bogus addresses seen (debug var)",
			&sim_invalid_addrs, /* initial value */0, /* format */NULL);

	for (i=0; i<pcstat_nelt; i++)
	{
		char buf[512], buf1[512];
		struct stat_stat_t *stat;

		/* track the named statistical variable by text address */

		/* find it... */
		stat = STATS->stat_find_stat(sdb, pcstat_vars[i]);
		if (!stat)
			fatal("cannot locate any statistic named `%s'", pcstat_vars[i]);

		/* stat must be an integral type */
		if (stat->sc != msc_int && stat->sc != msc_uint && stat->sc != msc_counter)
			fatal("`-pcstat' statistical variable `%s' is not an integral type",
					stat->name);

		/* register this stat */
		pcstat_stats[i] = stat;
		pcstat_lastvals[i] = STATVAL(stat);

		/* declare the sparce text distribution */
		sprintf(buf, "%s_by_pc", stat->name);
		sprintf(buf1, "%s (by text address)", stat->desc);
		pcstat_sdists[i] = STATS->stat_reg_sdist(sdb, buf, buf1,
				/* initial value */0,
				/* print format */(PF_COUNT|PF_PDF),
				/* format */"0x%lx %lu %.2f",
				/* print fn */NULL);
	}
	LOADER->ld_reg_stats(sdb);
	MEMORY->mem_reg_stats(mem, sdb);
}



/* initialize the simulator */
void
sim::sim_init(void)
{
	sim_num_refs = 0;

	/* allocate and initialize register file */
	REGS->regs_init(&regs0);

	/* allocate and initialize memory space */
	mem = MEMORY->mem_create("mem");
	MEMORY->mem_init(mem);

	/* compute static power estimates */
	POWER->calculate_power();

}


/* load program into simulated state */
void
sim::sim_load_prog(char *fname,		/* program to load */
		int argc, char **argv,	/* program arguments */
		char **envp)		/* program environment */
{

	/* load program text and data, set up environment, memory, and regs */
	LOADER->ld_load_prog(fname, argc, argv, envp, &regs0, mem, TRUE);

	/* initialize here, so symbols can be loaded */
	if (ptrace_nelt == 2)
	{
		/* generate a pipeline trace */
		PTRACE->ptrace_open(/* fname */ptrace_opts[0], /* range */ptrace_opts[1]);
	}
	else if (ptrace_nelt == 0)
	{
		/* no pipetracing */;
	}
	else
		fatal("bad pipetrace args, use: <fname|stdout|stderr> <range>");

	/* finish initialization of the simulation engine */
	fu_pool = RESOURCE->res_create_pool("fu-pool", fu_config, N_ELT(fu_config));
	rslink_init(MAX_RS_LINKS);
	tracer_init();
	fetch_init();
	cv_init();
	eventq_init();
	readyq_init();
	ruu_init();
	lsq_init();
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim::sim_aux_stats(FILE *stream)             /* output stream */
{
	/* nada */
}

/* un-initialize the simulator */
void
sim::sim_uninit(void)
{
	if (ptrace_nelt > 0)
		PTRACE->ptrace_close();
}



/* allocate and initialize register update unit (RUU) */
/*static*/ void
sim::ruu_init(void)
{
	RUU = (RUU_station*) calloc(RUU_size, sizeof(struct RUU_station));
	if (!RUU)
		fatal("out of virtual memory");

	RUU_num = 0;
	RUU_head = RUU_tail = 0;
	RUU_count = 0;
	RUU_fcount = 0;
}

/* dump the contents of the RUU */
/*static*/ void
sim::ruu_dumpent(struct RUU_station *rs,		/* ptr to RUU station */
		int index,				/* entry index */
		FILE *stream,			/* output stream */
		int header)				/* print header? */
{
	if (!stream)
		stream = stderr;

	if (header)
		fprintf(stream, "idx: %2d: opcode: %s, inst: `",
				index, MD_OP_NAME(rs->op));
	else
		fprintf(stream, "       opcode: %s, inst: `",
				MD_OP_NAME(rs->op));
	MACHINE->md_print_insn(rs->IR, rs->PC, stream);
	fprintf(stream, "'\n");
	MISC->myfprintf(stream, "         PC: 0x%08p, NPC: 0x%08p (pred_PC: 0x%08p)\n",
			rs->PC, rs->next_PC, rs->pred_PC);
	fprintf(stream, "         in_LSQ: %s, ea_comp: %s, recover_inst: %s\n",
			rs->in_LSQ ? "t" : "f",
					rs->ea_comp ? "t" : "f",
							rs->recover_inst ? "t" : "f");
	MISC->myfprintf(stream, "         spec_mode: %s, addr: 0x%08p, tag: 0x%08x\n",
			rs->spec_mode ? "t" : "f", rs->addr, rs->tag);
	fprintf(stream, "         seq: 0x%08x, ptrace_seq: 0x%08x\n",
			rs->seq, rs->ptrace_seq);
	fprintf(stream, "         queued: %s, issued: %s, completed: %s\n",
			rs->queued ? "t" : "f",
					rs->issued ? "t" : "f",
							rs->completed ? "t" : "f");
	fprintf(stream, "         operands ready: %s\n",
			OPERANDS_READY(rs) ? "t" : "f");
}

/* dump the contents of the RUU */
/*static*/ void
sim::ruu_dump(FILE *stream)				/* output stream */
{
	int num, head;
	struct RUU_station *rs;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** RUU state **\n");
	fprintf(stream, "RUU_head: %d, RUU_tail: %d\n", RUU_head, RUU_tail);
	fprintf(stream, "RUU_num: %d\n", RUU_num);

	num = RUU_num;
	head = RUU_head;
	while (num)
	{
		rs = &RUU[head];
		ruu_dumpent(rs, rs - RUU, stream, /* header */TRUE);
		head = (head + 1) % RUU_size;
		num--;
	}
}

/*
 * load/store queue (LSQ): holds loads and stores in program order, indicating
 * status of load/store access:
 *
 *   - issued: address computation complete, memory access in progress
 *   - completed: memory access has completed, stored value available
 *   - squashed: memory access was squashed, ignore this entry
 *
 * loads may execute when:
 *   1) register operands are ready, and
 *   2) memory operands are ready (no earlier unresolved store)
 *
 * loads are serviced by:
 *   1) previous store at same address in LSQ (hit latency), or
 *   2) data cache (hit latency + miss latency)
 *
 * stores may execute when:
 *   1) register operands are ready
 *
 * stores are serviced by:
 *   1) depositing store value into the load/store queue
 *   2) writing store value to the store buffer (plus tag check) at commit
 *   3) writing store buffer entry to data cache when cache is free
 *
 * NOTE: the load/store queue can bypass a store value to a load in the same
 *   cycle the store executes (using a bypass network), thus stores complete
 *   in effective zero time after their effective address is known
 */



/* allocate and initialize the load/store queue (LSQ) */
/*static*/ void
sim::lsq_init(void)
{
	LSQ = (RUU_station*) calloc(LSQ_size, sizeof(struct RUU_station));
	if (!LSQ)
		fatal("out of virtual memory");

	LSQ_num = 0;
	LSQ_head = LSQ_tail = 0;
	LSQ_count = 0;
	LSQ_fcount = 0;
}

/* dump the contents of the RUU */
/*static*/ void
sim::lsq_dump(FILE *stream)				/* output stream */
{
	int num, head;
	struct RUU_station *rs;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** LSQ state **\n");
	fprintf(stream, "LSQ_head: %d, LSQ_tail: %d\n", LSQ_head, LSQ_tail);
	fprintf(stream, "LSQ_num: %d\n", LSQ_num);

	num = LSQ_num;
	head = LSQ_head;
	while (num)
	{
		rs = &LSQ[head];
		ruu_dumpent(rs, rs - LSQ, stream, /* header */TRUE);
		head = (head + 1) % LSQ_size;
		num--;
	}
}



/* initialize the free RS_LINK pool */
/*static*/ void
sim::rslink_init(int nlinks)			/* total number of RS_LINK available */
{
	int i;
	struct RS_link *link;

	rslink_free_list = NULL;
	for (i=0; i<nlinks; i++)
	{
		link = (RS_link*) calloc(1, sizeof(struct RS_link));
		if (!link)
			fatal("out of virtual memory");
		link->next = rslink_free_list;
		rslink_free_list = link;
	}
}

/* service all functional unit release events, this function is called
   once per cycle, and it used to step the BUSY timers attached to each
   functional unit in the function unit resource pool, as long as a functional
   unit's BUSY count is > 0, it cannot be issued an operation */
/*static*/ void
sim::ruu_release_fu(void)
{
	int i;

	/* walk all resource units, decrement busy counts by one */
	for (i=0; i<fu_pool->num_resources; i++)
	{
		/* resource is released when BUSY hits zero */
		if (fu_pool->resources[i].busy > 0)
			fu_pool->resources[i].busy--;
	}
}


/*
 * the execution unit event queue implementation follows, the event queue
 * indicates which instruction will complete next, the writeback handler
 * drains this queue
 */



/* initialize the event queue structures */
/*static*/ void
sim::eventq_init(void)
{
	event_queue = NULL;
}

/* dump the contents of the event queue */
/*static*/ void
sim::eventq_dump(FILE *stream)			/* output stream */
{
	struct RS_link *ev;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** event queue state **\n");

	for (ev = event_queue; ev != NULL; ev = ev->next)
	{
		/* is event still valid? */
		if (RSLINK_VALID(ev))
		{
			struct RUU_station *rs = RSLINK_RS(ev);

			fprintf(stream, "idx: %2d: @ %.0f\n",
					(int)(rs - (rs->in_LSQ ? LSQ : RUU)), (double)ev->x.when);
			ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU),
					stream, /* !header */FALSE);
		}
	}
}

/* insert an event for RS into the event queue, event queue is sorted from
   earliest to latest event, event and associated side-effects will be
   apparent at the start of cycle WHEN */
/*static*/ void
sim::eventq_queue_event(struct RUU_station *rs, tick_t when)
{
	struct RS_link *prev, *ev, *new_ev;

	if (rs->completed)
		panic("event completed");

	if (when <= sim_cycle)
		panic("event occurred in the past");

	/* get a free event record */
	RSLINK_NEW(new_ev, rs);
	new_ev->x.when = when;

	/* locate insertion point */
	for (prev=NULL, ev=event_queue;
	ev && ev->x.when < when;
	prev=ev, ev=ev->next);

	if (prev)
	{
		/* insert middle or end */
		new_ev->next = prev->next;
		prev->next = new_ev;
	}
	else
	{
		/* insert at beginning */
		new_ev->next = event_queue;
		event_queue = new_ev;
	}
}

/* return the next event that has already occurred, returns NULL when no
   remaining events or all remaining events are in the future */
/*static*/ struct RUU_station *
sim::eventq_next_event(void)
{
	struct RS_link *ev;

	if (event_queue && event_queue->x.when <= sim_cycle)
	{
		/* unlink and return first event on priority list */
		ev = event_queue;
		event_queue = event_queue->next;

		/* event still valid? */
		if (RSLINK_VALID(ev))
		{
			struct RUU_station *rs = RSLINK_RS(ev);

			/* reclaim event record */
			RSLINK_FREE(ev);

			/* event is valid, return resv station */
			return rs;
		}
		else
		{
			/* reclaim event record */
			RSLINK_FREE(ev);

			/* receiving inst was squashed, return next event */
			return eventq_next_event();
		}
	}
	else
	{
		/* no event or no event is ready */
		return NULL;
	}
}


/*
 * the ready instruction queue implementation follows, the ready instruction
 * queue indicates which instruction have all of there *register* dependencies
 * satisfied, instruction will issue when 1) all memory dependencies for
 * the instruction have been satisfied (see lsq_refresh() for details on how
 * this is accomplished) and 2) resources are available; ready queue is fully
 * constructed each cycle before any operation is issued from it -- this
 * ensures that instruction issue priorities are properly observed; NOTE:
 * RS_LINK nodes are used for the event queue list so that it need not be
 * updated during squash events
 */



/* initialize the event queue structures */
/*static*/ void
sim::readyq_init(void)
{
	ready_queue = NULL;
}

/* dump the contents of the ready queue */
/*static*/ void
sim::readyq_dump(FILE *stream)			/* output stream */
{
	struct RS_link *link;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** ready queue state **\n");

	for (link = ready_queue; link != NULL; link = link->next)
	{
		/* is entry still valid? */
		if (RSLINK_VALID(link))
		{
			struct RUU_station *rs = RSLINK_RS(link);

			ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU),
					stream, /* header */TRUE);
		}
	}
}

/* insert ready node into the ready list using ready instruction scheduling
   policy; currently the following scheduling policy is enforced:

     memory and long latency operands, and branch instructions first

   then

     all other instructions, oldest instructions first

  this policy works well because branches pass through the machine quicker
  which works to reduce branch misprediction latencies, and very long latency
  instructions (such loads and multiplies) get priority since they are very
  likely on the program's critical path */
/*static*/ void
sim::readyq_enqueue(struct RUU_station *rs)		/* RS to enqueue */
{
	struct RS_link *prev, *node, *new_node;

	/* node is now queued */
	if (rs->queued)
		panic("node is already queued");
	rs->queued = TRUE;

	/* get a free ready list node */
	RSLINK_NEW(new_node, rs);
	new_node->x.seq = rs->seq;

	/* locate insertion point */
	if (rs->in_LSQ || MD_OP_FLAGS(rs->op) & (F_LONGLAT|F_CTRL))
	{
		/* insert loads/stores and long latency ops at the head of the queue */
		prev = NULL;
		node = ready_queue;
	}
	else
	{
		/* otherwise insert in program order (earliest seq first) */
		for (prev=NULL, node=ready_queue;
		node && node->x.seq < rs->seq;
		prev=node, node=node->next);
	}

	if (prev)
	{
		/* insert middle or end */
		new_node->next = prev->next;
		prev->next = new_node;
	}
	else
	{
		/* insert at beginning */
		new_node->next = ready_queue;
		ready_queue = new_node;
	}
}


/*
 * the create vector maps a logical register to a creator in the RUU (and
 * specific output operand) or the architected register file (if RS_link
 * is NULL)
 */


/* initialize the create vector */
/*static*/ void
sim::cv_init(void)
{
	int i;

	/* initially all registers are valid in the architected register file,
     i.e., the create vector entry is CVLINK_NULL */
	for (i=0; i < MD_TOTAL_REGS; i++)
	{
		create_vector[i] = CVLINK_NULL;
		create_vector_rt[i] = 0;
		spec_create_vector[i] = CVLINK_NULL;
		spec_create_vector_rt[i] = 0;
	}

	/* all create vector entries are non-speculative */
	BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);
}

/* dump the contents of the create vector */
/*static*/ void
sim::cv_dump(FILE *stream)				/* output stream */
{
	int i;
	struct CV_link ent;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** create vector state **\n");

	for (i=0; i < MD_TOTAL_REGS; i++)
	{
		ent = CREATE_VECTOR(i);
		if (!ent.rs)
			fprintf(stream, "[cv%02d]: from architected reg file\n", i);
		else
			fprintf(stream, "[cv%02d]: from %s, idx: %d\n",
					i, (ent.rs->in_LSQ ? "LSQ" : "RUU"),
					(int)(ent.rs - (ent.rs->in_LSQ ? LSQ : RUU)));
	}
}


/*
 *  RUU_COMMIT() - instruction retirement pipeline stage
 */

/* this function commits the results of the oldest completed entries from the
   RUU and LSQ to the architected reg file, stores in the LSQ will commit
   their store data to the data cache at this point as well */
/*static*/ void
sim::ruu_commit(void)
{


	int i, lat, events, committed = 0;

	/* all values must be retired to the architected reg file in program order */
	while (RUU_num > 0 && committed < ruu_commit_width)
	{
		struct RUU_station *rs = &(RUU[RUU_head]);

		if (!rs->completed)
		{
			/* at least RUU entry must be complete */
			break;
		}

		/* default commit events */
		events = 0;

		/* load/stores must retire load/store queue entry as well */
		if (RUU[RUU_head].ea_comp)
		{
			/* load/store, retire head of LSQ as well */
			if (LSQ_num <= 0 || !LSQ[LSQ_head].in_LSQ)
				panic("RUU out of sync with LSQ");

			/* load/store operation must be complete */
			if (!LSQ[LSQ_head].completed)
			{
				/* load/store operation is not yet complete */
				break;
			}

			if ((MD_OP_FLAGS(LSQ[LSQ_head].op) & (F_MEM|F_STORE))
					== (F_MEM|F_STORE))
			{
				struct res_template *fu;


				/* stores must retire their store value to the cache at commit,
		 try to get a store port (functional unit allocation) */
				fu = RESOURCE->res_get(fu_pool, MD_OP_FUCLASS(LSQ[LSQ_head].op));
				if (fu)
				{
					/* reserve the functional unit */
					if (fu->master->busy)
						panic("functional unit already in use");

					/* schedule functional unit release event */
					fu->master->busy = fu->issuelat;

#if 0
					TESTEMAX
					if ((LSQ[LSQ_head].addr&~3) == 0x1500){
						printf("RUU_COMMIT = %d\n", 0x1500);
						//int z;
						//for (z=0;z<32;z++)
						//	printf(":%d\n", regs0.regs_R[z]);
						printf ("dado: %d\n", regs0.regs_R[3]);
					}
#endif
					/* go to the data cache */
					if (cache_dl1)
					{
						/* Wattch -- D-cache access */
						dcache_access++;

						//printf(">>>>>>> ruu_commit %d\n", LSQ[LSQ_head].addr&~3);

						/* commit store value to D-cache */
						lat =
							CACHE->cache_access(cache_dl1, Write, (LSQ[LSQ_head].addr&~3),
									NULL, 4, sim_cycle, NULL, NULL);

						if (lat > cache_dl1_lat)
							events |= PEV_CACHEMISS;
					}

					/* all loads and stores must to access D-TLB */
					if (dtlb)
					{
						/* access the D-TLB */
						lat =
							CACHE->cache_access(dtlb, Read, (LSQ[LSQ_head].addr & ~3),
									NULL, 4, sim_cycle, NULL, NULL);

						if (lat > 1)
							events |= PEV_TLBMISS;
					}
				}
				else
				{
					/* no store ports left, cannot continue to commit insts */
					break;
				}
			}

			/* invalidate load/store operation instance */
			LSQ[LSQ_head].tag++;

			/* indicate to pipeline trace that this instruction retired */
			PTRACE->ptrace_newstage(LSQ[LSQ_head].ptrace_seq, PST_COMMIT, events);
			PTRACE->ptrace_endinst(LSQ[LSQ_head].ptrace_seq);

			/* commit head of LSQ as well */
			LSQ_head = (LSQ_head + 1) % LSQ_size;
			LSQ_num--;
		}

		/* Wattch -- committed instruction to arch reg file */
		if ((MD_OP_FLAGS(rs->op) & (F_ICOMP|F_FCOMP)) || ((MD_OP_FLAGS(rs->op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD))) {
			regfile_access++;
#ifdef DYNAMIC_AF
			regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rc);
			regfile_num_pop_count_cycle++;
#endif
		}

		if (pred
				&& bpred_spec_update == spec_CT
				&& (MD_OP_FLAGS(rs->op) & F_CTRL))
		{
			/* Wattch -- bpred access */
			bpred_access++;

			BPRED->bpred_update(pred,
					/* branch address */rs->PC,
					/* actual target address */rs->next_PC,
					/* taken? */rs->next_PC != (rs->PC +
							sizeof(md_inst_t)),
							/* pred taken? */rs->pred_PC != (rs->PC +
									sizeof(md_inst_t)),
									/* correct pred? */rs->pred_PC == rs->next_PC,
									/* opcode */rs->op,
									/* dir predictor update pointer */&rs->dir_update);
		}

		/* invalidate RUU operation instance */
		RUU[RUU_head].tag++;

		/* indicate to pipeline trace that this instruction retired */
		PTRACE->ptrace_newstage(RUU[RUU_head].ptrace_seq, PST_COMMIT, events);
		PTRACE->ptrace_endinst(RUU[RUU_head].ptrace_seq);

		/* commit head entry of RUU */
		RUU_head = (RUU_head + 1) % RUU_size;
		RUU_num--;

		/* one more instruction committed to architected state */
		committed++;

		for (i=0; i<MAX_ODEPS; i++)
		{
			if (rs->odep_list[i])
				panic ("retired instruction has odeps\n");
		}
	}
}


/*
 *  RUU_RECOVER() - squash mispredicted microarchitecture state
 */

/* recover processor microarchitecture state back to point of the
   mis-predicted branch at RUU[BRANCH_INDEX] */
/*static*/ void
sim::ruu_recover(int branch_index)			/* index of mis-pred branch */
{
	int i, RUU_index = RUU_tail, LSQ_index = LSQ_tail;
	int RUU_prev_tail = RUU_tail, LSQ_prev_tail = LSQ_tail;

	/* recover from the tail of the RUU towards the head until the branch index
     is reached, this direction ensures that the LSQ can be synchronized with
     the RUU */

	/* go to first element to squash */
	RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
	LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;

	/* traverse to older insts until the mispredicted branch is encountered */
	while (RUU_index != branch_index)
	{
		/* the RUU should not drain since the mispredicted branch will remain */
		if (!RUU_num)
			panic("empty RUU");

		/* should meet up with the tail first */
		if (RUU_index == RUU_head)
			panic("RUU head and tail broken");

		/* is this operation an effective addr calc for a load or store? */
		if (RUU[RUU_index].ea_comp)
		{
			/* should be at least one load or store in the LSQ */
			if (!LSQ_num)
				panic("RUU and LSQ out of sync");

			/* recover any resources consumed by the load or store operation */
			for (i=0; i<MAX_ODEPS; i++)
			{
				RSLINK_FREE_LIST(LSQ[LSQ_index].odep_list[i]);
				/* blow away the consuming op list */
				LSQ[LSQ_index].odep_list[i] = NULL;
			}

			/* squash this LSQ entry */
			LSQ[LSQ_index].tag++;

			/* indicate in pipetrace that this instruction was squashed */
			PTRACE->ptrace_endinst(LSQ[LSQ_index].ptrace_seq);

			/* go to next earlier LSQ slot */
			LSQ_prev_tail = LSQ_index;
			LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
			LSQ_num--;
		}

		/* recover any resources used by this RUU operation */
		for (i=0; i<MAX_ODEPS; i++)
		{
			RSLINK_FREE_LIST(RUU[RUU_index].odep_list[i]);
			/* blow away the consuming op list */
			RUU[RUU_index].odep_list[i] = NULL;
		}

		/* squash this RUU entry */
		RUU[RUU_index].tag++;

		/* indicate in pipetrace that this instruction was squashed */
		PTRACE->ptrace_endinst(RUU[RUU_index].ptrace_seq);

		/* go to next earlier slot in the RUU */
		RUU_prev_tail = RUU_index;
		RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
		RUU_num--;
	}

	/* reset head/tail pointers to point to the mis-predicted branch */
	RUU_tail = RUU_prev_tail;
	LSQ_tail = LSQ_prev_tail;

	/* revert create vector back to last precise create vector state, NOTE:
     this is accomplished by resetting all the copied-on-write bits in the
     USE_SPEC_CV bit vector */
	BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);

	/* FIXME: could reset functional units at squash time */
}


/*
 *  RUU_WRITEBACK() - instruction result writeback pipeline stage
 */

/* writeback completed operation results from the functional units to RUU,
   at this point, the output dependency chains of completing instructions
   are also walked to determine if any dependent instruction now has all
   of its register operands, if so the (nearly) ready instruction is inserted
   into the ready instruction queue */
/*static*/ void
sim::ruu_writeback(void)
{
	int i;
	struct RUU_station *rs;

	/* service all completed events */
	while ((rs = eventq_next_event()))
	{
		/* RS has completed execution and (possibly) produced a result */
		if (!OPERANDS_READY(rs) || rs->queued || !rs->issued || rs->completed)
			panic("inst completed and !ready, !issued, or completed");

		/* operation has completed */
		rs->completed = TRUE;

		/* Wattch -- 1) Writeback result to resultbus
                   2) Write result to phys. regs (RUU)
		   3) Access wakeup logic
		 */
		if(!(MD_OP_FLAGS(rs->op) & F_CTRL)) {
			window_access++;
			window_preg_access++;
			window_wakeup_access++;
			resultbus_access++;
#ifdef DYNAMIC_AF
			window_total_pop_count_cycle += POWER->pop_count(rs->val_rc);
			window_num_pop_count_cycle++;
			resultbus_total_pop_count_cycle += POWER->pop_count(rs->val_rc);
			resultbus_num_pop_count_cycle++;
#endif
		}

		/* does this operation reveal a mis-predicted branch? */
		if (rs->recover_inst)
		{
			if (rs->in_LSQ)
				panic("mis-predicted load or store?!?!?");

			/* recover processor state and reinit fetch to correct path */
			ruu_recover(rs - RUU);
			tracer_recover();
			BPRED->bpred_recover(pred, rs->PC, rs->stack_recover_idx);

			/* stall fetch until I-fetch and I-decode recover */
			ruu_fetch_issue_delay = ruu_branch_penalty;

			/* continue writeback of the branch/control instruction */
		}

		/* if we speculatively update branch-predictor, do it here */
		if (pred
				&& bpred_spec_update == spec_WB
				&& !rs->in_LSQ
				&& (MD_OP_FLAGS(rs->op) & F_CTRL))
		{
			/* Wattch -- bpred access */
			bpred_access++;
			BPRED->bpred_update(pred,
					/* branch address */rs->PC,
					/* actual target address */rs->next_PC,
					/* taken? */rs->next_PC != (rs->PC +
							sizeof(md_inst_t)),
							/* pred taken? */rs->pred_PC != (rs->PC +
									sizeof(md_inst_t)),
									/* correct pred? */rs->pred_PC == rs->next_PC,
									/* opcode */rs->op,
									/* dir predictor update pointer */&rs->dir_update);
		}

		/* entered writeback stage, indicate in pipe trace */
		PTRACE->ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK,
				rs->recover_inst ? PEV_MPDETECT : 0);

		/* broadcast results to consuming operations, this is more efficiently
         accomplished by walking the output dependency chains of the
	 completed instruction */
		for (i=0; i<MAX_ODEPS; i++)
		{
			if (rs->onames[i] != NA)
			{
				struct CV_link link;
				struct RS_link *olink, *olink_next;

				if (rs->spec_mode)
				{
					/* update the speculative create vector, future operations
		     get value from later creator or architected reg file */
					link = spec_create_vector[rs->onames[i]];
					if (/* !NULL */link.rs
							&& /* refs RS */(link.rs == rs && link.odep_num == i))
					{
						/* the result can now be read from a physical register,
			 indicate this as so */
						spec_create_vector[rs->onames[i]] = CVLINK_NULL;
						spec_create_vector_rt[rs->onames[i]] = sim_cycle;
					}
					/* else, creator invalidated or there is another creator */
				}
				else
				{
					/* update the non-speculative create vector, future
		     operations get value from later creator or architected
		     reg file */
					link = create_vector[rs->onames[i]];
					if (/* !NULL */link.rs
							&& /* refs RS */(link.rs == rs && link.odep_num == i))
					{
						/* the result can now be read from a physical register,
			 indicate this as so */
						create_vector[rs->onames[i]] = CVLINK_NULL;
						create_vector_rt[rs->onames[i]] = sim_cycle;
					}
					/* else, creator invalidated or there is another creator */
				}

				/* walk output list, queue up ready operations */
				for (olink=rs->odep_list[i]; olink; olink=olink_next)
				{
					if (RSLINK_VALID(olink))
					{
						if (olink->rs->idep_ready[olink->x.opnum])
							panic("output dependence already satisfied");

						/* input is now ready */
						olink->rs->idep_ready[olink->x.opnum] = TRUE;

						/* are all the register operands of target ready? */
						if (OPERANDS_READY(olink->rs))
						{
							/* yes! enqueue instruction as ready, NOTE: stores
			     complete at dispatch, so no need to enqueue
			     them */
							if (!olink->rs->in_LSQ
									|| ((MD_OP_FLAGS(olink->rs->op)&(F_MEM|F_STORE))
											== (F_MEM|F_STORE)))
								readyq_enqueue(olink->rs);
							/* else, ld op, issued when no mem conflict */
						}
					}

					/* grab link to next element prior to free */
					olink_next = olink->next;

					/* free dependence link element */
					RSLINK_FREE(olink);
				}
				/* blow away the consuming op list */
				rs->odep_list[i] = NULL;

			} /* if not NA output */

		} /* for all outputs */

	} /* for all writeback events */

}


/*
 *  LSQ_REFRESH() - memory access dependence checker/scheduler
 */

/* this function locates ready instructions whose memory dependencies have
   been satisfied, this is accomplished by walking the LSQ for loads, looking
   for blocking memory dependency condition (e.g., earlier store with an
   unknown address) */
#define MAX_STD_UNKNOWNS		64
/*static*/ void
sim::lsq_refresh(void)
{
	int i, j, index, n_std_unknowns;
	md_addr_t std_unknowns[MAX_STD_UNKNOWNS];

	/* scan entire queue for ready loads: scan from oldest instruction
     (head) until we reach the tail or an unresolved store, after which no
     other instruction will become ready */
	for (i=0, index=LSQ_head, n_std_unknowns=0;
	i < LSQ_num;
	i++, index=(index + 1) % LSQ_size)
	{
		/* terminate search for ready loads after first unresolved store,
	 as no later load could be resolved in its presence */
		if (/* store? */
				(MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
		{
			if (!STORE_ADDR_READY(&LSQ[index]))
			{
				/* FIXME: a later STD + STD known could hide the STA unknown */
				/* sta unknown, blocks all later loads, stop search */
				break;
			}
			else if (!OPERANDS_READY(&LSQ[index]))
			{
				/* sta known, but std unknown, may block a later store, record
		 this address for later referral, we use an array here because
		 for most simulations the number of entries to search will be
		 very small */
				if (n_std_unknowns == MAX_STD_UNKNOWNS)
					fatal("STD unknown array overflow, increase MAX_STD_UNKNOWNS");
				std_unknowns[n_std_unknowns++] = LSQ[index].addr;
			}
			else /* STORE_ADDR_READY() && OPERANDS_READY() */
			{
				/* a later STD known hides an earlier STD unknown */
				for (j=0; j<n_std_unknowns; j++)
				{
					if (std_unknowns[j] == /* STA/STD known */LSQ[index].addr)
						std_unknowns[j] = /* bogus addr */0;
				}
			}
		}

		if (/* load? */
				((MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD))
				&& /* queued? */!LSQ[index].queued
				&& /* waiting? */!LSQ[index].issued
				&& /* completed? */!LSQ[index].completed
				&& /* regs ready? */OPERANDS_READY(&LSQ[index]))
		{
			/* no STA unknown conflict (because we got to this check), check for
	     a STD unknown conflict */
			for (j=0; j<n_std_unknowns; j++)
			{
				/* found a relevant STD unknown? */
				if (std_unknowns[j] == LSQ[index].addr)
					break;
			}
			if (j == n_std_unknowns)
			{
				/* no STA or STD unknown conflicts, put load on ready queue */
				readyq_enqueue(&LSQ[index]);
			}
		}
	}
}


/*
 *  RUU_ISSUE() - issue instructions to functional units
 */

/* attempt to issue all operations in the ready queue; insts in the ready
   instruction queue have all register dependencies satisfied, this function
   must then 1) ensure the instructions memory dependencies have been satisfied
   (see lsq_refresh() for details on this process) and 2) a function unit
   is available in this cycle to commence execution of the operation; if all
   goes well, the function unit is allocated, a writeback event is scheduled,
   and the instruction begins execution */
/*static*/ void
sim::ruu_issue(void)
{
	int i, load_lat, tlb_lat, n_issued;
	struct RS_link *node, *next_node;
	struct res_template *fu;

	/* FIXME: could be a little more efficient when scanning the ready queue */

	/* copy and then blow away the ready list, NOTE: the ready list is
     always totally reclaimed each cycle, and instructions that are not
     issue are explicitly reinserted into the ready instruction queue,
     this management strategy ensures that the ready instruction queue
     is always properly sorted */
	node = ready_queue;
	ready_queue = NULL;

	/* visit all ready instructions (i.e., insts whose register input
     dependencies have been satisfied, stop issue when no more instructions
     are available or issue bandwidth is exhausted */
	for (n_issued=0;
	node && n_issued < ruu_issue_width;
	node = next_node)
	{
		next_node = node->next;

		/* still valid? */
		if (RSLINK_VALID(node))
		{
			struct RUU_station *rs = RSLINK_RS(node);

			/* issue operation, both reg and mem deps have been satisfied */
			if (!OPERANDS_READY(rs) || !rs->queued
					|| rs->issued || rs->completed)
				panic("issued inst !ready, issued, or completed");

			/* Wattch -- access window selection logic */
			window_selection_access++;

			/* node is now un-queued */
			rs->queued = FALSE;

			if (rs->in_LSQ
					&& ((MD_OP_FLAGS(rs->op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE)))
			{
				/* stores complete in effectively zero time, result is
		 written into the load/store queue, the actual store into
		 the memory system occurs when the instruction is retired
		 (see ruu_commit()) */
				rs->issued = TRUE;
				rs->completed = TRUE;

				if (rs->onames[0] || rs->onames[1])
					panic("store creates result");

				if (rs->recover_inst)
					panic("mis-predicted store");

				/* entered execute stage, indicate in pipe trace */
				PTRACE->ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK, 0);

				/* one more inst issued */
				n_issued++;

				/* Wattch -- LSQ access -- write data into store buffer */
				lsq_access++;
				lsq_store_data_access++;
				lsq_preg_access++;
#ifdef DYNAMIC_AF
				lsq_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
				lsq_num_pop_count_cycle++;
#endif
			}
			else
			{
				/* issue the instruction to a functional unit */
				if (MD_OP_FUCLASS(rs->op) != NA)
				{
					fu = RESOURCE->res_get(fu_pool, MD_OP_FUCLASS(rs->op));
					if (fu)
					{
						/* got one! issue inst to functional unit */
						rs->issued = TRUE;

						/* reserve the functional unit */
						if (fu->master->busy)
							panic("functional unit already in use");

						/* schedule functional unit release event */
						fu->master->busy = fu->issuelat;

						/* schedule a result writeback event */
						if (rs->in_LSQ
								&& ((MD_OP_FLAGS(rs->op) & (F_MEM|F_LOAD))
										== (F_MEM|F_LOAD)))
						{
							int events = 0;

							/* Wattch -- LSQ access */
							lsq_access++;
							lsq_wakeup_access++;

							/* for loads, determine cache access latency:
			     first scan LSQ to see if a store forward is
			     possible, if not, access the data cache */
							load_lat = 0;
							i = (rs - LSQ);
							if (i != LSQ_head)
							{
								for (;;)
								{
									/* go to next earlier LSQ entry */
									i = (i + (LSQ_size-1)) % LSQ_size;

									/* FIXME: not dealing with partials! */
									if ((MD_OP_FLAGS(LSQ[i].op) & F_STORE)
											&& (LSQ[i].addr == rs->addr))
									{
										/* hit in the LSQ */
										load_lat = 1;
										lsq_access++;
										lsq_preg_access++;
										lsq_load_data_access++;
#ifdef DYNAMIC_AF
										lsq_total_pop_count_cycle += POWER->pop_count(rs->val_ra_result);
										lsq_num_pop_count_cycle++;
#endif
										break;
									}

									/* scan finished? */
									if (i == LSQ_head)
										break;
								}
							}

							/* was the value store forwared from the LSQ? */
							if (!load_lat)
							{
								int valid_addr = MACHINE->MD_VALID_ADDR(rs->addr);

								if (!spec_mode && !valid_addr)
									sim_invalid_addrs++;

								/* no! go to the data cache if addr is valid */
								if (cache_dl1 && valid_addr)
								{
									/* Wattch -- D-cache access */
									dcache_access++;
									/* access the cache if non-faulting */

									/* MSG
				if ((rs->addr&~3) == 0x1500)
					printf ("RUU_ISSUE  = %d\n", 0x1500);
									 */
									load_lat =
										CACHE->cache_access(cache_dl1, Read,
												(rs->addr & ~3), NULL, 4,
												sim_cycle, NULL, NULL);

									if (load_lat > cache_dl1_lat)
										events |= PEV_CACHEMISS;
								}
								else
								{
									/* no caches defined, just use op latency */
									load_lat = fu->oplat;
								}
							}

							/* all loads and stores must to access D-TLB */
							if (dtlb && MACHINE->MD_VALID_ADDR(rs->addr))
							{
								/* access the D-DLB, NOTE: this code will
				 initiate speculative TLB misses */
								tlb_lat =
									CACHE->cache_access(dtlb, Read, (rs->addr & ~3),
											NULL, 4, sim_cycle, NULL, NULL);

								if (tlb_lat > 1)
									events |= PEV_TLBMISS;

								/* D-cache/D-TLB accesses occur in parallel */
								load_lat = MAX(tlb_lat, load_lat);
							}

							/* use computed cache access latency */
							eventq_queue_event(rs, sim_cycle + load_lat);

							/* entered execute stage, indicate in pipe trace */
							PTRACE->ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
									((rs->ea_comp ? PEV_AGEN : 0)
											| events));
						}
						else /* !load && !store */
						{
							/* Wattch -- ALU access Wattch-FIXME
			     (different op types)
			     also spread out power of multi-cycle ops
							 */
							alu_access++;

							if((MD_OP_FLAGS(rs->op) & (F_FCOMP))== (F_FCOMP))
								falu_access++;
							else
								ialu_access++;

							/* use deterministic functional unit latency */
							eventq_queue_event(rs, sim_cycle + fu->oplat);

							/* entered execute stage, indicate in pipe trace */
							PTRACE->ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
									rs->ea_comp ? PEV_AGEN : 0);
						}

						/* Wattch -- window access */
						window_access++;
						/* read values from window send to FUs */
						window_preg_access++;
						window_preg_access++;
#ifdef DYNAMIC_AF
						window_total_pop_count_cycle += POWER->pop_count(rs->val_ra) + POWER->pop_count(rs->val_rb);
						window_num_pop_count_cycle+=2;
#endif

						/* one more inst issued */
						n_issued++;
					}
					else /* no functional unit */
					{
						/* insufficient functional unit resources, put operation
			 back onto the ready list, we'll try to issue it
			 again next cycle */
						readyq_enqueue(rs);
					}
				}
				else /* does not require a functional unit! */
				{
					/* FIXME: need better solution for these */
					/* the instruction does not need a functional unit */
					rs->issued = TRUE;

					/* schedule a result event */
					eventq_queue_event(rs, sim_cycle + 1);

					/* entered execute stage, indicate in pipe trace */
					PTRACE->ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
							rs->ea_comp ? PEV_AGEN : 0);

					/* Wattch -- Window access */
					window_access++;
					/* read values from window send to FUs */
					window_preg_access++;
					window_preg_access++;
#ifdef DYNAMIC_AF
					window_total_pop_count_cycle += POWER->pop_count(rs->val_ra) + POWER->pop_count(rs->val_rb);
					window_num_pop_count_cycle+=2;
#endif

					/* one more inst issued */
					n_issued++;
				}
			} /* !store */

		}
		/* else, RUU entry was squashed */

		/* reclaim ready list entry, NOTE: this is done whether or not the
         instruction issued, since the instruction was once again reinserted
         into the ready queue if it did not issue, this ensures that the ready
         queue is always properly sorted */
		RSLINK_FREE(node);
	}

	/* put any instruction not issued back into the ready queue, go through
     normal channels to ensure instruction stay ordered correctly */
	for (; node; node = next_node)
	{
		next_node = node->next;

		/* still valid? */
		if (RSLINK_VALID(node))
		{
			struct RUU_station *rs = RSLINK_RS(node);

			/* node is now un-queued */
			rs->queued = FALSE;

			/* not issued, put operation back onto the ready list, we'll try to
	     issue it again next cycle */
			readyq_enqueue(rs);
		}
		/* else, RUU entry was squashed */

		/* reclaim ready list entry, NOTE: this is done whether or not the
         instruction issued, since the instruction was once again reinserted
         into the ready queue if it did not issue, this ensures that the ready
         queue is always properly sorted */
		RSLINK_FREE(node);
	}
}


/*
 * routines for generating on-the-fly instruction traces with support
 * for control and data misspeculation modeling
 */




/* dump speculative register state */
/*static*/ void
sim::rspec_dump(FILE *stream)			/* output stream */
{
	int i;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** speculative register contents **\n");

	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

	/* dump speculative integer regs */
	for (i=0; i < MD_NUM_IREGS; i++)
	{
		if (BITMAP_SET_P(use_spec_R, R_BMAP_SZ, i))
		{
			MACHINE->md_print_ireg(spec_regs_R, i, stream);
			fprintf(stream, "\n");
		}
	}

	/* dump speculative FP regs */
	for (i=0; i < MD_NUM_FREGS; i++)
	{
		if (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, i))
		{
			MACHINE->md_print_fpreg(spec_regs_F, i, stream);
			fprintf(stream, "\n");
		}
	}

	/* dump speculative CTRL regs */
	for (i=0; i < MD_NUM_CREGS; i++)
	{
		if (BITMAP_SET_P(use_spec_C, C_BMAP_SZ, i))
		{
			MACHINE->md_print_creg(spec_regs_C, i, stream);
			fprintf(stream, "\n");
		}
	}
}



/* recover instruction trace generator state to precise state state immediately
   before the first mis-predicted branch; this is accomplished by resetting
   all register value copied-on-write bitmasks are reset, and the speculative
   memory hash table is cleared */
/*static*/ void
sim::tracer_recover(void)
{
	int i;
	struct spec_mem_ent *ent, *ent_next;

	/* better be in mis-speculative trace generation mode */
	if (!spec_mode)
		panic("cannot recover unless in speculative mode");

	/* reset to non-speculative trace generation mode */
	spec_mode = FALSE;

	/* reset copied-on-write register bitmasks back to non-speculative state */
	BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);

	/* reset memory state back to non-speculative state */
	/* FIXME: could version stamps be used here?!?!? */
	for (i=0; i<STORE_HASH_SIZE; i++)
	{
		/* release all hash table buckets */
		for (ent=store_htable[i]; ent; ent=ent_next)
		{
			ent_next = ent->next;
			ent->next = bucket_free_list;
			bucket_free_list = ent;
		}
		store_htable[i] = NULL;
	}

	/* if pipetracing, indicate squash of instructions in the inst fetch queue */
	if (PTRACE->ptrace_active)
	{
		while (fetch_num != 0)
		{
			/* squash the next instruction from the IFETCH -> DISPATCH queue */
			PTRACE->ptrace_endinst(fetch_data[fetch_head].ptrace_seq);

			/* consume instruction from IFETCH -> DISPATCH queue */
			fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
			fetch_num--;
		}
	}

	/* reset IFETCH state */
	fetch_num = 0;
	fetch_tail = fetch_head = 0;
	fetch_pred_PC = fetch_regs_PC = recover_PC;
}

/* initialize the speculative instruction state generator state */
/*static*/ void
sim::tracer_init(void)
{
	int i;

	/* initially in non-speculative mode */
	spec_mode = FALSE;

	/* register state is from non-speculative state buffers */
	BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);

	/* memory state is from non-speculative memory pages */
	for (i=0; i<STORE_HASH_SIZE; i++)
		store_htable[i] = NULL;
}


/* this functional provides a layer of mis-speculated state over the
   non-speculative memory state, when in mis-speculation trace generation mode,
   the simulator will call this function to access memory, instead of the
   non-speculative memory access interfaces defined in memory.h; when storage
   is written, an entry is allocated in the speculative memory hash table,
   future reads and writes while in mis-speculative trace generation mode will
   access this buffer instead of non-speculative memory state; when the trace
   generator transitions back to non-speculative trace generation mode,
   tracer_recover() clears this table, returns any access fault */
/*static*/ enum md_fault_type
sim::spec_mem_access(struct mem_t *mem,		/* memory space to access */
		enum mem_cmd cmd,		/* Read or Write access cmd */
		md_addr_t addr,			/* virtual address of access */
		void *p,			/* input/output buffer */
		int nbytes)			/* number of bytes to access */
{
	int i, index;
	struct spec_mem_ent *ent, *prev;

	/* FIXME: partially overlapping writes are not combined... */
	/* FIXME: partially overlapping reads are not handled correctly... */

	/* check alignments, even speculative this test should always pass */
	if ((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
	{
		/* no can do, return zero result */
		for (i=0; i < nbytes; i++)
			((char *)p)[i] = 0;

		return md_fault_none;
	}

	/* check permissions */
	if (!((addr >= LOADER->ld_text_base && addr < (LOADER->ld_text_base+LOADER->ld_text_size)
			&& cmd == Read)
			|| MACHINE->MD_VALID_ADDR(addr)))
	{
		/* no can do, return zero result */
		for (i=0; i < nbytes; i++)
			((char *)p)[i] = 0;

		return md_fault_none;
	}

	/* has this memory state been copied on mis-speculative write? */
	index = HASH_ADDR(addr);
	for (prev=NULL,ent=store_htable[index]; ent; prev=ent,ent=ent->next)
	{
		if (ent->addr == addr)
		{
			/* reorder chains to speed access into hash table */
			if (prev != NULL)
			{
				/* not at head of list, relink the hash table entry at front */
				prev->next = ent->next;
				ent->next = store_htable[index];
				store_htable[index] = ent;
			}
			break;
		}
	}

	/* no, if it is a write, allocate a hash table entry to hold the data */
	if (!ent && cmd == Write)
	{
		/* try to get an entry from the free list, if available */
		if (!bucket_free_list)
		{
			/* otherwise, call calloc() to get the needed storage */
			bucket_free_list = (spec_mem_ent*) calloc(1, sizeof(struct spec_mem_ent));
			if (!bucket_free_list)
				fatal("out of virtual memory");
		}
		ent = bucket_free_list;
		bucket_free_list = bucket_free_list->next;

		if (!bugcompat_mode)
		{
			/* insert into hash table */
			ent->next = store_htable[index];
			store_htable[index] = ent;
			ent->addr = addr;
			ent->data[0] = 0; ent->data[1] = 0;
		}
	}



	if (addr >=0x80000000)
	{
		printf("\n >>>>>>>>>> SPEC_MEM_SHARED - never tested! \n");
		exit(0);
		/* handle the read or write to speculative or non-speculative storage */
#if 0
		switch (nbytes)
		{
		case 1:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((byte_t *)p) = *((byte_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((byte_t *)p) = (byte_t) this->Simplescalar->mshared->spec_mem_read(addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((byte_t *)(&ent->data[0])) = *((byte_t *)p);
			}
		case 4:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *)p) = *((word_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((word_t *)p) = this->Simplescalar->mshared->spec_mem_read(addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *)(&ent->data[0])) = *((word_t *)p);
			}
		}
#endif
	}
	else

	{
		/* handle the read or write to speculative or non-speculative storage */
		switch (nbytes)
		{
		case 1:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((byte_t *)p) = *((byte_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((byte_t *)(&ent->data[0])) = *((byte_t *)p);
			}
			break;
		case 2:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((half_t *)p) = *((half_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((half_t *)p) = MEM_READ_HALF(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((half_t *)&ent->data[0]) = *((half_t *)p);
			}
			break;
		case 4:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *)p) = *((word_t *)&ent->data[0]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *)&ent->data[0]) = *((word_t *)p);
			}
			break;
		case 8:
			if (cmd == Read)
			{
				if (ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *)p) = *((word_t *)&ent->data[0]);
					*(((word_t *)p)+1) = *((word_t *)&ent->data[1]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate
					   memory pages with speculative loads */
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
					*(((word_t *)p)+1) =
						MEM_READ_WORD(mem, addr + sizeof(word_t));
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *)&ent->data[0]) = *((word_t *)p);
				*((word_t *)&ent->data[1]) = *(((word_t *)p)+1);
			}
			break;
		default:
			panic("access size not supported in mis-speculative mode");
		}
	}

	return md_fault_none;
}

/* dump speculative memory state */
/*static*/ void
sim::mspec_dump(FILE *stream)			/* output stream */
{
	int i;
	struct spec_mem_ent *ent;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** speculative memory contents **\n");

	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

	for (i=0; i<STORE_HASH_SIZE; i++)
	{
		/* dump contents of all hash table buckets */
		for (ent=store_htable[i]; ent; ent=ent->next)
		{
			MISC->myfprintf(stream, "[0x%08p]: %12.0f/0x%08x:%08x\n",
					ent->addr, (double)(*((double *)ent->data)),
					*((unsigned int *)&ent->data[0]),
					*(((unsigned int *)&ent->data[0]) + 1));
		}
	}
}

/*
 *  RUU_DISPATCH() - decode instructions and allocate RUU and LSQ resources
 */

/* link RS onto the output chain number of whichever operation will next
   create the architected register value IDEP_NAME */
/*static*/ INLINE void
sim::ruu_link_idep(struct RUU_station *rs,		/* rs station to link */
		int idep_num,			/* input dependence number */
		int idep_name)			/* input register name */
{
	struct CV_link head;
	struct RS_link *link;

	/* any dependence? */
	if (idep_name == NA)
	{
		/* no input dependence for this input slot, mark operand as ready */
		rs->idep_ready[idep_num] = TRUE;
		return;
	}

	/* locate creator of operand */
	head = CREATE_VECTOR(idep_name);

	/* any creator? */
	if (!head.rs)
	{
		/* no active creator, use value available in architected reg file,
         indicate the operand is ready for use */
		rs->idep_ready[idep_num] = TRUE;

		/* Wattch -- regfile access (value from arch regfile) */
		regfile_access++;
#ifdef DYNAMIC_AF
		if(idep_num == 0)
			regfile_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
		else
			regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rb);
		regfile_num_pop_count_cycle++;
#endif

		return;
	}
	/* else, creator operation will make this value sometime in the future */

	/* indicate value will be created sometime in the future, i.e., operand
     is not yet ready for use */
	rs->idep_ready[idep_num] = FALSE;

	/* link onto creator's output list of dependant operand */
	RSLINK_NEW(link, rs); link->x.opnum = idep_num;
	link->next = head.rs->odep_list[head.odep_num];
	head.rs->odep_list[head.odep_num] = link;
}

/* make RS the creator of architected register ODEP_NAME */
/*static*/ INLINE void
sim::ruu_install_odep(struct RUU_station *rs,	/* creating RUU station */
		int odep_num,			/* output operand number */
		int odep_name)			/* output register name */
{
	struct CV_link cv;

	/* any dependence? */
	if (odep_name == NA)
	{
		/* no value created */
		rs->onames[odep_num] = NA;
		return;
	}
	/* else, create a RS_NULL terminated output chain in create vector */

	/* record output name, used to update create vector at completion */
	rs->onames[odep_num] = odep_name;

	/* initialize output chain to empty list */
	rs->odep_list[odep_num] = NULL;

	/* indicate this operation is latest creator of ODEP_NAME */
	CVLINK_INIT(cv, rs, odep_num);
	SET_CREATE_VECTOR(odep_name, cv);
}

/* dispatch instructions from the IFETCH -> DISPATCH queue: instructions are
   first decoded, then they allocated RUU (and LSQ for load/stores) resources
   and input and output dependence chains are updated accordingly */
/*static*/ void
sim::ruu_dispatch(void)
{
	int i;
	int n_dispatched;			/* total insts dispatched */
	md_inst_t inst;			/* actual instruction bits */
	enum md_opcode op;			/* decoded opcode enum */
	int out1, out2, in1, in2, in3;	/* output/input register names */
	md_addr_t target_PC;			/* actual next/target PC address */
	md_addr_t addr;			/* effective address, if load/store */
	struct RUU_station *rs;		/* RUU station being allocated */
	struct RUU_station *lsq;		/* LSQ station for ld/st's */
	struct bpred_update_t *dir_update_ptr;/* branch predictor dir update ptr */
	int stack_recover_idx;		/* bpred retstack recovery index */
	unsigned int pseq;			/* pipetrace sequence number */
	int is_write;				/* store? */
	int made_check;			/* used to ensure DLite entry */
	int br_taken, br_pred_taken;		/* if br, taken?  predicted taken? */
	int fetch_redirected = FALSE;
	byte_t temp_byte;			/* temp variable for spec mem access */
	half_t temp_half;			/* " ditto " */
	word_t temp_word;			/* " ditto " */
#ifdef HOST_HAS_QUAD
	quad_t temp_quad;			/* " ditto " */
#endif /* HOST_HAS_QUAD */
	enum md_fault_type fault;

	/* Wattch:  Added for pop count generation (AFs) */
	quad_t val_ra, val_rb, val_rc, val_ra_result;

	made_check = FALSE;
	n_dispatched = 0;
	while (/* instruction decode B/W left? */
			n_dispatched < (ruu_decode_width * fetch_speed)
			/* RUU and LSQ not full? */
			&& RUU_num < RUU_size && LSQ_num < LSQ_size
			/* insts still available from fetch unit? */
			&& fetch_num != 0
			/* on an acceptable trace path */
			&& (ruu_include_spec || !spec_mode))
	{
		/* if issuing in-order, block until last op issues if inorder issue */
		if (ruu_inorder_issue
				&& (last_op.rs && RSLINK_VALID(&last_op)
				&& !OPERANDS_READY(last_op.rs)))
		{
			/* stall until last operation is ready to issue */
			break;
		}

		/* get the next instruction from the IFETCH -> DISPATCH queue */
		inst = fetch_data[fetch_head].IR;
		regs0.regs_PC = fetch_data[fetch_head].regs_PC;
		pred_PC = fetch_data[fetch_head].pred_PC;
		dir_update_ptr = &(fetch_data[fetch_head].dir_update);
		stack_recover_idx = fetch_data[fetch_head].stack_recover_idx;
		pseq = fetch_data[fetch_head].ptrace_seq;

		/* decode the inst */
		MD_SET_OPCODE(op, inst);

		/* compute default next PC */
		regs0.regs_NPC = regs0.regs_PC + sizeof(md_inst_t);

		/* drain RUU for TRAPs and system calls */
		if (MD_OP_FLAGS(op) & F_TRAP)
		{
			if (RUU_num != 0)
				break;

			/* else, syscall is only instruction in the machine, at this
	     point we should not be in (mis-)speculative mode */
			if (spec_mode)
				panic("drained and speculative");
		}

		/* maintain $r0 semantics (in spec and non-spec space) */
		regs0.regs_R[MD_REG_ZERO] = 0; spec_regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
		regs0.regs_F.d[MD_REG_ZERO] = 0.0; spec_regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

		if (!spec_mode)
		{
			/* one more non-speculative instruction executed */
			GLOBAL->sim_num_insn++;
		}

		/* default effective address (none) and access */
		addr = 0; is_write = FALSE;

		/* Wattch: Get values of source operands */
#if defined(TARGET_PISA)
		val_ra = GPR(RS);
		val_rb = GPR(RT);
#elif defined(TARGET_ALPHA)
		val_ra = GPR(RA);
		val_rb = GPR(RB);
#endif

		/* set default fault - none */
		fault = md_fault_none;

		/* MSG */
		//if ((GPR(BS) + GPR(RD)) == 0x1000)
		//	printf (">>> %10d | %10d \n", BS, RD);

		/* more decoding and execution */
		switch (op)
		{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,CLASS,O1,O2,I1,I2,I3)		\
	case OP:							\
	/* compute output/input dependencies to out1-2 and in1-3 */	\
	out1 = O1; out2 = O2;						\
	in1 = I1; in2 = I2; in3 = I3;					\
	/* execute the instruction */					\
	SYMCAT(OP,_IMPL);						\
	break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	case OP:							\
	/* could speculatively decode a bogus inst, convert to NOP */	\
	op = MD_NOP_OP;						\
	/* compute output/input dependencies to out1-2 and in1-3 */	\
	out1 = NA; out2 = NA;						\
	in1 = NA; in2 = NA; in3 = NA;					\
	/* no EXPR */							\
	break;
#define CONNECT(OP)	/* nada... */
		/* the following macro wraps the instruction fault declaration macro
	     with a test to see if the trace generator is in non-speculative
	     mode, if so the instruction fault is declared, otherwise, the
	     error is shunted because instruction faults need to be masked on
	     the mis-speculated instruction paths */
#define DECLARE_FAULT(FAULT)						\
	{								\
	if (!spec_mode)						\
	fault = (FAULT);						\
	/* else, spec fault, ignore it, always terminate exec... */	\
	break;							\
	}
#include "machine.def"
		default:
			/* can speculatively decode a bogus inst, convert to a NOP */
			op = MD_NOP_OP;
			/* compute output/input dependencies to out1-2 and in1-3 */	\
			out1 = NA; out2 = NA;
			in1 = NA; in2 = NA; in3 = NA;
			/* no EXPR */
		}

		/* operation sets next PC */
		if (fault != md_fault_none) {
			fatal("non-speculative fault (%d) detected @ 0x%08p",
					fault, regs0.regs_PC);
		}

		/* Wattch: Get result values */
#if defined(TARGET_PISA)
		val_rc = GPR(RD);
		val_ra_result = GPR(RS);
#elif defined(TARGET_ALPHA)
		val_rc = GPR(RC);
		val_ra_result = GPR(RA);
#endif

		/* update memory access stats */
		if (MD_OP_FLAGS(op) & F_MEM)
		{
			sim_total_refs++;
			if (!spec_mode)
				sim_num_refs++;

			if (MD_OP_FLAGS(op) & F_STORE)
				is_write = TRUE;
			else
			{
				sim_total_loads++;
				if (!spec_mode)
					sim_num_loads++;
			}
		}

		br_taken = (regs0.regs_NPC != (regs0.regs_PC + sizeof(md_inst_t)));
		br_pred_taken = (pred_PC != (regs0.regs_PC + sizeof(md_inst_t)));

		if ((pred_PC != regs0.regs_NPC && pred_perfect)
				|| ((MD_OP_FLAGS(op) & (F_CTRL|F_DIRJMP)) == (F_CTRL|F_DIRJMP)
						&& target_PC != pred_PC && br_pred_taken))
		{
			/* Either 1) we're simulating perfect prediction and are in a
             mis-predict state and need to patch up, or 2) We're not simulating
             perfect prediction, we've predicted the branch taken, but our
             predicted target doesn't match the computed target (i.e.,
             mis-fetch).  Just update the PC values and do a fetch squash.
             This is just like calling fetch_squash() except we pre-anticipate
             the updates to the fetch values at the end of this function.  If
             case #2, also charge a mispredict penalty for redirecting fetch */
			fetch_pred_PC = fetch_regs_PC = regs0.regs_NPC;
			/* was: if (pred_perfect) */
			if (pred_perfect)
				pred_PC = regs0.regs_NPC;

			fetch_head = (ruu_ifq_size-1);
			fetch_num = 1;
			fetch_tail = 0;

			if (!pred_perfect)
				ruu_fetch_issue_delay = ruu_branch_penalty;

			fetch_redirected = TRUE;
		}

		/* is this a NOP */
		if (op != MD_NOP_OP)
		{
			/* for load/stores:
	       idep #0     - store operand (value that is store'ed)
	       idep #1, #2 - eff addr computation inputs (addr of access)

	     resulting RUU/LSQ operation pair:
	       RUU (effective address computation operation):
		 idep #0, #1 - eff addr computation inputs (addr of access)
	       LSQ (memory access operation):
		 idep #0     - operand input (value that is store'd)
		 idep #1     - eff addr computation result (from RUU op)

	     effective address computation is transfered via the reserved
	     name DTMP
			 */

			/* Wattch -- Dispatch + RAT lookup stage */
			rename_access++;

			/* fill in RUU reservation station */
			rs = &RUU[RUU_tail];

			rs->IR = inst;
			rs->op = op;
			rs->PC = regs0.regs_PC;
			rs->next_PC = regs0.regs_NPC; rs->pred_PC = pred_PC;
			rs->in_LSQ = FALSE;
			rs->ea_comp = FALSE;
			rs->recover_inst = FALSE;
			rs->dir_update = *dir_update_ptr;
			rs->stack_recover_idx = stack_recover_idx;
			rs->spec_mode = spec_mode;
			rs->addr = 0;
			/* rs->tag is already set */
			rs->seq = ++inst_seq;
			rs->queued = rs->issued = rs->completed = FALSE;
			rs->ptrace_seq = pseq;

			/* Wattch: Maintain values through core for AFs*/
			rs->val_ra = val_ra;
			rs->val_rb = val_rb;
			rs->val_rc = val_rc;
			rs->val_ra_result = val_ra_result;

			/* split ld/st's into two operations: eff addr comp + mem access */
			if (MD_OP_FLAGS(op) & F_MEM)
			{
				/* convert RUU operation from ld/st to an add (eff addr comp) */
				rs->op = MD_AGEN_OP;
				rs->ea_comp = TRUE;

				/* fill in LSQ reservation station */
				lsq = &LSQ[LSQ_tail];

				lsq->IR = inst;
				lsq->op = op;
				lsq->PC = regs0.regs_PC;
				lsq->next_PC = regs0.regs_NPC; lsq->pred_PC = pred_PC;
				lsq->in_LSQ = TRUE;
				lsq->ea_comp = FALSE;
				lsq->recover_inst = FALSE;
				lsq->dir_update.pdir1 = lsq->dir_update.pdir2 = NULL;
				lsq->dir_update.pmeta = NULL;
				lsq->stack_recover_idx = 0;
				lsq->spec_mode = spec_mode;
				lsq->addr = addr;
				/* lsq->tag is already set */
				lsq->seq = ++inst_seq;
				lsq->queued = lsq->issued = lsq->completed = FALSE;
				lsq->ptrace_seq = ptrace_seq++;

				/* Wattch: Maintain values through core for AFs*/
				lsq->val_ra = val_ra;
				lsq->val_rb = val_rb;
				lsq->val_rc = val_rc;
				lsq->val_ra_result = val_ra_result;

				/* pipetrace this uop */
				PTRACE->ptrace_newuop(lsq->ptrace_seq, "internal ld/st", lsq->PC, 0);
				PTRACE->ptrace_newstage(lsq->ptrace_seq, PST_DISPATCH, 0);

				/* link eff addr computation onto operand's output chains */
				ruu_link_idep(rs, /* idep_ready[] index */0, NA);
				ruu_link_idep(rs, /* idep_ready[] index */1, in2);
				ruu_link_idep(rs, /* idep_ready[] index */2, in3);

				/* install output after inputs to prevent self reference */
				ruu_install_odep(rs, /* odep_list[] index */0, DTMP);
				ruu_install_odep(rs, /* odep_list[] index */1, NA);

				/* link memory access onto output chain of eff addr operation */
				ruu_link_idep(lsq,
						/* idep_ready[] index */STORE_OP_INDEX/* 0 */,
						in1);
				ruu_link_idep(lsq,
						/* idep_ready[] index */STORE_ADDR_INDEX/* 1 */,
						DTMP);
				ruu_link_idep(lsq, /* idep_ready[] index */2, NA);

				/* install output after inputs to prevent self reference */
				ruu_install_odep(lsq, /* odep_list[] index */0, out1);
				ruu_install_odep(lsq, /* odep_list[] index */1, out2);

				/* install operation in the RUU and LSQ */
				n_dispatched++;
				RUU_tail = (RUU_tail + 1) % RUU_size;
				RUU_num++;
				LSQ_tail = (LSQ_tail + 1) % LSQ_size;
				LSQ_num++;

				if (OPERANDS_READY(rs))
				{
					/* Wattch -- both operands ready, 2 window write accesses */
					/* Wattch -- FIXME: currently being read from arch.
		     regfile (in ruu_link_idep) and written to window here.
		     should these values be read from arch. regfile or
		     another window entry? */
					window_access++;
					window_access++;
					window_preg_access++;
					window_preg_access++;

#ifdef DYNAMIC_AF
					regfile_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
					regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rb);
					regfile_num_pop_count_cycle+=2;
#endif

					/* eff addr computation ready, queue it on ready list */
					readyq_enqueue(rs);
				}
				else if (ONE_OPERANDS_READY(rs))
				{
					/* Wattch -- one operand ready, 1 window write accesses */
					window_access++;
					window_preg_access++;
#ifdef DYNAMIC_AF
					if(rs->idep_ready[0])
						regfile_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
					else
						regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rb);
					regfile_num_pop_count_cycle++;
#endif

				}
				/* issue may continue when the load/store is issued */
				RSLINK_INIT(last_op, lsq);

				/* issue stores only, loads are issued by lsq_refresh() */
				if (((MD_OP_FLAGS(op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
						&& OPERANDS_READY(lsq))
				{
					/* Wattch -- store operand ready, 1 LSQ access */
					lsq_store_data_access++;
					/* panic("store immediately ready"); */
					/* put operation on ready list, ruu_issue() issue it later */
					readyq_enqueue(lsq);
				}
			}
			else /* !(MD_OP_FLAGS(op) & F_MEM) */
			{
				/* Wattch: Regfile writes taken care of inside ruu_link_idep */
				/* link onto producing operation */
				ruu_link_idep(rs, /* idep_ready[] index */0, in1);
				ruu_link_idep(rs, /* idep_ready[] index */1, in2);
				ruu_link_idep(rs, /* idep_ready[] index */2, in3);

				/* install output after inputs to prevent self reference */
				ruu_install_odep(rs, /* odep_list[] index */0, out1);
				ruu_install_odep(rs, /* odep_list[] index */1, out2);

				/* install operation in the RUU */
				n_dispatched++;
				RUU_tail = (RUU_tail + 1) % RUU_size;
				RUU_num++;

				/* issue op if all its reg operands are ready (no mem input) */
				if (OPERANDS_READY(rs))
				{
					/* Wattch -- window access,
		     both operands ready, write them to window,
					 */
					window_access++;
					window_access++;
					window_preg_access++;
					window_preg_access++;

#ifdef DYNAMIC_AF
					regfile_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
					regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rb);
					regfile_num_pop_count_cycle+=2;
#endif

					/* put operation on ready list, ruu_issue() issue it later */
					readyq_enqueue(rs);
					/* issue may continue */
					last_op = RSLINK_NULL;
				}
				else if (ONE_OPERANDS_READY(rs))
				{
					/* Wattch -- window access,
		     one operand ready, write them to window,
					 */
					window_access++;
					window_preg_access++;
#ifdef DYNAMIC_AF
					if(rs->idep_ready[0])
						regfile_total_pop_count_cycle += POWER->pop_count(rs->val_ra);
					else
						regfile_total_pop_count_cycle += POWER->pop_count(rs->val_rb);
					regfile_num_pop_count_cycle++;
#endif

					/* could not issue this inst, stall issue until we can */
					RSLINK_INIT(last_op, rs);
				}
				else
				{
					/* could not issue this inst, stall issue until we can */
					RSLINK_INIT(last_op, rs);
				}
			}
		}
		else
		{
			/* this is a NOP, no need to update RUU/LSQ state */
			rs = NULL;
		}

		/* one more instruction executed, speculative or otherwise */
		sim_total_insn++;
		if (MD_OP_FLAGS(op) & F_CTRL)
			sim_total_branches++;

		if (!spec_mode)
		{
#if 0 /* moved above for EIO trace file support */
			/* one more non-speculative instruction executed */
			sim_num_insn++;
#endif

			/* if this is a branching instruction update BTB, i.e., only
	     non-speculative state is committed into the BTB */
			if (MD_OP_FLAGS(op) & F_CTRL)
			{
				sim_num_branches++;
				if (pred && bpred_spec_update == spec_ID)
				{
					BPRED->bpred_update(pred,
							/* branch address */regs0.regs_PC,
							/* actual target address */regs0.regs_NPC,
							/* taken? */regs0.regs_NPC != (regs0.regs_PC +
									sizeof(md_inst_t)),
									/* pred taken? */pred_PC != (regs0.regs_PC +
											sizeof(md_inst_t)),
											/* correct pred? */pred_PC == regs0.regs_NPC,
											/* opcode */op,
											/* predictor update ptr */&rs->dir_update);
				}
			}

			/* is the trace generator trasitioning into mis-speculation mode? */
			if (pred_PC != regs0.regs_NPC && !fetch_redirected)
			{
				/* entering mis-speculation mode, indicate this and save PC */
				spec_mode = TRUE;
				rs->recover_inst = TRUE;
				recover_PC = regs0.regs_NPC;
			}
		}

		/* entered decode/allocate stage, indicate in pipe trace */
		PTRACE->ptrace_newstage(pseq, PST_DISPATCH,
				(pred_PC != regs0.regs_NPC) ? PEV_MPOCCURED : 0);
		if (op == MD_NOP_OP)
		{
			/* end of the line */
			PTRACE->ptrace_endinst(pseq);
		}

		/* update any stats tracked by PC */
		for (i=0; i<pcstat_nelt; i++)
		{
			counter_t newval;
			int delta;

			/* check if any tracked stats changed */
			newval = STATVAL(pcstat_stats[i]);
			delta = newval - pcstat_lastvals[i];
			if (delta != 0)
			{
				STATS->stat_add_samples(pcstat_sdists[i], regs0.regs_PC, delta);
				pcstat_lastvals[i] = newval;
			}
		}

		/* consume instruction from IFETCH -> DISPATCH queue */
		fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
		fetch_num--;


#if 0
		o Dubugador foi retirado

		/* check for DLite debugger entry condition */
		made_check = TRUE;
		if (dlite_check_break(pred_PC,
				is_write ? ACCESS_WRITE : ACCESS_READ,
						addr, sim_num_insn, sim_cycle))
			dlite_main(regs.regs_PC, pred_PC, sim_cycle, &regs, mem);
	}

	/* need to enter DLite at least once per cycle */
	if (!made_check)
	{
		if (dlite_check_break(/* no next PC */0,
				is_write ? ACCESS_WRITE : ACCESS_READ,
						addr, sim_num_insn, sim_cycle))
			dlite_main(regs.regs_PC, /* no next PC */0, sim_cycle, &regs, mem);
	}
#endif

}

}


/*
 *  RUU_FETCH() - instruction fetch pipeline stage(s)
 */

/* initialize the instruction fetch pipeline stage */
/*static*/ void
sim::fetch_init(void)
{
	/* allocate the IFETCH -> DISPATCH instruction queue */
	fetch_data =
		(struct fetch_rec *)calloc(ruu_ifq_size, sizeof(struct fetch_rec));
	if (!fetch_data)
		fatal("out of virtual memory");

	fetch_num = 0;
	fetch_tail = fetch_head = 0;
	IFQ_count = 0;
	IFQ_fcount = 0;
}

/* dump contents of fetch stage registers and fetch queue */
void
sim::fetch_dump(FILE *stream)			/* output stream */
{
	int num, head;

	if (!stream)
		stream = stderr;

	fprintf(stream, "** fetch stage state **\n");

	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
	MISC->myfprintf(stream, "pred_PC: 0x%08p, recover_PC: 0x%08p\n",
			pred_PC, recover_PC);
	MISC->myfprintf(stream, "fetch_regs_PC: 0x%08p, fetch_pred_PC: 0x%08p\n",
			fetch_regs_PC, fetch_pred_PC);
	fprintf(stream, "\n");

	fprintf(stream, "** fetch queue contents **\n");
	fprintf(stream, "fetch_num: %d\n", fetch_num);
	fprintf(stream, "fetch_head: %d, fetch_tail: %d\n",
			fetch_head, fetch_tail);

	num = fetch_num;
	head = fetch_head;
	while (num)
	{
		fprintf(stream, "idx: %2d: inst: `", head);
		MACHINE->md_print_insn(fetch_data[head].IR, fetch_data[head].regs_PC, stream);
		fprintf(stream, "'\n");
		MISC->myfprintf(stream, "         regs_PC: 0x%08p, pred_PC: 0x%08p\n",
				fetch_data[head].regs_PC, fetch_data[head].pred_PC);
		head = (head + 1) & (ruu_ifq_size - 1);
		num--;
	}
}



/* fetch up as many instruction as one branch prediction and one cache line
   acess will support without overflowing the IFETCH -> DISPATCH QUEUE */
/*static*/ void
sim::ruu_fetch(void)
{

	int i, lat, tlb_lat, done = FALSE;
	md_inst_t inst;
	int stack_recover_idx;
	int branch_cnt;

	for (i=0, branch_cnt=0;
	/* fetch up to as many instruction as the DISPATCH stage can decode */
	i < (ruu_decode_width * fetch_speed)
	/* fetch until IFETCH -> DISPATCH queue fills */
	&& fetch_num < ruu_ifq_size
	/* and no IFETCH blocking condition encountered */
	&& !done;
	i++)
	{
		/* is this a bogus text address? (can happen on mis-spec path) */
		if (interrupt)		
		{
			break;
                }
			
		/* MSG Debbug */
		/*if (record.flag_interrupt)
		{
			fetch_regs_PC = record.regs.regs_NPC;
			record.flag_interrupt = false;
		}
		else*/

		/* fetch an instruction at the next predicted fetch address */
		fetch_regs_PC = fetch_pred_PC;

		/* Wattch: add power for i-fetch stage */
		icache_access++;

		// MSO: Ok, but now the INT_HANDLER start in the address 0x0030000. Changed the first condition
		//if (LOADER->ld_text_base <= fetch_regs_PC
		if (0x00300000 <= fetch_regs_PC
				&& fetch_regs_PC < (LOADER->ld_text_base+LOADER->ld_text_size)
				&& !(fetch_regs_PC & (sizeof(md_inst_t)-1)))
		{
			/* read instruction from memory */
			MEMORY->mem_access(mem, Read, fetch_regs_PC, &inst, sizeof(md_inst_t));

			/* address is within program text, read instruction from memory */
			lat = cache_il1_lat;
			if (cache_il1)
			{
				/* access the I-cache */
				lat =
					CACHE->cache_access(cache_il1, Read, IACOMPRESS(fetch_regs_PC),
							NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
							NULL, NULL);

				if (lat > cache_il1_lat)
					last_inst_missed = TRUE;
			}

			if (itlb)
			{
				/* access the I-TLB, NOTE: this code will initiate
					speculative TLB misses */
				tlb_lat =
					CACHE->cache_access(itlb, Read, IACOMPRESS(fetch_regs_PC),
							NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle,
							NULL, NULL);
				if (tlb_lat > 1)
					last_inst_tmissed = TRUE;

				/* I-cache/I-TLB accesses occur in parallel */
				lat = MAX(tlb_lat, lat);
			}

			/* I-cache/I-TLB miss? assumes I-cache hit >= I-TLB hit */
			if (lat != cache_il1_lat)
			{
				/* I-cache miss, block fetch until it is resolved */
				ruu_fetch_issue_delay += lat - 1;
				break;
			}
			/* else, I-cache/I-TLB hit */
		}
		else
		{
			/* fetch PC is bogus, send a NOP down the pipeline */
			inst = MD_NOP_INST;
		}
		/* have a valid inst, here */

		/* possibly use the BTB target */
		if (pred)
		{
			enum md_opcode op;

			/* pre-decode instruction, used for bpred stats recording */
			MD_SET_OPCODE(op, inst);

			/* get the next predicted fetch address; only use branch predictor
					result for branches (assumes pre-decode bits); NOTE: returned
					value may be 1 if bpred can only predict a direction */
			if (MD_OP_FLAGS(op) & F_CTRL)
				fetch_pred_PC =
					BPRED->bpred_lookup(pred,
							/* branch address */fetch_regs_PC,
							/* target address *//* FIXME: not computed */0,
							/* opcode */op,
							/* call? */MD_IS_CALL(op),
							/* return? */MD_IS_RETURN(op),
							/* updt */&(fetch_data[fetch_tail].dir_update),
							/* RSB index */&stack_recover_idx);
			else
				fetch_pred_PC = 0;

			/* valid address returned from branch predictor? */
			if (!fetch_pred_PC)
			{
				/* no predicted taken target, attempt not taken target */
				fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
			}
			else
			{
				/* go with target, NOTE: discontinuous fetch, so terminate */
				branch_cnt++;
				if (branch_cnt >= fetch_speed)
					done = TRUE;
			}
		}
		else
		{
			/* no predictor, just default to predict not taken, and
				continue fetching instructions linearly */
			fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
		}
		
		/* commit this instruction to the IFETCH -> DISPATCH queue */
		fetch_data[fetch_tail].IR = inst;
		fetch_data[fetch_tail].regs_PC = fetch_regs_PC;
		fetch_data[fetch_tail].pred_PC = fetch_pred_PC;
		fetch_data[fetch_tail].stack_recover_idx = stack_recover_idx;
		fetch_data[fetch_tail].ptrace_seq = ptrace_seq++;

		/* for pipe trace */
		PTRACE->ptrace_newinst(fetch_data[fetch_tail].ptrace_seq,
				inst, fetch_data[fetch_tail].regs_PC,
				0);
		PTRACE->ptrace_newstage(fetch_data[fetch_tail].ptrace_seq,
				PST_IFETCH,
				((last_inst_missed ? PEV_CACHEMISS : 0)
						| (last_inst_tmissed ? PEV_TLBMISS : 0)));
		last_inst_missed = FALSE;
		last_inst_tmissed = FALSE;

		/* adjust instruction fetch queue */
		fetch_tail = (fetch_tail + 1) & (ruu_ifq_size - 1);
		fetch_num++;


	}
}

/* start simulation, program loaded, processor precise state initialized */
int
sim::sim_main(void)
{
	/* ignore any floating point exceptions, they may occur on mis-speculated
     execution paths */
	signal(SIGFPE, SIG_IGN);

	/* set up program entry state */
	regs0.regs_PC = LOADER->ld_prog_entry;
	regs0.regs_NPC = regs0.regs_PC + sizeof(md_inst_t);

#if 0
	/* check for DLite debugger entry condition */
	if (dlite_check_break(regs0.regs_PC, /* no access */0, /* addr */0, 0, 0))
		dlite_main(regs0.regs_PC, regs0.regs_PC + sizeof(md_inst_t),
				sim_cycle, &regs0, mem);
#endif

	/* fast forward simulator loop, performs functional simulation for
     FASTFWD_COUNT insts, then turns on performance (timing) simulation */
	if (fastfwd_count > 0)
	{
		int icount;
		md_inst_t inst;			/* actual instruction bits */
		enum md_opcode op;		/* decoded opcode enum */
		md_addr_t target_PC;		/* actual next/target PC address */
		md_addr_t addr;			/* effective address, if load/store */
		int is_write;			/* store? */
		byte_t temp_byte;			/* temp variable for spec mem access */
		half_t temp_half;			/* " ditto " */
		word_t temp_word;			/* " ditto " */
#ifdef HOST_HAS_QUAD
		quad_t temp_quad;			/* " ditto " */
#endif /* HOST_HAS_QUAD */
		enum md_fault_type fault;

		fprintf(stderr, "sim: ** fast forwarding %d insts **\n", fastfwd_count);

		for (icount=0; icount < fastfwd_count; icount++)
		{
			/* maintain $r0 semantics */
			regs0.regs_R[MD_REG_ZERO] = 0;
#ifdef TARGET_ALPHA
			regs0.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

			/* get the next instruction to execute */
			inst = __UNCHK_MEM_READ(mem, regs0.regs_PC, md_inst_t);

			/* set default reference address */
			addr = 0; is_write = FALSE;

			/* set default fault - none */
			fault = md_fault_none;

			/* decode the instruction */
			MD_SET_OPCODE(op, inst);

			/* execute the instruction */
			switch (op)
			{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
	case OP:							\
	SYMCAT(OP,_IMPL);						\
	break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
	case OP:							\
	panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)						\
	{ fault = (FAULT); break; }
#include "machine.def"

			default:
				panic("attempted to execute a bogus opcode");
			}

			if (fault != md_fault_none)
				fatal("fault (%d) detected @ 0x%08p", fault, regs0.regs_PC);

			/* update memory access stats */
			if (MD_OP_FLAGS(op) & F_MEM)
			{
				if (MD_OP_FLAGS(op) & F_STORE)
					is_write = TRUE;
			}

#if 0
			/* check for DLite debugger entry condition */
			if (dlite_check_break(regs.regs_NPC,
					is_write ? ACCESS_WRITE : ACCESS_READ,
							addr, sim_num_insn, sim_num_insn))
				dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);
#endif

			/* go to the next instruction */
			regs0.regs_PC = regs0.regs_NPC;
			regs0.regs_NPC += sizeof(md_inst_t);

		}
	}

	fprintf(stderr, "sim: ** starting performance simulation **\n");

	/* set up timing simulation entry state */
	fetch_regs_PC = regs0.regs_PC - sizeof(md_inst_t);
	fetch_pred_PC = regs0.regs_PC;
	regs0.regs_PC = regs0.regs_PC - sizeof(md_inst_t);

	/* main simulator loop, NOTE: the pipe stages are traverse in reverse order
     to eliminate this/next state synchronization and relaxation problems */
	for (;;)
	{

		/* safe state */
		if (((interrupt) && (RUU_num == 0)) && (LSQ_num == 0))
		{
			//printf("interrupt actived fetch_pred_PC 0x%x  fetch_regs_PC 0x%x \n", fetch_pred_PC, fetch_regs_PC);
			regs0.regs_R[27] = fetch_pred_PC;
                        fetch_pred_PC = 0x00300000;	
			interrupt = false;
			//printf("interrupt actived fetch_pred_PC 0x%x  fetch_regs_PC 0x%x \n", fetch_pred_PC, fetch_regs_PC);
		}

		/* RUU/LSQ sanity checks */
		if (RUU_num < LSQ_num)
			panic("RUU_num < LSQ_num");
		if (((RUU_head + RUU_num) % RUU_size) != RUU_tail)
			panic("RUU_head/RUU_tail wedged");
		if (((LSQ_head + LSQ_num) % LSQ_size) != LSQ_tail)
			panic("LSQ_head/LSQ_tail wedged");

		/*3 added for Wattch to clear hardware access counters */
		POWER->clear_access_stats();

		/* check if pipetracing is still active */
		PTRACE->ptrace_check_active(regs0.regs_PC, GLOBAL->sim_num_insn, sim_cycle);

		/* indicate new cycle in pipetrace */
		PTRACE->ptrace_newcycle(sim_cycle);

		/* commit entries from RUU/LSQ to architected register file */
		ruu_commit();

		/* service function unit release events */
		ruu_release_fu();

		/* ==> may have ready queue entries carried over from previous cycles */

		/* service result completions, also readies dependent operations */
		/* ==> inserts operations into ready queue --> register deps resolved */
		ruu_writeback();
		if (!bugcompat_mode)
		{
			/* try to locate memory operations that are ready to execute */
			/* ==> inserts operations into ready queue --> mem deps resolved */
			lsq_refresh();

			/* issue operations ready to execute from a previous cycle */
			/* <== drains ready queue <-- ready operations commence execution */
			ruu_issue();
		}

		/* decode and dispatch new operations */
		/* ==> insert ops w/ no deps or all regs ready --> reg deps resolved */
		ruu_dispatch();

		if (bugcompat_mode)
		{
			/* try to locate memory operations that are ready to execute */
			/* ==> inserts operations into ready queue --> mem deps resolved */
			lsq_refresh();

			/* issue operations ready to execute from a previous cycle */
			/* <== drains ready queue <-- ready operations commence execution */
			ruu_issue();
		}

		if (!ruu_fetch_issue_delay)
			ruu_fetch();
		else
			ruu_fetch_issue_delay--;

		
		/* Added by Wattch to update per-cycle power statistics */
		POWER->update_power_stats();


		/* update buffer occupancy stats */
		IFQ_count += fetch_num;
		IFQ_fcount += ((fetch_num == ruu_ifq_size) ? 1 : 0);
		RUU_count += RUU_num;
		RUU_fcount += ((RUU_num == RUU_size) ? 1 : 0);
		LSQ_count += LSQ_num;
		LSQ_fcount += ((LSQ_num == LSQ_size) ? 1 : 0);
	  	
  		
		wait();

		/* go to next cycle */
		//sim_cycle++;

		/* finish early? */
		if (max_insts && sim_total_insn >= max_insts)
			return 0;
	}
}

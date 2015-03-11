#ifndef SIM_H
#define SIM_H

#include <stdio.h>
#include <setjmp.h>
#include <time.h>

#include "memory.hpp"
#include "regs.hpp"
#include "stats.hpp"
#include "options.hpp"
#include "misc.hpp"
#include "cache.hpp"
#include "loader.hpp"
#include "bpred.hpp"
#include "resource.hpp"
#include "eval.hpp"
#include "ptrace.hpp"
#include "syscall.hpp"
#include "endian.hpp"
#include "bitmap.hpp"
#include "symbol.hpp"
#include "range.hpp"
#include "libexo.hpp"
//#include "dlite.hpp" O debugador nao foi implementado

/* added for Wattch */
#include "power.hpp"
#include "global.hpp"

#undef GAMBA_MEM
#define GAMBA_MEM MEMORY

/* text-based stat profiles */
#define MAX_PCSTAT_VARS 8


// pq ta aki?
/* total RS links allocated at program start */
#define MAX_RS_LINKS                    4096
/*
 * functional unit resource configuration
 */

/* resource pool indices, NOTE: update these if you change FU_CONFIG */
#define FU_IALU_INDEX			0
#define FU_IMULT_INDEX			1
#define FU_MEMPORT_INDEX		2
#define FU_FPALU_INDEX			3
#define FU_FPMULT_INDEX			4


/* wedge all stat values into a counter_t */
#define STATVAL(STAT)							\
  ((STAT)->sc == msc_int							\
   ? (counter_t)*((STAT)->variant.for_int.var)			\
   : ((STAT)->sc == msc_uint						\
      ? (counter_t)*((STAT)->variant.for_uint.var)		\
      : ((STAT)->sc == msc_counter					\
	 ? *((STAT)->variant.for_counter.var)				\
	 : (panic("bad stat class"), 0))))



// TOTALMENTE EM DUVIDA
/* resource pool definition, NOTE: update FU_*_INDEX defs if you change this */
struct res_desc fu_config[] = {
  {
    "integer-ALU",
    4,
    0,
    {
      { IntALU, 1, 1 }
    }
  },
  {
    "integer-MULT/DIV",
    1,
    0,
    {
      { IntMULT, 3, 1 },
      { IntDIV, 20, 19 }
    }
  },
  {
    "memory-port",
    2,
    0,
    {
      { RdPort, 1, 1 },
      { WrPort, 1, 1 }
    }
  },
  {
    "FP-adder",
    4,
    0,
    {
      { FloatADD, 2, 1 },
      { FloatCMP, 2, 1 },
      { FloatCVT, 2, 1 }
    }
  },
  {
    "FP-MULT/DIV",
    1,
    0,
    {
      { FloatMULT, 4, 1 },
      { FloatDIV, 12, 12 },
      { FloatSQRT, 24, 24 }
    }
  },
};


/*
 * processor core definitions and declarations
 */

/* inst tag type, used to tag an operation instance in the RUU */
typedef unsigned int INST_TAG_TYPE;

/* inst sequence type, used to order instructions in the ready list, if
   this rolls over the ready list order temporarily will get messed up,
   but execution will continue and complete correctly */
typedef unsigned int INST_SEQ_TYPE;


/* total input dependencies possible */
#define MAX_IDEPS               3

/* total output dependencies possible */
#define MAX_ODEPS               2

/* a register update unit (RUU) station, this record is contained in the
   processors RUU, which serves as a collection of ordered reservations
   stations.  The reservation stations capture register results and await
   the time when all operands are ready, at which time the instruction is
   issued to the functional units; the RUU is an order circular queue, in which
   instructions are inserted in fetch (program) order, results are stored in
   the RUU buffers, and later when an RUU entry is the oldest entry in the
   machines, it and its instruction's value is retired to the architectural
   register file in program order, NOTE: the RUU and LSQ share the same
   structure, this is useful because loads and stores are split into two
   operations: an effective address add and a load/store, the add is inserted
   into the RUU and the load/store inserted into the LSQ, allowing the add
   to wake up the load/store when effective address computation has finished */
struct RUU_station {
  /* inst info */
  md_inst_t IR;			/* instruction bits */
  enum md_opcode op;			/* decoded instruction opcode */
  md_addr_t PC, next_PC, pred_PC;	/* inst PC, next PC, predicted PC */
  int in_LSQ;				/* non-zero if op is in LSQ */
  int ea_comp;				/* non-zero if op is an addr comp */
  int recover_inst;			/* start of mis-speculation? */
  int stack_recover_idx;		/* non-speculative TOS for RSB pred */
  struct bpred_update_t dir_update;	/* bpred direction update info */
  int spec_mode;			/* non-zero if issued in spec_mode */
  md_addr_t addr;			/* effective address for ld/st's */
  INST_TAG_TYPE tag;			/* RUU slot tag, increment to
					   squash operation */
  INST_SEQ_TYPE seq;			/* instruction sequence, used to
					   sort the ready list and tag inst */
  unsigned int ptrace_seq;		/* pipetrace sequence number */

  /* instruction status */
  int queued;				/* operands ready and queued */
  int issued;				/* operation is/was executing */
  int completed;			/* operation has completed execution */

  /* Wattch: values of source operands and result operand used for AF generation */
  quad_t val_ra, val_rb, val_rc, val_ra_result;

  /* output operand dependency list, these lists are used to
     limit the number of associative searches into the RUU when
     instructions complete and need to wake up dependent insts */
  int onames[MAX_ODEPS];		/* output logical names (NA=unused) */
  struct RS_link *odep_list[MAX_ODEPS];	/* chains to consuming operations */

  /* input dependent links, the output chains rooted above use these
     fields to mark input operands as ready, when all these fields have
     been set non-zero, the RUU operation has all of its register
     operands, it may commence execution as soon as all of its memory
     operands are known to be read (see lsq_refresh() for details on
     enforcing memory dependencies) */
  int idep_ready[MAX_IDEPS];		/* input operand ready? */
};

/* non-zero if all register operands are ready, update with MAX_IDEPS */
#define OPERANDS_READY(RS)                                              \
  ((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2])

/* non-zero if one register operands is ready, update with MAX_IDEPS */
#define ONE_OPERANDS_READY(RS)                                              \
  ((RS)->idep_ready[0] || (RS)->idep_ready[1])


/*
 * input dependencies for stores in the LSQ:
 *   idep #0 - operand input (value that is store'd)
 *   idep #1 - effective address input (address of store operation)
 */
#define STORE_OP_INDEX                  0
#define STORE_ADDR_INDEX                1

#define STORE_OP_READY(RS)              ((RS)->idep_ready[STORE_OP_INDEX])
#define STORE_ADDR_READY(RS)            ((RS)->idep_ready[STORE_ADDR_INDEX])


/*
 * RS_LINK defs and decls
 */

/* a reservation station link: this structure links elements of a RUU
   reservation station list; used for ready instruction queue, event queue, and
   output dependency lists; each RS_LINK node contains a pointer to the RUU
   entry it references along with an instance tag, the RS_LINK is only valid if
   the instruction instance tag matches the instruction RUU entry instance tag;
   this strategy allows entries in the RUU can be squashed and reused without
   updating the lists that point to it, which significantly improves the
   performance of (all to frequent) squash events */
struct RS_link {
  struct RS_link *next;			/* next entry in list */
  struct RUU_station *rs;		/* referenced RUU resv station */
  INST_TAG_TYPE tag;			/* inst instance sequence number */
  union {
    tick_t when;			/* time stamp of entry (for eventq) */
    INST_SEQ_TYPE seq;			/* inst sequence */
    int opnum;				/* input/output operand number */
  } x;
};



/* NULL value for an RS link */
#define RSLINK_NULL_DATA		{ NULL, NULL, 0 }

/* create and initialize an RS link */
#define RSLINK_INIT(RSL, RS)						\
  ((RSL).next = NULL, (RSL).rs = (RS), (RSL).tag = (RS)->tag)

/* non-zero if RS link is NULL */
#define RSLINK_IS_NULL(LINK)            ((LINK)->rs == NULL)

/* non-zero if RS link is to a valid (non-squashed) entry */
#define RSLINK_VALID(LINK)              ((LINK)->tag == (LINK)->rs->tag)

/* extra RUU reservation station pointer */
#define RSLINK_RS(LINK)                 ((LINK)->rs)

/* get a new RS link record */
#define RSLINK_NEW(DST, RS)						\
  { struct RS_link *n_link;						\
    if (!rslink_free_list)						\
      panic("out of rs links");						\
    n_link = rslink_free_list;						\
    rslink_free_list = rslink_free_list->next;				\
    n_link->next = NULL;						\
    n_link->rs = (RS); n_link->tag = n_link->rs->tag;			\
    (DST) = n_link;							\
  }

/* free an RS link record */
#define RSLINK_FREE(LINK)						\
  {  struct RS_link *f_link = (LINK);					\
     f_link->rs = NULL; f_link->tag = 0;				\
     f_link->next = rslink_free_list;					\
     rslink_free_list = f_link;						\
  }

/* FIXME: could this be faster!!! */
/* free an RS link list */
#define RSLINK_FREE_LIST(LINK)						\
  {  struct RS_link *fl_link, *fl_link_next;				\
     for (fl_link=(LINK); fl_link; fl_link=fl_link_next)		\
       {								\
	 fl_link_next = fl_link->next;					\
	 RSLINK_FREE(fl_link);						\
       }								\
  }


/* an entry in the create vector */
struct CV_link {
  struct RUU_station *rs;               /* creator's reservation station */
  int odep_num;                         /* specific output operand */
};

/* get a new create vector link */
#define CVLINK_INIT(CV, RS,ONUM)	((CV).rs = (RS), (CV).odep_num = (ONUM))

/* size of the create vector (one entry per architected register) */
#define CV_BMAP_SZ              (BITMAP_SIZE(MD_TOTAL_REGS))

/* read a create vector entry */
#define CREATE_VECTOR(N)        (BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N))\
				 ? spec_create_vector[N]                \
				 : create_vector[N])

/* read a create vector timestamp entry */
#define CREATE_VECTOR_RT(N)     (BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N))\
				 ? spec_create_vector_rt[N]             \
				 : create_vector_rt[N])

/* set a create vector entry */
#define SET_CREATE_VECTOR(N, L) (spec_mode                              \
				 ? (BITMAP_SET(use_spec_cv, CV_BMAP_SZ, (N)),\
				    spec_create_vector[N] = (L))        \
				 : (create_vector[N] = (L)))

/* integer register file */
#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
/* floating point register file */
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
/* miscellaneous registers */
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))

/* speculative memory hash table size, NOTE: this must be a power-of-two */
#define STORE_HASH_SIZE		32

/* speculative memory hash table definition, accesses go through this hash
   table when accessing memory in speculative mode, the hash table flush the
   table when recovering from mispredicted branches */
struct spec_mem_ent {
  struct spec_mem_ent *next;		/* ptr to next hash table bucket */
  md_addr_t addr;			/* virtual address of spec state */
  unsigned int data[2];			/* spec buffer, up to 8 bytes */
};

/* IFETCH -> DISPATCH instruction queue definition */
struct fetch_rec {
  md_inst_t IR;				/* inst register */
  md_addr_t regs_PC, pred_PC;		/* current PC, predicted next PC */
  struct bpred_update_t dir_update;	/* bpred direction update info */
  int stack_recover_idx;		/* branch predictor RSB index */
  unsigned int ptrace_seq;		/* print trace sequence id */
};

/* speculative memory hash table address hash function */
#define HASH_ADDR(ADDR)							\
  ((((ADDR) >> 24)^((ADDR) >> 16)^((ADDR) >> 8)^(ADDR)) & (STORE_HASH_SIZE-1))


/*
 * configure the instruction decode engine
 */

#define DNA			(0)

#if defined(TARGET_PISA)

/* general register dependence decoders */
#define DGPR(N)			(N)
#define DGPR_D(N)		((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DHI			(0+32+32)
#define DLO			(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)

#elif defined(TARGET_ALPHA)

/* general register dependence decoders, $r31 maps to DNA (0) */
#define DGPR(N)			(31 - (N)) /* was: (((N) == 31) ? DNA : (N)) */

/* floating point register dependence decoders */
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))

/* miscellaneous register dependence decoders */
#define DFPCR			(0+32+32)
#define DUNIQ			(1+32+32)
#define DTMP			(2+32+32)

#else
#error No ISA target defined...
#endif


/*
 * configure the execution engine
 */

/* next program counter */
#define SET_NPC(EXPR)           (this->regs0.regs_NPC = (EXPR))

/* target program counter */
#undef  SET_TPC
#define SET_TPC(EXPR)		(target_PC = (EXPR))

/* current program counter */
#define CPC                     (this->regs0.regs_PC)
#define SET_CPC(EXPR)           (this->regs0.regs_PC = (EXPR))

/* general purpose register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define GPR(N)                  (BITMAP_SET_P(use_spec_R, R_BMAP_SZ, (N))\
				 ? spec_regs_R[N]                       \
				 : this->regs0.regs_R[N])

#define SET_GPR(N,EXPR)         (spec_mode				\
				 ? ((spec_regs_R[N] = (EXPR)),		\
				    BITMAP_SET(use_spec_R, R_BMAP_SZ, (N)),\
				    spec_regs_R[N])			\
				 : (this->regs0.regs_R[N] = (EXPR)))

#if defined(TARGET_PISA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_L(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.l[(N)]                   \
				 : this->regs0.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.l[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.l[(N)])			\
				 : (this->regs0.regs_F.l[(N)] = (EXPR)))
#define FPR_F(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.f[(N)]                   \
				 : this->regs0.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.f[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.f[(N)])			\
				 : (this->regs0.regs_F.f[(N)] = (EXPR)))
#define FPR_D(N)                (BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1))\
				 ? spec_regs_F.d[(N) >> 1]              \
				 : this->regs0.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)       (spec_mode				\
				 ? ((spec_regs_F.d[(N) >> 1] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)),\
				    spec_regs_F.d[(N) >> 1])		\
				 : (this->regs0.regs_F.d[(N) >> 1] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define HI			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*hi*/0)\
				 ? spec_regs_C.hi			\
				 : this->regs0.regs_C.hi)
#define SET_HI(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.hi = (EXPR)),		\
				    BITMAP_SET(use_spec_C, C_BMAP_SZ,/*hi*/0),\
				    spec_regs_C.hi)			\
				 : (this->regs0.regs_C.hi = (EXPR)))
#define LO			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*lo*/1)\
				 ? spec_regs_C.lo			\
				 : this->regs0.regs_C.lo)
#define SET_LO(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.lo = (EXPR)),		\
				    BITMAP_SET(use_spec_C, C_BMAP_SZ,/*lo*/1),\
				    spec_regs_C.lo)			\
				 : (this->regs0.regs_C.lo = (EXPR)))
#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2)\
				 ? spec_regs_C.fcc			\
				 : this->regs0.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/1),\
				    spec_regs_C.fcc)			\
				 : (this->regs0.regs_C.fcc = (EXPR)))

#elif defined(TARGET_ALPHA)

/* floating point register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPR_Q(N)		(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))\
				 ? spec_regs_F.q[(N)]                   \
				 : this->regs0.regs_F.q[(N)])
#define SET_FPR_Q(N,EXPR)	(spec_mode				\
				 ? ((spec_regs_F.q[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),\
				    spec_regs_F.q[(N)])			\
				 : (this->regs0.regs_F.q[(N)] = (EXPR)))
#define FPR(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N))\
				 ? spec_regs_F.d[(N)]			\
				 : this->regs0.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(spec_mode				\
				 ? ((spec_regs_F.d[(N)] = (EXPR)),	\
				    BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)),\
				    spec_regs_F.d[(N)])			\
				 : (this->regs0.regs_F.d[(N)] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage
   provided for fast recovery during wrong path execute (see tracer_recover()
   for details on this process */
#define FPCR			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fpcr*/0)\
				 ? spec_regs_C.fpcr			\
				 : this->regs0.regs_C.fpcr)
#define SET_FPCR(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fpcr = (EXPR)),	\
				   BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fpcr*/0),\
				    spec_regs_C.fpcr)			\
				 : (this->regs0.regs_C.fpcr = (EXPR)))
#define UNIQ			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*uniq*/1)\
				 ? spec_regs_C.uniq			\
				 : this->regs0.regs_C.uniq)
#define SET_UNIQ(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.uniq = (EXPR)),	\
				   BITMAP_SET(use_spec_C,C_BMAP_SZ,/*uniq*/1),\
				    spec_regs_C.uniq)			\
				 : (this->regs0.regs_C.uniq = (EXPR)))
#define FCC			(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2)\
				 ? spec_regs_C.fcc			\
				 : this->regs0.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode				\
				 ? ((spec_regs_C.fcc = (EXPR)),		\
				    BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/1),\
				    spec_regs_C.fcc)			\
				 : (this->regs0.regs_C.fcc = (EXPR)))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros, NOTE: speculative copy on
   write storage provided for fast recovery during wrong path execute (see
   tracer_recover() for details on this process */

/*
#define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
  (addr = (SRC),							\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V)))\
    : ((FAULT) = this->MEMORY->mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V)))),	\
   SRC_V)
*/

/* MSG */
#define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
	(addr = (SRC),							\
	(spec_mode																										\
	    ? ((FAULT) = spec_mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V)))					\
	    : ((addr >= 0x80000000 )															\
				? ((FAULT) = this->memshared_access(Read, addr, &SRC_V, sizeof(SRC_V)))		\
				: ((FAULT) = this->MEMORY->mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V))))),				\
	SRC_V)


#define READ_BYTE(SRC, FAULT)	__READ_SPECMEM((SRC), temp_byte, (FAULT))
#define READ_HALF(SRC, FAULT)	__READ_SPECMEM((SRC), temp_half, (FAULT))
#define READ_WORD(SRC, FAULT)	__READ_SPECMEM((SRC), temp_word, (FAULT))
#ifdef HOST_HAS_QUAD
#define READ_QUAD(SRC, FAULT)	__READ_SPECMEM((SRC), temp_quad, (FAULT))
#endif /* HOST_HAS_QUAD */

/*
#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
  (DST_V = (SRC), addr = (DST),						\
   (spec_mode								\
    ? ((FAULT) = spec_mem_access(mem, Write, addr, &DST_V, sizeof(DST_V)))\
    : ((FAULT) = this->MEMORY->mem_access(mem, Write, addr, &DST_V, sizeof(DST_V)))))
*/

/*MSG*/
#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
	(DST_V = (SRC), addr = (DST),					\
	(spec_mode											\
	    ? ((FAULT) = spec_mem_access(mem, Write, addr, &DST_V, sizeof(DST_V)))\
	    : ((addr >= 0x80000000 )														\
			? ((FAULT) = this->memshared_access(Write, addr, &DST_V, sizeof(DST_V)))	\
	   		: ((FAULT) = this->MEMORY->mem_access(mem, Write, addr, &DST_V, sizeof(DST_V))))))


#define WRITE_BYTE(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_byte, (FAULT))
#define WRITE_HALF(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_half, (FAULT))
#define WRITE_WORD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_word, (FAULT))
#ifdef HOST_HAS_QUAD
#define WRITE_QUAD(SRC, DST, FAULT)					\
  __WRITE_SPECMEM((SRC), (DST), temp_quad, (FAULT))
#endif /* HOST_HAS_QUAD */

/* system call handler macro */
#define SYSCALL(INST)							\
  (/* only execute system calls in non-speculative mode */		\
   (spec_mode ? panic("speculative syscall") : (void) 0),		\
   this->SYSC->sys_syscall(&this->regs0, this->mem, INST, TRUE))


typedef struct{
	md_addr_t inst;
	regs_t regs;
	bool flag_interrupt;
} record_interrupt;


class simplescalar;

class sim
{
	private:
		/*
		 * This file implements a very detailed out-of-order issue superscalar
		 * processor with a two-level memory system and speculative execution support.
		 * This simulator is a performance simulator, tracking the latency of all
		 * pipeline operations.
		 */

		/* simulated registers */
		 struct regs_t regs0;

		 /*MSG: registers copy for interrupt*/
		 struct regs_t regs1;

		/* simulated memory */
		 struct mem_t *mem;


		/*
		 * simulator options
		 */

		/* maximum number of inst's to execute */
		 unsigned int max_insts;

		/* number of insts skipped before timing starts */
		 int fastfwd_count;

		/* pipeline trace range and output filename */
		 int ptrace_nelt;
		 char *ptrace_opts[2];

		/* instruction fetch queue size (in insts) */
		 int ruu_ifq_size;

		/* extra branch mis-prediction latency */
		 int ruu_branch_penalty;

		/* speed of front-end of machine relative to execution core */
		 int fetch_speed;

		/* branch predictor type {nottaken|taken|perfect|bimod|2lev} */
		 char *pred_type;

		/* bimodal predictor config (<table_size>) */
		 int bimod_nelt;

		/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
		int twolev_nelt;


		/* combining predictor config (<meta_table_size> */
		int comb_nelt;


		/* BTB predictor config (<num_sets> <associativity>) */
		int btb_nelt;

		/* run pipeline with in-order issue */
		int ruu_inorder_issue;

		/* issue instructions down wrong execution paths */
		int ruu_include_spec;

		/* l1 data cache config, i.e., {<config>|none} */
		char *cache_dl1_opt;

		/* l1 data cache hit latency (in cycles) */
		int cache_dl1_lat;

		/* l2 data cache config, i.e., {<config>|none} */
		char *cache_dl2_opt;

		/* l2 data cache hit latency (in cycles) */
		int cache_dl2_lat;

		/* l1 instruction cache config, i.e., {<config>|dl1|dl2|none} */
		char *cache_il1_opt;

		/* l1 instruction cache hit latency (in cycles) */
		int cache_il1_lat;

		/* l2 instruction cache config, i.e., {<config>|dl1|dl2|none} */
		char *cache_il2_opt;

		/* l2 instruction cache hit latency (in cycles) */
		int cache_il2_lat;

		/* flush caches on system calls */
		int flush_on_syscalls;

		/* convert 64-bit inst addresses to 32-bit inst equivalents */
		int compress_icache_addrs;

		/* memory access latency (<first_chunk> <inter_chunk>) */
		int mem_nelt;
		int mem_lat[2];
		 // { /* lat to first chunk */18, /* lat between remaining chunks */2 };

		/* memory access bus width (in bytes) */
		int mem_bus_width;

		/* instruction TLB config, i.e., {<config>|none} */
		char *itlb_opt;

		/* data TLB config, i.e., {<config>|none} */
		char *dtlb_opt;

		/* inst/data TLB miss latency (in cycles) */
		int tlb_miss_lat;

		/* total number of integer multiplier/dividers available */
		int res_imult;

		int pcstat_nelt;
		char *pcstat_vars[MAX_PCSTAT_VARS];

		/* operate in backward-compatible bugs mode (for testing only) */
		int bugcompat_mode;

		/*
		 * simulator stats
		 */

		/* total number of instructions executed */
		counter_t sim_total_insn;

		/* total number of memory references committed */
		counter_t sim_num_refs;

		/* total number of memory references executed */
		counter_t sim_total_refs;

		/* total number of loads committed */
		counter_t sim_num_loads;

		/* total number of loads executed */
		counter_t sim_total_loads;

		/* total number of branches committed */
		counter_t sim_num_branches;

		/* total number of branches executed */
		counter_t sim_total_branches;

		/* occupancy counters */
		counter_t IFQ_count;		/* cumulative IFQ occupancy */
		counter_t IFQ_fcount;		/* cumulative IFQ full count */
		counter_t RUU_count;		/* cumulative RUU occupancy */
		counter_t RUU_fcount;		/* cumulative RUU full count */
		counter_t LSQ_count;		/* cumulative LSQ occupancy */
		counter_t LSQ_fcount;		/* cumulative LSQ full count */

		/* total non-speculative bogus addresses seen (debug var) */
		counter_t sim_invalid_addrs;

		/*
		 * simulator state variables
		 */

		/* instruction sequence counter, used to assign unique id's to insts */
		unsigned int inst_seq;

		/* pipetrace instruction sequence counter */
		unsigned int ptrace_seq;

		/* speculation mode, non-zero when mis-speculating, i.e., executing
		   instructions down the wrong path, thus state recovery will eventually have
		   to occur that resets processor register and memory state back to the last
		   precise state */
		int spec_mode;

		/* cycles until fetch issue resumes */
		unsigned ruu_fetch_issue_delay;

		/* perfect prediction enabled */
		 int pred_perfect;

		/* speculative bpred-update enabled */
		char *bpred_spec_opt;
		enum { spec_ID, spec_WB, spec_CT } bpred_spec_update;


		/* branch predictor */
		struct bpred_t *pred;

		/* functional unit resource pool */
		struct res_pool *fu_pool;

		/* text-based stat profiles */
		struct stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
		counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
		struct stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];


		/* register update unit, combination of reservation stations and reorder
		   buffer device, organized as a circular queue */
		struct RUU_station *RUU;		/* register update unit */
		int RUU_head, RUU_tail;		/* RUU head and tail pointers */
		int RUU_num;			/* num entries currently in RUU */

		struct RUU_station *LSQ;         /* load/store queue */
		int LSQ_head, LSQ_tail;          /* LSQ head and tail pointers */
		int LSQ_num;                     /* num entries currently in LSQ */


		/* RS link free list, grab RS_LINKs from here, when needed */
		struct RS_link *rslink_free_list;

		struct RS_link RSLINK_NULL;

		/* pending event queue, sorted from soonest to latest event (in time), NOTE:
		   RS_LINK nodes are used for the event queue list so that it need not be
		   updated during squash events */
		struct RS_link *event_queue;

		/* the ready instruction queue */
		struct RS_link *ready_queue;

		/* a NULL create vector entry */
		struct CV_link CVLINK_NULL;

		/* the create vector, NOTE: speculative copy on write storage provided
		   for fast recovery during wrong path execute (see tracer_recover() for
		   details on this process */
		BITMAP_TYPE(MD_TOTAL_REGS, use_spec_cv);
		struct CV_link create_vector[MD_TOTAL_REGS];
		struct CV_link spec_create_vector[MD_TOTAL_REGS];

		/* these arrays shadow the create vector an indicate when a register was
		   last created */
		tick_t create_vector_rt[MD_TOTAL_REGS];
		tick_t spec_create_vector_rt[MD_TOTAL_REGS];

		BITMAP_TYPE(MD_NUM_IREGS, use_spec_R);
		md_gpr_t spec_regs_R;

		BITMAP_TYPE(MD_NUM_FREGS, use_spec_F);
		md_fpr_t spec_regs_F;


		BITMAP_TYPE(MD_NUM_FREGS, use_spec_C);
		md_ctrl_t spec_regs_C;

		/* speculative memory hash table */
		struct spec_mem_ent *store_htable[STORE_HASH_SIZE];

		/* speculative memory hash table bucket free list */
		struct spec_mem_ent *bucket_free_list;


		/* program counter */
		md_addr_t pred_PC;
		md_addr_t recover_PC;

		/* fetch unit next fetch address */
		md_addr_t fetch_regs_PC;
		md_addr_t fetch_pred_PC;

		struct fetch_rec *fetch_data;	/* IFETCH -> DISPATCH inst queue */
		int fetch_num;			/* num entries in IF -> DIS queue */
		int fetch_tail, fetch_head;	/* head and tail pointers of queue */

		/* the last operation that ruu_dispatch() attempted to dispatch, for
		   implementing in-order issue */
		struct RS_link last_op;

		int last_inst_missed;
		int last_inst_tmissed;

		//-------------------------------------------------------------

		void ruu_init(void);
		void lsq_init(void);
		void rslink_init(int nlinks);
		void eventq_init(void);
		void readyq_init(void);
		void cv_init(void);
		void tracer_init(void);
		void fetch_init(void);

		void
		ruu_dumpent(struct RUU_station *rs,		/* ptr to RUU station */
			    int index,				/* entry index */
			    FILE *stream,			/* output stream */
			    int header);				/* print header? */

		void
		ruu_dump(FILE *stream);				/* output stream */



		void
		lsq_dump(FILE *stream);				/* output stream */


		void
		ruu_release_fu(void);

		void
		eventq_dump(FILE *stream);			/* output stream */

		void
		eventq_queue_event(struct RUU_station *rs, tick_t when);

		struct RUU_station *
		eventq_next_event(void);

		void
		readyq_dump(FILE *stream);

		void
		readyq_enqueue(struct RUU_station *rs);

		void
		cv_dump(FILE *stream);

		void
		ruu_commit(void);

		void
		ruu_recover(int branch_index);

		void
		tracer_recover(void);

		void
		ruu_writeback(void);

		void
		lsq_refresh(void);

		void
		ruu_issue(void);

		void
		rspec_dump(FILE *stream);

		enum md_fault_type
		spec_mem_access(struct mem_t *mem,		/* memory space to access */
				enum mem_cmd cmd,		/* Read or Write access cmd */
				md_addr_t addr,			/* virtual address of access */
				void *p,			/* input/output buffer */
				int nbytes);			/* number of bytes to access */

		void
		mspec_dump(FILE *stream);

		INLINE void
		ruu_link_idep(struct RUU_station *rs,		/* rs station to link */
			      int idep_num,			/* input dependence number */
			      int idep_name);			/* input register name */

		INLINE void
		ruu_install_odep(struct RUU_station *rs,	/* creating RUU station */
				 int odep_num,			/* output operand number */
				 int odep_name);			/* output register name */

		void
		ruu_dispatch(void);

		void
		ruu_fetch(void);

		//--------------------------------------------

	public:

		simplescalar *Simplescalar;

		global *GLOBAL;
		bpred *BPRED;
		options *OPTIONS;
		power *POWER;
		stats *STATS;
		memory *MEMORY;
		regs *REGS;
		loader *LOADER;
		resource *RESOURCE;
		machine *MACHINE;
		eval *EVAL;
		eio *EIOobj;
		endian *ENDIAN;
		syscalle *SYSC;
		cache *CACHE;
		misc *MISC;
		symbol *SYMBOL;
		range *RANGE;
		ptrace *PTRACE;
		libexo *LIBEXO;

		sim(simplescalar*);

		/* MSG */
		bool interrupt;

		void free_objs();

		md_fault_type sim::memshared_access(mem_cmd cmd,
											md_addr_t addr,				/* target address to access - word - unsigned int - 32bits*/
											void *host,					/* host memory address to access */
											int nbytes);

		md_addr_t IACOMPRESS(md_addr_t A);

		int ISCOMPRESS(int SZ);

		unsigned int
		mem_access_latency(int blk_sz);

		unsigned int			/* latency of block access */
		dl1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
			      md_addr_t baddr,		/* block address to access */
			      int bsize,		/* size of block to access */
			      struct cache_blk_t *blk,	/* ptr to block in upper level */
			      tick_t now);		/* time of access */

		unsigned int			/* latency of block access */
		dl2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
			      md_addr_t baddr,		/* block address to access */
			      int bsize,		/* size of block to access */
			      struct cache_blk_t *blk,	/* ptr to block in upper level */
			      tick_t now);		/* time of access */

		unsigned int			/* latency of block access */
		il1_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
			      md_addr_t baddr,		/* block address to access */
			      int bsize,		/* size of block to access */
			      struct cache_blk_t *blk,	/* ptr to block in upper level */
			      tick_t now);		/* time of access */

		unsigned int			/* latency of block access */
		il2_access_fn(enum mem_cmd cmd,		/* access cmd, Read or Write */
			      md_addr_t baddr,		/* block address to access */
			      int bsize,		/* size of block to access */
			      struct cache_blk_t *blk,	/* ptr to block in upper level */
			      tick_t now);		/* time of access */


		unsigned int			/* latency of block access */
		itlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
			       md_addr_t baddr,		/* block address to access */
			       int bsize,		/* size of block to access */
			       struct cache_blk_t *blk,	/* ptr to block in upper level */
			       tick_t now);		/* time of access */

		unsigned int			/* latency of block access */
		dtlb_access_fn(enum mem_cmd cmd,	/* access cmd, Read or Write */
			       md_addr_t baddr,	/* block address to access */
			       int bsize,		/* size of block to access */
			       struct cache_blk_t *blk,	/* ptr to block in upper level */
			       tick_t now);		/* time of access */


		/* cycle counter */
		 tick_t sim_cycle;

		 /* MSG */
		 tick_t cycle_wait_bus;
		 counter_t num_bus_access;
		 counter_t cycle_bus_busy;

		int bimod_config[1];
		//  { /* bimod tbl size */2048 };

		int twolev_config[4];
		 // { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE};


		int comb_config[1];
		//  { /* meta_table_size */1024 };

		 /* return address stack (RAS) size */
		int ras_size;

		int btb_config[2];
		 // { /* nsets */512, /* assoc */4 };

		/* instruction decode B/W (insts/cycle) */
		int ruu_decode_width;

		/* instruction issue B/W (insts/cycle) */
		int ruu_issue_width;

		/* instruction commit B/W (insts/cycle) */
		int ruu_commit_width;

		/* register update unit (RUU) size */
		int RUU_size;

		/* load/store queue (LSQ) size */
		int LSQ_size;

		/* total number of integer ALU's available */
		int res_ialu;


		/* total number of memory system ports available (to CPU) */
		int res_memport;

		/* total number of floating point ALU's available */
		int res_fpalu;

		/* total number of floating point multiplier/dividers available */
		int res_fpmult;

		/* options for Wattch */
		int data_width ;

		/* counters added for Wattch */
		counter_t rename_access;
		counter_t bpred_access;
		counter_t window_access;
		counter_t lsq_access;
		counter_t regfile_access;
		counter_t icache_access;
		counter_t dcache_access;
		counter_t dcache2_access;
		counter_t alu_access;
		counter_t ialu_access;
		counter_t falu_access;
		counter_t resultbus_access;

		counter_t window_preg_access;
		counter_t window_selection_access;
		counter_t window_wakeup_access;
		counter_t lsq_store_data_access;
		counter_t lsq_load_data_access;
		counter_t lsq_preg_access;
		counter_t lsq_wakeup_access;

		counter_t window_total_pop_count_cycle;
		counter_t window_num_pop_count_cycle;
		counter_t lsq_total_pop_count_cycle;
		counter_t lsq_num_pop_count_cycle;
		counter_t regfile_total_pop_count_cycle;
		counter_t regfile_num_pop_count_cycle;
		counter_t resultbus_total_pop_count_cycle;
		counter_t resultbus_num_pop_count_cycle;

		/* level 1 instruction cache, entry level instruction cache */
		struct cache_t *cache_il1;

		/* level 1 instruction cache */
		struct cache_t *cache_il2;

		/* level 1 data cache, entry level data cache */
		struct cache_t *cache_dl1;

		/* level 2 data cache */
		struct cache_t *cache_dl2;

		/* instruction TLB */
		struct cache_t *itlb;

		/* data TLB */
		struct cache_t *dtlb;

		//--------------------------------------
		/*
		 * main simulator interfaces, called in the following order
		 */

		/* register simulator-specific options */
		void sim_reg_options(struct opt_odb_t *odb);

		/* main() parses options next... */

		/* check simulator-specific option values */
		void sim_check_options(struct opt_odb_t *odb, int argc, char **argv);

		/* register simulator-specific statistics */
		void sim_reg_stats(struct stat_sdb_t *sdb);

		/* initialize the simulator */
		void sim_init(void);

		/* load program into simulated state */
		void sim_load_prog(char *fname, int argc, char **argv, char **envp);

		/* main() prints the option database values next... */

		/* print simulator-specific configuration information */
		void sim_aux_config(FILE *stream);

		/* start simulation, program loaded, processor precise state initialized */
		int sim_main(void);

		/* main() prints the stats database values next... */

		/* dump simulator-specific auxiliary simulator statistics */
		void sim_aux_stats(FILE *stream);

		/* un-initialize simulator-specific state */
		void sim_uninit(void);

		/* print all simulator stats */
		void
		sim_print_stats(FILE *fd);		/* output stream */

		void
		fetch_dump(FILE *stream);			/* output stream */

		// MSO: (03/06/2008) Method cycle used in the SystemC
		void cycle();

};

#endif


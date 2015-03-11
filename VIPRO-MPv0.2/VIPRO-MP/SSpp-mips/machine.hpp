#ifndef MACHINE_H
#define MACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include "host.hpp"

/*
 * This file contains various definitions needed to decode, disassemble, and
 * execute PISA (portable ISA) instructions.
 */

/* build for PISA target */
#define TARGET_PISA

/* not applicable/available, usable in most definition contexts */
#define NA		0

/*
 * target-dependent type definitions
 */

/* define MD_QUAD_ADDRS if the target requires 64-bit (quadword) addresses */
#undef MD_QUAD_ADDRS

/*
 * target-dependent memory module configuration
 */

/* physical memory page size (must be a power-of-two) */
#define MD_PAGE_SIZE		4096
#define MD_LOG_PAGE_SIZE	12

/*
 * target-dependent register file definitions, used by regs.[hc]
 */

/* number of integer registers */
#define MD_NUM_IREGS		32

/* number of floating point registers */
#define MD_NUM_FREGS		32

/* number of control registers */
#define MD_NUM_CREGS		3

/* total number of registers, excluding PC and NPC */
#define MD_TOTAL_REGS							\
  (/*int*/32 + /*fp*/32 + /*misc*/3 + /*tmp*/1 + /*mem*/1 + /*ctrl*/1)

/*
 * target-dependent instruction faults
 */

enum md_fault_type {
  md_fault_none = 0,		/* no fault */
  md_fault_access,		/* storage access fault */
  md_fault_alignment,	/* storage alignment fault */
  md_fault_overflow,	/* signed arithmetic overflow fault */
  md_fault_div0,		/* division by zero fault */
  md_fault_break,		/* BREAK instruction fault */
  md_fault_unimpl,		/* unimplemented instruction fault */
  md_fault_internal		/* internal S/W fault */
};


/* general purpose (integer) register file entry type */
typedef sword_t md_gpr_t[MD_NUM_IREGS];

/* floating point register file entry type */
typedef union {
  sword_t l[MD_NUM_FREGS];	/* integer word view */
  sfloat_t f[MD_NUM_FREGS];	/* single-precision floating point view */
  dfloat_t d[MD_NUM_FREGS/2];	/* double-prediction floating point view */
} md_fpr_t;

/* control register file contents */
typedef struct {
  sword_t hi, lo;		/* multiplier HI/LO result registers */
  int fcc;			/* floating point condition codes */
} md_ctrl_t;

/* well known registers */
enum md_reg_names {
  MD_REG_ZERO = 0,	/* zero register */
  MD_REG_GP = 28,	/* global data section pointer */
  MD_REG_SP = 29,	/* stack pointer */
  MD_REG_FP = 30	/* frame pointer */
};


/*
 * target-dependent instruction format definition
 */

/* instruction formats */
typedef struct {
  word_t a;		/* simplescalar opcode (must be unsigned) */
  word_t b;		/* simplescalar unsigned immediate fields */
} md_inst_t;


/*
 * target-dependent loader module configuration
 */

/* virtual memory segment limits */
#define MD_TEXT_BASE		0x00400000
#define MD_DATA_BASE		0x10000000
#define MD_STACK_BASE 		0x7fffc000

/* maximum size of argc+argv+envp environment */
#define MD_MAX_ENVIRON		16384


/*
 * machine.def specific definitions
 */

/* returns the opcode field value of SimpleScalar instruction INST */
#define MD_OPFIELD(INST)		(INST.a & 0xff)
#define MD_SET_OPCODE(OP, INST)	((OP) = (md_opcode) ((INST).a & 0xff))

/* largest opcode field value (currently upper 8-bit are used for pre/post-
   incr/decr operation specifiers */
#define MD_MAX_MASK		255

/* global opcode names, these are returned by the decoder (MD_OP_ENUM()) */
enum md_opcode {
  OP_NA = 0,	/* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) OP,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) OP,
#define CONNECT(OP)
#include "machine.def" // 123 inst
  OP_MAX // total = 128	/* number of opcodes + NA */
};

/* inst -> enum md_opcode mapping, use this macro to decode insts */
#define MD_OP_ENUM(MSK)		(Sim->MACHINE->md_mask2op[MSK])


/* enum md_opcode -> description string */
#define MD_OP_NAME(OP)		(md_op2name[OP])


/* enum md_opcode -> opcode operand format, used by disassembler */
#define MD_OP_FORMAT(OP)	(md_op2format[OP])


/* function unit classes, update md_fu2name if you update this definition */
enum md_fu_class {
  FUClass_NA = 0,	/* inst does not use a functional unit */
  IntALU,		/* integer ALU */
  IntMULT,		/* integer multiplier */
  IntDIV,		/* integer divider */
  FloatADD,		/* floating point adder/subtractor */
  FloatCMP,		/* floating point comparator */
  FloatCVT,		/* floating point<->integer converter */
  FloatMULT,		/* floating point multiplier */
  FloatDIV,		/* floating point divider */
  FloatSQRT,		/* floating point square root */
  RdPort,		/* memory read port */
  WrPort,		/* memory write port */
  NUM_FU_CLASSES	/* total functional unit classes */
};

/* enum md_opcode -> enum md_fu_class, used by performance simulators */
#define MD_OP_FUCLASS(OP)	(md_op2fu[OP])


/* enum md_fu_class -> description string */
#define MD_FU_NAME(FU)		(md_fu2name[FU])


/* instruction flags */
#define F_ICOMP		0x00000001	/* integer computation */
#define F_FCOMP		0x00000002	/* FP computation */
#define F_CTRL		0x00000004	/* control inst */
#define F_UNCOND	0x00000008	/*   unconditional change */
#define F_COND		0x00000010	/*   conditional change */
#define F_MEM		0x00000020	/* memory access inst */
#define F_LOAD		0x00000040	/*   load inst */
#define F_STORE		0x00000080	/*   store inst */
#define F_DISP		0x00000100	/*   displaced (R+C) addr mode */
#define F_RR		0x00000200	/*   R+R addr mode */
#define F_DIRECT	0x00000400	/*   direct addressing mode */
#define F_TRAP		0x00000800	/* traping inst */
#define F_LONGLAT	0x00001000	/* long latency inst (for sched) */
#define F_DIRJMP	0x00002000	/* direct jump */
#define F_INDIRJMP	0x00004000	/* indirect jump */
#define F_CALL		0x00008000	/* function call */
#define F_FPCOND	0x00010000	/* FP conditional branch */
#define F_IMM		0x00020000	/* instruction has immediate operand */

/* enum md_opcode -> opcode flags, used by simulators */
#define MD_OP_FLAGS(OP)		(md_op2flags[OP])


/* integer register specifiers */
#undef  RS	/* defined in /usr/include/sys/syscall.h on HPUX boxes */
#define RS		(inst.b >> 24)			/* reg source #1 */
#define RT		((inst.b >> 16) & 0xff)		/* reg source #2 */
#define RD		((inst.b >> 8) & 0xff)		/* reg dest */

/* returns shift amount field value */
#define SHAMT		(inst.b & 0xff)

/* floating point register field synonyms */
#define FS		RS
#define FT		RT
#define FD		RD

/* returns 16-bit signed immediate field value */
#define IMM		((int)((/* signed */short)(inst.b & 0xffff)))

/* returns 16-bit unsigned immediate field value */
#define UIMM		(inst.b & 0xffff)

/* returns 26-bit unsigned absolute jump target field value */
#define TARG		(inst.b & 0x3ffffff)

/* returns break code immediate field value */
#define BCODE		(inst.b & 0xfffff)

/* load/store 16-bit signed offset field value, synonym for imm field */
#define OFS		IMM		/* alias to IMM */

/* load/store base register specifier, synonym for RS field */
#define BS		RS		/* alias to rs */

/* largest signed integer */
#define MAXINT_VAL	0x7fffffff

/* check for overflow in X+Y, both signed */
#define OVER(X,Y)							\
  ((((X) > 0) && ((Y) > 0) && (MAXINT_VAL - (X) < (Y)))			\
   || (((X) < 0) && ((Y) < 0) && (-MAXINT_VAL - (X) > (Y))))

/* check for underflow in X-Y, both signed */
#define UNDER(X,Y)							\
  ((((X) > 0) && ((Y) < 0) && (MAXINT_VAL + (Y) < (X)))			\
   || (((X) < 0) && ((Y) > 0) && (-MAXINT_VAL + (Y) > (X))))

/* default target PC handling */
#ifndef SET_TPC
#define SET_TPC(PC)	(void)0
#endif /* SET_TPC */

#ifdef BYTES_BIG_ENDIAN
/* lwl/swl defs */
#define WL_SIZE(ADDR)		((ADDR) & 0x03)
#define WL_BASE(ADDR)		((ADDR) & ~0x03)
#define WL_PROT_MASK(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
#define WL_PROT_MASK1(ADDR)	(md_lr_masks[WL_SIZE(ADDR)])
#define WL_PROT_MASK2(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])

/* lwr/swr defs */
#define WR_SIZE(ADDR)		(((ADDR) & 0x03)+1)
#define WR_BASE(ADDR)		((ADDR) & ~0x03)
#define WR_PROT_MASK(ADDR)	(~(md_lr_masks[WR_SIZE(ADDR)]))
#define WR_PROT_MASK1(ADDR)	((md_lr_masks[WR_SIZE(ADDR)]))
#define WR_PROT_MASK2(ADDR)	(md_lr_masks[4-WR_SIZE(ADDR)])
#else /* BYTES_LITTLE_ENDIAN */
/* lwl/swl defs */
#define WL_SIZE(ADDR)		(4-((ADDR) & 0x03))
#define WL_BASE(ADDR)		((ADDR) & ~0x03)
#define WL_PROT_MASK(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])
#define WL_PROT_MASK1(ADDR)	(md_lr_masks[WL_SIZE(ADDR)])
#define WL_PROT_MASK2(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])

/* lwr/swr defs */
#define WR_SIZE(ADDR)		(((ADDR) & 0x03)+1)
#define WR_BASE(ADDR)		((ADDR) & ~0x03)
#define WR_PROT_MASK(ADDR)	(~(md_lr_masks[WR_SIZE(ADDR)]))
#define WR_PROT_MASK1(ADDR)	((md_lr_masks[WR_SIZE(ADDR)]))
#define WR_PROT_MASK2(ADDR)	(md_lr_masks[4-WR_SIZE(ADDR)])
#endif


#if 0
/* lwl/swl defs */
#define WL_SIZE(ADDR)       (4-((ADDR) & 0x03))
#define WL_BASE(ADDR)       ((ADDR) & ~0x03)
#define WL_PROT_MASK(ADDR)  (md_lr_masks[4-WL_SIZE(ADDR)])

/* lwr/swr defs */
#define WR_SIZE(ADDR)       (((ADDR) & 0x03)+1)
#define WR_BASE(ADDR)       ((ADDR) & ~0x03)
#define WR_PROT_MASK(ADDR)  (~(md_lr_masks[WR_SIZE(ADDR)]))
/* #else */
/* lwl/swl stuff */
#define WL_SIZE(ADDR)		((ADDR) & 0x03)
#define WL_BASE(ADDR)		((ADDR) & ~0x03)
#define WL_PROT_MASK1(ADDR)	(md_lr_masks[WL_SIZE(ADDR)])
#define WL_PROT_MASK2(ADDR)	(md_lr_masks[4-WL_SIZE(ADDR)])

/* lwr/swr stuff */
#define WR_SIZE(ADDR)		(((ADDR) & 0x03)+1)
#define WR_BASE(ADDR)		((ADDR) & ~0x03)
#define WR_PROT_MASK1(ADDR)	((md_lr_masks[WR_SIZE(ADDR)]))
#define WR_PROT_MASK2(ADDR)	(md_lr_masks[4-WR_SIZE(ADDR)])
#endif


/*
 * various other helper macros/functions
 */

/* non-zero if system call is an exit() */
#define	SS_SYS_exit			1
#define MD_EXIT_SYSCALL(REGS)		((REGS)->regs_R[2] == SS_SYS_exit)

/* non-zero if system call is a write to stdout/stderr */
#define	SS_SYS_write		4
#define MD_OUTPUT_SYSCALL(REGS)						\
  ((REGS)->regs_R[2] == SS_SYS_write					\
   && ((REGS)->regs_R[4] == /* stdout */1				\
       || (REGS)->regs_R[4] == /* stderr */2))

/* returns stream of an output system call, translated to host */
#define MD_STREAM_FILENO(REGS)		((REGS)->regs_R[4])

/* returns non-zero if instruction is a function call */
#define MD_IS_CALL(OP)							\
  ((MD_OP_FLAGS(OP) & (F_CTRL|F_CALL)) == (F_CTRL|F_CALL))

/* returns non-zero if instruction is a function return */
#define MD_IS_RETURN(OP)		((OP) == JR && (RS) == 31)

/* returns non-zero if instruction is an indirect jump */
#define MD_IS_INDIR(OP)			((OP) == JR)

/* addressing mode probe, enums and strings */
enum md_amode_type {
  md_amode_imm,		/* immediate addressing mode */
  md_amode_gp,		/* global data access through global pointer */
  md_amode_sp,		/* stack access through stack pointer */
  md_amode_fp,		/* stack access through frame pointer */
  md_amode_disp,	/* (reg + const) addressing */
  md_amode_rr,		/* (reg + reg) addressing */
  md_amode_NUM
};


/* addressing mode pre-probe FSM, must see all instructions */
#define MD_AMODE_PREPROBE(OP, FSM)					\
  { if ((OP) == LUI) (FSM) = (RT); }

/* compute addressing mode, only for loads/stores */
#define MD_AMODE_PROBE(AM, OP, FSM)					\
  {									\
    if (MD_OP_FLAGS(OP) & F_DISP)					\
      {									\
	if ((BS) == (FSM))						\
	  (AM) = md_amode_imm;						\
	else if ((BS) == MD_REG_GP)					\
	  (AM) = md_amode_gp;						\
	else if ((BS) == MD_REG_SP)					\
	  (AM) = md_amode_sp;						\
	else if ((BS) == MD_REG_FP) /* && bind_to_seg(addr) == seg_stack */\
	  (AM) = md_amode_fp;						\
	else								\
	  (AM) = md_amode_disp;						\
      }									\
    else if (MD_OP_FLAGS(OP) & F_RR)					\
      (AM) = md_amode_rr;						\
    else								\
      panic("cannot decode addressing mode");				\
  }

/* addressing mode pre-probe FSM, after all loads and stores */
#define MD_AMODE_POSTPROBE(FSM)						\
  { (FSM) = MD_REG_ZERO; }


/*
 * EIO package configuration/macros
 */

/* expected EIO file format */
#define MD_EIO_FILE_FORMAT		EIO_PISA_FORMAT

#define MD_MISC_REGS_TO_EXO(REGS)					\
  Sim->LIBEXO->exo_new(ec_list,							\
	  /*icnt*/Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)Sim->GLOBAL->sim_num_insn),	\
	  /*PC*/Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)(REGS)->regs_PC),	\
	  /*NPC*/Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)(REGS)->regs_NPC),	\
	  /*HI*/Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.hi),	\
	  /*LO*/Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.lo),	\
	  /*FCC*/Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.fcc),\
	  NULL)

#define MD_IREG_TO_EXO(REGS, IDX)					\
  Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)(REGS)->regs_R[IDX])

#define MD_FREG_TO_EXO(REGS, IDX)					\
  Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)(REGS)->regs_F.l[IDX])

#define MD_EXO_TO_MISC_REGS(EXO, ICNT, REGS)				\
  /* check EXO format for errors... */					\
  if (!exo								\
      || exo->ec != ec_list						\
      || !exo->as_list.head						\
      || exo->as_list.head->ec != ec_integer				\
      || !exo->as_list.head->next					\
      || exo->as_list.head->next->ec != ec_address			\
      || !exo->as_list.head->next->next					\
      || exo->as_list.head->next->next->ec != ec_address		\
      || !exo->as_list.head->next->next->next				\
      || exo->as_list.head->next->next->next->ec != ec_integer		\
      || !exo->as_list.head->next->next->next->next			\
      || exo->as_list.head->next->next->next->next->ec != ec_integer	\
      || !exo->as_list.head->next->next->next->next->next		\
      || exo->as_list.head->next->next->next->next->next->ec != ec_integer\
      || exo->as_list.head->next->next->next->next->next->next != NULL)	\
  Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO misc regs");				\
  (ICNT) = (counter_t)exo->as_list.head->as_integer.val;		\
  (REGS)->regs_PC = (md_addr_t)exo->as_list.head->next->as_integer.val;	\
  (REGS)->regs_NPC =							\
    (md_addr_t)exo->as_list.head->next->next->as_integer.val;		\
  (REGS)->regs_C.hi =							\
    (word_t)exo->as_list.head->next->next->next->as_integer.val;	\
  (REGS)->regs_C.lo =							\
    (word_t)exo->as_list.head->next->next->next->next->as_integer.val;	\
  (REGS)->regs_C.fcc =							\
    (int)exo->as_list.head->next->next->next->next->next->as_integer.val;

#define MD_EXO_TO_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] = (word_t)(EXO)->as_integer.val)

#define MD_EXO_TO_FREG(EXO, REGS, IDX)					\
  ((REGS)->regs_F.l[IDX] = (word_t)(EXO)->as_integer.val)

#define MD_EXO_CMP_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] != (sword_t)(EXO)->as_integer.val)

#define MD_FIRST_IN_REG			2
#define MD_LAST_IN_REG			7

#define MD_FIRST_OUT_REG		2
#define MD_LAST_OUT_REG			7


/*
 * configure the EXO package
 */


/* engenharia alternativa para fazer funcionar esse codigo dos infernos    */
extern struct stat_stat_t *
	stat_reg_squad(struct stat_sdb_t *sdb,	/* stat database */
		       char *name,		/* stat variable name */
		       char *desc,		/* stat variable description */
		       squad_t *var,		/* stat variable */
		       squad_t init_val,	/* stat variable initial value */
		       char *format);



/*
 * configure the stats package
 */

/* counter stats */
#ifdef HOST_HAS_QUAD
#define stat_reg_counter		STATS->stat_reg_squad
#define msc_counter			msc_squad
#define for_counter			for_squad
#else /* !HOST_HAS_QUAD */
#define stat_reg_counter		STATS->stat_reg_double
#define msc_counter			msc_double
#define for_counter			for_double
#endif /* HOST_HAS_QUAD */

/* address stats */
#define stat_reg_addr			STATS->stat_reg_uint


/*
 * configure the DLite! debugger
 */

/* register bank specifier */
enum md_reg_type {
  rt_gpr,		/* general purpose register */
  rt_lpr,		/* integer-precision floating pointer register */
  rt_fpr,		/* single-precision floating pointer register */
  rt_dpr,		/* double-precision floating pointer register */
  rt_ctrl,		/* control register */
  rt_PC,		/* program counter */
  rt_NPC,		/* next program counter */
  rt_NUM
};

/* register name specifier */
struct md_reg_names_t {
  char *str;			/* register name */
  enum md_reg_type file;	/* register file */
  int reg;			/* register index */
};



/*
 * configure sim-outorder specifics
 */

/* primitive operation used to compute addresses within pipeline */
#define MD_AGEN_OP		ADD

/* NOP operation when injected into the pipeline */
#define MD_NOP_OP		NOP



/*
 * configure branch predictors
 */

/* shift used to ignore branch address least significant bits, usually
   log2(sizeof(md_inst_t)) */
#define MD_BR_SHIFT		3	/* log2(8) */


/* preferred nop instruction definition */
md_inst_t MD_NOP_INST = { NOP, 0 };


/* enum md_opcode -> description string */
char *md_op2name[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) NAME,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NAME,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_opcode -> opcode operand format, used by disassembler */
char *md_op2format[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) OPFORM,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NULL,
#define CONNECT(OP)
#include "machine.def"
};



/* enum md_opcode -> enum md_fu_class, used by performance simulators */
enum md_fu_class md_op2fu [OP_MAX] =  {
  FUClass_NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) RES,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) FUClass_NA,
#define CONNECT(OP)
#include "machine.def"
};


/* enum md_fu_class -> description string */
char *md_fu2name[NUM_FU_CLASSES] = {
  NULL, /* NA */
  "fu-int-ALU",
  "fu-int-multiply",
  "fu-int-divide",
  "fu-FP-add/sub",
  "fu-FP-comparison",
  "fu-FP-conversion",
  "fu-FP-multiply",
  "fu-FP-divide",
  "fu-FP-sqrt",
  "rd-port",
  "wr-port"
};

/* enum md_opcode -> opcode flags, used by simulators */
unsigned int md_op2flags[OP_MAX] = {
  NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3) FLAGS,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NA,
#define CONNECT(OP)
#include "machine.def"
};

/* lwl/lwr/swl/swr masks */
word_t md_lr_masks[] = {
#ifdef BYTES_BIG_ENDIAN
  0x00000000,
  0x000000ff,
  0x0000ffff,
  0x00ffffff,
  0xffffffff,
#else
  0xffffffff,
  0x00ffffff,
  0x0000ffff,
  0x000000ff,
  0x00000000,
#endif
};

char *md_amode_str[md_amode_NUM] =
{
  "(const)",		/* immediate addressing mode */
  "(gp + const)",	/* global data access through global pointer */
  "(sp + const)",	/* stack access through stack pointer */
  "(fp + const)",	/* stack access through frame pointer */
  "(reg + const)",	/* (reg + const) addressing */
  "(reg + reg)"		/* (reg + reg) addressing */
};

/* symbolic register names, parser is case-insensitive */
struct md_reg_names_t md_reg_names[] =
{
  /* name */	/* file */	/* reg */

  /* integer register file */
  { "$r0",	rt_gpr,		0 },
  { "$zero",	rt_gpr,		0 },
  { "$r1",	rt_gpr,		1 },
  { "$r2",	rt_gpr,		2 },
  { "$r3",	rt_gpr,		3 },
  { "$r4",	rt_gpr,		4 },
  { "$r5",	rt_gpr,		5 },
  { "$r6",	rt_gpr,		6 },
  { "$r7",	rt_gpr,		7 },
  { "$r8",	rt_gpr,		8 },
  { "$r9",	rt_gpr,		9 },
  { "$r10",	rt_gpr,		10 },
  { "$r11",	rt_gpr,		11 },
  { "$r12",	rt_gpr,		12 },
  { "$r13",	rt_gpr,		13 },
  { "$r14",	rt_gpr,		14 },
  { "$r15",	rt_gpr,		15 },
  { "$r16",	rt_gpr,		16 },
  { "$r17",	rt_gpr,		17 },
  { "$r18",	rt_gpr,		18 },
  { "$r19",	rt_gpr,		19 },
  { "$r20",	rt_gpr,		20 },
  { "$r21",	rt_gpr,		21 },
  { "$r22",	rt_gpr,		22 },
  { "$r23",	rt_gpr,		23 },
  { "$r24",	rt_gpr,		24 },
  { "$r25",	rt_gpr,		25 },
  { "$r26",	rt_gpr,		26 },
  { "$r27",	rt_gpr,		27 },
  { "$r28",	rt_gpr,		28 },
  { "$gp",	rt_gpr,		28 },
  { "$r29",	rt_gpr,		29 },
  { "$sp",	rt_gpr,		29 },
  { "$r30",	rt_gpr,		30 },
  { "$fp",	rt_gpr,		30 },
  { "$r31",	rt_gpr,		31 },

  /* floating point register file - single precision */
  { "$f0",	rt_fpr,		0 },
  { "$f1",	rt_fpr,		1 },
  { "$f2",	rt_fpr,		2 },
  { "$f3",	rt_fpr,		3 },
  { "$f4",	rt_fpr,		4 },
  { "$f5",	rt_fpr,		5 },
  { "$f6",	rt_fpr,		6 },
  { "$f7",	rt_fpr,		7 },
  { "$f8",	rt_fpr,		8 },
  { "$f9",	rt_fpr,		9 },
  { "$f10",	rt_fpr,		10 },
  { "$f11",	rt_fpr,		11 },
  { "$f12",	rt_fpr,		12 },
  { "$f13",	rt_fpr,		13 },
  { "$f14",	rt_fpr,		14 },
  { "$f15",	rt_fpr,		15 },
  { "$f16",	rt_fpr,		16 },
  { "$f17",	rt_fpr,		17 },
  { "$f18",	rt_fpr,		18 },
  { "$f19",	rt_fpr,		19 },
  { "$f20",	rt_fpr,		20 },
  { "$f21",	rt_fpr,		21 },
  { "$f22",	rt_fpr,		22 },
  { "$f23",	rt_fpr,		23 },
  { "$f24",	rt_fpr,		24 },
  { "$f25",	rt_fpr,		25 },
  { "$f26",	rt_fpr,		26 },
  { "$f27",	rt_fpr,		27 },
  { "$f28",	rt_fpr,		28 },
  { "$f29",	rt_fpr,		29 },
  { "$f30",	rt_fpr,		30 },
  { "$f31",	rt_fpr,		31 },

  /* floating point register file - double precision */
  { "$d0",	rt_dpr,		0 },
  { "$d1",	rt_dpr,		1 },
  { "$d2",	rt_dpr,		2 },
  { "$d3",	rt_dpr,		3 },
  { "$d4",	rt_dpr,		4 },
  { "$d5",	rt_dpr,		5 },
  { "$d6",	rt_dpr,		6 },
  { "$d7",	rt_dpr,		7 },
  { "$d8",	rt_dpr,		8 },
  { "$d9",	rt_dpr,		9 },
  { "$d10",	rt_dpr,		10 },
  { "$d11",	rt_dpr,		11 },
  { "$d12",	rt_dpr,		12 },
  { "$d13",	rt_dpr,		13 },
  { "$d14",	rt_dpr,		14 },
  { "$d15",	rt_dpr,		15 },

  /* floating point register file - integer precision */
  { "$l0",	rt_lpr,		0 },
  { "$l1",	rt_lpr,		1 },
  { "$l2",	rt_lpr,		2 },
  { "$l3",	rt_lpr,		3 },
  { "$l4",	rt_lpr,		4 },
  { "$l5",	rt_lpr,		5 },
  { "$l6",	rt_lpr,		6 },
  { "$l7",	rt_lpr,		7 },
  { "$l8",	rt_lpr,		8 },
  { "$l9",	rt_lpr,		9 },
  { "$l10",	rt_lpr,		10 },
  { "$l11",	rt_lpr,		11 },
  { "$l12",	rt_lpr,		12 },
  { "$l13",	rt_lpr,		13 },
  { "$l14",	rt_lpr,		14 },
  { "$l15",	rt_lpr,		15 },
  { "$l16",	rt_lpr,		16 },
  { "$l17",	rt_lpr,		17 },
  { "$l18",	rt_lpr,		18 },
  { "$l19",	rt_lpr,		19 },
  { "$l20",	rt_lpr,		20 },
  { "$l21",	rt_lpr,		21 },
  { "$l22",	rt_lpr,		22 },
  { "$l23",	rt_lpr,		23 },
  { "$l24",	rt_lpr,		24 },
  { "$l25",	rt_lpr,		25 },
  { "$l26",	rt_lpr,		26 },
  { "$l27",	rt_lpr,		27 },
  { "$l28",	rt_lpr,		28 },
  { "$l29",	rt_lpr,		29 },
  { "$l30",	rt_lpr,		30 },
  { "$l31",	rt_lpr,		31 },

  /* miscellaneous registers */
  { "$hi",	rt_ctrl,	0 },
  { "$lo",	rt_ctrl,	1 },
  { "$fcc",	rt_ctrl,	2 },

  /* program counters */
  { "$pc",	rt_PC,		0 },
  { "$npc",	rt_NPC,		0 },

  /* sentinel */
  { NULL,	rt_gpr,		0 }
};

/* address type definition */
typedef word_t md_addr_t;

/* EXO pointer class */
typedef word_t exo_address_t;

/* EXO integer class, 64-bit encoding */
typedef word_t exo_integer_t;

/* EXO floating point class, 64-bit encoding */
typedef double exo_float_t;

/* default register accessor object */
struct eval_value_t;
struct regs_t;

class sim;
class misc;

class machine
{
	private:
		misc *MISC;
		sim *Sim;

	public:

		/* opcode mask -> enum md_opcodem, used by decoder (MD_OP_ENUM()) */
		enum md_opcode md_mask2op[MD_MAX_MASK+1];

		machine(sim*, misc*);

		//macro
		/* non-zero for a valid address, used to determine if speculative accesses
		   should access the DL1 data cache */
		int MD_VALID_ADDR(md_addr_t ADDR);



		/* returns a register name string */
		char *md_reg_name(enum md_reg_type rt, int reg);

		char *						/* err str, NULL for no err */
		md_reg_obj(struct regs_t *regs,			/* registers to access */
			   int is_write,			/* access type */
			   enum md_reg_type rt,			/* reg bank to probe */
			   int reg,				/* register number */
			   struct eval_value_t *val);		/* input, output */

		/* print integer REG(S) to STREAM */
		void md_print_ireg(md_gpr_t regs, int reg, FILE *stream);
		void md_print_iregs(md_gpr_t regs, FILE *stream);

		/* print floating point REG(S) to STREAM */
		void md_print_fpreg(md_fpr_t regs, int reg, FILE *stream);
		void md_print_fpregs(md_fpr_t regs, FILE *stream);

		/* print control REG(S) to STREAM */
		void md_print_creg(md_ctrl_t regs, int reg, FILE *stream);
		void md_print_cregs(md_ctrl_t regs, FILE *stream);

		/* xor checksum registers */
		word_t md_xor_regs(struct regs_t *regs);

		/*
		 * target-dependent routines
		 */

		/* intialize the inst decoder, this function builds the ISA decode tables */
		void md_init_decoder(void);

		/* disassemble an instruction */
		void
		md_print_insn(md_inst_t inst,		/* instruction to disassemble */
			      md_addr_t pc,		/* addr of inst, used for PC-rels */
			      FILE *stream);		/* output stream */

};

#endif


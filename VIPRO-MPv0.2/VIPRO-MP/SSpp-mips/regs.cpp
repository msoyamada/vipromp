

#include "regs.hpp"
#include "sim.hpp"

regs::regs(sim *S)
{
	Sim = S;
}

/* create a register file */
struct regs_t *
regs::regs_create(void)
{
  struct regs_t *regs;

  regs = (regs_t*) calloc(1, sizeof(struct regs_t));
  if (!regs)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  return regs;
}

/* initialize architected register state */
void
regs::regs_init(struct regs_t *regs)		/* register file to initialize */
{
  /* FIXME: assuming all entries should be zero... */
  memset(regs, 0, sizeof(*regs));

  /* regs->regs_R[MD_SP_INDEX] and regs->regs_PC initialized by loader... */
}




#if 0

/* floating point register file format */
union regs_FP_t {
    md_gpr_t l[MD_NUM_FREGS];			/* integer word view */
    md_SS_FLOAT_TYPE f[SS_NUM_REGS];		/* single-precision FP view */
    SS_DOUBLE_TYPE d[SS_NUM_REGS/2];		/* double-precision FP view */
};

/* floating point register file */
extern union md_regs_FP_t regs_F;

/* (signed) hi register, holds mult/div results */
extern SS_WORD_TYPE regs_HI;

/* (signed) lo register, holds mult/div results */
extern SS_WORD_TYPE regs_LO;

/* floating point condition codes */
extern int regs_FCC;

/* program counter */
extern SS_ADDR_TYPE regs_PC;

/* dump all architected register state values to output stream STREAM */
void
regs_dump(FILE *stream)		/* output stream */
{
  int i;

  /* stderr is the default output stream */
  if (!stream)
    stream = stderr;

  /* dump processor register state */
  fprintf(stream, "Processor state:\n");
  fprintf(stream, "    PC: 0x%08x\n", regs_PC);
  for (i=0; i<SS_NUM_REGS; i += 2)
    {
      fprintf(stream, "    R[%2d]: %12d/0x%08x",
	      i, regs_R[i], regs_R[i]);
      fprintf(stream, "  R[%2d]: %12d/0x%08x\n",
	      i+1, regs_R[i+1], regs_R[i+1]);
    }
  fprintf(stream, "    HI:      %10d/0x%08x  LO:      %10d/0x%08x\n",
	  regs_HI, regs_HI, regs_LO, regs_LO);
  for (i=0; i<SS_NUM_REGS; i += 2)
    {
      fprintf(stream, "    F[%2d]: %12d/0x%08x",
	      i, regs_F.l[i], regs_F.l[i]);
      fprintf(stream, "  F[%2d]: %12d/0x%08x\n",
	      i+1, regs_F.l[i+1], regs_F.l[i+1]);
    }
  fprintf(stream, "    FCC:                0x%08x\n", regs_FCC);
}

/* (signed) integer register file */
SS_WORD_TYPE regs_R[SS_NUM_REGS];

/* floating point register file */
union regs_FP regs_F;

/* (signed) hi register, holds mult/div results */
SS_WORD_TYPE regs_HI;
/* (signed) lo register, holds mult/div results */
SS_WORD_TYPE regs_LO;

/* floating point condition codes */
int regs_FCC;

/* program counter */
SS_ADDR_TYPE regs_PC;

#endif

#include "machine.hpp"
#include "eval.hpp"
#include "regs.hpp"
#include "misc.hpp"
#include "sim.hpp"


machine::machine(sim *S, misc *M)
{
	Sim = S;
	MISC = M;
	
	for (int i = 0; i < MD_MAX_MASK+1; i++)
		md_mask2op[i] = OP_NA;
	
}

int machine::MD_VALID_ADDR(md_addr_t ADDR)
{
	return (((ADDR) >= Sim->LOADER->ld_text_base && (ADDR) < (Sim->LOADER->ld_text_base + Sim->LOADER->ld_text_size))	
  		 || ((ADDR) >= Sim->LOADER->ld_data_base && (ADDR) < Sim->LOADER->ld_stack_base));
}						
  

/* returns a register name string */
char *
machine::md_reg_name(enum md_reg_type rt, int reg)
{
  int i;

  for (i=0; md_reg_names[i].str != NULL; i++)
    {
      if (md_reg_names[i].file == rt && md_reg_names[i].reg == reg)
	return md_reg_names[i].str;
    }

  /* no found... */
  return NULL;
}

char *						/* err str, NULL for no err */
machine::md_reg_obj(struct regs_t *regs,			/* registers to access */
	   int is_write,			/* access type */
	   enum md_reg_type rt,			/* reg bank to probe */
	   int reg,				/* register number */
	   struct eval_value_t *val)		/* input, output */
{
  switch (rt)
    {
    case rt_gpr:
      if (reg < 0 || reg >= MD_NUM_IREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_uint;
	  val->value.as_uint = regs->regs_R[reg];
	}
      else
	regs->regs_R[reg] = Sim->EVAL->eval_as_uint(*val);
      break;

    case rt_lpr:
      if (reg < 0 || reg >= MD_NUM_FREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_uint;
	  val->value.as_uint = regs->regs_F.l[reg];
	}
      else
	regs->regs_F.l[reg] = Sim->EVAL->eval_as_uint(*val);
      break;

    case rt_fpr:
      if (reg < 0 || reg >= MD_NUM_FREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_float;
	  val->value.as_float = regs->regs_F.f[reg];
	}
      else
	regs->regs_F.f[reg] = Sim->EVAL->eval_as_float(*val);
      break;

    case rt_dpr:
      if (reg < 0 || reg >= MD_NUM_FREGS/2)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_double;
	  val->value.as_double = regs->regs_F.d[reg];
	}
      else
	regs->regs_F.d[reg] = Sim->EVAL->eval_as_double(*val);
      break;

    case rt_ctrl:
      switch (reg)
	{
	case /* HI */0:
	  if (!is_write)
	    {
	      val->type = et_uint;
	      val->value.as_uint = regs->regs_C.hi;
	    }
	  else
	    regs->regs_C.hi = Sim->EVAL->eval_as_uint(*val);
	  break;

	case /* LO */1:
	  if (!is_write)
	    {
	      val->type = et_uint;
	      val->value.as_uint = regs->regs_C.lo;
	    }
	  else
	    regs->regs_C.lo = Sim->EVAL->eval_as_uint(*val);
	  break;

	case /* FCC */2:
	  if (!is_write)
	    {
	      val->type = et_int;
	      val->value.as_int = regs->regs_C.fcc;
	    }
	  else
	    regs->regs_C.fcc = Sim->EVAL->eval_as_uint(*val);
	  break;

	default:
	  return "register number out of range";
	}
      break;

    case rt_PC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs->regs_PC;
	}
      else
	regs->regs_PC = Sim->EVAL->eval_as_addr(*val);
      break;

    case rt_NPC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs->regs_NPC;
	}
      else
	regs->regs_NPC = Sim->EVAL->eval_as_addr(*val);
      break;

    default:
      panic("bogus register bank");
    }

  /* no error */
  return NULL;
}

/* print integer REG(S) to STREAM */
void
machine::md_print_ireg(md_gpr_t regs, int reg, FILE *stream)
{
  fprintf(stream, "%4s: %12d/0x%08x",
	  md_reg_name(rt_gpr, reg), regs[reg], regs[reg]);
}

void
machine::md_print_iregs(md_gpr_t regs, FILE *stream)
{
  int i;

  for (i=0; i < MD_NUM_IREGS; i += 2)
    {
      md_print_ireg(regs, i, stream);
      fprintf(stream, "  ");
      md_print_ireg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}

/* print floating point REGS to STREAM */
void
machine::md_print_fpreg(md_fpr_t regs, int reg, FILE *stream)
{
  fprintf(stream, "%4s: %12d/0x%08x/%f",
	  md_reg_name(rt_fpr, reg), regs.l[reg], regs.l[reg], regs.f[reg]);
  if (/* even? */!(reg & 1))
    {
      fprintf(stream, " (%4s as double: %f)",
	      md_reg_name(rt_dpr, reg/2), regs.d[reg/2]);
    }
}

void
machine::md_print_fpregs(md_fpr_t regs, FILE *stream)
{
  int i;

  /* floating point registers */
  for (i=0; i < MD_NUM_FREGS; i += 2)
    {
      md_print_fpreg(regs, i, stream);
      fprintf(stream, "\n");

      md_print_fpreg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}

void
machine::md_print_creg(md_ctrl_t regs, int reg, FILE *stream)
{
  /* index is only used to iterate over these registers, hence no enums... */
  switch (reg)
    {
    case 0:
      fprintf(stream, "HI: 0x%08x", regs.hi);
      break;

    case 1:
      fprintf(stream, "LO: 0x%08x", regs.lo);
      break;

    case 2:
      fprintf(stream, "FCC: 0x%08x", regs.fcc);
      break;

    default:
      panic("bogus control register index");
    }
}

void
machine::md_print_cregs(md_ctrl_t regs, FILE *stream)
{
  md_print_creg(regs, 0, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 1, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 2, stream);
  fprintf(stream, "\n");
}

/* xor checksum registers */
word_t
machine::md_xor_regs(struct regs_t *regs)
{
  int i;
  word_t checksum = 0;

  for (i=0; i < MD_NUM_IREGS; i++)
    checksum ^= regs->regs_R[i];

  for (i=0; i < MD_NUM_FREGS; i++)
    checksum ^= regs->regs_F.l[i];

  checksum ^= regs->regs_C.hi;
  checksum ^= regs->regs_C.lo;
  checksum ^= regs->regs_C.fcc;
  checksum ^= regs->regs_PC;
  checksum ^= regs->regs_NPC;

  return checksum;
}


/* intialize the inst decoder, this function builds the ISA decode tables */
void
machine::md_init_decoder(void)
{
  /* FIXME: CONNECT defined? */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3)		\
  if (md_mask2op[(MSK)]) fatal("doubly defined mask value");		\
  if ((MSK) > MD_MAX_MASK) fatal("mask value is too large");		\
  md_mask2op[(MSK)]=(OP);
#include "machine.def"
}

/* disassemble a SimpleScalar instruction */
void
machine::md_print_insn(md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream)		/* output stream */
{
  enum md_opcode op;

  /* use stderr as default output stream */
  if (!stream)
    stream = stderr;

  /* decode the instruction, assumes predecoded text segment */
  MD_SET_OPCODE(op, inst);

  /* disassemble the instruction */
  if (op == OP_NA || op >= OP_MAX)
    {
      /* bogus instruction */
      fprintf(stream, "<invalid inst: 0x%08x:%08x>", inst.a, inst.b);
    }
  else
    {
      char *s;

      fprintf(stream, "%-10s", MD_OP_NAME(op));

      s = MD_OP_FORMAT(op);
      while (*s) {
	switch (*s) {
	case 'd':
	  fprintf(stream, "r%d", RD);
	  break;
	case 's':
	  fprintf(stream, "r%d", RS);
	  break;
	case 't':
	  fprintf(stream, "r%d", RT);
	  break;
	case 'b':
	  fprintf(stream, "r%d", BS);
	  break;
	case 'D':
	  fprintf(stream, "f%d", FD);
	  break;
	case 'S':
	  fprintf(stream, "f%d", FS);
	  break;
	case 'T':
	  fprintf(stream, "f%d", FT);
	  break;
	case 'j':
	  fprintf(stream, "0x%x", (pc + 8 + (OFS << 2)));
	  break;
	case 'o':
	case 'i':
	  fprintf(stream, "%d", IMM);
	  break;
	case 'H':
	  fprintf(stream, "%d", SHAMT);
	  break;
	case 'u':
	  fprintf(stream, "%u", UIMM);
	  break;
	case 'U':
	  fprintf(stream, "0x%x", UIMM);
	  break;
	case 'J':
	  fprintf(stream, "0x%x", ((pc & 036000000000) | (TARG << 2)));
	  break;
	case 'B':
	  fprintf(stream, "0x%x", BCODE);
	  break;
#if 0 /* FIXME: obsolete... */
	case ')':
	  /* handle pre- or post-inc/dec */
	  if (SS_COMP_OP == SS_COMP_NOP)
	    fprintf(stream, ")");
	  else if (SS_COMP_OP == SS_COMP_POST_INC)
	    fprintf(stream, ")+");
	  else if (SS_COMP_OP == SS_COMP_POST_DEC)
	    fprintf(stream, ")-");
	  else if (SS_COMP_OP == SS_COMP_PRE_INC)
	    fprintf(stream, ")^+");
	  else if (SS_COMP_OP == SS_COMP_PRE_DEC)
	    fprintf(stream, ")^-");
	  else if (SS_COMP_OP == SS_COMP_POST_DBL_INC)
	    fprintf(stream, ")++");
	  else if (SS_COMP_OP == SS_COMP_POST_DBL_DEC)
	    fprintf(stream, ")--");
	  else if (SS_COMP_OP == SS_COMP_PRE_DBL_INC)
	    fprintf(stream, ")^++");
	  else if (SS_COMP_OP == SS_COMP_PRE_DBL_DEC)
	    fprintf(stream, ")^--");
	  else
	    panic("bogus SS_COMP_OP");
	  break;
#endif
	default:
	  /* anything unrecognized, e.g., '.' is just passed through */
	  fputc(*s, stream);
	}
	s++;
      }
    }
}


#if 0

/* INC_DEC expression step tables, they map (operation, size) -> step value,
   and speed up pre/post-incr/desc handling */

/* force a nasty address */
#define XX		0x6bababab

/* before increment */
int ss_fore_tab[/* operand size */8][/* operation */5] = {
             /* NOP   POSTI POSTD  PREI   PRED */
/* byte */    {  0,    0,    0,     1,     -1,  },
/* half */    {  0,    0,    0,     2,     -2,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* word */    {  0,    0,    0,     4,     -4,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* dword */   {  0,    0,    0,     8,     -8,  },
};

/* after increment */
int ss_aft_tab[/* operand size */8][/* operation */5] = {
             /* NOP   POSTI POSTD  PREI   PRED */
/* byte */    {  0,    1,    -1,    0,     0,   },
/* half */    {  0,    2,    -2,    0,     0,   },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* word */    {  0,    4,    -4,    0,     0,   },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* invalid */ {  XX,   XX,   XX,    XX,    XX,  },
/* dword */   {  0,    8,    -8,    0,     0,   },
};

/* LWL/LWR implementation workspace */
md_addr_t ss_lr_temp;

/* temporary variables */
md_addr_t temp_bs, temp_rd;


#endif

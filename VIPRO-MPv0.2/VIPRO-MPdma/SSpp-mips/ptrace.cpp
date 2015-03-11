#include "ptrace.hpp"
#include "sim.hpp"

ptrace::ptrace(sim* S)
{
	Sim = S;

	/* pipetrace file */
	ptrace_outfd = NULL;

	/* pipetracing is active */
	ptrace_active = FALSE;

	/* one-shot switch for pipetracing */
	ptrace_oneshot = FALSE;
}



/* main interfaces, with fast checks */
void ptrace::ptrace_newinst(unsigned int A,	/* instruction sequence number */
				 md_inst_t B,	/* new instruction */
				 md_addr_t C,		/* program counter of instruction */
				 md_addr_t D)	/* address referenced, if load/store */						
{
  if (ptrace_active) 
  	__ptrace_newinst(A,B,C,D);
}

void ptrace::ptrace_newuop(unsigned int A,	/* instruction sequence number */
				char *B,		/* new uop description */
				md_addr_t C,		/* program counter of instruction */
				md_addr_t D)	/* address referenced, if load/store */						
{ 
  if (ptrace_active) __ptrace_newuop(A,B,C,D);
}

void ptrace::ptrace_endinst(unsigned int A)
{					
  if (ptrace_active) __ptrace_endinst(A);
}


void ptrace::ptrace_newcycle(tick_t A)
{				
  if (ptrace_active) __ptrace_newcycle(A);
}

void ptrace::ptrace_newstage(unsigned int A,	/* instruction sequence number */
				  char *B,		/* pipeline stage entered */
				  unsigned int C)/* pipeline events while in stage */						
{
  if (ptrace_active) __ptrace_newstage(A,B,C);
}

  
void ptrace::ptrace_check_active(md_addr_t PC, counter_t ICNT, counter_t CYCLE)				
{  
	if ((ptrace_outfd != NULL) && (!Sim->RANGE->range_cmp_range1(&ptrace_range, PC, ICNT, CYCLE)))
	{
		if (!ptrace_oneshot)
			ptrace_active = ptrace_oneshot = TRUE;
		else
			ptrace_active = FALSE;
	}
	else 
		ptrace_active = FALSE;
}	

/* open pipeline trace */
void
ptrace::ptrace_open(char *fname,		/* output filename */
	    char *range)		/* trace range */
{
  char *errstr;

  /* parse the output range */
  if (!range)
    {
      /* no range */
      errstr = Sim->RANGE->range_parse_range(":", &ptrace_range);
      if (errstr)
	Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__,"cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = TRUE;
    }
  else
    {
      errstr = Sim->RANGE->range_parse_range(range, &ptrace_range);
      if (errstr)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = FALSE;
    }

  if (ptrace_range.start.ptype != ptrace_range.end.ptype)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"range endpoints are not of the same type");

  /* open output trace file */
  if (!fname || !strcmp(fname, "-") || !strcmp(fname, "stderr"))
    ptrace_outfd = stderr;
  else if (!strcmp(fname, "stdout"))
    ptrace_outfd = stdout;
  else
    {
      ptrace_outfd = fopen(fname, "w");
      if (!ptrace_outfd)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"cannot open pipetrace output file `%s'", fname);
    }
}

/* close pipeline trace */
void
ptrace::ptrace_close(void)
{
  if (ptrace_outfd != NULL && ptrace_outfd != stderr && ptrace_outfd != stdout)
    fclose(ptrace_outfd);
}

/* declare a new instruction */
void
ptrace::__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
		 md_inst_t inst,	/* new instruction */
		 md_addr_t pc,		/* program counter of instruction */
		 md_addr_t addr)	/* address referenced, if load/store */
{
  Sim->MISC->myfprintf(ptrace_outfd, "+ %u 0x%08p 0x%08p ", iseq, pc, addr);
  Sim->MACHINE->md_print_insn(inst, addr, ptrace_outfd);
  fprintf(ptrace_outfd, "\n");

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new uop */
void
ptrace::__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
		char *uop_desc,		/* new uop description */
		md_addr_t pc,		/* program counter of instruction */
		md_addr_t addr)		/* address referenced, if load/store */
{
  Sim->MISC->myfprintf(ptrace_outfd,
	    "+ %u 0x%08p 0x%08p [%s]\n", iseq, pc, addr, uop_desc);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare instruction retirement or squash */
void
ptrace::__ptrace_endinst(unsigned int iseq)	/* instruction sequence number */
{
  fprintf(ptrace_outfd, "- %u\n", iseq);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new cycle */
void
ptrace::__ptrace_newcycle(tick_t cycle)		/* new cycle */
{
  fprintf(ptrace_outfd, "@ %.0f\n", (double)cycle);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* indicate instruction transition to a new pipeline stage */
void
ptrace::__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents)/* pipeline events while in stage */
{
  fprintf(ptrace_outfd, "* %u %s 0x%08x\n", iseq, pstage, pevents);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

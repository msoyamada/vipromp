#include <iostream>
#include "range.hpp"
#include "sim.hpp"

namespace std { extern "C" int errno; }

range::range(sim *S)
{
	Sim = S;
}

/* parse execution position *PSTR to *POS */
char *						/* error string, or NULL */
range::range_parse_pos(char *pstr,			/* execution position string */
		struct range_pos_t *pos)	/* position return buffer */
{
  char *s, *endp;
  struct sym_sym_t *sym;
#if !defined(__CYGWIN32__)
//  extern int errno;
#endif

  /* determine position type */
  if (pstr[0] == '@')
    {
      /* address position */
      pos->ptype = pt_addr;
      s = pstr + 1;
    }
  else if (pstr[0] == '#')
    {
      /* cycle count position */
      pos->ptype = pt_cycle;
      s = pstr + 1;
    }
  else
    {
      /* inst count position */
      pos->ptype = pt_inst;
      s = pstr;
    }

  /* get position value */
  errno = 0;
  pos->pos = (counter_t)strtoul(s, &endp, /* parse base */0);
  if (!errno && !*endp)
    {
      /* good conversion */
      return NULL;
    }

  /* else, not an integer, attempt double conversion */
  errno = 0;
  pos->pos = (counter_t)strtod(s, &endp);
  if (!errno && !*endp)
    {
      /* good conversion */
      /* FIXME: ignoring decimal point!! */
      return NULL;
    }

  /* else, attempt symbol lookup */
  Sim->SYMBOL->sym_loadsyms(Sim->LOADER->ld_prog_fname, /* !locals */FALSE);
  sym = Sim->SYMBOL->sym_bind_name(s, NULL, sdb_any);
  if (sym != NULL)
    {
      pos->pos = (counter_t)sym->addr;
      return NULL;
    }

  /* else, no binding made */
  return "cannot bind execution position to a value";
}

/* print execution position *POS */
void
range::range_print_pos(struct range_pos_t *pos,	/* execution position */
		FILE *stream)			/* output stream */
{
  switch (pos->ptype)
    {
    case pt_addr:
      Sim->MISC->myfprintf(stream, "@0x%08p", (md_addr_t)pos->pos);
      break;
    case pt_inst:
      fprintf(stream, "%.0f", (double)pos->pos);
      break;
    case pt_cycle:
      fprintf(stream, "#%.0f", (double)pos->pos);
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus execution position type");
    }
}

/* parse execution range *RSTR to *RANGE */
char *						/* error string, or NULL */
range::range_parse_range(char *rstr,			/* execution range string */
		  struct range_range_t *range)	/* range return buffer */
{
  char *pos1, *pos2, *p, buf[512], *errstr;

  /* make a copy of the execution range */
  strcpy(buf, rstr);
  pos1 = buf;

  /* find mid-point */
  p = buf;
  while (*p != ':' && *p != '\0')
    {
      p++;
    }
  if (*p != ':')
    return "badly formed execution range";
  *p = '\0';

  /* this is where the second position will start */
  pos2 = p + 1;

  /* parse start position */
  if (*pos1 && *pos1 != ':')
    {
      errstr = range_parse_pos(pos1, &range->start);
      if (errstr)
	return errstr;
    }
  else
    {
      /* default start range */
      range->start.ptype = pt_inst;
      range->start.pos = 0;
    }

  /* parse end position */
  if (*pos2)
    {
      if (*pos2 == '+')
	{
	  int delta;
	  char *endp;
#if !defined(__CYGWIN32__)
	//  extern int errno;
#endif

	  /* get delta value */
	  errno = 0;
	  delta = strtol(pos2 + 1, &endp, /* parse base */0);
	  if (!errno && !*endp)
	    {
	      /* good conversion */
	      range->end.ptype = range->start.ptype;
	      range->end.pos = range->start.pos + delta;
	    }
	  else
	    {
	      /* bad conversion */
	      return "badly formed execution range delta";
	    }
	}
      else
	{
	  errstr = range_parse_pos(pos2, &range->end);
	  if (errstr)
	    return errstr;
	}
    }
  else
    {
      /* default end range */
      range->end.ptype = range->start.ptype;
#ifdef HOST_HAS_QUAD
      range->end.pos = ULL(0x7fffffffffffffff);
#else /* !__GNUC__ */
      range->end.pos = 281474976645120.0;
#endif /* __GNUC__ */
    }

  /* no error */
  return NULL;
}

/* print execution range *RANGE */
void
range::range_print_range(struct range_range_t *range,	/* execution range */
		  FILE *stream)			/* output stream */
{
  range_print_pos(&range->start, stream);
  fprintf(stream, ":");
  range_print_pos(&range->end, stream);
}

/* determine if inputs match execution position */
int						/* relation to position */
range::range_cmp_pos(struct range_pos_t *pos,		/* execution position */
	      counter_t val)			/* position value */
{
  if (val < pos->pos)
    return /* before */-1;
  else if (val == pos->pos)
    return /* equal */0;
  else /* if (pos->pos < val) */
    return /* after */1;
}

/* determine if inputs are in range */
int						/* relation to range */
range::range_cmp_range(struct range_range_t *range,	/* execution range */
		counter_t val)			/* position value */
{
  if (range->start.ptype != range->end.ptype)
    Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "invalid range");

  if (val < range->start.pos)
    return /* before */-1;
  else if (range->start.pos <= val && val <= range->end.pos)
    return /* inside */0;
  else /* if (range->end.pos < val) */
    return /* after */1;
}

/* determine if inputs are in range, passes all possible info needed */
int						/* relation to range */
range::range_cmp_range1(struct range_range_t *range,	/* execution range */
		 md_addr_t addr,		/* address value */
		 counter_t icount,		/* instruction count */
		 counter_t cycle)		/* cycle count */
{
  if (range->start.ptype != range->end.ptype)
    Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "invalid range");

  switch (range->start.ptype)
    {
    case pt_addr:
      if (addr < (md_addr_t)range->start.pos)
	return /* before */-1;
      else if ((md_addr_t)range->start.pos <= addr && addr <= (md_addr_t)range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < addr) */
	return /* after */1;
      break;
    case pt_inst:
      if (icount < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= icount && icount <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < icount) */
	return /* after */1;
      break;
    case pt_cycle:
      if (cycle < range->start.pos)
	return /* before */-1;
      else if (range->start.pos <= cycle && cycle <= range->end.pos)
	return /* inside */0;
      else /* if (range->end.pos < cycle) */
	return /* after */1;
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus range type");
    }
}

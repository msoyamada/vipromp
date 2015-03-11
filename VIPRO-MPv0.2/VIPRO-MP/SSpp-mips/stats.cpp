
#include "stats.hpp"
#include "sim.hpp"
#include "misc.hpp"

stats::stats(sim *S)
{
	Sim = S;
}


/* compare two indicies in a sparse array hash table, used by qsort() */
int
compare_fn(const void *p1, const void *p2)
{
  struct bucket_t **pb1 = (struct bucket_t **)p1, 
  		  **pb2 = (struct bucket_t **)p2;

  /* compare indices */
  if ((*pb1)->index < (*pb2)->index)
    return -1;
  else if ((*pb1)->index > (*pb2)->index)
    return 1;
  else /* ((*pb1)->index == (*pb2)->index) */
    return 0;
}


/* evaluate a stat as an expression */
struct eval_value_t
stats::stat_eval_ident(struct eval_state_t *es)/* an expression evaluator */
{
  struct stat_sdb_t *sdb = (struct stat_sdb_t *) es->user_ptr;
  struct stat_stat_t *stat;
  static struct eval_value_t err_value = { et_int, { 0 } };
  struct eval_value_t val;

  /* locate the stat variable */
  for (stat = sdb->stats; stat != NULL; stat = stat->next)
    {
      if (!strcmp(stat->name, es->tok_buf))
	{
	  /* found it! */
	  break;
	}
    }
  if (!stat)
    {
      /* could not find stat variable */
      Sim->EVAL->eval_error = ERR_UNDEFVAR;
      return err_value;
    }
  /* else, return the value of stat */

  /* convert the stat variable value to a typed expression value */
  switch (stat->sc)
    {
    case msc_int:
      val.type = et_int;
      val.value.as_int = *stat->variant.for_int.var;
      break;
    case msc_uint:
      val.type = et_uint;
      val.value.as_uint = *stat->variant.for_uint.var;
      break;
#ifdef HOST_HAS_QUAD
    case msc_quad:
      /* FIXME: cast to double, eval package doesn't support long long's */
      val.type = et_double;
#ifdef _MSC_VER /* FIXME: MSC does not implement quad_t to double conversion */
      val.value.as_double = (double)(squad_t)*stat->variant.for_quad.var;
#else /* !_MSC_VER */
      val.value.as_double = (double)*stat->variant.for_quad.var;
#endif /* _MSC_VER */
      break;
    case msc_squad:
      /* FIXME: cast to double, eval package doesn't support long long's */
      val.type = et_double;
      val.value.as_double = (double)*stat->variant.for_squad.var;
      break;
#endif /* HOST_HAS_QUAD */
    case msc_float:
      val.type = et_float;
      val.value.as_float = *stat->variant.for_float.var;
      break;
    case msc_double:
      val.type = et_double;
      val.value.as_double = *stat->variant.for_double.var;
      break;
    case msc_dist:
    case msc_sdist:
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "stat distributions not allowed in formula expressions");
      break;
    case msc_formula:
      {
	/* instantiate a new evaluator to avoid recursion problems */
	struct eval_state_t *es = Sim->EVAL->eval_new(/*stat_eval_ident,*/ sdb);
	char *endp;

	val = Sim->EVAL->eval_expr(es, stat->variant.for_formula.formula, &endp);
	if (Sim->EVAL->eval_error != ERR_NOERR || *endp != '\0')
	  {
	    /* pass through eval_error */
	    val = err_value;
	  }
	/* else, use value returned */
	Sim->EVAL->eval_delete(es);
      }
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, __FILE__, __FUNCTION__, __LINE__, "bogus stat class");
    }

  return val;
}

/* create a new stats database */
struct stat_sdb_t *
stats::stat_new(void)
{
  struct stat_sdb_t *sdb;

  sdb = (struct stat_sdb_t *)calloc(1, sizeof(struct stat_sdb_t));
  if (!sdb)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  sdb->stats = NULL;
  sdb->evaluator = Sim->EVAL->eval_new(/*stat_eval_ident,*/ sdb);

  return sdb;
}

/* delete a stats database */
void
stats::stat_delete(struct stat_sdb_t *sdb)	/* stats database */
{
  int i;
  struct stat_stat_t *stat, *stat_next;
  struct bucket_t *bucket, *bucket_next;

  /* free all individual stat variables */
  for (stat = sdb->stats; stat != NULL; stat = stat_next)
    {
      stat_next = stat->next;
      stat->next = NULL;

      /* free stat */
      switch (stat->sc)
	{
	case msc_int:
	case msc_uint:
#ifdef HOST_HAS_QUAD
	case msc_quad:
	case msc_squad:
#endif /* HOST_HAS_QUAD */
	case msc_float:
	case msc_double:
	case msc_formula:
	  /* no other storage to deallocate */
	  break;
	case msc_dist:
	  /* free distribution array */
	  free(stat->variant.for_dist.arr);
	  stat->variant.for_dist.arr = NULL;
	  break;
	case msc_sdist:
	  /* free all hash table buckets */
	  for (i=0; i<HTAB_SZ; i++)
	    {
	      for (bucket = stat->variant.for_sdist.sarr[i];
		   bucket != NULL;
		   bucket = bucket_next)
		{
		  bucket_next = bucket->next;
		  bucket->next = NULL;
		  free(bucket);
		}
	      stat->variant.for_sdist.sarr[i] = NULL;
	    }
	  /* free hash table array */
	  free(stat->variant.for_sdist.sarr);
	  stat->variant.for_sdist.sarr = NULL;
	  break;
	default:
	  Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus stat class");
	}
      /* free stat variable record */
      free(stat);
    }
  sdb->stats = NULL;
  Sim->EVAL->eval_delete(sdb->evaluator);
  sdb->evaluator = NULL;
  free(sdb);
}

/* add stat variable STAT to stat database SDB */
void
stats::add_stat(struct stat_sdb_t *sdb,	/* stat database */
	 struct stat_stat_t *stat)	/* stat variable */
{
  struct stat_stat_t *elt, *prev;

  /* append at end of stat database list */
  for (prev=NULL, elt=sdb->stats; elt != NULL; prev=elt, elt=elt->next)
    /* nada */;

  /* append stat to stats chain */
  if (prev != NULL)
    prev->next = stat;
  else /* prev == NULL */
    sdb->stats = stat;
  stat->next = NULL;
}

/* register an integer statistical variable */
struct stat_stat_t *
stats::stat_reg_int(struct stat_sdb_t *sdb,	/* stat database */
	     char *name,		/* stat variable name */
	     char *desc,		/* stat variable description */
	     int *var,			/* stat variable */
	     int init_val,		/* stat variable initial value */
	     char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format = format : stat->format = "%12d";
  stat->sc = msc_int;
  stat->variant.for_int.var = var;
  stat->variant.for_int.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}

/* register an unsigned integer statistical variable */
struct stat_stat_t *
stats::stat_reg_uint(struct stat_sdb_t *sdb,	/* stat database */
	      char *name,		/* stat variable name */
	      char *desc,		/* stat variable description */
	      unsigned int *var,	/* stat variable */
	      unsigned int init_val,	/* stat variable initial value */
	      char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format = format : stat->format = "%12u";
  stat->sc = msc_uint;
  stat->variant.for_uint.var = var;
  stat->variant.for_uint.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}

#ifdef HOST_HAS_QUAD
/* register a quadword integer statistical variable */
struct stat_stat_t *
stats::stat_reg_quad(struct stat_sdb_t *sdb,	/* stat database */
	      char *name,		/* stat variable name */
	      char *desc,		/* stat variable description */
	      quad_t *var,		/* stat variable */
	      quad_t init_val,		/* stat variable initial value */
	      char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format = "%12lu";
  stat->sc = msc_quad;
  stat->variant.for_quad.var = var;
  stat->variant.for_quad.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}

/* register a signed quadword integer statistical variable */
struct stat_stat_t *
stats::stat_reg_squad(struct stat_sdb_t *sdb,	/* stat database */
	       char *name,		/* stat variable name */
	       char *desc,		/* stat variable description */
	       squad_t *var,		/* stat variable */
	       squad_t init_val,	/* stat variable initial value */
	       char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  "%12ld";
  stat->sc = msc_squad;
  stat->variant.for_squad.var = var;
  stat->variant.for_squad.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}
#endif /* HOST_HAS_QUAD */

/* register a float statistical variable */
struct stat_stat_t *
stats::stat_reg_float(struct stat_sdb_t *sdb,	/* stat database */
	       char *name,		/* stat variable name */
	       char *desc,		/* stat variable description */
	       float *var,		/* stat variable */
	       float init_val,		/* stat variable initial value */
	       char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  "%12.4f";
  stat->sc = msc_float;
  stat->variant.for_float.var = var;
  stat->variant.for_float.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}

/* register a double statistical variable */
struct stat_stat_t *
stats::stat_reg_double(struct stat_sdb_t *sdb,	/* stat database */
		char *name,		/* stat variable name */
		char *desc,		/* stat variable description */
		double *var,		/* stat variable */
		double init_val,	/* stat variable initial value */
		char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  "%12.4f";
  stat->sc = msc_double;
  stat->variant.for_double.var = var;
  stat->variant.for_double.init_val = init_val;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  *var = init_val;

  return stat;
}

/* create an array distribution (w/ fixed size buckets) in stat database SDB,
   the array distribution has ARR_SZ buckets with BUCKET_SZ indicies in each
   bucked, PF specifies the distribution components to print for optional
   format FORMAT; the indicies may be optionally replaced with the strings from
   IMAP, or the entire distribution can be printed with the optional
   user-specified print function PRINT_FN */
struct stat_stat_t *
stats::stat_reg_dist(struct stat_sdb_t *sdb,	/* stat database */
	      char *name,		/* stat variable name */
	      char *desc,		/* stat variable description */
	      unsigned int init_val,	/* dist initial value */
	      unsigned int arr_sz,	/* array size */
	      unsigned int bucket_sz,	/* array bucket size */
	      int pf,			/* print format, use PF_* defs */
	      char *format,		/* optional variable output format */
	      char **imap,		/* optional index -> string map */
	      print_fn_t print_fn)	/* optional user print function */
{
  unsigned int i;
  struct stat_stat_t *stat;
  unsigned int *arr;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  NULL;
  stat->sc = msc_dist;
  stat->variant.for_dist.init_val = init_val;
  stat->variant.for_dist.arr_sz = arr_sz;
  stat->variant.for_dist.bucket_sz = bucket_sz;
  stat->variant.for_dist.pf = pf;
  stat->variant.for_dist.imap = imap;
  stat->variant.for_dist.print_fn = print_fn;
  stat->variant.for_dist.overflows = 0;

  arr = (unsigned int *)calloc(arr_sz, sizeof(unsigned int));
  if (!arr)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");
  stat->variant.for_dist.arr = arr;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  /* initialize stat */
  for (i=0; i < arr_sz; i++)
    arr[i] = init_val;

  return stat;
}

/* create a sparse array distribution in stat database SDB, while the sparse
   array consumes more memory per bucket than an array distribution, it can
   efficiently map any number of indicies from 0 to 2^32-1, PF specifies the
   distribution components to print for optional format FORMAT; the indicies
   may be optionally replaced with the strings from IMAP, or the entire
   distribution can be printed with the optional user-specified print function
   PRINT_FN */
struct stat_stat_t *
stats::stat_reg_sdist(struct stat_sdb_t *sdb,	/* stat database */
	       char *name,		/* stat variable name */
	       char *desc,		/* stat variable description */
	       unsigned int init_val,	/* dist initial value */
	       int pf,			/* print format, use PF_* defs */
	       char *format,		/* optional variable output format */
	       print_fn_t print_fn)	/* optional user print function */
{
  struct stat_stat_t *stat;
  struct bucket_t **sarr;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  NULL;
  stat->sc = msc_sdist;
  stat->variant.for_sdist.init_val = init_val;
  stat->variant.for_sdist.pf = pf;
  stat->variant.for_sdist.print_fn = print_fn;

  /* allocate hash table */
  sarr = (struct bucket_t **)calloc(HTAB_SZ, sizeof(struct bucket_t *));
  if (!sarr)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");
  stat->variant.for_sdist.sarr = sarr;

  /* link onto SDB chain */
  add_stat(sdb, stat);

  return stat;
}

/* add NSAMPLES to array or sparse array distribution STAT */
void
stats::stat_add_samples(struct stat_stat_t *stat,/* stat database */
		 md_addr_t index,	/* distribution index of samples */
		 int nsamples)		/* number of samples to add to dist */
{
  switch (stat->sc)
    {
    case msc_dist:
      {
	unsigned int i;

	/* compute array index */
	i = index / stat->variant.for_dist.bucket_sz;

	/* check for overflow */
	if (i >= stat->variant.for_dist.arr_sz)
	  stat->variant.for_dist.overflows += nsamples;
	else
	  stat->variant.for_dist.arr[i] += nsamples;
      }
      break;
    case msc_sdist:
      {
	struct bucket_t *bucket;
	int hash = HTAB_HASH(index);

	if (hash < 0 || hash >= HTAB_SZ)
	  Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "hash table index overflow");

	/* find bucket */
	for (bucket = stat->variant.for_sdist.sarr[hash];
	     bucket != NULL;
	     bucket = bucket->next)
	  {
	    if (bucket->index == index)
	      break;
	  }
	if (!bucket)
	  {
	    /* add a new sample bucket */
	    bucket = (struct bucket_t *)calloc(1, sizeof(struct bucket_t));
	    if (!bucket)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");
	    bucket->next = stat->variant.for_sdist.sarr[hash];
	    stat->variant.for_sdist.sarr[hash] = bucket;
	    bucket->index = index;
	    bucket->count = stat->variant.for_sdist.init_val;
	  }
	bucket->count += nsamples;
      }
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "stat variable is not an array distribution");
    }
}

/* add a single sample to array or sparse array distribution STAT */
void
stats::stat_add_sample(struct stat_stat_t *stat,/* stat variable */
		md_addr_t index)	/* index of sample */
{
  stat_add_samples(stat, index, 1);
}

/* register a double statistical formula, the formula is evaluated when the
   statistic is printed, the formula expression may reference any registered
   statistical variable and, in addition, the standard operators '(', ')', '+',
   '-', '*', and '/', and literal (i.e., C-format decimal, hexidecimal, and
   octal) constants are also supported; NOTE: all terms are immediately
   converted to double values and the result is a double value, see eval.h
   for more information on formulas */
struct stat_stat_t *
stats::stat_reg_formula(struct stat_sdb_t *sdb,/* stat database */
		 char *name,		/* stat variable name */
		 char *desc,		/* stat variable description */
		 char *formula,		/* formula expression */
		 char *format)		/* optional variable output format */
{
  struct stat_stat_t *stat;

  stat = (struct stat_stat_t *)calloc(1, sizeof(struct stat_stat_t));
  if (!stat)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

  stat->name = Sim->MISC->mystrdup(name);
  stat->desc = Sim->MISC->mystrdup(desc);
  format ? stat->format =  format : stat->format =  "%12.4f";
  stat->sc = msc_formula;
  stat->variant.for_formula.formula = Sim->MISC->mystrdup(formula);

  /* link onto SDB chain */
  add_stat(sdb, stat);

  return stat;
}

/* print an array distribution */
void
stats::print_dist(struct stat_stat_t *stat,	/* stat variable */
	   FILE *fd)			/* output stream */
{
  unsigned int i, bcount, imax, imin;
  double btotal, bsum, bvar, bavg, bsqsum;
  int pf = stat->variant.for_dist.pf;

  /* count and sum entries */
  bcount = 0; btotal = 0.0; bvar = 0.0; bsqsum = 0.0;
  imax = 0; imin = UINT_MAX;
  for (i=0; i<stat->variant.for_dist.arr_sz; i++)
    {
      bcount++;
      btotal += stat->variant.for_dist.arr[i];
      /* on-line variance computation, tres cool, no!?! */
      bsqsum += ((double)stat->variant.for_dist.arr[i] *
		 (double)stat->variant.for_dist.arr[i]);
      bavg = btotal / MAX((double)bcount, 1.0);
      bvar = (bsqsum - ((double)bcount * bavg * bavg)) / 
	(double)(((bcount - 1) > 0) ? (bcount - 1) : 1);
    }

  /* print header */
  fprintf(fd, "\n");
  fprintf(fd, "%-22s # %s\n", stat->name, stat->desc);
  fprintf(fd, "%s.array_size = %u\n",
	  stat->name, stat->variant.for_dist.arr_sz);
  fprintf(fd, "%s.bucket_size = %u\n",
	  stat->name, stat->variant.for_dist.bucket_sz);

  fprintf(fd, "%s.count = %u\n", stat->name, bcount);
  fprintf(fd, "%s.total = %.0f\n", stat->name, btotal);
  if (bcount > 0)
    {
      fprintf(fd, "%s.imin = %u\n", stat->name, 0U);
      fprintf(fd, "%s.imax = %u\n", stat->name, bcount);
    }
  else
    {
      fprintf(fd, "%s.imin = %d\n", stat->name, -1);
      fprintf(fd, "%s.imax = %d\n", stat->name, -1);
    }
  fprintf(fd, "%s.average = %8.4f\n", stat->name, btotal/MAX(bcount, 1.0));
  fprintf(fd, "%s.std_dev = %8.4f\n", stat->name, sqrt(bvar));
  fprintf(fd, "%s.overflows = %u\n",
	  stat->name, stat->variant.for_dist.overflows);

  fprintf(fd, "# pdf == prob dist fn, cdf == cumulative dist fn\n");
  fprintf(fd, "# %14s ", "index");
  if (pf & PF_COUNT)
    fprintf(fd, "%10s ", "count");
  if (pf & PF_PDF)
    fprintf(fd, "%6s ", "pdf");
  if (pf & PF_CDF)
    fprintf(fd, "%6s ", "cdf");
  fprintf(fd, "\n");

  fprintf(fd, "%s.start_dist\n", stat->name);

  if (bcount > 0)
    {
      /* print the array */
      bsum = 0.0;
      for (i=0; i<bcount; i++)
	{
	  bsum += (double)stat->variant.for_dist.arr[i];
	  if (stat->variant.for_dist.print_fn)
	    {
	      stat->variant.for_dist.print_fn(stat,
					      i,
					      stat->variant.for_dist.arr[i],
					      bsum,
					      btotal);
	    }
	  else
	    {
	      if (stat->format == NULL)
		{
		  if (stat->variant.for_dist.imap)
		    fprintf(fd, "%-16s ", stat->variant.for_dist.imap[i]);
		  else
		    fprintf(fd, "%16u ",
			    i * stat->variant.for_dist.bucket_sz);
		  if (pf & PF_COUNT)
		    fprintf(fd, "%10u ", stat->variant.for_dist.arr[i]);
		  if (pf & PF_PDF)
		    fprintf(fd, "%6.2f ",
			    (double)stat->variant.for_dist.arr[i] /
			    MAX(btotal, 1.0) * 100.0);
		  if (pf & PF_CDF)
		    fprintf(fd, "%6.2f ", bsum/MAX(btotal, 1.0) * 100.0);
		}
	      else
		{
		  if (pf == (PF_COUNT|PF_PDF|PF_CDF))
		    {
		      if (stat->variant.for_dist.imap)
		        fprintf(fd, stat->format,
			        stat->variant.for_dist.imap[i],
			        stat->variant.for_dist.arr[i],
			        (double)stat->variant.for_dist.arr[i] /
			        MAX(btotal, 1.0) * 100.0,
			        bsum/MAX(btotal, 1.0) * 100.0);
		      else
		        fprintf(fd, stat->format,
			        i * stat->variant.for_dist.bucket_sz,
			        stat->variant.for_dist.arr[i],
			        (double)stat->variant.for_dist.arr[i] /
			        MAX(btotal, 1.0) * 100.0,
			        bsum/MAX(btotal, 1.0) * 100.0);
		    }
		  else
		    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "distribution format not yet implemented");
		}
	      fprintf(fd, "\n");
	    }
	}
    }

  fprintf(fd, "%s.end_dist\n", stat->name);
}

/* print a sparse array distribution */
void
stats::print_sdist(struct stat_stat_t *stat,	/* stat variable */
	    FILE *fd)			/* output stream */
{
  unsigned int i, bcount;
  md_addr_t imax, imin;
  double btotal, bsum, bvar, bavg, bsqsum;
  struct bucket_t *bucket;
  int pf = stat->variant.for_sdist.pf;

  /* count and sum entries */
  bcount = 0; btotal = 0.0; bvar = 0.0; bsqsum = 0.0;
  imax = 0; imin = UINT_MAX;
  for (i=0; i<HTAB_SZ; i++)
    {
      for (bucket = stat->variant.for_sdist.sarr[i];
	   bucket != NULL;
	   bucket = bucket->next)
	{
	  bcount++;
	  btotal += bucket->count;
	  /* on-line variance computation, tres cool, no!?! */
	  bsqsum += ((double)bucket->count * (double)bucket->count);
	  bavg = btotal / (double)bcount;
	  bvar = (bsqsum - ((double)bcount * bavg * bavg)) / 
	    (double)(((bcount - 1) > 0) ? (bcount - 1) : 1);
	  if (bucket->index < imin)
	    imin = bucket->index;
	  if (bucket->index > imax)
	    imax = bucket->index;
	}
    }

  /* print header */
  fprintf(fd, "\n");
  fprintf(fd, "%-22s # %s\n", stat->name, stat->desc);
  fprintf(fd, "%s.count = %u\n", stat->name, bcount);
  fprintf(fd, "%s.total = %.0f\n", stat->name, btotal);
  if (bcount > 0)
    {
      Sim->MISC->myfprintf(fd, "%s.imin = 0x%p\n", stat->name, imin);
      Sim->MISC->myfprintf(fd, "%s.imax = 0x%p\n", stat->name, imax);
    }
  else
    {
      fprintf(fd, "%s.imin = %d\n", stat->name, -1);
      fprintf(fd, "%s.imax = %d\n", stat->name, -1);
    }
  fprintf(fd, "%s.average = %8.4f\n", stat->name, btotal/bcount);
  fprintf(fd, "%s.std_dev = %8.4f\n", stat->name, sqrt(bvar));
  fprintf(fd, "%s.overflows = 0\n", stat->name);

  fprintf(fd, "# pdf == prob dist fn, cdf == cumulative dist fn\n");
  fprintf(fd, "# %14s ", "index");
  if (pf & PF_COUNT)
    fprintf(fd, "%10s ", "count");
  if (pf & PF_PDF)
    fprintf(fd, "%6s ", "pdf");
  if (pf & PF_CDF)
    fprintf(fd, "%6s ", "cdf");
  fprintf(fd, "\n");

  fprintf(fd, "%s.start_dist\n", stat->name);

  if (bcount > 0)
    {
      unsigned int bindex;
      struct bucket_t **barr;

      /* collect all buckets */
      barr = (struct bucket_t **)calloc(bcount, sizeof(struct bucket_t *));
      if (!barr)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");
      for (bindex=0,i=0; i<HTAB_SZ; i++)
	{
	  for (bucket = stat->variant.for_sdist.sarr[i];
	       bucket != NULL;
	       bucket = bucket->next)
	    {
	      barr[bindex++] = bucket;
	    }
	}

      /* sort the array by index */
      qsort(barr, bcount, sizeof(struct bucket_t *), compare_fn);

      /* print the array */
      bsum = 0.0;
      for (i=0; i<bcount; i++)
	{
	  bsum += (double)barr[i]->count;
	  if (stat->variant.for_sdist.print_fn)
	    {
	      stat->variant.for_sdist.print_fn(stat,
					       barr[i]->index,
					       barr[i]->count,
					       bsum,
					       btotal);
	    }
	  else
	    {
	      if (stat->format == NULL)
		{
		  Sim->MISC->myfprintf(fd, "0x%p ", barr[i]->index);
		  if (pf & PF_COUNT)
		    fprintf(fd, "%10u ", barr[i]->count);
		  if (pf & PF_PDF)
		    fprintf(fd, "%6.2f ",
			    (double)barr[i]->count/MAX(btotal, 1.0) * 100.0);
		  if (pf & PF_CDF)
		    fprintf(fd, "%6.2f ", bsum/MAX(btotal, 1.0) * 100.0);
		}
	      else
		{
		  if (pf == (PF_COUNT|PF_PDF|PF_CDF))
		    {
		      Sim->MISC->myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count,
				(double)barr[i]->count/MAX(btotal, 1.0)*100.0,
				bsum/MAX(btotal, 1.0) * 100.0);
		    }
		  else if (pf == (PF_COUNT|PF_PDF))
		    {
		      Sim->MISC->myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count,
				(double)barr[i]->count/MAX(btotal, 1.0)*100.0);
		    }
		  else if (pf == PF_COUNT)
		    {
		      Sim->MISC->myfprintf(fd, stat->format,
				barr[i]->index, barr[i]->count);
		    }
		  else
		    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "distribution format not yet implemented");
		}
	      fprintf(fd, "\n");
	    }
	}

      /* all done, release bucket pointer array */
      free(barr);
    }

  fprintf(fd, "%s.end_dist\n", stat->name);
}

/* print the value of stat variable STAT */
void
stats::stat_print_stat(struct stat_sdb_t *sdb,	/* stat database */
		struct stat_stat_t *stat,/* stat variable */
		FILE *fd)		/* output stream */
{
  struct eval_value_t val;

  switch (stat->sc)
    {
    case msc_int:
      fprintf(fd, "%-22s ", stat->name);
      Sim->MISC->myfprintf(fd, stat->format, *stat->variant.for_int.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case msc_uint:
      fprintf(fd, "%-22s ", stat->name);
      Sim->MISC->myfprintf(fd, stat->format, *stat->variant.for_uint.var);
      fprintf(fd, " # %s", stat->desc);
      break;
#ifdef HOST_HAS_QUAD
    case msc_quad:
      {
	char buf[128];

	fprintf(fd, "%-22s ", stat->name);
	Sim->MISC->mysprintf(buf, stat->format, *stat->variant.for_quad.var);
	fprintf(fd, "%s # %s", buf, stat->desc);
      }
      break;
    case msc_squad:
      {
	char buf[128];

	fprintf(fd, "%-22s ", stat->name);
	Sim->MISC->mysprintf(buf, stat->format, *stat->variant.for_squad.var);
	fprintf(fd, "%s # %s", buf, stat->desc);
      }
      break;
#endif /* HOST_HAS_QUAD */
    case msc_float:
      fprintf(fd, "%-22s ", stat->name);
      Sim->MISC->myfprintf(fd, stat->format, (double)*stat->variant.for_float.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case msc_double:
      fprintf(fd, "%-22s ", stat->name);
      Sim->MISC->myfprintf(fd, stat->format, *stat->variant.for_double.var);
      fprintf(fd, " # %s", stat->desc);
      break;
    case msc_dist:
      print_dist(stat, fd);
      break;
    case msc_sdist:
      print_sdist(stat, fd);
      break;
    case msc_formula:
      {
	/* instantiate a new evaluator to avoid recursion problems */
	struct eval_state_t *es = Sim->EVAL->eval_new(/*stat_eval_ident,*/ sdb);
	char *endp;

	fprintf(fd, "%-22s ", stat->name);
	val = Sim->EVAL->eval_expr(es, stat->variant.for_formula.formula, &endp);
	if (Sim->EVAL->eval_error != ERR_NOERR || *endp != '\0')
	  fprintf(fd, "<error: %s>", eval_err_str[Sim->EVAL->eval_error]);
	else
	  Sim->MISC->myfprintf(fd, stat->format, Sim->EVAL->eval_as_double(val));
	fprintf(fd, " # %s", stat->desc);

	/* done with the evaluator */
	Sim->EVAL->eval_delete(es);
      }
      break;
    default:
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus stat class");
    }
  fprintf(fd, "\n");
}

/* print the value of all stat variables in stat database SDB */
void
stats::stat_print_stats(struct stat_sdb_t *sdb,/* stat database */
		 FILE *fd)		/* output stream */
{
  struct stat_stat_t *stat;

  if (!sdb)
    {
      /* no stats */
      return;
    }

  for (stat=sdb->stats; stat != NULL; stat=stat->next)
    stat_print_stat(sdb, stat, fd);
}

/* find a stat variable, returns NULL if it is not found */
struct stat_stat_t *
stats::stat_find_stat(struct stat_sdb_t *sdb,	/* stat database */
	       char *stat_name)		/* stat name */
{
  struct stat_stat_t *stat;

  for (stat = sdb->stats; stat != NULL; stat = stat->next)
    {
      if (!strcmp(stat->name, stat_name))
	break;
    }
  return stat;
}



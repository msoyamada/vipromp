
#include "resource.hpp"
#include "sim.hpp"

resource::resource(sim *S)
{
	Sim = S;
}

/* create a resource pool */
struct res_pool *
resource::res_create_pool(char *name, struct res_desc *pool, int ndesc)
{
  int i, j, k, index, ninsts;
  struct res_desc *inst_pool;
  struct res_pool *res;

  /* count total instances */
  for (ninsts=0,i=0; i<ndesc; i++)
    {
      if (pool[i].quantity > MAX_INSTS_PER_CLASS)
        Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"too many functional units, increase MAX_INSTS_PER_CLASS");
      ninsts += pool[i].quantity;
    }

  /* allocate the instance table */
  inst_pool = (struct res_desc *)calloc(ninsts, sizeof(struct res_desc));
  if (!inst_pool)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"out of virtual memory");

  /* fill in the instance table */
  for (index=0,i=0; i<ndesc; i++)
    {
      for (j=0; j<pool[i].quantity; j++)
	{
	  inst_pool[index] = pool[i];
	  inst_pool[index].quantity = 1;
	  inst_pool[index].busy = FALSE;
	  for (k=0; k<MAX_RES_CLASSES && inst_pool[index].x[k].class0; k++)
	    inst_pool[index].x[k].master = &inst_pool[index];
	  index++;
	}
    }
  assert(index == ninsts);

  /* allocate the resouce pool descriptor */
  res = (struct res_pool *)calloc(1, sizeof(struct res_pool));
  if (!res)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__,"out of virtual memory");
  res->name = name;
  res->num_resources = ninsts;
  res->resources = inst_pool;

  /* fill in the resource table map - slow to build, but fast to access */
  for (i=0; i<ninsts; i++)
    {
      struct res_template *plate;
      for (j=0; j<MAX_RES_CLASSES; j++)
	{
	  plate = &res->resources[i].x[j];
	  if (plate->class0)
	    {
	      assert(plate->class0 < MAX_RES_CLASSES);
	      res->table[plate->class0][res->nents[plate->class0]++] = plate;
	    }
	  else
	    /* all done with this instance */
	    break;
	}
    }

  return res;
}

/* get a free resource from resource pool POOL that can execute a
   operation of class CLASS, returns a pointer to the resource template,
   returns NULL, if there are currently no free resources available,
   follow the MASTER link to the master resource descriptor;
   NOTE: caller is responsible for reseting the busy flag in the beginning
   of the cycle when the resource can once again accept a new operation */
struct res_template *
resource::res_get(struct res_pool *pool, int class0)
{
  int i;

  /* must be a valid class */
  assert(class0 < MAX_RES_CLASSES);

  /* must be at least one resource in this class */
  assert(pool->table[class0][0]);

  for (i=0; i<MAX_INSTS_PER_CLASS; i++)
    {
      if (pool->table[class0][i])
	{
	  if (!pool->table[class0][i]->master->busy)
	    return pool->table[class0][i];
	}
      else
	break;
    }
  /* none found */
  return NULL;
}

/* dump the resource pool POOL to stream STREAM */
void
resource::res_dump(struct res_pool *pool, FILE *stream)
{
  int i, j;

  if (!stream)
    stream = stderr;

  fprintf(stream, "Resource pool: %s:\n", pool->name);
  fprintf(stream, "\tcontains %d resource instances\n", pool->num_resources);
  for (i=0; i<MAX_RES_CLASSES; i++)
    {
      fprintf(stream, "\tclass: %d: %d matching instances\n",
	      i, pool->nents[i]);
      fprintf(stream, "\tmatching: ");
      for (j=0; j<MAX_INSTS_PER_CLASS; j++)
	{
	  if (!pool->table[i][j])
	    break;
	  fprintf(stream, "\t%s (busy for %d cycles) ",
		  pool->table[i][j]->master->name,
		  pool->table[i][j]->master->busy);
	}
      assert(j == pool->nents[i]);
      fprintf(stream, "\n");
    }
}

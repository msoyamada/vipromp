#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "host.hpp"
#include "misc.hpp"

/* maximum number of resource classes supported */
#define MAX_RES_CLASSES		16

/* maximum number of resource instances for a class supported */
#define MAX_INSTS_PER_CLASS	8

struct res_template {
    int class0;				/* matching resource class: insts
					   with this resource class will be
					   able to execute on this unit */
    int oplat;				/* operation latency: cycles until
					   result is ready for use */
    int issuelat;			/* issue latency: number of cycles
					   before another operation can be
					   issued on this resource */
    struct res_desc *master;		/* master resource record */
  };


/* resource descriptor */
struct res_desc {
  char *name;				/* name of functional unit */
  int quantity;				/* total instances of this unit */
  int busy;				/* non-zero if this unit is busy */
  struct res_template x[MAX_RES_CLASSES];
};

/* resource pool: one entry per resource instance */
struct res_pool {
  char *name;				/* pool name */
  int num_resources;			/* total number of res instances */
  struct res_desc *resources;		/* resource instances */
  /* res class -> res template mapping table, lists are NULL terminated */
  int nents[MAX_RES_CLASSES];
  struct res_template *table[MAX_RES_CLASSES][MAX_INSTS_PER_CLASS];
};

class sim;

class resource
{
	private:
		sim *Sim;

	public:
	
		resource(sim*);
		/* create a resource pool */
		struct res_pool *res_create_pool(char *name, struct res_desc *pool, int ndesc);

		/* get a free resource from resource pool POOL that can execute a
		   operation of class CLASS, returns a pointer to the resource template,
		   returns NULL, if there are currently no free resources available,
		   follow the MASTER link to the master resource descriptor;
		   NOTE: caller is responsible for reseting the busy flag in the beginning
		   of the cycle when the resource can once again accept a new operation */
		struct res_template *res_get(struct res_pool *pool, int class0);

		/* dump the resource pool POOL to stream STREAM */
		void res_dump(struct res_pool *pool, FILE *stream);
	
};

#endif



#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "memory.hpp"
#include "stats.hpp"

/* highly associative caches are implemented using a hash table lookup to
   speed block access, this macro decides if a cache is "highly associative" */
#define CACHE_HIGHLY_ASSOC(cp)	((cp)->assoc > 4)

/* cache replacement policy */
enum cache_policy {
  LRU,		/* replace least recently used block (perfect LRU) */
  Random,	/* replace a random block */
  FIFO		/* replace the oldest block in the set */
};

/* block status values */
#define CACHE_BLK_VALID		0x00000001	/* block in valid, in use */
#define CACHE_BLK_DIRTY		0x00000002	/* dirty block */

/* cache block (or line) definition */
struct cache_blk_t
{
  struct cache_blk_t *way_next;	/* next block in the ordered way chain, used
				   to order blocks for replacement */
  struct cache_blk_t *way_prev;	/* previous block in the order way chain */
  struct cache_blk_t *hash_next;/* next block in the hash bucket chain, only
				   used in highly-associative caches */
  /* since hash table lists are typically small, there is no previous
     pointer, deletion requires a trip through the hash table bucket list */
  md_addr_t tag;		/* data block tag value */
  unsigned int status;		/* block status, see CACHE_BLK_* defs above */
  tick_t ready;		/* time when block will be accessible, field
				   is set when a miss fetch is initiated */
  byte_t *user_data;		/* pointer to user defined data, e.g.,
				   pre-decode data or physical page address */
  /* DATA should be pointer-aligned due to preceeding field */
  /* NOTE: this is a variable-size tail array, this must be the LAST field
     defined in this structure! */
  byte_t data[1];		/* actual data block starts here, block size
				   should probably be a multiple of 8 */
};

/* cache set definition (one or more blocks sharing the same set index) */
struct cache_set_t
{
  struct cache_blk_t **hash;	/* hash table: for fast access w/assoc, NULL
				   for low-assoc caches */
  struct cache_blk_t *way_head;	/* head of way list */
  struct cache_blk_t *way_tail;	/* tail pf way list */
  struct cache_blk_t *blks;	/* cache blocks, allocated sequentially, so
				   this pointer can also be used for random
				   access to cache blocks */
};


enum pointer_fn_sim {dl1_access_fn_i , dl2_access_fn_i, 
		     il1_access_fn_i , il2_access_fn_i, 
		     itlb_access_fn_i, dtlb_access_fn_i}; 



/* cache definition */
struct cache_t
{
  /* parameters */
  char *name;			/* cache name */
  int nsets;			/* number of sets */
  int bsize;			/* block size in bytes */
  int balloc;			/* maintain cache contents? */
  int usize;			/* user allocated data size */
  int assoc;			/* cache associativity */
  enum cache_policy policy;	/* cache replacement policy */
  unsigned int hit_latency;	/* cache hit latency */

  /* miss/replacement handler, read/write BSIZE bytes starting at BADDR
     from/into cache block BLK, returns the latency of the operation
     if initiated at NOW, returned latencies indicate how long it takes
     for the cache access to continue (e.g., fill a write buffer), the
     miss/repl functions are required to track how this operation will
     effect the latency of later operations (e.g., write buffer fills),
     if !BALLOC, then just return the latency; BLK_ACCESS_FN is also
     responsible for generating any user data and incorporating the latency
     of that operation */
//  unsigned int					/* latency of block access */
//    (*blk_access_fn)(enum mem_cmd cmd,		/* block access command */
//		          md_addr_t baddr,		/* program address to access */
//		          int bsize,			/* size of the cache block */
//		          struct cache_blk_t *blk,	/* ptr to cache block struct */
//		          tick_t now);		/* when fetch was initiated */
  enum pointer_fn_sim pfs; 
  /* derived data, for fast decoding */
  int hsize;			/* cache set ha
  sh table size */
  md_addr_t blk_mask;
  int set_shift;
  md_addr_t set_mask;		/* use *after* shift */
  int tag_shift;
  md_addr_t tag_mask;		/* use *after* shift */
  md_addr_t tagset_mask;	/* used for fast hit detection */

  /* bus resource */
  tick_t bus_free;		/* time when bus to next level of cache is
				   free, NOTE: the bus model assumes only a
				   single, fully-pipelined port to the next
 				   level of memory that requires the bus only
 				   one cycle for cache line transfer (the
 				   latency of the access to the lower level
 				   may be more than one cycle, as specified
 				   by the miss handler */

  /* per-cache stats */
  counter_t hits;		/* total number of hits */
  counter_t misses;		/* total number of misses */
  counter_t replacements;	/* total number of replacements at misses */
  counter_t writebacks;		/* total number of writebacks at misses */
  counter_t invalidations;	/* total number of external invalidations */

  /* last block to hit, used to optimize cache hit processing */
  md_addr_t last_tagset;	/* tag of last line accessed */
  struct cache_blk_t *last_blk;	/* cache block last accessed */

  /* data blocks */
  byte_t *data;			/* pointer to data blocks allocation */

  /* NOTE: this is a variable-size tail array, this must be the LAST field
     defined in this structure! */
  struct cache_set_t sets[1];	/* each entry is a set */
};


/* cache access functions, these are safe, they check alignment and
   permissions */
#define cache_double(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(double), now, udata)
#define cache_float(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(float), now, udata)
#define cache_dword(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(long long), now, udata)
#define cache_word(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(int), now, udata)
#define cache_half(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(short), now, udata)
#define cache_byte(cp, cmd, addr, p, now, udata)	\
  cache_access(cp, cmd, addr, p, sizeof(char), now, udata)



/* define que estavam no arquivo cache.c     */
/* ------------------------------------------*/
/* cache access macros */
#define CACHE_TAG(cp, addr)	((addr) >> (cp)->tag_shift)
#define CACHE_SET(cp, addr)	(((addr) >> (cp)->set_shift) & (cp)->set_mask)
#define CACHE_BLK(cp, addr)	((addr) & (cp)->blk_mask)
#define CACHE_TAGSET(cp, addr)	((addr) & (cp)->tagset_mask)

/* extract/reconstruct a block address */
#define CACHE_BADDR(cp, addr)	((addr) & ~(cp)->blk_mask)
#define CACHE_MK_BADDR(cp, tag, set)					\
  (((tag) << (cp)->tag_shift)|((set) << (cp)->set_shift))

/* index an array of cache blocks, non-trivial due to variable length blocks */
#define CACHE_BINDEX(cp, blks, i)					\
  ((struct cache_blk_t *)(((char *)(blks)) +				\
			  (i)*(sizeof(struct cache_blk_t) +		\
			       ((cp)->balloc				\
				? (cp)->bsize*sizeof(byte_t) : 0))))

/* cache data block accessor, type parameterized */
#define __CACHE_ACCESS(type, data, bofs)				\
  (*((type *)(((char *)data) + (bofs))))

/* cache data block accessors, by type */
#define CACHE_DOUBLE(data, bofs)  __CACHE_ACCESS(double, data, bofs)
#define CACHE_FLOAT(data, bofs)	  __CACHE_ACCESS(float, data, bofs)
#define CACHE_WORD(data, bofs)	  __CACHE_ACCESS(unsigned int, data, bofs)
#define CACHE_HALF(data, bofs)	  __CACHE_ACCESS(unsigned short, data, bofs)
#define CACHE_BYTE(data, bofs)	  __CACHE_ACCESS(unsigned char, data, bofs)

/* cache block hashing macros, this macro is used to index into a cache
   set hash table (to find the correct block on N in an N-way cache), the
   cache set index function is CACHE_SET, defined above */
#define CACHE_HASH(cp, key)						\
  (((key >> 24) ^ (key >> 16) ^ (key >> 8) ^ key) & ((cp)->hsize-1))

/* copy data out of a cache block to buffer indicated by argument pointer p */
#define CACHE_BCOPY(cmd, blk, bofs, p, nbytes)	\
  if (cmd == Read)							\
    {									\
      switch (nbytes) {							\
      case 1:								\
	*((byte_t *)p) = CACHE_BYTE(&blk->data[0], bofs); break;	\
      case 2:								\
	*((half_t *)p) = CACHE_HALF(&blk->data[0], bofs); break;	\
      case 4:								\
	*((word_t *)p) = CACHE_WORD(&blk->data[0], bofs); break;	\
      default:								\
	{ /* >= 8, power of two, fits in block */			\
	  int words = nbytes >> 2;					\
	  while (words-- > 0)						\
	    {								\
	      *((word_t *)p) = CACHE_WORD(&blk->data[0], bofs);	\
	      p += 4; bofs += 4;					\
	    }\
	}\
      }\
    }\
  else /* cmd == Write */						\
    {									\
      switch (nbytes) {							\
      case 1:								\
	CACHE_BYTE(&blk->data[0], bofs) = *((byte_t *)p); break;	\
      case 2:								\
        CACHE_HALF(&blk->data[0], bofs) = *((half_t *)p); break;	\
      case 4:								\
	CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p); break;	\
      default:								\
	{ /* >= 8, power of two, fits in block */			\
	  int words = nbytes >> 2;					\
	  while (words-- > 0)						\
	    {								\
	      CACHE_WORD(&blk->data[0], bofs) = *((word_t *)p);		\
	      p += 4; bofs += 4;					\
	    }\
	}\
    }\
  }

/* bound squad_t/dfloat_t to positive int */
#define BOUND_POS(N)		((int)(MIN(MAX(0, (N)), 2147483647)))

/* where to insert a block onto the ordered way chain */
enum list_loc_t { Head, Tail };

class sim;

class cache 
{
	private:
	
		void
		unlink_htab_ent(struct cache_t *cp, struct cache_set_t *set, struct cache_blk_t *blk);
		void
		link_htab_ent(struct cache_t *cp, struct cache_set_t *set,  struct cache_blk_t *blk);	
		void
		update_way_list(struct cache_set_t *set, struct cache_blk_t *blk, enum list_loc_t where);
		
		unsigned int			/* total latency of access */
		mem_access_latency(int blk_sz, int *mem_bus_width, int *mem_lat);
		
		sim *Sim;

	public:
	
		cache(sim *s);
		

				/* create and initialize a general cache structure */
		struct cache_t *			/* pointer to cache created */
		cache_create(char *name,		/* name of the cache */
			     int nsets,			/* total number of sets in cache */
			     int bsize,			/* block (line) size of cache */
			     int balloc,		/* allocate data space for blocks? */
			     int usize,			/* size of user data to alloc w/blks */
			     int assoc,			/* associativity of cache */
			     enum cache_policy policy,	/* replacement policy w/in sets */
			     /* block access function, see description w/in struct cache def */
			//     unsigned int (*blk_access_fn)(enum mem_cmd cmd,
			//				   md_addr_t baddr, int bsize,
			//				   struct cache_blk_t *blk,
			//				   tick_t now),
			     unsigned int hit_latency, enum pointer_fn_sim pfs);/* latency in cycles for a hit */
			     
		/* parse policy */
		enum cache_policy			/* replacement policy enum */
		cache_char2policy(char c);		/* replacement policy as a char */
			     
		/* print cache configuration */
		void
		cache_config(struct cache_t *cp,	/* cache instance */
			     FILE *stream);		/* output stream */

		/* register cache stats */
		void
		cache_reg_stats(struct cache_t *cp,	/* cache instance */
				struct stat_sdb_t *sdb);/* stats database */

		/* print cache stats */
		void
		cache_stats(struct cache_t *cp,		/* cache instance */
			    FILE *stream);		/* output stream */

		/* access a cache, perform a CMD operation on cache CP at address ADDR,
		   places NBYTES of data at *P, returns latency of operation if initiated
		   at NOW, places pointer to block user data in *UDATA, *P is untouched if
		   cache blocks are not allocated (!CP->BALLOC), UDATA should be NULL if no
		   user data is attached to blocks */
		unsigned int				/* latency of access in cycles */
		cache_access(struct cache_t *cp,	/* cache to access */
			     enum mem_cmd cmd,		/* access type, Read or Write */
			     md_addr_t addr,		/* address of access */
			     void *vp,			/* ptr to buffer for input/output */
			     int nbytes,		/* number of bytes to access */
			     tick_t now,		/* time of access */
			     byte_t **udata,		/* for return of user data ptr */
			     md_addr_t *repl_addr);	/* for address of replaced block */
		
		/* return non-zero if block containing address ADDR is contained in cache
		   CP, this interface is used primarily for debugging and asserting cache
		   invariants */
		int					/* non-zero if access would hit */
		cache_probe(struct cache_t *cp,		/* cache instance to probe */
			    md_addr_t addr);		/* address of block to probe */

		/* flush the entire cache, returns latency of the operation */
		unsigned int				/* latency of the flush operation */
		cache_flush(struct cache_t *cp,		/* cache instance to flush */
			    tick_t now);		/* time of cache flush */

		/* flush the block containing ADDR from the cache CP, returns the latency of
		   the block flush operation */
		unsigned int				/* latency of flush operation */
		cache_flush_addr(struct cache_t *cp,	/* cache instance to flush */
				 md_addr_t addr,	/* address of block to flush */
				 tick_t now);		/* time of cache flush */
};

#endif


#ifndef EIO_H
#define EIO_H

#include <stdio.h>


/* EIO file formats */
#define EIO_PISA_FORMAT			1
#define EIO_ALPHA_FORMAT		2

/* EIO file version */
#define EIO_FILE_VERSION		3


#ifdef _MSC_VER
#define write		_write
#endif

#define EIO_FILE_HEADER							\
  "/* This is a SimpleScalar EIO file - DO NOT MOVE OR EDIT THIS LINE! */\n"
/*
   EIO transaction format:

   (inst_count, pc,
    ... reg inputs ...
    [r2, r3, r4, r5, r6, r7],
    ... mem inputs ...
    ((addr, size, blob), ...)
    ... reg outputs ...
    [r2, r3, r4, r5, r6, r7],
    ... mem outputs ...
    ((addr, size, blob), ...)
   )
*/


struct mem_rec_t {
  md_addr_t addr;
  unsigned size, maxsize;
  struct exo_term_t *exo;
  struct exo_term_t *blob;
};

/* size of padding that can be filled on the end of a blob tail */
#define BLOB_TAIL_SIZE		256

class sim;

class eio
{
	private:
	
		sim *Sim;
	
		counter_t eio_trans_icnt;

		/* reg recs */
		struct exo_term_t *input_regs;
		struct exo_term_t *output_regs;

		/* input memory recs */
		struct exo_term_t *input_mem;
		struct mem_rec_t input_mem_rec;

		/* output memory recs */
		struct exo_term_t *output_mem;
		struct mem_rec_t output_mem_rec;

		int seen_write;
		
		//mem_access_fn local_mem_fn;

#if 0
		enum md_fault_type
		my_mem_fn(struct mem_t *mem,		/* memory space to access */
			  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
			  md_addr_t addr,		/* target address to access */
			  void *vp,			/* host memory address to access */
			  int nbytes);
#endif
	
	public:
		eio(sim*);

		FILE *eio_create(char *fname);
		FILE *eio_open(char *fname);

		/* returns non-zero if file FNAME has a valid EIO header */
		int eio_valid(char *fname);

		void eio_close(FILE *fd);

		/* check point current architected state to stream FD, returns
		   EIO transaction count (an EIO file pointer) */
		counter_t
		eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
				struct mem_t *mem,		/* memory to dump */
				FILE *fd);			/* stream to write to */

		/* read check point of architected state from stream FD, returns
		   EIO transaction count (an EIO file pointer) */
		counter_t
		eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
				struct mem_t *mem,		/* memory to dump */
				FILE *fd);			/* stream to read */

#if 0
		/* syscall proxy handler, with EIO tracing support, architect registers
		   and memory are assumed to be precise when this function is called,
		   register and memory are updated with the results of the sustem call */
		void
		eio_write_trace(FILE *eio_fd,			/* EIO stream file desc */
				counter_t icnt,			/* instruction count */
				struct regs_t *regs,		/* registers to update */
				mem_access_fn mem_fn,		/* generic memory accessor */
				struct mem_t *mem,		/* memory to update */
				md_inst_t inst);		/* system call inst */

#endif
		/* syscall proxy handler from an EIO trace, architect registers
		   and memory are assumed to be precise when this function is called,
		   register and memory are updated with the results of the sustem call */
		void
		eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
			       counter_t icnt,			/* instruction count */
			       struct regs_t *regs,		/* registers to update */
			       /*mem_access_fn mem_fn,*/		/* generic memory accessor */
			       struct mem_t *mem,		/* memory to update */
			       md_inst_t inst);			/* system call inst */

		/* fast forward EIO trace EIO_FD to the transaction just after ICNT */
		void eio_fast_forward(FILE *eio_fd, counter_t icnt);

};

#endif


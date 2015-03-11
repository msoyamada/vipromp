#ifndef ENDIAN_H
#define ENDIAN_H

#include "loader.hpp"

/* data swapping functions, from big/little to little/big endian format */
#if 0 /* FIXME: disabled until further notice... */
#define __SWAP_HALF(N)	((((N) & 0xff) << 8) | (((unsigned short)(N)) >> 8))
#define SWAP_HALF(N)	(sim_swap_bytes ? __SWAP_HALF(N) : (N))

#define __SWAP_WORD(N)	(((N) << 24) |					\
			 (((N) << 8) & 0x00ff0000) |			\
			 (((N) >> 8) & 0x0000ff00) |			\
			 (((unsigned int)(N)) >> 24))
#define SWAP_WORD(N)	(sim_swap_bytes ? __SWAP_WORD(N) : (N))
#else
#define SWAP_HALF(N)	(N)
#define SWAP_WORD(N)	(N)
#endif

/* recognized endian formats */
enum endian_t { endian_big, endian_little, endian_unknown};

class sim;

class endian
{
	private:

	sim *Sim;
	
	public:
		endian(sim *);
		
		/* probe host (simulator) byte endian format */
		enum endian_t
		endian_host_byte_order(void);

		/* probe host (simulator) double word endian format */
		enum endian_t
		endian_host_word_order(void);

		/* probe target (simulated program) byte endian format, only
		   valid after program has been loaded */
		enum endian_t
		endian_target_byte_order(void);

		/* probe target (simulated program) double word endian format,
		   only valid after program has been loaded */
		enum endian_t
		endian_target_word_order(void);


};

#endif


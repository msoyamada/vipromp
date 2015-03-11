#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdio.h>
#include "../SSpp-mips/memory.hpp"
#include "../SSpp-mips/machine.hpp"
#include "../SSpp-mips/host.hpp"

#include "simple_dma_slave_if.hpp"
#include <systemc.h>

class channel
	: public simple_dma_slave_if
	, public sc_module
{

public:
	// ports
	sc_in_clk clock;

	SC_HAS_PROCESS(channel);

	channel(  sc_module_name name_
	          , md_addr_t low_limit
	          , md_addr_t high_limit
	          , int _latency = 18);

	simple_bus_status read(void *host,
						unsigned int addr,
						unsigned int nbytes
						);								/* host memory address to access */

	simple_bus_status write(void *host,
						unsigned int addr,
						unsigned int nbytes
						);								/* host memory address to access */

	word_t spec_mem_read(md_addr_t addr);

	md_addr_t translate_addr(md_addr_t addr);

	md_addr_t getLowLimit (void) { return low; };
	md_addr_t getHighLimit(void) { return high; };

	void print_mem(unsigned int addr_low = 0, unsigned int addr_high = 0);
	
	int get_latency();

private:

	int latency;
	unsigned char *mem;
	unsigned int size;
	md_addr_t low, high;
	int m_wait_count;
	unsigned int m_nr_wait_states;

};

#endif


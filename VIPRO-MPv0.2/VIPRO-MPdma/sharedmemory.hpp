#ifndef sharedmemory_H
#define sharedmemory_H

#include <stdio.h>

#include "bus/simple_bus_slave_if.hpp"
#include <systemc.h>

class sharedmemory
	: public simple_bus_slave_if
	, public sc_module
{

public:
	// ports
	sc_in_clk clock;

	SC_HAS_PROCESS(sharedmemory);

	sharedmemory(  sc_module_name name_
	          , unsigned int low_limit
	          , unsigned int high_limit
	          , int _latency = 18);


	/* functions of the simple_bus_slave_if */
	simple_bus_status read(void *host,
						unsigned int addr,
						unsigned int nbytes
						);		

	simple_bus_status write(void *host,
						unsigned int addr,
						unsigned int nbytes
						);								
	/* -------------------------------------*/

	unsigned int spec_mem_read(unsigned int addr);

	unsigned int translate_addr(unsigned int addr);

	unsigned int getLowLimit (void) { return low; };
	unsigned int getHighLimit(void) { return high; };

	void print_mem(unsigned int addr_low = 0, unsigned int addr_high = 0);

	int get_latency();

private:

	int latency;
	unsigned char *mem;
	unsigned int size;
	unsigned int low, high;
	int m_wait_count;
	unsigned int m_nr_wait_states;

};

#endif


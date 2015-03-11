#include <stdio.h>
#include "sharedmemory.hpp"
#include "bus/simple_bus_tools.hpp"


/* byte_t: unsigned char */
/* word_t: unsigned int  */

/* specify values of simplescalar memory unsed */
sharedmemory::sharedmemory(sc_module_name name_, unsigned int low_limit,
			unsigned int high_limit, int _latency) : sc_module(name_)
{

    sc_assert(low_limit <= high_limit);
    sc_assert((high_limit-low_limit+1)%4 == 0);


    latency = _latency;

	size = (high_limit - low_limit + 1);
	low = low_limit;
	high = high_limit;

	/* allocate shared memory */
	mem = new unsigned char [size];

	m_wait_count = 0;
	m_nr_wait_states = (latency-3);

	for (int i = 0; i < size; i++)
		mem[i] = 0;
}

simple_bus_status sharedmemory::read(void *host, unsigned int addr, unsigned int nbytes)
{
	 // accept a new call if m_wait_count < 0)
	 m_wait_count--;

	  if ((m_wait_count == 0)||(m_nr_wait_states <= 0))
	    {

		  byte_t *p = (byte_t*) host;
		  switch (nbytes)
		  {
		  case 1:
			  *((byte_t *)p) = mem[translate_addr(addr)];
			  p[1] = 0;
			  p[2] = 0;
			  p[3] = 0;
			  break;
		  case 2:
			  p[0] = (byte_t) mem[translate_addr(addr)];
			  p[1] = (byte_t) mem[translate_addr(addr)+ 1] ;
			  p[2] = 0;
			  p[3] = 0;
			  break;
		  case 4:
		  case 8:
			  p[0] = (byte_t) mem[translate_addr(addr)];
			  p[1] = (byte_t) mem[translate_addr(addr)+ 1] ;
			  p[2] = (byte_t) mem[translate_addr(addr)+ 2] ;
			  p[3] = (byte_t) mem[translate_addr(addr)+ 3] ;
			  break;
		  }

	      //*host = mem[(addr - low)/4];
	      return SIMPLE_BUS_OK;
	    }
		 if (m_wait_count < 0)
		    {
		      m_wait_count = m_nr_wait_states;
		      return SIMPLE_BUS_WAIT;
		    }

	  return SIMPLE_BUS_WAIT;
}


simple_bus_status sharedmemory::write(void *host, unsigned int addr, unsigned int nbytes)
{
	  // accept a new call if m_wait_count < 0)
	  m_wait_count--;
	  if ((m_wait_count == 0)||(m_nr_wait_states <= 0))
	  {
		  byte_t *p = (byte_t*) host;

		  //printf(">>>>> %d, 0x%X\n", p[0], addr);

		  switch (nbytes)
		  {
		  case 1:
			  mem[translate_addr(addr)] = *((byte_t *)p);
			  break;
		  case 2:
			  mem[translate_addr(addr)] 	= (byte_t) p[0];
			  mem[translate_addr(addr) + 1] = (byte_t) p[1];
		  case 4:
		  case 8:
			  mem[translate_addr(addr)] 	= (byte_t) p[0];
			  mem[translate_addr(addr) + 1] = (byte_t) p[1];
			  mem[translate_addr(addr) + 2] = (byte_t) p[2];
			  mem[translate_addr(addr) + 3] = (byte_t) p[3];
			  break;
		  }

	      //mem[(addr - low)/4] = *host;
	      return SIMPLE_BUS_OK;
	  }
	  if (m_wait_count < 0)
	  {
	      m_wait_count = m_nr_wait_states;
	      return SIMPLE_BUS_WAIT;
	  }

	  return SIMPLE_BUS_WAIT;
}

unsigned int sharedmemory::spec_mem_read(unsigned int addr)
{
	return mem[translate_addr(addr)];
}

/* convert addr of simplescalar memory to my shared memory */
unsigned int sharedmemory::translate_addr(unsigned int addr)
{
	return addr-low;
}

void sharedmemory::print_mem(unsigned int addr_low, unsigned int addr_high)
{
	addr_low  = (addr_low  == 0)? low  : addr_low;
	addr_high = (addr_high == 0)? high : addr_high;

	for (unsigned int i = addr_low-low; i < addr_high-low+1; i++)
	{
		if (mem[i] != 0)
			printf("[0x%X] > %d \n", i+low, mem[i]);
	}
}


int sharedmemory::get_latency()
{
	return latency;
}

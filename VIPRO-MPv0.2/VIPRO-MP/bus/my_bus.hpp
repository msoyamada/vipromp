

#ifndef __my_bus_h
#define __my_bus_h

#include <systemc.h>

#include "simple_bus_slave_if.hpp"
#include "simple_bus_blocking_if.hpp"
#include "simple_bus_arbiter.hpp"
#include "simple_bus_tools.hpp"

class my_bus
  : public simple_bus_blocking_if
  , public sc_module
{
public:
  // ports
  sc_in_clk clock;
  sc_port<simple_bus_arbiter_if> arbiter_port;
  sc_port<simple_bus_slave_if, 0> slave_port;

  SC_HAS_PROCESS(my_bus);

  // constructor
  my_bus(sc_module_name name_
             , bool verbose = false)
    : sc_module(name_)
    , m_verbose(verbose)
  {
    /* method cycle */
    SC_METHOD(main_action);
    dont_initialize();
    sensitive_neg << clock;
  }

  // process
  void main_action();

  simple_bus_status read_ss(unsigned int addr,								/* target address to access - word - unsigned int - 32bits*/
			void *host,														/* host memory address to access */
			unsigned int nbytes,
			unsigned int unique_priority,
			bool lock);

  simple_bus_status write_ss(unsigned int addr,								/* target address to access - word - unsigned int - 32bits*/
			 void *host,														/* host memory address to access */
			 unsigned int nbytes,
			 unsigned int unique_priority,
			 bool lock);

  int get_latency(unsigned int address);

/*
  // direct BUS interface
  bool direct_read(int *data, unsigned int address);
  bool direct_write(int *data, unsigned int address);

  // non-blocking BUS interface
  void read(unsigned int unique_priority
	    , int *data
	    , unsigned int address
	    , bool lock = false);
  void write(unsigned int unique_priority
	     , int *data
	     , unsigned int address
	     , bool lock = false);
  simple_bus_status get_status(unsigned int unique_priority);

  // blocking BUS interface

  simple_bus_status burst_read(unsigned int unique_priority
			       , int *data
			       , unsigned int start_address
			       , unsigned int length = 1
			       , bool lock = false);

  simple_bus_status burst_write(unsigned int unique_priority
				, int *data
				, unsigned int start_address
				, unsigned int length = 1
				, bool lock = false);
*/
private:
  void handle_request();
//  void end_of_elaboration();
  simple_bus_slave_if * get_slave(unsigned int address);
  simple_bus_request * get_request(unsigned int priority);
  simple_bus_request * get_next_request();
  void clear_locks();

private:
  bool m_verbose;
  simple_bus_request_vec m_requests;
  simple_bus_request *m_current_request;

}; // end class simple_bus

#endif

/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2002 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.3 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  simple_dma_blocking_if.h : The blocking dma interface.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef __simple_dma_blocking_if_h
#define __simple_dma_blocking_if_h

#include <systemc.h>

#include "../bus/simple_bus_tools.hpp"
//#include "../memory.hpp"

class simple_dma_blocking_if
  : public virtual sc_interface
{
public:

// 	virtual simple_bus_status read_ss(unsigned int addr,/* target address to access - word - unsigned int - 32bits*/
// 				void *host,		    /* host memory address to access */
// 				unsigned int nbytes,
// 				unsigned int unique_priority,
// 				bool lock)= 0;
// 
// 	virtual simple_bus_status write_ss(unsigned int addr,/* target address to access - word - unsigned int - 32bits*/
// 				 void *host,		     /* host memory address to access */
// 				 unsigned int nbytes,
// 				 unsigned int unique_priority,
// 				 bool lock) = 0;
	
	virtual simple_bus_status read(void *host, unsigned int addr, unsigned int nbytes)=0;

	virtual simple_bus_status write(void *host, unsigned int addr, unsigned int nbytes)=0;
	
	virtual int get_latency(unsigned int address)=0;

	virtual bool request(unsigned int ss_priority)=0;

}; // end class simple_dma_blocking_if

#endif

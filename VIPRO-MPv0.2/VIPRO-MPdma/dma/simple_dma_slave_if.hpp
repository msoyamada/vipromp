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

  simple_dma_slave_if.h : The Slave dma interface.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef __simple_dma_slave_if_h
#define __simple_dma_slave_if_h

#include <systemc.h>

#include "../bus/simple_bus_tools.hpp"
#include "../SSpp-mips/memory.hpp"

class simple_dma_slave_if
	: public virtual sc_interface
{
public:

	virtual simple_bus_status read(void *host,
			unsigned int addr,
			unsigned int nbytes
			) = 0;								/* host memory address to access */

	virtual simple_bus_status write(void *host,
			unsigned int addr,
			unsigned int nbytes
			) = 0;									/* host memory address to access */

	virtual md_addr_t getLowLimit (void) = 0;
	virtual md_addr_t getHighLimit(void) = 0;

	virtual int get_latency()=0;

	/*
	// Slave interface
  virtual simple_bus_status read(int *data, unsigned int address) = 0;
  virtual simple_bus_status write(int *data, unsigned int address) = 0;

  virtual unsigned int start_address() const = 0;
  virtual unsigned int end_address() const = 0;

*/

}; // end class simple_dma_slave_if

#endif

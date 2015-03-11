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

  simple_dma_types.h : The common types.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#ifndef __simple_d_h
#define __simple_d_h

#include <stdio.h>
#include <systemc.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

enum simple_dma_status { SIMPLE_DMA_OK
			 , SIMPLE_DMA_REQUEST
			 , SIMPLE_DMA_WAIT
			 , SIMPLE_DMA_ERROR = 0 };

// needed for more readable debug output
// only needed for more readable debug output
char simple_dma_status_str[4][20] = {"SIMPLE_DMA_OK"
                                   , "SIMPLE_DMA_REQUEST"
                                   , "SIMPLE_DMA_WAIT"
                                   , "SIMPLE_DMA_ERROR"};


struct simple_dma_request{
		unsigned int priority;
		unsigned int addr;
		int nbytes;
		int ntbytes;
		bool do_read;
		unsigned int p_write;
		int channel_status;
		simple_dma_status status;
		sc_event transfer_done;

		simple_dma_request();
	};

inline simple_dma_request::simple_dma_request()
	: priority(0)
{}

struct simple_dma_request;
typedef sc_pvector<simple_dma_request *> simple_dma_request_vec;

#endif

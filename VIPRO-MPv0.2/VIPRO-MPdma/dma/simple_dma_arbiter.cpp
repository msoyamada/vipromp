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

  simple_dma_arbiter.cpp : The arbitration unit.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "simple_dma_arbiter.hpp"

simple_dma_request * simple_dma_arbiter::arbitrate(const simple_dma_request_vec &requests){
  unsigned int i;
  
  simple_dma_request *best_request = requests[0];

  // highest priority: status==SIMPLE_DMA_WAIT:
  for (i = 0; i < requests.size(); i++){
	  simple_dma_request *request = requests[i];
	  if (request->status == SIMPLE_DMA_WAIT){
		  return request;
	  }
  }

  // second priority: priority (ID)
  for (i = 1; i < requests.size(); i++){
	  if ((requests[i]->priority < best_request->priority) && (requests[i]->status == SIMPLE_DMA_REQUEST)){
		  best_request = requests[i];
	  }
  }

  return best_request;

}

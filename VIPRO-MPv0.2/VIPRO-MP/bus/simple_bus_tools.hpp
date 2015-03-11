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

  simple_bus_types.h : The common types.

  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef __simple_bus_tools_h
#define __simple_bus_tools_h

#include <stdio.h>
#include <systemc.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

typedef unsigned char byte_t;
typedef unsigned int  word_t;

enum simple_bus_status { SIMPLE_BUS_OK = 0
			 , SIMPLE_BUS_REQUEST
			 , SIMPLE_BUS_WAIT
			 , SIMPLE_BUS_ERROR };

// needed for more readable debug output
// only needed for more readable debug output
char simple_bus_status_str[4][20] = {"SIMPLE_BUS_OK"
                                   , "SIMPLE_BUS_REQUEST"
                                   , "SIMPLE_BUS_WAIT"
                                   , "SIMPLE_BUS_ERROR"};


enum simple_bus_lock_status { SIMPLE_BUS_LOCK_NO = 0
							, SIMPLE_BUS_LOCK_SET
							, SIMPLE_BUS_LOCK_GRANTED
};

struct simple_bus_request
{
  // parameters
  unsigned int priority;

  // request parameters
  bool do_write;
  unsigned int address;
  unsigned int end_address;
  unsigned int nbytes;
  void *data;
  simple_bus_lock_status lock;

  // request status
  sc_event transfer_done;
  simple_bus_status status;

  // default constructor
  simple_bus_request();
};

inline simple_bus_request::simple_bus_request()
  : priority(0)
  , do_write(false)
  , address(0)
  , end_address(0)
  , data((int *)0)
  , lock(SIMPLE_BUS_LOCK_NO)
  , status(SIMPLE_BUS_OK)
{}

struct simple_bus_request;
typedef sc_pvector<simple_bus_request *> simple_bus_request_vec;


int sb_fprintf(FILE *fp, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = 0;
  do {
    errno = 0;
    ret = vfprintf(fp, fmt, ap);
  } while (errno == EINTR);
  return ret;
}


#endif

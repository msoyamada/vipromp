/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2001 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.2 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_refined_if.h -- implementation of indexed protocols 
  		     
  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_REFINED_IF_H
#define SC_REFINED_IF_H

/*
#include "systemc/utils/sc_hash.h"
#include "systemc/communication/sc_signal.h"
*/
#include "sc_ms/sc_link_mp_if.h"

// link mp interface for refined ports
//	only for **virtual** derivation by sc_link_mp.
//	cannot be instantiated
template<class T>
class sc_refined_if : public virtual sc_link_mp_if {
protected:
  sc_refined_if( ) {  }
  
  typedef sc_phash<int, sc_interface* > refined_signals;
  typedef sc_phash<int, void* >		attribute_data;

  refined_signals	data_signals;
  refined_signals	address_signals;
  refined_signals	control_signals;
  attribute_data	attribute_data_list;
  
public:
  // destructor
  virtual ~sc_refined_if( ) {  }

  sc_interface* get_signal(int id, sc_terminal_type ttype ) {
    sc_interface* sig = 0;

    switch( ttype ) {
    case TT_DATA:
      data_signals.lookup( id, &sig );
      break;
    
    case TT_ADDRESS:
      address_signals.lookup( id, &sig );
      break;
    
    case TT_CONTROL:
      control_signals.lookup( id, &sig );
      break;
    } // switch

    return sig;
  }

  void add_signal( sc_interface* intf, sc_terminal_type ttype, int id ) {
    switch( ttype ) {
    case TT_DATA:
      data_signals.insert( id, intf );
      break;
    
    case TT_ADDRESS:
      address_signals.insert( id, intf );
      break;
    
    case TT_CONTROL:
      control_signals.insert( id, intf );
      break;
    } // switch
  }

  void add_attribute_data( void * data, int id ) {
    attribute_data_list.insert( id, data );
  }

  void * get_attribute_data( int id ) {
    void *d;
    attribute_data_list.lookup( id, &d );
    return d;
  }
};  // sc_refined_if

#endif // SC_INDEXED_H

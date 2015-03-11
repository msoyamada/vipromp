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

  sc_self_binding_port.h -- interface for terminals of master/slave ports.

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_SELF_BINDING_PORT_H
#define SC_SELF_BINDING_PORT_H

// #include "systemc/utils/sc_hash.h"
#include "sc_ms/sc_self_binding_port_if.h"
#include "sc_ms/sc_terminal_if.h"
#include "sc_ms/sc_self_binding_attribute_if.h"

class sc_self_binding_port;
extern sc_self_binding_port* current_port_protocol;

//---------------------------------------------------------------
class sc_self_binding_port : public virtual sc_self_binding_port_if
{
public:
	virtual ~sc_self_binding_port( ); 

protected:
  typedef sc_phash<int, sc_self_binding_terminal_if*>	terminals;
  typedef sc_phash<int, sc_self_binding_attribute_if*> 	attributes;

  terminals		data_terminals;
  terminals		address_terminals;
  terminals		control_terminals;
  attributes	attribute_list;
  
  // add the terminal to one of the maps
  void	add_terminal( terminals& the_list, 
                      sc_self_binding_terminal_if* intf, 
                      int id );
public:
  virtual void register_self_binding_port( ) { 
    // register yourself with the global variable. not MT safe
    current_port_protocol	=	this;
  }

  // constructor
  sc_self_binding_port( ) {  register_self_binding_port( );  }

  virtual int	get_data_index( )		{ return data_terminals.count( );    }
  virtual int	get_address_index( )	{ return address_terminals.count( ); }
  virtual int	get_control_index( )	{ return control_terminals.count( ); }
  virtual int	get_attribute_index( )	{ return attribute_list.count( ); }

  void add_terminal_if(sc_self_binding_terminal_if* intf, int id );
  void add_attribute_if( sc_self_binding_attribute_if *, int id );
};  // sc_self_binding_port



#endif // SELF_BINDING_PORT_IF_H 

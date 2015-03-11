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

  sc_self_binding_port_if.h -- interface for self binding ports 
  		     
  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_SELF_BINDING_PORT_IF_H
#define SC_SELF_BINDING_PORT_IF_H


class sc_self_binding_terminal_if;
class sc_self_binding_attribute_if;

class sc_self_binding_port_if
{
public:
  virtual ~sc_self_binding_port_if( ) { }
  // virtual void bind( self_binding_port_if* ) = 0;
  // virtual void bind( sc_link_mp_if* ) = 0;
  virtual void add_terminal_if(sc_self_binding_terminal_if* , int id ) = 0;
  virtual void add_attribute_if(sc_self_binding_attribute_if* , int id ) = 0;

  // register yourself to some global place so that the member terminals
  // can add themselves to the list of terminals available
  virtual void	register_self_binding_port( ) = 0;
  //virtual void *get_attribute( int ) = 0;

  virtual int	get_data_index( ) = 0;
  virtual int	get_address_index( ) = 0;
  virtual int	get_control_index( ) = 0;
  virtual int	get_attribute_index( ) = 0;
};  // sc_self_binding_port_if

				 
#endif // SC_SELF_BINDING_PORT_IF_H

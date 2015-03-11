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

  sc_self_binding_if.h -- self binding interface for attributes and terminals
  
  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_SELF_BINDING_IF_H
#define SC_SELF_BINDING_IF_H

class sc_interface;
class sc_self_binding_port_if;

class sc_self_binding_if
{
public:
  // get the last registered self binding port
  virtual sc_self_binding_port_if*	get_registered_self_binding_port( )  = 0;
  virtual int				get_index( ) = 0;
}; // sc_self_binding_if

#endif // SC_SELF_BINDING_IF_H

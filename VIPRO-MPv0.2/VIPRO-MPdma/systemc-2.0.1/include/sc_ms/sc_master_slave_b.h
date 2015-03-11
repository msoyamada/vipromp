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

  sc_master_slave_b.h -- interface for abstract and refined master/slave ports.

  Original Author: Dirk Vermeersch, Coware, Inc.
		   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_MASTER_SLAVE_B_H
#define SC_MASTER_SLAVE_B_H

#include "sc_ms/sc_master_slave_if.h"
#include "sc_ms/sc_link_mp_if.h"
/*
#include "systemc/communication/sc_port.h"
#include "systemc/communication/sc_signal_ports.h"
*/

template<class IF >
class sc_master_slave_b : 
public virtual sc_port< IF >, public virtual sc_master_slave_base 
{
public:
  sc_master_slave_b( ) : the_interface( 0 ) {
    
  }
  IF*	get_master_slave_interface(  ) {
    return the_interface;
  }

  IF* operator ->( ) {
    return get_master_slave_interface( );
  }

  virtual void end_of_elaboration( ) {
    the_interface = get_master_slave_interface_dynamic( );
    the_interface->validate( );
  }

  virtual const char * name ( ) {
    return sc_port< IF >:: name( );
  }

protected:
  IF*	get_master_slave_interface_dynamic(  ) {
    if ( the_interface )
      return the_interface;  // return the cached value
    
    IF* abs_link = dynamic_cast< IF* >( get_interface( ) );
    if ( abs_link == 0 )
      REPORT_ERROR(0, name( ) );
    return abs_link;
  }

private:
  IF*	the_interface;	// saving a local copy avoids dynamic cast's
};


#endif  // SC_MASTER_SLAVE_B_H

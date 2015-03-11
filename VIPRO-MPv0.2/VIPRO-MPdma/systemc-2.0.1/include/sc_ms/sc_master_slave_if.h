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

  sc_master_slave_if.h -- Provides a simulation context for use with multiple
                       simulations.

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_MASTER_SLAVE_IF_H
#define SC_MASTER_SLAVE_IF_H

#include <typeinfo>
#include "systemc.h"

// sc_terminal_type : the type of bus protocol terminals
// identifies the purpose of the terminal
// terminals will also have an id ( or assigned an id automatically )
// terminals with the same id/type on the same sc_link_mp will be connected
enum sc_terminal_type 	{TT_DATA, TT_ADDRESS, TT_CONTROL};
enum sc_port_role 	{SC_MASTER, SC_SLAVE };

class sc_slave_process;
typedef sc_slave_process* sc_slave_handle;

class sc_link_mp_if;
typedef sc_pvector<sc_slave_handle>  sc_slave_proc_list;

void execute_slave_process( sc_slave_handle handle );

sc_slave_handle register_slave_process( const char* name,
                                         SC_ENTRY_FUNC entry_fn,
                                         sc_module* module );

#define declare_slave_process(handle, name, module_tag, func, port)         \
    sc_slave_handle handle;                                                 \
    {                                                                       \
        SC_DECL_HELPER_STRUCT( module_tag, func );                          \
        handle = register_slave_process( name,           		    \
                     SC_MAKE_FUNC_PTR( module_tag, func ), this );          \
	port.make_sensitive_slave( handle ); 				    \
    }
      
#define SC_SLAVE(func, port)                                                \
    declare_slave_process( func ## _handle,                                 \
                            #func,                                          \
                            SC_CURRENT_USER_MODULE,                         \
                            func, port )                                    \


//-----------------------------------------------------------------
// base class for all master/slave interfaces
class sc_master_slave_if: public virtual sc_interface  {
public:
  virtual ~sc_master_slave_if( ) { }
};

//-----------------------------------------------------------------
// base class for all master/slave ports
class sc_master_slave_base 
{ 
public:
  virtual sc_port_role 	port_role( ) = 0;
  virtual void 		register_master_slave_port( sc_link_mp_if* ) = 0;
  virtual const char *	port_typename( ) { return 0; }

  // can_bind is called to check if two ports connected to the same link
  // are compatible.  ** This is not used for checking compatiblity between
  // port-to-port binding**
  virtual bool 		can_bind( sc_master_slave_base* other ) {
    const char * this_name 	= port_typename( );
    const char * other_name 	= other->port_typename( );
    return strcmp( this_name, other_name)==0;
  }

  virtual const char * name ( ) = 0;
private:
  const char * name_;
};

//-----------------------------------------------------------------
class SBprotocol
{
public:
  virtual ~SBprotocol( ) { }
};

#define SC_PROTOCOL(name)  struct name : public SBprotocol

//-----------------------------------------------------------------
// read interface for the master/slave
template<class T>
class sc_master_slave_read_if 
{
public:
  // virtual const T& read( ) =0 ;
};

// write interface master/slave
template<class T>
class sc_master_slave_write_if 
{
public:
  // virtual void write( const T& ) = 0;
};

template<class T>
class sc_master_slave_read_write_if :
  public virtual sc_master_slave_read_if<T>,
  public virtual sc_master_slave_write_if<T>
{

};

#endif // SC_MASTER_SLAVE_IF_H

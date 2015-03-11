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

  sc_abstract_port_if.h -- interface for abstract master/slave ports.

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_ABSTRACT_PORT_IF_H
#define SC_ABSTRACT_PORT_IF_H
  
#include "sc_ms/sc_master_slave_b.h"
#include "sc_ms/sc_link_mp.h"
#include "sc_ms/sc_abstract_if.h"


// forward declarations
template<class T> class sc_abstract_master_port_if;
template<class T> class sc_abstract_inmaster_port_if;
template<class T> class sc_abstract_outmaster_port_if;
template<class T> class sc_abstract_inoutmaster_port_if;

template<class T> class sc_abstract_slave_port_if;
template<class T> class sc_abstract_inslave_port_if;
template<class T> class sc_abstract_outslave_port_if;
template<class T> class sc_abstract_inoutslave_port_if;

//---------------------------------------------------------------------
// common base for all abstract master/slave ports
template<class T, class IF>
class sc_abstract_master_slave_base : public virtual sc_master_slave_b< IF >
{
public:
  typedef IF 	if_type;

  virtual const char* port_typename( ) { return "sc_abstract_master_slave"; }

protected:
  void  do_write( const T& data ) {
    IF* link = get_master_slave_interface( );
    link->reset_write_count( );
    link->write( data );
  }
  const T&  do_read(  ) {
    return get_master_slave_interface( )->read( );
  }
  void reset_write_count( ) {
    IF* link = get_master_slave_interface( );
    link->reset_write_count( );
  }
};


//---------------------------------------------------------------------
// common base for all abstract master ports
template<class T, class IF>
class sc_abstract_master_base : public virtual sc_abstract_master_slave_base<T, IF >
{
public:
  virtual sc_port_role port_role( ) { return SC_MASTER; }
};

//---------------------------------------------------------------------
// common base for all abstract slave ports
template<class T, class IF>
class sc_abstract_slave_base : public virtual sc_abstract_master_slave_base<T, IF >
{
public:
  virtual sc_port_role port_role( ) { return SC_SLAVE; }
  void		make_sensitive_slave( sc_slave_handle handle ) {
    m_slave_procs.push_back( handle );
  }
protected:
  sc_slave_proc_list	m_slave_procs;
};

////////////////////////////////////////////////////////////////////
// master port interface
//------------------------------------------------------------------
template<class T> 
class sc_abstract_master_port_if : 
public virtual sc_abstract_master_base< T, sc_abstract_master_if<T> >
{		 
public:
  typedef sc_abstract_master_port_if<T>		this_type;
  typedef sc_abstract_master_if<T>		if_type;

  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ return bind( link); }
  void operator( ) ( this_type& other )		{ return bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* /* link */ ) {
    get_master_slave_interface_dynamic( )->add_master( this );
  }
  // run the slaves
  void operator ( ) ( ) { 
    // get_master_slave_interface( )->run_slaves( ); 
    get_master_slave_interface( )->operator( ) ( ); 
  }
};

//------------------------------------------------------------------
template<class T> 
class sc_abstract_inmaster_port_if : 
  public virtual sc_abstract_master_base< T, sc_abstract_inmaster_if<T> >,
  public virtual sc_master_slave_read_if< T >
{
public:
  typedef sc_abstract_inmaster_port_if<T>		this_type;
  typedef sc_abstract_inmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_abstract_inoutmaster_port_if<T>& other) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  void operator( ) ( sc_abstract_inoutmaster_port_if<T>& other) { bind( other ); }
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    get_master_slave_interface_dynamic( )->add_inmaster( this );
  }
  const T& read( ) { 
    reset_write_count( );
    return do_read( ); 
  }
};

//------------------------------------------------------------------
template<class T> 
class sc_abstract_outmaster_port_if : 
  public virtual sc_abstract_master_base< T, sc_abstract_outmaster_if<T> >,
  public virtual sc_master_slave_write_if< T >
{
public:
  typedef sc_abstract_outmaster_port_if<T>		this_type;
  typedef sc_abstract_outmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_abstract_inoutmaster_port_if<T>& other) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )		{ bind( other); }
  void operator( ) ( sc_abstract_inoutmaster_port_if<T>& other) { bind( other ); }

  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    get_master_slave_interface_dynamic( )->add_outmaster( this );
  }
  void write( const T& data )	{ do_write( data ); }
};

//------------------------------------------------------------------
template<class T> 
class sc_abstract_inoutmaster_port_if : 
public virtual sc_abstract_master_base< T, sc_abstract_inoutmaster_if<T> >,
  public virtual sc_master_slave_read_write_if< T >
{
public:
  typedef sc_abstract_inoutmaster_port_if<T>		this_type;
  typedef sc_abstract_inoutmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  
  
  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    get_master_slave_interface_dynamic( )->add_inoutmaster( this );
  }
  const T& read( ) { 
    reset_write_count( );
    return do_read( ); 
  }
  void write( const T& data )	{ do_write( data ); }
};


////////////////////////////////////////////////////////////////////
// slave port interface
//------------------------------------------------------------------
template<class T> 
class sc_abstract_slave_port_if : 
  public virtual sc_abstract_slave_base< T, sc_abstract_slave_if<T> >
{
public:
  typedef sc_abstract_slave_port_if<T>		this_type;
  typedef sc_abstract_slave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_slave( this );
    alink->add_slave_proc ( m_slave_procs );
  }
};

//------------------------------------------------------------------
template<class T> class sc_abstract_inslave_port_if : 
  public virtual sc_abstract_slave_base< T, sc_abstract_inslave_if<T> >,
  public virtual sc_master_slave_read_if< T >
{
public:
  typedef sc_abstract_inslave_port_if<T>		this_type;
  typedef sc_abstract_inslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_abstract_inoutslave_port_if<T>& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  void operator( ) ( sc_abstract_outslave_port_if<T>& other ) { bind( other ); }
  
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink  = get_master_slave_interface_dynamic(  );
    alink->add_inslave( this );
    alink->add_inslave_proc ( m_slave_procs );
  }
  const T& read( ) 		{ return do_read( ); }
};

//------------------------------------------------------------------
template<class T> class sc_abstract_outslave_port_if : 
  public sc_abstract_slave_base< T, sc_abstract_outslave_if<T> >,
  public virtual sc_master_slave_write_if< T >
{
public:
  typedef sc_abstract_outslave_port_if<T>		this_type;
  typedef sc_abstract_outslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_abstract_inoutslave_port_if<T>& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  void operator( ) ( sc_abstract_inoutslave_port_if<T>& other ) { bind( other ); }

  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    if_type* alink  = get_master_slave_interface_dynamic( );
    alink->add_outslave( this );
    alink->add_outslave_proc ( m_slave_procs );
  }
  void write( const T& data )	{ do_write( data ); }
};

//------------------------------------------------------------------
template<class T> class sc_abstract_inoutslave_port_if : 
  public virtual sc_abstract_slave_base< T, sc_abstract_inoutslave_if<T> >,
  public virtual sc_master_slave_read_write_if< T >
{
public:
  typedef sc_abstract_inoutslave_port_if<T>		this_type;
  typedef sc_abstract_inoutslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link);  }
  void operator( ) ( this_type& other )		{ bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink  = get_master_slave_interface_dynamic( );
    alink->add_inoutslave( this );
    alink->add_inoutslave_proc ( m_slave_procs );
  }
  const T& read( ) 		{ return do_read( ); }
  void write( const T& data )	{ do_write( data ); }
  bool input( )			{ return get_master_slave_interface( )->input( ); }
};

#endif  // SC_ABSTRACT_IF_H

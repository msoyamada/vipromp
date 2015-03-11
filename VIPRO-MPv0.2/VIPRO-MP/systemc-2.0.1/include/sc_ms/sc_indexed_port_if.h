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

  sc_indexed_port_if.h -- interface for indexed master/slave ports.

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_INDEXED_PORT_IF_H
#define SC_INDEXED_PORT_IF_H
  
#include "sc_ms/sc_abstract_port_if.h"
#include "sc_ms/sc_indexed_if.h"


// forward declarations
template<class T, uint64> class sc_indexed_master_port_if;
template<class T, uint64> class sc_indexed_inmaster_port_if;
template<class T, uint64> class sc_indexed_outmaster_port_if;
template<class T, uint64> class sc_indexed_inoutmaster_port_if;

template<class T, uint64> class sc_indexed_slave_port_if;
template<class T, uint64> class sc_indexed_inslave_port_if;
template<class T, uint64> class sc_indexed_outslave_port_if;
template<class T, uint64> class sc_indexed_inoutslave_port_if;

//---------------------------------------------------------------------
// common base for all indexed master ports
template<class T, uint64 N, class IF>
class sc_indexed_master_base : public virtual sc_abstract_master_base<T, IF >
{
public:
  virtual sc_port_role port_role( ) { return SC_MASTER; }
  const char*	port_typename( ) { return "sc_indexed_master_slave"; }
  uint64 get_address( ) { 
    return get_master_slave_interface( )->get_address( );
  }

protected:
  void set_address( uint64 i ) {
    if ( i > N ) {
      char msg[BUFSIZ];
      sprintf( msg, "index out of range: port `%s\'", name( ) );
      REPORT_WARNING(0, msg );
    }
    get_master_slave_interface( )->set_address( i );
  }

};

//---------------------------------------------------------------------
// common base for all indexed slave ports
template<class T, uint64 N, class IF>
class sc_indexed_slave_base : public virtual sc_abstract_slave_base<T, IF >
{
public:
  uint64 get_address( ) { 
    return get_master_slave_interface( )->get_address( );
  }
  const char*	port_typename( ) { return "sc_indexed_master_slave"; }
};

////////////////////////////////////////////////////////////////////
// master port interface
//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_master_port_if : 
public virtual sc_indexed_master_base< T, N, sc_indexed_master_if<T> >
{
public:
  typedef sc_indexed_master_base< T, N, sc_indexed_master_if<T> > base_type;
  typedef sc_indexed_master_port_if<T,N>	this_type;
  typedef sc_indexed_master_if<T> if_type;

  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ return bind( link); }
  void operator( ) ( this_type& other )		{ return bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_master( this );
  }
  // run the slaves
  void operator ( ) ( ) { 
    get_master_slave_interface( )->operator( ) ( ); 
  }

  this_type& operator [ ] (uint64 i ) {  // used by indexed ports
    set_address( i );
    return *this;
  }
};


//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_inmaster_port_if : 
  public virtual sc_indexed_master_base< T, N, sc_indexed_inmaster_if<T> >,
  public virtual sc_master_slave_read_if< T >
{
public:
  typedef sc_indexed_master_base< T, N, sc_indexed_inmaster_if<T> > base_type;
  typedef sc_indexed_inmaster_port_if<T,N>		this_type;
  typedef sc_indexed_inmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_indexed_inoutmaster_port_if<T,N>& other) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )		{ bind( other); }
  void operator( ) ( sc_indexed_inoutmaster_port_if<T,N>& other) { bind( other ); }
  
  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_inmaster( this );
  }
  const T& read( ) { 
    reset_write_count( );
    return do_read( ); 
  }

  operator const T& ( ) {
      return read( );
  };
  
  this_type& operator [ ] (uint64 i ) {  // used by indexed ports
    set_address( i );
    return *this;
  }
};

//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_outmaster_port_if : 
  public virtual sc_indexed_master_base< T, N, sc_indexed_outmaster_if<T> >,
  public virtual sc_master_slave_write_if< T >
{
public:
  typedef sc_indexed_master_base< T, N, sc_indexed_outmaster_if<T> > base_type;
  typedef sc_indexed_outmaster_port_if<T,N>		this_type;
  typedef sc_indexed_outmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_indexed_inoutmaster_port_if<T,N>& other) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )		{ bind( other); }
  void operator( ) ( sc_indexed_inoutmaster_port_if<T,N>& other) { bind( other ); }

  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_outmaster( this );
  }
  void write( const T& data )	{ do_write( data ); }

  this_type& operator [ ] ( uint64 i ) {  // used by indexed ports
    set_address( i );
    return *this;
  }
  
  this_type& operator = ( const T& data ) {
    write( data );
    return *this;
  }
};

//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_inoutmaster_port_if : 
public virtual sc_indexed_master_base< T, N, sc_indexed_inoutmaster_if<T> >,
  public virtual sc_master_slave_read_write_if< T >
{
public:
  typedef sc_indexed_master_base< T, N, sc_indexed_inoutmaster_if<T> > base_type;
  typedef sc_indexed_inoutmaster_port_if<T,N>		this_type;
  typedef sc_indexed_inoutmaster_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  
  
  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_inoutmaster( this );
  }
  const T& read( ) { 
    reset_write_count( );
    return do_read( ); 
  }

  operator const T& ( ) {
      return read( );
  };
  
  void write( const T& data )	{ do_write( data ); }

  this_type& operator [ ] (uint64 i ) {  // used by indexed ports
    set_address( i );
    return *this;
  }
  
  this_type& operator = ( const T& data ) {
    write( data );
    return *this;
  }
};


////////////////////////////////////////////////////////////////////
// slave port interface
//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_slave_port_if : 
  public virtual sc_indexed_slave_base< T, N, sc_indexed_slave_if<T> >
{
public:
  typedef sc_indexed_slave_port_if<T,N>		this_type;
  typedef sc_indexed_slave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* /* link */ ) {
    if_type* alink = get_master_slave_interface_dynamic( );
    alink->add_slave( this );
    alink->add_slave_proc ( m_slave_procs );
  }
};


//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_inslave_port_if : 
  public virtual sc_indexed_slave_base< T, N, sc_indexed_inslave_if<T> >,
  public virtual sc_master_slave_read_if< T >
{
public:
  typedef sc_indexed_inslave_port_if<T,N>		this_type;
  typedef sc_indexed_inslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_indexed_inoutslave_port_if<T,N>& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  void operator( ) ( sc_indexed_outslave_port_if<T,N>& other ) { bind( other ); }
  
  
  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    if_type* alink  = get_master_slave_interface_dynamic(  );
    alink->add_inslave( this );
    alink->add_inslave_proc ( m_slave_procs );
  }
  const T& read( ) 		{ return do_read( ); }
  operator const T& ( )		{ return read( ); };
};

//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_outslave_port_if : 
  public sc_indexed_slave_base< T, N, sc_indexed_outslave_if<T> >,
  public virtual sc_master_slave_write_if< T >
{
public:
  typedef sc_indexed_outslave_port_if<T,N>		this_type;
  typedef sc_indexed_outslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  virtual void bind( sc_indexed_inoutslave_port_if<T,N>& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  void operator( ) ( sc_indexed_inoutslave_port_if<T,N>& other ) { bind( other ); }

  virtual void register_master_slave_port( sc_link_mp_if*  ) {
    if_type* alink  = get_master_slave_interface_dynamic( );
    alink->add_outslave( this );
    alink->add_outslave_proc ( m_slave_procs );
  }
  void write( const T& data )	{ do_write( data ); }
};

//------------------------------------------------------------------
template<class T, uint64 N> 
class sc_indexed_inoutslave_port_if : 
  public virtual sc_indexed_slave_base< T, N, sc_indexed_inoutslave_if<T> >,
  public virtual sc_master_slave_read_write_if< T >
{
public:
  typedef sc_indexed_inoutslave_port_if<T,N>		this_type;
  typedef sc_indexed_inoutslave_if<T> if_type;
  
  void bind( if_type& link ) {
    sc_port_base::bind( static_cast<sc_interface&>( link ) );
  }
  void bind( this_type& other ) {
    sc_port_base::bind( static_cast<sc_port_base&>( other ) );
  }
  
  void operator( ) ( if_type& link )		{ bind( link); }
  void operator( ) ( this_type& other )	{ bind( other); }
  
  virtual void register_master_slave_port( sc_link_mp_if* ) {
    if_type* alink  = get_master_slave_interface_dynamic( );
    alink->add_inoutslave( this );
    alink->add_inoutslave_proc( m_slave_procs );
  }
  const T& read( ) 		{ return do_read( ); }
  operator const T& ( )		{ return read( ); };
  
  void write( const T& data )	{ do_write( data ); }
  bool input( ) 		{ return get_master_slave_interface( )->input( ); }
};


#endif  // SC_INDEXED_IF_H

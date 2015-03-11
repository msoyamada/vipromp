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

  sc_link_mp.h -- implementation of multi-point link

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_LINK_MP_H
#define SC_LINK_MP_H

#include "systemc/utils/sc_string.h"
#include "sc_ms/sc_abstract_if.h"
#include "sc_ms/sc_indexed_if.h"
#include "sc_ms/sc_refined_if.h"

class sc_link_mp_b : public virtual sc_link_mp_if 
{
public:
  sc_string gen_unique_name( const char *nm=0 );
};
//----------------------------------------------------------------
// template<class T>
// class sc_link_mp
template<class T = int>
class sc_link_mp :
  public virtual sc_link_mp_b,
  // public virtual sc_link_mp_if,
  public virtual sc_abstract_master_if<T>,
  public virtual sc_abstract_inmaster_if<T>,
  public virtual sc_abstract_outmaster_if<T>,
  public virtual sc_abstract_inoutmaster_if<T>,
  public virtual sc_abstract_slave_if<T>,
  public virtual sc_abstract_inslave_if<T>,
  public virtual sc_abstract_outslave_if<T>,
  public virtual sc_abstract_inoutslave_if<T>,

  public virtual sc_indexed_master_if<T>,
  public virtual sc_indexed_inmaster_if<T>,
  public virtual sc_indexed_outmaster_if<T>,
  public virtual sc_indexed_inoutmaster_if<T>,
  public virtual sc_indexed_slave_if<T>,
  public virtual sc_indexed_inslave_if<T>,
  public virtual sc_indexed_outslave_if<T>,
  public virtual sc_indexed_inoutslave_if<T>,

  public virtual sc_refined_if<T>
{
public:
  sc_link_mp( const char* nm = 0 ) : 
    validated_( false )
  { 
    name_ = gen_unique_name( nm ? nm : "link" );
  }

public:
/*
  void		run_slaves( ) 		{  run( slave_procs ); }
  void		run_inslaves( ) 	{  run( inslave_procs ); }
  void		run_outslaves( )        {  run( outslave_procs ); }
  void		run_inoutslaves( )      {  run( inoutslave_procs ); }
  void	run( sc_slave_proc_list& list);
*/

  virtual void register_port( sc_port_base& port, const char* port_name);
  
  bool is_validated( ) 	{ return validated_; }
  void validate( );
  void add_port( sc_master_slave_base* port ) { port_list_.push_back( port ); }
  
  void	add_master( sc_master_slave_base* port ); 
  void  add_inmaster( sc_master_slave_base* port );
  void  add_outmaster( sc_master_slave_base* port );
  void  add_inoutmaster( sc_master_slave_base* port );
  
  void	add_slave( sc_master_slave_base* port ); 
  void	add_inslave( sc_master_slave_base* port ); 
  void	add_outslave( sc_master_slave_base* port ); 
  void	add_inoutslave( sc_master_slave_base* port ); 

  void	add_slave_proc( sc_slave_proc_list& l );
  void	add_inslave_proc( sc_slave_proc_list& l );
  void	add_outslave_proc( sc_slave_proc_list& l );
  void	add_inoutslave_proc( sc_slave_proc_list& l );

  void report_error( const char * str ) {
    // char msg[BUFSIZ];
    // sprintf( msg, "%s: %s" ,str, name() );
    REPORT_ERROR(0, str );
  } // report_error

  const sc_string& name( ) { return name_; }

private:
  void	add_to_list( sc_slave_proc_list& , sc_slave_proc_list& );
  bool	can_bind( sc_master_slave_base*, sc_master_slave_base *);

private:
  sc_string name_;
  sc_master_slave_list  port_list_;
  bool			validated_;

/*
  // the list of all slave processess attached to this link.
  sc_slave_proc_list	slave_procs;
  sc_slave_proc_list	inslave_procs;
  sc_slave_proc_list	outslave_procs;
  sc_slave_proc_list	inoutslave_procs;
*/

  sc_master_slave_list	master_list;
  sc_master_slave_list	inmaster_list;
  sc_master_slave_list	outmaster_list;
  sc_master_slave_list	inoutmaster_list;

  sc_master_slave_list	slave_list;
  sc_master_slave_list	inslave_list;
  sc_master_slave_list	outslave_list;
  sc_master_slave_list	inoutslave_list;
};  // sc_link_mp


////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------

//----------------------------------------------------------------------
template<class T>
void sc_link_mp<T>::register_port( sc_port_base& port, 
                                   const char* port_name) 
{
  sc_master_slave_base* ms_port 
    =  dynamic_cast< sc_master_slave_base* >( &port );
  
  if ( !ms_port ) {
    cout << "sc_link_mp.h::register_port : Error " 
      << port_name << " is not an master/slave port " << endl;
  }

  add_port( ms_port );
  // do the double dispatch
  ms_port->register_master_slave_port( this );
} // register_port

//----------------------------------------------------------------------
template<class T>
void sc_link_mp<T>::validate( ) 
{
  if ( is_validated( ) ) {
    return;
  }

  assert( port_list_.size( ) );
  sc_master_slave_list::iterator it	= port_list_.begin( );
  sc_master_slave_list::iterator last	= port_list_.end( );

  sc_master_slave_base *port = *it; // save the first one
  bool valid = true; 

  for ( it++ ; ( it != last) && valid ; it++ ) {
    valid = can_bind( port, *it );
  }
  validated_ = true;
} // validate

//----------------------------------------------------------------------
template<class T>
bool sc_link_mp<T>::can_bind( sc_master_slave_base* port1, 
                              sc_master_slave_base* port2 )
{
  if ( ! port1->can_bind( port2 ) ) {
    char msg[BUFSIZ];
    sprintf( msg,  
	     " incompatible port protocols : `%s\' and `%s\'  : Cannot bind port `%s\' to link `%s\'",
	     port1->port_typename( ), 
	     port2->port_typename( ), 
	     port1->name( ), 
	     this->name( ).c_str( ) );

    REPORT_ERROR( 0, msg );
  }
  return true;
}


////////////////////////////////////////////////////////////////////
// add master ports to the appropriate list
template<class T> 
void sc_link_mp<T>::add_master( sc_master_slave_base* port ) { 
  assert( port->port_role( ) == SC_MASTER );
  master_list.push_back( port ); 
}

template<class T> 
void sc_link_mp<T>::add_inmaster( sc_master_slave_base* port ) {
  assert( port->port_role( ) == SC_MASTER );
  inmaster_list.push_back( port );
}

template<class T> 
void sc_link_mp<T>::add_outmaster( sc_master_slave_base* port ) {
  assert( port->port_role( ) == SC_MASTER );
  outmaster_list.push_back( port );
}

template<class T> 
void sc_link_mp<T>::add_inoutmaster( sc_master_slave_base* port ) {
  assert( port->port_role( ) == SC_MASTER );
  inoutmaster_list.push_back( port );
}
  

////////////////////////////////////////////////////////////////////
// add slave ports to the appropriate list
template<class T> 
void sc_link_mp<T>::add_slave( sc_master_slave_base* port ) { 
  assert( port->port_role( ) == SC_SLAVE );
  slave_list.push_back( port );
}

template<class T> 
void sc_link_mp<T>::add_inslave( sc_master_slave_base* port ) { 
  assert( port->port_role( ) == SC_SLAVE );
  inslave_list.push_back( port );
}

template<class T> 
void sc_link_mp<T>::add_outslave( sc_master_slave_base* port ) { 
  assert( port->port_role( ) == SC_SLAVE );
  outslave_list.push_back( port );
}

template<class T> 
void sc_link_mp<T>::add_inoutslave( sc_master_slave_base* port ) { 
  assert( port->port_role( ) == SC_SLAVE );
  inoutslave_list.push_back( port );
}

////////////////////////////////////////////////////////////////////
// add slave procs to the appropriate list
template<class T> 
void sc_link_mp<T>::add_slave_proc( sc_slave_proc_list& l ) {
  add_to_list( slave_procs, l );
}
template<class T> 
void sc_link_mp<T>::add_inslave_proc( sc_slave_proc_list& l )  {
  add_to_list( inslave_procs, l );
}

template<class T> 
void sc_link_mp<T>::add_outslave_proc( sc_slave_proc_list& l ) {
  add_to_list( outslave_procs, l );
}

template<class T> 
void sc_link_mp<T>::add_inoutslave_proc( sc_slave_proc_list& l )  {
  add_to_list( inoutslave_procs, l );
}


////////////////////////////////////////////////////////////////////
template<class T> 
void sc_link_mp<T>::add_to_list( sc_slave_proc_list& list1, 
			   sc_slave_proc_list& list2 ) 
{
  for ( sc_slave_proc_list::iterator 
	it 	= list2.begin( ),
	last 	= list2.end( );
       last != it;
       it++ ) {
    list1.push_back( *it );
  }
}


#endif  // SC_LINK_MP_H

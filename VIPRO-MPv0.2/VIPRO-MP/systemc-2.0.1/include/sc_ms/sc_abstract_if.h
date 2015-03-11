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

  sc_abstract_if.h -- interface for abstract protocols

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_ABSTRACT_IF_H
#define SC_ABSTRACT_IF_H

#include "sc_ms/sc_link_mp_if.h"
			   

// link mp interface for abstract ports
//	only for **virtual** derivation, cannot be instantiated
class sc_abstract_b_if : public virtual sc_link_mp_if {
protected:
  sc_abstract_b_if( ) {  }

public:
  virtual void add_slave_proc( sc_slave_proc_list& ) = 0;
  virtual void add_inslave_proc( sc_slave_proc_list& ) = 0;
  virtual void add_outslave_proc( sc_slave_proc_list& ) = 0;
  virtual void add_inoutslave_proc( sc_slave_proc_list& ) = 0;

/*
  virtual void run_slaves( )		=0;
  virtual void run_inslaves( )		=0;
  virtual void run_outslaves( )		=0;
  virtual void run_inoutslaves( )	=0;
*/

  void		run_slaves( ) 		{  run( slave_procs ); }
  void		run_inslaves( ) 	{  run( inslave_procs ); }
  void		run_outslaves( )        {  run( outslave_procs ); }
  void		run_inoutslaves( )      {  run( inoutslave_procs ); }

  void 		run( sc_slave_proc_list& list) {
    int size = list.size( );
    sc_slave_handle* l_slaves_static = list.raw_data( );
    sc_slave_handle h;
    
    for(int i = 0; i < size ; i++) {
      h = l_slaves_static[i];
      execute_slave_process( h );
    }
  }

  virtual void add_master	( sc_master_slave_base* )	=0;
  virtual void add_inmaster	( sc_master_slave_base* )	=0;
  virtual void add_outmaster	( sc_master_slave_base* )	=0;
  virtual void add_inoutmaster	( sc_master_slave_base* )	=0;
  
  virtual void add_slave	( sc_master_slave_base* )	=0;
  virtual void add_inslave	( sc_master_slave_base* )	=0;
  virtual void add_outslave	( sc_master_slave_base* )	=0;
  virtual void add_inoutslave	( sc_master_slave_base* )	=0;

  sc_slave_proc_list	slave_procs;
  sc_slave_proc_list	inslave_procs;
  sc_slave_proc_list	outslave_procs;
  sc_slave_proc_list	inoutslave_procs;

};
			   
//---------------------------------------------------------------
// templatized abstract link 	  
template<class T>
class sc_abstract_if : public virtual sc_abstract_b_if
{
public:
  // constructor
  sc_abstract_if( ) : write_count( 0 ) {  }
  void		reset_write_count( )	{ write_count = 0; }
  const T&	read( )			{ return value; }
  
  // write data and check to see if there have been any other writes
  void		write( const T& data ) {
    if ( write_count > 0 ) {
      report_error( "Cannot write twice to the same link" );
    }
    write_count++;
    value = data;
  }
  
protected:
  void		free_write( const T& data ) { value = data; } 
  bool		get_input( )		{ return is_input; }
  void 		set_input( bool b )	{ is_input = b; }

protected:
  //void		run( sc_slave_proc_list& list)	=0;

private:
  T 	value;
  int	write_count;	// for error checking, only one slave can write
  bool	is_input;	// are we currently reading or writing
			// inout slaves can use this information to
			// respond in the read or write mode.
};

//-------------------------------------------------------------------
template<class T> 
class sc_abstract_master_if : public virtual sc_abstract_if< T >
{
public:
  void operator ( ) ( ) { 
    run_slaves( );
  }
};

template<class T> 
class sc_abstract_inmaster_if :  public virtual sc_abstract_if< T >
{
public:
  const T& read( ) {
    reset_write_count( );
    run_outslaves( );
    run_inoutslaves( );
    return sc_abstract_if<T>::read( );
  }
};

template<class T> 
class sc_abstract_outmaster_if : public virtual sc_abstract_if< T >
{
public:
  
  void write(const T& data ) {
    sc_abstract_if<T>::free_write( data );
    run_inslaves( );
    run_inoutslaves( );
  } 
};

template<class T> 
class sc_abstract_inoutmaster_if :  public virtual sc_abstract_if< T >
{
public:
  void write(const T& data ) {
    set_input( true );
    sc_abstract_if<T>::free_write( data );
    run_inslaves( );
    run_inoutslaves( );
  }
  
  const T& read( ) {
    reset_write_count( );
    set_input( false );
    run_outslaves( );
    run_inoutslaves( );
    return sc_abstract_if<T>::read( );
  }
  bool input( ) { return get_input( ); }
};


//-------------------------------------------------------------------
template<class T> 
class sc_abstract_slave_if : public virtual sc_abstract_if< T >
{
public:
};

template<class T> 
class sc_abstract_inslave_if : public virtual sc_abstract_if< T >
{
public:
};

template<class T> 
class sc_abstract_outslave_if : public virtual sc_abstract_if< T >
{
public:
};

template<class T> 
class sc_abstract_inoutslave_if : public virtual sc_abstract_if< T >
{
public:
  bool input( ) { return get_input ( ); }
};

#endif  // SC_ABSTRACT_IF_H

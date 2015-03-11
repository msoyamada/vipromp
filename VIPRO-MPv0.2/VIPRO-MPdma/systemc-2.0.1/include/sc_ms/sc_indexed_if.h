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

  sc_indexed_if.h -- implementation of indexed port

  Original Author: Dirk Vermeersch, Coware, Inc.
                   Vijay Kumar, Coware, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICAT,NION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/
#ifndef SC_INDEXED_IF_H
#define SC_INDEXED_IF_H

#include "sc_ms/sc_abstract_if.h"

template< class T >
class sc_indexed_if : public virtual sc_abstract_if<T>
{
public:
  void		set_address( uint64 i )	{ last_address = i; }
  uint64	get_address( ) const	{ return last_address; }
protected:
  uint64	last_address;
};

//-------------------------------------------------------------------
template<class T> 
class sc_indexed_master_if : public virtual sc_indexed_if< T >
{
public:
  void operator ( ) ( ) { 
    run_slaves( );
  }
};

template<class T> 
class sc_indexed_inmaster_if :  public virtual sc_indexed_if< T >
{
public:
  const T& read( ) {
    reset_write_count( );
    run_outslaves( );
    run_inoutslaves( );
    return sc_indexed_if<T>::read( );
  }
};

template<class T> 
class sc_indexed_outmaster_if : public virtual sc_indexed_if< T >
{
public:
  
  void write(const T& data ) {
    sc_indexed_if<T>::free_write( data );
    run_inslaves( );
    run_inoutslaves( );
  } 
};

template<class T> 
class sc_indexed_inoutmaster_if :  public virtual sc_indexed_if< T >
{
public:
  void write(const T& data ) {
    set_input( true );
    sc_indexed_if<T>::free_write( data );
    run_inslaves( );
    run_inoutslaves( );
  }
  
  const T& read( ) {
    reset_write_count( );
    set_input( false );
    run_outslaves( );
    run_inoutslaves( );
    return sc_indexed_if<T>::read( );
  }
  bool input( ) { return get_input( ); }
};


//-------------------------------------------------------------------
template<class T> 
class sc_indexed_slave_if : public virtual sc_indexed_if< T >
{
public:
};

template<class T> 
class sc_indexed_inslave_if : public virtual sc_indexed_if< T >
{
public:
};

template<class T> 
class sc_indexed_outslave_if : public virtual sc_indexed_if< T >
{
public:
};

template<class T> 
class sc_indexed_inoutslave_if : public virtual sc_indexed_if< T >
{
public:
  bool input( ) { return get_input ( ); }
};


#endif // SC_INDEXED_IF_H

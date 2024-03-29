/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2002 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.3 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_cor.h -- Coroutine abstract base classes.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-12-18

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef SC_COR_H
#define SC_COR_H


#include <assert.h>
#include <stdlib.h>

class sc_simcontext;


// ----------------------------------------------------------------------------
//  TYPEDEF : sc_cor_fn
//
//  Function type for creating coroutines.
// ----------------------------------------------------------------------------

typedef void (sc_cor_fn)( void* );


// ----------------------------------------------------------------------------
//  CLASS : sc_cor
//
//  Coroutine abstract base class.
// ----------------------------------------------------------------------------

class sc_cor
{
protected:

    // constructor
    sc_cor() {}

public:

    // destructor
    virtual ~sc_cor() {}

    // switch stack protection on/off
    virtual void stack_protect( bool enable ) {}

private:

    // disabled
    sc_cor( const sc_cor& );
    sc_cor& operator = ( const sc_cor& );
};


// ----------------------------------------------------------------------------
//  CLASS : sc_cor_pkg
//
//  Coroutine package abstract base class.
// ----------------------------------------------------------------------------

class sc_cor_pkg
{
public:

    // constructor
    sc_cor_pkg( sc_simcontext* simc )
        : m_simc( simc ) { assert( simc != 0 ); }

    // destructor
    virtual ~sc_cor_pkg() {}

    // create a new coroutine
    virtual sc_cor* create( size_t stack_size, sc_cor_fn* fn, void* arg ) = 0;

    // yield to the next coroutine
    virtual void yield( sc_cor* next_cor ) = 0;

    // abort the current coroutine (and resume the next coroutine)
    virtual void abort( sc_cor* next_cor ) = 0;

    // get the main coroutine
    virtual sc_cor* get_main() = 0;

    // get the simulation context
    sc_simcontext* simcontext()
        { return m_simc; }

private:

    sc_simcontext* m_simc;

private:

    // disabled
    sc_cor_pkg();
    sc_cor_pkg( const sc_cor_pkg& );
    sc_cor_pkg& operator = ( const sc_cor_pkg& );
};


#endif

// Taf!

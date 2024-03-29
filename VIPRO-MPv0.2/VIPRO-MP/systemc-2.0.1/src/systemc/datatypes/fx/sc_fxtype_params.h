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

  sc_fxtype_params.h - 

  Original Author: Martin Janssen, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef SC_FXTYPE_PARAMS_H
#define SC_FXTYPE_PARAMS_H


#include "systemc/datatypes/fx/sc_context.h"


namespace sc_dt
{

// classes defined in this module
class sc_fxtype_params;


// ----------------------------------------------------------------------------
//  CLASS : sc_fxtype_params
//
//  Fixed-point type parameters class.
// ----------------------------------------------------------------------------

class sc_fxtype_params
{
public:

             sc_fxtype_params();
             sc_fxtype_params( int, int );
             sc_fxtype_params(           sc_q_mode, sc_o_mode, int = 0 );
             sc_fxtype_params( int, int, sc_q_mode, sc_o_mode, int = 0 );
             sc_fxtype_params( const sc_fxtype_params& );
	     sc_fxtype_params( const sc_fxtype_params&,
			       int, int );
	     sc_fxtype_params( const sc_fxtype_params&,
			                 sc_q_mode, sc_o_mode, int = 0 );
    explicit sc_fxtype_params( sc_without_context );

    sc_fxtype_params& operator = ( const sc_fxtype_params& );

    friend bool operator == ( const sc_fxtype_params&,
                              const sc_fxtype_params& );
    friend bool operator != ( const sc_fxtype_params&,
			      const sc_fxtype_params& );

    int wl() const;
    void wl( int );

    int iwl() const;
    void iwl( int );

    sc_q_mode q_mode() const;
    void q_mode( sc_q_mode );

    sc_o_mode o_mode() const;
    void o_mode( sc_o_mode );

    int n_bits() const;
    void n_bits( int );

    const sc_string to_string() const;

    void print( ostream& = cout ) const;
    void dump( ostream& = cout ) const;

private:

    int       m_wl;
    int       m_iwl;
    sc_q_mode m_q_mode;
    sc_o_mode m_o_mode;
    int       m_n_bits;
};


// ----------------------------------------------------------------------------
//  TYPEDEF : sc_fxtype_context
//
//  Context type for the fixed-point type parameters.
// ----------------------------------------------------------------------------

typedef sc_context<sc_fxtype_params> sc_fxtype_context;


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
sc_fxtype_params::sc_fxtype_params()
{
    *this = sc_fxtype_context::default_value();
}

inline
sc_fxtype_params::sc_fxtype_params( int wl_, int iwl_ )
{
    *this = sc_fxtype_context::default_value();

    SC_CHECK_WL_( wl_ );
    m_wl  = wl_;
    m_iwl = iwl_;
}

inline
sc_fxtype_params::sc_fxtype_params( sc_q_mode q_mode_,
                                    sc_o_mode o_mode_, int n_bits_ )
{
    *this = sc_fxtype_context::default_value();

    SC_CHECK_N_BITS_( n_bits_ );
    m_q_mode = q_mode_;
    m_o_mode = o_mode_;
    m_n_bits = n_bits_;
}

inline
sc_fxtype_params::sc_fxtype_params( int wl_, int iwl_,
                                    sc_q_mode q_mode_,
                                    sc_o_mode o_mode_, int n_bits_ )
{
    SC_CHECK_WL_( wl_ );
    SC_CHECK_N_BITS_( n_bits_ );
    m_wl     = wl_;
    m_iwl    = iwl_;
    m_q_mode = q_mode_;
    m_o_mode = o_mode_;
    m_n_bits = n_bits_;
}

inline
sc_fxtype_params::sc_fxtype_params( const sc_fxtype_params& a )
: m_wl( a.m_wl ), m_iwl( a.m_iwl ),
  m_q_mode( a.m_q_mode ),
  m_o_mode( a.m_o_mode ), m_n_bits( a.m_n_bits )
{}

inline
sc_fxtype_params::sc_fxtype_params( const sc_fxtype_params& a,
				    int wl_, int iwl_ )
: m_wl( wl_ ), m_iwl( iwl_ ),
  m_q_mode( a.m_q_mode ),
  m_o_mode( a.m_o_mode ), m_n_bits( a.m_n_bits )
{}

inline
sc_fxtype_params::sc_fxtype_params( const sc_fxtype_params& a,
				    sc_q_mode q_mode_,
				    sc_o_mode o_mode_, int n_bits_ )
: m_wl( a.m_wl ), m_iwl( a.m_iwl ),
  m_q_mode( q_mode_ ),
  m_o_mode( o_mode_ ), m_n_bits( n_bits_ )
{}

inline
sc_fxtype_params::sc_fxtype_params( sc_without_context )
: m_wl    ( SC_DEFAULT_WL_ ),
  m_iwl   ( SC_DEFAULT_IWL_ ),
  m_q_mode( SC_DEFAULT_Q_MODE_ ),
  m_o_mode( SC_DEFAULT_O_MODE_ ),
  m_n_bits( SC_DEFAULT_N_BITS_ )
{}


inline
sc_fxtype_params&
sc_fxtype_params::operator = ( const sc_fxtype_params& a )
{
    if( &a != this )
    {
        m_wl     = a.m_wl;
	m_iwl    = a.m_iwl;
	m_q_mode = a.m_q_mode;
	m_o_mode = a.m_o_mode;
	m_n_bits = a.m_n_bits;
    }
    return *this;
}


inline
bool
operator == ( const sc_fxtype_params& a, const sc_fxtype_params& b )
{
    return ( a.m_wl     == b.m_wl     &&
	     a.m_iwl    == b.m_iwl    &&
	     a.m_q_mode == b.m_q_mode &&
	     a.m_o_mode == b.m_o_mode &&
	     a.m_n_bits == b.m_n_bits );
}

inline
bool
operator != ( const sc_fxtype_params& a, const sc_fxtype_params& b )
{
    return ( a.m_wl     != b.m_wl     ||
	     a.m_iwl    != b.m_iwl    ||
	     a.m_q_mode != b.m_q_mode ||
	     a.m_o_mode != b.m_o_mode ||
	     a.m_n_bits != b.m_n_bits );
}


inline
int
sc_fxtype_params::wl() const
{
    return m_wl;
}

inline
void
sc_fxtype_params::wl( int wl_ )
{
    SC_CHECK_WL_( wl_ );
    m_wl = wl_;
}


inline
int
sc_fxtype_params::iwl() const
{
    return m_iwl;
}

inline
void
sc_fxtype_params::iwl( int iwl_ )
{
    m_iwl = iwl_;
}


inline
sc_q_mode
sc_fxtype_params::q_mode() const
{
    return m_q_mode;
}

inline
void
sc_fxtype_params::q_mode( sc_q_mode q_mode_ )
{
    m_q_mode = q_mode_;
}


inline
sc_o_mode
sc_fxtype_params::o_mode() const
{
    return m_o_mode;
}

inline
void
sc_fxtype_params::o_mode( sc_o_mode o_mode_ )
{
    m_o_mode = o_mode_;
}


inline
int
sc_fxtype_params::n_bits() const
{
    return m_n_bits;
}

inline
void
sc_fxtype_params::n_bits( int n_bits_ )
{
    SC_CHECK_N_BITS_( n_bits_ );
    m_n_bits = n_bits_;
}


inline
ostream&
operator << ( ostream& os, const sc_fxtype_params& a )
{
    a.print( os );
    return os;
}

} // namespace sc_dt


#endif

// Taf!

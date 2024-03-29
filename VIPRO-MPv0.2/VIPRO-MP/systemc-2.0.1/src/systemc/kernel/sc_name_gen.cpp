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

  sc_name_gen.cpp -- Unique name generator.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "systemc/kernel/sc_kernel_ids.h"
#include "systemc/kernel/sc_name_gen.h"
#include "systemc/utils/sc_iostream.h"


// ----------------------------------------------------------------------------
//  CLASS : sc_name_gen
//
//  Unique name generator class.
// ----------------------------------------------------------------------------

sc_name_gen::sc_name_gen()
{}

sc_name_gen::~sc_name_gen()
{
    sc_strhash<int*>::iterator it( m_unique_name_map );
    for( ; ! it.empty(); it ++ ) {
	delete it.contents();
    }
    m_unique_name_map.erase();
}


// to generate unique names for objects in an MT-Safe way

const char*
sc_name_gen::gen_unique_name( const char* basename_ )
{
    if( basename_ == 0 ) {
	SC_REPORT_ERROR( SC_ID_GEN_UNIQUE_NAME_, 0 );
    }
    int* c = m_unique_name_map[basename_];
    if( c == 0 ) {
	c = new int( -1 );
	m_unique_name_map.insert( CCAST<char*>( basename_ ), c );
    }
    sprintf( m_unique_name, "%s_%d", basename_, ++ (*c) );
    return m_unique_name;
}


// Taf!

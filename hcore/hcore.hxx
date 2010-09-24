/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hcore.hxx
 * \brief Basic system level stuff.
 *
 * This file holds (except HException class declaration) main #defines,
 * macros and global variables used acros whole yaal.
 */

#ifndef YAAL_HCORE_HCORE_HXX_INCLUDED
#define YAAL_HCORE_HCORE_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hprogramoptionshandler.hxx"

namespace yaal
{

namespace hcore
{

HProgramOptionsHandler& yaal_options( void );
bool eq( double long, double long );
void set_env( HString const&, HString const&, bool = true );
void decode_set_env( HString );
void init_locale( char const* const = NULL );
void banner( void );
double long strtold( HString const& );

namespace extendable
{

typedef double long ( *yaal_strtold_t )( HString const& );
void set_strtold_impl( yaal_strtold_t );

}

}

}



#endif /* not YAAL_HCORE_HCORE_HXX_INCLUDED */


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	base.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "base.h"

#include "hstring.h"

using namespace yaal::hcore;

namespace yaal
{

template<>
int long lexical_cast( HString const& val )
	{
	int l_iBase = 10;
	if ( ( val.get_length() > 2 ) && ( val[ 1 ] == 'x' ) )
		l_iBase = 16;
	return ( ::strtol( val.raw(), NULL, l_iBase ) );
	}

template<>
int lexical_cast( HString const& val )
	{
	return ( static_cast<int>( lexical_cast<int long>( val ) ) );
	}

template<>
int lexical_cast( char const* const& val )
	{
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	}

template<>
int lexical_cast( char* const& val )
	{
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	}

template<>
double long lexical_cast( HString const& val )
	{
	return ( ::strtold( val.raw(), NULL ) );
	}

template<>
double lexical_cast( HString const& val )
	{
	return ( static_cast<double>( lexical_cast<double long>( val ) ) );
	}

char const* error_message( int a_iCode )
	{
	return ( ::strerror( a_iCode ) );
	}

}


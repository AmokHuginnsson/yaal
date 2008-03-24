/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstringstream.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hstringstream.h"

namespace yaal
{

namespace tools
{

HStringStream::HStringStream( void ) : f_bUsed( false ), f_oBuffer( "" )
	{
	}

HStringStream::HStringStream( char const* const a_pcInit ) : f_bUsed( false ), f_oBuffer( a_pcInit )
	{
	}

char const* const HStringStream::raw( void ) const
	{
	return ( f_oBuffer.raw() );
	}

yaal::hcore::HString const& HStringStream::string( void ) const
	{
	return ( f_oBuffer );
	}

int HStringStream::do_write( void const* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( f_bUsed )
		{
		f_oBuffer.clear();
		f_bUsed = false;
		}
	f_oBuffer.insert( f_oBuffer.get_length(), a_iSize, static_cast<char const* const>( a_pvBuffer ) );
	return ( a_iSize );
	M_EPILOG
	}

void HStringStream::do_flush( void ) const
	{
	f_oBuffer.clear();
	}

int HStringStream::do_read( void* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	int l_iLength = f_oBuffer.get_length();
	l_iLength = ( l_iLength < a_iSize ? l_iLength : a_iSize );
	strncpy( static_cast<char* const>( a_pvBuffer ), f_oBuffer, l_iLength );
	return ( l_iLength );
	M_EPILOG
	}

void HStringStream::use( void ) const
	{
	f_bUsed = true;
	}

void HStringStream::clear( void )
	{
	M_PROLOG
	f_oBuffer.clear();
	return;
	M_EPILOG
	}

char const* const operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& stream )
	{
	stream.use();
	return ( stream.raw() );
	}

}

}


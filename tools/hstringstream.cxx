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

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HStringStream::HStringStream( void ) : f_bUsed( false ), f_oBuffer( "" )
	{
	}

HStringStream::HStringStream( HString const& a_oInit ) : f_bUsed( false ), f_oBuffer( a_oInit )
	{
	}

HStringStream& HStringStream::operator = ( HString const& s )
	{
	f_oBuffer = s;
	f_bUsed = false;
	return ( *this );
	}

char const* HStringStream::raw( void ) const
	{
	return ( f_oBuffer.raw() );
	}

yaal::hcore::HString const& HStringStream::string( void ) const
	{
	return ( f_oBuffer );
	}

int long HStringStream::do_write( void const* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	if ( f_bUsed )
		{
		f_oBuffer.clear();
		f_bUsed = false;
		}
	f_oBuffer.insert( f_oBuffer.get_length(), a_lSize, static_cast<char const* const>( a_pvBuffer ) );
	return ( a_lSize );
	M_EPILOG
	}

void HStringStream::do_flush( void ) const
	{
	M_PROLOG
	f_oBuffer.clear();
	return;
	M_EPILOG
	}

int long HStringStream::do_read( void* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	int long l_iLength = f_oBuffer.get_length();
	l_iLength = ( l_iLength < a_lSize ? l_iLength : a_lSize );
	::strncpy( static_cast<char* const>( a_pvBuffer ), f_oBuffer.raw(), l_iLength );
	return ( l_iLength );
	M_EPILOG
	}

void HStringStream::use( void ) const
	{
	f_bUsed = true;
	return;
	}

void HStringStream::clear( void )
	{
	M_PROLOG
	f_oBuffer.clear();
	return;
	M_EPILOG
	}

char const* HStringStream::consume( void ) const
	{
	M_PROLOG
	use();
	return ( f_oBuffer.raw() );
	M_EPILOG
	}

char const* operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& stream )
	{
	M_PROLOG
	stream.use();
	return ( stream.raw() );
	M_EPILOG
	}

bool HStringStream::is_empty( void ) const
	{
	return ( f_bUsed || f_oBuffer.is_empty() );
	}

}

}


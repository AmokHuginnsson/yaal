/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreaminterface.cxx - this file is integral part of `yaal' project.

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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hstreaminterface.h"

#include "hcore/hstring.h"

namespace yaal
{

namespace hcore
{

char const* const HStreamInterface::eols = "\r\n"; /* order matters */

HStreamInterface::HStreamInterface( void ) : f_oCache( 1, cache_t::D_AUTO_GROW )
	{
	return;
	}

HStreamInterface::~HStreamInterface( void )
	{
	return;
	}

HStreamInterface& HStreamInterface::operator << ( char const* const a_pcString )
	{
	M_PROLOG
	if ( a_pcString )
		do_write( a_pcString, strlen( a_pcString ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( char const a_cChar )
	{
	M_PROLOG
	HString str( a_cChar );
	do_write( str, sizeof ( char const ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int const a_iInteger )
	{
	M_PROLOG
	int long l_lTmp = a_iInteger;
	return ( operator << ( l_lTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int unsigned const a_uiUnsignedInteger )
	{
	M_PROLOG
	int long unsigned l_ulTmp = a_uiUnsignedInteger;
	return ( operator << ( l_ulTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int long const a_lLongInteger )
	{
	M_PROLOG
	HString str( a_lLongInteger );
	do_write( str, str.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int long unsigned const a_ulUnsignedLongInteger )
	{
	M_PROLOG
	HString str( a_ulUnsignedLongInteger );
	do_write( str, str.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( double const a_dDouble )
	{
	M_PROLOG
	HString str( a_dDouble );
	do_write( str, str.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( void const* const a_pvPtr )
	{
	M_PROLOG
	HString str( a_pvPtr );
	do_write( str, str.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( HStreamInterface& ( *const HFILE_INTERFACE )( HStreamInterface& ) )
	{
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
	}

HStreamInterface& endl( HStreamInterface& a_roFile )
	{
	M_PROLOG
	a_roFile.do_write( "\n", 1 );
	return ( a_roFile );
	M_EPILOG
	}

HStreamInterface& flush( HStreamInterface& a_roFile )
	{
	M_PROLOG
	a_roFile.do_flush();
	return ( a_roFile );
	M_EPILOG
	}

int HStreamInterface::read_until( HString& a_roMessage, char const* const a_pcStopSet, bool a_bStripDelim )
	{
	M_PROLOG
	int l_iCtr = 0;
	char* l_pcBuffer = NULL;
	int err = 0;
	do
		{
		f_oCache.pool_realloc( l_iCtr + 2 );
		l_pcBuffer = f_oCache.raw();
		err = do_read( l_pcBuffer + l_iCtr, sizeof ( char ) * 1 );
		}
	while ( ( err > 0 ) && ! ::strchr( a_pcStopSet, l_pcBuffer[ l_iCtr ++ ] ) );
	if ( a_bStripDelim && ( err > 0 ) )
		-- l_iCtr;
	if ( l_iCtr >= 0 )
		l_pcBuffer[ l_iCtr ] = 0;
	a_roMessage = l_pcBuffer;
	return ( l_iCtr );
	M_EPILOG
	}

int HStreamInterface::read( void* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	return ( do_read( a_pvBuffer, a_iSize ) );
	M_EPILOG
	}

int HStreamInterface::write( void const* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	return ( do_write( a_pvBuffer, a_iSize ) );
	M_EPILOG
	}

}

}


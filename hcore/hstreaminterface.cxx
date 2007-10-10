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

HStreamInterface::HStreamInterface( void ) : f_oVarTmpBuffer()
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
	do_write_string( a_pcString );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( char const a_cChar )
	{
	M_PROLOG
	HString str( a_cChar );
	do_write_string( str );
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
	do_write_string( str );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int long unsigned const a_ulUnsignedLongInteger )
	{
	M_PROLOG
	HString str( a_ulUnsignedLongInteger );
	do_write_string( str );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( double const a_dDouble )
	{
	M_PROLOG
	HString str( a_dDouble );
	do_write_string( str );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( void const* const a_pvPtr )
	{
	M_PROLOG
	HString str( a_pvPtr );
	do_write_string( str );
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
	a_roFile.do_write_string( "\n" );
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

int HStreamInterface::read_until( HString& a_roMessage, char const* const a_pcStopSet )
	{
	M_PROLOG
	int l_iCtr = 0;
	char* l_pcPtr = NULL;
	a_roMessage = "";
	do
		{
		f_oVarTmpBuffer.hs_realloc( l_iCtr + 1 );
		l_pcPtr = f_oVarTmpBuffer.raw();
		if ( do_read( l_pcPtr + l_iCtr, sizeof ( char ) * 1 ) <= 0 )
			break;
		}
	while ( ! ::strchr( a_pcStopSet, l_pcPtr[ l_iCtr ++ ] ) );
	l_iCtr --; /* go back one char for stripping terminator */
	if ( l_iCtr > 0 )
		{
		l_pcPtr[ l_iCtr ] = 0;
		if ( l_iCtr > 0 )
			{
			a_roMessage.hs_realloc( l_iCtr );
			::memcpy( a_roMessage.raw(), l_pcPtr, l_iCtr + 1 );
			}
		}
	return ( l_iCtr );
	M_EPILOG
	}

}

}


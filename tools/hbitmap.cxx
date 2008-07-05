/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hbitmap.cxx - this file is integral part of `yaal' project.

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
#include <cstdio>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hbitmap.h"
#include "hcore/xalloc.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HBitmap::HBitmap( void )
	: f_lAllocatedBytes( 0 ), f_lSize( 0 ), f_pvBlock( NULL )
	{
	return;
	}

HBitmap::~HBitmap( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

void HBitmap::clear( void )
	{
	if ( f_lAllocatedBytes )
		xfree( f_pvBlock );
	f_pvBlock = NULL;
	f_lAllocatedBytes = 0;
	f_lSize = 0;
	return;
	}

HBitmap::HBitmap( int long const& a_lSize )
	: f_lAllocatedBytes( ( a_lSize >> 3 ) + ( ( a_lSize & 7 ) ? 1 : 0 ) ),
	f_lSize( a_lSize ), f_pvBlock( NULL )
	{
	M_PROLOG
	M_ASSERT( a_lSize > 0 );
	f_pvBlock = xcalloc<char>( f_lAllocatedBytes );
	M_ENSURE( f_pvBlock );
	return;
	M_EPILOG
	}

HBitmap::HBitmap( HBitmap const& b )
	: f_lAllocatedBytes( 0 ), f_lSize( 0 ), f_pvBlock( NULL )
	{
	M_PROLOG
	operator = ( b );
	return;
	M_EPILOG
	}

HBitmap& HBitmap::operator = ( HBitmap const& b )
	{
	M_PROLOG
	if ( &b != this )
		{
		clear();
		f_lAllocatedBytes = b.f_lAllocatedBytes;
		f_lSize = b.f_lSize;
		f_pvBlock = f_lAllocatedBytes ? xcalloc<char>( f_lAllocatedBytes ) : b.f_pvBlock;
		M_ENSURE( ! f_lSize || f_pvBlock );
		if ( f_pvBlock )
			{
			int long unsigned copyBytes = ( ( f_lSize >> 3 ) + ( ( f_lSize & 7 ) ? 1 : 0 ) );
			::memcpy( f_pvBlock, b.f_pvBlock, copyBytes );
			}
		}
	return ( *this );
	M_EPILOG
	}

void HBitmap::use( void* a_pvBlock, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( a_lSize > 0 );
	clear();
	f_lSize = a_lSize;
	f_pvBlock = a_pvBlock;
	return;
	M_EPILOG
	}

int long unsigned g_pulMaskBitSet[] = 
	{
	yaal::power<2,31>::value,
	yaal::power<2,30>::value,
	yaal::power<2,29>::value,
	yaal::power<2,28>::value,
	yaal::power<2,27>::value,
	yaal::power<2,26>::value,
	yaal::power<2,25>::value,
	yaal::power<2,24>::value,
	yaal::power<2,23>::value,
	yaal::power<2,22>::value,
	yaal::power<2,21>::value,
	yaal::power<2,20>::value,
	yaal::power<2,19>::value,
	yaal::power<2,18>::value,
	yaal::power<2,17>::value,
	yaal::power<2,16>::value,
	yaal::power<2,15>::value,
	yaal::power<2,14>::value,
	yaal::power<2,13>::value,
	yaal::power<2,12>::value,
	yaal::power<2,11>::value,
	yaal::power<2,10>::value,
	yaal::binary<1000000000>::value,
	yaal::binary<100000000>::value,
	yaal::binary<10000000>::value,
	yaal::binary<1000000>::value,
	yaal::binary<100000>::value,
	yaal::binary<10000>::value,
	yaal::binary<1000>::value,
	yaal::binary<100>::value,
	yaal::binary<10>::value,
	yaal::binary<1>::value
	};

int long unsigned g_pulMaskBitClear[] =
	{
	0xffffffff - yaal::power<2,31>::value,
	0xffffffff - yaal::power<2,30>::value,
	0xffffffff - yaal::power<2,29>::value,
	0xffffffff - yaal::power<2,28>::value,
	0xffffffff - yaal::power<2,27>::value,
	0xffffffff - yaal::power<2,26>::value,
	0xffffffff - yaal::power<2,25>::value,
	0xffffffff - yaal::power<2,24>::value,
	0xffffffff - yaal::power<2,23>::value,
	0xffffffff - yaal::power<2,22>::value,
	0xffffffff - yaal::power<2,21>::value,
	0xffffffff - yaal::power<2,20>::value,
	0xffffffff - yaal::power<2,19>::value,
	0xffffffff - yaal::power<2,18>::value,
	0xffffffff - yaal::power<2,17>::value,
	0xffffffff - yaal::power<2,16>::value,
	0xffffffff - yaal::power<2,15>::value,
	0xffffffff - yaal::power<2,14>::value,
	0xffffffff - yaal::power<2,13>::value,
	0xffffffff - yaal::power<2,12>::value,
	0xffffffff - yaal::power<2,11>::value,
	0xffffffff - yaal::power<2,10>::value,
	0xffffffff - yaal::binary<1000000000>::value,
	0xffffffff - yaal::binary<100000000>::value,
	0xffffffff - yaal::binary<10000000>::value,
	0xffffffff - yaal::binary<1000000>::value,
	0xffffffff - yaal::binary<100000>::value,
	0xffffffff - yaal::binary<10000>::value,
	0xffffffff - yaal::binary<1000>::value,
	0xffffffff - yaal::binary<100>::value,
	0xffffffff - yaal::binary<10>::value,
	0xffffffff - yaal::binary<1>::value
	};

char unsigned g_pucMaskBitSet[] =
	{
	yaal::binary<10000000>::value,
	yaal::binary<1000000>::value,
	yaal::binary<100000>::value,
	yaal::binary<10000>::value,
	yaal::binary<1000>::value,
	yaal::binary<100>::value,
	yaal::binary<10>::value,
	yaal::binary<1>::value
	};
char unsigned g_pucMaskBitClear[] =
	{
	yaal::binary<1111111>::value,
	yaal::binary<10111111>::value,
	yaal::binary<11011111>::value,
	yaal::binary<11101111>::value,
	yaal::binary<11110111>::value,
	yaal::binary<11111011>::value,
	yaal::binary<11111101>::value,
	yaal::binary<11111110>::value
	};

bool HBitmap::get( int long const& a_lNumber ) const
	{
	M_PROLOG
	M_ASSERT( a_lNumber >= 0 );
	M_ASSERT( a_lNumber < f_lSize );
	int short l_hState = 0, l_iOffset = 0;
	int long l_lDword;
	char* l_pcAddress = static_cast<char*>( f_pvBlock );
	l_lDword = a_lNumber >> 3;
	l_iOffset = static_cast<short>( a_lNumber & 7 );
	l_hState = static_cast<short>( *( l_pcAddress + l_lDword ) & g_pucMaskBitSet[ l_iOffset ] );
	if ( l_hState )
		l_hState = 1;
	return ( l_hState ? true : false );
	M_EPILOG
	}
	
void HBitmap::set( int long const& a_lNumber, bool const& a_bState )
	{
	M_PROLOG
	M_ASSERT( a_lNumber >= 0 );
	M_ASSERT( a_lNumber < f_lSize );
	int l_iOffset;
	int long l_lDword;
	char unsigned* l_pcAddress = static_cast<char unsigned*>( f_pvBlock );
	l_lDword = a_lNumber >> 3;
	l_iOffset = a_lNumber & 7;
	/* FIXME g++-4.3 bug
	if ( a_iState )
		*( l_pcAddress + l_lDword ) |= g_pucMaskBitSet[ l_iOffset ];
	else
		*( l_pcAddress + l_lDword ) &= g_pucMaskBitClear[ l_iOffset ];
	*/
	if ( a_bState )
		*( l_pcAddress + l_lDword ) = static_cast<char unsigned>( *( l_pcAddress + l_lDword ) | g_pucMaskBitSet[ l_iOffset ] );
	else
		*( l_pcAddress + l_lDword ) = static_cast<char unsigned>( *( l_pcAddress + l_lDword ) & g_pucMaskBitClear[ l_iOffset ] );
	return;
	M_EPILOG
	}
	
void HBitmap::rotate_right( int long const& a_iStart,
		int long const& a_iLen, int long const& a_iVal )
	{
	M_ASSERT( ( a_iVal > 0 ) && ( a_iVal < a_iLen ) && ( a_iStart >= 0 ) && ( a_iLen > 0 ) );
	int long l_iSize = ( a_iLen + a_iStart ) / 8;
	if ( ( a_iLen + a_iStart ) % 8 )
		l_iSize++;
	HBitmap tmp( *this );
	for ( int long i = 0; i < a_iLen; i++ )
		set( a_iStart + i, tmp.get( a_iStart + ( i + a_iVal ) % a_iLen ) );
	return ;
	}
	
void HBitmap::rotate_left( int long const& a_iStart,
		int long const& a_iLen, int long const& a_iVal )
	{
	M_ASSERT( ( a_iVal > 0 ) && ( a_iVal < a_iLen ) && ( a_iStart >= 0 ) && ( a_iLen > 0 ) );
	int long l_iSize = ( a_iLen + a_iStart ) / 8;
	if ( ( a_iLen + a_iStart ) % 8 )
		l_iSize++;
	HBitmap tmp( *this );
	for ( int long i = 0; i < a_iLen; i++ )
		set( a_iStart + ( i + a_iVal ) % a_iLen, tmp.get( a_iStart + i ) );
	return ;
	}
	
}

}


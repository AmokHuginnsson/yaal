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

int long HBitmap::get_size( void ) const
	{
	return ( f_lSize );
	}

int long HBitmap::size( void ) const
	{
	return ( f_lSize );
	}

void const* HBitmap::raw( void ) const
	{
	return ( f_pvBlock );
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
	: f_lAllocatedBytes( octets_for_bits( a_lSize ) ),
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
		if ( b.f_lAllocatedBytes )
			copy( b.f_pvBlock, b.f_lSize );
		else
			use( b.f_pvBlock, b.f_lSize );
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

void HBitmap::copy( void const* a_pvBlock, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( a_lSize > 0 );
	clear();
	int long unsigned copyBytes = octets_for_bits( a_lSize );
	f_pvBlock = xcalloc<char>( copyBytes );
	M_ENSURE( f_pvBlock );
	f_lSize = a_lSize;
	f_lAllocatedBytes = copyBytes;
	::memcpy( f_pvBlock, a_pvBlock, copyBytes );
	return;
	M_EPILOG
	}

int long HBitmap::octets_for_bits( int long const& bits ) const
	{
	return ( ( ( bits >> 3 ) + ( ( bits & 7 ) ? 1 : 0 ) ) );
	}

bool HBitmap::operator == ( HBitmap const& b ) const
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	return ( ( f_pvBlock == b.f_pvBlock ) || ! ::memcmp( f_pvBlock, b.f_pvBlock, octets_for_bits( f_lSize ) ) );
	M_EPILOG
	}

bool HBitmap::operator != ( HBitmap const& b ) const
	{
	M_PROLOG
	return ( ! operator == ( b ) );
	M_EPILOG
	}

HBitmap& HBitmap::operator |= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	for ( int long i = 0; i < f_lSize; ++ i )
		static_cast<char*>( f_pvBlock )[ i ] = static_cast<char>( static_cast<char*>( f_pvBlock )[ i ] | static_cast<char*>( b.f_pvBlock )[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator &= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	for ( int long i = 0; i < f_lSize; ++ i )
		static_cast<char*>( f_pvBlock )[ i ] = static_cast<char>( static_cast<char*>( f_pvBlock )[ i ] & static_cast<char*>( b.f_pvBlock )[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator ^= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	for ( int long i = 0; i < f_lSize; ++ i )
		static_cast<char*>( f_pvBlock )[ i ] = static_cast<char>( static_cast<char*>( f_pvBlock )[ i ] ^ static_cast<char*>( b.f_pvBlock )[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap HBitmap::operator | ( HBitmap const& b ) const
	{
	M_PROLOG
	HBitmap m( *this );
	m |= b;
	return ( m );
	M_EPILOG
	}

HBitmap HBitmap::operator & ( HBitmap const& b ) const
	{
	M_PROLOG
	HBitmap m( *this );
	m &= b;
	return ( m );
	M_EPILOG
	}

HBitmap HBitmap::operator ^ ( HBitmap const& b ) const
	{
	M_PROLOG
	HBitmap m( *this );
	m ^= b;
	return ( m );
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
	int long l_iOffset;
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

HBitmap::const_iterator HBitmap::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

HBitmap::const_iterator HBitmap::find( int long const& idx ) const
	{
	M_ASSERT( ( idx >= 0 ) && ( idx < f_lSize ) );
	return ( const_iterator( this, idx ) );
	}

HBitmap::const_iterator HBitmap::end( void ) const
	{
	return ( const_iterator( this, f_lSize ) );
	}

HBitmap::iterator HBitmap::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

HBitmap::iterator HBitmap::find( int long const& idx )
	{
	M_ASSERT( ( idx >= 0 ) && ( idx < f_lSize ) );
	return ( iterator( this, idx ) );
	}

HBitmap::iterator HBitmap::end( void )
	{
	return ( iterator( this, f_lSize ) );
	}

template<>
bool HBitmap::HIterator<bool>::operator* ( void )
	{
	M_ASSERT( f_poOwner );
	return ( f_poOwner->get( f_lIndex ) );
	}

template<>
HBitmap::HBit HBitmap::HIterator<HBitmap::HBit>::operator* ( void )
	{
	M_ASSERT( f_poOwner );
	return ( HBitmap::HBit( f_poOwner, f_lIndex ) );
	}

HBitmap::HBit::HBit( HBitmap* a_poOwner, int long const& idx )
	: f_poOwner( a_poOwner ), f_lIndex( idx )
	{
	}

HBitmap::HBit::HBit( HBit const& bit )
	: f_poOwner( bit.f_poOwner ), f_lIndex( bit.f_lIndex )
	{
	}

HBitmap::HBit& HBitmap::HBit::operator = ( HBitmap::HBit const& bit )
	{
	if ( &bit != this )
		{
		f_poOwner = bit.f_poOwner;
		f_lIndex = bit.f_lIndex;
		}
	return ( *this );
	}

bool HBitmap::HBit::operator == ( bool const& bit ) const
	{
	return ( f_poOwner->get( f_lIndex ) == bit );
	}

bool HBitmap::HBit::operator != ( bool const& bit ) const
	{
	return ( f_poOwner->get( f_lIndex ) != bit );
	}

HBitmap::HBit::operator bool ( void ) const
	{
	return ( f_poOwner->get( f_lIndex ) );
	}

HBitmap::HBit& HBitmap::HBit::operator = ( bool const& bit )
	{
	f_poOwner->set( f_lIndex, bit );
	return ( *this );
	}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hbitmap.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hbitmap.hxx"
#include "hcore/hpool.hxx"

typedef yaal::hcore::HPool<char*> mem_pool_t;

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

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

HBitmap::HBitmap( void )
	: f_lAllocatedBytes( 0 ), f_lSize( 0 ), f_pvData( NULL )
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
	return ( block() );
	}

void HBitmap::clear( void )
	{
	if ( f_lAllocatedBytes )
		delete static_cast<mem_pool_t*>( f_pvData );
	f_pvData = NULL;
	f_lAllocatedBytes = 0;
	f_lSize = 0;
	return;
	}

HBitmap::HBitmap( int long const& a_lSize )
	: f_lAllocatedBytes( 0 ), f_lSize( 0 ), f_pvData( NULL )
	{
	M_PROLOG
	M_ASSERT( a_lSize > 0 );
	ensure_pool( a_lSize );
	return;
	M_EPILOG
	}

HBitmap::HBitmap( HBitmap const& b )
	: f_lAllocatedBytes( 0 ), f_lSize( 0 ), f_pvData( NULL )
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
			copy( b.block(), b.f_lSize );
		else
			{
			/*
			 * The source is just reference to some external memory,
			 * so it is meant to be writeable.
			 */
			if ( b.f_lSize )
				use( const_cast<void*>( b.block() ), b.f_lSize );
			else
				clear();
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
	f_pvData = a_pvBlock;
	return;
	M_EPILOG
	}

void HBitmap::copy( void const* a_pvBlock, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( a_lSize > 0 );
	int long copyBytes = octets_for_bits( a_lSize );
	ensure_pool( a_lSize );
	::memcpy( block(), a_pvBlock, copyBytes );
	return;
	M_EPILOG
	}

void* HBitmap::block( void )
	{
	return ( f_lAllocatedBytes ? static_cast<mem_pool_t*>( f_pvData )->raw() : f_pvData );
	}

void const* HBitmap::block( void ) const
	{
	return ( f_lAllocatedBytes ? static_cast<mem_pool_t*>( f_pvData )->raw() : f_pvData );
	}

void HBitmap::ensure_pool( int long const& newSize )
	{
	M_PROLOG
	f_lSize = newSize;
	int long newPoolSize = octets_for_bits( f_lSize );
	if ( f_lAllocatedBytes )
		static_cast<mem_pool_t*>( f_pvData )->pool_realloc( newPoolSize );
	else
		f_pvData = new mem_pool_t( newPoolSize, mem_pool_t::AUTO_GROW );
	f_lAllocatedBytes = newPoolSize;
	M_ENSURE( f_pvData );
	return;
	M_EPILOG
	}

void HBitmap::push_back( bool const& bit )
	{
	M_PROLOG
	M_ASSERT( f_lAllocatedBytes || ! f_pvData );
	ensure_pool( ++ f_lSize );
	set( f_lSize - 1, bit );
	M_EPILOG
	}

int long HBitmap::octets_for_bits( int long const& bits ) const
	{
	return ( ( ( bits >> 3 ) + ( ( bits & 7 ) ? 1 : 0 ) ) );
	}

void HBitmap::fill( bool const& bit )
	{
	M_PROLOG
	fill( 0, f_lSize, bit );
	return;
	M_EPILOG
	}

u8_t g_pucMaskBitKeepLeft[ 8 ] =
	{
	yaal::obinary<010000000>::value,
	yaal::obinary<011000000>::value,
	yaal::obinary<011100000>::value,
	yaal::obinary<011110000>::value,
	yaal::obinary<011111000>::value,
	yaal::obinary<011111100>::value,
	yaal::obinary<011111110>::value,
	yaal::obinary<011111111>::value
	};

u8_t g_pucMaskBitKeepRight[ 8 ] =
	{
	yaal::obinary<011111111>::value,
	yaal::obinary<001111111>::value,
	yaal::obinary<000111111>::value,
	yaal::obinary<000011111>::value,
	yaal::obinary<000001111>::value,
	yaal::obinary<000000111>::value,
	yaal::obinary<000000011>::value,
	yaal::obinary<000000001>::value
	};

void HBitmap::fill( int long const& offset, int long const& amount, bool const& bit )
	{
	M_PROLOG
	int long byteOffset = octets_for_bits( offset );
	int long til = offset + amount;
	int long byteAmount = ( octets_for_bits( offset + amount ) - byteOffset ) - ( ( offset & 7 ) ? 1 : 0 ) - ( ( til & 7 ) ? 1 : 0 );
	u8_t filler = static_cast<u8_t>( bit ? 0xff : 0 );
	char* data = static_cast<char*>( block() );
	if ( byteAmount > 0 )
		::memset( data + byteOffset, filler, byteAmount );
	if ( offset & 7 )
		{
		if ( bit )
			data[ byteOffset - 1 ] = static_cast<u8_t>( data[ byteOffset - 1 ] | g_pucMaskBitKeepRight[ ( offset & 7 ) - 1 ] );
		else
			data[ byteOffset - 1 ] = static_cast<u8_t>( data[ byteOffset - 1 ] & g_pucMaskBitKeepLeft[ ( offset & 7 ) - 1 ] );
		}
	if ( til & 7 )
		{
		if ( bit )
			data[ byteOffset + byteAmount ] = static_cast<u8_t>( data[ byteOffset - 1 ] | g_pucMaskBitKeepLeft[ ( til & 7 ) - 1 ] );
		else
			data[ byteOffset + byteOffset ] = static_cast<u8_t>( data[ byteOffset - 1 ] & g_pucMaskBitKeepRight[ ( til & 7 ) - 1 ] );
		}
	return;
	M_EPILOG
	}

void HBitmap::reserve( int long const& a_lSize )
	{
	M_PROLOG
	ensure_pool( a_lSize );
	return;
	M_EPILOG
	}

bool HBitmap::operator == ( HBitmap const& b ) const
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	return ( ( f_pvData == b.f_pvData ) || ! ::memcmp( block(), b.block(), octets_for_bits( f_lSize ) ) );
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
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < f_lSize; ++ i )
		dst[ i ] = static_cast<char>( dst[ i ] | src[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator &= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < f_lSize; ++ i )
		dst[ i ] = static_cast<char>( dst[ i ] & src[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator ^= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( f_lSize == b.f_lSize );
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < f_lSize; ++ i )
		dst[ i ] = static_cast<char>( dst[ i ] ^ src[ i ] );
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

HBitmap& HBitmap::operator += ( HBitmap const& bmp )
	{
	M_PROLOG
	/* Super slow !!! FIXME */
	for ( HBitmap::const_iterator it = bmp.begin(); it != bmp.end(); ++ it )
		push_back( *it );
	return ( *this );
	M_EPILOG
	}

u32_t g_pulMaskBitSet[ 32 ] = 
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
	yaal::obinary<010000000000>::value,
	yaal::obinary<01000000000>::value,
	yaal::obinary<0100000000>::value,
	yaal::obinary<010000000>::value,
	yaal::obinary<01000000>::value,
	yaal::obinary<0100000>::value,
	yaal::obinary<010000>::value,
	yaal::obinary<01000>::value,
	yaal::obinary<0100>::value,
	yaal::obinary<010>::value,
	yaal::obinary<01>::value
	};

u32_t g_pulMaskBitClear[ 32 ] =
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
	0xffffffff - yaal::obinary<010000000000>::value,
	0xffffffff - yaal::obinary<01000000000>::value,
	0xffffffff - yaal::obinary<0100000000>::value,
	0xffffffff - yaal::obinary<010000000>::value,
	0xffffffff - yaal::obinary<01000000>::value,
	0xffffffff - yaal::obinary<0100000>::value,
	0xffffffff - yaal::obinary<010000>::value,
	0xffffffff - yaal::obinary<01000>::value,
	0xffffffff - yaal::obinary<0100>::value,
	0xffffffff - yaal::obinary<010>::value,
	0xffffffff - yaal::obinary<01>::value
	};

u8_t g_pucMaskBitSet[ 8 ] =
	{
	yaal::obinary<010000000>::value,
	yaal::obinary<01000000>::value,
	yaal::obinary<0100000>::value,
	yaal::obinary<010000>::value,
	yaal::obinary<01000>::value,
	yaal::obinary<0100>::value,
	yaal::obinary<010>::value,
	yaal::obinary<01>::value
	};

u8_t g_pucMaskBitClear[ 8 ] =
	{
	yaal::obinary<01111111>::value,
	yaal::obinary<010111111>::value,
	yaal::obinary<011011111>::value,
	yaal::obinary<011101111>::value,
	yaal::obinary<011110111>::value,
	yaal::obinary<011111011>::value,
	yaal::obinary<011111101>::value,
	yaal::obinary<011111110>::value
	};

bool HBitmap::get( int long const& a_lNumber ) const
	{
	M_PROLOG
	M_ASSERT( a_lNumber >= 0 );
	M_ASSERT( a_lNumber < f_lSize );
	int short l_hState = 0, l_iOffset = 0;
	int long l_lDword;
	char const* l_pcAddress = static_cast<char const*>( block() );
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
	char unsigned* l_pcAddress = static_cast<char unsigned*>( block() );
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

HBitmap::const_iterator HBitmap::rbegin( void ) const
	{
	return ( const_iterator( this, f_lSize - 1 ) );
	}

HBitmap::const_iterator HBitmap::rend( void ) const
	{
	return ( const_iterator( this, -1 ) );
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

HBitmap::iterator HBitmap::rbegin( void )
	{
	return ( iterator( this, f_lSize - 1 ) );
	}

HBitmap::iterator HBitmap::rend( void )
	{
	return ( iterator( this, -1 ) );
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


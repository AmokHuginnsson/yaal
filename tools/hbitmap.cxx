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

#include <cstring>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hbitmap.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/pod.hxx"

using namespace yaal::hcore;
using namespace yaal::meta;

namespace yaal
{

namespace tools
{

template<>
bool HBitmap::HIterator<bool>::operator* ( void )
	{
	M_ASSERT( _owner );
	return ( _owner->get( _index ) );
	}

template<>
HBitmap::HBit HBitmap::HIterator<HBitmap::HBit>::operator* ( void )
	{
	M_ASSERT( _owner );
	return ( HBitmap::HBit( _owner, _index ) );
	}

HBitmap::HBitmap( void )
	: _allocatedBytes( 0 ), _size( 0 ), _data( NULL )
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
	return ( _size );
	}

int long HBitmap::size( void ) const
	{
	return ( _size );
	}

void const* HBitmap::raw( void ) const
	{
	return ( block() );
	}

void HBitmap::clear( void )
	{
	if ( _allocatedBytes )
		delete static_cast<HChunk*>( _data );
	_data = NULL;
	_allocatedBytes = 0;
	_size = 0;
	return;
	}

HBitmap::HBitmap( int long size_ )
	: _allocatedBytes( 0 ), _size( 0 ), _data( NULL )
	{
	M_PROLOG
	M_ASSERT( size_ > 0 );
	ensure_pool( size_ );
	return;
	M_EPILOG
	}

HBitmap::HBitmap( HBitmap const& b )
	: _allocatedBytes( 0 ), _size( 0 ), _data( NULL )
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
		if ( b._allocatedBytes )
			copy( b.block(), b._size );
		else
			{
			/*
			 * The source is just reference to some external memory,
			 * so it is meant to be writeable.
			 */
			if ( b._size )
				use( const_cast<void*>( b.block() ), b._size );
			else
				clear();
			}
		}
	return ( *this );
	M_EPILOG
	}

void HBitmap::use( void* block_, int long size_ )
	{
	M_PROLOG
	M_ASSERT( size_ > 0 );
	clear();
	_size = size_;
	_data = block_;
	return;
	M_EPILOG
	}

void HBitmap::copy( void const* block_, int long size_ )
	{
	M_PROLOG
	M_ASSERT( size_ > 0 );
	int long copyBytes = octets_for_bits( size_ );
	ensure_pool( size_ );
	::memcpy( block(), block_, copyBytes );
	return;
	M_EPILOG
	}

void* HBitmap::block( void )
	{
	return ( _allocatedBytes ? static_cast<HChunk*>( _data )->raw() : _data );
	}

void const* HBitmap::block( void ) const
	{
	return ( _allocatedBytes ? static_cast<HChunk*>( _data )->raw() : _data );
	}

void HBitmap::ensure_pool( int long newSize )
	{
	M_PROLOG
	_size = newSize;
	int long newPoolSize = octets_for_bits( _size );
	if ( _allocatedBytes )
		static_cast<HChunk*>( _data )->realloc( newPoolSize );
	else
		_data = new HChunk( newPoolSize, HChunk::STRATEGY::GEOMETRIC );
	_allocatedBytes = newPoolSize;
	M_ENSURE( _data );
	return;
	M_EPILOG
	}

void HBitmap::push_back( bool bit )
	{
	M_PROLOG
	M_ASSERT( _allocatedBytes || ! _data );
	ensure_pool( ++ _size );
	set( _size - 1, bit );
	M_EPILOG
	}

int long HBitmap::octets_for_bits( int long bits ) const
	{
	return ( ( ( bits >> 3 ) + ( ( bits & 7 ) ? 1 : 0 ) ) );
	}

void HBitmap::fill( bool bit )
	{
	M_PROLOG
	fill( 0, _size, bit );
	return;
	M_EPILOG
	}

u8_t _maskBitKeepLeft_[ 8 ] =
	{
	obinary<010000000>::value,
	obinary<011000000>::value,
	obinary<011100000>::value,
	obinary<011110000>::value,
	obinary<011111000>::value,
	obinary<011111100>::value,
	obinary<011111110>::value,
	obinary<011111111>::value
	};

u8_t _maskBitKeepRight_[ 8 ] =
	{
	obinary<011111111>::value,
	obinary<001111111>::value,
	obinary<000111111>::value,
	obinary<000011111>::value,
	obinary<000001111>::value,
	obinary<000000111>::value,
	obinary<000000011>::value,
	obinary<000000001>::value
	};

void HBitmap::fill( int long offset, int long amount, bool bit )
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
			data[ byteOffset - 1 ] = static_cast<u8_t>( data[ byteOffset - 1 ] | _maskBitKeepRight_[ ( offset & 7 ) - 1 ] );
		else
			data[ byteOffset - 1 ] = static_cast<u8_t>( data[ byteOffset - 1 ] & _maskBitKeepLeft_[ ( offset & 7 ) - 1 ] );
		}
	if ( til & 7 )
		{
		if ( bit )
			data[ byteOffset + byteAmount ] = static_cast<u8_t>( data[ byteOffset - 1 ] | _maskBitKeepLeft_[ ( til & 7 ) - 1 ] );
		else
			data[ byteOffset + byteOffset ] = static_cast<u8_t>( data[ byteOffset - 1 ] & _maskBitKeepRight_[ ( til & 7 ) - 1 ] );
		}
	return;
	M_EPILOG
	}

void HBitmap::reserve( int long size_ )
	{
	M_PROLOG
	ensure_pool( size_ );
	return;
	M_EPILOG
	}

bool HBitmap::operator == ( HBitmap const& b ) const
	{
	M_PROLOG
	M_ASSERT( _size == b._size );
	return ( ( _data == b._data ) || ! ::memcmp( block(), b.block(), octets_for_bits( _size ) ) );
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
	M_ASSERT( _size == b._size );
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < _size; ++ i )
		dst[ i ] = static_cast<char>( dst[ i ] | src[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator &= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( _size == b._size );
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < _size; ++ i )
		dst[ i ] = static_cast<char>( dst[ i ] & src[ i ] );
	return ( *this );
	M_EPILOG
	}

HBitmap& HBitmap::operator ^= ( HBitmap const& b )
	{
	M_PROLOG
	M_ASSERT( _size == b._size );
	char* dst = static_cast<char*>( block() );
	char const* src = static_cast<char const*>( b.block() );
	for ( int long i = 0; i < _size; ++ i )
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
	for ( HBitmap::const_iterator it( bmp.begin() ), endIt( bmp.end() ); it != endIt; ++ it )
		push_back( *it );
	return ( *this );
	M_EPILOG
	}

u32_t _maskBitSet_[ 32 ] = 
	{
	power<2,31>::value,
	power<2,30>::value,
	power<2,29>::value,
	power<2,28>::value,
	power<2,27>::value,
	power<2,26>::value,
	power<2,25>::value,
	power<2,24>::value,
	power<2,23>::value,
	power<2,22>::value,
	power<2,21>::value,
	power<2,20>::value,
	power<2,19>::value,
	power<2,18>::value,
	power<2,17>::value,
	power<2,16>::value,
	power<2,15>::value,
	power<2,14>::value,
	power<2,13>::value,
	power<2,12>::value,
	power<2,11>::value,
	obinary<010000000000>::value,
	obinary<01000000000>::value,
	obinary<0100000000>::value,
	obinary<010000000>::value,
	obinary<01000000>::value,
	obinary<0100000>::value,
	obinary<010000>::value,
	obinary<01000>::value,
	obinary<0100>::value,
	obinary<010>::value,
	obinary<01>::value
	};

u32_t _maskBitClear_[ 32 ] =
	{
	0xffffffff ^ power<2,31>::value,
	0xffffffff ^ power<2,30>::value,
	0xffffffff ^ power<2,29>::value,
	0xffffffff ^ power<2,28>::value,
	0xffffffff ^ power<2,27>::value,
	0xffffffff ^ power<2,26>::value,
	0xffffffff ^ power<2,25>::value,
	0xffffffff ^ power<2,24>::value,
	0xffffffff ^ power<2,23>::value,
	0xffffffff ^ power<2,22>::value,
	0xffffffff ^ power<2,21>::value,
	0xffffffff ^ power<2,20>::value,
	0xffffffff ^ power<2,19>::value,
	0xffffffff ^ power<2,18>::value,
	0xffffffff ^ power<2,17>::value,
	0xffffffff ^ power<2,16>::value,
	0xffffffff ^ power<2,15>::value,
	0xffffffff ^ power<2,14>::value,
	0xffffffff ^ power<2,13>::value,
	0xffffffff ^ power<2,12>::value,
	0xffffffff ^ power<2,11>::value,
	0xffffffff ^ obinary<010000000000>::value,
	0xffffffff ^ obinary<01000000000>::value,
	0xffffffff ^ obinary<0100000000>::value,
	0xffffffff ^ obinary<010000000>::value,
	0xffffffff ^ obinary<01000000>::value,
	0xffffffff ^ obinary<0100000>::value,
	0xffffffff ^ obinary<010000>::value,
	0xffffffff ^ obinary<01000>::value,
	0xffffffff ^ obinary<0100>::value,
	0xffffffff ^ obinary<010>::value,
	0xffffffff ^ obinary<01>::value
	};

u8_t _maskBitSetByte_[ 8 ] =
	{
	obinary<010000000>::value,
	obinary<01000000>::value,
	obinary<0100000>::value,
	obinary<010000>::value,
	obinary<01000>::value,
	obinary<0100>::value,
	obinary<010>::value,
	obinary<01>::value
	};

u8_t _maskBitClearByte_[ 8 ] =
	{
	obinary<01111111>::value,
	obinary<010111111>::value,
	obinary<011011111>::value,
	obinary<011101111>::value,
	obinary<011110111>::value,
	obinary<011111011>::value,
	obinary<011111101>::value,
	obinary<011111110>::value
	};


bool HBitmap::get( int long number_ ) const
	{
	M_PROLOG
	M_ASSERT( number_ >= 0 );
	M_ASSERT( number_ < _size );
	int long byteNo( number_ >> 3 );
	char const* address = static_cast<char const*>( block() );
	int short offset( static_cast<short>( number_ & 7 ) );
	int short state(static_cast<short>( *( address + byteNo ) & _maskBitSetByte_[ offset ] ) );
	if ( state )
		state = 1;
	return ( state ? true : false );
	M_EPILOG
	}
	
void HBitmap::set( int long number_, bool state_ )
	{
	M_PROLOG
	M_ASSERT( number_ >= 0 );
	M_ASSERT( number_ < _size );
	int long offset( number_ & 7 );
	int long byteNo( number_ >> 3 );
	char unsigned* address = static_cast<char unsigned*>( block() );
	/* FIXME g++-4.3 bug
	if ( state_ )
		*( address + byteNo ) |= _maskBitSet_[ offset ];
	else
		*( address + byteNo ) &= _maskBitClear_[ offset ];
	*/
	if ( state_ )
		*( address + byteNo ) = static_cast<char unsigned>( *( address + byteNo ) | _maskBitSetByte_[ offset ] );
	else
		*( address + byteNo ) = static_cast<char unsigned>( *( address + byteNo ) & _maskBitClearByte_[ offset ] );
	return;
	M_EPILOG
	}
	
void HBitmap::rotate_right( int long start_,
		int long len_, int long val_ )
	{
	M_ASSERT( ( val_ > 0 ) && ( val_ < len_ ) && ( start_ >= 0 ) && ( len_ > 0 ) );
	int long byteCount = ( len_ + start_ ) / 8;
	if ( ( len_ + start_ ) % 8 )
		byteCount++;
	HBitmap tmp( *this );
	for ( int long i = 0; i < len_; i++ )
		set( start_ + i, tmp.get( start_ + ( i + val_ ) % len_ ) );
	return ;
	}
	
void HBitmap::rotate_left( int long start_,
		int long len_, int long val_ )
	{
	M_ASSERT( ( val_ > 0 ) && ( val_ < len_ ) && ( start_ >= 0 ) && ( len_ > 0 ) );
	int long byteCount = ( len_ + start_ ) / 8;
	if ( ( len_ + start_ ) % 8 )
		byteCount++;
	HBitmap tmp( *this );
	for ( int long i = 0; i < len_; i++ )
		set( start_ + ( i + val_ ) % len_, tmp.get( start_ + i ) );
	return ;
	}

HBitmap::const_iterator HBitmap::begin( void ) const
	{
	return ( const_iterator( this, 0 ) );
	}

HBitmap::const_iterator HBitmap::find( int long idx ) const
	{
	M_ASSERT( ( idx >= 0 ) && ( idx < _size ) );
	return ( const_iterator( this, idx ) );
	}

HBitmap::const_iterator HBitmap::end( void ) const
	{
	return ( const_iterator( this, _size ) );
	}

HBitmap::const_iterator HBitmap::rbegin( void ) const
	{
	return ( const_iterator( this, _size - 1 ) );
	}

HBitmap::const_iterator HBitmap::rend( void ) const
	{
	return ( const_iterator( this, -1 ) );
	}

HBitmap::iterator HBitmap::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

HBitmap::iterator HBitmap::find( int long idx )
	{
	M_ASSERT( ( idx >= 0 ) && ( idx < _size ) );
	return ( iterator( this, idx ) );
	}

HBitmap::iterator HBitmap::end( void )
	{
	return ( iterator( this, _size ) );
	}

HBitmap::iterator HBitmap::rbegin( void )
	{
	return ( iterator( this, _size - 1 ) );
	}

HBitmap::iterator HBitmap::rend( void )
	{
	return ( iterator( this, -1 ) );
	}

HBitmap::HBit::HBit( HBitmap* owner_, int long idx )
	: _owner( owner_ ), _index( idx )
	{
	}

HBitmap::HBit::HBit( HBit const& bit )
	: _owner( bit._owner ), _index( bit._index )
	{
	}

HBitmap::HBit& HBitmap::HBit::operator = ( HBitmap::HBit const& bit )
	{
	if ( &bit != this )
		{
		_owner = bit._owner;
		_index = bit._index;
		}
	return ( *this );
	}

bool HBitmap::HBit::operator == ( bool bit ) const
	{
	return ( _owner->get( _index ) == bit );
	}

bool HBitmap::HBit::operator != ( bool bit ) const
	{
	return ( _owner->get( _index ) != bit );
	}

HBitmap::HBit::operator bool ( void ) const
	{
	return ( _owner->get( _index ) );
	}

HBitmap::HBit& HBitmap::HBit::operator = ( bool bit )
	{
	_owner->set( _index, bit );
	return ( *this );
	}

}

}


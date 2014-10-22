/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hchunk.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hchunk.hxx"
#include "algorithm.hxx"
#include "memory.hxx"

namespace yaal {

namespace hcore {

HChunk::HChunk( void )
	: _size( 0 ), _data( NULL ) {
	return;
}

HChunk::HChunk( int long size_, STRATEGY::enum_t strategy_ )
	: _size( 0 ), _data( NULL ) {
	M_PROLOG
	realloc( size_, strategy_ );
	return;
	M_EPILOG
}

HChunk::HChunk( HChunk&& chunk_ )
	: _size( 0 ), _data( NULL ) {
	M_PROLOG
	swap( chunk_ );
	return;
	M_EPILOG
}

HChunk& HChunk::operator = ( HChunk&& chunk_ ) {
	M_PROLOG
	if ( &chunk_ != this ) {
		swap( chunk_ );
		chunk_.free();
	}
	return ( *this );
	M_EPILOG
}

HChunk::~HChunk( void ) {
	M_PROLOG
	free();
	return;
	M_DESTRUCTOR_EPILOG
}

void HChunk::free( void ) {
	if ( _data )
		memory::free( _data );
	_size = 0;
	return;
}

void* HChunk::realloc( int long size_, STRATEGY::enum_t strategy_ ) {
	if ( size_ < 1 )
		M_THROW( "bad size", size_ );
	if ( size_ > _size ) {
		int long newSize( 0 );
		if ( strategy_ == STRATEGY::GEOMETRIC ) {
			newSize = 1;
			while ( newSize < size_ )
				newSize <<= 1;
		} else {
			M_ASSERT( strategy_ == STRATEGY::EXACT );
			newSize = size_;
		}
		_data = memory::realloc<char>( _data, newSize );
		::memset( static_cast<char*>( _data ) + _size, 0, static_cast<size_t>( newSize - _size ) );
		_size = newSize;
	}
	return ( _data );
}

void HChunk::swap( HChunk& chunk_ ) {
	if ( &chunk_ != this ) {
		using yaal::swap;
		swap( _data, chunk_._data );
		swap( _size, chunk_._size );
	}
	return;
}

}

}


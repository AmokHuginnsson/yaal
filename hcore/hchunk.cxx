/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hchunk.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
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
		if ( strategy_ == STRATEGY::GEOMETRIC ) {
			int long newSize = 1;
			while ( newSize < size_ )
				newSize <<= 1;
			size_ = newSize;
		} else {
			M_ASSERT( strategy_ == STRATEGY::EXACT );
		}
		_data = memory::realloc<char>( _data, size_ );
		::memset( static_cast<char*>( _data ) + _size, 0, size_ - _size );
		_size = size_;
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


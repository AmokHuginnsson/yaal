/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	: _size( 0 )
	, _data( nullptr ) {
	return;
}

HChunk::HChunk( int long size_, STRATEGY strategy_ )
	: _size( 0 )
	, _data( nullptr ) {
	M_PROLOG
	realloc( size_, strategy_ );
	return;
	M_EPILOG
}

HChunk::HChunk( HChunk&& chunk_ )
	: _size( 0 )
	, _data( nullptr ) {
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
	if ( _data ) {
		memory::free( _data );
	}
	_size = 0;
	return;
}

void* HChunk::realloc( int long size_, STRATEGY strategy_ ) {
	if ( size_ < 1 ) {
		M_THROW( "bad size", size_ );
	}
	if ( size_ > _size ) {
		int long newSize( 0 );
		if ( strategy_ == STRATEGY::GEOMETRIC ) {
			newSize = 1;
			while ( ( newSize < size_ ) && newSize ) {
				newSize <<= 1;
			}
			if ( ! newSize ) {
				M_THROW( "requested allocation size is too big", size_ );
			}
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


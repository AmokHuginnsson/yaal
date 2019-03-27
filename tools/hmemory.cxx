/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmemory.hxx"
#include "hcore/algorithm.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {

static int const UNINITIALIZED = -1;

}

int long HMemoryObserver::do_commit( int long valid_, int long cursorRead_, int long cursorWrite_, int long size_ ) {
	M_ENSURE_T( ( size_ >= 0 ) && ( size_ <= ( _size - valid_ ) ), HMemory );
	M_ASSERT( cursorWrite_ <= ( ( cursorRead_ + valid_ ) % _size ) );
	bool validWrapped( cursorRead_ > ( ( cursorRead_ + valid_ ) % _size ) );
	int long flatCursorWrite( cursorWrite_ );
	if ( validWrapped ) {
		if ( cursorWrite_ < cursorRead_ ) {
			flatCursorWrite += _size;
		}
	} else {
		M_ASSERT( cursorWrite_ >= cursorRead_ );
	}
	return ( max( valid_, flatCursorWrite + size_ - cursorRead_ ) );
}

int long HMemoryProvider::do_commit( int long valid_, int long cursorRead_, int long cursorWrite_, int long size_ ) {
	M_ASSERT( size_ >= 0 );
	M_ASSERT( ( cursorWrite_ >= cursorRead_ ) && ( cursorWrite_ <= ( cursorRead_ + valid_ ) ) );
	_chunk.realloc( _size = cursorWrite_ + size_ );
	return ( max( valid_, cursorWrite_ + size_ - cursorRead_ ) );
}

HMemory::HMemory( HMemoryHandlingStrategyInterface& memory_, INITIAL_STATE initialState_ )
	: _memory( memory_ )
	, _valid( initialState_ == INITIAL_STATE::AUTO ? UNINITIALIZED : ( initialState_ == INITIAL_STATE::VALID ? memory_.get_size() : 0 ) )
	, _cursorRead( 0 )
	, _cursorWrite( 0 ) {
	return;
}

HMemory::~HMemory( void ) {
	return;
}

bool HMemory::operator == ( HMemory const& other ) const {
	M_PROLOG
	return (
		( other._memory.get_size() == _memory.get_size() )
		&& ( ! ::memcmp( other._memory.get_memory(), _memory.get_memory(), static_cast<size_t>( _memory.get_size() ) ) )
	);
	M_EPILOG
}

int long HMemory::do_write( void const* src_, int long size_ ) {
	M_PROLOG
	if ( _valid == UNINITIALIZED ) { /* First data access. */
		_valid = 0;
	}
	_valid = _memory.commit( _valid, _cursorRead, _cursorWrite, size_ );
	char const* src( static_cast<char const*>( src_ ) );
	int long size( size_ );
	if ( ( _cursorWrite + size ) > _memory.get_size() ) {
		int long partSize( _memory.get_size() - _cursorWrite );
		::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src, static_cast<size_t>( partSize ) );
		_cursorWrite = 0;
		size -= partSize;
		src += partSize;
	}
	::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src, static_cast<size_t>( size ) );
	_cursorWrite += size;
	return ( size );
	M_EPILOG
}

/*
 * Do not change this!
 * Flush on HMemory should be a no-op.
 */
void HMemory::do_flush( void ) {
	return;
}

int long HMemory::do_read( void* dest_, int long size_ ) {
	M_PROLOG
	if ( _valid == UNINITIALIZED ) { /* First data access. */
		_valid = _memory.get_size();
	}
	int long size( min( size_, _valid ) );
	if ( ( _cursorRead + size ) > _memory.get_size() ) {
		int long part1( _memory.get_size() - _cursorRead );
		int long part2( size - part1 );
		::memcpy( dest_, static_cast<char*>( _memory.get_memory() ) + _cursorRead, static_cast<size_t>( part1 ) );
		::memcpy( static_cast<char*>( dest_ ) + part1, static_cast<char*>( _memory.get_memory() ), static_cast<size_t>( part2 ) );
		_cursorRead = part2;
	} else {
		::memcpy( dest_, static_cast<char const*>( _memory.get_memory() ) + _cursorRead, static_cast<size_t>( size ) );
		_cursorRead += size;
	}
	_valid -= size;
	if ( ( _valid == 0 ) && ( size == 0 ) && ( size_ > 0 ) ) {
		_valid = UNINITIALIZED;
		size = -1;
	}
	return ( size );
	M_EPILOG
}

void HMemory::do_seek( int long offset_, SEEK seek_ ) {
	M_PROLOG
	M_ENSURE( _valid != UNINITIALIZED );
	int long currentPosition( max( _cursorRead, _cursorWrite ) );
	int long newPosition( UNINITIALIZED );
	switch ( seek_ ) {
		case ( SEEK::BEGIN ): {
			newPosition = offset_;
		} break;
		case ( SEEK::CURRENT ): {
			newPosition = currentPosition + offset_;
		} break;
		case ( SEEK::END ): {
			newPosition = _cursorRead + _valid + offset_;
		} break;
	}
	int long newValid( _cursorRead + _valid - newPosition );
	M_ENSURE( ( newPosition >= 0 ) && ( newValid >= 0 ) );
	M_ASSERT( newValid <= _memory.get_size() );
	newPosition %= _memory.get_size();
	_valid = newValid;
	_cursorWrite = _cursorRead = newPosition;
	return;
	M_EPILOG
}

bool HMemory::do_is_valid( void ) const {
	M_PROLOG
	return ( _valid != UNINITIALIZED );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HMemory::do_poll_type( void ) const {
	M_PROLOG
	return ( is_valid() ? POLL_TYPE::EMULATED : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HMemory::do_data( void ) const {
	M_PROLOG
	return ( is_valid() ? this : memory::INVALID_HANDLE );
	M_EPILOG
}

}

}


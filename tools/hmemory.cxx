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

HMemory::HMemory( HMemoryHandlingStrategyInterface& memory_, INITIAL_STATE initialState_ )
	: _memory( memory_ )
	, _valid( initialState_ == INITIAL_STATE::AUTO ? -1 : ( initialState_ == INITIAL_STATE::VALID ? memory_.get_size() : 0 ) )
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
	if ( _valid == -1 ) { /* First data access. */
		_valid = 0;
	}
	_memory.commit( _valid, _cursorRead, _cursorWrite, size_ );
	int long maxWrite( _memory.get_size() - _valid );
	int long size( min( size_, maxWrite ) );
	if ( ( _cursorWrite + size ) > _memory.get_size() ) {
		int long part1( _memory.get_size() - _cursorWrite );
		int long part2( size - part1 );
		::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src_, static_cast<size_t>( part1 ) );
		::memcpy( static_cast<char*>( _memory.get_memory() ), static_cast<char const*>( src_ ) + part1, static_cast<size_t>( part2 ) );
		_cursorWrite = part2;
	} else {
		::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src_, static_cast<size_t>( size ) );
		_cursorWrite += size;
	}
	_valid += size;
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
	if ( _valid == -1 ) { /* First data access. */
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
	return ( size );
	M_EPILOG
}

bool HMemory::do_is_valid( void ) const {
	M_PROLOG
	return ( ( _cursorRead < _memory.get_size() ) && ( _cursorWrite < _memory.get_size() ) );
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


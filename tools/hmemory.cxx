/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmemory.cxx - this file is integral part of `yaal' project.

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
#include "hmemory.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace tools {

HMemory::HMemory( HMemoryHandlingStrategyInterface& memory_, INITIAL_STATE::enum_t initialState_ )
	: _memory( memory_ ),
	_valid( initialState_ == INITIAL_STATE::AUTO ? -1 : ( initialState_ == INITIAL_STATE::VALID ? memory_.get_size() : 0 ) ),
	_cursorRead( 0 ), _cursorWrite( 0 ) {
	return;
}

HMemory::~HMemory( void ) {
	return;
}

bool HMemory::operator == ( HMemory const& other ) const {
	M_PROLOG
	return ( ( other._memory.get_size() == _memory.get_size() ) && ( ! memcmp( other._memory.get_memory(), _memory.get_memory(), _memory.get_size() ) ) );
	M_EPILOG
}

int long HMemory::do_write( void const* const src_, int long size_ ) {
	M_PROLOG
	if ( _valid == -1 ) /* First data access. */
		_valid = 0;
	_memory.commit( _cursorWrite + size_ );
	int long maxWrite( _memory.get_size() - _valid );
	int long size( min( size_, maxWrite ) );
	if ( ( _cursorWrite + size ) > _memory.get_size() ) {
		int long part1( _memory.get_size() - _cursorWrite );
		int long part2( size - part1 );
		::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src_, part1 );
		::memcpy( static_cast<char*>( _memory.get_memory() ), static_cast<char const* const>( src_ ) + part1, part2 );
		_cursorWrite = part2;
	} else {
		::memcpy( static_cast<char*>( _memory.get_memory() ) + _cursorWrite, src_, size );
		_cursorWrite += size;
	}
	_valid += size;
	return ( size );
	M_EPILOG
}

void HMemory::do_flush( void ) {
	return;
}

int long HMemory::do_read( void* const dest_, int long size_ ) {
	M_PROLOG
	if ( _valid == -1 ) /* First data access. */
		_valid = _memory.get_size();
	int long size( min( size_, _valid ) );
	if ( ( _cursorRead + size ) > _memory.get_size() ) {
		int long part1( _memory.get_size() - _cursorRead );
		int long part2( size - part1 );
		::memcpy( dest_, static_cast<char*>( _memory.get_memory() ) + _cursorRead, part1 );
		::memcpy( static_cast<char*>( dest_ ) + part1, static_cast<char*>( _memory.get_memory() ), part2 );
		_cursorWrite = part2;
	} else {
		::memcpy( dest_, static_cast<char const* const>( _memory.get_memory() ) + _cursorRead, size );
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

}

}


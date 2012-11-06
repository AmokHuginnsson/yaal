/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstringstream.cxx - this file is integral part of `yaal' project.

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
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/algorithm.hxx"
#include "hstringstream.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HStringStream::HStringStream( void )
	: _used( false ), _offset( 0 ), _buffer( "" ) {
}

HStringStream::HStringStream( HString const& init_ )
	: _used( false ), _offset( 0 ), _buffer( init_ ) {
}

HStringStream& HStringStream::operator = ( HString const& s ) {
	_buffer = s;
	_offset = 0;
	_used = false;
	return ( *this );
}

char const* HStringStream::raw( void ) const {
	return ( _buffer.raw() );
}

yaal::hcore::HString const& HStringStream::string( void ) const {
	return ( _buffer );
}

int long HStringStream::do_write( void const* const buffer_, int long size_ ) {
	M_PROLOG
	if ( _used ) {
		_buffer.clear();
		_used = false;
	}
	_buffer.append( static_cast<char const* const>( buffer_ ), size_ );
	return ( size_ );
	M_EPILOG
}

void HStringStream::do_flush( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int long HStringStream::do_read( void* const buffer_, int long size_ ) {
	M_PROLOG
	int long length( _buffer.get_length() );
	int long toCopy( yaal::min( length - _offset, size_ ) );
	if ( length > 0 )
		::strncpy( static_cast<char* const>( buffer_ ), _buffer.raw() + _offset, toCopy );
	_offset += toCopy;
	if ( _offset >= length ) {
		_offset = 0;
		_buffer.clear();
		_used = true;
	}
	return ( toCopy );
	M_EPILOG
}

void HStringStream::use( void ) const {
	_used = true;
	return;
}

void HStringStream::clear( void ) {
	M_PROLOG
	_buffer.clear();
	return;
	M_EPILOG
}

char const* HStringStream::consume( void ) const {
	M_PROLOG
	use();
	return ( _buffer.raw() );
	M_EPILOG
}

char const* operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& stream ) {
	M_PROLOG
	stream.use();
	return ( stream.raw() );
	M_EPILOG
}

bool HStringStream::is_empty( void ) const {
	M_PROLOG
	return ( _used || _buffer.is_empty() );
	M_EPILOG
}

bool HStringStream::do_is_valid( void ) const {
	M_PROLOG
	return ( true );
	M_EPILOG
}

}

}


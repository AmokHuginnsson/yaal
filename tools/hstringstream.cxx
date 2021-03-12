/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/algorithm.hxx"
#include "hstringstream.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HStringStream::HStringStream( void )
	: _used( false )
	, _offset( 0 )
	, _buffer()
	, _utf8() {
}

HStringStream::HStringStream( HString const& init_ )
	: _used( false )
	, _offset( 0 )
	, _buffer( init_ )
	, _utf8() {
}

void HStringStream::set_buffer( HString const& s_ ) {
	M_PROLOG
	clear();
	_buffer.assign( s_ );
	_utf8.reset();
	_offset = 0;
	_used = false;
	return;
	M_EPILOG
}

void HStringStream::str( HString const& s_ ) {
	M_PROLOG
	set_buffer( s_ );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HStringStream::string( void ) const {
	const_cast<HStringStream*>( this )->flush();
	return ( _buffer );
}

yaal::hcore::HString const& HStringStream::str( void ) const {
	return ( string() );
}

int long HStringStream::do_write( void const* buffer_, int long size_ ) {
	M_PROLOG
	if ( _used ) {
		_buffer.clear();
		_utf8.reset();
		_offset = 0;
		_used = false;
	}
	_buffer.append( static_cast<char const*>( buffer_ ), size_ );
	return size_;
	M_EPILOG
}

void HStringStream::do_flush( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int long HStringStream::do_read( void* buffer_, int long size_ ) {
	M_PROLOG
	if ( _buffer.get_length() != _utf8.character_count() ) {
		_utf8 = _buffer;
	}
	int long toCopy( yaal::min( _utf8.byte_count() - _offset, size_ ) );
	if ( toCopy > 0 ) {
		::strncpy( static_cast<char*>( buffer_ ), _utf8.c_str() + _offset, static_cast<size_t>( toCopy ) );
		_offset += toCopy;
	}
	if ( _offset >= _utf8.byte_count() ) {
		_offset = 0;
		_buffer.clear();
		_utf8.reset();
		_used = true;
	}
	return toCopy;
	M_EPILOG
}

void HStringStream::use( void ) const {
	_used = true;
	return;
}

void HStringStream::do_reset( void ) {
	M_PROLOG
	HStreamInterface::do_reset();
	_buffer.clear();
	_utf8.reset();
	_offset = 0;
	_used = false;
	return;
	M_EPILOG
}

HString const& HStringStream::consume( void ) const {
	M_PROLOG
	const_cast<HStringStream*>( this )->flush();
	use();
	return ( _buffer );
	M_EPILOG
}

HString const& operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& stream ) {
	M_PROLOG
	return ( stream.consume() );
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

HStreamInterface::POLL_TYPE HStringStream::do_poll_type( void ) const {
	M_PROLOG
	return ( is_valid() ? POLL_TYPE::EMULATED : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HStringStream::do_data( void ) const {
	M_PROLOG
	return ( is_valid() ? this : memory::INVALID_HANDLE );
	M_EPILOG
}

}

}


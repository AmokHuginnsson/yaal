/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hsynchronizedstream.hxx"

namespace yaal {

namespace hcore {

HSynchronizedStream::HSynchronizedStream( void )
	: _mutex( HMutex::TYPE::RECURSIVE )
	, _streamOwned()
	, _streamRef( nullptr ) {
	return;
}

HSynchronizedStream::HSynchronizedStream( owned_stream_t stream_ )
	: _mutex( HMutex::TYPE::RECURSIVE )
	, _streamOwned( stream_ )
	, _streamRef( _streamOwned.raw() ) {
	M_PROLOG
	return;
	M_EPILOG
}

HSynchronizedStream::HSynchronizedStream( ref_stream_t stream_ )
	: _mutex( HMutex::TYPE::RECURSIVE )
	, _streamOwned()
	, _streamRef( &stream_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HSynchronizedStream::reset( owned_stream_t stream_ ) {
	M_PROLOG
	_streamOwned = stream_;
	_streamRef = _streamOwned.raw();
	return;
	M_EPILOG
}

void HSynchronizedStream::reset( ref_stream_t stream_ ) {
	M_PROLOG
	_streamOwned.reset();
	_streamRef = &stream_;
	return;
	M_EPILOG
}

HLock HSynchronizedStream::acquire( void ) {
	M_PROLOG
	return ( HLock( _mutex ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( HString const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char const* val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( code_point_t val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int short val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int short unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long long unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( double val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( double long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( float val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( void const* val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( manipulator_t const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( HManipulator const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( HString& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( bool& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( code_point_t& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( char& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( char unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int short& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int short unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long long unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( double& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( double long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( float& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( void const*& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( manipulator_t const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
}

int long HSynchronizedStream::do_read_until( yaal::hcore::HString& store, char const* delim, bool strip ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until( store, delim, strip ) );
	M_EPILOG
}

int long HSynchronizedStream::do_read_until_n( yaal::hcore::HString& store, int long maxcount, char const* delim, bool strip ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until_n( store, maxcount, delim, strip ) );
	M_EPILOG
}

int long HSynchronizedStream::do_read_while( yaal::hcore::HString& store, char const* acquire_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_while( store, acquire_ ) );
	M_EPILOG
}

int long HSynchronizedStream::do_read_while_n( yaal::hcore::HString& store, int long maxcount, char const* acquire_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_while_n( store, maxcount, acquire_ ) );
	M_EPILOG
}

int HSynchronizedStream::do_peek( void ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_peek() );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_fill( code_point_t val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_fill( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_width( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_width( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_precision( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_precision( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_base( BASE val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_base( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_float_format( FLOAT_FORMAT val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_float_format( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_adjust( ADJUST val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_adjust( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_skipws( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_skipws( val_ ) );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_boolalpha( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_boolalpha( val_ ) );
	M_EPILOG
}

bool HSynchronizedStream::do_get_skipws( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_skipws() );
	M_EPILOG
}

bool HSynchronizedStream::do_get_boolalpha( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_boolalpha() );
	M_EPILOG
}

code_point_t HSynchronizedStream::do_get_fill( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_fill() );
	M_EPILOG
}

int HSynchronizedStream::do_get_width( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_width() );
	M_EPILOG
}

int HSynchronizedStream::do_get_precision( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_precision() );
	M_EPILOG
}

HStreamInterface::BASE HSynchronizedStream::do_get_base( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_base() );
	M_EPILOG
}

HStreamInterface::FLOAT_FORMAT HSynchronizedStream::do_get_float_format( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_float_format() );
	M_EPILOG
}

HStreamInterface::ADJUST HSynchronizedStream::do_get_adjust( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_get_adjust() );
	M_EPILOG
}

int long HSynchronizedStream::do_write( void const* buf_, int long size_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( _streamRef ? _streamRef->write( buf_, size_ ) : 0 );
	M_EPILOG
}

int long HSynchronizedStream::do_read( void* buf_, int long size_ ) {
	M_PROLOG
	HLock l( _mutex );
	return ( _streamRef ? _streamRef->read( buf_, size_ ) : 0 );
	M_EPILOG
}

void HSynchronizedStream::do_flush( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef )
		_streamRef->flush();
	return;
	M_EPILOG
}

bool HSynchronizedStream::do_is_valid( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( _streamRef ? _streamRef->is_valid() : false );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HSynchronizedStream::do_poll_type( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( _streamRef ? _streamRef->poll_type() : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HSynchronizedStream::do_data( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( _streamRef ? _streamRef->data() : memory::INVALID_HANDLE );
	M_EPILOG
}

bool HSynchronizedStream::do_good( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_good() );
	M_EPILOG
}

bool HSynchronizedStream::do_fail( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_fail() );
	M_EPILOG
}

bool HSynchronizedStream::do_bad( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_bad() );
	M_EPILOG
}

void HSynchronizedStream::do_clear( void ) {
	M_PROLOG
	HLock l( _mutex );
	HStreamInterface::do_clear();
	return;
	M_EPILOG
}

}

}


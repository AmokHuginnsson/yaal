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

void HSynchronizedStream::reset_owned( owned_stream_t stream_ ) {
	M_PROLOG
	HLock l( _mutex );
	_streamOwned = stream_;
	_streamRef = _streamOwned.raw();
	return;
	M_EPILOG
}

void HSynchronizedStream::reset_referenced( ref_stream_t stream_ ) {
	M_PROLOG
	HLock l( _mutex );
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
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char const* val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( code_point_t val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( char unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int short val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int short unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( int long long unsigned val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( double val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( double long val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( float val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( void const* val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		_streamRef->write( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( manipulator_t const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		*_streamRef << val_;
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_output( HManipulator const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_output( val_ );
	} else if ( _streamRef ) {
		*_streamRef << val_;
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( HString& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( bool& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( code_point_t& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( char& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( char unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int short& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int short unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( int long long unsigned& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( double& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( double long& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( float& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		_streamRef->read( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( void const*& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		*_streamRef >> val_;
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_input( manipulator_t const& val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_input( val_ );
	} else if ( _streamRef ) {
		*_streamRef >> val_;
	}
	return ( *this );
	M_EPILOG
}

int long HSynchronizedStream::do_read_until( yaal::hcore::HString& store, char const* delim, bool strip ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		nRead = HStreamInterface::do_read_until( store, delim, strip );
	} else if ( _streamRef ) {
		nRead = _streamRef->read_until( store, delim, strip );
	}
	return ( nRead );
	M_EPILOG
}

int long HSynchronizedStream::do_read_until_n( yaal::hcore::HString& store, int long maxcount, char const* delim, bool strip ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		nRead = HStreamInterface::do_read_until_n( store, maxcount, delim, strip );
	} else if ( _streamRef ) {
		nRead = _streamRef->read_until_n( store, maxcount, delim, strip );
	}
	return ( nRead );
	M_EPILOG
}

int long HSynchronizedStream::do_read_while( yaal::hcore::HString& store, char const* acquire_ ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		nRead = HStreamInterface::do_read_while( store, acquire_ );
	} else if ( _streamRef ) {
		nRead = _streamRef->read_while( store, acquire_ );
	}
	return ( nRead );
	M_EPILOG
}

int long HSynchronizedStream::do_read_while_n( yaal::hcore::HString& store, int long maxcount, char const* acquire_ ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		nRead = HStreamInterface::do_read_while_n( store, maxcount, acquire_ );
	} else if ( _streamRef ) {
		nRead = _streamRef->read_while_n( store, maxcount, acquire_ );
	}
	return ( nRead );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_consume( yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_consume( pattern_ );
	} else if ( _streamRef ) {
		_streamRef->consume( pattern_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_ignore( int count_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_ignore( count_ );
	} else if ( _streamRef ) {
		_streamRef->ignore( count_ );
	}
	return ( *this );
	M_EPILOG
}

int HSynchronizedStream::do_peek( void ) {
	M_PROLOG
	HLock l( _mutex );
	int code( 0 );
	if ( _streamRef == this ) {
		code = HStreamInterface::do_peek();
	} else if ( _streamRef ) {
		code = _streamRef->peek();
	}
	return ( code );
	M_EPILOG
}

int HSynchronizedStream::do_immediate_read_size( void ) const {
	M_PROLOG
	HLock l( _mutex );
	int code( 0 );
	if ( _streamRef == this ) {
		code = HStreamInterface::do_immediate_read_size();
	} else if ( _streamRef ) {
		code = _streamRef->immediate_read_size();
	}
	return ( code );
	M_EPILOG
}

int HSynchronizedStream::do_pending_write_size( void ) const {
	M_PROLOG
	HLock l( _mutex );
	int code( 0 );
	if ( _streamRef == this ) {
		code = HStreamInterface::do_pending_write_size();
	} else if ( _streamRef ) {
		code = _streamRef->pending_write_size();
	}
	return ( code );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_fill( code_point_t val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_fill( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_fill( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_width( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_width( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_width( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_precision( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_precision( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_precision( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_mode( MODE val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_mode( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_mode( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_base( BASE val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_base( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_base( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_float_format( FLOAT_FORMAT val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_float_format( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_float_format( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_adjust( ADJUST val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_adjust( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_adjust( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_skipws( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_skipws( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_skipws( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_boolalpha( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_boolalpha( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_boolalpha( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_buffered_io( bool val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_buffered_io( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_buffered_io( val_ );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HSynchronizedStream::do_set_io_buffer_size( int val_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_set_io_buffer_size( val_ );
	} else if ( _streamRef ) {
		_streamRef->set_io_buffer_size( val_ );
	}
	return ( *this );
	M_EPILOG
}

bool HSynchronizedStream::do_get_skipws( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_skipws() : HStreamInterface::do_get_skipws() );
	M_EPILOG
}

bool HSynchronizedStream::do_get_boolalpha( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_boolalpha() : HStreamInterface::do_get_boolalpha() );
	M_EPILOG
}

bool HSynchronizedStream::do_get_buffered_io( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_buffered_io() : HStreamInterface::do_get_buffered_io() );
	M_EPILOG
}

int HSynchronizedStream::do_get_io_buffer_size( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_io_buffer_size() : HStreamInterface::do_get_io_buffer_size() );
	M_EPILOG
}

code_point_t HSynchronizedStream::do_get_fill( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_fill() : 0_ycp );
	M_EPILOG
}

int HSynchronizedStream::do_get_width( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_width() : HStreamInterface::do_get_width() );
	M_EPILOG
}

int HSynchronizedStream::do_get_precision( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_precision() : HStreamInterface::do_get_precision() );
	M_EPILOG
}

HStreamInterface::MODE HSynchronizedStream::do_get_mode( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_mode() : HStreamInterface::do_get_mode() );
	M_EPILOG
}

HStreamInterface::BASE HSynchronizedStream::do_get_base( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_base() : HStreamInterface::do_get_base() );
	M_EPILOG
}

HStreamInterface::FLOAT_FORMAT HSynchronizedStream::do_get_float_format( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_float_format() : HStreamInterface::do_get_float_format() );
	M_EPILOG
}

HStreamInterface::ADJUST HSynchronizedStream::do_get_adjust( void ) const {
	M_PROLOG
	HLock l( _mutex );
	return ( ( _streamRef && ( _streamRef != this ) ) ? _streamRef->get_adjust() : HStreamInterface::do_get_adjust() );
	M_EPILOG
}

int long HSynchronizedStream::do_write( void const* buf_, int long size_ ) {
	M_PROLOG
	HLock l( _mutex );
	int long nWritten( 0 );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_write()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		nWritten = _streamRef->write( buf_, size_ );
	}
	return ( nWritten );
	M_EPILOG
}

int long HSynchronizedStream::do_read( void* buf_, int long size_ ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_read()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		nRead = _streamRef->read( buf_, size_ );
	}
	return ( nRead );
	M_EPILOG
}

int long HSynchronizedStream::do_read_some( void* buf_, int long size_ ) {
	M_PROLOG
	HLock l( _mutex );
	int long nRead( 0 );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_read_some()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		nRead = _streamRef->read_some( buf_, size_ );
	}
	return ( nRead );
	M_EPILOG
}

void HSynchronizedStream::do_seek( int long offset_, SEEK anchor_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_seek()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		_streamRef->seek( offset_, anchor_ );
	}
	return;
	M_EPILOG
}

void HSynchronizedStream::do_flush( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_flush()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		_streamRef->flush();
	}
	return;
	M_EPILOG
}

bool HSynchronizedStream::do_is_valid( void ) const {
	M_PROLOG
	HLock l( _mutex );
	bool valid( false );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_is_valid()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		valid = _streamRef->is_valid();
	}
	return ( valid );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HSynchronizedStream::do_poll_type( void ) const {
	M_PROLOG
	HLock l( _mutex );
	POLL_TYPE pollType( POLL_TYPE::INVALID );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_poll_type()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		pollType = _streamRef->poll_type();
	}
	return ( pollType );
	M_EPILOG
}

void const* HSynchronizedStream::do_data( void ) const {
	M_PROLOG
	HLock l( _mutex );
	void const* handle( memory::INVALID_HANDLE );
	if ( _streamRef == this ) {
		M_ASSERT( !"Missing implementation of `do_data()` in HSynchronizedStream inherited class."[0] );
	} else if ( _streamRef ) {
		handle = _streamRef->data();
	}
	return ( handle );
	M_EPILOG
}

bool HSynchronizedStream::do_good( void ) const {
	M_PROLOG
	HLock l( _mutex );
	bool isGood( false );
	if ( _streamRef == this ) {
		isGood = HStreamInterface::do_good();
	} else if ( _streamRef )  {
		isGood = _streamRef->good();
	}
	return ( isGood );
	M_EPILOG
}

bool HSynchronizedStream::do_fail( void ) const {
	M_PROLOG
	HLock l( _mutex );
	bool failed( true );
	if ( _streamRef == this ) {
		failed = HStreamInterface::do_fail();
	} else if ( _streamRef )  {
		failed = _streamRef->fail();
	}
	return ( failed );
	M_EPILOG
}

bool HSynchronizedStream::do_bad( void ) const {
	M_PROLOG
	HLock l( _mutex );
	bool isBad( false );
	if ( _streamRef == this ) {
		isBad = HStreamInterface::do_good();
	} else if ( _streamRef )  {
		isBad = _streamRef->good();
	}
	return ( isBad );
	M_EPILOG
}

void HSynchronizedStream::do_clear( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_clear();
	} else if ( _streamRef ) {
		_streamRef->clear();
	}
	return;
	M_EPILOG
}

void HSynchronizedStream::do_reset( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef == this ) {
		HStreamInterface::do_reset();
	} else if ( _streamRef ) {
		_streamRef->reset();
	}
	return;
	M_EPILOG
}

}

}


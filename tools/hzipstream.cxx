/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hzipstream.cxx - this file is integral part of `yaal' project.

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
#include <zlib.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hzipstream.hxx"
#include "tools.hxx"

namespace yaal {

namespace tools {

int _compressionLevel_ = Z_DEFAULT_COMPRESSION; /* Library dafault, it is the same as 6. */
int long _zBufferSize_ = 256 * 1024; /* As advised in the documentation. */

HZipStream::HZipStream( MODE::mode_t mode_ )
	: _mode( mode_ ), _error( Z_OK ), _streamOwned(), _streamRef( NULL ),
	_zStream( sizeof ( z_stream ) ), _zBufferIn( _zBufferSize_ ), _zBufferOut( _zBufferSize_ ),
	_offset( 0 ) {
	M_PROLOG
	init();
	return;
	M_EPILOG
}

HZipStream::HZipStream( owned_stream_t stream_, MODE::mode_t mode_ )
	: _mode( mode_ ), _error( Z_OK ), _streamOwned( stream_ ), _streamRef( _streamOwned.raw() ),
	_zStream( sizeof ( z_stream ) ), _zBufferIn( _zBufferSize_ ), _zBufferOut( _zBufferSize_ ),
	_offset( 0 ) {
	M_PROLOG
	init();
	return;
	M_EPILOG
}

HZipStream::HZipStream( ref_stream_t stream_, MODE::mode_t mode_ )
	: _mode( mode_ ), _error( Z_OK ), _streamOwned(), _streamRef( &stream_ ),
	_zStream( sizeof ( z_stream ) ), _zBufferIn( _zBufferSize_ ), _zBufferOut( _zBufferSize_ ),
	_offset( 0 ) {
	M_PROLOG
	init();
	return;
	M_EPILOG
}

HZipStream::~HZipStream( void ) {
	M_PROLOG
	cleanup();
	return;
	M_DESTRUCTOR_EPILOG
}

void HZipStream::cleanup( void ) {
	M_PROLOG
	z_stream* zstream( _zStream.get<z_stream>() );
	if ( _mode == MODE::DEFLATE ) {
		do_write( NULL, 0 );
		deflateEnd( zstream );
	} else
		inflateEnd( zstream );
	_offset = 0;
	_error = Z_OK;
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T>
inline int fwd_inflateInit( T arg_ ) {
	return ( inflateInit( arg_ ) );
}
template<typename T1, typename T2>
inline int fwd_deflateInit( T1 arg1_, T2 arg2_ ) {
	return ( deflateInit( arg1_, arg2_ ) );
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

void HZipStream::init( void ) {
	M_PROLOG
	z_stream* zstream( _zStream.get<z_stream>() );
	zstream->zalloc = Z_NULL;
	zstream->zfree = Z_NULL;
	zstream->opaque = Z_NULL;
	zstream->avail_in = 0;
	zstream->next_in = Z_NULL;
	zstream->avail_out = static_cast<uInt>( _zBufferOut.get_size() );
	zstream->next_out = Z_NULL;
	M_ENSURE( ( _error = ( ( _mode == MODE::DEFLATE ) ? fwd_deflateInit( zstream, _compressionLevel_ ) : fwd_inflateInit( zstream ) ) ) == Z_OK );
	return;
	M_EPILOG
}

void HZipStream::reset( owned_stream_t stream_ ) {
	M_PROLOG
	_streamOwned = stream_;
	_streamRef = _streamOwned.raw();
	cleanup();
	init();
	M_EPILOG
}

void HZipStream::reset( ref_stream_t stream_ ) {
	M_PROLOG
	_streamOwned.reset();
	_streamRef = &stream_;
	cleanup();
	init();
	M_EPILOG
}

int long HZipStream::do_write( void const* const buf_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _streamRef );
	z_stream* zstream( _zStream.get<z_stream>() );
	zstream->avail_in = static_cast<uInt>( size_ );
	/* Hello, idiotic interface :(
	 */
	zstream->next_in = const_cast<Bytef*>( static_cast<Bytef const*>( buf_ ) );
	int long nWrite( 0 );
	void* buf( _zBufferOut.raw() );
	int long const CHUNK( _zBufferOut.get_size() );
	do {
		zstream->avail_out = static_cast<uInt>( CHUNK );
		zstream->next_out = reinterpret_cast<Bytef*>( buf );
		int err( deflate( zstream, size_ > 0 ? Z_NO_FLUSH : Z_FINISH ) );
		M_ASSERT( err != Z_STREAM_ERROR ); /* lets debug zlib library, why the f*ck not */
		err = 0; /* in release, unused variable warning */
		nWrite += _streamRef->write( buf, CHUNK - zstream->avail_out );
	} while ( zstream->avail_out == 0 );
	M_ASSERT( zstream->avail_in == 0 ); /* yeach, why the f*ck not */
	return ( nWrite );
	M_EPILOG
}

int long HZipStream::prepare_data( void ) {
	M_PROLOG
	_offset = 0;
	z_stream* zstream( _zStream.get<z_stream>() );
	int long const CHUNK( _zBufferIn.get_size() );
	void* in = _zBufferIn.raw();
	int long nRead( 0 );
	if ( zstream->avail_out ) {
		nRead = _streamRef->read( in, CHUNK );
		zstream->next_in = static_cast<Bytef*>( in );
		zstream->avail_in = static_cast<uInt>( nRead );
	} else
		nRead = zstream->avail_in;
	if ( nRead > 0 ) {
		zstream->next_out = reinterpret_cast<Bytef*>( _zBufferOut.raw() );
		zstream->avail_out = static_cast<uInt>( CHUNK );
		_error = inflate( zstream, Z_NO_FLUSH );
		M_ENSURE( ( _error == Z_OK ) || ( _error == Z_STREAM_END ) );
	}
	return ( nRead > 0 ? CHUNK - zstream->avail_out : 0 );
	M_EPILOG
}

int long HZipStream::do_read( void* const buf_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _streamRef );
	z_stream* zstream( _zStream.get<z_stream>() );
	int long const CHUNK( _zBufferOut.get_size() );
	char* buf( _zBufferOut.get<char>() );
	int long copied( 0 );
	while ( copied < size_ ) {
		int long have( min<int long>( ( CHUNK - zstream->avail_out ) - _offset, size_ - copied ) );
		if ( have < 1 ) {
			if ( ( _error == Z_STREAM_END ) || ! prepare_data() )
				break;
			continue;
		}
		::memcpy( static_cast<char*>( buf_ ) + copied, buf + _offset, have );
		copied += have;
		_offset += have;
	}
	return ( copied );
	M_EPILOG
}

void HZipStream::do_flush( void ) const {
	M_PROLOG
	M_ASSERT( _streamRef );
	_streamRef->flush();
	return;
	M_EPILOG
}

bool HZipStream::do_is_valid( void ) const {
	M_PROLOG
	return ( _streamRef ? _streamRef->is_valid() : false );
	M_EPILOG
}

template<typename call_t>
HZipStream& HZipStream::operator()( call_t call_ ) {
	M_PROLOG
	M_ASSERT( _streamRef );
	call_( _streamRef );
	return ( *this );
	M_EPILOG
}

char const* HZipStream::error_message( int ) {
	char const* msg( "zlib: ok" );
	switch ( _error ) {
		case Z_OK:
		break;
		case Z_ERRNO:
			msg = "zlib: read/write error";
		break;
		case Z_STREAM_ERROR:
			msg = "zlib: invalid compression level";
		break;
		case Z_DATA_ERROR:
			msg = "zlib: invalid or incomplete deflate data";
		break;
		case Z_MEM_ERROR:
			msg = "zlib: out of memory";
		break;
		case Z_VERSION_ERROR:
			msg = "zlib: version mismatch!";
		break;
		case Z_NEED_DICT:
			msg = "zlib: dictionary required to uncompress data";
		break;
		default:
			msg = "zlib: unknown error";
		break;
	}
	return ( msg );
}

}

}


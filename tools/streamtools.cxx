/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  streamtools.cxx - this file is integral part of `yaal' project.

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

#ifdef __MSVCXX__
#include <unistd.h>
#endif /* #ifdef __MSVCXX__ */

#include <arpa/inet.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "streamtools.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hsocket.hxx"
#include "hcore/hnumber.hxx"
#include "hcore/hcomplex.hxx"
#include "hcore/htime.hxx"
#include "util.hxx"

using namespace yaal::hcore;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

HBinaryFormatter::HBinaryFormatter( HStreamInterface* stream_ )
	: _stream( stream_ ) {
}

HStreamInterface& HBinaryFormatter::operator << ( HStreamInterface::manipulator_t M ) {
	M_PROLOG
	return ( M( *_stream) );
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T>
inline T fwd_htons( T arg_ ) {
	return ( static_cast<T>( htons( static_cast<uint16_t>( arg_ ) ) ) );
}
template<typename T>
inline T fwd_htonl( T arg_ ) {
	return ( static_cast<T>( htonl( static_cast<uint32_t> ( arg_ ) ) ) );
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

HBinaryFormatter& HBinaryFormatter::operator << ( int short v_ ) {
	M_PROLOG
	int short v( fwd_htons( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int short unsigned v_ ) {
	M_PROLOG
	int short v( static_cast<int short>( fwd_htons( v_ ) ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int v_ ) {
	M_PROLOG
	int v( fwd_htonl( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int unsigned v_ ) {
	M_PROLOG
	int unsigned v( fwd_htonl( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int long v_ ) {
	M_PROLOG
	int long v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int long unsigned v_ ) {
	M_PROLOG
	int long unsigned v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int long long v_ ) {
	M_PROLOG
	int long long v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( int long long unsigned v_ ) {
	M_PROLOG
	int long long unsigned v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( float v_ ) {
	M_PROLOG
	float v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( double v_ ) {
	M_PROLOG
	double v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::operator << ( double long v_ ) {
	M_PROLOG
	double long v( hton( v_ ) );
	return ( binary( &v, static_cast<int>( sizeof ( v ) ) ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::binary( void const* buf_, int size_ ) {
	M_PROLOG
	int const mask[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	for ( int i = 0; i < size_; ++ i ) {
		for ( int b = 0; b < 8; ++ b )
			(*_stream) << ( static_cast<char const*>( buf_ )[ i ] & mask[ b ] ? '1' : '0' );
	}
	return ( *this );
	M_EPILOG
}

HBinaryFormatter HBinaryFormatterSeed::create( HStreamInterface* stream_ ) const {
	M_PROLOG
	return ( HBinaryFormatter( stream_ ) );
	M_EPILOG
}

HBinaryFormatter operator << ( HStreamInterface& stream_, HBinaryFormatterSeed const& seed_ ) {
	M_PROLOG
	return ( seed_.create( &stream_ ) );
	M_EPILOG
}

HBinaryFormatterSeed bin;

HString get_stream_id( HStreamInterface* stream ) {
	HString name;
	if ( HFile* f = dynamic_cast<HFile*>( stream ) ) {
		name = f->get_path();
	} else if ( HSocket* s = dynamic_cast<HSocket*>( stream ) ) {
		name = s->get_host_name();
	} else if ( stream == &cin ) {
		name = "*standard input*";
	} else if ( stream == &cout ) {
		name = "*standard output*";
	} else if ( stream == &cerr ) {
		name = "*standard error*";
	} else if ( stream == &clog ) {
		name = "*standard log*";
	} else if ( stream == &hcore::log ) {
		name = "*log*";
	} else {
		name = "*anonymous stream*";
	}
	return ( name );
}

HStreamInterface& ensure( HStreamInterface& stream_ ) {
	M_PROLOG
	M_ENSURE_EX( stream_.is_valid(), get_stream_id( &stream_ ) );
	return ( stream_ );
	M_EPILOG
}

HStreamInterface::ptr_t ensure( HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	M_ENSURE_EX( stream_->is_valid(), get_stream_id( stream_.raw() ) );
	return ( stream_ );
	M_EPILOG
}

HTee::HTee( HTee const& tee_ )
	: HStreamInterface()
	, _stream1( tee_._stream1 )
	, _stream2( tee_._stream2 ) {
}

int long HTee::do_write( void const* const data_, int long size_ ) {
	M_PROLOG
	int long w1( _stream1.write( data_, size_ ) );
	int long w2( _stream2.write( data_, size_ ) );
	return ( ( w1 + w2 ) / 2 );
	M_EPILOG
}

int long HTee::do_read( void* const, int long ) {
	M_PROLOG
	M_ASSERT( 0 && "oparation makes no sense for tee stream" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( -1 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
	M_EPILOG
}

void HTee::do_flush( void ) {
	M_PROLOG
	_stream1.flush();
	_stream2.flush();
	return;
	M_EPILOG
}

bool HTee::do_is_valid( void ) const {
	M_PROLOG
	return ( _stream1.is_valid() && _stream2.is_valid() );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HTee::do_poll_type( void ) const {
	M_PROLOG
	return ( is_valid() ? POLL_TYPE::EMULATED : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HTee::do_data( void ) const {
	M_PROLOG
	return ( is_valid() ? this : memory::INVALID_HANDLE );
	M_EPILOG
}

bool HTee::do_good( void ) const {
	M_PROLOG
	return ( _stream1.good() && _stream2.good() );
	M_EPILOG
}

bool HTee::do_fail( void ) const {
	M_PROLOG
	return ( _stream1.fail() || _stream2.fail() );
	M_EPILOG
}

bool HTee::do_bad( void ) const {
	M_PROLOG
	return ( _stream1.bad() || _stream2.bad() );
	M_EPILOG
}

HTee tee( HStreamInterface::ptr_t stream1_, HStreamInterface::ptr_t stream2_ ) {
	M_PROLOG
	return ( HTee( *stream1_, *stream2_ ) );
	M_EPILOG
}

HTee tee( HStreamInterface& stream1_, HStreamInterface::ptr_t stream2_ ) {
	M_PROLOG
	return ( HTee( stream1_, *stream2_ ) );
	M_EPILOG
}

HTee tee( HStreamInterface::ptr_t stream1_, HStreamInterface& stream2_ ) {
	M_PROLOG
	return ( HTee( *stream1_, stream2_ ) );
	M_EPILOG
}

HTee tee( HStreamInterface& stream1_, HStreamInterface& stream2_ ) {
	M_PROLOG
	return ( HTee( stream1_, stream2_ ) );
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, yaal::tools::HBitmap const& bitmap_ ) {
	M_PROLOG
	for ( HBitmap::const_iterator it( bitmap_.begin() ), end( bitmap_.end() ); it != end; ++ it )
		stream_ << ( *it ? 1 : 0 );
	return ( stream_ );
	M_EPILOG
}

}

namespace hcore {

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HNumber const& number_ ) {
	M_PROLOG
	HNumber n( number_ );
	n.round( out.get_precision() );
	out << n.to_string();
	return ( out );
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTime const& t_ ) {
	M_PROLOG
	out << t_.string();
	return ( out );
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HComplex const& c_ ) {
	M_PROLOG
	out << "(" << c_.re() << "+" << c_.im() << "i)";
	return ( out );
	M_EPILOG
}

}

}


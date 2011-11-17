/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	streamtools.cxx - this file is integral part of yaal project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "streamtools.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hsocket.hxx"
#include "hcore/hnumber.hxx"
#include "hcore/htime.hxx"

using namespace yaal::hcore;

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
	HFile* f = dynamic_cast<HFile*>( stream );
	HSocket* s = dynamic_cast<HSocket*>( stream );
	return ( f ? f->get_path() : ( s ? s->get_host_name() : "anonymous stream" ) );
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
	: HStreamInterface(), _stream1( tee_._stream1 ), _stream2( tee_._stream2 )
	{}

int long HTee::do_write( void const* const data_, int long size_ ) {
	M_PROLOG
	int long w1( _stream1.write( data_, size_ ) );
	int long w2( _stream2.write( data_, size_ ) );
	return ( ( w1 + w2 ) / 2 );
	M_EPILOG
}

int long HTee::do_read( void* const, int long ) {
	M_PROLOG
	M_ASSERT( ! "oparation makes no sense for tee stream" );
#ifdef __MSVCXX__
	return ( -1 );
#endif /* #ifdef __MSVCXX__ */
	M_EPILOG
}

void HTee::do_flush( void ) const {
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

}

namespace hcore {

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HNumber const& n ) {
	M_PROLOG
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

}

}


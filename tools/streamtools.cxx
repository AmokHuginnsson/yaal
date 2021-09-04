/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#include <cstring>
#ifdef __MSVCXX__
#include <unistd.h>
#endif /* #ifdef __MSVCXX__ */

#ifdef HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif /* #ifdef HAVE_ARPA_INET_H */

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
#include "hcore/number.hxx"
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
template<typename T>
inline T fwd_htons( T arg_ ) {
	return ( static_cast<T>( htons( static_cast<uint16_t>( arg_ ) ) ) );
}
template<typename T>
inline T fwd_htonl( T arg_ ) {
	return ( static_cast<T>( htonl( static_cast<uint32_t> ( arg_ ) ) ) );
}
#pragma GCC diagnostic pop
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
	double_long_storage doubleLongStorage;
	store_double_long( v_, doubleLongStorage );
	reverse( doubleLongStorage.data, doubleLongStorage.data + SIZEOF_DOUBLE_LONG );
	return ( binary( doubleLongStorage.data, SIZEOF_DOUBLE_LONG ) );
	M_EPILOG
}

HBinaryFormatter& HBinaryFormatter::binary( void const* buf_, int size_ ) {
	M_PROLOG
	int const mask[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
	for ( int i = 0; i < size_; ++ i ) {
		for ( int b = 0; b < 8; ++ b ) {
			(*_stream) << ( static_cast<char const*>( buf_ )[ i ] & mask[ b ] ? '1' : '0' );
		}
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
	return name;
}

HStreamInterface& ensure( HStreamInterface& stream_ ) {
	M_PROLOG
	M_ENSURE( stream_.is_valid(), get_stream_id( &stream_ ) );
	return stream_;
	M_EPILOG
}

HStreamInterface::ptr_t ensure( HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	M_ENSURE( stream_->is_valid(), get_stream_id( stream_.raw() ) );
	return stream_;
	M_EPILOG
}

namespace stream {

int long pump( yaal::hcore::HStreamInterface& source_, yaal::hcore::HStreamInterface& sink_ ) {
	M_PROLOG
	return ( pump( source_, sink_, system::get_page_size() ) );
	M_EPILOG
}

int long pump( yaal::hcore::HStreamInterface& source_, yaal::hcore::HStreamInterface& sink_, int bufferSize_ ) {
	M_PROLOG
	HChunk chunk( bufferSize_ );
	char* buf( chunk.get<char>() );
	bool ok( true );
	int long nWrittenTotal( 0 );
	while ( ok ) {
		int nRead( static_cast<int>( M_TEMP_FAILURE_RETRY( source_.read( buf, bufferSize_ ) ) ) );
		if ( nRead <= 0 ) {
			break;
		}
		int toWrite( nRead );
		char* p( buf );
		while ( toWrite > 0 ) {
			int nWritten( static_cast<int>( M_TEMP_FAILURE_RETRY( sink_.write( p, toWrite ) ) ) );
			if ( nWritten <= 0 ) {
				ok = false;
				if ( nWritten < 0 ) {
					nWrittenTotal = -1;
				}
				break;
			}
			nWrittenTotal += nWritten;
			toWrite -= nWritten;
			p += nWritten;
		}
	}
	sink_.flush();
	return nWrittenTotal;
	M_EPILOG
}

}

HCat::HCat( streams_t&& streams_ )
	: _streams( yaal::move( streams_ ) )
	, _current( 0 ) {
	return;
}

HCat::HCat( HCat const& other_ )
	: _streams( other_._streams )
	, _current( other_._current ) {
}

HCat::HCat( HCat&& other_ )
	: _streams( yaal::move( other_._streams ) )
	, _current( other_._current ) {
	other_._current = -1;
}

HCat::~HCat( void ) {
	return;
}

int long HCat::do_write( void const*, int long ) {
	M_ASSERT( 0 && "operation makes no sense for `cat` stream" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( -1 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

int long HCat::do_read( void* buffer_, int long size_ ) {
	int long nReadTotal( 0 );
	while ( ( _current < _streams.get_size() ) && ( size_ > 0 ) ) {
		int long nRead( _streams[_current]->read( static_cast<char*>( buffer_ ) + nReadTotal, size_ ) );
		if ( nRead > 0 ) {
			nReadTotal += nRead;
			size_ -= nRead;
		} else {
			++ _current;
		}
	}
	return nReadTotal;
}

void HCat::do_flush( void ) {
	return _streams[_current]->flush();
}

bool HCat::do_is_valid( void ) const {
	return ( ( _current < _streams.get_size() ) && _streams[_current]->is_valid() ) || ( immediate_read_size() > 0 );
}

HStreamInterface::POLL_TYPE HCat::do_poll_type( void ) const {
	return ( _current < _streams.get_size() )
		? _streams[_current]->poll_type()
		: ( immediate_read_size() > 0 ? HStreamInterface::POLL_TYPE::EMULATED : HStreamInterface::POLL_TYPE::INVALID );
}

void const* HCat::do_data( void ) const {
	return ( _current < _streams.get_size() ) ? _streams[_current]->data() : this;
}

HTee::HTee( HTee const& tee_ )
	: HStreamInterface()
	, _stream1( tee_._stream1 )
	, _stream2( tee_._stream2 ) {
}

HTee::~HTee( void ) {
	M_PROLOG
	flush();
	return;
	M_DESTRUCTOR_EPILOG
}

int long HTee::do_write( void const* data_, int long size_ ) {
	M_PROLOG
	int long w1( _stream1.write( data_, size_ ) );
	int long w2( _stream2.write( data_, size_ ) );
	return ( ( w1 + w2 ) / 2 );
	M_EPILOG
}

int long HTee::do_read( void*, int long ) {
	M_PROLOG
	M_ASSERT( 0 && "operation makes no sense for `tee` stream" );
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

int HTee::do_immediate_read_size( void ) const {
	M_PROLOG
	return ( min( _stream1.immediate_read_size(), _stream2.immediate_read_size() ) );
	M_EPILOG
}

int HTee::do_pending_write_size( void ) const {
	M_PROLOG
	return ( min( _stream1.pending_write_size(), _stream2.pending_write_size() ) );
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
	return stream_;
	M_EPILOG
}

}

namespace hcore {

struct HNumber::ElementaryFunctions {
	static void serialize( HStreamInterface& stream_, HNumber const& number_ ) {
		M_PROLOG
		stream_ << number_._precision << number_._leafCount << number_._integralPartSize << static_cast<u8_t>( number_._negative ? 1 : 0 );
		stream_.write( number_._canonical.raw(), number_._leafCount * static_cast<int>( sizeof ( HNumber::integer_t ) ) );
		return;
		M_EPILOG
	}
	static void deserialize( HStreamInterface& stream_, HNumber& number_ ) {
		M_PROLOG
		u8_t neg( 0 );
		HNumber n;
		do {
			stream_ >> n._precision;
			if ( ! stream_.good() ) {
				break;
			}
			stream_ >> n._leafCount;
			if ( ! stream_.good() ) {
				break;
			}
			stream_ >> n._integralPartSize;
			if ( ! stream_.good() ) {
				break;
			}
			stream_ >> neg;
			if ( ! stream_.good() ) {
				break;
			}
			number_._negative = neg != 0;

			if (
				( ( neg != 0 ) && ( neg != 1 ) )
				|| ( n._precision < number::HARDCODED_MINIMUM_PRECISION )
				|| ( n._leafCount < 0 )
				|| ( n._integralPartSize < 0 )
				|| ( n._integralPartSize > n._leafCount )
			) {
				break;
			}
			if ( n._leafCount > 0 ) {
				n._canonical.realloc( chunk_size<HNumber::integer_t>( n._leafCount ) );
				int toRead( n._leafCount * static_cast<int>( sizeof ( HNumber::integer_t ) ) );
				if ( stream_.read( n._canonical.raw(), toRead ) != toRead ) {
					break;
				}
			}
			number_ = yaal::move( n );
		} while ( false );
		return;
		M_EPILOG
	}
};

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HNumber const& number_ ) {
	M_PROLOG
	if ( out.get_mode() == HStreamInterface::MODE::TEXT ) {
		HNumber n( number_ );
		n.round( out.get_precision() );
		out << n.to_string();
	} else {
		HNumber::ElementaryFunctions::serialize( out, number_ );
	}
	return out;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HNumber& number_ ) {
	M_PROLOG
	if ( in.get_mode() == HStreamInterface::MODE::TEXT ) {
		HString strNo;
		in >> strNo;
		number_ = strNo;
	} else {
		HNumber::ElementaryFunctions::deserialize( in, number_ );
	}
	return in;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTime const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == HStreamInterface::MODE::TEXT ) {
		out << t_.string();
	} else {
		out << t_.raw();
	}
	return out;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HTime& t_ ) {
	M_PROLOG
	if ( in.get_mode() == HStreamInterface::MODE::TEXT ) {
		HString strTime;
		in >> strTime;
		t_.from_string( strTime );
	} else {
		i64_t v( 0 );
		in >> v;
		if ( in.good() ) {
			t_.set( v );
		}
	}
	return in;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HComplex const& c_ ) {
	M_PROLOG
	if ( out.get_mode() == HStreamInterface::MODE::TEXT ) {
		out << "(" << c_.re() << "+" << c_.im() << "i)";
	} else {
		out << c_.re() << c_.im();
	}
	return out;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HComplex& c_ ) {
	M_PROLOG
	HComplex::value_type re( 0 );
	HComplex::value_type im( 0 );
	if ( in.get_mode() == HStreamInterface::MODE::TEXT ) {
		do {
			HString s;
			in >> s;
			if ( s.is_empty() || ( s.front() != '(' ) ) {
				break;
			}
			int long plusIdx( s.find( '+'_ycp ) );
			if ( ( plusIdx == HString::npos ) || ( plusIdx > ( s.get_length() - 4 ) ) ) {
				break;
			}
			re = lexical_cast<HComplex::value_type>( s.substr( 1, plusIdx - 1 ) );
			im = lexical_cast<HComplex::value_type>( s.substr( plusIdx + 1, s.get_length() - ( plusIdx + 3 ) ) );
			c_.set( re, im );
		} while ( false );
	} else {
		do {
			in >> re;
			if ( ! in.good() ) {
				break;
			}
			in >> im;
			if ( ! in.good() ) {
				break;
			}
			c_.set( re, im );
		} while ( false );
	}
	return in;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& os_, time::duration_t const& duration_ ) {
	M_PROLOG
	if ( os_.get_mode() == HStreamInterface::MODE::TEXT ) {
		os_ << duration_to_string( duration_, time::scale( duration_ ) );
	} else {
		os_ << duration_.get();
	}
	return os_;
	M_EPILOG
}

yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& is_, time::duration_t& duration_ ) {
	M_PROLOG
	if ( is_.get_mode() == HStreamInterface::MODE::TEXT ) {
		bool skipWS( is_.get_skipws() );
		is_.set_skipws( false );
		HString trace;
		HString strVal;
		HString strUnit;
		HString white;
		time::duration_t duration( 0 );
		while ( true ) {
			if ( ! is_.good() ) {
				break;
			}
			trace.clear();
			is_.read_while( white, character_class<CHARACTER_CLASS::WHITESPACE>().data() );
			trace.append( white );
			is_ >> strVal;
			trace.append( strVal );
			time::duration_t::value_type val( 0 );
			try {
				val = lexical_cast<time::duration_t::value_type>( strVal );
			} catch ( ... ) {
				break;
			}
			is_.read_while( white, character_class<CHARACTER_CLASS::WHITESPACE>().data() );
			trace.append( white );
			is_ >> strUnit;
			trace.append( strUnit );
			time::UNIT unit( time::UNIT::SECOND );
			try {
				unit = lexical_cast<time::UNIT>( strUnit );
			} catch ( ... ) {
				break;
			}
			duration += time::duration( val, unit );
			duration_ = duration;
		}
		if ( ! trace.is_empty() ) {
			HUTF8String utf8( trace );
			is_.unread( utf8.c_str(), utf8.byte_count() );
		}
		is_.set_skipws( skipWS );
	} else {
		time::duration_t::value_type val( 0 );
		is_ >> val;
		duration_ = time::duration_t( val );
	}
	return is_;
	M_EPILOG
}

HDurationFormatter::HDurationFormatter( yaal::hcore::HStreamInterface* stream_, yaal::hcore::time::UNIT scale_, yaal::hcore::time::UNIT_FORM durationUnitForm_, bool autoScale_ )
	: _stream( stream_ )
	, _scale( scale_ )
	, _durationUnitForm( durationUnitForm_ )
	, _autoScale( autoScale_ ) {
}

HDurationFormatter& HDurationFormatter::operator << ( time::duration_t const& duration_ ) {
	M_PROLOG
	if ( _stream->get_mode() == HStreamInterface::MODE::TEXT ) {
		*_stream << duration_to_string( duration_, _autoScale ? time::scale( duration_ ) : _scale, _durationUnitForm );
	} else {
		*_stream << duration_;
	}
	return ( *this );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HDurationFormatter::operator << ( yaal::hcore::HStreamInterface::manipulator_t M ) {
	M_PROLOG
	return ( M( *_stream) );
	M_EPILOG
}

HDurationFormatterSeed::HDurationFormatterSeed( yaal::hcore::time::UNIT scale_, yaal::hcore::time::UNIT_FORM durationUnitForm_, bool autoScale_ )
	: _scale( scale_ )
	, _durationUnitForm( durationUnitForm_ )
	, _autoScale( autoScale_ ) {
	return;
}

HDurationFormatter HDurationFormatterSeed::create( yaal::hcore::HStreamInterface* stream_ ) const {
	return ( HDurationFormatter( stream_, _scale, _durationUnitForm, _autoScale ) );
}

HDurationFormatterSeed HDurationFormatterSeed::operator() ( yaal::hcore::time::UNIT scale_, yaal::hcore::time::UNIT_FORM unitForm_ ) const {
	HDurationFormatterSeed seed( scale_, unitForm_, false );
	return seed;
}

HDurationFormatterSeed HDurationFormatterSeed::operator() ( yaal::hcore::time::UNIT_FORM unitForm_ ) const {
	HDurationFormatterSeed seed( time::UNIT::NANOSECOND, unitForm_, true );
	return seed;
}

HDurationFormatterSeed const durationformat( time::UNIT::NANOSECOND, time::UNIT_FORM::FULL, true );

HDurationFormatter operator << ( yaal::hcore::HStreamInterface& os_, HDurationFormatterSeed const& dfs_ ) {
	return ( dfs_.create( &os_ ) );
}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "base64.hxx"
#include "hcore/hformat.hxx"
#include "hcore/pod.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

char const _base64EncodeTable_[][65] = {
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
};
u8_t _base64DecodeTable_[2][ 256 ];

class HBase64DecodeTableInitializer {
public:
	HBase64DecodeTableInitializer( void ) {
		::memset( _base64DecodeTable_[0], 0, sizeof ( _base64DecodeTable_[0] ) );
		::memset( _base64DecodeTable_[1], 0, sizeof ( _base64DecodeTable_[1] ) );
		for ( size_t m = 0; m < 2; ++ m ) {
			for ( size_t i = 0; i < sizeof ( _base64EncodeTable_[m] ); ++ i ) {
				_base64DecodeTable_[m][ static_cast<int>( _base64EncodeTable_[m][ i ] ) ] = static_cast<u8_t>( i );
			}
		}
	}
} base64DecodeTableInit;

namespace {

void do_buf_3_to_4( HChunk& out, int long& outSize, u32_t in, int mode, int long pad = 0 ) {
	M_ASSERT( ( mode == 0 ) || ( mode == 1 ) );
	static int const ENC_OUT_SIZE( 4 );
	out.realloc( outSize + ENC_OUT_SIZE );
	char* buf( out.get<char>() + outSize );
	buf[ 0 ] = _base64EncodeTable_[mode][ ( in >> 18 ) & 63 ];
	buf[ 1 ] = _base64EncodeTable_[mode][ ( in >> 12 ) & 63 ];
	buf[ 2 ] = _base64EncodeTable_[mode][ ( in >> 6 ) & 63 ];
	buf[ 3 ] = _base64EncodeTable_[mode][ in & 63 ];
	if ( pad ) {
		::memset( ( buf + ENC_OUT_SIZE - pad ), '=', static_cast<size_t>( pad ) );
	}
	outSize += ENC_OUT_SIZE;
	return;
}

void do_buf_4_to_3( char* out, u32_t const& in, int size = 3 ) {
	out[ 0 ] = static_cast<char>( ( in >> 16 ) & 255 );
	if ( size > 1 ) {
		out[ 1 ] = static_cast<char>( ( in >> 8 ) & 255 );
	}
	if ( size > 2 ) {
		out[ 2 ] = static_cast<char>( in & 255 );
	}
	return;
}

void base64_raw_encode( HChunk& output_, int long& outputSize_, char const* ptr, int long length, bool standardCompliantMode ) {
	M_PROLOG
	int shifts[] = { 16, 8, 0 };
	static int const PREALLOCATE( 128 );
	output_.realloc( PREALLOCATE );
	outputSize_ = 0;
	u32_t coder = 0;
	for ( int long i = 0; i < length; ++ i ) {
		int shift = shifts[ i % 3 ];
		coder |= static_cast<u32_t>( static_cast<u8_t>( ptr[ i ] ) << shift );
		if ( ! shift ) {
			do_buf_3_to_4( output_, outputSize_, coder, standardCompliantMode ? 1 : 0 );
			coder = 0;
		}
	}
	if ( length % 3 ) {
		do_buf_3_to_4( output_, outputSize_, coder, standardCompliantMode ? 1 : 0, 3 - ( length % 3 ) );
	}
	return;
	M_EPILOG
}

}

yaal::hcore::HString base64::encode( yaal::hcore::HUTF8String const& message, bool standardCompliantMode ) {
	M_PROLOG
	HChunk c;
	int long size( 0 );
	base64_raw_encode( c, size, message.raw(), message.byte_count(), standardCompliantMode );
	return ( HString( c.get<char>(), size ) );
	M_EPILOG
}

inline bool is_base64_character( code_point_t ch_, bool standardCompliantMode_ ) {
	return ( ( ( ch_ >= 'A' ) && ( ch_ <= 'Z' ) )
				|| ( ( ch_ >= 'a' ) && ( ch_ <= 'z' ) )
				|| ( ( ch_ >= '0' ) && ( ch_ <= '9' ) )
				|| ( ! standardCompliantMode_ && ( ch_ == '-' ) ) || ( standardCompliantMode_ && ( ch_ == '+' ) )
				|| ( ! standardCompliantMode_ && ( ch_ == '_' ) ) || ( standardCompliantMode_ && ( ch_ == '/' ) )
				|| ( ch_ == '=' ) );
}

namespace {

int long base64_raw_decode( char const* input, int long inputSize, char* output, int long bufSize, bool standardCompliantMode ) {
	M_PROLOG
	int shifts[] = { 18, 12, 6, 0 };
	u32_t coder = 0;
	int long size( 0 );
	int long i( 0 );
	M_ENSURE( ( ( ( ( inputSize * 3 ) / 4 ) + 1 ) <= bufSize ) || ( ! inputSize ) );
	for ( ; ( i < inputSize ) && ( input[ i ] != '=' ); ++ i ) {
		int shift = shifts[ i % 4 ];
		/*
		 * static_cast<u8_t>() is done for clipping.
		 * static_cast<u32_t>() is done to silence (spurious) clang warning.
		 */
		coder |= static_cast<u32_t>( static_cast<u8_t>( ( _base64DecodeTable_[standardCompliantMode ? 1 : 0][ static_cast<u8_t>( input[ i ] ) ] ) ) << shift );
		if ( ! shift ) {
			do_buf_4_to_3( output + size, coder );
			size += 3;
			coder = 0;
		}
	}
	if ( ( i %= 4 ) ) {
		M_ENSURE( i > 1 );
		do_buf_4_to_3( output + size, coder, static_cast<int>( ( bufSize - size - 1 ) ) );
		size += ( ( i * 3 ) / 4 );
	}
	return size;
	M_EPILOG
}

}

yaal::hcore::HString base64::decode( yaal::hcore::HString const& message, bool standardCompliantMode ) {
	M_PROLOG
	int long len = message.get_length();
	HChunk output;
	if ( len > 0 ) {
		output.realloc( len, HChunk::STRATEGY::EXACT );
		HChunk input( len );
		char* inputBuffer( input.get<char>() );
		int i( 0 );
		int pos( 0 );
		for ( code_point_t ch : message ) {
			M_ENSURE( is_base64_character( ch, standardCompliantMode ) || is_whitespace( ch ), format( "char: %C, at position: %ld", ch, pos ) );
			if ( is_base64_character( ch, standardCompliantMode ) ) {
				inputBuffer[i] = static_cast<char>( ch.get() );
				++ i;
			}
			++ pos;
		}
		base64_raw_decode( inputBuffer, len, output.get<char>(), output.get_size(), standardCompliantMode );
	}
	return ( len > 0 ? output.get<char>() : "" );
	M_EPILOG
}

void base64::encode( yaal::hcore::HStreamInterface& in, yaal::hcore::HStreamInterface& out, bool standardCompliantMode, int wrap_ ) {
	M_PROLOG
	M_ENSURE( wrap_ >= 0 );
	int const BASE64LINELEN = 57;
	char buf[BASE64LINELEN];
	int long inputSize( 0 );
	int long outputSize( 0 );
	HChunk output;
	int long offset( 0 );
	bool needEndl( false );
	while ( ( inputSize = in.read( buf, sizeof ( buf ) ) ) > 0 ) {
		base64_raw_encode( output, outputSize, buf, inputSize, standardCompliantMode );
		char const* ptr = output.get<char>();
		if ( wrap_ ) {
			while ( ( offset + outputSize ) >= wrap_ ) {
				int long nWrite( wrap_ - offset );
				out.write( ptr, nWrite );
				ptr += nWrite;
				outputSize -= nWrite;
				offset = 0;
				out << endl;
				needEndl = false;
			}
			if ( outputSize > 0 ) {
				out.write( ptr, outputSize );
				offset = outputSize;
				needEndl = true;
			}
		} else {
			out.write( ptr, outputSize );
		}
	}
	if ( wrap_ && needEndl ) {
		out << endl;
	}
	return;
	M_EPILOG
}

void base64::decode( yaal::hcore::HStreamInterface& in, yaal::hcore::HStreamInterface& out, bool standardCompliantMode ) {
	M_PROLOG
	int const BASE64LINELEN = 76;
	int const BUF_LEN = 80;
	char outputBuffer[BUF_LEN];
	HString line( BUF_LEN, 0_ycp );
	HChunk buffer( BASE64LINELEN + 1 );
	char* inputBuffer( buffer.get<char>() );
	int inputSize( 0 );
	int long pos( 0 );
	while ( in.read_until_n( line, BUF_LEN ) ) {
		for ( code_point_t ch : line ) {
			M_ENSURE( is_base64_character( ch, standardCompliantMode ) || is_whitespace( ch ), format( "char: %C, at position: %ld", ch, pos ) );
			if ( is_base64_character( ch, standardCompliantMode	) ) {
				inputBuffer[inputSize++] = static_cast<char>( ch.get() );
			}
			if ( inputSize >= BASE64LINELEN ) {
				M_ASSERT( inputSize == BASE64LINELEN );
				int long outputSize( base64_raw_decode( inputBuffer, inputSize, outputBuffer, sizeof ( outputBuffer ), standardCompliantMode ) );
				out.write( outputBuffer, outputSize );
				inputSize = 0;
			}
			++ pos;
		}
	}
	if ( inputSize > 0 ) {
		int long outputSize( base64_raw_decode( inputBuffer, inputSize, outputBuffer, sizeof ( outputBuffer ), standardCompliantMode ) );
		out.write( outputBuffer, outputSize );
	}
	return;
	M_EPILOG
}

}

}


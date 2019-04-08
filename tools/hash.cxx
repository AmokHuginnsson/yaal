/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"
#include "hash.hxx"
#include "hstreamblockiterator.hxx"
#include "hbitmap.hxx"
#include "hstringstream.hxx"
#include "hmemory.hxx"

using namespace std;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace hash {

void change_endianness( u32_t*, int long );
void update_md5_state( u32_t*, HStreamBlockIterator::HBlock const& );

md5_hash_t md5( HStreamInterface& stream ) {
	M_PROLOG
	static u32_t const BLOCK_SIZE = 512;
	static u32_t const MESSAGE_LENGTH_SIZE( BLOCK_SIZE >> 3 );
	static u32_t const SUPPLEMENT_SIZE = 1;
	static u32_t const STATE_SIZE = 4;
	static u32_t const STATE0 = 0x67452301;
	static u32_t const STATE1 = 0xefcdab89;
	static u32_t const STATE2 = 0x98badcfe;
	static u32_t const STATE3 = 0x10325476;
	HStreamBlockIterator source( stream, MESSAGE_LENGTH_SIZE );
	u32_t last( 0 );
	u32_t total( 0 );
	u32_t totalH( 0 );
	u32_t state[ STATE_SIZE ] = { STATE0, STATE1, STATE2, STATE3 };
	do {
		HStreamBlockIterator::HBlock block = *source;
		++ source;
		last = static_cast<u32_t>( block.octets() << 3 );
		if ( ( total + last ) < total ) {
			++ totalH;
		}
		total += last;
		if ( last < BLOCK_SIZE ) {
			u32_t* x = static_cast<u32_t*>( block.data() );
			HBitmap bmp;
			bmp.use( block.data(), BLOCK_SIZE );
			bmp.fill( static_cast<int long>( last ), static_cast<int long>( BLOCK_SIZE - last ), false );
			bmp.set( static_cast<int long>( last ), true );
			if ( last >= ( ( BLOCK_SIZE - MESSAGE_LENGTH_SIZE ) - SUPPLEMENT_SIZE ) ) {
				update_md5_state( state, block );
				yaal::fill( x, x + ( BLOCK_SIZE >> 5 ), 0u );
			}
			x[ 14 ] = total;
			x[ 15 ] = totalH;
			update_md5_state( state, block );
		} else {
			update_md5_state( state, block );
		}
	} while ( last == BLOCK_SIZE );
	md5_hash_t result;
	memcpy( result.data(), state, static_cast<size_t>( result.size() ) );
	return ( result );
	M_EPILOG
}

md5_hash_t md5( HStreamInterface::ptr_t stream ) {
	M_PROLOG
	return ( md5( *stream ) );
	M_EPILOG
}

md5_hash_t md5( HString const& string ) {
	M_PROLOG
	HStringStream ss( string );
	return ( md5( ss ) );
	M_EPILOG
}

void update_sha1_state( u32_t*, HStreamBlockIterator::HBlock const& );

sha1_hash_t sha1( HStreamInterface& stream ) {
	M_PROLOG
	static u32_t const BLOCK_SIZE = 512;
	static u32_t const MESSAGE_LENGTH_SIZE( BLOCK_SIZE >> 3 );
	static u32_t const SUPPLEMENT_SIZE = 1;
	static u32_t const STATE_SIZE = 5;
	static u32_t const STATE0 = 0x67452301;
	static u32_t const STATE1 = 0xefcdab89;
	static u32_t const STATE2 = 0x98badcfe;
	static u32_t const STATE3 = 0x10325476;
	static u32_t const STATE4 = 0xc3d2e1f0;
	HStreamBlockIterator source( stream, MESSAGE_LENGTH_SIZE );
	u32_t last( 0 );
	u32_t total( 0 );
	u32_t totalH( 0 );
	u32_t state[ STATE_SIZE ] = { STATE0, STATE1, STATE2, STATE3, STATE4 };
	do {
		HStreamBlockIterator::HBlock block = *source;
		++ source;
		last = static_cast<u32_t>( block.octets() << 3 );
		if ( ( total + last ) < total ) {
			++ totalH;
		}
		total += last;
		if ( last < BLOCK_SIZE ) {
			u32_t* x = static_cast<u32_t*>( block.data() );
			HBitmap bmp;
			bmp.use( block.data(),BLOCK_SIZE );
			bmp.fill( static_cast<int long>( last ), static_cast<int long>( BLOCK_SIZE - last ), false );
			bmp.set( static_cast<int long>( last ), true );
			if ( last >= ( ( BLOCK_SIZE - MESSAGE_LENGTH_SIZE ) - SUPPLEMENT_SIZE ) ) {
				update_sha1_state( state, block );
				yaal::fill( x, x + ( BLOCK_SIZE >> 5 ), 0u );
			}
			x[ 14 ] = totalH;
			x[ 15 ] = total;
			change_endianness( x + 14, 2 );
			update_sha1_state( state, block );
		} else {
			update_sha1_state( state, block );
		}
	} while ( last == BLOCK_SIZE );
	change_endianness( state, STATE_SIZE );
	sha1_hash_t result;
	memcpy( result.data(), state, static_cast<size_t>( result.size() ) );
	return ( result );
	M_EPILOG
}

sha1_hash_t sha1( HStreamInterface::ptr_t stream ) {
	M_PROLOG
	return ( sha1( *stream ) );
	M_EPILOG
}

sha1_hash_t sha1( HString const& string ) {
	M_PROLOG
	HStringStream ss( string );
	return ( sha1( ss ) );
	M_EPILOG
}

void update_md5_state( u32_t* state, HStreamBlockIterator::HBlock const& block ) {
#define M_F( x, y, z ) ( ( ( x ) & ( y ) ) | ( ~( x ) & ( z ) ) )
#define M_G( x, y, z ) ( ( ( x ) & ( z ) ) | ( ( y ) & ~( z ) ) )
#define M_H( x, y, z ) ( ( x ) ^ ( y ) ^ ( z ) )
#define M_I( x, y, z ) ( ( y ) ^ ( ( x ) | ~( z ) ) )

#define M_FF( v, w, x, y, z, s, ac ) { \
 v += M_F( ( w ), ( x ), ( y ) ) + ( z ) + static_cast<u32_t>( ac ); \
 v = M_ROTATE_LEFT( v, s ) + w; \
}
#define M_GG( v, w, x, y, z, s, ac ) { \
 v += M_G( ( w ), ( x ), ( y ) ) + ( z ) + static_cast<u32_t>( ac ); \
 v = M_ROTATE_LEFT( ( v ), ( s ) ) + ( w ); \
}
#define M_HH( v, w, x, y, z, s, ac ) { \
 v += M_H( ( w ), ( x ), ( y ) ) + ( z ) + ( ac ); \
 v = M_ROTATE_LEFT( ( v ), ( s ) ) + ( w ); \
}
#define M_II( v, w, x, y, z, s, ac ) { \
 v += M_I( ( w ), ( x ), ( y ) ) + ( z ) + ( ac ); \
 v = M_ROTATE_LEFT( ( v ), ( s ) ) + ( w ); \
}

#define M_ROTATE_LEFT( x, s ) ( ( ( x ) << ( s ) ) | ( ( x ) >> ( 32 - ( s ) ) ) )

	static int const S11 = 7;
	static int const S12 = 12;
	static int const S13 = 17;
	static int const S14 = 22;
	static int const S21 = 5;
	static int const S22 = 9;
	static int const S23 = 14;
	static int const S24 = 20;
	static int const S31 = 4;
	static int const S32 = 11;
	static int const S33 = 16;
	static int const S34 = 23;
	static int const S41 = 6;
	static int const S42 = 10;
	static int const S43 = 15;
	static int const S44 = 21;
	u32_t a = state[ 0 ];
	u32_t b = state[ 1 ];
	u32_t c = state[ 2 ];
	u32_t d = state[ 3 ];
	u32_t* x = reinterpret_cast<u32_t*>( block.data() );
	/* Cycle 1 */
	M_FF( a, b, c, d, x[ 0], S11, 0xd76aa478 ); /* 1 */
	M_FF( d, a, b, c, x[ 1], S12, 0xe8c7b756 ); /* 2 */
	M_FF( c, d, a, b, x[ 2], S13, 0x242070db ); /* 3 */
	M_FF( b, c, d, a, x[ 3], S14, 0xc1bdceee ); /* 4 */
	M_FF( a, b, c, d, x[ 4], S11, 0xf57c0faf ); /* 5 */
	M_FF( d, a, b, c, x[ 5], S12, 0x4787c62a ); /* 6 */
	M_FF( c, d, a, b, x[ 6], S13, 0xa8304613 ); /* 7 */
	M_FF( b, c, d, a, x[ 7], S14, 0xfd469501 ); /* 8 */
	M_FF( a, b, c, d, x[ 8], S11, 0x698098d8 ); /* 9 */
	M_FF( d, a, b, c, x[ 9], S12, 0x8b44f7af ); /* 10 */
	M_FF( c, d, a, b, x[10], S13, 0xffff5bb1 ); /* 11 */
	M_FF( b, c, d, a, x[11], S14, 0x895cd7be ); /* 12 */
	M_FF( a, b, c, d, x[12], S11, 0x6b901122 ); /* 13 */
	M_FF( d, a, b, c, x[13], S12, 0xfd987193 ); /* 14 */
	M_FF( c, d, a, b, x[14], S13, 0xa679438e ); /* 15 */
	M_FF( b, c, d, a, x[15], S14, 0x49b40821 ); /* 16 */

	/* Cycle 2 */
	M_GG( a, b, c, d, x[ 1], S21, 0xf61e2562 ); /* 17 */
	M_GG( d, a, b, c, x[ 6], S22, 0xc040b340 ); /* 18 */
	M_GG( c, d, a, b, x[11], S23, 0x265e5a51 ); /* 19 */
	M_GG( b, c, d, a, x[ 0], S24, 0xe9b6c7aa ); /* 20 */
	M_GG( a, b, c, d, x[ 5], S21, 0xd62f105d ); /* 21 */
	M_GG( d, a, b, c, x[10], S22,	0x02441453 ); /* 22 */
	M_GG( c, d, a, b, x[15], S23, 0xd8a1e681 ); /* 23 */
	M_GG( b, c, d, a, x[ 4], S24, 0xe7d3fbc8 ); /* 24 */
	M_GG( a, b, c, d, x[ 9], S21, 0x21e1cde6 ); /* 25 */
	M_GG( d, a, b, c, x[14], S22, 0xc33707d6 ); /* 26 */
	M_GG( c, d, a, b, x[ 3], S23, 0xf4d50d87 ); /* 27 */
	M_GG( b, c, d, a, x[ 8], S24, 0x455a14ed ); /* 28 */
	M_GG( a, b, c, d, x[13], S21, 0xa9e3e905 ); /* 29 */
	M_GG( d, a, b, c, x[ 2], S22, 0xfcefa3f8 ); /* 30 */
	M_GG( c, d, a, b, x[ 7], S23, 0x676f02d9 ); /* 31 */
	M_GG( b, c, d, a, x[12], S24, 0x8d2a4c8a ); /* 32 */

	/* Cycle 3 */
	M_HH( a, b, c, d, x[ 5], S31, 0xfffa3942 ); /* 33 */
	M_HH( d, a, b, c, x[ 8], S32, 0x8771f681 ); /* 34 */
	M_HH( c, d, a, b, x[11], S33, 0x6d9d6122 ); /* 35 */
	M_HH( b, c, d, a, x[14], S34, 0xfde5380c ); /* 36 */
	M_HH( a, b, c, d, x[ 1], S31, 0xa4beea44 ); /* 37 */
	M_HH( d, a, b, c, x[ 4], S32, 0x4bdecfa9 ); /* 38 */
	M_HH( c, d, a, b, x[ 7], S33, 0xf6bb4b60 ); /* 39 */
	M_HH( b, c, d, a, x[10], S34, 0xbebfbc70 ); /* 40 */
	M_HH( a, b, c, d, x[13], S31, 0x289b7ec6 ); /* 41 */
	M_HH( d, a, b, c, x[ 0], S32, 0xeaa127fa ); /* 42 */
	M_HH( c, d, a, b, x[ 3], S33, 0xd4ef3085 ); /* 43 */
	M_HH( b, c, d, a, x[ 6], S34, 0x04881d05 ); /* 44 */
	M_HH( a, b, c, d, x[ 9], S31, 0xd9d4d039 ); /* 45 */
	M_HH( d, a, b, c, x[12], S32, 0xe6db99e5 ); /* 46 */
	M_HH( c, d, a, b, x[15], S33, 0x1fa27cf8 ); /* 47 */
	M_HH( b, c, d, a, x[ 2], S34, 0xc4ac5665 ); /* 48 */

	/* Cycle 4 */
	M_II( a, b, c, d, x[ 0], S41, 0xf4292244 ); /* 49 */
	M_II( d, a, b, c, x[ 7], S42, 0x432aff97 ); /* 50 */
	M_II( c, d, a, b, x[14], S43, 0xab9423a7 ); /* 51 */
	M_II( b, c, d, a, x[ 5], S44, 0xfc93a039 ); /* 52 */
	M_II( a, b, c, d, x[12], S41, 0x655b59c3 ); /* 53 */
	M_II( d, a, b, c, x[ 3], S42, 0x8f0ccc92 ); /* 54 */
	M_II( c, d, a, b, x[10], S43, 0xffeff47d ); /* 55 */
	M_II( b, c, d, a, x[ 1], S44, 0x85845dd1 ); /* 56 */
	M_II( a, b, c, d, x[ 8], S41, 0x6fa87e4f ); /* 57 */
	M_II( d, a, b, c, x[15], S42, 0xfe2ce6e0 ); /* 58 */
	M_II( c, d, a, b, x[ 6], S43, 0xa3014314 ); /* 59 */
	M_II( b, c, d, a, x[13], S44, 0x4e0811a1 ); /* 60 */
	M_II( a, b, c, d, x[ 4], S41, 0xf7537e82 ); /* 61 */
	M_II( d, a, b, c, x[11], S42, 0xbd3af235 ); /* 62 */
	M_II( c, d, a, b, x[ 2], S43, 0x2ad7d2bb ); /* 63 */
	M_II( b, c, d, a, x[ 9], S44, 0xeb86d391 ); /* 64 */

	state[ 0 ] += a;
	state[ 1 ] += b;
	state[ 2 ] += c;
	state[ 3 ] += d;

	return;

#undef M_F
#undef M_G
#undef M_H
#undef M_I
#undef M_FF
#undef M_GG
#undef M_HH
#undef M_II
#undef M_ROTATE_LEFT
}

void update_sha1_state( u32_t* state, HStreamBlockIterator::HBlock const& block ) {
#define M_ROTATE_LEFT( x, s ) ( ( ( x ) << ( s ) ) | ( ( x ) >> ( 32 - ( s ) ) ) )
	static int const WORK_BUFFER_SIZE = 80;
	static int const INPUT_CHUNK_SIZE = 16;
	u32_t a = state[ 0 ];
	u32_t b = state[ 1 ];
	u32_t c = state[ 2 ];
	u32_t d = state[ 3 ];
	u32_t e = state[ 4 ];
	u32_t x[ WORK_BUFFER_SIZE ];
	::memcpy( x, block.data(), INPUT_CHUNK_SIZE * sizeof ( u32_t ) );
	u32_t tmp( 0 );
	change_endianness( x, INPUT_CHUNK_SIZE );
	for ( int i = INPUT_CHUNK_SIZE; i < WORK_BUFFER_SIZE; ++ i ) {
		tmp = x[ i - 3 ] ^ x[ i - 8 ] ^ x[ i - 14 ] ^ x[ i - 16 ], x[ i ] = M_ROTATE_LEFT( tmp, 1 );
	}
  for ( int i = 0; i < WORK_BUFFER_SIZE; ++ i ) {
		u32_t f( 0 );
		u32_t k( 0 );
		if ( ( 0 <= i ) && ( i <= 19 ) ) {
			f = ( b & c ) | ( ( ~ b ) & d );
			k = 0x5a827999;
		} else if ( ( 20 <= i ) && ( i <= 39 ) ) {
			f = b ^ c ^ d;
			k = 0x6ed9eba1;
		} else if ( ( 40 <= i ) && ( i <= 59 ) ) {
			f = ( b & c ) | ( b & d ) | ( c & d );
			k = 0x8f1bbcdc;
		} else {
			f = b ^ c ^ d;
			k = 0xca62c1d6;
		}

		tmp = M_ROTATE_LEFT( a, 5 ) + f + e + k + x[ i ];
		e = d;
		d = c;
		c = M_ROTATE_LEFT( b, 30 );
		b = a;
		a = tmp;
	}
	state[ 0 ] += a;
	state[ 1 ] += b;
	state[ 2 ] += c;
	state[ 3 ] += d;
	state[ 4 ] += e;
	return;
#undef M_ROTATE_LEFT
}

void change_endianness( u32_t* mem, int long size ) {
	while ( size -- )
		mem[ size ] =
			  ( ( mem[ size ] & 0xff000000 ) >> 24 )
			| ( ( mem[ size ] & 0x00ff0000 ) >> 8 )
			| ( ( mem[ size ] & 0x0000ff00 ) << 8 )
			| ( ( mem[ size ] & 0x000000ff ) << 24 );
}

void change_endianness( u64_t*, int long );

void update_sha512_state( u64_t*, HStreamBlockIterator::HBlock const& );

sha512_hash_t sha512( HStreamInterface& stream ) {
	M_PROLOG
	static u32_t const BLOCK_SIZE = 1024;
	static u32_t const MESSAGE_LENGTH_SIZE( BLOCK_SIZE >> 3 );
	static u32_t const SUPPLEMENT_SIZE = 1;
	static u32_t const STATE_SIZE( 8 );
	HStreamBlockIterator source( stream, MESSAGE_LENGTH_SIZE );
	u32_t last( 0 );
	u32_t total( 0 );
	u32_t totalH( 0 );
	u64_t state[ STATE_SIZE ] = {
		0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
		0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
	};

	do {
		HStreamBlockIterator::HBlock block = *source;
		++ source;
		last = static_cast<u32_t>( block.octets() << 3 );
		if ( ( total + last ) < total ) {
			++ totalH;
		}
		total += last;
		if ( last < BLOCK_SIZE ) {
			u64_t* x = static_cast<u64_t*>( block.data() );
			HBitmap bmp;
			bmp.use( block.data(),BLOCK_SIZE );
			bmp.fill( static_cast<int long>( last ), static_cast<int long>( BLOCK_SIZE - last ), false );
			bmp.set( static_cast<int long>( last ), true );
			if ( last >= ( ( BLOCK_SIZE - MESSAGE_LENGTH_SIZE ) - SUPPLEMENT_SIZE ) ) {
				update_sha512_state( state, block );
				yaal::fill( x, x + ( BLOCK_SIZE >> 5 ), 0u );
			}
			x[ 14 ] = totalH;
			x[ 15 ] = total;
			change_endianness( x + 14, 2 );
			update_sha512_state( state, block );
		} else {
			update_sha512_state( state, block );
		}
	} while ( last == BLOCK_SIZE );
	change_endianness( state, STATE_SIZE );
	sha512_hash_t result;
	memcpy( result.data(), state, static_cast<size_t>( result.size() ) );
	return ( result );
	M_EPILOG
}

sha512_hash_t sha512( HStreamInterface::ptr_t stream ) {
	M_PROLOG
	return ( sha512( *stream ) );
	M_EPILOG
}

sha512_hash_t sha512( HString const& string ) {
	M_PROLOG
	HStringStream ss( string );
	return ( sha512( ss ) );
	M_EPILOG
}

void update_sha512_state( u64_t* state, HStreamBlockIterator::HBlock const& block ) {
	static u64_t const roundConst[] = {
		0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
		0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
		0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
		0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
		0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
		0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
		0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
		0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
		0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
		0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
		0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
		0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
		0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
		0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
		0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
		0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
	};
	static int const ROUND_COUNT( yaal::size( roundConst ) );
#define M_ROTATE_RIGHT64( x, s ) ( ( ( x ) >> ( s ) ) | ( ( x ) << ( 64 - ( s ) ) ) )
	static int const INPUT_CHUNK_SIZE( 16 );
	u64_t a( state[ 0 ] );
	u64_t b( state[ 1 ] );
	u64_t c( state[ 2 ] );
	u64_t d( state[ 3 ] );
	u64_t e( state[ 4 ] );
	u64_t f( state[ 5 ] );
	u64_t g( state[ 6 ] );
	u64_t h( state[ 7 ] );
	u64_t x[ ROUND_COUNT ];
	::memcpy( x, block.data(), INPUT_CHUNK_SIZE * sizeof ( u64_t ) );
	change_endianness( x, INPUT_CHUNK_SIZE );
	for ( int i( INPUT_CHUNK_SIZE ); i < ROUND_COUNT; ++ i ) {
		u64_t s0 = M_ROTATE_RIGHT64( x[i - 15 ], 1 ) ^ M_ROTATE_RIGHT64( x[i - 15], 8 ) ^ ( x[i - 15] >> 7 );
		u64_t s1 = M_ROTATE_RIGHT64( x[i - 2], 19 ) ^ M_ROTATE_RIGHT64( x[i - 2], 61 ) ^ ( x[i - 2] >> 6 );
		x[i] = x[i - 16] + s0 + x[i - 7] + s1;
	}

	for ( int i( 0 ); i < ROUND_COUNT; ++ i ) {
		u64_t S0( M_ROTATE_RIGHT64( a, 28 ) ^ M_ROTATE_RIGHT64( a, 34 ) ^ M_ROTATE_RIGHT64( a, 39 ) );
		u64_t S1( M_ROTATE_RIGHT64( e, 14 ) ^ M_ROTATE_RIGHT64( e, 18 ) ^ M_ROTATE_RIGHT64( e, 41 ) );
		u64_t ch( ( e & f ) ^ ( ( ~e ) & g ) );
		u64_t temp1( h + S1 + ch + roundConst[i] + x[i] );
		u64_t maj( ( a & b ) ^ ( a & c ) ^ ( b & c ) );
		u64_t temp2( S0 + maj );
		h = g;
		g = f;
		f = e;
		e = d + temp1;
		d = c;
		c = b;
		b = a;
		a = temp1 + temp2;
	}

	state[ 0 ] += a;
	state[ 1 ] += b;
	state[ 2 ] += c;
	state[ 3 ] += d;
	state[ 4 ] += e;
	state[ 5 ] += f;
	state[ 6 ] += g;
	state[ 7 ] += h;
	return;
#undef M_ROTATE_RIGHT64
}

template<typename hash_t>
inline yaal::hcore::HString hash_to_string( hash_t const& hash_ ) {
	HString s;
	s.reserve( 2 * hash_.size() );
	static int const BUF_SIZE( 3 );
	char buf[trait::to_unsigned<int, BUF_SIZE>::value];
	for ( u8_t o : hash_ ) {
		snprintf( buf, BUF_SIZE, "%02x", o );
		s.append( buf );
	}
	return ( s );
}

yaal::hcore::HString to_string( md5_hash_t const& hash_ ) {
	return ( hash_to_string( hash_ ) );
}

yaal::hcore::HString to_string( sha1_hash_t const& hash_ ) {
	return ( hash_to_string( hash_ ) );
}

yaal::hcore::HString to_string( sha512_hash_t const& hash_ ) {
	return ( hash_to_string( hash_ ) );
}

void change_endianness( u64_t* mem, int long size ) {
	while ( size -- )
		mem[ size ] =
			  ( ( mem[ size ] & 0xff00000000000000ULL ) >> 56 )
			| ( ( mem[ size ] & 0x00ff000000000000ULL ) >> 40 )
			| ( ( mem[ size ] & 0x0000ff0000000000ULL ) >> 24 )
			| ( ( mem[ size ] & 0x000000ff00000000ULL ) >> 8 )
			| ( ( mem[ size ] & 0x00000000ff000000ULL ) << 8 )
			| ( ( mem[ size ] & 0x0000000000ff0000ULL ) << 24 )
			| ( ( mem[ size ] & 0x000000000000ff00ULL ) << 40 )
			| ( ( mem[ size ] & 0x00000000000000ffULL ) << 56 );
}

namespace {
void memxor( void* dest_, void const* src_, int long size_ ) {
	char* dst( static_cast<char*>( dest_ ) );
	char const* src( static_cast<char const*>( src_ ) );
	for ( int long i( 0 ); i < size_; ++ i ) {
		dst[i] ^= src[i];
	}
	return;
}
template<typename hash_t>
struct hash_function;

template<>
struct hash_function<md5_hash_t> {
	static int const BLOCK_SIZE = 512;
	md5_hash_t operator()( yaal::hcore::HString const& str_ ) {
		return ( md5( str_ ) );
	}
	md5_hash_t operator()( HMemory& mem_ ) {
		return ( md5( mem_ ) );
	}
};
template<>
struct hash_function<sha1_hash_t> {
	static int const BLOCK_SIZE = 512;
	sha1_hash_t operator()( yaal::hcore::HString const& str_ ) {
		return ( sha1( str_ ) );
	}
	sha1_hash_t operator()( HMemory& mem_ ) {
		return ( sha1( mem_ ) );
	}
};
template<>
struct hash_function<sha512_hash_t> {
	static int const BLOCK_SIZE = 1024;
	sha512_hash_t operator()( yaal::hcore::HString const& str_ ) {
		return ( sha512( str_ ) );
	}
	sha512_hash_t operator()( HMemory& mem_ ) {
		return ( sha512( mem_ ) );
	}
};
}

template<typename hash_t>
hash_t hmac( yaal::hcore::HString const& key_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	hash_function<hash_t> f;
	int const hashLen( sizeof ( hash_t ) );
	int blockSize( hash_function<hash_t>::BLOCK_SIZE );
	int blockBytes( blockSize >> 3 );
	HUTF8String key( key_ );
	HUTF8String message( message_ );
	HChunk ipad( blockBytes + message.byte_count() );
	HChunk opad( blockBytes + hashLen );
	::memset( ipad.raw(), 0x36, static_cast<size_t>( blockBytes ) );
	::memset( opad.raw(), 0x5c, static_cast<size_t>( blockBytes ) );
	if ( key_.get_length() <= blockBytes ) {
		memxor( ipad.raw(), key.c_str(), key.byte_count() );
		memxor( opad.raw(), key.c_str(), key.byte_count() );
	} else {
		hash_t keyBinHashed( f( key_ ) );
		memxor( ipad.raw(), keyBinHashed.data(), keyBinHashed.size() );
		memxor( opad.raw(), keyBinHashed.data(), keyBinHashed.size() );
	}
	::memcpy( ipad.get<char>() + blockBytes, message.c_str(), static_cast<size_t>( message.byte_count() ) );
	HMemory m( make_resource<HMemoryObserver>( ipad.raw(), blockBytes + message.byte_count() ) );
	hash_t ipadHashed( f( m ) );
	::memcpy( opad.get<char>() + blockBytes, ipadHashed.data(), static_cast<size_t>( hashLen ) );
	HMemory mout( make_resource<HMemoryObserver>( opad.raw(), blockBytes + hashLen ) );
	return ( f( mout ) );
	M_EPILOG
}

md5_hash_t hmac_md5( yaal::hcore::HString const& key_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	return ( hmac<md5_hash_t>( key_, message_ ) );
	M_EPILOG
}

sha1_hash_t hmac_sha1( yaal::hcore::HString const& key_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	return ( hmac<sha1_hash_t>( key_, message_ ) );
	M_EPILOG
}

sha512_hash_t hmac_sha512( yaal::hcore::HString const& key_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	return ( hmac<sha512_hash_t>( key_, message_ ) );
	M_EPILOG
}

yaal::hcore::HString hmac( FUNCTION function_, yaal::hcore::HString const& key_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	HString s;
	switch ( function_ ) {
		case ( FUNCTION::MD5 ): s = to_string( hmac_md5( key_, message_ ) ); break;
		case ( FUNCTION::SHA1 ): s = to_string( hmac_sha1( key_, message_ ) ); break;
		case ( FUNCTION::SHA512 ): s = to_string( hmac_sha512( key_, message_ ) ); break;
	}
	return ( s );
	M_EPILOG
}

yaal::hcore::HString string( FUNCTION function_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	HString s;
	switch ( function_ ) {
		case ( FUNCTION::MD5 ): s = to_string( md5( message_ ) ); break;
		case ( FUNCTION::SHA1 ): s = to_string( sha1( message_ ) ); break;
		case ( FUNCTION::SHA512 ): s = to_string( sha512( message_ ) ); break;
	}
	return ( s );
	M_EPILOG
}

yaal::hcore::HString string( FUNCTION function_, yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	HString s;
	switch ( function_ ) {
		case ( FUNCTION::MD5 ): s = to_string( md5( stream_ ) ); break;
		case ( FUNCTION::SHA1 ): s = to_string( sha1( stream_ ) ); break;
		case ( FUNCTION::SHA512 ): s = to_string( sha512( stream_ ) ); break;
	}
	return ( s );
	M_EPILOG
}

}

}

}


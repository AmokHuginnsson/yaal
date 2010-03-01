/*
---	         yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hash.cxx - this file is integral part of yaal project.

	i.	You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
		  of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
	1. You cannot use any part of sources of this software.
	2. You cannot redistribute any part of sources of this software.
	3. No reverse engineering is allowed.
	4. If you want redistribute binary package you cannot demand any fees
	   for this software.
	   You cannot even demand cost of the carrier (CD for example).
	5. You cannot include it to any commercial enterprise (for example 
	   as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/algorithm.hxx"
#include "hcore/pod.hxx"
#include "hash.hxx"
#include "hstreamblockiterator.hxx"
#include "hbitmap.hxx"

using namespace std;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace hash
{

void change_endianess( u32_t*, int long );
void update_md5_state( u32_t*, HStreamBlockIterator::HBlock const& );

yaal::hcore::HString md5( HStreamInterface& stream )
	{
	M_PROLOG
	static int const BLOCK_SIZE = 512;
	static int const MESSAGE_LENGTH_SIZE = 64;
	static int const SUPPLEMENT_SIZE = 1;
	static int const STATE_SIZE = 4;
	static u32_t const STATE0 = 0x67452301;
	static u32_t const STATE1 = 0xefcdab89;
	static u32_t const STATE2 = 0x98badcfe;
	static u32_t const STATE3 = 0x10325476;
	HStreamBlockIterator source( stream, BLOCK_SIZE >> 3 );
	int last = 0;
	u32_t total = 0;
	u32_t totalH = 0;
	u32_t state[ STATE_SIZE ] = { STATE0, STATE1, STATE2, STATE3 };
	do
		{
		HStreamBlockIterator::HBlock block = *source;
		++ source;
		last = static_cast<int>( block.octets() << 3 );
		if ( ( total + last ) < total )
			++ totalH;
		total += last;
		if ( last < BLOCK_SIZE )
			{
			u32_t* x = static_cast<u32_t*>( block.data() );
			HBitmap bmp;
			bmp.use( block.data(), BLOCK_SIZE );
			bmp.fill( last, BLOCK_SIZE - last, false );
			bmp.set( last, true );
			if ( last >= ( ( BLOCK_SIZE - MESSAGE_LENGTH_SIZE ) - SUPPLEMENT_SIZE ) )
				{
				update_md5_state( state, block );
				yaal::fill( x, x + ( BLOCK_SIZE >> 5 ), 0 );
				}
			x[ 14 ] = total;
			x[ 15 ] = totalH;
			update_md5_state( state, block );
			}
		else
			update_md5_state( state, block );
		}
	while ( last == BLOCK_SIZE );
	HString result;
	change_endianess( state, STATE_SIZE );
	result.format( "%08x%08x%08x%08x", state[ 0 ], state[ 1 ], state[ 2 ], state[ 3 ] );
	return ( result );
	M_EPILOG
	}

yaal::hcore::HString md5( HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	return ( md5( *stream ) );
	M_EPILOG
	}

void update_sha1_state( u32_t*, HStreamBlockIterator::HBlock const& );

yaal::hcore::HString sha1( HStreamInterface& stream )
	{
	M_PROLOG
	static int const BLOCK_SIZE = 512;
	static int const MESSAGE_LENGTH_SIZE = 64;
	static int const SUPPLEMENT_SIZE = 1;
	static int const STATE_SIZE = 5;
	static u32_t const STATE0 = 0x67452301;
	static u32_t const STATE1 = 0xefcdab89;
	static u32_t const STATE2 = 0x98badcfe;
	static u32_t const STATE3 = 0x10325476;
	static u32_t const STATE4 = 0xc3d2e1f0;
	HStreamBlockIterator source( stream, BLOCK_SIZE >> 3 );
	int last = 0;
	u32_t total = 0;
	u32_t totalH = 0;
	u32_t state[ STATE_SIZE ] = { STATE0, STATE1, STATE2, STATE3, STATE4 };
	do
		{
		HStreamBlockIterator::HBlock block = *source;
		++ source;
		last = static_cast<int>( block.octets() << 3 );
		if ( ( total + last ) < total )
			++ totalH;
		total += last;
		if ( last < BLOCK_SIZE )
			{
			u32_t* x = static_cast<u32_t*>( block.data() );
			HBitmap bmp;
			bmp.use( block.data(),BLOCK_SIZE );
			bmp.fill( last, BLOCK_SIZE - last, false );
			bmp.set( last, true );
			if ( last >= ( ( BLOCK_SIZE - MESSAGE_LENGTH_SIZE ) - SUPPLEMENT_SIZE ) )
				{
				update_sha1_state( state, block );
				yaal::fill( x, x + ( BLOCK_SIZE >> 5 ), 0 );
				}
			x[ 14 ] = totalH;
			x[ 15 ] = total;
			change_endianess( x + 14, 2 );
			update_sha1_state( state, block );
			}
		else
			update_sha1_state( state, block );
		}
	while ( last == BLOCK_SIZE );
	HString result;
	result.format( "%08x%08x%08x%08x%08x", state[ 0 ], state[ 1 ], state[ 2 ], state[ 3 ], state[ 4 ] );
	return ( result );
	M_EPILOG
	}

yaal::hcore::HString sha1( HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	return ( sha1( *stream ) );
	M_EPILOG
	}

void update_md5_state( u32_t* state, HStreamBlockIterator::HBlock const& block )
	{
#define M_F(x, y, z) (((x)&(y)) | (~(x)&(z)))
#define M_G(x, y, z) (((x)&(z)) | ((y)&~(z)))
#define M_H(x, y, z) ((x)^(y)^(z))
#define M_I(x, y, z) ((y)^((x)|~(z)))

#define M_FF(v, w, x, y, z, s, ac) { \
 v += M_F(w, x, y) + z + (u32_t)(ac); \
 v = M_ROTATE_LEFT(v, s) + w; \
 }
#define M_GG(v, w, x, y, z, s, ac) { \
 v += M_G(w, x, y) + z + (u32_t)(ac); \
 v = M_ROTATE_LEFT(v, s) + w; \
 }
#define M_HH(v, w, x, y, z, s, ac) { \
 v += M_H(w, x, y) + z + ac; \
 v = M_ROTATE_LEFT(v, s) + w; \
 }
#define M_II(v, w, x, y, z, s, ac) { \
 v += M_I(w, x, y) + z + ac; \
 v = M_ROTATE_LEFT(v, s) + w; \
 }

#define M_ROTATE_LEFT(x, s) ((x << s) | (x >> (32 - s)))

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
	M_FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	M_FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	M_FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	M_FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	M_FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	M_FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	M_FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	M_FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	M_FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	M_FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	M_FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	M_FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	M_FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	M_FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	M_FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	M_FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Cycle 2 */
	M_GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	M_GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	M_GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	M_GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	M_GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	M_GG(d, a, b, c, x[10], S22,	0x2441453); /* 22 */
	M_GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	M_GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	M_GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	M_GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	M_GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	M_GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	M_GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	M_GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	M_GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	M_GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Cycle 3 */
	M_HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	M_HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	M_HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	M_HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	M_HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	M_HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	M_HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	M_HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	M_HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	M_HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	M_HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	M_HH(b, c, d, a, x[ 6], S34,	0x4881d05); /* 44 */
	M_HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	M_HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	M_HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	M_HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Cycle 4 */
	M_II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	M_II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	M_II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	M_II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	M_II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	M_II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	M_II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	M_II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	M_II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	M_II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	M_II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	M_II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	M_II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	M_II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	M_II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	M_II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

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

void update_sha1_state( u32_t* state, HStreamBlockIterator::HBlock const& block )
	{
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
	u32_t tmp = 0;
	change_endianess( x, INPUT_CHUNK_SIZE );
	for ( int i = INPUT_CHUNK_SIZE; i < WORK_BUFFER_SIZE; ++ i )
		tmp = x[ i - 3 ] ^ x[ i - 8 ] ^ x[ i - 14 ] ^ x[ i - 16 ], x[ i ] = M_ROTATE_LEFT( tmp, 1 );
  for ( int i = 0; i < WORK_BUFFER_SIZE; ++ i )
		{
		u32_t f;
		u32_t k;
		if ( ( 0 <= i ) && ( i <= 19 ) )
			{
			f = ( b & c ) | ( ( ~ b ) & d );
			k = 0x5a827999;
			}
		else if ( ( 20 <= i ) && ( i <= 39 ) )
			{
			f = b ^ c ^ d;
			k = 0x6ed9eba1;
			}
		else if ( ( 40 <= i ) && ( i <= 59 ) )
			{
			f = ( b & c ) | ( b & d ) | ( c & d );
			k = 0x8f1bbcdc;
			}
		else
			{
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

void change_endianess( u32_t* mem, int long size )
	{
	while ( size -- )
		mem[ size ] =
			  ( ( mem[ size ] & 0xff000000 ) >> 24 )
			| ( ( mem[ size ] & 0x00ff0000 ) >> 8 )
			| ( ( mem[ size ] & 0x0000ff00 ) << 8 )
			| ( ( mem[ size ] & 0x000000ff ) << 24 );
	}

}

}

}


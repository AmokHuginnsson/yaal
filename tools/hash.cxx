/*
---	         hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hash.cxx - this file is integral part of hanalyser.h project.

	i.	You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
		  of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
	1. You can not use any part of sources of this software.
	2. You can not redistribute any part of sources of this software.
	3. No reverse engineering is allowed.
	4. If you want redistribute binary package you can not demand any fees
	   for this software.
	   You can not even demand cost of the carrier (CD for example).
	5. You can not include it to any commercial enterprise (for example 
	   as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <arpa/inet.h>

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hash.h"

using namespace std;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace hash
{

void update_md5_state( u32_t*, HBitmap const& );
void change_endianess( u32_t*, int long );

yaal::hcore::HString md5( HBitSourceInterface const& bitSource )
	{
	static int const D_BLOCK_SIZE = 512;
	static int const D_MESSAGE_LENGTH_SIZE = 64;
	static int const D_SUPPLEMENT_SIZE = 1;
	static int const D_STATE_SIZE = 4;
	static u32_t const D_S0 = 0x67452301;
	static u32_t const D_S1 = 0xefcdab89;
	static u32_t const D_S2 = 0x98badcfe;
	static u32_t const D_S3 = 0x10325476;
	int long block = 0;
	HBitmap bmp( D_BLOCK_SIZE );
	int last = 0;
	int long total = 0;
	u32_t state[ D_STATE_SIZE ] = { D_S0, D_S1, D_S2, D_S3 };
	do
		{
		bmp = bitSource.get_nth_block( block ++, D_BLOCK_SIZE );
		last = static_cast<int>( bmp.get_size() );
		total += last;
		if ( last < D_BLOCK_SIZE )
			{
			bmp.reserve( D_BLOCK_SIZE );
			bmp.fill( last, D_BLOCK_SIZE - last, false );
			u32_t* x = const_cast<u32_t*>( static_cast<u32_t const*>( bmp.raw() ) );
			bmp.set( last, true );
			if ( last >= ( ( D_BLOCK_SIZE - D_MESSAGE_LENGTH_SIZE ) - D_SUPPLEMENT_SIZE ) )
				{
				update_md5_state( state, bmp );
				bmp.fill( false );
				}
			x[ 14 ] = total;
			M_ASSERT( x[ 15 ] == 0 );
			update_md5_state( state, bmp );
			}
		else
			update_md5_state( state, bmp );
		}
	while ( last == D_BLOCK_SIZE );
	HString result;
	change_endianess( state, 4 );
	result.format( "%08x%08x%08x%08x", state[ 0 ], state[ 1 ], state[ 2 ], state[ 3 ] );
	return ( result );
	}

void update_md5_state( u32_t* state, HBitmap const& bmp )
	{
#define F(x, y, z) (((x)&(y)) | (~(x)&(z)))
#define G(x, y, z) (((x)&(z)) | ((y)&~(z)))
#define H(x, y, z) ((x)^(y)^(z))
#define I(x, y, z) ((y)^((x)|~(z)))

#define FF(v, w, x, y, z, s, ac) { \
 v += F(w, x, y) + z + (u32_t)(ac); \
 v = ROTATE_LEFT(v, s) + w; \
 }
#define GG(v, w, x, y, z, s, ac) { \
 v += G(w, x, y) + z + (u32_t)(ac); \
 v = ROTATE_LEFT(v, s) + w; \
 }
#define HH(v, w, x, y, z, s, ac) { \
 v += H(w, x, y) + z + ac; \
 v = ROTATE_LEFT(v, s) + w; \
 }
#define II(v, w, x, y, z, s, ac) { \
 v += I(w, x, y) + z + ac; \
 v = ROTATE_LEFT(v, s) + w; \
 }

#define ROTATE_LEFT(x, s) ((x << s) | (x >> (32 - s)))

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
	u32_t a = state[ 0 ];
	u32_t b = state[ 1 ];
	u32_t c = state[ 2 ];
	u32_t d = state[ 3 ];
	u32_t* x = const_cast<u32_t*>( static_cast<u32_t const*>( bmp.raw() ) );
/*	change_endianess( x, 16 ); */
	/* Cycle 1 */
	FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Cycle 2 */
	GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22,	0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Cycle 3 */
	HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], S34,	0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Cycle 4 */
	II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[ 0 ] += a;
	state[ 1 ] += b;
	state[ 2 ] += c;
	state[ 3 ] += d;

	return;

#undef F
#undef G
#undef H
#undef I
#undef FF
#undef GG
#undef HH
#undef II
#undef ROTATE_LEFT
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


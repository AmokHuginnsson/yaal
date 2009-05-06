/*
---				   `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdes.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hdes.hxx"
#include "hbitmap.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

typedef int long unsigned u32_t;
typedef int short unsigned u16_t;

/*! \brief The DES algorithm helper data tables.
 */
namespace des
	{
	
extern const u8_t n_pcBeginingPermutation[];
extern const u8_t n_pcEndingPermutation[];
extern const u8_t n_pcKeyPermutation[];
extern const u8_t n_pcCountOfMoves[];
extern const u8_t n_pcPermutationOfCompresion[];
extern const u8_t n_pcPermutationOfExpanding[];
extern const u8_t n_pcPBlockPermutation[];
extern const u8_t n_pppcSBlock[][ 4 ][ 16 ];

	}

using namespace des;

HDes::HDes ( void )
	{
	flush_keys();
	return;
	}

HDes::~HDes ( void )
	{
	M_PROLOG
	flush_keys();
	return;
	M_EPILOG
	}

void HDes::generate_keys( u8_t* a_pcPassword )
	{
	int l_iCtr = 0, l_iCtrLoc = 0;
	u8_t l_pcIKeyLow[ DES::BLOCK_SIZE ];
	u8_t l_pcIKeyHigh[ DES::BLOCK_SIZE ];
	u8_t l_pcTmpKey[ DES::BLOCK_SIZE ];
	flush_keys();
	for ( l_iCtr = 0; l_iCtr < DES::BLOCK_SIZE; ++ l_iCtr )
		{
		l_pcIKeyHigh[ l_iCtr ] = a_pcPassword[ l_iCtr ];
		l_pcIKeyLow[ l_iCtr ] = a_pcPassword[ l_iCtr + DES::BLOCK_SIZE ];
		}
	permutate( l_pcIKeyHigh, n_pcKeyPermutation, 56 );
	permutate( l_pcIKeyLow, n_pcKeyPermutation, 56 );
	HBitmap hi;
	HBitmap low;
	hi.use( l_pcIKeyHigh, 2 * DES::HALF_KEY_SIZE );
	low.use( l_pcIKeyHigh, 2 * DES::HALF_KEY_SIZE );
	for ( l_iCtr = 0; l_iCtr < DES::IKEYS_COUNT; l_iCtr ++ )
		{
		hi.rotate_left( 0, 28, n_pcCountOfMoves[ l_iCtr ] );
		low.rotate_left( 0, 28, n_pcCountOfMoves[ l_iCtr ] );
		hi.rotate_left( 28, 28, n_pcCountOfMoves[ l_iCtr ] );
		low.rotate_left( 28, 28, n_pcCountOfMoves[ l_iCtr ] );
		for ( l_iCtrLoc = 0; l_iCtrLoc < DES::BLOCK_SIZE; l_iCtrLoc ++ )
			l_pcTmpKey[ l_iCtrLoc ] = l_pcIKeyHigh[ l_iCtrLoc ];
		permutate( l_pcTmpKey, n_pcPermutationOfCompresion, 48 );
		for ( l_iCtrLoc = 0; l_iCtrLoc < 6; l_iCtrLoc ++ )
			f_pppcIKeys[ 0 ][ l_iCtr ][ l_iCtrLoc ] = l_pcTmpKey[ l_iCtrLoc ];
		for ( l_iCtrLoc = 0; l_iCtrLoc < DES::BLOCK_SIZE; l_iCtrLoc ++ )
			l_pcTmpKey[ l_iCtrLoc ] = l_pcIKeyLow[ l_iCtrLoc ];
		permutate( l_pcTmpKey, n_pcPermutationOfCompresion, 48 );
		for ( l_iCtrLoc = 0; l_iCtrLoc < 6; l_iCtrLoc ++ )
			f_pppcIKeys[ 1 ][ l_iCtr ][ l_iCtrLoc ] = l_pcTmpKey[ l_iCtrLoc ];
		}
	memset ( a_pcPassword, 0, DES::PASSWORD_SIZE );
	return ;
	}

void HDes::flush_keys ( void )
	{
	int l_iCtr = 0, l_iCtrLoc = 0;
	for ( l_iCtr = 0; l_iCtr < DES::SIDES_COUNT; l_iCtr ++ )
		for ( l_iCtrLoc = 0; l_iCtrLoc < DES::IKEYS_COUNT; l_iCtrLoc ++ )
			memset ( f_pppcIKeys[ l_iCtr ][ l_iCtrLoc ], 0, DES::IKEY_SIZE );
	return;
	}

void HDes::crypt ( u8_t* a_pcBuffer, int a_iBlockCount, int a_iSide )
	{
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < a_iBlockCount; l_iCtr ++ )
		_3des( a_pcBuffer + ( l_iCtr << 3 ), a_iSide );
	return;
	}
	
void HDes::_3des( u8_t* a_pcBlock, int a_iSide )
	{
	permutate( a_pcBlock, n_pcBeginingPermutation, 64 );
	_des ( a_pcBlock, a_iSide, 0 );
	_des ( a_pcBlock, 1 - a_iSide, 1 );
	_des ( a_pcBlock, a_iSide, 0 );
	permutate( a_pcBlock, n_pcEndingPermutation, 64 );
	return;
	}
	
void HDes::_des( u8_t* a_pcBlock, int a_iSide, int a_iPart )
	{
	int l_iCycle = 0, l_iCtr = 0, l_iCtrLoc, l_iCol, l_iRow;
	u8_t l_pcBuf[ DES::BLOCK_SIZE ], l_pcBufT[ DES::BLOCK_SIZE ];
	u8_t l_pcBufL[ DES::BLOCK_SIZE ], l_pcBufR[ DES::BLOCK_SIZE ];
	u8_t l_cMask = 0, * l_pcEndKey = NULL;
	reinterpret_cast<u32_t*>( l_pcBufL )[ 0 ] = reinterpret_cast<u32_t*>( a_pcBlock )[ 0 ];
	reinterpret_cast<u32_t*>( l_pcBufR )[ 0 ] = reinterpret_cast<u32_t*>( a_pcBlock )[ 1 ];
	reinterpret_cast<u32_t*>( l_pcBufT )[ 0 ] = reinterpret_cast<u32_t*>( a_pcBlock )[ 1 ];
	HBitmap src;
	HBitmap row;
	HBitmap col;
	src.use( l_pcBufT, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	row.use( &l_iRow, sizeof ( l_iRow ) * DES::BITS_IN_BYTE );
	col.use( &l_iCol, sizeof ( l_iCol ) * DES::BITS_IN_BYTE );
	for ( l_iCycle = 0; l_iCycle < DES::IKEYS_COUNT; l_iCycle ++ )
		{
		reinterpret_cast<u32_t*>( l_pcBuf )[ 0 ] = 0;
		if ( a_iSide )
			l_pcEndKey = f_pppcIKeys[ a_iPart ][ l_iCycle ];
		else
			l_pcEndKey = f_pppcIKeys[ a_iPart ][ 15 - l_iCycle ];
		permutate( l_pcBufT, n_pcPermutationOfExpanding, 48 );
		reinterpret_cast<u32_t*>( l_pcBufT )[ 0 ] = reinterpret_cast<u32_t*>( l_pcBufT )[ 0 ]
																											^ reinterpret_cast<u32_t*>( l_pcEndKey )[ 0 ];
		reinterpret_cast<u16_t*>( l_pcBufT )[ 2 ] = static_cast<u16_t>( reinterpret_cast<u16_t*>( l_pcBufT )[ 2 ]
				^ reinterpret_cast<u16_t*>( l_pcEndKey )[ 2 ] );
		for ( l_iCtr = 0; l_iCtr < DES::BLOCK_SIZE; l_iCtr ++ )
			{
			l_iCol = l_iRow = 0;
			row.set( 6, src.get( static_cast<u32_t>( l_iCtr ) * 6 ) );
			row.set( 7, src.get( static_cast<u32_t>( l_iCtr ) * 6 + 5 ) );
			for ( l_iCtrLoc = 0; l_iCtrLoc < 4; l_iCtrLoc ++ )
				col.set( static_cast<u32_t>( l_iCtrLoc ) + 4,
						src.get( static_cast<u32_t>( l_iCtr ) * 6 + l_iCtrLoc + 1 ) );
			l_cMask = n_pppcSBlock[ l_iCtr ][ l_iRow ][ l_iCol ];
			if ( ! ( l_iCtr & 1 ) )
				l_cMask = static_cast<char>( l_cMask << 4 );
				/* FIXME g++ 4.3 bug *///l_cMask <<= 4;
			l_pcBuf[ l_iCtr >> 1 ] = static_cast<char>( l_pcBuf[ l_iCtr >> 1 ] | l_cMask );
			/* FIXME g++ 4.3 bug *///l_pcBuf[ l_iCtr >> 1 ] |= l_cMask;
			}
		permutate( l_pcBuf, n_pcPBlockPermutation, 32 );
		reinterpret_cast<u32_t*>( l_pcBufT )[ 0 ] = reinterpret_cast<u32_t*>( l_pcBuf )[ 0 ]
																											^ reinterpret_cast<u32_t*>( l_pcBufL )[ 0 ];
		reinterpret_cast<u32_t*>( l_pcBufL )[ 0 ] = reinterpret_cast<u32_t*>( l_pcBufR )[ 0 ];
		reinterpret_cast<u32_t*>( l_pcBufR )[ 0 ] = reinterpret_cast<u32_t*>( l_pcBufT )[ 0 ];
		}
	reinterpret_cast<u32_t*>( a_pcBlock )[ 0 ] = reinterpret_cast<u32_t*>( l_pcBufR )[ 0 ];
	reinterpret_cast<u32_t*>( a_pcBlock )[ 1 ] = reinterpret_cast<u32_t*>( l_pcBufL )[ 0 ];
	return;
	}

void HDes::permutate( u8_t* a_pcBuffer, const u8_t* a_pcTab, int a_iLen ) const
	{
	int l_iCtr = 0;
	u8_t l_pcBufTmp[ DES::BLOCK_SIZE ];
	::memset( l_pcBufTmp, 0, DES::BLOCK_SIZE );
	HBitmap src;
	HBitmap dst;
	src.use( a_pcBuffer, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	dst.use( l_pcBufTmp, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	for ( l_iCtr = 0; l_iCtr < a_iLen; l_iCtr ++ )
		dst.set( static_cast<u32_t>( l_iCtr ),
				src.get( static_cast<u32_t>( a_pcTab[ l_iCtr ] ) ) );
	::memcpy( a_pcBuffer, l_pcBufTmp, DES::BLOCK_SIZE );
	return;
	}	
	
namespace des
	{
	
const u8_t n_pcBeginingPermutation[] =
		{
				57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
				61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7,
				56, 48, 40, 32, 24, 16, 8, 0, 58, 50, 42, 34, 26, 18, 10, 2,
				60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6
		};
		
const u8_t n_pcEndingPermutation[] =
		{
				39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30,
				37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28,
				35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26,
				33, 1, 41, 9, 49, 17, 57, 25, 32, 0, 40, 8, 48, 16, 56, 24
		};
		
/* key permutation */
const u8_t n_pcKeyPermutation[] =
		{
				56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17,
				9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35,
				62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21,
				13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3
		};
		
/* half-key shift permutation */
/* iteration number I,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 */
const u8_t n_pcCountOfMoves[] =
		{
				1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
		};

/* commpresion permutation */
const u8_t n_pcPermutationOfCompresion[] =
		{
				13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9,
				22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1,
				40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
				43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
		};
		
/* extension permutation */
const u8_t n_pcPermutationOfExpanding[] =
		{
				31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8,
				7, 8, 9, 10, 11, 12, 11, 12, 13, 14, 15, 16,
				15, 16, 17, 18, 19, 20, 19, 20, 21, 22, 23, 24,
				23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0
		};
		
/* P-block permutation */
const u8_t n_pcPBlockPermutation[] =
		{
				15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
				1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24
		};
		
/* S-Blocks */
const u8_t n_pppcSBlock[][ 4 ][ 16 ] =
		{
{
		{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 }, //S1
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
},
{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10}, //S2
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
},
{
		{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8}, //S3
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
},
{
		{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15}, //S4
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9 , 4, 5, 11, 12, 7, 2, 14}
},
{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9}, //S5
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
},
{
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, //S6
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
},
{
		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, //S7
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
},
{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, //S8
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
}
		};

	}

}

}


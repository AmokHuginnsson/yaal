/*
---				   `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdes.c - this file is integral part of `stdhapi' project.

	i.  You may not make any changes in Copyright information.
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

#include <string.h>

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hdes.h"
#include "bit.h"

typedef unsigned long int uli_t;
typedef unsigned short int usi_t;

namespace des
	{
	
extern const uc_t n_pcBeginingPermutation [ ];
extern const uc_t n_pcEndingPermutation [ ];
extern const uc_t n_pcKeyPermutation [ ];
extern const uc_t n_pcCountOfMoves [ ];
extern const uc_t n_pcPermutationOfCompresion [ ];
extern const uc_t n_pcPermutationOfExpanding [ ];
extern const uc_t n_pcPBlockPermutation [ ];
extern const uc_t n_pppcSBlock [ ] [ 4 ] [ 16 ];

	}

using namespace des;

HDes::HDes ( void )
	{
	flush_keys ( );
	return;
	}

HDes::~HDes ( void )
	{
	flush_keys ( );
	return;
	}

void HDes::generate_keys ( uc_t * a_pcPassword )
	{
	int l_iCtr = 0, l_iCtrLoc = 0;
	uc_t l_pcIKeyLow [ D_DES_BLOCK_SIZE ];
	uc_t l_pcIKeyHigh [ D_DES_BLOCK_SIZE ];
	uc_t l_pcTmpKey [ D_DES_BLOCK_SIZE ];
	flush_keys ( );
	for ( l_iCtr = 0; l_iCtr < D_DES_BLOCK_SIZE; l_iCtr ++ )
		{
		l_pcIKeyHigh [ l_iCtr ] = a_pcPassword [ l_iCtr ];
		l_pcIKeyLow [ l_iCtr ] = a_pcPassword [ l_iCtr + D_DES_BLOCK_SIZE ];
		}
	permutate ( l_pcIKeyHigh, n_pcKeyPermutation, 56 );
	permutate ( l_pcIKeyLow, n_pcKeyPermutation, 56 );
	for ( l_iCtr = 0; l_iCtr < D_DES_IKEYS_COUNT; l_iCtr ++ )
		{
		rol ( ( uc_t * ) l_pcIKeyHigh, 0, 28, n_pcCountOfMoves [ l_iCtr ] );
		rol ( ( uc_t * ) l_pcIKeyLow, 0, 28, n_pcCountOfMoves [ l_iCtr ] );
		rol ( ( uc_t * ) l_pcIKeyHigh, 28, 28, n_pcCountOfMoves [ l_iCtr ] );
		rol ( ( uc_t * ) l_pcIKeyLow, 28, 28, n_pcCountOfMoves [ l_iCtr ] );
		for ( l_iCtrLoc = 0; l_iCtrLoc < D_DES_BLOCK_SIZE; l_iCtrLoc ++ )
			l_pcTmpKey [ l_iCtrLoc ] = l_pcIKeyHigh [ l_iCtrLoc ];
		permutate ( l_pcTmpKey, n_pcPermutationOfCompresion, 48 );
		for ( l_iCtrLoc = 0; l_iCtrLoc < 6; l_iCtrLoc ++ )
			f_pppcIKeys [ 0 ] [ l_iCtr ] [ l_iCtrLoc ] = l_pcTmpKey [ l_iCtrLoc ];
		for ( l_iCtrLoc = 0; l_iCtrLoc < D_DES_BLOCK_SIZE; l_iCtrLoc ++ )
			l_pcTmpKey [ l_iCtrLoc ] = l_pcIKeyLow [ l_iCtrLoc ];
		permutate ( l_pcTmpKey, n_pcPermutationOfCompresion, 48 );
		for ( l_iCtrLoc = 0; l_iCtrLoc < 6; l_iCtrLoc ++ )
			f_pppcIKeys [ 1 ] [ l_iCtr ] [ l_iCtrLoc ] = l_pcTmpKey [ l_iCtrLoc ];
		}
	memset ( a_pcPassword, 0, D_DES_PASSWORD_SIZE );
	return ;
	}

void HDes::flush_keys ( void )
	{
	int l_iCtr = 0, l_iCtrLoc = 0;
	for ( l_iCtr = 0; l_iCtr < D_DES_SIDES_COUNT; l_iCtr ++ )
		for ( l_iCtrLoc = 0; l_iCtrLoc < D_DES_IKEYS_COUNT; l_iCtrLoc ++ )
			memset ( f_pppcIKeys [ l_iCtr ] [ l_iCtrLoc ], 0, D_DES_IKEY_SIZE );
	return;
	}

void HDes::crypt ( uc_t * a_pcBuffer, int a_iBlockCount, int a_iSide )
	{
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < a_iBlockCount; l_iCtr ++ )
		_3des ( a_pcBuffer + ( l_iCtr << 3 ), a_iSide );
	return;
	}
	
void HDes::_3des ( uc_t * a_pcBlock, int a_iSide )
	{
	permutate ( a_pcBlock, n_pcBeginingPermutation, 64 );
	_des ( a_pcBlock, a_iSide, 0 );
	_des ( a_pcBlock, 1 - a_iSide, 1 );
	_des ( a_pcBlock, a_iSide, 0 );
	permutate ( a_pcBlock, n_pcEndingPermutation, 64 );
	return;
	}
	
void HDes::_des ( uc_t * a_pcBlock, int a_iSide, int a_iPart )
	{
	int l_iCycle = 0, l_iCtr = 0, l_iCtrLoc, l_iCol, l_iRow;
	uc_t l_pcBuf [ D_DES_BLOCK_SIZE ], l_pcBufT [ D_DES_BLOCK_SIZE ];
	uc_t l_pcBufL [ D_DES_BLOCK_SIZE ], l_pcBufR [ D_DES_BLOCK_SIZE ];
	uc_t l_cMask = 0, * l_pcEndKey = NULL;
	( ( uli_t * ) l_pcBufL ) [ 0 ] = ( ( uli_t * ) a_pcBlock ) [ 0 ];
	( ( uli_t * ) l_pcBufR ) [ 0 ] = ( ( uli_t * ) a_pcBlock ) [ 1 ];
	( ( uli_t * ) l_pcBufT ) [ 0 ] = ( ( uli_t * ) a_pcBlock ) [ 1 ];
	for ( l_iCycle = 0; l_iCycle < D_DES_IKEYS_COUNT; l_iCycle ++ )
		{
		( ( uli_t * ) l_pcBuf ) [ 0 ] = 0;
		if ( a_iSide ) l_pcEndKey = f_pppcIKeys [ a_iPart ] [ l_iCycle ];
		else l_pcEndKey = f_pppcIKeys [ a_iPart ] [ 15 - l_iCycle ];
		permutate ( l_pcBufT, n_pcPermutationOfExpanding, 48 );
		( ( uli_t * ) l_pcBufT ) [ 0 ] = xxor32 ( ( ( uli_t * ) l_pcBufT ) [ 0 ],
																						( ( uli_t * ) l_pcEndKey ) [ 0 ] );
		( ( usi_t * ) l_pcBufT ) [ 2 ] = xxor16 ( ( ( usi_t * ) l_pcBufT ) [ 2 ],
																						( ( usi_t * ) l_pcEndKey ) [ 2 ] );
		for ( l_iCtr = 0; l_iCtr < D_DES_BLOCK_SIZE; l_iCtr ++ )
			{
			l_iCol = l_iRow = 0;
			setbit ( & l_iRow, 6, getbit ( l_pcBufT, ( uli_t ) l_iCtr * 6 ) );
			setbit ( & l_iRow, 7, getbit ( l_pcBufT, ( uli_t ) l_iCtr * 6 + 5 ) );
			for ( l_iCtrLoc = 0; l_iCtrLoc < 4; l_iCtrLoc ++ )
				setbit ( & l_iCol, ( uli_t ) l_iCtrLoc + 4,
						getbit ( l_pcBufT, ( uli_t ) l_iCtr * 6 + l_iCtrLoc + 1 ) );
			l_cMask = n_pppcSBlock [ l_iCtr ] [ l_iRow ] [ l_iCol ];
			if ( ! ( l_iCtr & 1 ) ) l_cMask <<= 4;
			l_pcBuf [ l_iCtr >> 1 ] |= l_cMask;
			}
		permutate ( l_pcBuf, n_pcPBlockPermutation, 32 );
		( ( uli_t * ) l_pcBufT ) [ 0 ] = xxor32 ( ( ( uli_t * ) l_pcBuf ) [ 0 ],
																							( ( uli_t * ) l_pcBufL ) [ 0 ] );
		( ( uli_t * ) l_pcBufL ) [ 0 ] = ( ( uli_t * ) l_pcBufR ) [ 0 ];
		( ( uli_t * ) l_pcBufR ) [ 0 ] = ( ( uli_t * ) l_pcBufT ) [ 0 ];
		}
	( ( uli_t * ) a_pcBlock ) [ 0 ] = ( ( uli_t * ) l_pcBufR ) [ 0 ];
	( ( uli_t * ) a_pcBlock ) [ 1 ] = ( ( uli_t * ) l_pcBufL ) [ 0 ];
	return;
	}

void HDes::permutate ( uc_t * a_pcBuffer, const uc_t * a_pcTab, int a_iLen )
	{
	int l_iCtr = 0;
	uc_t l_pcBufTmp [ D_DES_BLOCK_SIZE ];
	( ( uli_t * ) l_pcBufTmp ) [ 0 ] = 0;
	( ( uli_t * ) l_pcBufTmp ) [ 1 ] = 0;
	for ( l_iCtr = 0; l_iCtr < a_iLen; l_iCtr ++ )
		setbit ( l_pcBufTmp, ( uli_t ) l_iCtr,
				getbit ( a_pcBuffer,  ( uli_t ) a_pcTab [ l_iCtr ] ) );
	( ( uli_t * ) a_pcBuffer ) [ 0 ] = ( ( uli_t * ) l_pcBufTmp ) [ 0 ];
	( ( uli_t * ) a_pcBuffer ) [ 1 ] = ( ( uli_t * ) l_pcBufTmp ) [ 1 ];
	return;
	}	
	
namespace des
	{
	
const uc_t n_pcBeginingPermutation [ ] =
		{
				57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
				61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7,
				56, 48, 40, 32, 24, 16, 8, 0, 58, 50, 42, 34, 26, 18, 10, 2,
				60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6
		};
		
const uc_t n_pcEndingPermutation [ ] =
		{
				39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30,
				37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28,
				35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26,
				33, 1, 41, 9, 49, 17, 57, 25, 32, 0, 40, 8, 48, 16, 56, 24
		};
		
/* key permutation */
const uc_t n_pcKeyPermutation [ ] =
		{
				56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17,
				9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35,
				62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21,
				13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3
		};
		
/* half-key shift permutation */
/* iteration number I,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 */
const uc_t n_pcCountOfMoves [ ] =
		{
				1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
		};

/* commpresion permutation */
const uc_t n_pcPermutationOfCompresion [ ] =
		{
				13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9,
				22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1,
				40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
				43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
		};
		
/* extension permutation */
const uc_t n_pcPermutationOfExpanding [ ] =
		{
				31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8,
				7, 8, 9, 10, 11, 12, 11, 12, 13, 14, 15, 16,
				15, 16, 17, 18, 19, 20, 19, 20, 21, 22, 23, 24,
				23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0
		};
		
/* P-block permutation */
const uc_t n_pcPBlockPermutation [ ] =
		{
				15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
				1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24
		};
		
/* S-Blocks */
const uc_t n_pppcSBlock [ ] [ 4 ] [ 16 ] =
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


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdes.hxx"
#include "hbitmap.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

/*! \brief The DES algorithm helper data tables.
 */
namespace des {

extern const u8_t _beginningPermutation_[];
extern const u8_t _endingPermutation_[];
extern const u8_t _keyPermutation_[];
extern const u8_t _countOfMoves_[];
extern const u8_t _permutationOfCompression_[];
extern const u8_t _permutationOfExpanding_[];
extern const u8_t _pBlockPermutation_[];
extern const u8_t _sBlock_[][ 4 ][ 16 ];

}

using namespace des;

HDes::HDes( HString const& password_ ) {
	flush_keys();
	HUTF8String utf8( password_ );
	generate_keys( reinterpret_cast<u8_t const*>( utf8.c_str() ),
			static_cast<int>( utf8.byte_count() ) );
	return;
}

HDes::~HDes( void ) {
	M_PROLOG
	flush_keys();
	return;
	M_DESTRUCTOR_EPILOG
}

void HDes::generate_keys( u8_t const* password_, int len_ ) {
	u8_t iKeyLow[ DES::BLOCK_SIZE ];
	u8_t iKeyHigh[ DES::BLOCK_SIZE ];
	u8_t tmpKey[ DES::BLOCK_SIZE ];
	for ( int i( 0 ); i < DES::BLOCK_SIZE; ++ i ) {
		iKeyHigh[ i ] = password_[ i % len_ ];
		iKeyLow[ i ] = password_[ ( i + DES::BLOCK_SIZE ) % len_ ];
	}
	permute( iKeyHigh, _keyPermutation_, 56 );
	permute( iKeyLow, _keyPermutation_, 56 );
	HBitmap hi;
	HBitmap low;
	hi.use( iKeyHigh, 2 * DES::HALF_KEY_SIZE );
	low.use( iKeyHigh, 2 * DES::HALF_KEY_SIZE );
	for ( int key( 0 ); key < DES::IKEYS_COUNT; key ++ ) {
		hi.rotate_left( 0, 28, _countOfMoves_[ key ] );
		low.rotate_left( 0, 28, _countOfMoves_[ key ] );
		hi.rotate_left( 28, 28, _countOfMoves_[ key ] );
		low.rotate_left( 28, 28, _countOfMoves_[ key ] );
		for ( int i( 0 ); i < DES::BLOCK_SIZE; ++ i )
			tmpKey[ i ] = iKeyHigh[ i ];
		permute( tmpKey, _permutationOfCompression_, 48 );
		for ( int i( 0 ); i < 6; ++ i )
			reinterpret_cast<u8_t*>( _IKeys[ 0 ][ key ] )[ i ] = tmpKey[ i ];
		for ( int i( 0 ); i < DES::BLOCK_SIZE; ++ i )
			tmpKey[ i ] = iKeyLow[ i ];
		permute( tmpKey, _permutationOfCompression_, 48 );
		for ( int i( 0 ); i < 6; ++ i )
			reinterpret_cast<u8_t*>( _IKeys[ 1 ][ key ] )[ i ] = tmpKey[ i ];
	}
	return;
}

void HDes::flush_keys( void ) {
	for ( int ctr( 0 ); ctr < DES::SIDES_COUNT; ctr ++ )
		for ( int ctrLoc( 0 ); ctrLoc < DES::IKEYS_COUNT; ctrLoc ++ )
			::memset( _IKeys[ ctr ][ ctrLoc ], 0, DES::IKEY_SIZE );
	return;
}

void HDes::crypt( u8_t* buffer_, int long size_, action_t const& action_ ) {
	M_PROLOG
	M_ENSURE( ( action_ == CRYPT ) || ( action_ == DECRYPT ) );
	M_ENSURE( ! ( size_ % 8 ) );
	for ( int long i( 0 ); i < size_; i += 8 ) {
		u32_t block[2];
		::memcpy( block, buffer_ + i, 8 );
		_3des( block, action_ );
		::memcpy( buffer_ + i, block, 8 );
	}
	return;
	M_EPILOG
}

void HDes::_3des( u32_t* block_, int action_ ) {
	permute( reinterpret_cast<u8_t*>( block_ ), _beginningPermutation_, 64 );
	_des( block_, action_, 0 );
	_des( block_, 1 - action_, 1 );
	_des( block_, action_, 0 );
	permute( reinterpret_cast<u8_t*>( block_ ), _endingPermutation_, 64 );
	return;
}

void HDes::_des( u32_t* block_, int action_, int part_ ) {
	int col( 0 ), row( 0 );
	u32_t buf[ DES::BLOCK_SIZE / static_cast<int>( sizeof ( u32_t ) ) ];
	u32_t bufT[ DES::BLOCK_SIZE / static_cast<int>( sizeof ( u32_t ) ) ];
	u32_t bufL[ DES::BLOCK_SIZE / static_cast<int>( sizeof ( u32_t ) ) ];
	u32_t bufR[ DES::BLOCK_SIZE / static_cast<int>( sizeof ( u32_t ) ) ];
	bufL[ 0 ] = block_[ 0 ];
	bufR[ 0 ] = block_[ 1 ];
	bufT[ 0 ] = block_[ 1 ];
	HBitmap src;
	HBitmap rowBmp;
	HBitmap colBmp;
	src.use( bufT, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	rowBmp.use( &row, static_cast<int long>( sizeof ( row ) * DES::BITS_IN_BYTE ) );
	colBmp.use( &col, static_cast<int long>( sizeof ( col ) * DES::BITS_IN_BYTE ) );
	for ( int cycle( 0 ); cycle < DES::IKEYS_COUNT; ++ cycle ) {
		buf[ 0 ] = 0;
		u32_t* endKey( nullptr );
		if ( action_ )
			endKey = _IKeys[ part_ ][ cycle ];
		else
			endKey = _IKeys[ part_ ][ 15 - cycle ];
		permute( reinterpret_cast<u8_t*>( bufT ), _permutationOfExpanding_, 48 );
		bufT[ 0 ] = bufT[ 0 ] ^ endKey[ 0 ];
		reinterpret_cast<u16_t*>( bufT )[ 2 ] = static_cast<u16_t>( reinterpret_cast<u16_t*>( bufT )[ 2 ]
				^ reinterpret_cast<u16_t*>( endKey )[ 2 ] );
		for ( int ctr( 0 ); ctr < DES::BLOCK_SIZE; ctr ++ ) {
			col = row = 0;
			rowBmp.set( 6, src.get( ctr * 6 ) );
			rowBmp.set( 7, src.get( ctr * 6 + 5 ) );
			for ( int i( 0 ); i < 4; ++ i )
				colBmp.set( i + 4, src.get( ctr * 6 + i + 1 ) );
			u8_t mask( _sBlock_[ ctr ][ row ][ col ] );
			if ( ! ( ctr & 1 ) )
				mask = static_cast<u8_t>( mask << 4 );
				/* FIXME g++ 4.3 bug *///mask <<= 4;
			reinterpret_cast<u8_t*>( buf )[ ctr >> 1 ] = static_cast<u8_t>( reinterpret_cast<u8_t*>( buf )[ ctr >> 1 ] | mask );
			/* FIXME g++ 4.3 bug *///buf[ ctr >> 1 ] |= mask;
		}
		permute( reinterpret_cast<u8_t*>( buf ), _pBlockPermutation_, 32 );
		bufT[ 0 ] = buf[ 0 ] ^ bufL[ 0 ];
		bufL[ 0 ] = bufR[ 0 ];
		bufR[ 0 ] = bufT[ 0 ];
	}
	block_[ 0 ] = bufR[ 0 ];
	block_[ 1 ] = bufL[ 0 ];
	return;
}

void HDes::permute( u8_t* buffer_, const u8_t* tab_, int len_ ) const {
	int ctr = 0;
	u8_t bufTmp[ DES::BLOCK_SIZE ];
	::memset( bufTmp, 0, DES::BLOCK_SIZE );
	HBitmap src;
	HBitmap dst;
	src.use( buffer_, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	dst.use( bufTmp, DES::BLOCK_SIZE * DES::BITS_IN_BYTE );
	for ( ctr = 0; ctr < len_; ctr ++ )
		dst.set( ctr, src.get( tab_[ ctr ] ) );
	::memcpy( buffer_, bufTmp, DES::BLOCK_SIZE );
	return;
}

namespace des {

const u8_t _beginningPermutation_[] = {
				57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
				61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7,
				56, 48, 40, 32, 24, 16, 8, 0, 58, 50, 42, 34, 26, 18, 10, 2,
				60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6
	};

const u8_t _endingPermutation_[] = {
				39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30,
				37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28,
				35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26,
				33, 1, 41, 9, 49, 17, 57, 25, 32, 0, 40, 8, 48, 16, 56, 24
	};

/* key permutation */
const u8_t _keyPermutation_[] = {
				56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17,
				9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35,
				62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21,
				13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3
	};

/* half-key shift permutation */
/* iteration number I,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 */
const u8_t _countOfMoves_[] = {
				1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
	};

/* compression permutation */
const u8_t _permutationOfCompression_[] = {
				13, 16, 10, 23, 0, 4, 2, 27, 14, 5, 20, 9,
				22, 18, 11, 3, 25, 7, 15, 6, 26, 19, 12, 1,
				40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
				43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
	};

/* extension permutation */
const u8_t _permutationOfExpanding_[] = {
				31,  0,  1,  2,  3,  4,  3,  4,  5,  6,
				 7,  8,  7,  8,  9, 10, 11, 12, 11, 12,
				13, 14, 15, 16, 15, 16, 17, 18, 19, 20,
				19, 20, 21, 22, 23, 24, 23, 24, 25, 26,
				27, 28, 27, 28, 29, 30, 31, 0
	};

/* P-block permutation */
const u8_t _pBlockPermutation_[] = {
				15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
				1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24
	};

/* S-Blocks */
const u8_t _sBlock_[][ 4 ][ 16 ] = { {
		{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 }, //S1
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
}, {
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10}, //S2
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
}, {
		{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8}, //S3
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
}, {
		{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15}, //S4
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9 , 4, 5, 11, 12, 7, 2, 14}
}, {
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9}, //S5
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
}, {
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, //S6
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
}, {
		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, //S7
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
}, {
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, //S8
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
}
	};

}

}

}


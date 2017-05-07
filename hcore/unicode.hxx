/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  unicode.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/utf.hxx
 * \brief UTF-8 namespace definitions.
 */

#ifndef YAAL_HCORE_UTF8_HXX_INCLUDED
#define YAAL_HCORE_UTF8_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/numeric.hxx"

namespace yaal {

namespace unicode {

static u8_t const ENC_1_BYTES_MARK_MASK(  meta::obinary<010000000>::value );

static u8_t const ENC_2_BYTES_MARK_VALUE( meta::obinary<011000000>::value );
static u8_t const ENC_2_BYTES_VALUE_MASK( meta::obinary<000011111>::value );
static u8_t const ENC_2_BYTES_MARK_MASK(  meta::obinary<011100000>::value );

static u8_t const ENC_3_BYTES_MARK_VALUE( meta::obinary<011100000>::value );
static u8_t const ENC_3_BYTES_VALUE_MASK( meta::obinary<000001111>::value );
static u8_t const ENC_3_BYTES_MARK_MASK(  meta::obinary<011110000>::value );

static u8_t const ENC_4_BYTES_MARK_VALUE( meta::obinary<011110000>::value );
static u8_t const ENC_4_BYTES_VALUE_MASK( meta::obinary<000000111>::value );
static u8_t const ENC_4_BYTES_MARK_MASK(  meta::obinary<011111000>::value );

static u8_t const TAIL_BYTES_MARK_VALUE(  meta::obinary<010000000>::value );
static u8_t const TAIL_BYTES_VALUE_MASK(  meta::obinary<000111111>::value );
static u8_t const TAIL_BYTES_MARK_MASK(   meta::obinary<011000000>::value );

static u32_t const MAX_1_BYTE_CODE_POINT( 0x00007f );
static u32_t const MAX_2_BYTE_CODE_POINT( 0x0007ff );
static u32_t const MAX_3_BYTE_CODE_POINT( 0x00ffff );
static u32_t const MAX_4_BYTE_CODE_POINT( 0x10ffff );

inline int count_characters( char const* str_, int size_ ) {
	int cc( 0 );
	for ( char const* end( str_ + size_ ); str_ < end; ++ cc ) {
		if ( ! ( *str_ & unicode::ENC_1_BYTES_MARK_MASK ) ) {
			++ str_;
		} else if ( ( *str_ & unicode::ENC_2_BYTES_MARK_MASK ) == unicode::ENC_2_BYTES_MARK_VALUE ) {
			str_ += 2;
		} else if ( ( *str_ & unicode::ENC_3_BYTES_MARK_MASK ) == unicode::ENC_3_BYTES_MARK_VALUE ) {
			str_ += 3;
		} else if ( ( *str_ & unicode::ENC_4_BYTES_MARK_MASK ) == unicode::ENC_4_BYTES_MARK_VALUE ) {
			str_ += 4;
		} else {
			M_ASSERT( !"Invalid UTF-8 head sequence."[0] );
		}
	}
	return ( cc );
}

inline int rank( code_point_t value_ ) {
	int r( 0 );
	if ( value_ <= MAX_1_BYTE_CODE_POINT ) {
		r = 1;
	} else if ( value_ <= MAX_2_BYTE_CODE_POINT ) {
		r = 2;
	} else if ( value_ <= MAX_4_BYTE_CODE_POINT ) {
		r = 4;
	} else {
		throw yaal::hcore::HOutOfRangeException( yaal::hcore::to_string( "Unicode code point is out of range: " ).append( value_ ) );
	}
	return ( r );
}

inline int utf8_length( code_point_t value_ ) {
	int r( 0 );
	if ( value_ <= MAX_1_BYTE_CODE_POINT ) {
		r = 1;
	} else if ( value_ <= MAX_2_BYTE_CODE_POINT ) {
		r = 2;
	} else if ( value_ <= MAX_3_BYTE_CODE_POINT ) {
		r = 3;
	} else if ( value_ <= MAX_4_BYTE_CODE_POINT ) {
		r = 4;
	} else {
		throw yaal::hcore::HOutOfRangeException( yaal::hcore::to_string( "Unicode code point is out of range: " ).append( value_ ) );
	}
	return ( r );
}

}

}

#endif /* #ifndef YAAL_HCORE_UTF8_HXX_INCLUDED */


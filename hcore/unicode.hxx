/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/unicode.hxx
 * \brief Unicode and UTF-8 namespace definitions.
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

static u32_t const UCS_MAX_1_BYTE_CODE_POINT( 0x0000ff );
static u32_t const UCS_MAX_2_BYTE_CODE_POINT( 0x00ffff );
static u32_t const UCS_MAX_4_BYTE_CODE_POINT( 0x10ffff );

static u32_t const UTF8_MAX_1_BYTE_CODE_POINT( 0x00007f );
static u32_t const UTF8_MAX_2_BYTE_CODE_POINT( 0x0007ff );
static u32_t const UTF8_MAX_3_BYTE_CODE_POINT( 0x00ffff );
static u32_t const UTF8_MAX_4_BYTE_CODE_POINT( 0x10ffff );

inline int utf8_declared_length( yaal::u8_t head_ ) {
	int length( 0 );
	if ( ! ( head_ & unicode::ENC_1_BYTES_MARK_MASK ) ) {
		length = 1;
	} else if ( ( head_ & unicode::ENC_2_BYTES_MARK_MASK ) == unicode::ENC_2_BYTES_MARK_VALUE ) {
		length = 2;
	} else if ( ( head_ & unicode::ENC_3_BYTES_MARK_MASK ) == unicode::ENC_3_BYTES_MARK_VALUE ) {
		length = 3;
	} else if ( ( head_ & unicode::ENC_4_BYTES_MARK_MASK ) == unicode::ENC_4_BYTES_MARK_VALUE ) {
		length = 4;
	} else {
		M_ASSERT( !"Invalid UTF-8 head sequence."[0] );
	}
	return ( length );
}

inline int utf8_declared_length( char head_ ) {
	return ( utf8_declared_length( static_cast<yaal::u8_t>( head_ ) ) );
}

inline int count_characters( char const* str_, int size_ ) {
	int cc( 0 );
	for ( char const* end( str_ + size_ ); str_ < end; ++ cc ) {
		str_ += utf8_declared_length( *str_ );
	}
	return ( cc );
}

inline int rank( code_point_t value_ ) {
	int r( 0 );
	if ( value_ <= UCS_MAX_1_BYTE_CODE_POINT ) {
		r = 1;
	} else if ( value_ <= UCS_MAX_2_BYTE_CODE_POINT ) {
		r = 2;
	} else if ( value_ <= UCS_MAX_4_BYTE_CODE_POINT ) {
		r = 4;
	} else {
		throw yaal::hcore::HOutOfRangeException( yaal::hcore::to_string( "Unicode code point is out of range: " ).append( value_.get() ) );
	}
	return ( r );
}

inline int utf8_length( code_point_t value_ ) {
	int l( 0 );
	if ( value_ <= UTF8_MAX_1_BYTE_CODE_POINT ) {
		l = 1;
	} else if ( value_ <= UTF8_MAX_2_BYTE_CODE_POINT ) {
		l = 2;
	} else if ( value_ <= UTF8_MAX_3_BYTE_CODE_POINT ) {
		l = 3;
	} else if ( value_ <= UTF8_MAX_4_BYTE_CODE_POINT ) {
		l = 4;
	} else {
		throw yaal::hcore::HOutOfRangeException( yaal::hcore::to_string( "Unicode code point is out of range: " ).append( value_.get() ) );
	}
	return ( l );
}

namespace CODE_POINT {

static code_point_t const INVALID                               = code_point_t( static_cast<code_point_t::value_type>( -1 ) );

/* UCS-1 code points, UTF-8 1-byte code points,     0 -  127, U+0000 - U+007f */

static code_point_t const NUL                                   = code_point_t( 0x0000 );
static code_point_t const LINE_FEED                             = code_point_t( 0x00a0 );
static code_point_t const SPACE                                 = code_point_t( 0x0020 );
static code_point_t const LATIN_CAPITAL_LETTER_O                = code_point_t( 0x004f );
static code_point_t const LATIN_SMALL_LETTER_O                  = code_point_t( 0x006f );

/* UCS-1 code points, UTF-8 2-byte code points.   128 -  255, U+0080 - U+00ff */

/* Polish diacritic letters. */
static code_point_t const LATIN_CAPITAL_LETTER_O_WITH_ACUTE     = code_point_t( 0x00d3 );
static code_point_t const LATIN_SMALL_LETTER_O_WITH_ACUTE       = code_point_t( 0x00f3 );

/* UCS-2 code points, UTF-8 2-byte code points.   256 - 2047, U+0100 - U+07ff */

/* Polish diacritic letters. */
static code_point_t const LATIN_CAPITAL_LETTER_A_WITH_OGONEK    = code_point_t( 0x0104 );
static code_point_t const LATIN_SMALL_LETTER_A_WITH_OGONEK      = code_point_t( 0x0105 );
static code_point_t const LATIN_CAPITAL_LETTER_C_WITH_ACUTE     = code_point_t( 0x0106 );
static code_point_t const LATIN_SMALL_LETTER_C_WITH_ACUTE       = code_point_t( 0x0107 );
static code_point_t const LATIN_CAPITAL_LETTER_E_WITH_OGONEK    = code_point_t( 0x0118 );
static code_point_t const LATIN_SMALL_LETTER_E_WITH_OGONEK      = code_point_t( 0x0119 );
static code_point_t const LATIN_CAPITAL_LETTER_L_WITH_STROKE    = code_point_t( 0x0141 );
static code_point_t const LATIN_SMALL_LETTER_L_WITH_STROKE      = code_point_t( 0x0142 );
static code_point_t const LATIN_CAPITAL_LETTER_N_WITH_ACUTE     = code_point_t( 0x0143 );
static code_point_t const LATIN_SMALL_LETTER_N_WITH_ACUTE       = code_point_t( 0x0144 );
static code_point_t const LATIN_CAPITAL_LETTER_S_WITH_ACUTE     = code_point_t( 0x015a );
static code_point_t const LATIN_SMALL_LETTER_S_WITH_ACUTE       = code_point_t( 0x015b );
static code_point_t const LATIN_CAPITAL_LETTER_Z_WITH_ACUTE     = code_point_t( 0x0179 );
static code_point_t const LATIN_SMALL_LETTER_Z_WITH_ACUTE       = code_point_t( 0x017a );
static code_point_t const LATIN_CAPITAL_LETTER_Z_WITH_DOT_ABOVE = code_point_t( 0x017b );
static code_point_t const LATIN_SMALL_LETTER_Z_WITH_DOT_ABOVE   = code_point_t( 0x017c );

/* Greek alphabet. */
static code_point_t const GREEK_CAPITAL_LETTER_ALPHA            = code_point_t( 0x0391 );
static code_point_t const GREEK_CAPITAL_LETTER_BETA             = code_point_t( 0x0392 );
static code_point_t const GREEK_CAPITAL_LETTER_GAMMA            = code_point_t( 0x0393 );
static code_point_t const GREEK_CAPITAL_LETTER_DELTA            = code_point_t( 0x0394 );
static code_point_t const GREEK_CAPITAL_LETTER_EPSILON          = code_point_t( 0x0395 );
static code_point_t const GREEK_CAPITAL_LETTER_ZETA             = code_point_t( 0x0396 );
static code_point_t const GREEK_CAPITAL_LETTER_ETA              = code_point_t( 0x0397 );
static code_point_t const GREEK_CAPITAL_LETTER_THETA            = code_point_t( 0x0398 );
static code_point_t const GREEK_CAPITAL_LETTER_IOTA             = code_point_t( 0x0399 );
static code_point_t const GREEK_CAPITAL_LETTER_KAPPA            = code_point_t( 0x039a );
static code_point_t const GREEK_CAPITAL_LETTER_LAMBDA           = code_point_t( 0x039b );
static code_point_t const GREEK_CAPITAL_LETTER_MU               = code_point_t( 0x039c );
static code_point_t const GREEK_CAPITAL_LETTER_NU               = code_point_t( 0x039d );
static code_point_t const GREEK_CAPITAL_LETTER_XI               = code_point_t( 0x039e );
static code_point_t const GREEK_CAPITAL_LETTER_OMICRON          = code_point_t( 0x039f );
static code_point_t const GREEK_CAPITAL_LETTER_PI               = code_point_t( 0x03a0 );
static code_point_t const GREEK_CAPITAL_LETTER_RHO              = code_point_t( 0x03a1 );
/* Skipped value: 0x03a2 */
static code_point_t const GREEK_CAPITAL_LETTER_SIGMA            = code_point_t( 0x03a3 );
static code_point_t const GREEK_CAPITAL_LETTER_TAU              = code_point_t( 0x03a4 );
static code_point_t const GREEK_CAPITAL_LETTER_UPSILON          = code_point_t( 0x03a5 );
static code_point_t const GREEK_CAPITAL_LETTER_PHI              = code_point_t( 0x03a6 );
static code_point_t const GREEK_CAPITAL_LETTER_CHI              = code_point_t( 0x03a7 );
static code_point_t const GREEK_CAPITAL_LETTER_PSI              = code_point_t( 0x03a8 );
static code_point_t const GREEK_CAPITAL_LETTER_OMEGA            = code_point_t( 0x03a9 );

static code_point_t const GREEK_SMALL_LETTER_ALPHA              = code_point_t( 0x03b1 );
static code_point_t const GREEK_SMALL_LETTER_BETA               = code_point_t( 0x03b2 );
static code_point_t const GREEK_SMALL_LETTER_GAMMA              = code_point_t( 0x03b3 );
static code_point_t const GREEK_SMALL_LETTER_DELTA              = code_point_t( 0x03b4 );
static code_point_t const GREEK_SMALL_LETTER_EPSILON            = code_point_t( 0x03b5 );
static code_point_t const GREEK_SMALL_LETTER_ZETA               = code_point_t( 0x03b6 );
static code_point_t const GREEK_SMALL_LETTER_ETA                = code_point_t( 0x03b7 );
static code_point_t const GREEK_SMALL_LETTER_THETA              = code_point_t( 0x03b8 );
static code_point_t const GREEK_SMALL_LETTER_IOTA               = code_point_t( 0x03b9 );
static code_point_t const GREEK_SMALL_LETTER_KAPPA              = code_point_t( 0x03ba );
static code_point_t const GREEK_SMALL_LETTER_LAMBDA             = code_point_t( 0x03bb );
static code_point_t const GREEK_SMALL_LETTER_MU                 = code_point_t( 0x03bc );
static code_point_t const GREEK_SMALL_LETTER_NU                 = code_point_t( 0x03bd );
static code_point_t const GREEK_SMALL_LETTER_XI                 = code_point_t( 0x03be );
static code_point_t const GREEK_SMALL_LETTER_OMICRON            = code_point_t( 0x03bf );
static code_point_t const GREEK_SMALL_LETTER_PI                 = code_point_t( 0x03c0 );
static code_point_t const GREEK_SMALL_LETTER_RHO                = code_point_t( 0x03c1 );
static code_point_t const GREEK_SMALL_LETTER_SIGMAF             = code_point_t( 0x03c2 );
static code_point_t const GREEK_SMALL_LETTER_SIGMA              = code_point_t( 0x03c3 );
static code_point_t const GREEK_SMALL_LETTER_TAU                = code_point_t( 0x03c4 );
static code_point_t const GREEK_SMALL_LETTER_UPSILON            = code_point_t( 0x03c5 );
static code_point_t const GREEK_SMALL_LETTER_PHI                = code_point_t( 0x03c6 );
static code_point_t const GREEK_SMALL_LETTER_CHI                = code_point_t( 0x03c7 );
static code_point_t const GREEK_SMALL_LETTER_PSI                = code_point_t( 0x03c8 );
static code_point_t const GREEK_SMALL_LETTER_OMEGA              = code_point_t( 0x03c9 );

/* UCS-2 code points, UTF-8 3-byte code points. 2048 - 65535, U+0800 - U+ffff */

static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_A      = code_point_t( 0x1d43 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_B      = code_point_t( 0x1d47 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_C      = code_point_t( 0x1d9c );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_D      = code_point_t( 0x1d48 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_E      = code_point_t( 0x1d49 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_F      = code_point_t( 0x1da0 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_G      = code_point_t( 0x1d4d );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_H      = code_point_t( 0x02b0 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_I      = code_point_t( 0x2071 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_J      = code_point_t( 0x02b2 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_K      = code_point_t( 0x1d4f );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_L      = code_point_t( 0x02e1 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_M      = code_point_t( 0x1d50 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_N      = code_point_t( 0x207f );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_O      = code_point_t( 0x1d52 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_P      = code_point_t( 0x1d56 );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_Q = code_point_t( 0x ); */
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_R      = code_point_t( 0x02b3 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_S      = code_point_t( 0x02e2 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_T      = code_point_t( 0x1d57 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_U      = code_point_t( 0x1d58 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_V      = code_point_t( 0x1d5b );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_W      = code_point_t( 0x02b7 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_X      = code_point_t( 0x02e3 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_Y      = code_point_t( 0x02b8 );
static code_point_t const LATIN_SUPERSCRIPT_SMALL_LETTER_Z      = code_point_t( 0x1dbb );

static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_A    = code_point_t( 0x1d2c );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_B    = code_point_t( 0x1d2e );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_C = code_point_t( 0x ); */
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_D    = code_point_t( 0x1d30 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_E    = code_point_t( 0x1d31 );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_F = code_point_t( 0x ); */
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_G    = code_point_t( 0x1d33 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_H    = code_point_t( 0x1d34 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_I    = code_point_t( 0x1d35 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_J    = code_point_t( 0x1d36 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_K    = code_point_t( 0x1d37 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_L    = code_point_t( 0x1d38 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_M    = code_point_t( 0x1d39 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_N    = code_point_t( 0x1d3a );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_O    = code_point_t( 0x1d3c );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_P    = code_point_t( 0x1d3e );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_Q = code_point_t( 0x ); */
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_R    = code_point_t( 0x1d3f );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_S = code_point_t( 0x ); */
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_T    = code_point_t( 0x1d40 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_U    = code_point_t( 0x1d41 );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_V    = code_point_t( 0x2c7d );
static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_W    = code_point_t( 0x1d42 );
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_X = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUPERSCRIPT_CAPITAL_LETTER_Z = code_point_t( 0x ); */

static code_point_t const LATIN_SUPERSCRIPT_ZERO                = code_point_t( 0x2070 );
static code_point_t const LATIN_SUPERSCRIPT_ONE                 = code_point_t( 0x00b9 );
static code_point_t const LATIN_SUPERSCRIPT_TWO                 = code_point_t( 0x00b2 );
static code_point_t const LATIN_SUPERSCRIPT_THREE               = code_point_t( 0x00b3 );
static code_point_t const LATIN_SUPERSCRIPT_FOUR                = code_point_t( 0x2074 );
static code_point_t const LATIN_SUPERSCRIPT_FIVE                = code_point_t( 0x2075 );
static code_point_t const LATIN_SUPERSCRIPT_SIX                 = code_point_t( 0x2076 );
static code_point_t const LATIN_SUPERSCRIPT_SEVEN               = code_point_t( 0x2077 );
static code_point_t const LATIN_SUPERSCRIPT_EIGHT               = code_point_t( 0x2078 );
static code_point_t const LATIN_SUPERSCRIPT_NINE                = code_point_t( 0x2079 );

static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_A        = code_point_t( 0x2090 );
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_B = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_C = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_D = code_point_t( 0x ); */
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_E        = code_point_t( 0x2091 );
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_F = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_G = code_point_t( 0x ); */
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_H        = code_point_t( 0x2095 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_I        = code_point_t( 0x1d62 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_J        = code_point_t( 0x2c7c );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_K        = code_point_t( 0x2096 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_L        = code_point_t( 0x2097 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_M        = code_point_t( 0x2098 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_N        = code_point_t( 0x2099 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_O        = code_point_t( 0x2092 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_P        = code_point_t( 0x209a );
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_Q = code_point_t( 0x ); */
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_R        = code_point_t( 0x1d63 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_S        = code_point_t( 0x209b );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_T        = code_point_t( 0x209c );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_U        = code_point_t( 0x1d64 );
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_V        = code_point_t( 0x1d65 );
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_W = code_point_t( 0x ); */
static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_X        = code_point_t( 0x2093 );
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_SMALL_LETTER_Z = code_point_t( 0x ); */

/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_A = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_B = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_C = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_D = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_E = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_F = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_G = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_H = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_I = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_J = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_K = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_L = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_M = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_N = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_O = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_P = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_Q = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_R = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_S = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_T = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_U = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_V = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_W = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_X = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t const LATIN_SUBSCRIPT_CAPITAL_LETTER_Z = code_point_t( 0x ); */

static code_point_t const LATIN_SUBSCRIPT_ZERO                  = code_point_t( 0x2080 );
static code_point_t const LATIN_SUBSCRIPT_ONE                   = code_point_t( 0x2081 );
static code_point_t const LATIN_SUBSCRIPT_TWO                   = code_point_t( 0x2082 );
static code_point_t const LATIN_SUBSCRIPT_THREE                 = code_point_t( 0x2083 );
static code_point_t const LATIN_SUBSCRIPT_FOUR                  = code_point_t( 0x2084 );
static code_point_t const LATIN_SUBSCRIPT_FIVE                  = code_point_t( 0x2085 );
static code_point_t const LATIN_SUBSCRIPT_SIX                   = code_point_t( 0x2086 );
static code_point_t const LATIN_SUBSCRIPT_SEVEN                 = code_point_t( 0x2087 );
static code_point_t const LATIN_SUBSCRIPT_EIGHT                 = code_point_t( 0x2088 );
static code_point_t const LATIN_SUBSCRIPT_NINE                  = code_point_t( 0x2089 );

/* Mathematical Operators */

static code_point_t const N_ARY_PRODUCT                         = code_point_t( 0x220F );
static code_point_t const N_ARY_SUMMATION                       = code_point_t( 0x2211 );
static code_point_t const SQUARE_ROOT                           = code_point_t( 0x221a );

/* Miscellaneous Symbols. */
static code_point_t const SYMBOL_WHITE_FROWINING_FACE           = code_point_t( 0x2639 );
static code_point_t const SYMBOL_WHITE_SMILING_FACE             = code_point_t( 0x263a );
static code_point_t const SYMBOL_BLACK_SMILING_FACE             = code_point_t( 0x263b );

/* UCS-4 code points, UTF-8 4-byte code points. 65536 - 1114111, U+00010000 - U+0010ffff */

/* Emoji. */
static code_point_t const EMOJI_SNAKE                           = code_point_t( 0x1f40d );
static code_point_t const EMOJI_PENGUIN                         = code_point_t( 0x1f427 );
static code_point_t const EMOJI_PERSON_WALKING                  = code_point_t( 0x1f6b6 );
static code_point_t const EMOJI_STOP_SIGN                       = code_point_t( 0x1f6d1 );

}

}

}

#endif /* #ifndef YAAL_HCORE_UTF8_HXX_INCLUDED */


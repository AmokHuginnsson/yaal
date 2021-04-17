/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/unicode.hxx
 * \brief Unicode and UTF-8 namespace definitions.
 */

#ifndef YAAL_HCORE_UTF8_HXX_INCLUDED
#define YAAL_HCORE_UTF8_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
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

static u32_t const MASK( 0x1fffff );

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
	return length;
}

inline int utf8_declared_length( char head_ ) {
	return ( utf8_declared_length( static_cast<yaal::u8_t>( head_ ) ) );
}

inline int count_characters( char const* str_, int size_ ) {
	int cc( 0 );
	for ( char const* end( str_ + size_ ); str_ < end; ++ cc ) {
		str_ += utf8_declared_length( *str_ );
	}
	return cc;
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
	return r;
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
	return l;
}

namespace CODE_POINT {

#define M_DEFINE_CODE_POINT( name, codePoint, expectedValue ) \
static code_point_t constexpr name = code_point_t( codePoint ); \
static_assert( name.get() == expectedValue[0], "Wrong code poing value for: `" expectedValue "`" )

static code_point_t constexpr INVALID                               = code_point_t( static_cast<code_point_t::value_type>( -1 ) );

/* UCS-1 code points, UTF-8 1-byte code points,     0 -  127, U+0000 - U+007f */

M_DEFINE_CODE_POINT( NUL,                                                  0x0000, U"\0" );
M_DEFINE_CODE_POINT( LINE_FEED,                                            0x000a, U"\n" );
M_DEFINE_CODE_POINT( SPACE,                                                0x0020, U" " );
M_DEFINE_CODE_POINT( EXCLAMATION_MARK,                                     0x0021, U"!" );
M_DEFINE_CODE_POINT( QUOTATION_MARK,                                       0x0022, U"\"" );
M_DEFINE_CODE_POINT( NUMBER_SIGN,                                          0x0023, U"#" );
M_DEFINE_CODE_POINT( DOLLAR_SIGN,                                          0x0024, U"$" );
M_DEFINE_CODE_POINT( PERCENT_SIGN,                                         0x0025, U"%" );
M_DEFINE_CODE_POINT( AMPERSAND,                                            0x0026, U"&" );
M_DEFINE_CODE_POINT( APOSTROPHE,                                           0x0027, U"'" );
M_DEFINE_CODE_POINT( SLASH,                                                0x002f, U"/" );
M_DEFINE_CODE_POINT( LEFT_PARENTHESIS,                                     0x0028, U"(" );
M_DEFINE_CODE_POINT( RIGHT_PARENTHESIS,                                    0x0029, U")" );
M_DEFINE_CODE_POINT( ASTERISK,                                             0x002a, U"*" );
M_DEFINE_CODE_POINT( PLUS_SIGN,                                            0x002b, U"+" );
M_DEFINE_CODE_POINT( COMMA,                                                0x002c, U"," );
M_DEFINE_CODE_POINT( HYPHEN_MINUS,                                         0x002d, U"-" );
M_DEFINE_CODE_POINT( FULL_STOP,                                            0x002e, U"." );
M_DEFINE_CODE_POINT( COLON,                                                0x003a, U":" );
M_DEFINE_CODE_POINT( SEMICOLON,                                            0x003b, U";" );
M_DEFINE_CODE_POINT( LESS_THAN_SIGN,                                       0x003c, U"<" );
M_DEFINE_CODE_POINT( EQUAL_SIGN,                                           0x003d, U"=" );
M_DEFINE_CODE_POINT( GREATER_THAN_SIGN,                                    0x003e, U">" );
M_DEFINE_CODE_POINT( QUESTION_MARK,                                        0x003f, U"?" );
M_DEFINE_CODE_POINT( AT_SIGN,                                              0x0040, U"@" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_O,                               0x004f, U"O" );
M_DEFINE_CODE_POINT( LEFT_SQUARE_BRACKET,                                  0x005b, U"[" );
M_DEFINE_CODE_POINT( BACKSLASH,                                            0x005c, U"\\" );
M_DEFINE_CODE_POINT( RIGHT_SQUARE_BRACKET,                                 0x005d, U"]" );
M_DEFINE_CODE_POINT( CIRCUMFLEX_ACCENT,                                    0x005e, U"^" );
M_DEFINE_CODE_POINT( LOW_LINE,                                             0x005f, U"_" );
M_DEFINE_CODE_POINT( GRAVE_ACCENT,                                         0x0060, U"`" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_O,                                 0x006f, U"o" );
M_DEFINE_CODE_POINT( LEFT_CURLY_BRACKET,                                   0x007b, U"{" );
M_DEFINE_CODE_POINT( VERTICAL_BAR,                                         0x007c, U"|" );
M_DEFINE_CODE_POINT( RIGHT_CURLY_BRACKET,                                  0x007d, U"}" );
M_DEFINE_CODE_POINT( TILDE,                                                0x007e, U"~" );

/* UCS-1 code points, UTF-8 2-byte code points.   128 -  255, U+0080 - U+00ff */

/* Polish diacritic letters. */
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_O_WITH_ACUTE,                    0x00d3, U"Ó" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_O_WITH_ACUTE,                      0x00f3, U"ó" );

/* UCS-2 code points, UTF-8 2-byte code points.   256 - 2047, U+0100 - U+07ff */

/* Polish diacritic letters. */
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_A_WITH_OGONEK,                   0x0104, U"Ą" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_A_WITH_OGONEK,                     0x0105, U"ą" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_C_WITH_ACUTE,                    0x0106, U"Ć" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_C_WITH_ACUTE,                      0x0107, U"ć" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_E_WITH_OGONEK,                   0x0118, U"Ę" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_E_WITH_OGONEK,                     0x0119, U"ę" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_L_WITH_STROKE,                   0x0141, U"Ł" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_L_WITH_STROKE,                     0x0142, U"ł" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_N_WITH_ACUTE,                    0x0143, U"Ń" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_N_WITH_ACUTE,                      0x0144, U"ń" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_S_WITH_ACUTE,                    0x015a, U"Ś" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_S_WITH_ACUTE,                      0x015b, U"ś" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_Z_WITH_ACUTE,                    0x0179, U"Ź" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_Z_WITH_ACUTE,                      0x017a, U"ź" );
M_DEFINE_CODE_POINT( LATIN_CAPITAL_LETTER_Z_WITH_DOT_ABOVE,                0x017b, U"Ż" );
M_DEFINE_CODE_POINT( LATIN_SMALL_LETTER_Z_WITH_DOT_ABOVE,                  0x017c, U"ż" );

/* Greek alphabet. */
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_ALPHA,                           0x0391, U"Α" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_BETA,                            0x0392, U"Β" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_GAMMA,                           0x0393, U"Γ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_DELTA,                           0x0394, U"Δ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_EPSILON,                         0x0395, U"Ε" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_ZETA,                            0x0396, U"Ζ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_ETA,                             0x0397, U"Η" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_THETA,                           0x0398, U"Θ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_IOTA,                            0x0399, U"Ι" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_KAPPA,                           0x039a, U"Κ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_LAMBDA,                          0x039b, U"Λ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_MU,                              0x039c, U"Μ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_NU,                              0x039d, U"Ν" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_XI,                              0x039e, U"Ξ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_OMICRON,                         0x039f, U"Ο" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_PI,                              0x03a0, U"Π" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_RHO,                             0x03a1, U"Ρ" );
/* Skipped value: 0x03a2 */
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_SIGMA,                           0x03a3, U"Σ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_TAU,                             0x03a4, U"Τ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_UPSILON,                         0x03a5, U"Υ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_PHI,                             0x03a6, U"Φ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_CHI,                             0x03a7, U"Χ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_PSI,                             0x03a8, U"Ψ" );
M_DEFINE_CODE_POINT( GREEK_CAPITAL_LETTER_OMEGA,                           0x03a9, U"Ω" );

M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_ALPHA,                             0x03b1, U"α" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_BETA,                              0x03b2, U"β" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_GAMMA,                             0x03b3, U"γ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_DELTA,                             0x03b4, U"δ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_EPSILON,                           0x03b5, U"ε" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_ZETA,                              0x03b6, U"ζ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_ETA,                               0x03b7, U"η" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_THETA,                             0x03b8, U"θ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_IOTA,                              0x03b9, U"ι" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_KAPPA,                             0x03ba, U"κ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_LAMBDA,                            0x03bb, U"λ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_MU,                                0x03bc, U"μ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_NU,                                0x03bd, U"ν" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_XI,                                0x03be, U"ξ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_OMICRON,                           0x03bf, U"ο" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_PI,                                0x03c0, U"π" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_RHO,                               0x03c1, U"ρ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_SIGMAF,                            0x03c2, U"ς" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_SIGMA,                             0x03c3, U"σ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_TAU,                               0x03c4, U"τ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_UPSILON,                           0x03c5, U"υ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_PHI,                               0x03c6, U"φ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_CHI,                               0x03c7, U"χ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_PSI,                               0x03c8, U"ψ" );
M_DEFINE_CODE_POINT( GREEK_SMALL_LETTER_OMEGA,                             0x03c9, U"ω" );

/* UCS-2 code points, UTF-8 3-byte code points. 2048 - 65535, U+0800 - U+ffff */

M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_A,                     0x1d43, U"ᵃ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_B,                     0x1d47, U"ᵇ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_C,                     0x1d9c, U"ᶜ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_D,                     0x1d48, U"ᵈ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_E,                     0x1d49, U"ᵉ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_F,                     0x1da0, U"ᶠ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_G,                     0x1d4d, U"ᵍ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_H,                     0x02b0, U"ʰ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_I,                     0x2071, U"ⁱ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_J,                     0x02b2, U"ʲ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_K,                     0x1d4f, U"ᵏ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_L,                     0x02e1, U"ˡ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_M,                     0x1d50, U"ᵐ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_N,                     0x207f, U"ⁿ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_O,                     0x1d52, U"ᵒ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_P,                     0x1d56, U"ᵖ" );
/* missing from Unicode *//* M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_Q,                     0x    , U"?" ); */
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_R,                     0x02b3, U"ʳ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_S,                     0x02e2, U"ˢ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_T,                     0x1d57, U"ᵗ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_U,                     0x1d58, U"ᵘ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_V,                     0x1d5b, U"ᵛ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_W,                     0x02b7, U"ʷ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_X,                     0x02e3, U"ˣ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_Y,                     0x02b8, U"ʸ" );
M_DEFINE_CODE_POINT( LATIN_SUPERSCRIPT_SMALL_LETTER_Z,                     0x1dbb, U"ᶻ" );

static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_A    = code_point_t( 0x1d2c );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_B    = code_point_t( 0x1d2e );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_C = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_D    = code_point_t( 0x1d30 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_E    = code_point_t( 0x1d31 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_F = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_G    = code_point_t( 0x1d33 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_H    = code_point_t( 0x1d34 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_I    = code_point_t( 0x1d35 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_J    = code_point_t( 0x1d36 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_K    = code_point_t( 0x1d37 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_L    = code_point_t( 0x1d38 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_M    = code_point_t( 0x1d39 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_N    = code_point_t( 0x1d3a );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_O    = code_point_t( 0x1d3c );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_P    = code_point_t( 0x1d3e );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_Q = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_R    = code_point_t( 0x1d3f );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_S = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_T    = code_point_t( 0x1d40 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_U    = code_point_t( 0x1d41 );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_V    = code_point_t( 0x2c7d );
static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_W    = code_point_t( 0x1d42 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_X = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUPERSCRIPT_CAPITAL_LETTER_Z = code_point_t( 0x ); */

static code_point_t constexpr LATIN_SUPERSCRIPT_ZERO                = code_point_t( 0x2070 );
static code_point_t constexpr LATIN_SUPERSCRIPT_ONE                 = code_point_t( 0x00b9 );
static code_point_t constexpr LATIN_SUPERSCRIPT_TWO                 = code_point_t( 0x00b2 );
static code_point_t constexpr LATIN_SUPERSCRIPT_THREE               = code_point_t( 0x00b3 );
static code_point_t constexpr LATIN_SUPERSCRIPT_FOUR                = code_point_t( 0x2074 );
static code_point_t constexpr LATIN_SUPERSCRIPT_FIVE                = code_point_t( 0x2075 );
static code_point_t constexpr LATIN_SUPERSCRIPT_SIX                 = code_point_t( 0x2076 );
static code_point_t constexpr LATIN_SUPERSCRIPT_SEVEN               = code_point_t( 0x2077 );
static code_point_t constexpr LATIN_SUPERSCRIPT_EIGHT               = code_point_t( 0x2078 );
static code_point_t constexpr LATIN_SUPERSCRIPT_NINE                = code_point_t( 0x2079 );

static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_A        = code_point_t( 0x2090 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_B = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_C = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_D = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_E        = code_point_t( 0x2091 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_F = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_G = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_H        = code_point_t( 0x2095 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_I        = code_point_t( 0x1d62 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_J        = code_point_t( 0x2c7c );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_K        = code_point_t( 0x2096 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_L        = code_point_t( 0x2097 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_M        = code_point_t( 0x2098 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_N        = code_point_t( 0x2099 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_O        = code_point_t( 0x2092 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_P        = code_point_t( 0x209a );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_Q = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_R        = code_point_t( 0x1d63 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_S        = code_point_t( 0x209b );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_T        = code_point_t( 0x209c );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_U        = code_point_t( 0x1d64 );
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_V        = code_point_t( 0x1d65 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_W = code_point_t( 0x ); */
static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_X        = code_point_t( 0x2093 );
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_SMALL_LETTER_Z = code_point_t( 0x ); */

/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_A = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_B = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_C = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_D = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_E = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_F = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_G = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_H = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_I = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_J = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_K = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_L = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_M = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_N = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_O = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_P = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_Q = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_R = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_S = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_T = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_U = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_V = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_W = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_X = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_Y = code_point_t( 0x ); */
/* missing from Unicode *//* static code_point_t constexpr LATIN_SUBSCRIPT_CAPITAL_LETTER_Z = code_point_t( 0x ); */

static code_point_t constexpr LATIN_SUBSCRIPT_ZERO                  = code_point_t( 0x2080 );
static code_point_t constexpr LATIN_SUBSCRIPT_ONE                   = code_point_t( 0x2081 );
static code_point_t constexpr LATIN_SUBSCRIPT_TWO                   = code_point_t( 0x2082 );
static code_point_t constexpr LATIN_SUBSCRIPT_THREE                 = code_point_t( 0x2083 );
static code_point_t constexpr LATIN_SUBSCRIPT_FOUR                  = code_point_t( 0x2084 );
static code_point_t constexpr LATIN_SUBSCRIPT_FIVE                  = code_point_t( 0x2085 );
static code_point_t constexpr LATIN_SUBSCRIPT_SIX                   = code_point_t( 0x2086 );
static code_point_t constexpr LATIN_SUBSCRIPT_SEVEN                 = code_point_t( 0x2087 );
static code_point_t constexpr LATIN_SUBSCRIPT_EIGHT                 = code_point_t( 0x2088 );
static code_point_t constexpr LATIN_SUBSCRIPT_NINE                  = code_point_t( 0x2089 );

/* Mathematical Operators */

static code_point_t constexpr N_ARY_PRODUCT                         = code_point_t( 0x220F );
static code_point_t constexpr N_ARY_SUMMATION                       = code_point_t( 0x2211 );
static code_point_t constexpr SQUARE_ROOT                           = code_point_t( 0x221a );

/* Miscellaneous Symbols. */
static code_point_t constexpr SYMBOL_WHITE_FROWINING_FACE           = code_point_t( 0x2639 );
static code_point_t constexpr SYMBOL_WHITE_SMILING_FACE             = code_point_t( 0x263a );
static code_point_t constexpr SYMBOL_BLACK_SMILING_FACE             = code_point_t( 0x263b );

/* UCS-4 code points, UTF-8 4-byte code points. 65536 - 1114111, U+00010000 - U+0010ffff */

/* Emoji. */
static code_point_t constexpr EMOJI_SNAKE                           = code_point_t( 0x1f40d );
static code_point_t constexpr EMOJI_PENGUIN                         = code_point_t( 0x1f427 );
static code_point_t constexpr EMOJI_PERSON_WALKING                  = code_point_t( 0x1f6b6 );
static code_point_t constexpr EMOJI_STOP_SIGN                       = code_point_t( 0x1f6d1 );

/* Private Use Areas */

static code_point_t constexpr PRIVATE_USE_AREA                      = code_point_t( 0xe000 );
static code_point_t constexpr PRIVATE_USE_AREA_END                  = code_point_t( 0xf8ff );
static code_point_t constexpr SUPPLEMENTARY_PRIVATE_USE_AREA_A      = code_point_t( 0xF0000 );
static code_point_t constexpr SUPPLEMENTARY_PRIVATE_USE_AREA_A_END  = code_point_t( 0xFFFFF );
static code_point_t constexpr SUPPLEMENTARY_PRIVATE_USE_AREA_B      = code_point_t( 0x100000 );
static code_point_t constexpr SUPPLEMENTARY_PRIVATE_USE_AREA_B_END  = code_point_t( 0x10FFFF );

#undef M_DEFINE_CODE_POINT

}

}

}

#endif /* #ifndef YAAL_HCORE_UTF8_HXX_INCLUDED */


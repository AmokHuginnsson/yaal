/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/base.hxx
 * \brief Basic declarations used across whole library.
 *
 * This file holds main #defines, macros and global variables
 * used across whole yaal.
 */

#ifndef YAAL_HCORE_BASE_HXX_INCLUDED
#define YAAL_HCORE_BASE_HXX_INCLUDED 1

#include <atomic>

#include "hcore/compat.hxx"
#include "hcore/macro.hxx"

/*! \mainpage yaal library documentation.
 *
 * \htmlinclude ./main.xml
 */

/*! \brief All library API belongs here.
 *
 * This namaspace contains all yaal library API and code.
 * YAAL code and API is divided into several sections.
 *
 * yaal comes from YAAL which comes from Y.A.A.L. which comes from:
 *  - Yet
 *  - Another
 *  - Abstraction
 *  - Layer
 */
namespace yaal {

typedef std::atomic<bool> exit_flag_t;
extern M_YAAL_HCORE_PUBLIC_API exit_flag_t _isKilled_;

struct LexicalCast {};

/*! \brief Convert between distinct datatypes.
 *
 * \tparam to_t - destination type.
 * \param val - value to be converted to destination type.
 * \return val equivalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& val );

template<typename to_t>
inline to_t lexical_cast( char* val ) {
	return ( lexical_cast<to_t, char const*>( val ) );
}

/*! \brief Generate error message (possibly human readable) based on error code.
 *
 * \param code - error code to describe.
 * \return Message string describing error of given error code.
 */
char const* error_message( int code );

/*! \brief The crudest way of casting between types.
 *
 * It differs from reinterpret_cast<> in this regard that it can handle
 * cast to/from method pointers.
 *
 * \tparam to_t - destination type.
 * \param val - value to be converted to destination type.
 * \return val equivalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t bit_cast( from_t val_ ) {
	union caster {
		from_t _from;
		to_t _to;
		char _buf;
	} cast;
	/* We shall not use hcore/algorithm.hxx here or we suffer compilation slowdown. */
	int SIZE( static_cast<int>( sizeof ( from_t ) > sizeof ( to_t ) ? sizeof ( from_t ) : sizeof ( to_t ) ) );
	for ( int i( 0 ); i < SIZE; ++ i ) {
		(&cast._buf)[ i ] = 0;
	}
	cast._from = val_;
	return ( cast._to );
}

#if SIZEOF_DOUBLE_LONG > SIZEOF_DOUBLE
static int const SIZEOF_DOUBLE_LONG_PRECISION( 10 );
#else /* #if SIZEOF_DOUBLE_LONG > SIZEOF_DOUBLE */
static int const SIZEOF_DOUBLE_LONG_PRECISION( SIZEOF_DOUBLE_LONG );
#endif /* #else #if SIZEOF_DOUBLE_LONG > SIZEOF_DOUBLE */
inline void normalize_double_long( double long& val_ ) {
	char* mem( static_cast<char*>( static_cast<void*>( &val_ ) ) );
	for ( int i( SIZEOF_DOUBLE_LONG_PRECISION ); i < SIZEOF_DOUBLE_LONG; ++ i ) {
		mem[ i ] = 0;
	}
	return;
}

/*! \brief Temporarily backup some value on a side and restore it at end of scope.
 */
template<typename tType>
class HScopedValueReplacement {
	tType _orig;
	tType& _value;
public:
	HScopedValueReplacement( tType& value_ )
		: _orig( value_ ), _value( value_ ) {
		return;
	}
	HScopedValueReplacement( tType& value_, tType const& tmp_ )
		: _orig( value_ ), _value( value_ ) {
		_value = tmp_;
		return;
	}
	~HScopedValueReplacement( void ) {
		_value = _orig;
		return;
	}
};

template<int const N>
struct Int2Type {
	enum { value = N };
};

/*! \brief Get number of elements of C array.
 */
template <typename T, int long unsigned N>
inline constexpr int size( T const (&) [N] ) noexcept {
	return ( static_cast<int>( N ) );
}

template<typename iter_t>
bool is_hexadecimal( iter_t it_, iter_t end_ ) {
	int len( static_cast<int>( end_ - it_ ) );
	bool isHex( false );
	if ( ( len >= 4 ) && ( *it_ == '-' ) ) { /* -0x0 */
		++ it_;
		-- len;
	}
	if ( len >= 3 ) {
		if ( *it_ == '0' ) {
			++ it_;
			if ( ( *it_ == 'x' ) || ( *it_ == 'X' ) ) {
				++ it_;
				isHex = (
					( ( *it_ >= '0' ) && ( *it_ <= '9' ) )
					|| ( ( *it_ >= 'a' ) && ( *it_ <= 'f' ) )
					|| ( ( *it_ >= 'A' ) && ( *it_ <= 'F' ) )
				);
			}
		}
	}
	return ( isHex );
}

template<typename iter_t>
bool is_binary( iter_t it_, iter_t end_ ) {
	int len( static_cast<int>( end_ - it_ ) );
	bool isBin( false );
	if ( ( len >= 4 ) && ( *it_ == '-' ) ) { /* -0b0 */
		++ it_;
		-- len;
	}
	if ( len >= 3 ) {
		if ( *it_ == '0' ) {
			++ it_;
			if ( ( *it_ == 'b' ) || ( *it_ == 'B' ) ) {
				++ it_;
				isBin = ( ( *it_ == '0' ) || ( *it_ == '1' ) );
			}
		}
	}
	return ( isBin );
}

template<typename iter_t>
bool is_octal( iter_t it_, iter_t end_ ) {
	int len( static_cast<int>( end_ - it_ ) );
	bool isOctal( false );
	if ( ( len >= 3 ) && ( *it_ == '-' ) ) { /* -07 */
		++ it_;
		-- len;
	}
	if ( ( len >= 2 ) && ( *it_ == '0' ) ) {
		++ it_;
		-- len;
		if ( ( *it_ == 'o' ) || ( *it_ == 'O' ) ) {
			++ it_;
			-- len;
		}
		isOctal = ( ( len > 0 ) && ( *it_ >= '0' ) && ( *it_ <= '7' ) );
	}
	return ( isOctal );
}

template<typename type_t>
bool is_hexadecimal( type_t const& );
template<typename type_t>
bool is_binary( type_t const& );
template<typename type_t>
bool is_octal( type_t const& );

}

#endif /* #ifndef YAAL_HCORE_BASE_HXX_INCLUDED */


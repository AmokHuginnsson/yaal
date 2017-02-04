/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  base.hxx - this file is integral part of `yaal' project.

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
	for ( int i( 0 ), SIZE( static_cast<int>( sizeof ( from_t ) > sizeof ( to_t ) ? sizeof ( from_t ) : sizeof ( to_t ) ) ); i < SIZE; ++ i )
		(&cast._buf)[ i ] = 0;
	cast._from = val_;
	return ( cast._to );
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

/*! \brief Helper to find number of elements in C array.
 */
template <typename T, int long unsigned N>
char ( &YaalArrayElementCountHelper( T ( &YaalArrayElementCountHelperArray )[N] ) )[N];

template<typename type_t>
bool is_hexadecimal( type_t const& );
template<typename type_t>
bool is_binary( type_t const& );
template<typename type_t>
bool is_octal( type_t const& );
bool is_hexadecimal( char const*, int );
bool is_binary( char const*, int );
bool is_octal( char const*, int );

}

#endif /* #ifndef YAAL_HCORE_BASE_HXX_INCLUDED */


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	base.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/base.hxx
 * \brief Basic declarations used across whole library.
 *
 * This file holds main #defines, macros and global variables
 * used acros whole yaal.
 */

#ifndef YAAL_HCORE_BASE_HXX_INCLUDED
#define YAAL_HCORE_BASE_HXX_INCLUDED 1

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
namespace yaal
{

extern bool _isKilled_;

/*! \brief Convert between distinct datatypes.
 *
 * \tparam to_t - destination type.
 * \param val - value to be converted to destination type.
 * \return val equvalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& val );

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
 * \return val equvalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t bit_cast( from_t val_ )
	{
	union caster
		{
		from_t _from;
		to_t _to;
		char _buf;
		} cast;
	/* We shall not use hcore/algorithm.hxx here or we suffer compilation slowdown. */
	for ( int i( 0 ), SIZE( sizeof ( from_t ) > sizeof ( to_t ) ? sizeof ( from_t ) : sizeof ( to_t ) ); i < SIZE; ++ i )
		(&cast._buf)[ i ] = 0;
	cast._from = val_;
	return ( cast._to );
	}

template<typename field_t, int const id = 0>
class HField
	{
protected:
	field_t _field;
public:
	HField( void ) : _field() {}
	template<typename a0_t>
	HField( a0_t a0_ ) : _field( a0_ ) {}
	template<typename a0_t, typename a1_t>
	HField( a0_t a0_, a1_t a1_ ) : _field( a0_, a1_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_ ) : _field( a0_, a1_, a2_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_ ) : _field( a0_, a1_, a2_, a3_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_ ) : _field( a0_, a1_, a2_, a3_, a4_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_ ) : _field( a0_, a1_, a2_, a3_, a4_, a5_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_ ) : _field( a0_, a1_, a2_, a3_, a4_, a5_, a6_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_ ) : _field( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_, a8_t a8_ ) : _field( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t, typename a9_t>
	HField( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_, a8_t a8_, a9_t a9_ ) : _field( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_ ) {}
	virtual ~HField( void ) {}
	field_t& ref( void )
		{ return ( _field ); }
	field_t const& ref( void ) const
		{ return ( _field ); }
	};

template<typename tType>
class HScopedValueReplacement
	{
	tType _orig;
	tType& _value;
public:
	HScopedValueReplacement( tType& value_ ) : _orig( value_), _value( value_ ) {}
	HScopedValueReplacement( tType& value_, tType const& tmp_ ) : _orig( value_), _value( value_ ) { _value = tmp_; }
	~HScopedValueReplacement( void )
		{ _value = _orig; }
	};

template<typename id, typename T>
struct explicit_type
	{
	T _value;
	explicit explicit_type( T const& value_ ) : _value( value_ ) {}
	};

template <typename T, int long unsigned N>
char ( &YaalArrayElementCountHelper( T ( &YaalArrayElementCountHelperArray )[N] ) )[N];

template<typename type_t>
bool is_hexadecimal( type_t const& );
template<typename type_t>
bool is_binary( type_t const& );
template<typename type_t>
bool is_octal( type_t const& );

}

#endif /* #ifndef YAAL_HCORE_BASE_HXX_INCLUDED */


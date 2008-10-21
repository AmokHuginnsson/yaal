/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	base.h - this file is integral part of `yaal' project.

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

/*! \file hcore/base.h
 * \brief Basic declarations used across whole library.
 */

#ifndef __YAAL_HCORE_BASE_H
#define __YAAL_HCORE_BASE_H

#include "hcore/hexception.h"
#include "hcore/compat.h"

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

template<bool> struct static_assert_failure;
template<> struct static_assert_failure<true> { enum { value = 1 }; };
#define STATIC_ASSERT( condition ) { typedef char SAF##__LINE__ [ static_assert_failure<( condition )>::value ]; }

#define _(string) gettext (string)

#define M_VCSID(id) namespace { static char const* wrapper_VCSID( void ) { wrapper_VCSID(); return id; } }
#define M_VCSTID(id) namespace { static char const* wrapper_VCSTID( void ) { wrapper_VCSTID(); return id; } }
#define M_THROW( msg, e_no ) yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, msg, e_no )
#define M_PROLOG try{
#define M_EPILOG } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); throw; }
#define M_FINAL } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); e.print_error( true ); } catch ( yaal::hcore::HFailedAssertion& ) { exit( -1 ); } catch ( int const& retVal ) { return ( retVal ); }
#define M_ENSURE( condition ) do { if ( ! ( condition ) ){ yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno, error_message( errno ) ); } } while ( 0 );
#ifndef NDEBUG
#	define M_ASSERT( condition ) do { if ( ! ( condition ) )yaal::hcore::HException::failed_assert( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition ); } while ( 0 );
#else /* NDEBUG */
#	define M_ASSERT( c ) /**/
#endif /* not NDEBUG */

template<typename tType>
inline void swap( tType& left, tType& right )
	{
	if ( &left != &right )
		{
		tType tmp( left );
		left = right;
		right = tmp;
		}
	return;
	}

template<typename function_t, typename value_t>
class HBinder
	{
	template<typename func_t, typename arga_t, typename argb_t>
	struct type_of
		{
		static func_t _type_provider;
		typedef typeof( _type_provider( arga_t(), argb_t() ) ) type;
		};
	function_t CALLER;
	value_t f_tValue;
public:
	HBinder( function_t, value_t );
	template<typename tType>
	typename type_of<function_t, value_t, tType>::type operator()( tType value ) const
		{ return ( CALLER( value, f_tValue ) ); }
	};

template<typename function_t, typename value_t>
HBinder<function_t, value_t>::HBinder( function_t func, value_t value )
	: CALLER( func ), f_tValue( value )
	{ }

template<typename function_t, typename value_t>
HBinder<function_t, value_t> bind2nd( function_t func, value_t value )
	{
	return ( HBinder<function_t, value_t>( func, value ) );
	}

template<typename iterator_t, typename call_t>
void for_each( iterator_t it, iterator_t const& end, call_t& CALL )
	{
	for ( ; it != end; ++ it )
		CALL( *it );
	}

template<typename src_it_t, typename dst_it_t>
void copy( src_it_t src, src_it_t const& end, dst_it_t dst )
	{
	for ( ; src != end; ++ src, ++ dst )
		*dst = *src;
	return;
	}

template<typename first_it_t, typename second_it_t>
void swap_ranges( first_it_t first, first_it_t const& end, second_it_t second )
	{
	for ( ; first != end; ++ first, ++ second )
		swap( *first, *second ) ;
	return;
	}

template<typename dst_it_t, typename filler_t>
void fill( dst_it_t it, dst_it_t end, filler_t const& filler )
	{
	for ( ; it != end; ++ it )
		*it = filler;
	return;
	}

template<typename dst_it_t, typename filler_t>
void fill_n( dst_it_t it, int long const& count, filler_t const& filler )
	{
	for ( int long i = 0; i < count; ++ i, ++ it )
		*it = filler;
	return;
	}

template<typename iterator_t, typename generator_t>
void generate( iterator_t it, iterator_t const& end, generator_t generator )
	{
	for ( ; it != end; ++ it )
		*it = generator();
	return;
	}

template<typename iterator_t, typename return_t>
return_t accumulate( iterator_t it, iterator_t end, return_t ret )
	{
	for ( ; it != end; ++ it )
		ret += *it;
	return ( ret );
	}

/*! \brief Type trait alteration utilities.
 */
namespace trait
{

/*! \brief Meta function used to strip reference from type.
 *
 * \tparam T - type to strip reference from.
 */
template<typename T>
struct strip_reference
	{
	typedef T type;
	};

/*! \cond */
template<typename T>
struct strip_reference<T&>
	{
	typedef T type;
	};
/*! \endcond */

/*! \brief Meta function used to strip const from type.
 *
 * \tparam T - type to strip const from.
 */
template<typename T>
struct strip_const
	{
	typedef T type;
	};

/*! \cond */
template<typename T>
struct strip_const<T const>
	{
	typedef T type;
	};
/*! \endcond */

/*! \brief Meta function used to add const to a type.
 *
 * Add const to at type under condition that this type is not a reference.
 *
 * \tparam T - type to strip const from.
 */
template<typename T>
struct add_const_if_not_reference
	{
	typedef T const type;
	};

/*! \cond */
template<typename T>
struct add_const_if_not_reference<T&>
	{
	typedef T type;
	};
/*! \endcond */

/*! \brief Copy constness (or lack of it) from on type to another.
 *
 * \tparam source - get constness from this type.
 * \tparam destination - apply constness to this type.
 */
template<typename source, typename destination>
struct copy_const
	{
	typedef destination type;
	};

/*! \cond */
template<typename source, typename destination>
struct copy_const<source const, destination>
	{
	typedef destination const type;
	};
/*! \endcond */

}

template<int long unsigned const input>
struct binary
	{
	static int long unsigned const value = ( binary<input / 10>::value << 1 ) + ( input % 10 );
	};

template<>
struct binary<0>
	{
	static int long unsigned const value = 0;
	};

template<int long unsigned const input>
struct obinary
	{
	static int long unsigned const value = ( obinary<(input >> 3)>::value << 1 ) + ( input & 7 );
	};

template<>
struct obinary<0>
	{
	static int long unsigned const value = 0;
	};

template<int long unsigned const base, int long unsigned const exponent, int long unsigned const helper = 1>
struct power
	{
	static int long unsigned const value = power<base, exponent - 1, helper * base>::value;
	};

template<int long unsigned const base, int long unsigned const helper>
struct power<base,0,helper>
	{
	static int long unsigned const value = helper;
	};

template<typename tType>
tType min( tType left, tType right )
	{
	return ( left < right ? left : right );
	}

template<typename tType>
tType max( tType left, tType right )
	{
	return ( left >= right ? left : right );
	}

template<typename tType>
tType abs( tType val )
	{
	return ( val >= 0 ? val : - val );
	}

template<typename tType>
bool less( tType const& a_rtLeft, tType const& a_rtRight )
	{
	return ( a_rtLeft < a_rtRight );
	}

template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& );

template<typename tType>
inline tType const operator | ( tType const& left,
		tType const& right )
	{
	return ( static_cast<tType>( static_cast<int long unsigned>( left )
				| static_cast<int long unsigned>( right ) ) );
	}
template<typename tType>
inline tType& operator |= ( tType& left, tType const& right )
	{
	left = static_cast<tType>( static_cast<int long unsigned>( left )
			| static_cast<int long unsigned>( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator & ( tType const& left,
		tType const& right )
	{
	return ( static_cast<tType>( static_cast<int long unsigned>( left )
				& static_cast<int long unsigned>( right ) ) );
	}
template<typename tType>
inline tType& operator &= ( tType& left, tType const& right )
	{
	left = static_cast<tType>( static_cast<int long unsigned>( left )
			& static_cast<int long unsigned>( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator ^ ( tType const& left,
		tType const& right )
	{
	return ( static_cast<tType>( static_cast<int long unsigned>( left )
				^ static_cast<int long unsigned>( right ) ) );
	}
template<typename tType>
inline tType& operator ^= ( tType& left, tType const& right )
	{
	left = static_cast<tType>( static_cast<int long unsigned>( left )
			^ static_cast<int long unsigned>( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator ~ ( tType const& e )
	{
	return ( static_cast<tType>( ~ static_cast<int long unsigned>( e ) ) );
	}

/* those types definitions were in hinfo.h but this file (hexception.h)
 * is included into more files, we assume that sizeof ( int ) >= 4 */

typedef enum
	{
	D_VOID        = 0x0000,
	D_BOOL        = 0x0001,
	D_CHAR        = 0x0002,
	D_INT_SHORT   = 0x0004,
	D_INT         = 0x0008,
	D_INT_LONG    = 0x0010,
	D_FLOAT       = 0x0020,
	D_DOUBLE      = 0x0040,
	D_DOUBLE_LONG = 0x0080,
	D_VOID_PTR    = 0x0100,
	D_CHAR_PTR    = 0x0280,
	D_HSTRING     = 0x0400,
	D_HNUMBER     = 0x0800,
	D_HINFO       = 0x1000,
	D_HHASHMAP    = 0x2000,
	D_HLIST       = 0x4000,
	D_HTIME       = 0x8000,
	D_MASK        = 0xffff
	} type_t;

typedef char unsigned u8_t;
typedef int short unsigned u16_t;
typedef int unsigned u32_t;
#if 0
typedef int long unsigned u64_t;
#endif
typedef char i8_t;
typedef int short i16_t;
typedef int i32_t;
#if 0
typedef int long i64_t;
#endif

template<typename tType>
tType& clone( tType& object )
	{
	typedef typeof( *object ) ttType;
	return ( tType( new ttType( *object ) ) );
	}

class HNonCopyable
	{
public:
	HNonCopyable( void ) {}
	virtual ~HNonCopyable( void ) {}
private:
	HNonCopyable( HNonCopyable const& );
	HNonCopyable& operator = ( HNonCopyable const& );
	};

char const* error_message( int );

}

#endif /* not __YAAL_HCORE_BASE_H */

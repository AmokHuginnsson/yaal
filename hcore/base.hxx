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
 */

#ifndef YAAL_HCORE_BASE_HXX_INCLUDED
#define YAAL_HCORE_BASE_HXX_INCLUDED

#include "hcore/hexception.hxx"
#include "hcore/compat.hxx"

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

/*! \cond */
template<bool> struct static_assert_failure;
/*! \endcond */
/*! \brief Static assert.
 *
 * If static condition that is specified as a parameter is not met
 * then compilation of an unit fails.
 */
template<> struct static_assert_failure<true> { enum { value = 1 }; };
#define STATIC_ASSERT( condition ) { typedef char SAF##__LINE__[ static_assert_failure<( condition )>::value ]; }

#define _(string) gettext (string)

#define M_VCSID(id) namespace { static char const* wrapper_VCSID( void ) { wrapper_VCSID(); return id; } }
#define M_VCSTID(id) namespace { static char const* wrapper_VCSTID( void ) { wrapper_VCSTID(); return id; } }
#define M_THROW( msg, e_no ) yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, msg, e_no )
#define M_PROLOG try{
#define M_EPILOG } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); throw; }
#define M_FINAL } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); e.print_error( true ); } catch ( yaal::hcore::HFailedAssertion& ) { exit( -1 ); } catch ( int const& retVal ) { return ( retVal ); }
#define M_ENSURE( condition ) do { if ( ! ( condition ) ){ yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno, error_message( errno ) ); } } while ( 0 )
#ifndef NDEBUG
#	define M_ASSERT( condition ) do { if ( ! ( condition ) )yaal::hcore::HException::failed_assert( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition ); } while ( 0 )
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

/*! \brief Meta-function functor for binding given invariable argument as second argument to any function.
 *
 * Invariable argument may be bound to meta-function functor as well as old style C free standing function.
 *
 * Type descriptions:
 *
 * \tparam function_t - type of function to be bound.
 * \tparam value_t - type of invariable argument to be bound.
 */
template<typename function_t, typename value_t>
class HBinder
	{
	template<typename func_t, typename arga_t, typename argb_t>
	struct type_of
		{
		static func_t _type_provider;
		typedef __decltype( _type_provider( arga_t(), argb_t() ) ) type;
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

/*! \brief Meta-data definition for unary function functors.
 *
 * Type descriptions:
 *
 * \tparam res_t - result type of unary function.
 * \tparam arg_t - argument type of unary function.
 */
template<typename res_t, typename arg_t>
struct unary_function
	{
	typedef res_t result_t;
	typedef arg_t argument_t;
	};

/*! \brief Meta-data definition for binary function functors.
 *
 * Type descriptions:
 *
 * \tparam res_t - result type of binary function.
 * \tparam arg1st_t - first argument type of binary function.
 * \tparam arg2nd_t - second argument type of binary function.
 */
template<typename res_t, typename arg1st_t, typename arg2nd_t>
struct binary_function
	{
	typedef res_t result_t;
	typedef arg1st_t argument1st_t;
	typedef arg2nd_t argument2nd_t;
	};

/*! \brief Meta-function functor for binding given invariable argument as second argument.
 *
 * Type descriptions:
 *
 * \tparam result_t - result type of resulting unary function.
 * \tparam argument2nd_t - type of invariant argument.
 */
template<typename operation_t>
struct binder1st : public unary_function<typename operation_t::result_t, typename operation_t::argument2nd_t>
	{
	typename operation_t::argument2nd_t const _bound;
	binder1st( typename operation_t::argument2nd_t const& bound ) : _bound( bound ) {}
	typename operation_t::result_t operator()( typename operation_t::argument1st_t const& arg )
		{ return ( operation_t( arg, _bound ) ); }
	};

template<typename iterator_t, typename call_t>
void for_each( iterator_t it, iterator_t const& end, call_t CALL )
	{
	for ( ; it != end; ++ it )
		CALL( *it );
	}

template<typename src_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ dst )
		*dst = op( *it );
	}

template<typename src_iter_t, typename arg_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, arg_iter_t arg, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ arg, ++ dst )
		*dst = op( *arg, *it );
	}

template<typename src_it_t, typename dst_it_t>
void copy( src_it_t src, src_it_t const& end, dst_it_t dst )
	{
	for ( ; src != end; ++ src, ++ dst )
		*dst = *src;
	return;
	}

template<typename iter1_t, typename iter2_t>
bool equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
	}

template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t merge( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	for ( ; it1 != end1; ++ it1, ++ out )
		{
		for ( ; ( it2 != end2 ) && ( *it2 < *it1 ); ++ it2, ++ out )
			*out = *it2;
		*out = *it1;
		}
	for ( ; it2 != end2; ++ it2, ++ out )
		*out = *it2;
	return ( out );
	}

template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t set_union( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	while ( ( it1 != end1 ) || ( it2 != end2 ) )
		{
		if ( it1 != end1 )
			{
			if ( it2 != end2 )
				{
				if ( *it1 < *it2 )
					{
					*out = *it1;
					++ it1;
					}
				else if ( *it2 < *it1 )
					{
					*out = *it2;
					++ it2;
					}
				else
					{
					*out = *it1;
					++ it1;
					++ it2;
					}
				}
			else
				{
				*out = *it1;
				++ it1;
				}
			}
		else if ( it2 != end2 )
			{
			*out = *it2;
			++ it2;
			}
		++ out;
		}
	return ( out );
	}

template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t set_intersection( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ); ++ it1 )
		{
		for ( ; ( it2 != end2 ) && ( *it2 < *it1 ); ++ it2 )
			;
		if ( ( it2 != end2 ) && ! ( *it1 < *it2 ) )
			{
			*out = *it1;
			++ out;
			++ it2;
			}
		}
	return ( out );
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

template<typename iter_t>
int long distance( iter_t first, iter_t last )
	{
	int long dist = 0;
	while ( first != last )
		++ first, ++ dist;
	return ( dist );
	}

template<typename iter_t>
void advance( iter_t& it, int long const& dist )
	{
	for ( int long i = 0; i < dist; ++ i, ++ it )
		;
	}

/*! \brief Type trait alteration utilities.
 */
namespace trait
{

/*! \brief Meta function used to strip reference from type.
 *
 * \tparam T - type to strip reference from.
 * \retval type - a type T without reference trait.
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

/*! \brief Meta function used to strip one level of pointer from type.
 *
 * \tparam T - type to strip pointer from.
 * \retval type - stripped new type 
 */
template<typename T>
struct strip_pointer
	{
	typedef T type;
	};

/*! \cond */
template<typename T>
struct strip_pointer<T*>
	{
	typedef T type;
	};
/*! \endcond */

/*! \brief Meta function used to strip const from type.
 *
 * \tparam T - type to strip const from.
 * \retval type - a type without constness trait.
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
 * \retval type - a type with constness added (if original type was not a reference).
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
 * \retval type - a destination type with constness of source type.
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

/*! \brief A reference type wrapper.
 * 
 * Pass arguments by reference instead of by value with this trait.
 *
 * \tparam basic_t - type to be wrapper as reference.
 */
template<typename basic_t>
class reference
	{
	basic_t* _ref;
public:
	explicit reference( basic_t& obj ) : _ref( &obj ) {}
	operator basic_t& ( void ) const
		{ return ( *_ref ); }
	};

}

template<typename basic_t>
trait::reference<basic_t> ref( basic_t& obj )
	{
	return ( trait::reference<basic_t>( obj ) );
	}

template<typename basic_t>
trait::reference<basic_t const> cref( basic_t const& obj )
	{
	return ( trait::reference<basic_t const>( obj ) );
	}

/*! \brief Staticaly calculate maximul of a set.
 */
template<int long a0, int long a1,
	int long a2 = LONG_MIN, int long a3 = LONG_MIN,
	int long a4 = LONG_MIN, int long a5 = LONG_MIN,
	int long a6 = LONG_MIN, int long a7 = LONG_MIN,
	int long a8 = LONG_MIN, int long a9 = LONG_MIN>
struct static_max
	{
	static int long const b0 = a0 > a1 ? a0 : a1;
	static int long const b1 = b0 > a2 ? b0 : a2;
	static int long const b2 = b1 > a3 ? b1 : a3;
	static int long const b3 = b2 > a4 ? b2 : a4;
	static int long const b4 = b3 > a5 ? b3 : a5;
	static int long const b5 = b4 > a6 ? b4 : a6;
	static int long const b6 = b5 > a7 ? b5 : a7;
	static int long const b7 = b6 > a8 ? b6 : a8;
	static int long const value = b7 > a9 ? b7 : a9;
	};

/*! \brief Meta function, tells if two types are the same.
 *
 * \tparam T1 - first of two types to compare.
 * \tparam T2 - second of two types.
 * \retval value - 1 is two types are in fact the same, 0 otherwise.
 */
template<typename T1, typename T2>
struct same_type
	{
	static int const value = 0;
	};

/* \cond */
template<typename T1>
struct same_type<T1, T1>
	{
	static int const value = 1;
	};
/* \endcond */

/*! \brief Search for given type in a type list.
 *
 * \tparam t0_t, t1_t, ..., t9_t - list of types to search.
 * \tparam tType - look for this type in given type set.
 * \retval value - index of given type in given list of types, -1 if no matching type had been found.
 */
template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t, typename tType>
struct find_type
	{
	static int const value = -1 + same_type<t0_t, tType>::value
		+ 1 * same_type<t1_t, tType>::value
		+ 2 * same_type<t2_t, tType>::value
		+ 3 * same_type<t3_t, tType>::value
		+ 4 * same_type<t4_t, tType>::value
		+ 5 * same_type<t5_t, tType>::value
		+ 6 * same_type<t6_t, tType>::value
		+ 7 * same_type<t7_t, tType>::value
		+ 8 * same_type<t8_t, tType>::value
		+ 9 * same_type<t9_t, tType>::value;
	};

/*! \brief Simulate binary literal.
 *
 * \tparam input - a number in decimal form consisting only 0s and 1s.
 * \retval value - a number of value of binary interpretation of input.
 */
template<int long unsigned const input>
struct binary
	{
	static int long unsigned const value = ( binary<input / 10>::value << 1 ) + ( input % 10 );
	};

/*! \cond */
template<>
struct binary<0>
	{
	static int long unsigned const value = 0;
	};
/*! \endcond */

/*! \brief Simulate binary literal.
 *
 * \tparam input - a number in octal form consisting only 0s and 1s.
 * \retval value - a number of value of binary interpretation of input.
 */
template<int long unsigned const input>
struct obinary
	{
	static int long unsigned const value = ( obinary<(input >> 3)>::value << 1 ) + ( input & 7 );
	};

/*! \cond */
template<>
struct obinary<0>
	{
	static int long unsigned const value = 0;
	};
/*! \endcond */

/*! \brief Compile time power finction calculator.
 *
 * \tparam base - base of power function.
 * \tparam exponent - exponent of power function.
 * \retval value - base**exponent.
 */
template<int long unsigned const base, int long unsigned const exponent, int long unsigned const helper = 1>
struct power
	{
	static int long unsigned const value = power<base, exponent - 1, helper * base>::value;
	};

/*! \cond */
template<int long unsigned const base, int long unsigned const helper>
struct power<base,0,helper>
	{
	static int long unsigned const value = helper;
	};
/*! \endcond */

template<typename tType>
inline tType min( tType const& left, tType const& right )
	{
	return ( left < right ? left : right );
	}

template<typename tType>
inline tType max( tType const& left, tType const& right )
	{
	return ( left >= right ? left : right );
	}

template<typename tType>
inline tType abs( tType const& val )
	{
	return ( val >= 0 ? val : - val );
	}

/*! \brief Meta function functor plus operator.
 *
 * \tparam tType - type of added objects.
 */
template<typename tType>
struct plus : public binary_function<tType, tType, tType>
	{
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a + b ); }
	};

/*! \brief Meta function function minus operator.
 *
 * \tparam tType - type of substracted objects.
 */
template<typename tType>
struct minus : public binary_function<tType, tType, tType>
	{
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a + b ); }
	};

/*! \brief A functor performing multiplication operation.
 *
 * \tparam tType - type of multiplied objects.
 */
template<typename tType>
struct multiplies : public binary_function<tType, tType, tType>
	{
	/*! \brief Functor executor.
	 *
	 * \param a - first factor of multiplication.
	 * \param b - second factor of multiplication.
	 * \return result of multiplication a times b.
	 */
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a * b ); }
	};

/*! \brief A functor performing division operation.
 *
 * \tparam tType - type of divided/ing objects.
 */
template<typename tType>
struct divides : public binary_function<tType, tType, tType>
	{
	/*! \brief Functor executor.
	 *
	 * \param a - dividend part of division.
	 * \param b - divisor part of division.
	 * \return quotient of a divided by b.
	 */
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a / b ); }
	};

template<typename tType>
bool less( tType const& a_rtLeft, tType const& a_rtRight )
	{
	return ( a_rtLeft < a_rtRight );
	}

template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& );

template<typename tType>
inline tType operator | ( tType const& left,
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
inline tType operator & ( tType const& left,
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
inline tType operator ^ ( tType const& left,
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
inline tType operator ~ ( tType const& e )
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
	typedef __decltype( *object ) ttType;
	return ( tType( new ttType( *object ) ) );
	}

/*! \brief Interface preventing copying of objects.
 */
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

#endif /* not YAAL_HCORE_BASE_HXX_INCLUDED */


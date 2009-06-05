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
#define YAAL_HCORE_BASE_HXX_INCLUDED

#include "hcore/hexception.hxx"
#include "hcore/hstrongenum.hxx"
#include "hcore/trait.hxx"
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
/*! \brief Perform static (compile time) code check.
 *
 * \param condition - condition to be check during compilation.
 *
 * \post Unmet condition will make compilation fail on line where STATIC_ASSERT is used.
 */
#define STATIC_ASSERT( condition ) { typedef char SAF##__LINE__[ static_assert_failure<( condition )>::value ]; }

/*! \brief gettext library API convenience macro.
 */
#define _(string) gettext( string )

/*! \cond */
#define M_CONCAT_REAL( a, b ) a ## b
/*! \endcond */
/*! \brief Create uniqe C++ identifier from two other identifier.
 *
 * \param a - first part of identifier.
 * \param b - second part of identifier (usually __LINE__ or __COUNTER__).
 * \return EVAL(a)EVAL(b) - where EVAL(x) is x preprocessed by CPP.
 */
#define M_CONCAT( a, b ) M_CONCAT_REAL( a, b )
/*! \brief Create object file identifier for ident(1) program.
 */
#define M_VCSID(id) namespace { static char const* M_CONCAT(wrapper_VCSID, __LINE__)( void ) { M_CONCAT( wrapper_VCSID, __LINE__)(); return ( id ); } }
/*! \brief Throw auto generated exception.
 *
 * \param msg - message for exception.
 * \param e_no - error code for exception.
 */
#define M_THROW( msg, e_no ) yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, msg, e_no )
/*! \brief First statement of every exception guarder function/method.
 */
#define M_PROLOG try{
/*! \brief Last statement of every exception guarder function/method.
 */
#define M_EPILOG } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); throw; }
/*! \brief Last statement in <tt>int main( int, char** )</tt>.
 */
#define M_FINAL } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); e.print_error( true ); } catch ( yaal::hcore::HFailedAssertion& ) { exit( -1 ); } catch ( int const& retVal ) { return ( retVal ); }
/*! \brief Throw HExceptionT<> is condition is not met.
 *
 * Use this macro to test return status of low-level system function calls.
 *
 * \param condition - condition to be tested.
 */
#define M_ENSURE( condition ) do { if ( ! ( condition ) ){ yaal::hcore::throw_exception<self_t>( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno, error_message( errno ) ); } } while ( 0 )
#ifndef NDEBUG
/*! \brief Run-time assertion tester.
 *
 * This macro has meaning only in debug build.
 *
 * \param condition - condition to be tested.
 * \post Normal progam flow is continued only if condition is met, otherwise HFailedAssertion exception is thrown.
 */
#	define M_ASSERT( condition ) do { if ( ! ( condition ) )yaal::hcore::HException::failed_assert( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition ); } while ( 0 )
#else /* NDEBUG */
#	define M_ASSERT( c ) /**/
#endif /* not NDEBUG */

/*! \brief Swap contents of two variables.
 *
 * \param left - first variable to be swapped.
 * \param right - second variable to be swapped.
 *
 * \post After the call left holds value of right from before call, and right holds value of left from before call.
 */
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

/*! \brief Convenience function to construct HBinder<> object.
 *
 * \param func - function to bind argument to.
 * \param value - a bound value.
 * \return HBinder<>, a function object that encapsulates func function and its bound value.
 */
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

/*! \brief For each element in range pass its value into function.
 *
 * \param it - begining of the range.
 * \param end - one past the end of range.
 * \param CALL - invoke this function and pass values from range to it.
 */
template<typename iterator_t, typename call_t>
void for_each( iterator_t it, iterator_t const& end, call_t CALL )
	{
	for ( ; it != end; ++ it )
		CALL( *it );
	}

/*! \brief Replace elements in destination container by transformed elements from source range.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param dst - begining of destination container.
 * \param op - transforming operation, an unary function.
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ dst )
		*dst = op( *it );
	}

/*! \brief Replace elements in destination container by elements resulting from transformation of elements from source range and another container.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param arg - begining of second source container.
 * \param dst - begining of destination container.
 * \param op - transforming operation, a binary function.
 */
template<typename src_iter_t, typename arg_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, arg_iter_t arg, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ arg, ++ dst )
		*dst = op( *arg, *it );
	}

/*! \brief Copy range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param end - one past the end of source range of elements.
 * \param dst - begining of destination range.
 */
template<typename src_it_t, typename dst_it_t>
void copy( src_it_t src, src_it_t const& end, dst_it_t dst )
	{
	for ( ; src != end; ++ src, ++ dst )
		*dst = *src;
	return;
	}

/*! \brief Copy given number of elements from range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param count - number of elements to copy.
 * \param dst - begining of destination range.
 */
template<typename src_it_t, typename dst_it_t>
void copy_n( src_it_t src, int long const& count, dst_it_t dst )
	{
	for ( int long i = 0; i < count; ++ i, ++ src, ++ dst )
		*dst = *src;
	return;
	}

/*! \brief Checks if two ranges are same size and have same set of values.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past last element of second range.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t>
bool equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
	}

/*! \brief Joins two sorted ranges of elements into one sorted range of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
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

/*! \brief Create union of two sorted ranges of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
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

/*! \brief Create intersection of two sorted ranges of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
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

/*! \brief Swap contents (values) of two ranges of elements.
 *
 * \param first - begining of first range.
 * \param end - one past the end of first range.
 * \param second - begining of second range.
 */
template<typename first_it_t, typename second_it_t>
void swap_ranges( first_it_t first, first_it_t const& end, second_it_t second )
	{
	for ( ; first != end; ++ first, ++ second )
		swap( *first, *second ) ;
	return;
	}

/*! \brief Fill range of elements with given value.
 *
 * \param it - begining of range to fill.
 * \param end - one past the end of range.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
void fill( dst_it_t it, dst_it_t end, filler_t const& filler )
	{
	for ( ; it != end; ++ it )
		*it = filler;
	return;
	}

/*! \brief Fill specified amount of elements with given value.
 *
 * \param it - begining of range to fill.
 * \param count - amount of elements to set.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
void fill_n( dst_it_t it, int long const& count, filler_t const& filler )
	{
	for ( int long i = 0; i < count; ++ i, ++ it )
		*it = filler;
	return;
	}

/*! \brief Fill specified range with run-time generared values.
 *
 * \param it - begining of the range.
 * \param end - one past last element in range.
 * \param generator - function object that creates new values for range.
 */
template<typename iterator_t, typename generator_t>
void generate( iterator_t it, iterator_t const& end, generator_t generator )
	{
	for ( ; it != end; ++ it )
		*it = generator();
	return;
	}

/*! \brief Calculate sum of elements in range.
 *
 * \param it - begining of range of eleemnts to summed up.
 * \param end - one past last element of range to be summed up.
 * \param ret - starting value for sum operation.
 * \return sum of all elements in [it, end) + ret.
 */
template<typename iterator_t, typename return_t>
return_t accumulate( iterator_t it, iterator_t end, return_t ret )
	{
	for ( ; it != end; ++ it )
		ret += *it;
	return ( ret );
	}

/*! \brief Calculate distance between two iterators.
 *
 * \param first - iterator.
 * \param last - iterator.
 * \return last - first.
 */
template<typename iter_t>
int long distance( iter_t first, iter_t last )
	{
	int long dist = 0;
	while ( first != last )
		++ first, ++ dist;
	return ( dist );
	}

/*! \brief Move iterator forward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
void advance( iter_t& it, int long const& dist )
	{
	for ( int long i = 0; i < dist; ++ i, ++ it )
		;
	}

/*! \brief Convenience function, returns trait::reference<> object.
 *
 * \param obj - object that trait::reference<> shall be based on.
 * \return trait::reference<> wrapper of \e obj object.
 */
template<typename basic_t>
trait::reference<basic_t> ref( basic_t& obj )
	{
	return ( trait::reference<basic_t>( obj ) );
	}

/*! \brief Convenience function, returns trait::reference<const> object.
 *
 * \param obj - object that trait::reference<const> shall be based on.
 * \return trait::reference<const> wrapper of \e obj object.
 */
template<typename basic_t>
trait::reference<basic_t const> cref( basic_t const& obj )
	{
	return ( trait::reference<basic_t const>( obj ) );
	}

/*! \brief Staticaly calculate maximul of a set.
 *
 * \tparam a0, a1, ..., a9 - list of integers to find maximum.
 * \retval value - maximum integer from given set.
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

/*! \brief Get smaller of two values.
 *
 * \param left - first value to be considered as smaller.
 * \param right -  second value to be considered as smaller.
 * \return Smaller of two values.
 */
template<typename tType>
inline tType min( tType const& left, tType const& right )
	{
	return ( left < right ? left : right );
	}

/*! \brief Get bigger of two values.
 *
 * \param left - first value to be considered as bigger.
 * \param right -  second value to be considered as bigger.
 * \return Bigger of two values.
 */
template<typename tType>
inline tType max( tType const& left, tType const& right )
	{
	return ( left >= right ? left : right );
	}

/*! \brief Calculate absolute value of a number.
 *
 * \param val - a number which absolute value shall be calculated.
 * \return |val|
 */
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

/*! \brief Generic less (object ordering) operator.
 *
 * \param a_rtLeft - first of the objects to compare.
 * \param a_rtRight - second of the objects to compare.
 * \return True iff (if and only if) a_rtLeft < a_rtRight - first objects is lesser than second object.
 */
template<typename tType>
bool less( tType const& a_rtLeft, tType const& a_rtRight )
	{
	return ( a_rtLeft < a_rtRight );
	}

/*! \brief Convert between distinct datatypes.
 *
 * \tparam to_t - destination type.
 * \param val - value to be converted to destination type.
 * \return val equvalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& val );

/*! \brief POD type symbols. (Some non-POD yaal types included too).
 */
struct TYPE
	{
	typedef enum
		{
		UNKNOWN     = 0x0000,
		BOOL        = 0x0001,
		CHAR        = 0x0002,
		INT_SHORT   = 0x0004,
		INT         = 0x0008,
		INT_LONG    = 0x0010,
		FLOAT       = 0x0020,
		DOUBLE      = 0x0040,
		DOUBLE_LONG = 0x0080,
		VOID_PTR    = 0x0100,
		CHAR_PTR    = 0x0280,
		HSTRING     = 0x0400,
		HNUMBER     = 0x0800,
		HINFO       = 0x1000,
		HTIME       = 0x2000,
		CONTAINER   = 0x4000,
		VOID        = 0x8000
		} enum_t;
	template<typename tType>
	struct symbolic;
	};
typedef yaal::hcore::HStrongEnum<TYPE> type_t; /*!< Strong enumeration of PODs. */
template<> struct TYPE::symbolic<void> { static enum_t const type = VOID; };
template<> struct TYPE::symbolic<bool> { static enum_t const type = BOOL; };
template<> struct TYPE::symbolic<char> { static enum_t const type = CHAR; };
template<> struct TYPE::symbolic<int short> { static enum_t const type = INT_SHORT; };
template<> struct TYPE::symbolic<int> { static enum_t const type = INT; };
template<> struct TYPE::symbolic<int long> { static enum_t const type = INT_LONG; };
template<> struct TYPE::symbolic<float> { static enum_t const type = FLOAT; };
template<> struct TYPE::symbolic<double> { static enum_t const type = DOUBLE; };
template<> struct TYPE::symbolic<double long> { static enum_t const type = DOUBLE_LONG; };
template<> struct TYPE::symbolic<void*> { static enum_t const type = VOID_PTR; };
template<> struct TYPE::symbolic<char*> { static enum_t const type = CHAR_PTR; };
template<> struct TYPE::symbolic<yaal::hcore::HString> { static enum_t const type = HSTRING; };

template<typename tType>
struct TYPE::symbolic
	{ static enum_t const type = UNKNOWN; };

typedef char unsigned u8_t; /*!< 8 bit unsigned integer. */
typedef int short unsigned u16_t; /*!< 16 bit unsigned integer. */
typedef int unsigned u32_t; /*!< 32 bit unsigned integer. */
#if 0
typedef int long unsigned u64_t; /*!< 64 bit unsigned integer. */
#endif
typedef char signed i8_t; /*!< 8 bit signed integer. */
typedef int short signed i16_t; /*!< 16 bit signed integer. */
typedef int signed i32_t; /*!< 32 bit signed integer. */
#if 0
typedef int long signed i64_t; /*!< 64 bit signed integer. */
#endif

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

/*! \brief Generate error message (possibly human readable) based on error code.
 *
 * \param code - error code to describe.
 * \return Message string describing error of given error code.
 */
char const* error_message( int const& code );

}

#endif /* not YAAL_HCORE_BASE_HXX_INCLUDED */


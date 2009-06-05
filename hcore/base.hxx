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


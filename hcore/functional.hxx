/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	functional.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/functional.hxx
 * \brief Higher order programming basics.
 */

#ifndef YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED
#define YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED

#include "hcore/trait.hxx"

namespace yaal
{

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
	value_t _value;
public:
	HBinder( function_t, value_t );
	template<typename tType>
	typename type_of<function_t, value_t, tType>::type operator()( tType value ) const
		{ return ( CALLER( value, _value ) ); }
	};

template<typename function_t, typename value_t>
HBinder<function_t, value_t>::HBinder( function_t func, value_t value )
	: CALLER( func ), _value( value )
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

}

#endif /* not YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED */


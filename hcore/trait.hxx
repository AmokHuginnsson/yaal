/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	trait.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/trait.hxx
 * \brief Type trait calculus library.
 */

#ifndef YAAL_HCORE_TRAIT_HXX_INCLUDED
#define YAAL_HCORE_TRAIT_HXX_INCLUDED

namespace yaal
{

/*! \brief Type trait alteration utilities.
 */
namespace trait
{

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

/*! \brief Perform compile time ternary operator on types.
 *
 * \tparam condition - staticly checkable condition that tells which type will be used.
 * \tparam type_for_true - type to be used if condition is true.
 * \tparam type_for_false - type to be used if condition is false.
 * \retval type - conditional type.
 */
template<bool const condition, typename type_for_true, typename type_for_false>
struct ternary;

/* \cond */
template<typename type_for_true, typename type_for_false>
struct ternary<true, type_for_true, type_for_false>
	{
	typedef type_for_true type;
	};

template<typename type_for_true, typename type_for_false>
struct ternary<false, type_for_true, type_for_false>
	{
	typedef type_for_false type;
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
	static int const value = -1
		+ 1 * same_type<t0_t, tType>::value
		+ 2 * same_type<t1_t, tType>::value
		+ 3 * same_type<t2_t, tType>::value
		+ 4 * same_type<t3_t, tType>::value
		+ 5 * same_type<t4_t, tType>::value
		+ 6 * same_type<t5_t, tType>::value
		+ 7 * same_type<t6_t, tType>::value
		+ 8 * same_type<t7_t, tType>::value
		+ 9 * same_type<t8_t, tType>::value
		+ 10 * same_type<t9_t, tType>::value;
	};

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

/*! \brief Test is type is a reference type.
 *
 * \tparam T - type to check for reference triat.
 * \retval value - true iff T is a reference type.
 */
template<typename T>
struct is_reference
	{
	static bool const value = false;
	};

/*! \cond */
template<typename T>
struct is_reference<T&>
	{
	static bool const value = true;
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
	basic_t& operator->( void )
		{ return ( *_ref ); }
	template<typename basic_assgnable_t>
	basic_t& operator = ( basic_assgnable_t const& v )
		{
		*_ref = v;
		return ( *_ref );
		}
	};

template<typename T>
struct is_member
	{ static bool const value = false; };

template<typename return_t, typename class_t>
struct is_member<return_t ( class_t::* )( void )>
	{ static bool const value = true; };
template<typename return_t, typename class_t>
struct is_member<return_t ( class_t::* )( void ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t>
struct is_member<return_t ( class_t::* )( void ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t>
struct is_member<return_t ( class_t::* )( void ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t>
struct is_member<return_t ( class_t::* )( a0_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t>
struct is_member<return_t ( class_t::* )( a0_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t>
struct is_member<return_t ( class_t::* )( a0_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t>
struct is_member<return_t ( class_t::* )( a0_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const volatile>
	{ static bool const value = true; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) volatile>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct is_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const volatile>
	{ static bool const value = true; };

template<typename T>
struct return_type;

template<typename return_t, typename class_t>
struct return_type<return_t ( class_t::* )( void )>
	{ typedef return_t type; };
template<typename return_t, typename class_t>
struct return_type<return_t ( class_t::* )( void ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t>
struct return_type<return_t ( class_t::* )( void ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t>
struct return_type<return_t ( class_t::* )( void ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t>
struct return_type<return_t ( class_t::* )( a0_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct return_type<return_t ( class_t::* )( a0_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct return_type<return_t ( class_t::* )( a0_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct return_type<return_t ( class_t::* )( a0_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const volatile>
	{ typedef return_t type; };

template<typename return_t>
struct return_type<return_t ( * )( void )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t>
struct return_type<return_t ( * )( a0_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t>
struct return_type<return_t ( * )( a0_t, a1_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ typedef return_t type; };

template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ typedef return_t type; };

}

}

#endif /* not YAAL_HCORE_TRAIT_HXX_INCLUDED */


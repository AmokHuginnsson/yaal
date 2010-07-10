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

#include <cstddef>

#include "hcore/numeric.hxx"

namespace yaal
{

/*! \brief Type trait alteration utilities.
 */
namespace trait
{

/*! \brief Type that represents \e true in type calculus context.
 */
class true_type { char x; true_type( void ); true_type( true_type const& ); true_type& operator = ( true_type const& ); };
/*! \brief Type that represents \e false in type calculus context.
 */
class false_type { char x[100]; false_type(); false_type( false_type const& ); false_type& operator = ( false_type const& ); };
STATIC_ASSERT( sizeof ( true_type ) != sizeof ( false_type ) );

/*! \brief A dummy class.
 *
 * Dummy used for template metaprogramming tricks.
 */
class no_type {};

/*! \brief Meta function, tells if two types are the same.
 *
 * \tparam T1 - first of two types to compare.
 * \tparam T2 - second of two types.
 * \retval value - 1 is two types are in fact the same, 0 otherwise.
 * \retval type - true_type iff types are same.
 */
template<typename T1, typename T2>
struct same_type
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/* \cond */
template<typename T1>
struct same_type<T1, T1>
	{
	static bool const value = true;
	typedef trait::true_type type;
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

/*! \brief Convert value domain boolean into type domain boolean.
 *
 * \tparam value - a boolean value.
 * \retval type - boolean type.
 */
template<bool const value>
struct boolean_type;

/* \cond */
template<>
struct boolean_type<true>
	{
	typedef true_type type;
	};

template<>
struct boolean_type<false>
	{
	typedef false_type type;
	};
/* \endcond */

/*! \brief Convert type domain boolean into value domain boolean.
 *
 * \tparam type - boolean type.
 * \retval value - a boolean value.
 */
template<typename boolean>
struct boolean_value;

/* \cond */
template<>
struct boolean_value<true_type>
	{
	static bool const value = true;
	};

template<>
struct boolean_value<false_type>
	{
	static bool const value = false;
	};
/* \endcond */

/*! \brief Search for given type in a type list.
 *
 * \tparam tType - look for this type in given type set.
 * \tparam t0_t, t1_t, ..., t9_t - list of types to search.
 * \retval value - index of given type in given list of types, -1 if no matching type had been found.
 */
template<typename tType, typename t0_t, typename t1_t = no_type,
	typename t2_t = no_type, typename t3_t = no_type,
	typename t4_t = no_type, typename t5_t = no_type,
	typename t6_t = no_type, typename t7_t = no_type,
	typename t8_t = no_type, typename t9_t = no_type,
	typename t10_t = no_type, typename t11_t = no_type,
	typename t12_t = no_type, typename t13_t = no_type,
	typename t14_t = no_type, typename t15_t = no_type,
	typename t16_t = no_type, typename t17_t = no_type,
	typename t18_t = no_type, typename t19_t = no_type,
	typename t20_t = no_type>
struct find_type
	{
	static int const value = -1
		+ meta::ternary<same_type<tType, no_type>::value, 0, 1 * meta::to_int<same_type<t0_t, tType>::value>::value
		+ 2 * meta::to_int<same_type<t1_t, tType>::value>::value
		+ 3 * meta::to_int<same_type<t2_t, tType>::value>::value
		+ 4 * meta::to_int<same_type<t3_t, tType>::value>::value
		+ 5 * meta::to_int<same_type<t4_t, tType>::value>::value
		+ 6 * meta::to_int<same_type<t5_t, tType>::value>::value
		+ 7 * meta::to_int<same_type<t6_t, tType>::value>::value
		+ 8 * meta::to_int<same_type<t7_t, tType>::value>::value
		+ 9 * meta::to_int<same_type<t8_t, tType>::value>::value
		+ 10 * meta::to_int<same_type<t9_t, tType>::value>::value
		+ 11 * meta::to_int<same_type<t10_t, tType>::value>::value
		+ 12 * meta::to_int<same_type<t11_t, tType>::value>::value
		+ 13 * meta::to_int<same_type<t12_t, tType>::value>::value
		+ 14 * meta::to_int<same_type<t13_t, tType>::value>::value
		+ 15 * meta::to_int<same_type<t14_t, tType>::value>::value
		+ 16 * meta::to_int<same_type<t15_t, tType>::value>::value
		+ 17 * meta::to_int<same_type<t16_t, tType>::value>::value
		+ 18 * meta::to_int<same_type<t17_t, tType>::value>::value
		+ 19 * meta::to_int<same_type<t18_t, tType>::value>::value
		+ 20 * meta::to_int<same_type<t19_t, tType>::value>::value
		+ 21 * meta::to_int<same_type<t20_t, tType>::value>::value>::value;
	};

/*! \brief Get type from given list by index.
 *
 * \tparam index - index of retrieved type.
 * \tparam t0_t, t1_t, ..., t9_t - list of types.
 * \retval type - type from list at given index.
 */
template<int const index, typename t0_t, typename t1_t = no_type,
	typename t2_t = no_type, typename t3_t = no_type,
	typename t4_t = no_type, typename t5_t = no_type,
	typename t6_t = no_type, typename t7_t = no_type,
	typename t8_t = no_type, typename t9_t = no_type,
	typename t10_t = no_type, typename t11_t = no_type,
	typename t12_t = no_type, typename t13_t = no_type,
	typename t14_t = no_type, typename t15_t = no_type,
	typename t16_t = no_type, typename t17_t = no_type,
	typename t18_t = no_type, typename t19_t = no_type,
	typename t20_t = no_type>
struct select_index
	{
	typedef typename ternary<index == 0, t0_t,
					typename ternary<index == 1, t1_t,
					typename ternary<index == 2, t2_t,
					typename ternary<index == 3, t3_t,
					typename ternary<index == 4, t4_t,
					typename ternary<index == 5, t5_t,
					typename ternary<index == 6, t6_t,
					typename ternary<index == 7, t7_t,
					typename ternary<index == 8, t8_t,
					typename ternary<index == 9, t9_t,
					typename ternary<index == 10, t10_t,
					typename ternary<index == 11, t11_t,
					typename ternary<index == 12, t12_t,
					typename ternary<index == 13, t13_t,
					typename ternary<index == 14, t14_t,
					typename ternary<index == 15, t15_t,
					typename ternary<index == 16, t16_t,
					typename ternary<index == 17, t17_t,
					typename ternary<index == 18, t18_t,
					typename ternary<index == 19, t19_t,
					typename ternary<index == 20, t20_t, no_type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type type;
	};

/*! \brief Count occurences of a type.
 *
 * \tparam t0_t, t1_t, ..., t9_t - list of types to search.
 * \tparam tType - look for number of occurences of this type in given type set.
 * \retval value - number of occurences of given type in given list of types.
 */
template<typename tType, typename t0_t, typename t1_t = no_type,
	typename t2_t = no_type, typename t3_t = no_type,
	typename t4_t = no_type, typename t5_t = no_type,
	typename t6_t = no_type, typename t7_t = no_type,
	typename t8_t = no_type, typename t9_t = no_type,
	typename t10_t = no_type, typename t11_t = no_type,
	typename t12_t = no_type, typename t13_t = no_type,
	typename t14_t = no_type, typename t15_t = no_type,
	typename t16_t = no_type, typename t17_t = no_type,
	typename t18_t = no_type, typename t19_t = no_type,
	typename t20_t = no_type>
struct count_type
	{
	static int const value =
		+ meta::to_int<same_type<t0_t, tType>::value>::value
		+ meta::to_int<same_type<t1_t, tType>::value>::value
		+ meta::to_int<same_type<t2_t, tType>::value>::value
		+ meta::to_int<same_type<t3_t, tType>::value>::value
		+ meta::to_int<same_type<t4_t, tType>::value>::value
		+ meta::to_int<same_type<t5_t, tType>::value>::value
		+ meta::to_int<same_type<t6_t, tType>::value>::value
		+ meta::to_int<same_type<t7_t, tType>::value>::value
		+ meta::to_int<same_type<t8_t, tType>::value>::value
		+ meta::to_int<same_type<t9_t, tType>::value>::value
		+ meta::to_int<same_type<t10_t, tType>::value>::value
		+ meta::to_int<same_type<t11_t, tType>::value>::value
		+ meta::to_int<same_type<t12_t, tType>::value>::value
		+ meta::to_int<same_type<t13_t, tType>::value>::value
		+ meta::to_int<same_type<t14_t, tType>::value>::value
		+ meta::to_int<same_type<t15_t, tType>::value>::value
		+ meta::to_int<same_type<t16_t, tType>::value>::value
		+ meta::to_int<same_type<t17_t, tType>::value>::value
		+ meta::to_int<same_type<t18_t, tType>::value>::value
		+ meta::to_int<same_type<t19_t, tType>::value>::value
		+ meta::to_int<same_type<t20_t, tType>::value>::value;
	};
template<typename tType, typename t0_t, typename t1_t,
	typename t2_t, typename t3_t, typename t4_t, typename t5_t,
	typename t6_t, typename t7_t, typename t8_t, typename t9_t,
	typename t10_t, typename t11_t, typename t12_t, typename t13_t,
	typename t14_t, typename t15_t, typename t16_t, typename t17_t,
	typename t18_t, typename t19_t, typename t20_t>
int const count_type<tType, t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::value;

/*! \brief Test if type is a pointer type.
 *
 * \tparam T - type to check for pointer trait.
 * \retval value - true iff T is a pointer type.
 * \retval type - true_type iff T is a pointer type.
 */
template<typename T>
struct is_pointer
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/*! \cond */
template<typename T>
struct is_pointer<T*>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
template<typename T>
struct is_pointer<T const*>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
/*! \endcond */

/*! \brief Test if type is a const type.
 *
 * \tparam T - type to check for constness trait.
 * \retval value - true iff T is a const type.
 * \retval type - true_type iff T is a const type.
 */
template<typename T>
struct is_const
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/*! \cond */
template<typename T>
struct is_const<T const>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
/*! \endcond */

/*! \brief Test if type is a volatile type.
 *
 * \tparam T - type to check for volatileness trait.
 * \retval value - true iff T is a volatile type.
 * \retval type - true_type iff T is a volatile type.
 */
template<typename T>
struct is_volatile
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/*! \cond */
template<typename T>
struct is_volatile<T volatile>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
/*! \endcond */

/*! \brief Test if type is a reference type.
 *
 * \tparam T - type to check for reference trait.
 * \retval value - true iff T is a reference type.
 * \retval type - true_type iff T is a reference type.
 */
template<typename T>
struct is_reference
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/*! \cond */
template<typename T>
struct is_reference<T&>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
template<typename T>
struct is_reference<T const&>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
/*! \endcond */

/*! \brief Meta function used to make reference from type.
 *
 * \tparam T - type to make reference of.
 * \retval type - a type T with reference trait.
 */
template<typename T>
struct make_reference;

/* \cond */
template<>
struct make_reference<void>
	{
	typedef void type;
	};

template<>
struct make_reference<void const>
	{
	typedef void type;
	};

template<typename T>
struct make_reference<T&>
	{
	typedef T& type;
	};

template<typename T>
struct make_reference
	{
	typedef T& type;
	};
/* \endcond */

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

/*! \brief Meta function used to add one level of pointer to a type.
 *
 * \tparam T - type to add pointer to.
 * \retval type - pointer to T type.
 */
template<typename T>
struct make_pointer
	{
	typedef T* type;
	};

/*! \brief Meta function used to strip one level of pointer from type.
 *
 * \tparam T - type to strip pointer from.
 * \retval type - stripped new type.
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

/*! \brief Meta function used to add const to type.
 *
 * \tparam T - type to add const to.
 * \retval type - a type with constness trait.
 */
template<typename T>
struct make_const
	{
	typedef T const type;
	};

/*! \cond */
template<typename T>
struct make_const<T const>
	{
	typedef T const type;
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

template<typename T>
struct make_const_ref_ptr
	{
	typedef typename ternary<is_reference<T>::value,
						typename make_reference<
							typename ternary<is_pointer<typename strip_reference<T>::type>::value,
								typename make_pointer<typename strip_pointer<typename strip_reference<T>::type>::type const>::type,
								typename strip_reference<T>::type const
							>::type
						>::type,
						typename ternary<is_pointer<typename strip_reference<T>::type>::value,
							typename make_pointer<typename strip_pointer<typename strip_reference<T>::type>::type const>::type,
							T const>::type>::type type;
	};

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

/*! \brief Check if given class has been derived from another given class.
 *
 * \tparam derived_t - suspected derived type.
 * \tparam base_t - suspected base type.
 * \retval value - true iff derived_t is derived from base_t.
 */
template<typename derived_t, typename base_t>
struct is_kind_of
	{
	static true_type calc( base_t const* );
	static false_type calc( ... );
	static bool const value = sizeof ( calc( static_cast<derived_t const*>( NULL ) ) ) == sizeof ( true_type );
	};

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

/*! \brief Check if given type is a field type (actually a pointer to a field).
 *
 * \tparam T - type to check for being a field.
 * \retval value - true iff given type is a field type.
 */
template<typename T>
struct is_field
	{ static bool const value = false; };

template<typename class_t, typename T>
struct is_field<T class_t::*>
	{ static bool const value = true; };

/*! \brief Check if given type is a member type.
 *
 * \tparam T - type to check for being a member.
 * \retval value - true iff given type is a member type.
 */
template<typename T>
struct is_member
	{ static bool const value = false; };

/*! \cond */
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

/*! \endcond */

/*! \brief Get return type of member or function.
 *
 * \tparam T - type of member or function.
 * \retval type - return type of given member or function.
 */
template<typename T>
struct return_type
	{ typedef typename T::result_type type; };

/*! \cond */
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
template<typename return_t>
struct return_type<return_t ( * )( void )>
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
template<typename return_t, typename a0_t>
struct return_type<return_t ( * )( a0_t )>
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
template<typename return_t, typename a0_t, typename a1_t>
struct return_type<return_t ( * )( a0_t, a1_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
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
template<typename return_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct return_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ typedef return_t type; };

/*! \brief Get type of an indexed argument of member or function.
 *
 * \tparam T - type of member or function.
 * \tparam no - index of an argument to get type of.
 * \retval type - type of given argument of given member or function.
 */

template<typename T>
struct generic_argument_type
	{ template<int const> struct index { typedef no_type type; }; };

template<typename return_t, typename class_t>
struct generic_argument_type<return_t ( class_t::* )( void )>
	{ template<int const> struct index { typedef no_type type; }; };
template<typename return_t, typename class_t>
struct generic_argument_type<return_t ( class_t::* )( void ) const>
	{ template<int const> struct index { typedef no_type type; }; };
template<typename return_t, typename class_t>
struct generic_argument_type<return_t ( class_t::* )( void ) volatile>
	{ template<int const> struct index { typedef no_type type; }; };
template<typename return_t, typename class_t>
struct generic_argument_type<return_t ( class_t::* )( void ) const volatile>
	{ template<int const> struct index { typedef no_type type; }; };
template<typename return_t>
struct generic_argument_type<return_t ( * )( void )>
	{ template<int const> struct index { typedef no_type type; }; };

template<typename return_t, typename class_t, typename a0_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t>::type type; }; };
template<typename return_t, typename a0_t>
struct generic_argument_type<return_t ( * )( a0_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t, typename a9_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type type; }; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) const>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type type; }; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
struct generic_argument_type<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type type; }; };
template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t, typename a9_t, typename a10_t>
struct generic_argument_type<return_t ( * )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t )>
	{ template<int const no> struct index { typedef typename select_index<no, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type type; }; };

template<typename T>
struct functional_argument_type
	{
	template<typename real_class>
	static true_type has_argument_type( typename strip_reference<typename real_class::argument_type>::type* );
	template<typename real_class>
	static false_type has_argument_type( ... );
	template<typename real_class>
	static true_type has_first_argument_type( typename strip_reference<typename real_class::first_argument_type>::type* );
	template<typename real_class>
	static false_type has_first_argument_type( ... );
	template<typename real_class>
	static true_type has_second_argument_type( typename strip_reference<typename real_class::second_argument_type>::type* );
	template<typename real_class>
	static false_type has_second_argument_type( ... );

	static bool const a0 = ( sizeof ( has_argument_type<T>( 0 ) ) == sizeof ( true_type ) );
	static bool const a1 = ( sizeof ( has_first_argument_type<T>( 0 ) ) == sizeof ( true_type ) );
	static bool const a2 = ( sizeof ( has_second_argument_type<T>( 0 ) ) == sizeof ( true_type ) );

	template<typename U, int const no>
	struct get
		{ typedef no_type type; };

	template<typename U>
	struct get<U, 0>
		{ typedef typename U::argument_type type; };
	template<typename U>
	struct get<U, 1>
		{ typedef typename U::first_argument_type type; };
	template<typename U>
	struct get<U, 2>
		{ typedef typename U::second_argument_type type; };

	template<int const no> struct index
		{
		static int const idx = meta::ternary<( no == 0 ), meta::ternary<a0, 0, meta::ternary<a1, 1, -1>::value>::value,
														meta::ternary<( no == 1 ) && a2, 2,
														-1>::value>::value;
		typedef typename get<T, idx>::type type;
		};
	};

template<typename T, int const no>
struct argument_type
	{
	template<bool const force, typename U>
	struct resolve;

	template<typename U>
	struct resolve<false, U>
		{
		typedef U type;
		};

	template<typename U>
	struct resolve<true, U>
		{
		typedef typename generic_argument_type<T>::template index<no>::type type;
		};

	typedef typename functional_argument_type<T>::template index<no>::type functional_argument_type_t;

	typedef typename resolve<same_type<functional_argument_type_t, no_type>::value, functional_argument_type_t>::type type;
	};

/*! \endcond */

/*! \brief Get class type of given member.
 *
 * \tparam T - type of member or function.
 * \retval type - return type of given member or function.
 */
template<typename T>
struct class_type;

template<typename T>
struct class_type_from_member;
template<typename T>
struct class_type_from_field;

/*! \cond */
template<typename return_t, typename class_t>
struct class_type_from_member<return_t ( class_t::* )( void )>
	{ typedef class_t type; };
template<typename return_t, typename class_t>
struct class_type_from_member<return_t ( class_t::* )( void ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t>
struct class_type_from_member<return_t ( class_t::* )( void ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t>
struct class_type_from_member<return_t ( class_t::* )( void ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const volatile>
	{ typedef class_t type; };

template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t,
	typename a6_t, typename a7_t, typename a8_t, typename a9_t>
struct class_type_from_member<return_t ( class_t::* )( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const volatile>
	{ typedef class_t type; };

template<typename class_t, typename field_t>
struct class_type_from_field<field_t class_t::*>
	{ typedef class_t type; };

template<typename class_t>
struct class_type
	{
	template<typename real_class>
	static true_type has_this_type( typename strip_reference<typename real_class::this_type>::type* );
	template<typename real_class>
	static false_type has_this_type( ... );
	static bool const is_functional = ( sizeof ( has_this_type<class_t>( 0 ) ) == sizeof ( true_type ) );
	template<typename U, int const no>
	struct get
		{ typedef no_type type; };

	template<typename U>
	struct get<U, 1>
		{ typedef typename U::this_type type; };

	template<typename U>
	struct get<U, 2>
		{ typedef typename class_type_from_member<U>::type type; };

	template<typename U>
	struct get<U, 3>
		{ typedef typename class_type_from_field<U>::type type; };

	typedef typename get<class_t,
					meta::ternary<
						is_member<class_t>::value,
						2,
						meta::ternary<
							is_field<class_t>::value,
							3,
							is_functional ? 1 : 0
						>::value
					>::value>::type type;
	};

/*! \endcond */

template<typename field_t>
struct field_type;

template<typename class_t, typename field_t>
struct field_type<field_t class_t::*>
	{
	typedef field_t type;
	};

}

}

#endif /* not YAAL_HCORE_TRAIT_HXX_INCLUDED */


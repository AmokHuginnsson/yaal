/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/trait.hxx
 * \brief Type trait calculus library.
 */

#ifndef YAAL_HCORE_TRAIT_HXX_INCLUDED
#define YAAL_HCORE_TRAIT_HXX_INCLUDED 1

#include <cstddef>

#include "hcore/numeric.hxx"

namespace yaal {

/*! \brief Type trait alteration utilities.
 */
namespace trait {

/*! \brief Type that represents \e true in type calculus context.
 */
class true_type { char x; true_type( void ); true_type( true_type const& ); true_type& operator = ( true_type const& ); };
/*! \brief Type that represents \e false in type calculus context.
 */
class false_type { char x[100]; false_type(); false_type( false_type const& ); false_type& operator = ( false_type const& ); };
static_assert( sizeof ( true_type ) != sizeof ( false_type ), "true_type and false_type are not statically distinguishable" );

/*! \brief A dummy class.
 *
 * Dummy used for template metaprogramming tricks.
 */
class no_type {};

/*! \brief Allow getting sizeof given type on incomplete types.
 *
 * \tparam T - a type to get size of.
 * \retval A size of T type.
 */
template<typename T>
struct delayed_sizeof {
	static int const value = sizeof ( T );
};

/*! \brief Conditionally expose given type.
 *
 * \tparam condition - condition on which given type should be exposed.
 * \tparam T - type to expose if condition is true.
 * \retval type - conditionally T type.
 */
template<bool const condition, typename T = void>
struct enable_if {
	typedef T type;
};

/* \cond */
template<typename T>
struct enable_if<false, T> {
};
/* \endcond */

/*! \brief Unpack first type from parameter pack.
 *
 * \tparam head_t - first type in parameter pack.
 * \tparam tail_t - parameter pack types excluding first type.
 * \retval type - first type in parameter pack.
 */
template<typename head_t, typename... tail_t>
struct variadic_head {
	typedef head_t type;
};

/*! \brief Meta function, tells if two types are the same.
 *
 * \tparam T1 - first of two types to compare.
 * \tparam T2 - second of two types.
 * \retval value - 1 is two types are in fact the same, 0 otherwise.
 * \retval type - true_type iff types are same.
 */
template<typename T1, typename T2>
struct same_type {
	static bool const value = false;
	typedef trait::false_type type;
};

/* \cond */
template<typename T1>
struct same_type<T1, T1> {
	static bool const value = true;
	typedef trait::true_type type;
};
/* \endcond */

/*! \brief Perform compile time ternary operator on types.
 *
 * \tparam condition - statically checkable condition that tells which type will be used.
 * \tparam type_for_true - type to be used if condition is true.
 * \tparam type_for_false - type to be used if condition is false.
 * \retval type - conditional type.
 */
template<bool const condition, typename type_for_true, typename type_for_false>
struct ternary;

/* \cond */
template<typename type_for_true, typename type_for_false>
struct ternary<true, type_for_true, type_for_false> {
	typedef type_for_true type;
};

template<typename type_for_true, typename type_for_false>
struct ternary<false, type_for_true, type_for_false> {
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
struct boolean_type<true> {
	typedef true_type type;
};

template<>
struct boolean_type<false> {
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
struct boolean_value<true_type> {
	static bool const value = true;
};

template<>
struct boolean_value<false_type> {
	static bool const value = false;
};
/* \endcond */

/*! \brief Perform compile time boolean AND on types.
 *
 * \tparam T1,...Tn - boolean value type.
 * \retval value - true iff all types T1,...Tn are true types.
 * \retval type - true_type iff all types T1,...Tn are true types.
 */
template<typename...>
struct and_op;

/* \cond */
template<typename head_t>
struct and_op<head_t> {
	static bool const value = same_type<head_t, true_type>::value;
	typedef typename boolean_type<value>::type type;
};

template<typename head_t, typename... tail_t>
struct and_op<head_t, tail_t...> {
	static bool const value = and_op<tail_t...>::value && same_type<head_t, true_type>::value;
	typedef typename boolean_type<value>::type type;
};
/* \endcond */

/*! \brief Perform compile time boolean OR on types.
 *
 * \tparam T1,...Tn - boolean value type.
 * \retval value - true iff at least one of types T1,...Tn is true type.
 * \retval type - true_type iff at least one of types T1,...Tn is true type.
 */
template<typename...>
struct or_op;

/* \cond */
template<typename head_t>
struct or_op<head_t> {
	static bool const value = same_type<head_t, true_type>::value;
	typedef typename boolean_type<value>::type type;
};

template<typename head_t, typename... tail_t>
struct or_op<head_t, tail_t...> {
	static bool const value = or_op<tail_t...>::value || same_type<head_t, true_type>::value;
	typedef typename boolean_type<value>::type type;
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
struct find_type {
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
template<int const index, typename t0_t = no_type, typename t1_t = no_type,
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
struct select_index {
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

/*! \brief Count occurrences of a type.
 *
 * \tparam t0_t, t1_t, ..., t9_t - list of types to search.
 * \tparam tType - look for number of occurrences of this type in given type set.
 * \retval value - number of occurrences of given type in given list of types.
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
struct count_type {
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

/*! \brief Test if type is a const type.
 *
 * \tparam T - type to check for constness trait.
 * \retval value - true iff T is a const type.
 * \retval type - true_type iff T is a const type.
 */
template<typename T>
struct is_const {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \cond */
template<typename T>
struct is_const<T const> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<typename T>
struct is_const<T const&> {
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
struct is_volatile {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \cond */
template<typename T>
struct is_volatile<T volatile> {
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
struct is_reference {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \cond */
template<typename T>
struct is_reference<T&> {
	static bool const value = true;
	typedef trait::true_type type;
};
template<typename T>
struct is_reference<T const&> {
	static bool const value = true;
	typedef trait::true_type type;
};
/*! \endcond */

template<typename T>
struct make_unsigned {
	static_assert(
		find_type<T, int short unsigned, int unsigned, int long unsigned, int long long unsigned>::value >= 0,
		"Only integral types can be made unsigned."
	);
	typedef T type;
};

template<>
struct make_unsigned<int short> {
	typedef int short unsigned type;
};

template<>
struct make_unsigned<int> {
	typedef int unsigned type;
};

template<>
struct make_unsigned<int long> {
	typedef int long unsigned type;
};

template<>
struct make_unsigned<int long long> {
	typedef int long long unsigned type;
};

template<typename T, T const val>
struct to_unsigned {
	static_assert( val >= 0, "Invalid integer constant cast to unsigned." );
	static typename make_unsigned<T>::type const value = static_cast<typename make_unsigned<T>::type>( val );
};

/*! \brief Test if given type is a C-array type.
 *
 * \tparam T - type to test for being C style array.
 * \retval value - true iff T is C style array.
 * \return type - true_type iff T is C style array.
 */
template<typename T>
struct is_array {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \cond */
#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const SIZE>
struct is_array<T[SIZE]> {
	static_assert( SIZE >= 0, "Invalid array size." );
	static bool const value = true;
	typedef trait::true_type type;
};
#else
template<typename T, int /* f*ck */ unsigned const SIZE>
struct is_array<T[SIZE]> {
	static bool const value = true;
	typedef trait::true_type type;
};
#endif

template<typename T>
struct is_array<T[]> {
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
struct make_reference<void> {
	typedef void type;
};

template<>
struct make_reference<void const> {
	typedef void type;
};

template<typename T>
struct make_reference<T&> {
	typedef T& type;
};

template<typename T>
struct make_reference {
	typedef T& type;
};
/* \endcond */

/*! \brief Meta function used to append rvalue reference to given type.
 *
 * \tparam tType - type to add rvalue reference trait to.
 * \retval type - a type with rvalue reference trait added.
 */
template<typename tType>
struct add_rvalue_reference;

/*! cond */
template<>
struct add_rvalue_reference<void> {
	typedef void type;
};

template<>
struct add_rvalue_reference<void const> {
	typedef void type;
};

template<typename tType>
struct add_rvalue_reference {
	typedef tType&& type;
};
/* \endcond */

/*! \brief Define compile-time value for types with inaccessible constructors.
 */
template<typename tType>
typename add_rvalue_reference<tType>::type declval();

/*! \brief Meta function used to strip reference from type.
 *
 * \tparam T - type to strip reference from.
 * \retval type - a type T without reference trait.
 */
template<typename T>
struct strip_reference {
	typedef T type;
};

/*! \cond */
template<typename T>
struct strip_reference<T&> {
	typedef T type;
};
template<typename T>
struct strip_reference<T&&> {
	typedef T type;
};
/*! \endcond */

/*! \brief Meta function used to add one level of pointer to a type.
 *
 * \tparam T - type to add pointer to.
 * \retval type - pointer to T type.
 */
template<typename T>
struct add_pointer;

/*! \cond */
template<typename T>
struct add_pointer<T&> {
	typedef T* type;
};

template<typename T>
struct add_pointer {
	typedef T* type;
};
/*! \endcond */

/*! \brief Meta function used to strip one level of pointer from type.
 *
 * \tparam T - type to strip pointer from.
 * \retval type - stripped new type.
 */
template<typename T>
struct strip_pointer {
	typedef T type;
};

/*! \cond */
template<typename T>
struct strip_pointer<T*> {
	typedef T type;
};
/*! \endcond */

/*! \brief Meta function used to add const to type.
 *
 * \tparam T - type to add const to.
 * \retval type - a type with constness trait.
 */
template<typename T>
struct make_const {
	typedef T const type;
};

/*! \cond */
template<typename T>
struct make_const<T const> {
	typedef T const type;
};

template<typename T>
struct make_const<T&> {
	typedef T const& type;
};
/*! \endcond */

/*! \brief Meta function used to strip const from type.
 *
 * \tparam T - type to strip const from.
 * \retval type - a type without constness trait.
 */
template<typename T>
struct strip_const {
	typedef T type;
};

/*! \cond */
template<typename T>
struct strip_const<T const> {
	typedef T type;
};
/*! \endcond */

/*! \brief Meta function used to strip volatile from type.
 *
 * \tparam T - type to strip volatile from.
 * \retval type - a type without volatileness trait.
 */
template<typename T>
struct strip_volatile {
	typedef T type;
};

/*! \cond */
template<typename T>
struct strip_volatile<T volatile> {
	typedef T type;
};
/*! \endcond */

/*! \brief Meta function used to strip both const and volatile qualifiers from type.
 *
 * \tparam T - type to strip cv-qualifiers from.
 * \retval type - a type without any cv-qualifiers trait.
 */
template<typename T>
struct strip_cv {
	typedef typename strip_volatile<typename strip_const<T>::type>::type type;
};

/*! \brief Meta function used to strip all type modifiers.
 *
 * \tparam T - type to strip all modifiers from.
 * \retval type - a type without any modifiers.
 */
template<typename T>
struct strip {
	typedef typename strip_cv<typename strip_reference<T>::type>::type type;
};

/*! \brief Remove first dimension from array type.
 *
 * \tparam T - type to remove first array dimension from.
 */
template<typename T>
struct remove_extent {
	typedef T type;
};

/*! \cond */
#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const SIZE>
struct remove_extent<T[SIZE]> {
	static_assert( SIZE >= 0, "Invalid array size." );
	typedef T type;
};
#else
template<typename T, int /* f*ck */ unsigned const SIZE>
struct remove_extent<T[SIZE]> {
	typedef T type;
};
#endif

template<typename T>
struct remove_extent<T[]> {
	typedef T type;
};
/*! \endcond */

/*! \brief Turn function type into pointer type.
 */
template<typename T>
struct pointerize_function {
	typedef T type;
};

/*! \cond */
template<typename R, typename... ARG>
struct pointerize_function<R (ARG...)> {
	typedef R (*type)( ARG... );
};
/*! \endcond */

/*! \cond */
template<typename T>
struct is_pointer_helper {
	static bool const value = false;
	typedef trait::false_type type;
};
template<typename T>
struct is_pointer_helper<T*> {
	static bool const value = true;
	typedef trait::true_type type;
};
/*! \endcond */

/*! \brief Test if type is a pointer type.
 *
 * \tparam T - type to check for pointer trait.
 * \retval value - true iff T is a pointer type.
 * \retval type - true_type iff T is a pointer type.
 */
template<typename T>
struct is_pointer {
	typedef typename is_pointer_helper<typename strip_cv<T>::type>::type type;
	static bool const value = boolean_value<type>::value;
};

/*! \brief Check if one type is convertible to another type.
 *
 * \tparam from_t - source type.
 * \tparam to_t - destination type.
 * \retval value - true iff from_t is convertible to to_t, else false.
 * \retval type - true_type iff from_t is convertible to to_t, else false_type.
 */
template<typename from_t, typename to_t>
struct is_convertible {
	template <typename to_x_t>
	static void test_try( to_x_t );
	template<typename from_x_t, typename to_x_t, typename = decltype( test_try<to_x_t>( declval<from_x_t>() ) )>
	static trait::true_type test( int );
	template<typename, typename>
	static trait::false_type test( ... );
	static bool const value = sizeof ( test<from_t, to_t>( 0 ) ) == sizeof ( trait::true_type );
	typedef typename boolean_type<value>::type type;
};

/*! \brief Copy constness (or lack of it) from on type to another.
 *
 * \tparam source - get constness from this type.
 * \tparam destination - apply constness to this type.
 * \retval type - a destination type with constness of source type.
 */
template<typename source, typename destination>
struct copy_const {
	typedef destination type;
};

/*! \cond */
template<typename source, typename destination>
struct copy_const<source const, destination> {
	typedef destination const type;
};
template<typename source, typename destination>
struct copy_const<source const&, destination> {
	typedef destination const& type;
};
/*! \endcond */

template<typename T>
struct make_const_ref_ptr {
	typedef typename ternary<is_reference<T>::value,
						typename make_reference<
							typename ternary<is_pointer<typename strip_reference<T>::type>::value,
								typename add_pointer<typename strip_pointer<typename strip_reference<T>::type>::type const>::type,
								typename strip_reference<T>::type const
							>::type
						>::type,
						typename ternary<is_pointer<typename strip_reference<T>::type>::value,
							typename add_pointer<typename strip_pointer<typename strip_reference<T>::type>::type const>::type,
							T const>::type>::type type;
};

/*! \brief Check if given class has been derived from another given class.
 *
 * \tparam derived_t - suspected derived type.
 * \tparam base_t - suspected base type.
 * \retval value - true iff derived_t is derived from base_t.
 */
template<typename derived_t, typename base_t>
struct is_kind_of {
	static true_type calc( typename strip_reference<base_t>::type const* );
	static false_type calc( ... );
	static bool const value = sizeof ( calc( static_cast<typename strip_reference<derived_t>::type const*>( nullptr ) ) ) == sizeof ( true_type );
};

/*! \brief Interface preventing copying of objects.
 */
class HNonCopyable {
public:
	HNonCopyable( void ) {}
	virtual ~HNonCopyable( void ) {}
private:
	HNonCopyable( HNonCopyable const& ) = delete;
	HNonCopyable& operator = ( HNonCopyable const& ) = delete;
};

/*! \cond */
template<typename T>
struct is_field_impl {
	static bool const value = false;
};
template<typename class_t, typename T>
struct is_field_impl<T class_t::*> {
	static bool const value = true;
};
/*! \cond */

/*! \brief Check if given type is a field type (actually a pointer to a field).
 *
 * \tparam T - type to check for being a field.
 * \retval value - true iff given type is a field type.
 */
template<typename T>
struct is_field {
	static bool const value = is_field_impl<typename strip<T>::type>::value;
};

/*! \cond */
template<typename T>
struct is_function_impl {
	static bool const value = false;
};
template<typename return_t, typename... arg_t>
struct is_function_impl<return_t ( arg_t... )> {
	static bool const value = true;
};
template<typename return_t, typename... arg_t>
struct is_function_impl<return_t (&)( arg_t... )> {
	static bool const value = true;
};
/*! \endcond */

/*! \brief Check if given type is a function type.
 *
 * \tparam T - type to check for being a function.
 * \retval value - true iff given type is a function type.
 */
template<typename T>
struct is_function {
	static bool const value = is_function_impl<typename strip<T>::type>::value;
};

/*! \brief Remove references and cv-qualifiers and potentially transform C-array type to pointer type.
 *
 * \tparam T - type to decay.
 * \return type - decayed type.
 */
template<typename T>
struct decay {
	typedef typename strip_reference<T>::type U;
	typedef typename ternary<
		is_array<U>::value,
		typename remove_extent<U>::type*,
		typename ternary<
			is_function<U>::value,
			typename pointerize_function<U>::type,
			typename strip_cv<U>::type
		>::type
	>::type type;
};

/*! \brief Check if given type is a function pointer type.
 *
 * \tparam T - type to check for being a function pointer.
 * \retval value - true iff given type is a function pointer type.
 */
template<typename T>
struct is_function_pointer {
	static bool const value = false;
};

/*! \cond */
template<typename return_t, typename... arg_t>
struct is_function_pointer<return_t (*)( arg_t... )> {
	static bool const value = true;
};
/*! \endcond */

/*! \cond */
template<typename T>
struct is_member_impl {
	static bool const value = false;
};
template<typename return_t, typename class_t, typename... A>
struct is_member_impl<return_t ( class_t::* )( A... )> {
	static bool const value = true;
};
template<typename return_t, typename class_t, typename... A>
struct is_member_impl<return_t ( class_t::* )( A... ) const> {
	static bool const value = true;
};
template<typename return_t, typename class_t, typename... A>
struct is_member_impl<return_t ( class_t::* )( A... ) volatile> {
	static bool const value = true;
};
template<typename return_t, typename class_t, typename... A>
struct is_member_impl<return_t ( class_t::* )( A... ) const volatile> {
	static bool const value = true;
};
/*! \endcond */

/*! \brief Check if given type is a member type.
 *
 * \tparam T - type to check for being a member.
 * \retval value - true iff given type is a member type.
 */
template<typename T>
struct is_member {
	static bool const value = is_member_impl<typename strip<T>::type>::value;
};

/*! \cond */
template<typename T>
struct is_member_const_impl
	{ static bool const value = false; };
template<typename return_t, typename class_t, typename... A>
struct is_member_const_impl<return_t ( class_t::* )( A... ) const>
	{ static bool const value = true; };
template<typename return_t, typename class_t, typename... A>
struct is_member_const_impl<return_t ( class_t::* )( A... ) const volatile>
	{ static bool const value = true; };
/*! \endcond */

/*! \brief Check if given type is a const member type.
 *
 * \tparam T - type to check for being a const member.
 * \retval value - true iff given type is a const member type.
 */
template<typename T>
struct is_member_const {
	static bool const value = is_member_const_impl<typename strip<T>::type>::value;
};

namespace generic_helper {

template<typename T>
struct return_type
	{ typedef no_type type; };

template<typename return_t, typename class_t, typename... A>
struct return_type<return_t ( class_t::* )( A... )>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename... A>
struct return_type<return_t ( class_t::* )( A... ) const>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename... A>
struct return_type<return_t ( class_t::* )( A... ) volatile>
	{ typedef return_t type; };
template<typename return_t, typename class_t, typename... A>
struct return_type<return_t ( class_t::* )( A... ) const volatile>
	{ typedef return_t type; };
template<typename return_t, typename... A>
struct return_type<return_t ( * )( A... )>
	{ typedef return_t type; };
template<typename return_t, typename... A>
struct return_type<return_t ( & )( A... )>
	{ typedef return_t type; };
template<typename return_t, typename... A>
struct return_type<return_t ( A... )>
	{ typedef return_t type; };

template<typename T>
struct argument_type
	{ template<int const> struct index { typedef no_type type; }; };

template<typename return_t, typename class_t, typename... A>
struct argument_type<return_t ( class_t::* )( A... )>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };
template<typename return_t, typename class_t, typename... A>
struct argument_type<return_t ( class_t::* )( A... ) const>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };
template<typename return_t, typename class_t, typename... A>
struct argument_type<return_t ( class_t::* )( A... ) volatile>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };
template<typename return_t, typename class_t, typename... A>
struct argument_type<return_t ( class_t::* )( A... ) const volatile>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };
template<typename return_t, typename... A>
struct argument_type<return_t ( * )( A... )>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };
template<typename return_t, typename... A>
struct argument_type<return_t ( A... )>
	{ template<int const no> struct index { typedef typename select_index<no, A...>::type type; }; };

}

/*! \cond */

template<typename T>
struct functional_return_type {
	template<typename real_class>
	static true_type has_result_type( typename strip_reference<typename real_class::result_type>::type* );
	template<typename real_class>
	static false_type has_result_type( ... );
	static bool const has_result_type_value = ( sizeof ( has_result_type<T>( 0 ) ) == sizeof ( true_type ) );

	template<typename U, bool const has_type>
	struct get
		{ typedef no_type type; };

	template<typename U>
	struct get<U, true>
		{ typedef typename U::result_type type; };

	typedef typename get<T, has_result_type_value>::type type;
};

template<typename T>
struct functional_argument_type {
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
	template<typename real_class>
	static true_type has_third_argument_type( typename strip_reference<typename real_class::third_argument_type>::type* );
	template<typename real_class>
	static false_type has_third_argument_type( ... );
	template<typename real_class>
	static true_type has_fourth_argument_type( typename strip_reference<typename real_class::fourth_argument_type>::type* );
	template<typename real_class>
	static false_type has_fourth_argument_type( ... );
	template<typename real_class>
	static true_type has_fifth_argument_type( typename strip_reference<typename real_class::fifth_argument_type>::type* );
	template<typename real_class>
	static false_type has_fifth_argument_type( ... );
	template<typename real_class>
	static true_type has_sixth_argument_type( typename strip_reference<typename real_class::sixth_argument_type>::type* );
	template<typename real_class>
	static false_type has_sixth_argument_type( ... );
	template<typename real_class>
	static true_type has_seventh_argument_type( typename strip_reference<typename real_class::seventh_argument_type>::type* );
	template<typename real_class>
	static false_type has_seventh_argument_type( ... );
	template<typename real_class>
	static true_type has_eighth_argument_type( typename strip_reference<typename real_class::eighth_argument_type>::type* );
	template<typename real_class>
	static false_type has_eighth_argument_type( ... );
	template<typename real_class>
	static true_type has_ninth_argument_type( typename strip_reference<typename real_class::ninth_argument_type>::type* );
	template<typename real_class>
	static false_type has_ninth_argument_type( ... );
	template<typename real_class>
	static true_type has_tenth_argument_type( typename strip_reference<typename real_class::tenth_argument_type>::type* );
	template<typename real_class>
	static false_type has_tenth_argument_type( ... );
	template<typename real_class>
	static true_type has_eleventh_argument_type( typename strip_reference<typename real_class::eleventh_argument_type>::type* );
	template<typename real_class>
	static false_type has_eleventh_argument_type( ... );

	template<typename U, int const no>
	struct idx;

	template<typename U>
	struct idx<U, 0> {
		static bool const value = ( sizeof ( has_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 1> {
		static bool const value = ( sizeof ( has_first_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 2> {
		static bool const value = ( sizeof ( has_second_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 3> {
		static bool const value = ( sizeof ( has_third_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 4> {
		static bool const value = ( sizeof ( has_fourth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 5> {
		static bool const value = ( sizeof ( has_fifth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 6> {
		static bool const value = ( sizeof ( has_sixth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 7> {
		static bool const value = ( sizeof ( has_seventh_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 8> {
		static bool const value = ( sizeof ( has_eighth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 9> {
		static bool const value = ( sizeof ( has_ninth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 10> {
		static bool const value = ( sizeof ( has_tenth_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};
	template<typename U>
	struct idx<U, 11> {
		static bool const value = ( sizeof ( has_eleventh_argument_type<U>( 0 ) ) == sizeof ( true_type ) );
	};

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
	template<typename U>
	struct get<U, 3>
		{ typedef typename U::third_argument_type type; };
	template<typename U>
	struct get<U, 4>
		{ typedef typename U::fourth_argument_type type; };
	template<typename U>
	struct get<U, 5>
		{ typedef typename U::fifth_argument_type type; };
	template<typename U>
	struct get<U, 6>
		{ typedef typename U::sixth_argument_type type; };
	template<typename U>
	struct get<U, 7>
		{ typedef typename U::seventh_argument_type type; };
	template<typename U>
	struct get<U, 8>
		{ typedef typename U::eighth_argument_type type; };
	template<typename U>
	struct get<U, 9>
		{ typedef typename U::ninth_argument_type type; };
	template<typename U>
	struct get<U, 10>
		{ typedef typename U::tenth_argument_type type; };
	template<typename U>
	struct get<U, 11>
		{ typedef typename U::eleventh_argument_type type; };

	template<int const no> struct index {
		static int const argNo = meta::ternary<( no == 0 ), meta::ternary<idx<T, 0>::value, 0, meta::ternary<idx<T, 1>::value, 1, -1>::value>::value,
														meta::ternary<idx<T, no + 1>::value, no + 1, -1>::value>::value;
		typedef typename get<T, argNo>::type type;
	};
};
/*! \endcond */

/*! \brief Get return type of member or function.
 *
 * \tparam T - type of member or function.
 * \retval type - return type of given member or function.
 */
template<typename T>
struct return_type {
	template<bool const force, typename U>
	struct resolve;

	template<typename U>
	struct resolve<false, U> {
		typedef U type;
	};

	template<typename U>
	struct resolve<true, U> {
		typedef typename generic_helper::return_type<typename strip<T>::type>::type type;
	};

	typedef typename functional_return_type<typename strip_reference<T>::type>::type functional_return_type_t;

	typedef typename resolve<same_type<functional_return_type_t, no_type>::value, functional_return_type_t>::type type;
};

/*! \brief Get type of an indexed argument of member or function.
 *
 * \tparam T - type of member or function.
 * \tparam no - index of an argument to get type of.
 * \retval type - type of given argument of given member or function.
 */
template<typename T, int const no>
struct argument_type {
	template<bool const force, typename U>
	struct resolve;

	template<typename U>
	struct resolve<false, U> {
		typedef U type;
	};

	template<typename U>
	struct resolve<true, U> {
		typedef typename generic_helper::argument_type<typename strip<T>::type>::template index<no>::type type;
	};

	typedef typename functional_argument_type<typename strip<T>::type>::template index<no>::type functional_argument_type_t;

	typedef typename resolve<same_type<functional_argument_type_t, no_type>::value, functional_argument_type_t>::type type;
};

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
template<typename return_t, typename class_t, typename... A>
struct class_type_from_member<return_t ( class_t::* )( A... )>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename... A>
struct class_type_from_member<return_t ( class_t::* )( A... ) const>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename... A>
struct class_type_from_member<return_t ( class_t::* )( A... ) volatile>
	{ typedef class_t type; };
template<typename return_t, typename class_t, typename... A>
struct class_type_from_member<return_t ( class_t::* )( A... ) const volatile>
	{ typedef class_t type; };

template<typename class_t, typename field_t>
struct class_type_from_field<field_t class_t::*>
	{ typedef class_t type; };

template<typename T>
struct class_type {
	typedef typename strip<T>::type class_t;
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

/*! \cond */
template<typename class_t, typename field_t>
struct field_type<field_t class_t::*> {
	typedef field_t type;
};

namespace generic_helper {

template<typename field_t>
struct argument_count
	{ static int const value = 0; };

template<typename return_t, typename class_t, typename... A>
struct argument_count<return_t ( class_t::* )( A... )>
	{ static int const value = sizeof... ( A ); };
template<typename return_t, typename class_t, typename... A>
struct argument_count<return_t ( class_t::* )( A... ) const>
	{ static int const value = sizeof... ( A ); };
template<typename return_t, typename class_t, typename... A>
struct argument_count<return_t ( class_t::* )( A... ) volatile>
	{ static int const value = sizeof... ( A ); };
template<typename return_t, typename class_t, typename... A>
struct argument_count<return_t ( class_t::* )( A... ) const volatile>
	{ static int const value = sizeof... ( A ); };
template<typename return_t, typename... A>
struct argument_count<return_t ( * )( A... )>
	{ static int const value = sizeof... ( A ); };
template<typename return_t, typename... A>
struct argument_count<return_t ( A... )>
	{ static int const value = sizeof... ( A ); };
/*! \endcond */

}

template<typename T>
struct functional_argument_count {
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
	template<typename real_class>
	static true_type has_third_argument_type( typename strip_reference<typename real_class::third_argument_type>::type* );
	template<typename real_class>
	static false_type has_third_argument_type( ... );
	template<typename real_class>
	static true_type has_fourth_argument_type( typename strip_reference<typename real_class::fourth_argument_type>::type* );
	template<typename real_class>
	static false_type has_fourth_argument_type( ... );
	template<typename real_class>
	static true_type has_fifth_argument_type( typename strip_reference<typename real_class::fifth_argument_type>::type* );
	template<typename real_class>
	static false_type has_fifth_argument_type( ... );
	template<typename real_class>
	static true_type has_sixth_argument_type( typename strip_reference<typename real_class::sixth_argument_type>::type* );
	template<typename real_class>
	static false_type has_sixth_argument_type( ... );
	template<typename real_class>
	static true_type has_seventh_argument_type( typename strip_reference<typename real_class::seventh_argument_type>::type* );
	template<typename real_class>
	static false_type has_seventh_argument_type( ... );
	template<typename real_class>
	static true_type has_eighth_argument_type( typename strip_reference<typename real_class::eighth_argument_type>::type* );
	template<typename real_class>
	static false_type has_eighth_argument_type( ... );
	template<typename real_class>
	static true_type has_ninth_argument_type( typename strip_reference<typename real_class::ninth_argument_type>::type* );
	template<typename real_class>
	static false_type has_ninth_argument_type( ... );
	template<typename real_class>
	static true_type has_tenth_argument_type( typename strip_reference<typename real_class::tenth_argument_type>::type* );
	template<typename real_class>
	static false_type has_tenth_argument_type( ... );
	template<typename real_class>
	static true_type has_eleventh_argument_type( typename strip_reference<typename real_class::eleventh_argument_type>::type* );
	template<typename real_class>
	static false_type has_eleventh_argument_type( ... );

	static bool const ta0 = sizeof ( has_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta1 = sizeof ( has_first_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta2 = sizeof ( has_second_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta3 = sizeof ( has_third_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta4 = sizeof ( has_fourth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta5 = sizeof ( has_fifth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta6 = sizeof ( has_sixth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta7 = sizeof ( has_seventh_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta8 = sizeof ( has_eighth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta9 = sizeof ( has_ninth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta10 = sizeof ( has_tenth_argument_type<T>( 0 ) ) == sizeof ( true_type );
	static bool const ta11 = sizeof ( has_eleventh_argument_type<T>( 0 ) ) == sizeof ( true_type );

	template<typename U, int const no, bool const exists>
	struct get
		{ typedef no_type type; };

	template<typename U>
	struct get<U, 0, true>
		{ typedef typename U::argument_type type; };
	template<typename U>
	struct get<U, 1, true>
		{ typedef typename U::first_argument_type type; };
	template<typename U>
	struct get<U, 2, true>
		{ typedef typename U::second_argument_type type; };
	template<typename U>
	struct get<U, 3, true>
		{ typedef typename U::third_argument_type type; };
	template<typename U>
	struct get<U, 4, true>
		{ typedef typename U::fourth_argument_type type; };
	template<typename U>
	struct get<U, 5, true>
		{ typedef typename U::fifth_argument_type type; };
	template<typename U>
	struct get<U, 6, true>
		{ typedef typename U::sixth_argument_type type; };
	template<typename U>
	struct get<U, 7, true>
		{ typedef typename U::seventh_argument_type type; };
	template<typename U>
	struct get<U, 8, true>
		{ typedef typename U::eighth_argument_type type; };
	template<typename U>
	struct get<U, 9, true>
		{ typedef typename U::ninth_argument_type type; };
	template<typename U>
	struct get<U, 10, true>
		{ typedef typename U::tenth_argument_type type; };
	template<typename U>
	struct get<U, 11, true>
		{ typedef typename U::eleventh_argument_type type; };

	static int const a0 = meta::ternary<meta::logical_not<same_type<typename get<T, 0, ta0>::type, no_type>::value>::value, 1, 0>::value;
	static int const a1 = meta::ternary<meta::logical_not<same_type<typename get<T, 1, ta1>::type, no_type>::value>::value, 1, 0>::value;
	static int const a2 = meta::ternary<meta::logical_not<same_type<typename get<T, 2, ta2>::type, no_type>::value>::value, 1, 0>::value;
	static int const a3 = meta::ternary<meta::logical_not<same_type<typename get<T, 3, ta3>::type, no_type>::value>::value, 1, 0>::value;
	static int const a4 = meta::ternary<meta::logical_not<same_type<typename get<T, 4, ta4>::type, no_type>::value>::value, 1, 0>::value;
	static int const a5 = meta::ternary<meta::logical_not<same_type<typename get<T, 5, ta5>::type, no_type>::value>::value, 1, 0>::value;
	static int const a6 = meta::ternary<meta::logical_not<same_type<typename get<T, 6, ta6>::type, no_type>::value>::value, 1, 0>::value;
	static int const a7 = meta::ternary<meta::logical_not<same_type<typename get<T, 7, ta7>::type, no_type>::value>::value, 1, 0>::value;
	static int const a8 = meta::ternary<meta::logical_not<same_type<typename get<T, 8, ta8>::type, no_type>::value>::value, 1, 0>::value;
	static int const a9 = meta::ternary<meta::logical_not<same_type<typename get<T, 9, ta9>::type, no_type>::value>::value, 1, 0>::value;
	static int const a10 =  meta::ternary<meta::logical_not<same_type<typename get<T, 10, ta10>::type, no_type>::value>::value, 1, 0>::value;
	static int const a11 =  meta::ternary<meta::logical_not<same_type<typename get<T, 11, ta11>::type, no_type>::value>::value, 1, 0>::value;

	static int const arg_count = a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + a10 + a11 + meta::ternary<a1 != 0, 0, a0>::value;
	static int const value = meta::ternary<arg_count != 0, arg_count, -1>::value;
};

template<typename T>
struct argument_count {
	template<bool const force, int const count>
	struct resolve;

	template<int const count>
	struct resolve<false, count> {
		static int const value = count;
	};

	template<int const count>
	struct resolve<true, count> {
		static int const value = generic_helper::argument_count<typename strip<T>::type>::value;
	};

	static int const functionalArgumentCount = functional_argument_count<typename strip<T>::type>::value;

	static int const value = resolve<functionalArgumentCount == -1, functionalArgumentCount>::value;
};

}

}

#endif /* #ifndef YAAL_HCORE_TRAIT_HXX_INCLUDED */


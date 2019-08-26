/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/pod.hxx
 * \brief Trait helpers for PODs.
 */

#ifndef YAAL_HCORE_POD_HXX_INCLUDED
#define YAAL_HCORE_POD_HXX_INCLUDED 1

#include "config.hxx"

#include "hcore/trait.hxx"

namespace yaal {

/*! \brief Check if type is a POD type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a POD type.
 * \retval type - true_type iff T is a POD type.
 */
template<typename T>
struct is_pod {
	static bool const value = trait::is_pointer<T>::value;
	typedef typename trait::ternary<trait::is_pointer<T>::value, trait::true_type, trait::false_type>::type type;
};

/*! \cond */
template<> struct is_pod<bool> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<char> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<char unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int short> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int short unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long long> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long long unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<double> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<double long> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<float> { static bool const value = true; typedef trait::true_type type; };
/*! \endcond */

/*! \brief Check if type is an integral type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is an integral type.
 * \retval type - true_type iff T is an integral type.
 */
template<typename T>
struct is_integral {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \brief Check if type is a floating point type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a floating point type.
 * \retval type - true_type iff T is a floating point type.
 */
template<typename T>
struct is_floating_point {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \brief Check if type is a numeric type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a numeric type.
 * \retval type - true_type iff T is a numeric type.
 */
template<typename T>
struct is_numeric {
	static bool const value = ( is_integral<T>::value || is_floating_point<T>::value );
	typedef typename trait::ternary<value, trait::true_type, trait::false_type>::type type;
};

/*! \cond */
template<>
struct is_integral<int> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int short> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int short unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<char> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<char signed> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<char unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long long> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long long unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<float> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<double> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<double long> {
	static bool const value = true;
	typedef trait::true_type type;
};

namespace hcore { class HNumber; }

template<>
struct is_floating_point<hcore::HNumber> {
	static bool const value = true;
	typedef trait::true_type type;
};

/*! \endcond */

typedef char unsigned          u8_t;  /*!< 8 bit unsigned integer. */
typedef char signed            i8_t;  /*!< 8 bit signed integer. */
typedef int short unsigned     u16_t; /*!< 16 bit unsigned integer. */
typedef int short signed       i16_t; /*!< 16 bit signed integer. */
typedef int unsigned           u32_t; /*!< 32 bit unsigned integer. */
typedef int signed             i32_t; /*!< 32 bit signed integer. */
#if ( SIZEOF_INT_LONG == 8 )
typedef int long unsigned      u64_t; /*!< 64 bit unsigned integer. */
typedef int long signed        i64_t; /*!< 64 bit signed integer. */
#elif ( SIZEOF_INT_LONG_LONG == 8 ) /* #if ( SIZEOF_INT_LONG == 8 ) */
typedef int long long unsigned u64_t; /*!< 64 bit unsigned integer. */
typedef int long long signed   i64_t; /*!< 64 bit signed integer. */
#else /* #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */
#error Required type: a 64bit integer, is not available.
#endif /* #else #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */

#if ( SIZEOF_INT == SIZEOF_VOIDP )
typedef int                    int_native_t;
#elif ( SIZEOF_INT_LONG == SIZEOF_VOIDP )
typedef int long               int_native_t;
#elif ( SIZEOF_INT_LONG_LONG == SIZEOF_VOIDP )
typedef int long long          int_native_t;
#else /* Native integer type is either `int', or `int long', or `int long long'. */
#error Cannot establish native integer type.
#endif /* Native integer type detection */

typedef int long unsigned hash_value_t;

}

#endif /* #ifndef YAAL_HCORE_POD_HXX_INCLUDED */


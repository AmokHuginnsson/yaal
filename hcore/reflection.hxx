/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/reflection.hxx
 * \brief Higher order interfaces for PODs.
 */

#ifndef YAAL_HCORE_REFLECTION_HXX_INCLUDED
#define YAAL_HCORE_REFLECTION_HXX_INCLUDED 1

namespace yaal {

/*! \brief POD type symbols. (Some non-POD yaal types included too).
 */
enum class TYPE {
	UNKNOWN       = 0x0000,
	BOOL          = 0x0001,
	CHAR          = 0x0002,
	INT_SHORT     = 0x0004,
	INT           = 0x0008,
	INT_LONG      = 0x0010,
	INT_LONG_LONG = 0x0020,
	FLOAT         = 0x0040,
	DOUBLE        = 0x0080,
	DOUBLE_LONG   = 0x0100,
	POINTER       = 0x0200,
	HSTRING       = 0x0400,
	HNUMBER       = 0x0800,
	HINFO         = 0x1000,
	HTIME         = 0x2000
};
template<typename tType>
struct symbolic_type;

/*! \cond */
template<> struct symbolic_type<bool> { static TYPE const value = TYPE::BOOL; };
template<> struct symbolic_type<char> { static TYPE const value = TYPE::CHAR; };
template<> struct symbolic_type<char unsigned> { static TYPE const value = TYPE::CHAR; };
template<> struct symbolic_type<int short> { static TYPE const value = TYPE::INT_SHORT; };
template<> struct symbolic_type<int short unsigned> { static TYPE const value = TYPE::INT_SHORT; };
template<> struct symbolic_type<int> { static TYPE const value = TYPE::INT; };
template<> struct symbolic_type<int unsigned> { static TYPE const value = TYPE::INT; };
template<> struct symbolic_type<int long> { static TYPE const value = TYPE::INT_LONG; };
template<> struct symbolic_type<int long unsigned> { static TYPE const value = TYPE::INT_LONG; };
template<> struct symbolic_type<int long long> { static TYPE const value = TYPE::INT_LONG_LONG; };
template<> struct symbolic_type<int long long unsigned> { static TYPE const value = TYPE::INT_LONG_LONG; };
template<> struct symbolic_type<float> { static TYPE const value = TYPE::FLOAT; };
template<> struct symbolic_type<double> { static TYPE const value = TYPE::DOUBLE; };
template<> struct symbolic_type<double long> { static TYPE const value = TYPE::DOUBLE_LONG; };
template<typename T> struct symbolic_type<T*> { static TYPE const value = TYPE::POINTER; };
template<> struct symbolic_type<yaal::hcore::HString> { static TYPE const value = TYPE::HSTRING; };
/*! \endcond */

/*! \brief Convert static type information to value.
 *
 * \tparam tType - type to convert to value.
 * \retval value - a value that is symbolical representation of given type.
 */
template<typename tType>
struct symbolic_type {
	static TYPE const value = TYPE::UNKNOWN;
};

}

#endif /* #ifndef YAAL_HCORE_REFLECTION_HXX_INCLUDED */


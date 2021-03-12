/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/bitmaskenum.hxx
 * \brief Declaration and implementation of bitmask decorators for `enum class`.
 */

#ifndef YAAL_HCORE_BITMASKENUM_HXX_INCLUDED
#define YAAL_HCORE_BITMASKENUM_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal {

template<typename enum_t>
struct is_bitmask_enum {
	static bool const value = false;
};

namespace bitmaskenum {

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type& operator &= ( enum_t& left_, enum_t right_ ) {
	left_ = static_cast<enum_t>( static_cast<int long unsigned>( left_ ) & static_cast<int long unsigned>( right_ ) );
	return left_;
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type& operator |= ( enum_t& left_, enum_t right_ ) {
	left_ = static_cast<enum_t>( static_cast<int long unsigned>( left_ ) | static_cast<int long unsigned>( right_ ) );
	return left_;
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type& operator ^= ( enum_t& left_, enum_t right_ ) {
	left_ = static_cast<enum_t>( static_cast<int long unsigned>( left_ ) ^ static_cast<int long unsigned>( right_ ) );
	return left_;
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type operator & ( enum_t left_, enum_t right_ ) {
	return ( static_cast<enum_t>( static_cast<int long unsigned>( left_ ) & static_cast<int long unsigned>( right_ ) ) );
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type operator | ( enum_t left_, enum_t right_ ) {
	return ( static_cast<enum_t>( static_cast<int long unsigned>( left_ ) | static_cast<int long unsigned>( right_ ) ) );
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type operator ^ ( enum_t left_, enum_t right_ ) {
	return ( static_cast<enum_t>( static_cast<int long unsigned>( left_ ) ^ static_cast<int long unsigned>( right_ ) ) );
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, enum_t>::type operator ~ ( enum_t enum_ ) {
	return ( static_cast<enum_t>( ~static_cast<int long unsigned>( enum_ ) ) );
}

template<typename enum_t>
inline typename yaal::trait::enable_if<is_bitmask_enum<enum_t>::value, bool>::type operator ! ( enum_t enum_ ) {
	return ( static_cast<int long unsigned>( enum_ ) == 0 );
}

}

}

#define M_USING_ENUMBITMASK() \
using ::yaal::bitmaskenum::operator |=; \
using ::yaal::bitmaskenum::operator &=; \
using ::yaal::bitmaskenum::operator ^=; \
using ::yaal::bitmaskenum::operator |; \
using ::yaal::bitmaskenum::operator &; \
using ::yaal::bitmaskenum::operator ^; \
using ::yaal::bitmaskenum::operator ~; \
using ::yaal::bitmaskenum::operator !

#define M_ENUM_IS_A_BITMASK( enum_t ) \
template<> \
struct is_bitmask_enum<enum_t> { \
	static bool const value = true; \
}

#endif /* #ifndef YAAL_HCORE_BITMASKENUM_HXX_INCLUDED */


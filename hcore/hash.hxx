/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hash.hxx
 * \brief Declaration of hash<T> struct template.
 */

#ifndef YAAL_HCORE_HASH_HXX_INCLUDED
#define YAAL_HCORE_HASH_HXX_INCLUDED 1

#include "hcore/hpair.hxx"

namespace yaal {

namespace hcore {

template<typename key_t>
struct hash {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( key_t const& ) const;
};

template<typename key_t>
struct hash<key_t*> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( key_t* const& key_ ) const {
		return ( reinterpret_cast<hash_value_type>( key_ ) );
	}
};

template<typename first_t, typename second_t>
struct hash<yaal::hcore::HPair<first_t, second_t>> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( yaal::hcore::HPair<first_t, second_t> const& key_ ) const {
		return ( hash<first_t>()( key_.first ) * 3 + hash<second_t>()( key_.second ) );
	}
};

template<typename key_t, typename TAG>
struct hash<yaal::hcore::HTaggedPOD<key_t, TAG>> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( HTaggedPOD<key_t, TAG> const& key_ ) const {
		return ( hash<key_t>()( key_.get() ) );
	}
};

extern M_YAAL_HCORE_PUBLIC_API int long const _primes_[32];

}

}

#endif /* #ifndef YAAL_HCORE_HASH_HXX_INCLUDED */


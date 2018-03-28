/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hpair.hxx
 * \brief Declaration and implementation of HPair<> class template.
 */

#ifndef YAAL_HCORE_HPAIR_HXX_INCLUDED
#define YAAL_HCORE_HPAIR_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/algorithm_low.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

/*! \brief Implementation of ordered pair concept.
 *
 * \tparam first_t - type of first pair element.
 * \tparam second_t - type of second pair element.
 */
template<typename first_t, typename second_t>
class HPair final {
public:
	typedef first_t first_type;
	typedef second_t second_type;
	first_type first;
	second_type second;
	HPair( void )
		: first(), second() {
		return;
	}
	HPair( first_type const& key_, second_type const& value_ )
		: first( key_ ), second( value_ ) {
		return;
	}
	template<
		typename alt_first_t,
		typename alt_second_t,
		typename = typename trait::enable_if<
			trait::and_op<
				typename trait::is_convertible<alt_first_t const&, first_type>,
				typename trait::is_convertible<alt_second_t const&, second_type>
			>::value
		>
	>
	HPair( HPair<alt_first_t, alt_second_t> const& altPair_ )
		: first( altPair_.first ), second( altPair_.second ) {
		return;
	}

	constexpr HPair( HPair const& ) = default;
	constexpr HPair( HPair&& ) = default;

	template<typename alt_first_t, typename = typename trait::enable_if<trait::is_convertible<alt_first_t, first_type>::value>::type>
	HPair( alt_first_t&& key_, second_type const& value_ )
		: first( yaal::forward<alt_first_t>( key_ ) ), second( value_ ) {
		return;
	}
	template<typename alt_second_t, typename = typename trait::enable_if<trait::is_convertible<alt_second_t, second_type>::value>::type>
	HPair( HPair<first_type, alt_second_t>&& altPair_ )
		: first( altPair_.first ), second( yaal::forward<alt_second_t>( altPair_.second ) ) {
		return;
	}

	template<
		typename alt_first_t,
		typename alt_second_t,
		typename = typename trait::enable_if<
			trait::and_op<
				typename trait::is_convertible<alt_first_t, first_type>,
				typename trait::is_convertible<alt_second_t, second_type>
			>::value
		>
	>
	HPair( alt_first_t&& key_, alt_second_t&& value_ )
		: first( yaal::forward<alt_first_t>( key_ ) ), second( yaal::forward<alt_second_t>( value_ ) ) {
		return;
	}

	template<
		typename alt_first_t,
		typename alt_second_t,
		typename = typename trait::enable_if<
			trait::and_op<
				typename trait::is_convertible<alt_first_t, first_type>,
				typename trait::is_convertible<alt_second_t, second_type>
			>::value
		>
	>
	HPair( HPair<alt_first_t, alt_second_t>&& altPair_ )
		: first( yaal::forward<alt_first_t>( altPair_.first ) ), second( yaal::forward<alt_second_t>( altPair_.second ) ) {
		return;
	}

	HPair& operator = ( HPair const& pair_ ) {
		M_PROLOG
		if ( &pair_ != this ) {
			HPair tmp( pair_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	HPair& operator = ( HPair&& pair_ ) {
		M_PROLOG
		if ( &pair_ != this ) {
			swap( pair_ );
			first_type f{};
			second_type s{};
			using yaal::swap;
			swap( pair_.first, f );
			swap( pair_.second, s );
		}
		return ( *this );
		M_EPILOG
	}
	bool operator == ( HPair const& pair_ ) const {
		return ( ( first == pair_.first ) && ( second == pair_.second ) );
	}
	bool operator != ( HPair const& pair_ ) const {
		return ( ! operator == ( pair_ ) );
	}
	bool operator < ( HPair const& pair_ ) const {
		return ( ( first < pair_.first )
				|| ( ! ( pair_.first < first ) && ( second < pair_.second ) ) );
	}
	void swap( HPair& pair_ ) noexcept {
		if ( &pair_ != this ) {
			using yaal::swap;
			swap( first, pair_.first );
			swap( second, pair_.second );
		}
		return;
	}
};

template<typename first_type, typename second_type>
inline HPair<typename trait::decay<first_type>::type, typename trait::decay<second_type>::type> make_pair( first_type&& first_, second_type&& second_ ) {
	typedef HPair<typename trait::decay<first_type>::type, typename trait::decay<second_type>::type> pair_t;
	return ( pair_t( yaal::forward<first_type>( first_ ), yaal::forward<second_type>( second_ ) ) );
}

template<typename first_type, typename second_type>
inline void swap( yaal::hcore::HPair<first_type, second_type>& a, yaal::hcore::HPair<first_type, second_type>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HPAIR_HXX_INCLUDED */


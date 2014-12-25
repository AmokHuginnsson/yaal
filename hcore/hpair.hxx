/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpair.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
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
class HPair {
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
	HPair( first_type const& key_, typename trait::strip_reference<second_type>::type&& value_ )
		: first( key_ ), second( yaal::move( value_ ) ) {
		return;
	}
	HPair( typename trait::strip_reference<first_type>::type&& key_, second_type const& value_ )
		: first( yaal::move( key_ ) ), second( value_ ) {
		return;
	}
	HPair( typename trait::strip_reference<first_type>::type&& key_, typename trait::strip_reference<second_type>::type&& value_ )
		: first( yaal::move( key_ ) ), second( yaal::move( value_ ) ) {
		return;
	}
	HPair( HPair const& pair_ )
		: first( pair_.first ), second( pair_.second ) {
		return;
	}
	HPair( HPair&& pair_ )
		: first( yaal::move( pair_.first ) ), second( yaal::move( pair_.second ) ) {
		return;
	}
	template<typename alt_first_t, typename alt_second_t>
	HPair( HPair<alt_first_t, alt_second_t> const& altPair_ )
		: first( altPair_.first ), second( altPair_.second ) {
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
	void swap( HPair& pair_ ) {
		if ( &pair_ != this ) {
			using yaal::swap;
			swap( first, pair_.first );
			swap( second, pair_.second );
		}
		return;
	}
};

template<typename first_type, typename second_type>
inline HPair<first_type, second_type> make_pair( first_type const& first_, second_type const& second_ ) {
	return ( HPair<first_type, second_type>( first_, second_ ) );
}

template<typename first_type, typename second_type>
inline HPair<first_type, second_type> make_pair( first_type const& first_, typename trait::strip_reference<second_type>::type&& second_ ) {
	return ( HPair<first_type, second_type>( first_, yaal::move( second_ ) ) );
}

template<typename first_type, typename second_type>
inline HPair<first_type, second_type> make_pair( typename trait::strip_reference<first_type>::type&& first_, second_type const& second_ ) {
	return ( HPair<first_type, second_type>( yaal::move( first_ ), second_ ) );
}

template<typename first_type, typename second_type>
inline HPair<first_type, second_type> make_pair( typename trait::strip_reference<first_type>::type&& first_, typename trait::strip_reference<second_type>::type&& second_ ) {
	return ( HPair<first_type, second_type>( yaal::move( first_ ), yaal::move( second_ ) ) );
}

template<typename first_type, typename second_type>
inline void swap( yaal::hcore::HPair<first_type, second_type>& a, yaal::hcore::HPair<first_type, second_type>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HPAIR_HXX_INCLUDED */


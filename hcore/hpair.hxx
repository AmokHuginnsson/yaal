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
	HPair( void ) : first(), second() {}
	HPair( first_type const& key, second_type const& value ) : first( key ), second( value ) {}
	HPair( HPair const& pair ) : first( pair.first ), second( pair.second ) {}
	template<typename alt_first_t, typename alt_second_t>
	HPair( HPair<alt_first_t, alt_second_t> const& p ) : first( p.first ), second( p.second ) {}
	HPair& operator = ( HPair const& pair ) {
		M_PROLOG
		if ( &pair != this ) {
			HPair tmp( pair );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	bool operator == ( HPair const& pair ) const
		{	return ( ( first == pair.first ) && ( second == pair.second ) );	}
	bool operator != ( HPair const& pair ) const
		{	return ( ! operator == ( pair ) );	}
	bool operator < ( HPair const& pair ) const {
		return ( ( first < pair.first )
				|| ( ! ( pair.first < first ) && ( second < pair.second ) ) );
	}
	void swap( HPair& pair ) {
		if ( &pair != this ) {
			using yaal::swap;
			swap( first, pair.first );
			swap( second, pair.second );
		}
		return;
	}
};

template<typename first_type, typename second_type>
HPair<first_type, second_type> make_pair( first_type const& first, second_type const& second )
	{ return ( HPair<first_type, second_type>( first, second ) ); }

template<typename first_type, typename second_type>
inline void swap( yaal::hcore::HPair<first_type, second_type>& a, yaal::hcore::HPair<first_type, second_type>& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HPAIR_HXX_INCLUDED */


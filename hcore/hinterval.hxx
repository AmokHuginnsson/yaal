/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hinterval.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hinterval.hxx
 * \brief Declaration and implementation of HInterval<> class template.
 */

#ifndef YAAL_HCORE_HINTERVAL_HXX_INCLUDED
#define YAAL_HCORE_HINTERVAL_HXX_INCLUDED 1

#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* _errMsgHInterval_[];

template<typename value_t>
class HInterval {
public:
	typedef HInterval<value_t> this_type;
	typedef value_t value_type;
private:
	/*! \brief Error type of HInterval<> operations.
	 */
	struct ERROR {
		/*! \brief Error type flags.
		 */
		typedef enum {
			OK = 0, /*!< no error. */
			REVERSED_EDNPOINTS = 1,
			DIVISION_BY_ZERO = 2
		} error_t;
	};
	value_type _infimum;
	value_type _supremum;
public:
	HInterval( void )
		: _infimum(), _supremum() {}
	HInterval( value_type const& begin_, value_type const& end_ )
		: _infimum( begin_ ), _supremum( end_ ) {
		if ( _supremum < _infimum )
			M_THROW( _errMsgHInterval_[ERROR::REVERSED_EDNPOINTS], static_cast<int>( _infimum - _supremum ) );
	}
	value_type inf( void ) const {
		return ( _infimum );
	}
	value_type sup( void ) const {
		return ( _supremum );
	}
	HInterval& operator += ( HInterval const& interval_ ) {
		_infimum += interval_._infimum;
		_supremum += interval_._supremum;
		return ( *this );
	}
	HInterval& operator -= ( HInterval const& interval_ ) {
		_infimum -= interval_._supremum;
		_supremum -= interval_._infimum;
		return ( *this );
	}
	HInterval& operator *= ( HInterval const& interval_ ) {
		_infimum = min(
				min( _infimum * interval_._infimum, _infimum * interval_._supremum ),
				min( _supremum * interval_._infimum, _supremum * interval_._supremum ) );
		_supremum = max(
				max( _infimum * interval_._infimum, _infimum * interval_._supremum ),
				max( _supremum * interval_._infimum, _supremum * interval_._supremum ) );
		return ( *this );
	}
	HInterval& operator /= ( HInterval const& interval_ ) {
		if ( ( ( interval_._infimum < 0 ) && ( interval_._supremum > 0 ) )
				|| ( interval_._infimum == 0 ) || ( interval_._supremum == 0 ) )
			M_THROW( _errMsgHInterval_[ERROR::DIVISION_BY_ZERO], 0 );
		_infimum = min(
				min( _infimum / interval_._infimum, _infimum / interval_._supremum ),
				min( _supremum / interval_._infimum, _supremum / interval_._supremum ) );
		_supremum = max(
				max( _infimum / interval_._infimum, _infimum / interval_._supremum ),
				max( _supremum / interval_._infimum, _supremum / interval_._supremum ) );
		return ( *this );
	}
	HInterval& operator ++ ( void ) {
		++ _infimum;
		++ _supremum;
		return ( *this );
	}
	HInterval& operator -- ( void ) {
		-- _infimum;
		-- _supremum;
		return ( *this );
	}
	HInterval operator + ( HInterval const& interval_ ) const {
		HInterval interval( *this );
		return ( interval += interval_ );
	}
	HInterval operator - ( HInterval const& interval_ ) const {
		HInterval interval( *this );
		return ( interval -= interval_ );
	}
	HInterval operator * ( HInterval const& interval_ ) const {
		HInterval interval( *this );
		return ( interval *= interval_ );
	}
	HInterval operator / ( HInterval const& interval_ ) const {
		HInterval interval( *this );
		return ( interval /= interval_ );
	}
	HInterval operator ++ ( int ) const {
		HInterval interval( *this );
		return ( ++ interval );
	}
	HInterval operator -- ( int ) const {
		HInterval interval( *this );
		return ( -- interval );
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HINTERVAL_HXX_INCLUDED */


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  huniquemovable.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/huniquemovable.hxx
 * \brief Declaration and definition of HUniqueMovable<> class template.
 */

#ifndef YAAL_HCORE_HUNIQUEMOVABLE_HXX_INCLUDED
#define YAAL_HCORE_HUNIQUEMOVABLE_HXX_INCLUDED 1

#include <cstring>

#include "hcore/hexception.hxx"
#include "hcore/algorithm_low.hxx"

namespace yaal {

namespace hcore {

/*! \brief Keep memory movable object on stack and ensure its uniqeness.
 */
template<typename tType>
class HUniqueMovable final {
public:
	typedef tType value_type;
private:
	char _mem[sizeof ( value_type )];
	bool _owner;
public:
	template<typename... arg_t>
	HUniqueMovable( arg_t&&... arg_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( yaal::forward<arg_t>( arg_ )... );
		M_EPILOG
	}
	HUniqueMovable( HUniqueMovable&& um_ )
		: _mem(), _owner( true ) {
		::memcpy( _mem, um_._mem, sizeof ( value_type ) );
		um_._owner = false;
	}
	~HUniqueMovable( void ) {
		reset();
	}
/* cppcheck-suppress operatorEqVarError */
	HUniqueMovable& operator = ( HUniqueMovable&& um_ ) {
		M_PROLOG
		if ( &um_ != this ) {
			reset();
			if ( um_._owner ) {
				::memcpy( _mem, um_._mem, sizeof ( value_type ) );
				um_._owner = false;
				_owner = true;
			}
		}
		return ( *this );
		M_EPILOG
	}
	void reset( void ) {
		if ( _owner ) {
			M_SAFE( value_type* v( static_cast<value_type*>( static_cast<void*>( _mem ) ) ); v->~value_type() );
			_owner = false;
		}
	}
	value_type& operator*( void ) {
		M_ASSERT( _owner );
		return ( *static_cast<value_type*>( static_cast<void*>( _mem ) ) );
	}
	value_type const& operator*( void ) const {
		M_ASSERT( _owner );
		return ( *static_cast<value_type*>( static_cast<void*>( _mem ) ) );
	}
	value_type* operator->( void ) {
		M_ASSERT( _owner );
		return ( static_cast<value_type*>( static_cast<void*>( _mem ) ) );
	}
	value_type const* operator->( void ) const {
		M_ASSERT( _owner );
		return ( static_cast<value_type*>( static_cast<void*>( _mem ) ) );
	}
	bool has_ownership( void ) const {
		return ( _owner );
	}
private:
	HUniqueMovable( HUniqueMovable const& ) = delete;
	HUniqueMovable& operator = ( HUniqueMovable const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_HCORE_HUNIQUEMOVABLE_HXX_INCLUDED */


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

namespace yaal {

namespace hcore {

/*! \brief Provide reference semantics for HUniqueMovable<>.
 */
template<typename tType>
struct HUniqueMovableRef {
	char _mem[sizeof ( tType )];
	bool _owner;
	explicit HUniqueMovableRef( tType* obj_, bool owner_ )
		: _mem(), _owner( owner_ ) {
		if ( owner_ )
			::memcpy( static_cast<void*>( _mem ), static_cast<void*>( obj_ ), sizeof ( tType ) );
	}
};

/*! \brief Keep memory movable object on stack and ensure its uniqeness.
 */
template<typename tType>
class HUniqueMovable {
public:
	typedef tType value_type;
private:
	char _mem[sizeof ( value_type )];
	bool _owner;
public:
	HUniqueMovable( void )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type();
		M_EPILOG
	}
	template<typename a0_t>
	HUniqueMovable( a0_t a0_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t>
	HUniqueMovable( a0_t a0_, a1_t a1_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_, a6_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_, a8_t a8_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t, typename a9_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_, a8_t a8_, a9_t a9_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_ );
		M_EPILOG
	}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t, typename a9_t, typename a10_t>
	HUniqueMovable( a0_t a0_, a1_t a1_, a2_t a2_, a3_t a3_, a4_t a4_, a5_t a5_, a6_t a6_, a7_t a7_, a8_t a8_, a9_t a9_, a10_t a10_ )
		: _mem(), _owner( true ) {
		M_PROLOG
		new ( _mem ) value_type( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_ );
		M_EPILOG
	}
	HUniqueMovable( HUniqueMovable& um_ )
		: _mem(), _owner( true ) {
		::memcpy( _mem, um_._mem, sizeof ( value_type ) );
		um_._owner = false;
	}
	HUniqueMovable( HUniqueMovableRef<tType> um_ )
		: _mem(), _owner( um_._owner ) {
		if ( um_._owner )
			::memcpy( _mem, um_._mem, sizeof ( value_type ) );
	}
	~HUniqueMovable( void ) {
		reset();
	}
/* cppcheck-suppress operatorEqVarError */
	HUniqueMovable& operator = ( HUniqueMovable& um_ ) {
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
	HUniqueMovable& operator = ( HUniqueMovableRef<tType> um_ ) {
		M_PROLOG
		reset();
		if ( um_._owner ) {
			::memcpy( _mem, um_._mem, sizeof ( value_type ) );
			_owner = true;
		}
		return ( *this );
		M_EPILOG
	}
	void reset( void ) {
		if ( _owner ) {
			M_SAFE( static_cast<value_type*>( static_cast<void*>( _mem ) )->~value_type() );
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
	operator HUniqueMovableRef<value_type>( void ) {
		HUniqueMovableRef<value_type> ref( static_cast<value_type*>( static_cast<void*>( _mem ) ), _owner );
		_owner = false;
		return ( ref );
	}
	bool has_ownership( void ) const {
		return ( _owner );
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HUNIQUEMOVABLE_HXX_INCLUDED */


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hexclusiveaccessor.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hexclusiveaccessor.hxx
 * \brief Declaration and definition of HExclusiveAccessor<> class template.
 */

#ifndef YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED
#define YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED 1

#include "hcore/hthread.hxx"

namespace yaal {

namespace tools {

template<typename T>
class HExclusiveAccessor {
	T _object;
	yaal::hcore::external_lock_t _lock;
public:
	HExclusiveAccessor( yaal::hcore::external_lock_t lock_, T object_ )
		: _object( yaal::move( object_ ) ), _lock( yaal::move( lock_ ) ) {
		return;
	}
	HExclusiveAccessor( HExclusiveAccessor&& ea_ )
		: _object( yaal::move( ea_._object ) ), _lock( yaal::move( ea_._lock ) ) {
		return;
	}
	template<typename real_t>
	HExclusiveAccessor( HExclusiveAccessor<real_t>&& ea_ )
		: _object( yaal::move( ea_._object ) ), _lock( yaal::move( ea_._lock ) ) {
		return;
	}
	HExclusiveAccessor& operator = ( HExclusiveAccessor& ea_ ) {
		if ( &ea_ != this ) {
			_object = yaal::move( ea_._object );
			_lock = yaal::move( ea_._lock );
		}
		return ( *this );
	}
	T& operator->( void ) {
		M_ASSERT( _lock.has_ownership() );
		return ( _object );
	}
	T const& operator->( void ) const {
		M_ASSERT( _lock.has_ownership() );
		return ( _object );
	}
	T& operator*( void ) {
		M_ASSERT( _lock.has_ownership() );
		return ( *_object );
	}
	T const& operator*( void ) const {
		M_ASSERT( _lock.has_ownership() );
		return ( *_object );
	}
	template<typename real_t>
	operator HExclusiveAccessor<real_t>( void ) {
		HExclusiveAccessor<real_t> ea( _lock, _object );
		return ( ea );
	}
private:
	HExclusiveAccessor( HExclusiveAccessor const& ) = delete;
	HExclusiveAccessor& operator = ( HExclusiveAccessor const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED */


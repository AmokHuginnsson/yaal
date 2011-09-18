/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hexclusiveaccessor.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

/*! \brief Implementation of automatic exclusive access concept.
 */
template<typename T>
struct HExclusiveAccessorRef {
	T _object;
	yaal::hcore::external_lock_t _lock;
	HExclusiveAccessorRef( yaal::hcore::external_lock_t lock_, T object_ )
		: _object( object_ ), _lock( lock_ ) { }
private:
	HExclusiveAccessorRef( HExclusiveAccessorRef const& );
};

template<typename T>
class HExclusiveAccessor {
	T _object;
	yaal::hcore::external_lock_t _lock;
public:
	HExclusiveAccessor( yaal::hcore::external_lock_t lock_, T object_ )
		: _object( object_ ), _lock( lock_ ) { }
	HExclusiveAccessor( HExclusiveAccessor& ea_ )
		: _object( ea_._object ), _lock( ea_._lock ) { }
	template<typename real_t>
	HExclusiveAccessor( HExclusiveAccessor<real_t>& ea_ )
		: _object( ea_._object ), _lock( ea_._lock ) { }
	HExclusiveAccessor( HExclusiveAccessorRef<T> ear_ )
		: _object( ear_._object ), _lock( ear_._lock ) { }
	HExclusiveAccessor& operator = ( HExclusiveAccessor& ea_ ) {
		if ( &ea_ != this ) {
			_object = ea_._object;
			_lock = ea_._lock;
		}
		return ( *this );
	}
	HExclusiveAccessor& operator = ( HExclusiveAccessorRef<T> ea_ ) {
		_object = ea_._object;
		_lock = ea_._lock;
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
	template<typename real_t>
	operator HExclusiveAccessorRef<real_t>( void ) {
		HExclusiveAccessorRef<real_t> ref( _lock, _object );
		return ( ref );
	}
};

}

}

#endif /* #ifndef YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED */


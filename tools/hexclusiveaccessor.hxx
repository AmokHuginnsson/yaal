/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hexclusiveaccessor.hxx
 * \brief Declaration and definition of HExclusiveAccessor<> class template.
 */

#ifndef YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED
#define YAAL_TOOLS_HEXCLUSIVEACCESSOR_HXX_INCLUDED 1

#include "hcore/hthread.hxx"

namespace yaal {

namespace tools {

template<typename T>
class HExclusiveAccessor final {
	T _object;
	yaal::hcore::HLock _lock;
public:
	HExclusiveAccessor( yaal::hcore::HLock lock_, T object_ )
		: _object( yaal::move( object_ ) )
		, _lock( yaal::move( lock_ ) ) {
		return;
	}
	HExclusiveAccessor( HExclusiveAccessor&& ea_ )
		: _object( yaal::move( ea_._object ) )
		, _lock( yaal::move( ea_._lock ) ) {
		return;
	}
	template<typename real_t>
	HExclusiveAccessor( HExclusiveAccessor<real_t>&& ea_ )
		: _object( yaal::move( ea_._object ) )
		, _lock( yaal::move( ea_._lock ) ) {
		return;
	}
	HExclusiveAccessor& operator = ( HExclusiveAccessor&& ea_ ) {
		if ( &ea_ != this ) {
			_object = yaal::move( ea_._object );
			_lock = yaal::move( ea_._lock );
		}
		return ( *this );
	}
	T& operator->( void ) {
		M_ASSERT( _lock.owns_lock() );
		return ( _object );
	}
	T const& operator->( void ) const {
		M_ASSERT( _lock.owns_lock() );
		return ( _object );
	}
	T& operator*( void ) {
		M_ASSERT( _lock.owns_lock() );
		return ( *_object );
	}
	T const& operator*( void ) const {
		M_ASSERT( _lock.owns_lock() );
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


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hresource.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hresource.hxx
 * \brief Declaration of HResource<> class.
 *
 * Scoped resource guardian with definable resource free freeing function.
 */

#ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED
#define YAAL_HCORE_HRESOURCE_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace hcore {

namespace resource_helper {

template<typename T>
struct HResourceDeleter {
	void operator()( T* val_ ) {
		M_SAFE( delete val_ );
	}
};

template<typename T>
struct HResourceDeleter<T[]> {
	void operator()( T* val_ ) {
		M_SAFE( delete [] val_ );
	}
};

template<typename T>
struct trait {
	typedef T type;
	static bool const is_array = false;
};

template<typename T>
struct trait<T[]> {
	typedef T type;
	static bool const is_array = true;
};

template<typename T, typename deleter, bool const default_deleter>
class HResourceHolder;

template<typename T, typename deleter>
class HResourceHolder<T, deleter, true> {
protected:
	T* _resource;
public:
	HResourceHolder( void )
		: _resource( nullptr ) {
	}
	HResourceHolder( T* val_ )
		: _resource( val_ ) {
	}
	void do_delete( void ) {
		deleter()( _resource );
	}
	void swap( HResourceHolder& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _resource, other._resource );
		}
		return;
	}
};

template<typename T, typename deleter>
class HResourceHolder<T, deleter, false> {
protected:
	T* _resource;
	deleter _deleter;
public:
	HResourceHolder( void )
		: _resource( nullptr ), _deleter() {
	}
	HResourceHolder( T* val_, deleter deleter_ )
		: _resource( val_ ), _deleter( deleter_ ) {
	}
	void do_delete( void ) {
		_deleter( _resource );
	}
	void swap( HResourceHolder& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _resource, other._resource );
			swap( _deleter, other._deleter );
		}
		return;
	}
};

}

/*! \brief Raw resource life time tracker.
 */
template<typename type_t, typename free_t = resource_helper::HResourceDeleter<type_t>>
class HResource
	: public resource_helper::HResourceHolder<
		typename resource_helper::trait<type_t>::type,
		free_t,
		trait::same_type<free_t, resource_helper::HResourceDeleter<type_t>>::value
	> {
public:
	typedef resource_helper::HResourceHolder<
		typename resource_helper::trait<type_t>::type,
		free_t,
		trait::same_type<free_t, resource_helper::HResourceDeleter<type_t>>::value
	> base_type;
	typedef typename resource_helper::trait<type_t>::type value_type;
	typedef typename trait::make_reference<typename trait::strip_const<value_type>::type>::type reference;
	typedef typename trait::make_reference<value_type const>::type reference_const;
	typedef value_type* pointer;
	typedef value_type const* pointer_const;
private:
	using base_type::_resource;
	using base_type::do_delete;
public:
	HResource( void )
		: base_type() {
		return;
	}
	explicit HResource( pointer resource_ )
		: base_type( resource_ ) {
		return;
	}
	HResource( pointer resource_, free_t free_ )
		: base_type( resource_, free_ ) {
		return;
	}
	~HResource( void ) {
		reset();
		return;
	}
	HResource( HResource const& ) = delete;
	HResource( HResource&& src_ )
		: base_type() {
		pass( yaal::move( src_ ) );
		return;
	}
	template<typename real_t>
	HResource( HResource<real_t, free_t>&& src_ )
		: base_type() {
		pass( yaal::move( src_ ) );
		return;
	}
	HResource& operator = ( HResource const& ) = delete;
	HResource& operator = ( HResource&& src_ ) {
		if ( &src_ != this ) {
			pass( yaal::move( src_ ) );
		}
		return ( *this );
	}
	template<typename real_t>
	HResource& operator = ( HResource<real_t, free_t>&& src_ ) {
		if ( &reinterpret_cast<HResource&>( src_ ) != this ) {
			pass( yaal::move( src_ ) );
		}
		return ( *this );
	}
	reference_const operator*( void ) const {
		static_assert( !resource_helper::trait<type_t>::is_array, "indirection operator is called for an array" );
		M_ASSERT( _resource );
		return ( *_resource );
	}
	reference operator*( void ) {
		static_assert( !resource_helper::trait<type_t>::is_array, "indirection operator is called for an array" );
		M_ASSERT( _resource );
		return ( *_resource );
	}
	pointer_const operator->( void ) const {
		static_assert( !resource_helper::trait<type_t>::is_array, "structure dereference operator is called for an array" );
		M_ASSERT( _resource );
		return ( _resource );
	}
	pointer operator->( void ) {
		static_assert( !resource_helper::trait<type_t>::is_array, "structure dereference operator is called for an array" );
		M_ASSERT( _resource );
		return ( _resource );
	}
	reference_const operator[] ( int index_ ) const {
		static_assert( resource_helper::trait<type_t>::is_array, "array subscript operator is called for a scalar" );
		M_ASSERT( _resource );
		return ( _resource[index_] );
	}
	reference operator[] ( int index_ ) {
		static_assert( resource_helper::trait<type_t>::is_array, "array subscript operator is called for a scalar" );
		M_ASSERT( _resource );
		return ( _resource[index_] );
	}
	pointer_const raw( void ) const {
		return ( _resource );
	}
	pointer raw( void ) {
		return ( _resource );
	}
	pointer_const get( void ) const {
		return ( _resource );
	}
	pointer get( void ) {
		return ( _resource );
	}
	void reset( void ) {
		if ( _resource != nullptr ) {
			do_delete();
			_resource = nullptr;
		}
		return;
	}
	pointer release( void ) {
		pointer val( _resource );
		_resource = nullptr;
		return ( val );
	}
	bool operator ! ( void ) const {
		return ( _resource == nullptr );
	}
	using base_type::swap;
private:
	template<typename alien_t>
	void pass( alien_t&& src_ ) {
		HResource& src = reinterpret_cast<HResource&>( src_ );
		using yaal::swap;
		base_type::swap( src );
		src_.reset();
		return;
	}
};

template<typename tType, typename... arg_t>
HResource<tType> make_resource( arg_t&&... arg_ ) {
	return ( HResource<tType>( new tType( yaal::forward<arg_t>( arg_ )... ) ) );
}

template<typename type_t, typename free_t>
inline void swap( yaal::hcore::HResource<type_t, free_t>& a,
		yaal::hcore::HResource<type_t, free_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED */


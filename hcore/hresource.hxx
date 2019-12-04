/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

template<typename T, typename deleter, bool const default_deleter>
struct OResourceHolder;

template<typename T, typename deleter>
struct OResourceHolder<T, deleter, true> {
	typedef T type;
	T* _resource;
	OResourceHolder( void )
		: _resource( nullptr ) {
	}
	OResourceHolder( T* val_ )
		: _resource( val_ ) {
	}
	void do_delete( void ) {
		deleter()( _resource );
	}
	template<typename real_t>
	void swap( real_t& other ) {
		T* tmp( _resource );
		_resource = static_cast<T*>( other._resource );
		other._resource = static_cast<typename real_t::type*>( tmp );
		return;
	}
	OResourceHolder( OResourceHolder const& ) = default;
	OResourceHolder& operator = ( OResourceHolder const& ) = default;
};

template<typename T, typename deleter>
struct OResourceHolder<T, deleter, false> {
	typedef T type;
	T* _resource;
	deleter _deleter;
	OResourceHolder( void )
		: _resource( nullptr ), _deleter() {
	}
	OResourceHolder( T* val_, deleter deleter_ )
		: _resource( val_ ), _deleter( deleter_ ) {
	}
	void do_delete( void ) {
		_deleter( _resource );
	}
	template<typename real_t>
	void swap( real_t& other ) {
		T* tmp( _resource );
		_resource = static_cast<T*>( other._resource );
		other._resource = static_cast<typename real_t::type*>( tmp );
		using yaal::swap;
		swap( _deleter, other._deleter );
		return;
	}
	OResourceHolder( OResourceHolder const& ) = default;
	OResourceHolder& operator = ( OResourceHolder const& ) = default;
};

}

/*! \brief Raw resource life time tracker.
 */
template<typename type_t, typename free_t = resource_helper::HResourceDeleter<type_t>>
class HResource final {
public:
	typedef typename trait::decay<type_t>::type decayed_type;
	typedef typename trait::ternary<trait::is_array<type_t>::value, typename trait::strip_pointer<decayed_type>::type, decayed_type>::type value_type;
	typedef typename trait::make_reference<typename trait::strip_const<value_type>::type>::type reference;
	typedef typename trait::make_reference<value_type const>::type reference_const;
	typedef value_type* pointer;
	typedef value_type const* pointer_const;
	typedef resource_helper::OResourceHolder<
		value_type,
		free_t,
		trait::same_type<free_t, resource_helper::HResourceDeleter<type_t>>::value
	> holder;
private:
	holder _holder;
public:
	HResource( void )
		: _holder() {
		return;
	}
	explicit HResource( pointer resource_ )
		: _holder( resource_ ) {
		return;
	}
	HResource( pointer resource_, free_t free_ )
		: _holder( resource_, free_ ) {
		return;
	}
	~HResource( void ) {
		reset();
		return;
	}
	HResource( HResource const& ) = delete;
	HResource( HResource&& src_ ) noexcept
		: _holder() {
		pass( yaal::move( src_ ) );
		return;
	}
	template<typename real_type_t, typename real_free_t>
	HResource( HResource<real_type_t, real_free_t>&& src_ )
		: _holder() {
		static_assert( trait::is_kind_of<real_type_t, type_t>::value, "Unrelated types." );
		static_assert(
			trait::same_type<real_free_t, free_t>::value
			|| (
				trait::same_type<free_t, resource_helper::HResourceDeleter<type_t>>::value
				&& trait::same_type<real_free_t, resource_helper::HResourceDeleter<real_type_t>>::value
			),
			"Incompatible deleters."
		);
		pass( yaal::move( src_ ) );
		return;
	}
	HResource& operator = ( HResource const& ) = delete;
	HResource& operator = ( HResource&& src_ ) noexcept {
		if ( &src_ != this ) {
			pass( yaal::move( src_ ) );
		}
		return ( *this );
	}
	template<typename real_type_t, typename real_free_t>
	HResource& operator = ( HResource<real_type_t, real_free_t>&& src_ ) {
		static_assert( trait::is_kind_of<real_type_t, type_t>::value, "Unrelated types." );
		static_assert(
			trait::same_type<real_free_t, free_t>::value
			|| (
				trait::same_type<free_t, resource_helper::HResourceDeleter<type_t>>::value
				&& trait::same_type<real_free_t, resource_helper::HResourceDeleter<real_type_t>>::value
			),
			"Incompatible deleters."
		);
		if ( &reinterpret_cast<HResource&>( src_ ) != this ) {
			pass( yaal::move( src_ ) );
		}
		return ( *this );
	}
	reference_const operator*( void ) const {
		static_assert( !trait::is_array<type_t>::value, "indirection operator is called for an array" );
		M_ASSERT( _holder._resource );
		return ( *_holder._resource );
	}
	reference operator*( void ) {
		static_assert( !trait::is_array<type_t>::value, "indirection operator is called for an array" );
		M_ASSERT( _holder._resource );
		return ( *_holder._resource );
	}
	pointer_const operator->( void ) const {
		static_assert( !trait::is_array<type_t>::value, "structure dereference operator is called for an array" );
		M_ASSERT( _holder._resource );
		return ( _holder._resource );
	}
	pointer operator->( void ) {
		static_assert( !trait::is_array<type_t>::value, "structure dereference operator is called for an array" );
		M_ASSERT( _holder._resource );
		return ( _holder._resource );
	}
	reference_const operator[] ( int index_ ) const {
		static_assert( trait::is_array<type_t>::value, "array subscript operator is called for a scalar" );
		M_ASSERT( _holder._resource );
		return ( _holder._resource[index_] );
	}
	reference operator[] ( int index_ ) {
		static_assert( trait::is_array<type_t>::value, "array subscript operator is called for a scalar" );
		M_ASSERT( _holder._resource );
		return ( _holder._resource[index_] );
	}
	pointer_const raw( void ) const {
		return ( _holder._resource );
	}
	pointer raw( void ) {
		return ( _holder._resource );
	}
	pointer_const get( void ) const {
		return ( _holder._resource );
	}
	pointer get( void ) {
		return ( _holder._resource );
	}
	void reset( pointer ptr_ = pointer() ) {
		holder old( _holder );
		_holder._resource = ptr_;
		if ( old._resource != nullptr ) {
			old.do_delete();
		}
		return;
	}
	pointer release( void ) {
		pointer val( _holder._resource );
		_holder._resource = nullptr;
		return ( val );
	}
	explicit operator bool ( void ) const {
		return ( _holder._resource != nullptr );
	}
	bool operator == ( HResource const& other_ ) const {
		return ( _holder._resource == other_._holder._resource );
	}
	bool operator != ( HResource const& other_ ) const {
		return ( _holder._resource != other_._holder._resource );
	}
	void swap( HResource& other_ ) {
		_holder.swap( other_._holder );
		return;
	}
private:
	template<typename alien_t>
	void pass( alien_t&& src_ ) {
		_holder.swap( src_._holder );
		src_.reset();
		return;
	}
	template<typename real_type_t, typename real_free_t>
	friend class HResource;
};

template<typename tType, typename... arg_t>
HResource<tType> make_resource( arg_t&&... arg_ ) {
	return ( HResource<tType>( new ( memory::yaal ) tType( yaal::forward<arg_t>( arg_ )... ) ) );
}

template<typename type_t, typename free_t>
inline void swap( yaal::hcore::HResource<type_t, free_t>& a,
		yaal::hcore::HResource<type_t, free_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED */


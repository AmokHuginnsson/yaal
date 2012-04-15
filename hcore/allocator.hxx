/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/allocator.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_ALLOCATOR_HXX_INCLUDED
#define YAAL_HCORE_ALLOCATOR_HXX_INCLUDED 1

#include "hcore/memory.hxx"
#include "hcore/hpool.hxx"

namespace yaal {

namespace allocator {

template<typename T>
struct system {
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template<typename U>
	struct rebind {
		typedef system<U> other;
	};
	system( void ) {
	}
	template<typename U>
	system( system<U> const& ) {
	}
	void swap( system& ) {
	}
	pointer allocate( size_type n ) {
		return ( reinterpret_cast<pointer>( ::operator new ( sizeof ( T ) * n, memory::yaal ) ) );
	}
	pointer allocate( size_type n, const_pointer ) {
		return ( reinterpret_cast<pointer>( ::operator new ( sizeof ( T ) * n, memory::yaal ) ) );
	}
	void deallocate( pointer p, size_type n ) {
		::operator delete ( p, memory::yaal );
	}
	pointer address( reference r ) const {
		return ( &r );
	}
	const_pointer address( const_reference s ) const {
		return ( &s );
	}
	size_type max_size( void ) const {
		return ( 0x1fffffff );
	}
/*! \brief true iff you can deallocate with system<Y> something allocated with system<X>.
 */
	template<typename U>
	bool operator == ( system<U> const& ) const {
		return ( true );
	}
	template<typename U>
	bool operator != ( system<U> const& ) const {
		return ( false );
	}
	void construct( pointer p, const_reference t ) {
		new ( p ) T( t );
	}
	void destroy( pointer p ) {
		p->~T();
	}
};

template<typename T>
struct pool {
	typedef T* pointer;
	typedef T const* const_pointer;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef yaal::hcore::HPool<T> pool_t;
	pool_t _pool;
	template<typename U>
	struct rebind {
		typedef pool<U> other;
	};
	pool( void )
		: _pool()
		{}
	template<typename U>
	pool( pool<U> const& )
		: _pool()
		{}
	void swap( pool& pool_ ) {
		using yaal::swap;
		swap( _pool, pool_._pool );
	}
	pointer allocate( size_type ) {
		return ( _pool.alloc() );
	}
	pointer allocate( size_type, const_pointer ) {
		return ( _pool.alloc() );
	}
	void deallocate( pointer p, size_type ) {
		_pool.free( p );
	}
	pointer address( reference r ) const {
		return ( &r );
	}
	const_pointer address( const_reference s ) const {
		return ( &s );
	}
	size_type max_size( void ) const {
		return ( 0x1fffffff );
	}
/*! \brief true iff you can deallocate with pool<Y> something allocated with pool<X>.
 */
	template<typename U>
	bool operator == ( pool<U> const& pool_ ) const {
		return ( this == &pool_ );
	}
	template<typename U>
	bool operator != ( pool<U> const& pool_ ) const {
		return ( this != &pool_ );
	}
	void construct( pointer p, const_reference t ) {
		new ( p ) T( t );
	}
	void destroy( pointer p ) {
		p->~T();
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_ALLOCATOR_HXX_INCLUDED */


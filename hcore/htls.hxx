/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/htls.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/htls.hxx
 * \brief Declaration/implementation of HTLS<> class.
 */

#ifndef YAAL_HCORE_HTLS_HXX_INCLUDED
#define YAAL_HCORE_HTLS_HXX_INCLUDED 1

#include "hcore/compat.hxx"
#include "hcore/memory.hxx"
#include "hcore/hdeque.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"

namespace yaal {

namespace hcore {

/*! \brief HTLS<> adds automation around handling Thread Local Storage objects.
 */
template<typename tType>
class HTLS {
public:
	typedef HTLS<tType> this_type;
	typedef yaal::hcore::HPointer<tType> ptr_t;
	typedef yaal::hcore::HDeque<ptr_t> instances_t;
	typedef yaal::hcore::HBoundCall<> constructor_t;
	typedef typename instances_t::iterator iterator;
	typedef typename instances_t::const_iterator const_iterator;
private:
	static CXX_EXTENSION_TLS tType* _tls;
	yaal::hcore::HMutex _mutex;
	instances_t _instances;
	constructor_t _constructor;
public:
	HTLS( void )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( void )>( &this_type::construct ), this ) ) {}
	void construct( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>() );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0>
	HTLS( T0 const& a0_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const& )>( &this_type::construct ), this, a0_ ) ) {}
	template<typename T0>
	void construct( T0 const& a0_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1>
	HTLS( T0 const& a0_, T1 const& a1_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const& )>( &this_type::construct ), this, a0_, a1_ ) ) {}
	template<typename T0, typename T1>
	void construct( T0 const& a0_, T1 const& a1_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const& )>( &this_type::construct ), this, a0_, a1_, a2_ ) ) {}
	template<typename T0, typename T1, typename T2>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const&, T6 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_, a6_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_, a6_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const&, T6 const&, T7 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const&, T6 const&, T7 const&, T8 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_, T9 const& a9_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const&, T6 const&, T7 const&, T8 const&, T9 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_, T9 const& a9_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	HTLS( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_, T9 const& a9_, T10 const& a10_ )
		: _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const&, T3 const&, T4 const&, T5 const&, T6 const&, T7 const&, T8 const&, T9 const&, T10 const& )>( &this_type::construct ),
					this, a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_ ) ) {}
	template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	void construct( T0 const& a0_, T1 const& a1_, T2 const& a2_, T3 const& a3_, T4 const& a4_, T5 const& a5_, T6 const& a6_, T7 const& a7_, T8 const& a8_, T9 const& a9_, T10 const& a10_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! _tls );
		ptr_t p( make_pointer<tType>( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_ ) );
		_tls = p.raw();
		_instances.push_back( p );
		M_EPILOG
	}
	~HTLS( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		_instances.clear();
		_tls = NULL;
		M_DESTRUCTOR_EPILOG
	}
	external_lock_t acquire( void ) {
		M_PROLOG
		return ( external_lock_t( ref( _mutex ) ) );
		M_EPILOG
	}
	tType* operator->( void ) {
		M_PROLOG
		if ( ! _tls )
			_constructor();
		return ( _tls );
		M_EPILOG
	}
	tType& operator*( void ) {
		M_PROLOG
		if ( ! _tls )
			_constructor();
		return ( *_tls );
		M_EPILOG
	}
	const_iterator begin( void ) const {
		return ( _instances.begin() );
	}
/* cppcheck-suppress functionConst */
	iterator begin( void ) {
		return ( _instances.begin() );
	}
	const_iterator end( void ) const {
		return ( _instances.end() );
	}
/* cppcheck-suppress functionConst */
	iterator end( void ) {
		return ( _instances.end() );
	}
};
template<typename tType>
CXX_EXTENSION_TLS tType* HTLS<tType>::_tls = NULL;

}

}

#endif /* #ifndef YAAL_HCORE_HTLS_HXX_INCLUDED */


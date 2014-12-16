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
#include "hcore/hset.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"

namespace yaal {

namespace hcore {

template<typename tType>
class HTLS;

namespace tls {

typedef long int unsigned key_t;
typedef void ( *destruct_t )( void* );

key_t create( destruct_t );
void set( key_t, void const* );
void* get( key_t );
void free( key_t );

}

/*! \brief HTLS<> adds automation around handling Thread Local Storage objects.
 */
template<typename tType>
class HTLS : private trait::HNonCopyable {
public:
	typedef HTLS<tType> this_type;
	class HTLSHolder : private trait::HNonCopyable {
	public:
		typedef HTLSHolder this_type;
	protected:
		typedef HTLS<tType> owner_t;
		tType _object;
		owner_t* _owner;
	public:
		template<typename... arg_t>
		explicit HTLSHolder( owner_t* owner_, arg_t&&... arg_ )
			: _object( yaal::forward<arg_t>( arg_ )... ), _owner( owner_ ) {}
		virtual ~HTLSHolder( void ) {}
		operator tType const& ( void ) const
			{ return ( _object ); }
		operator tType& ( void )
			{ return ( _object ); }
		tType const* operator->( void ) const
			{ return ( &_object ); }
		tType* operator->( void )
			{ return ( &_object ); }
		template<typename assgnee_t>
		tType& operator = ( assgnee_t const& v_ ) {
			_object = v_;
			return ( _object );
		}
		owner_t* get_owner( void ) {
			return ( _owner );
		}
	private:
		HTLSHolder( HTLSHolder const& );
		HTLSHolder& operator = ( HTLSHolder const& );
	};
	typedef yaal::hcore::HResource<HTLSHolder> tls_holder_res_t;
	typedef yaal::hcore::HSet<HTLSHolder*> instances_t;
	typedef yaal::hcore::HBoundCall<> constructor_t;
	typedef typename instances_t::iterator iterator;
	typedef typename instances_t::const_iterator const_iterator;
private:
	tls::key_t _key;
	yaal::hcore::HMutex _mutex;
	instances_t _instances;
	constructor_t _constructor;
public:
	template<typename... arg_t>
	HTLS( arg_t&&... arg_ )
		: _key( tls::create( &HTLS::destruct ) ), _mutex(), _instances(),
		_constructor( call( static_cast<void ( this_type::* )( arg_t... )>( &this_type::construct ), this, yaal::forward<arg_t>( arg_ )... ) ) {
		return;
	}
	template<typename... arg_t>
	void construct( arg_t&&... arg_ ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		M_ASSERT( ! tls::get( _key ) );
		tls_holder_res_t res( new ( memory::yaal ) HTLSHolder( this, yaal::forward<arg_t>( arg_ )... ) );
		typename instances_t::insert_result ir( _instances.insert( res.get() ) );
		tls::set( _key, res.get() );
		M_ASSERT( res->get_owner() );
		res.release();
		return;
		M_EPILOG
	}
	static void destruct( void* tls_ ) {
		M_PROLOG
		HTLSHolder* th( static_cast<HTLSHolder*>( tls_ ) );
		this_type* tls( th->get_owner() );
		yaal::hcore::HLock l( tls->_mutex );
		tls->_instances.erase( th );
		M_SAFE( delete th );
		M_EPILOG
	}
	virtual ~HTLS( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		for ( typename instances_t::iterator it( _instances.begin() ), endIt( _instances.end() ); it != endIt; ++ it ) {
			M_SAFE( delete *it );
		}
		_instances.clear();
		tls::free( _key );
		M_DESTRUCTOR_EPILOG
	}
	external_lock_t acquire( void ) {
		M_PROLOG
		return ( external_lock_t( ref( _mutex ) ) );
		M_EPILOG
	}
	tType* operator->( void ) {
		M_PROLOG
		if ( ! tls::get( _key ) )
			_constructor();
		return ( &static_cast<tType&>( *static_cast<HTLSHolder*>( tls::get( _key ) ) ) );
		M_EPILOG
	}
	tType& operator*( void ) {
		M_PROLOG
		if ( ! tls::get( _key ) )
			_constructor();
		return ( *static_cast<HTLSHolder*>( tls::get( _key ) ) );
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
	int long get_instance_count( void ) const {
		return ( _instances.get_size() );
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HTLS_HXX_INCLUDED */


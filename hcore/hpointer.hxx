/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpointer.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED
#define YAAL_HCORE_HPOINTER_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/trait.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

/* Due to extream vitality of this struct,
 * none of the methods are guarded. */

/*! \brief Types of reference counters.
 */
struct REFERENCE_COUNTER_TYPE {
	static int const STRICT = 0;
	static int const WEAK = 1;
};

template<typename tType>
class HPointerFromThisInterface;

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
class HPointer;

/*! \brief Pointer life time tracker access policy.
 */
template<typename tType, typename pointer_type_t>
struct HPointerStrict {
	static tType const* raw( tType const* pointer_ ) {
		return ( pointer_type_t::raw( pointer_ ) );
	}
	static tType* raw( tType* pointer_ ) {
		return ( pointer_type_t::raw( pointer_ ) );
	}
	static typename trait::make_reference<tType>::type object_at( tType const* pointer_, int index_ ) {
		return ( pointer_type_t::object_at( pointer_, index_ ) );
	}
	static typename trait::make_reference<tType>::type object_at( tType* pointer_, int index_ ) {
		return ( pointer_type_t::object_at( pointer_, index_ ) );
	}
	static tType const* pointer( tType const* pointer_ ) {
		return ( pointer_ );
	}
	static tType* pointer( tType* pointer_ ) {
		return ( pointer_ );
	}
	template<typename deleter_t>
	inline static void delete_pointee( deleter_t const& deleter_ )
		{ deleter_->do_delete(); }
	template<typename ptr_t>
	static void initialize_from_this( HPointerFromThisInterface<tType>* obj, ptr_t const& ptr ) {
		obj->initialize_observer( ptr );
		return;
	}
	template<typename ptr_t>
	static void initialize_from_this( void*, ptr_t const& ) {
		return;
	}
	static void inc_reference_counter( int* referenceCounter_ ) {
		++ referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ];
		++ referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}
	static void dec_reference_counter( int* referenceCounter_ ) {
		M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 );
		M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ] > 0 );
		-- referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ];
		-- referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}
};

/*! \brief Pointer's observer access policy.
 */
template<typename tType, typename pointer_type_t>
struct HPointerWeak {
	template<typename deleter_t>
	inline static void delete_pointee( deleter_t const& ) {}
	static void inc_reference_counter( int* referenceCounter_ ) {
		++ referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}
	static void dec_reference_counter( int* referenceCounter_ ) {
		M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ] > 0 );
		-- referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}
};

/*! \brief Pointer to scalar deletion policy.
 */
template<typename tType>
struct HPointerScalar {
	template<typename real_t>
	static void delete_pointee( tType* pointer_ ) {
		M_SAFE( delete static_cast<real_t*>( pointer_ ) );
	}
	static tType const* raw( tType const* pointer_ ) {
		return ( pointer_ );
	}
	static tType* raw( tType* pointer_ ) {
		return ( pointer_ );
	}
};

/*! \brief Pointer to array deletion policy.
 */
template<typename tType>
struct HPointerArray {
	template<typename real_t>
	static void delete_pointee( tType* pointer_ ) {
		M_SAFE( delete [] static_cast<real_t*>( pointer_ ) );
	}
	static tType const& object_at( tType const* pointer_, int index_ ) {
		return ( pointer_[ index_ ] );
	}
	static tType& object_at( tType* pointer_, int index_ ) {
		return ( pointer_[ index_ ] );
	}
};

struct pointer_helper;

/*! \brief Smart pointer, reference counting implementation.
 *
 * \tparam tType - object type which life time will be guarded.
 * \tparam pointer_type_t - select pointer type, one of HPointerScalar or HPointerArray.
 * \tparam access_type_t - defines pointer kind, one of HPointerStrict or HPointerWeak.
 */
template<typename tType, template<typename>class pointer_type_t = HPointerScalar,
				 template<typename, typename>class access_type_t = HPointerStrict>
class HPointer {
	class HSharedBase {
	protected:
		int _referenceCounter[ 2 ];
		tType* _object;
		HSharedBase( tType* object_ ) : _referenceCounter(), _object( object_ ) {
			_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 0;
			_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 0;
		}
		virtual ~HSharedBase( void ) {}
		virtual void do_delete( void ) = 0;
		friend struct pointer_helper;
		friend class HPointer;
		template<typename T1, typename T2>
		friend struct HPointerStrict;
	private:
		HSharedBase( HSharedBase const& );
		HSharedBase& operator = ( HSharedBase const& );
	};
	template<typename deleter_t>
	class HShared : protected HSharedBase {
		deleter_t DELETER;
		HShared( deleter_t const& deleter_, tType* object_ )
			: HSharedBase( object_ ), DELETER( deleter_ )
			{ }
		virtual ~HShared( void ) {}
		virtual void do_delete( void ) {
			DELETER( HSharedBase::_object );
			HSharedBase::_object = NULL;
		}
		friend struct pointer_helper;
		friend class HPointer;
		template<typename T1, typename T2>
		friend struct HPointerStrict;
	};
	/*
	 * WARNING!
	 *
	 * Pointer to guarded object must be part of each
	 * smart-pointer instance so smart-pointer instance
	 * type information is preserved.
	 *
	 * It is necessary for proper resolving vtable
	 * bases in up/down casting of smart-pointers
	 * of objects of classes from multiple-inheritance
	 * (virtual or non-virtual) hierarchies.
	 */
	/*
	 * WARNING 2!
	 *
	 * Shared struct (a reference counters) must be initialized
	 * first in HPointer constructor. In case of memory allocation
	 * failure an object pointee must still be NULL.
	 */
	HSharedBase* _shared;
	tType* _object;
public:
	typedef tType value_type;
	typedef typename trait::make_reference<tType>::type reference;
	typedef typename trait::make_reference<tType const>::type const_reference;
	typedef pointer_type_t<tType> pointer_type;
	HPointer( void )
		: _shared( NULL ), _object( NULL ) {
		return;
	}
	template<typename real_t>
	explicit HPointer( real_t* const pointer_ )
		: _shared( pointer_ ? new HShared<void (*)( tType* )>( &pointer_type::template delete_pointee<real_t>, pointer_ ) : NULL ), _object( pointer_ ) {
		M_ASSERT( pointer_ );
		_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 1;
		_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 1;
		access_type_t<tType, pointer_type_t<tType> >::initialize_from_this( pointer_, *this );
		return;
	}
	template<typename real_t, typename deleter_t>
	explicit HPointer( real_t* const pointer_,  deleter_t deleter_ )
		: _shared( pointer_ ? new HShared<deleter_t>( deleter_, pointer_ ) : NULL ), _object( pointer_ ) {
		M_ASSERT( pointer_ );
		_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 1;
		_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 1;
		access_type_t<tType, pointer_type_t<tType> >::initialize_from_this( pointer_, *this );
		return;
	}
	virtual ~HPointer( void ) {
		/* The make_pointer() helper part.
		 */
		if ( _object )
			reset();
		else if ( _shared ) {
			/* This path is reached only when tType constructor throws
			 * in make_pointer() helper function.
			 */
			delete _shared;
			_shared = NULL;
		}
		return;
	}
	HPointer( HPointer const& pointer_ )
		: _shared( NULL ), _object( NULL ) {
		acquire( pointer_ );
		return;
	}
	template<typename alien_t, template<typename, typename>class alien_access_t>
	HPointer( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ )
		: _shared( NULL ), _object( NULL ) {
		acquire( pointer_ );
		return;
	}
	HPointer& operator = ( HPointer const& pointer_ ) {
		acquire( pointer_ );
		return ( *this );
	}
	template<typename alien_t, template<typename, typename>class alien_access_t>
	HPointer& operator = ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) {
		acquire( pointer_ );
		return ( *this );
	}
	const_reference operator* ( void ) const {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		return ( *access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	reference operator* ( void ) {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		return ( *access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	const_reference operator[] ( int index_ ) const {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		M_ASSERT( index_ >= 0 );
		return ( access_type_t<tType, pointer_type_t<tType> >::object_at( _object, index_ ) );
	}
	reference operator[] ( int index_ ) {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		M_ASSERT( index_ >= 0 );
		return ( access_type_t<tType, pointer_type_t<tType> >::object_at( _object, index_ ) );
	}

	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator == ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) == alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator == ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer(_object ) == pointer_ );
	}
	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator != ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) != alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator != ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) != pointer_ );
	}

	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator < ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) < alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator < ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer(_object ) < pointer_ );
	}
	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator > ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) > alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator > ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer(_object ) > pointer_ );
	}

	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator <= ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) <= alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator <= ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer(_object ) <= pointer_ );
	}
	template<typename alien_t, template<typename, typename>class alien_access_t>
	bool operator >= ( HPointer<alien_t, pointer_type_t, alien_access_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) >= alien_access_t<alien_t, pointer_type_t<alien_t> >::pointer( static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) ) );
	}
	template<typename alien_t>
	bool operator >= ( alien_t const* const pointer_ ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer(_object ) >= pointer_ );
	}

	tType const* operator->( void ) const {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		return ( access_type_t<tType, pointer_type_t<tType> >::raw( _object ) );
	}
	tType* operator->( void ) {
		M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
		return ( access_type_t<tType, pointer_type_t<tType> >::raw( _object ) );
	}
	tType const* raw( void ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	tType* raw( void ) {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	tType const* get( void ) const {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	tType* get( void ) {
		return ( access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	bool operator! ( void ) const {
		return ( ! access_type_t<tType, pointer_type_t<tType> >::pointer( _object ) );
	}
	void swap( HPointer& p ) {
		if ( &p != this ) {
			using yaal::swap;
			swap( _shared, p._shared );
			swap( _object, p._object );
		}
		return;
	}
	void reset( void ) {
		_shared && release();
		_shared = NULL;
		_object = NULL;
		return;
	}
private:
	bool release( void ) throw() {
		M_ASSERT( _shared && _object );
		if ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] == 1 )
			access_type_t<tType, pointer_type_t<tType> >::delete_pointee( _shared );
		access_type_t<tType, pointer_type_t<tType> >::dec_reference_counter( _shared->_referenceCounter );
		if ( ! _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] ) {
			delete _shared;
			_shared = NULL;
		}
		return ( ! ( _shared && _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] ) );
	}
	template<typename alien_t, template<typename, typename> class alien_access_t>
	void acquire( HPointer<alien_t, pointer_type_t, alien_access_t> const& from ) {
		HPointer const& alien = reinterpret_cast<HPointer const&>( from );
		if ( ( &alien != this ) && ( _shared != alien._shared ) ) {
			M_ASSERT( ( ! ( _shared && alien._shared ) )
					|| ( ( _shared && alien._shared )
						&& ( _object != static_cast<alien_t*>( static_cast<void*>( alien._object ) ) ) ) );
			if ( _shared )
				release();
			if ( alien._shared && ( alien._shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) ) {
				access_type_t<tType, pointer_type_t<tType> >::inc_reference_counter( alien._shared->_referenceCounter );
				_shared = alien._shared;
				assign( _object, static_cast<alien_t*>( static_cast<void*>( alien._object ) ) );
			} else {
				_shared = NULL;
				_object = NULL;
			}
		}
		return;
	}
	void assign( tType*& to, tType* from ) {
		to = from;
		return;
	}
	template<typename alien_t>
	void assign( tType*& to, alien_t* from ) {
		to = dynamic_cast<tType*>( from );
		return;
	}
	friend struct pointer_helper;
};

/*! \brief Interface for concept of getting smart pointer from `this'.
 */
template<typename tType>
class HPointerFromThisInterface {
protected:
	typedef HPointer<tType, HPointerScalar, HPointerStrict> ptr_t;
	typedef HPointer<tType, HPointerScalar, HPointerWeak> weak_t;
	weak_t _selfObserver;
public:
	HPointerFromThisInterface( void )
		: _selfObserver() {
		return;
	}
	virtual ~HPointerFromThisInterface( void ){}
	ptr_t get_pointer( void ) {
		return ( _selfObserver );
	}
	ptr_t const get_pointer( void ) const {
		return ( _selfObserver );
	}
protected:
	void initialize_observer( ptr_t const& firstOwner_ ) {
		_selfObserver = firstOwner_;
		return;
	}
	friend struct HPointerStrict<tType, HPointerScalar<tType> >;
};

template<typename alien_t, typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
bool operator == ( alien_t const* const pointer_, HPointer<tType, pointer_type_t, access_type_t> const& smartPointer_ ) {
	return ( smartPointer_ == pointer_ );
}

template<typename alien_t, typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
bool operator != ( alien_t const* const pointer_, HPointer<tType, pointer_type_t, access_type_t> const& smartPointer_ ) {
	return ( smartPointer_ != pointer_ );
}

struct pointer_helper {
	template<typename to_t, typename from_t, template<typename>class pointer_type_t,
					 template<typename, typename>class access_type_t>
	static typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> do_static_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
		HPointer<to_t, pointer_type_t, access_type_t> to;
		if ( from_._object ) {
			to._shared = reinterpret_cast<typename HPointer<to_t, pointer_type_t, access_type_t>::HSharedBase*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			access_type_t<to_t, pointer_type_t<to_t> >::inc_reference_counter( to._shared->_referenceCounter );
		}
		return ( to );
	}

	template<typename to_t, typename from_t, template<typename>class pointer_type_t,
					 template<typename, typename>class access_type_t>
	static typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> do_dynamic_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
		HPointer<to_t, pointer_type_t, access_type_t> to;
		if ( dynamic_cast<to_t*>( from_._object ) ) {
			to._shared = reinterpret_cast<typename HPointer<to_t, pointer_type_t, access_type_t>::HSharedBase*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			access_type_t<to_t, pointer_type_t<to_t> >::inc_reference_counter( to._shared->_referenceCounter );
		}
		return ( to );
	}

	template<typename to_t, typename from_t, template<typename>class pointer_type_t,
					 template<typename, typename>class access_type_t>
	static typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> do_const_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
		HPointer<to_t, pointer_type_t, access_type_t> to;
		if ( dynamic_cast<to_t*>( from_._object ) ) {
			to._shared = reinterpret_cast<typename HPointer<to_t, pointer_type_t, access_type_t>::HSharedBase*>( from_._shared );
			to._object = const_cast<to_t*>( from_._object );
			access_type_t<to_t, pointer_type_t<to_t> >::inc_reference_counter( to._shared->_referenceCounter );
		}
		return ( to );
	}

	template<typename tType>
	class HSpaceHolderDeleter {
		char _mem[sizeof ( tType )];
	public:
		HSpaceHolderDeleter( void ) : _mem() {}
		void operator()( tType* ) {
			mem()->~tType();
		}
		tType* mem( void ) {
			return ( static_cast<tType*>( static_cast<void*>( _mem ) ) );
		}
	};

	template<typename tType>
	static tType* do_make_pointer_pre( HPointer<tType>& ptr_ ) {
		typedef HPointer<tType> ptr_t;
		ptr_._shared = new ( memory::yaal ) typename ptr_t::template HShared<HSpaceHolderDeleter<tType> >( HSpaceHolderDeleter<tType>(), static_cast<tType*>( NULL ) );
		return ( static_cast<typename ptr_t::template HShared<HSpaceHolderDeleter<tType> >*>( ptr_._shared )->DELETER.mem() );
	}

	template<typename tType>
	static void do_make_pointer_post( HPointer<tType>& ptr_ ) {
		typedef HPointer<tType> ptr_t;
		ptr_._object = static_cast<typename ptr_t::template HShared<HSpaceHolderDeleter<tType> >*>( ptr_._shared )->DELETER.mem();
		ptr_._shared->_object = ptr_._object;
		HPointerStrict<tType, HPointerScalar<tType> >::inc_reference_counter( ptr_._shared->_referenceCounter );
		HPointerStrict<tType, HPointerScalar<tType> >::initialize_from_this( ptr_._object, ptr_ );
		return;
	}

};

template<typename to_t, typename from_t, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> pointer_static_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
	return ( pointer_helper::do_static_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> pointer_dynamic_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
	return ( pointer_helper::do_dynamic_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t>
typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t> pointer_const_cast( HPointer<from_t, pointer_type_t, access_type_t> from_ ) {
	return ( pointer_helper::do_const_cast<to_t>( from_ ) );
}

template<typename tType>
HPointer<tType> make_pointer( void ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType();
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t>
HPointer<tType> make_pointer( a0_t const& a0_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_, a3_t const& a3_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_, a6_t const& a6_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_, a6_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_,
		a6_t const& a6_, a7_t const& a7_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_,
		a6_t const& a6_, a7_t const& a7_, a8_t const& a8_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t, typename a9_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_,
		a6_t const& a6_, a7_t const& a7_, a8_t const& a8_, a9_t const& a9_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename tType, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t, typename a9_t, typename a10_t>
HPointer<tType> make_pointer( a0_t const& a0_, a1_t const& a1_,
		a2_t const& a2_, a3_t const& a3_, a4_t const& a4_, a5_t const& a5_,
		a6_t const& a6_, a7_t const& a7_, a8_t const& a8_, a9_t const& a9_,
		a10_t const& a10_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( a0_, a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_ );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

}

template<typename tType, template<typename>class pointer_type_t,
	template<typename, typename>class access_type_t>
inline void swap( yaal::hcore::HPointer<tType, pointer_type_t, access_type_t>& a, yaal::hcore::HPointer<tType, pointer_type_t, access_type_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED */


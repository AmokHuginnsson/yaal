/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  valuereference.hxx - this file is integral part of `yaal' project.

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

/*! \file tools/huginn/valuereference.hxx
 * \brief Declaration and implementation of HValueReference<> class template.
 */

#ifndef YAAL_TOOLS_HUGINN_VALUEREFERENCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_VALUEREFERENCE_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/trait.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace tools {

namespace huginn {

/* Due to extreme vitality of this structure,
 * none of the methods are guarded. */

/*! \brief Types of reference counters.
 */
struct REFERENCE_COUNTER_TYPE {
	static int const HOLDER = 0;
	static int const OBSERVER = 1;
};

template<typename tType>
class HValueReference;

template<typename tType>
class HSharedBase {
public:
	typedef typename trait::decay<typename trait::strip_const<tType>::type>::type decayed_type;
	typedef typename trait::ternary<trait::is_array<tType>::value, typename trait::strip_pointer<decayed_type>::type, decayed_type>::type value_type;
	typedef typename trait::make_reference<value_type>::type reference;
	typedef typename trait::make_reference<value_type const>::type const_reference;
protected:
	int _referenceCounter[ 2 ];
	value_type* _object;
	HSharedBase( value_type* object_ )
		: _referenceCounter()
		, _object( object_ ) {
		_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] = 1;
		_referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ] = 1;
	}
	void inc_reference_counter( trait::true_type* ) {
		++ _referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ];
		++ _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ];
	}
	void inc_reference_counter( trait::false_type* ) {
		++ _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ];
	}
	void dec_reference_counter( trait::true_type* ) {
		M_ASSERT( _referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 );
		M_ASSERT( _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ] > 0 );
		-- _referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ];
		-- _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ];
	}
	void dec_reference_counter( trait::false_type* ) {
		M_ASSERT( _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ] > 0 );
		-- _referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ];
	}
	virtual ~HSharedBase( void ) {}
	void do_delete( trait::true_type* ) {
		do_delete();
	}
	void do_delete( trait::false_type* ) {}
	virtual void do_delete( void ) = 0;
	virtual void destroy( void ) {
		delete this;
	}
	friend struct value_reference_helper;
	template<typename>
	friend class HValueReferenceBase;
	template<typename>
	friend class HValueReference;
private:
	HSharedBase( HSharedBase const& );
	HSharedBase& operator = ( HSharedBase const& );
};

template<typename tType, typename deleter_t, typename allocator_t>
class HSharedDeleterAllocator : protected HSharedBase<tType> {
	deleter_t DELETER;
protected:
	allocator_t _allocator;
	template<typename input_allocator>
	HSharedDeleterAllocator( deleter_t const& deleter_, input_allocator const& allocator_, tType* object_ )
		: HSharedBase<tType>( object_ )
		, DELETER( deleter_ )
		, _allocator( allocator_ ) {
	}
	virtual void do_delete( void ) override {
		DELETER( HSharedBase<tType>::_object );
		HSharedBase<tType>::_object = nullptr;
	}
	virtual void destroy( void ) override {
		typedef typename allocator_t:: template rebind<HSharedDeleterAllocator>::other allocator_type;
		allocator_type( this->_allocator ).deallocate( this, 1 );
	}
	friend struct value_reference_helper;
	template<typename>
	friend class HValueReference;
};

template<typename tType, typename deleter_t, typename allocator_t>
class HSharedDeleterAllocator;

struct value_reference_helper {
	template<typename to_t, typename from_t>
	static typename yaal::tools::huginn::HValueReference<to_t> do_static_cast( HValueReference<from_t> const& from_ ) {
		HValueReference<to_t> to;
		if ( from_._shared->_object ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		}
		return ( to );
	}

	template<typename to_t, typename from_t>
	static typename yaal::tools::huginn::HValueReference<to_t> do_dynamic_cast( HValueReference<from_t> const& from_ ) {
		HValueReference<to_t> to;
		if ( dynamic_cast<to_t*>( from_._shared->_object ) ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		}
		return ( to );
	}

	template<typename to_t, typename from_t>
	static typename yaal::tools::huginn::HValueReference<to_t> do_const_cast( HValueReference<from_t> const& from_ ) {
		HValueReference<to_t> to;
		if ( dynamic_cast<to_t*>( from_._shared->_object ) ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		}
		return ( to );
	}

	template<typename tType>
	class HSpaceHolderDeleter {
		typedef typename memory::aligned<static_cast<int>( sizeof ( tType ) ), tType>::type aligner_t;
		aligner_t _mem;
	public:
		HSpaceHolderDeleter( void ) : _mem() {}
		void operator()( tType* ) {
			mem()->~tType();
		}
		tType* mem( void ) {
			return ( _mem.mem() );
		}
	};

};

template<typename tType>
class HValueReferenceBase {
public:
	typedef HSharedBase<tType> shared_t;
	typedef typename shared_t::value_type value_type;
	typedef typename shared_t::reference reference;
	typedef typename shared_t::const_reference const_reference;
protected:
	shared_t* _shared;
public:
	HValueReferenceBase( void )
		: _shared( nullptr ) {
		return;
	}
	HValueReferenceBase( shared_t* shared_ )
		: _shared( shared_ ) {
		return;
	}
	HValueReferenceBase( HValueReferenceBase const& ) = delete;
	HValueReferenceBase& operator = ( HValueReferenceBase const& ) = delete;
protected:
	~HValueReferenceBase( void ) {
		if ( _shared ) {
			/* This path is reached only when tType constructor throws
			 * in make_value() helper function.
			 */
			_shared->destroy();
			_shared = nullptr;
		}
		return;
	}
	template<typename type>
	void do_reset( void ) {
		_shared && release<type>();
		_shared = nullptr;
		return;
	}
	template<typename type, typename alien_t>
	void acquire( HValueReferenceBase<alien_t> const& from ) {
		HValueReferenceBase const& alien = reinterpret_cast<HValueReferenceBase const&>( from );
		if ( ( &alien != this ) && ( _shared != alien._shared ) ) {
			if ( _shared ) {
				release<type>();
			}
			if ( alien._shared && ( alien._shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) ) {
				alien._shared->inc_reference_counter( static_cast<type*>( nullptr ) );
				_shared = alien._shared;
			} else {
				_shared = nullptr;
			}
		}
		return;
	}
	template<typename type>
	bool release( void ) throw() {
		M_ASSERT( _shared );
		if ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] == 1 ) {
			_shared->do_delete( static_cast<type*>( nullptr ) );
		}
		_shared->dec_reference_counter( static_cast<type*>( nullptr ) );
		if ( ! _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ] ) {
			_shared->destroy();
			_shared = nullptr;
		}
		return ( ! ( _shared && _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] ) );
	}
private:
	friend struct value_reference_helper;
};

/*
 * Work-around for bug in GCC 4.8.4,
 * Superfluous warning about non-virtual destructor in base class.
 * In our case base class has `protected' destructor so this warning
 * makes no sense.
 */
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
template<typename tType>
class HValueReferenceObserver final : public HValueReferenceBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic error "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
public:
	typedef HSharedBase<tType> shared_t;
	HValueReferenceObserver( void )
		: HValueReferenceBase<tType>() {
		return;
	}
	HValueReferenceObserver( HValueReferenceObserver const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	template<typename alien_t>
	HValueReferenceObserver( HValueReferenceObserver<alien_t> const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	HValueReferenceObserver( HValueReferenceObserver&& other_ ) noexcept
		: HValueReferenceBase<tType>() {
		swap( other_ );
		return;
	}
	HValueReferenceObserver( HValueReference<tType> const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	~HValueReferenceObserver( void ) {
		/* The make_value() helper part.
		 */
		if ( this->_shared ) {
			reset();
		}
	}
	HValueReferenceObserver& operator = ( HValueReferenceObserver const& pointer_ ) {
		this->template acquire<trait::false_type>( pointer_ );
		return ( *this );
	}
	HValueReferenceObserver& operator = ( HValueReferenceObserver&& other_ ) noexcept {
		if ( & other_ != this ) {
			swap( other_ );
			other_.reset();
		}
		return ( *this );
	}
	template<typename alien_t>
	HValueReferenceObserver& operator = ( HValueReferenceObserver<alien_t> const& pointer_ ) {
		this->template acquire<trait::false_type>( pointer_ );
		return ( *this );
	}
	void reset( void ) {
		this->template do_reset<trait::false_type>();
	}
	void swap( HValueReferenceObserver& p ) {
		/*
		 * Both fields are POD types (pointers: tType*, HSharedBase*)
		 * so they do not have a specialized implementation
		 * and we can explicitly request yaal generic implementation.
		 */
		yaal::swap( this->_shared, p._shared );
		return;
	}
};

/*! \brief Smart pointer, reference counting implementation.
 *
 * \tparam tType - object type which life time will be guarded.
 * \tparam value_reference_type_t - select pointer type, one of HValueReferenceScalar or HValueReferenceArray.
 * \tparam access_type_t - defines pointer kind, one of HValueReferenceStrict or HValueReferenceWeak.
 */
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
template<typename tType>
class HValueReference final : public HValueReferenceBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic error "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
public:
	typedef typename HValueReferenceBase<tType>::value_type value_type;
	typedef typename HValueReferenceBase<tType>::reference reference;
	typedef typename HValueReferenceBase<tType const>::const_reference const_reference;
	typedef value_reference_helper::HSpaceHolderDeleter<tType> space_holder_deleter_t;
	HValueReference( void )
		: HValueReferenceBase<tType>() {
		return;
	}
	HValueReference( std::nullptr_t )
		: HValueReferenceBase<tType>() {
	}
	template<typename allocator_t, typename... args_t>
	explicit HValueReference( allocator_t const& allocator_, args_t&&... args_  )
		: HValueReferenceBase<tType>() {
		typedef typename allocator_t::template rebind<HSharedDeleterAllocator<tType, value_reference_helper::HSpaceHolderDeleter<tType>, allocator_t>>::other allocator_type;
		allocator_type allocator( allocator_ );
		this->_shared = allocator.allocate( 1 );
		new ( this->_shared ) HSharedDeleterAllocator<tType, value_reference_helper::HSpaceHolderDeleter<tType>, allocator_t>( value_reference_helper::HSpaceHolderDeleter<tType>(), allocator, static_cast<tType*>( nullptr ) );
		tType* ptr( static_cast<HSharedDeleterAllocator<tType, value_reference_helper::HSpaceHolderDeleter<tType>, allocator_t>*>( this->_shared )->DELETER.mem() );
		new ( ptr ) tType( yaal::forward<args_t>( args_ )... );
		this->_shared->_object = ptr;
		return;
	}
	/* Used only in HObject::~HObject() */
	template<typename deleter_t, typename allocator_t>
	explicit HValueReference( tType* pointer_,  deleter_t deleter_, allocator_t allocator_ )
		: HValueReferenceBase<tType>() {
		M_ASSERT( pointer_ );
		typedef typename allocator_t::template rebind<HSharedDeleterAllocator<tType, deleter_t, allocator_t>>::other allocator_type;
		allocator_type allocator( allocator_ );
		this->_shared = allocator.allocate( 1 );
		new ( this->_shared ) HSharedDeleterAllocator<tType, deleter_t, allocator_type>( deleter_, allocator, pointer_ );
		return;
	}
	HValueReference( HValueReference const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	HValueReference( HValueReferenceObserver<tType> const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	template<typename alien_t>
	HValueReference( HValueReference<alien_t> const& pointer_ )
		: HValueReferenceBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	HValueReference( HValueReference&& other_ ) noexcept
		: HValueReferenceBase<tType>() {
		swap( other_ );
		return;
	}
	~HValueReference( void ) {
		/* The make_value() helper part.
		 */
		if ( this->_shared ) {
			reset();
		}
	}
	HValueReference& operator = ( HValueReference const& pointer_ ) {
		this->template acquire<trait::true_type>( pointer_ );
		return ( *this );
	}
	HValueReference& operator = ( HValueReference&& other_ ) noexcept {
		swap( other_ );
		other_.reset();
		return ( *this );
	}
	template<typename alien_t>
	HValueReference& operator = ( HValueReference<alien_t> const& pointer_ ) {
		this->template acquire<trait::true_type>( pointer_ );
		return ( *this );
	}
	void swap( HValueReference& p ) {
		/*
		 * Both fields are POD types (pointers: tType*, HSharedBase*)
		 * so they do not have a specialized implementation
		 * and we can explicitly request yaal generic implementation.
		 */
		yaal::swap( this->_shared, p._shared );
		return;
	}
	bool unique( void ) const {
		return ( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] == 1 ) );
	}
	int use_count( void ) const {
		return ( this->_shared ? this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] : 0 );
	}
	void reset( void ) {
		this->template do_reset<trait::true_type>();
	}
	const_reference operator* ( void ) const {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_shared->_object) );
	}
	reference operator* ( void ) {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_shared->_object) );
	}

	template<typename alien_t>
	bool operator == ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared == alien->_shared );
	}
	template<typename alien_t>
	bool operator == ( alien_t const* pointer_ ) const {
		return ( get() == pointer_ );
	}
	template<typename alien_t>
	bool operator != ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared != alien->_shared );
	}
	template<typename alien_t>
	bool operator != ( alien_t const* pointer_ ) const {
		return ( get() != pointer_ );
	}

	template<typename alien_t>
	bool operator < ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared < alien->_shared );
	}
	template<typename alien_t>
	bool operator < ( alien_t const* pointer_ ) const {
		return ( get() < pointer_ );
	}
	template<typename alien_t>
	bool operator > ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared > alien->_shared );
	}
	template<typename alien_t>
	bool operator > ( alien_t const* pointer_ ) const {
		return ( get() > pointer_ );
	}

	template<typename alien_t>
	bool operator <= ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared <= alien->_shared );
	}
	template<typename alien_t>
	bool operator <= ( alien_t const* pointer_ ) const {
		return ( get() <= pointer_ );
	}
	template<typename alien_t>
	bool operator >= ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared >= alien->_shared );
	}
	template<typename alien_t>
	bool operator >= ( alien_t const* pointer_ ) const {
		return ( get() >= pointer_ );
	}

	tType const* operator->( void ) const {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_shared->_object );
	}
	value_type* operator->( void ) {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_shared->_object );
	}
	tType const* raw( void ) const {
		return ( this->_shared ? this->_shared->_object : nullptr );
	}
	value_type* raw( void ) {
		return ( this->_shared ? this->_shared->_object : nullptr );
	}
	value_type const* get( void ) const {
		return ( this->_shared ? this->_shared->_object : nullptr );
	}
	value_type* get( void ) {
		return ( this->_shared ? this->_shared->_object : nullptr );
	}
	bool operator ! ( void ) const {
		return ( ! ( this->_shared && this->_shared->_object ) );
	}
private:
	friend struct value_reference_helper;
};

template<typename alien_t, typename tType>
bool operator == ( alien_t const* pointer_, HValueReference<tType> const& smartValueReference_ ) {
	return ( smartValueReference_ == pointer_ );
}

template<typename alien_t, typename tType>
bool operator != ( alien_t const* pointer_, HValueReference<tType> const& smartValueReference_ ) {
	return ( smartValueReference_ != pointer_ );
}

template<typename to_t, typename from_t>
typename yaal::tools::huginn::HValueReference<to_t> value_reference_static_cast( HValueReference<from_t> const& from_ ) {
	return ( value_reference_helper::do_static_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t>
typename yaal::tools::huginn::HValueReference<to_t> value_reference_dynamic_cast( HValueReference<from_t> const& from_ ) {
	return ( value_reference_helper::do_dynamic_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t>
typename yaal::tools::huginn::HValueReference<to_t> value_reference_const_cast( HValueReference<from_t> const& from_ ) {
	return ( value_reference_helper::do_const_cast<to_t>( from_ ) );
}

template<typename allocator_t, typename tType, typename... arg_t>
HValueReference<tType> allocate_value( allocator_t const& allocator_, arg_t&&... arg_ ) {
	return ( HValueReference<tType>( allocator_, yaal::forward<arg_t>( arg_ )... ) );
}

template<typename tType>
inline void swap( yaal::tools::huginn::HValueReference<tType>& a, yaal::tools::huginn::HValueReference<tType>& b ) {
	a.swap( b );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_VALUEREFERENCE_HXX_INCLUDED */


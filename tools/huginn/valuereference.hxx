/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	static int const OBJECT_OFFSET = 2;
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
	HSharedBase( void )
		: _referenceCounter{ 0, 0 } {
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
	void destroy( HValueReference<tType>* object_, trait::true_type* ) {
		destroy( object_ );
	}
	void destroy( HValueReference<tType>*, trait::false_type* ) {}
	virtual void destroy( HValueReference<tType>* ) = 0;
	virtual void deallocate( void ) = 0;
	tType* object( void ) {
		return ( reinterpret_cast<tType*>( &_referenceCounter[REFERENCE_COUNTER_TYPE::OBJECT_OFFSET] ) );
	}
	friend struct value_reference_helper;
	template<typename>
	friend class HValueReferenceBase;
	template<typename>
	friend class HValueReference;
	friend class HObjectFactory;
private:
	HSharedBase( HSharedBase const& );
	HSharedBase& operator = ( HSharedBase const& );
};

template<typename tType, typename value_t, typename allocator_t>
class HSharedValueHolder : protected HSharedBase<tType> {
public:
	typedef typename memory::aligned<static_cast<int>( sizeof ( value_t ) ), value_t>::type aligner_t;
private:
	aligner_t _mem;
protected:
	allocator_t _allocator;
	template<typename input_allocator>
	HSharedValueHolder( input_allocator const& allocator_ )
		: HSharedBase<tType>()
		, _mem()
		, _allocator( allocator_ ) {
	}
	virtual void destroy( HValueReference<tType>* object_ ) override {
		M_ASSERT( this->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] == 1 );
		this->HSharedBase<tType>::object()->destroy( object_ );
		if ( this->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] == 1 ) {
			this->HSharedBase<tType>::object()->~tType();
		}
	}
	virtual void deallocate( void ) override {
		typedef typename allocator_t:: template rebind<HSharedValueHolder>::other allocator_type;
		allocator_type( this->_allocator ).deallocate( this, 1 );
	}
	template<typename>
	friend class HValueReference;
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
			_shared->deallocate();
			_shared = nullptr;
		}
		return;
	}
	template<typename type, typename alien_t>
	void acquire( HValueReferenceBase<alien_t> const& from ) {
		HValueReferenceBase const& alien = reinterpret_cast<HValueReferenceBase const&>( from );
		if ( ( &alien != this ) && ( _shared != alien._shared ) ) {
			release<type>();
			if ( alien._shared && ( alien._shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) ) {
				alien._shared->inc_reference_counter( static_cast<type*>( nullptr ) );
				_shared = alien._shared;
			}
		}
		return;
	}
	template<typename type>
	void release( void ) throw() {
		if ( _shared ) {
			if ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] == 1 ) {
				_shared->destroy( static_cast<HValueReference<tType>*>( this ), static_cast<type*>( nullptr ) );
			}
			_shared->dec_reference_counter( static_cast<type*>( nullptr ) );
			if ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::OBSERVER ] == 0 ) {
				_shared->deallocate();
			}
			_shared = nullptr;
		}
		return;
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
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
template<typename tType>
class HValueReferenceObserver final : public HValueReferenceBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic pop
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
		this->template release<trait::false_type>();
	}
	void swap( HValueReferenceObserver& p ) {
		/*
		 * HSharedBase* is a POD type so they do not have a specialized implementation
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
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
template<typename tType>
class HValueReference final : public HValueReferenceBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic pop
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
public:
	typedef typename HValueReferenceBase<tType>::value_type value_type;
	typedef typename HValueReferenceBase<tType>::reference reference;
	typedef typename HValueReferenceBase<tType const>::const_reference const_reference;
	typedef HSharedBase<tType> shared_t;
	template<typename allocator_t, typename value_t>
	struct allocated_shared {
		typedef HSharedValueHolder<tType, value_t, allocator_t> type;
		static int const size = static_cast<int>( sizeof ( type ) );
	};
	HValueReference( void )
		: HValueReferenceBase<tType>() {
		return;
	}
	HValueReference( std::nullptr_t )
		: HValueReferenceBase<tType>() {
	}
	template<typename value_t, typename allocator_t, typename... args_t>
	explicit HValueReference( value_t*, allocator_t const& allocator_, args_t&&... args_  )
		: HValueReferenceBase<tType>() {
		typedef HSharedValueHolder<tType, value_t, allocator_t> shared_value_holder_t;
		typedef typename allocator_t::template rebind<shared_value_holder_t>::other allocator_type;
		allocator_type allocator( allocator_ );
		shared_value_holder_t* shared( allocator.allocate( 1 ) );
		this->_shared = shared;
		new ( shared ) shared_value_holder_t( allocator );
		value_t* ptr( static_cast<value_t*>( shared->object() ) );
		new ( ptr ) value_t( yaal::forward<args_t>( args_ )... );
		shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		return;
	}
	/* Used only in garbage collector. */
	HValueReference( typename HValueReferenceBase<tType>::shared_t* shared_ )
		: HValueReferenceBase<tType>( shared_ ) {
		this->_shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
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
	void swap( HValueReference& p ) {
		/*
		 * HSharedBase* is a POD type so they do not have a specialized implementation
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
		this->template release<trait::true_type>();
	}
	const_reference operator* ( void ) const {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_shared->object()) );
	}
	reference operator* ( void ) {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_shared->object()) );
	}

	template<typename alien_t>
	bool operator == ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared == alien->_shared );
	}
	template<typename alien_t>
	bool operator == ( alien_t const* pointer_ ) const {
		return ( raw() == pointer_ );
	}
	template<typename alien_t>
	bool operator != ( HValueReference<alien_t> const& pointer_ ) const {
		HValueReference const* alien = reinterpret_cast<HValueReference const *>( &pointer_ );
		return ( this->_shared != alien->_shared );
	}
	template<typename alien_t>
	bool operator != ( alien_t const* pointer_ ) const {
		return ( raw() != pointer_ );
	}

	value_type const* operator->( void ) const {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_shared->object() );
	}
	value_type* operator->( void ) {
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_shared->object() );
	}
	value_type const* raw( void ) const {
		return ( this->_shared->object() );
	}
	value_type* raw( void ) {
		return ( this->_shared->object() );
	}
	explicit operator bool ( void ) const {
		return ( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
	}
private:
	friend struct value_reference_helper;
};

template<typename tType>
bool operator == ( tType const* pointer_, HValueReference<tType> const& smartValueReference_ ) {
	return ( smartValueReference_ == pointer_ );
}

template<typename tType>
bool operator != ( tType const* pointer_, HValueReference<tType> const& smartValueReference_ ) {
	return ( smartValueReference_ != pointer_ );
}

template<typename allocator_t, typename base_t, typename tType, typename... arg_t>
HValueReference<base_t> allocate_value( allocator_t const& allocator_, arg_t&&... arg_ ) {
	return ( HValueReference<base_t>( static_cast<tType*>( nullptr ), allocator_, yaal::forward<arg_t>( arg_ )... ) );
}

template<typename tType>
inline void swap( yaal::tools::huginn::HValueReference<tType>& a, yaal::tools::huginn::HValueReference<tType>& b ) {
	a.swap( b );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_VALUEREFERENCE_HXX_INCLUDED */


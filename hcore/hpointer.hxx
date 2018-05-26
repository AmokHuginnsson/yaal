/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hpointer.hxx
 * \brief Declaration and implementation of HPointer<> class template.
 */

#ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED
#define YAAL_HCORE_HPOINTER_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/trait.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

/* Due to extreme vitality of this structure,
 * none of the methods are guarded. */

/*! \brief Types of reference counters.
 */
struct REFERENCE_COUNTER_TYPE {
	static int const HOLDER = 0;
	static int const OBSERVER = 1;
};

template<typename tType>
class HPointerFromThisInterface;

template<typename tType>
class HPointer;

template<typename T, typename U>
struct has_initialize_observer {
private:
	template<typename X, typename Y>
	static auto test( int )->decltype ( static_cast<X*>( nullptr )->initialize_observer( Y() ), static_cast<trait::true_type*>( nullptr ) );
	template<typename, typename>
	static trait::false_type* test( ... );
public:
	static bool const value = trait::same_type<decltype ( test<T, U>( 0 ) ), trait::true_type*>::value;
};

template<typename tType>
class HSharedBase {
public:
	typedef typename trait::decay<typename trait::strip_const<tType>::type>::type decayed_type;
	typedef typename trait::ternary<trait::is_array<tType>::value, typename trait::strip_pointer<decayed_type>::type, decayed_type>::type value_type;
	typedef typename trait::make_reference<value_type>::type reference;
	typedef typename trait::make_reference<value_type const>::type const_reference;
	typedef value_type* pointer;
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
	friend struct pointer_helper;
	template<typename>
	friend class HPointerBase;
	template<typename>
	friend class HPointer;
private:
	HSharedBase( HSharedBase const& );
	HSharedBase& operator = ( HSharedBase const& );
};

template<typename tType, typename real_t>
class HShared;

template<typename tType, typename deleter_t>
class HSharedDeleter;

template<typename tType, typename deleter_t, typename allocator_t>
class HSharedDeleterAllocator;

template<typename tType, typename deleter_t, typename allocator_t>
class HSharedDeleterAllocatorImpl;

struct pointer_helper {
	template<typename to_t, typename from_t>
	static typename yaal::hcore::HPointer<to_t> do_static_cast( HPointer<from_t> const& from_ ) {
		HPointer<to_t> to;
		if ( from_._object ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			to._shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		}
		return ( to );
	}

	template<typename to_t, typename from_t>
	static typename yaal::hcore::HPointer<to_t> do_dynamic_cast( HPointer<from_t> const& from_ ) {
		HPointer<to_t> to;
		if ( dynamic_cast<to_t*>( from_._object ) ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			to._shared->inc_reference_counter( static_cast<trait::true_type*>( nullptr ) );
		}
		return ( to );
	}

	template<typename to_t, typename from_t>
	static typename yaal::hcore::HPointer<to_t> do_const_cast( HPointer<from_t> const& from_ ) {
		HPointer<to_t> to;
		if ( dynamic_cast<to_t*>( from_._object ) ) {
			to._shared = reinterpret_cast<HSharedBase<to_t>*>( from_._shared );
			to._object = const_cast<to_t*>( from_._object );
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

	template<typename tType>
	static tType* do_make_pointer_pre( HPointer<tType>& ptr_ ) {
		ptr_._shared = new ( memory::yaal ) HSharedDeleter<tType, HSpaceHolderDeleter<tType>>( HSpaceHolderDeleter<tType>(), static_cast<tType*>( nullptr ) );
		return ( static_cast<HSharedDeleter<tType, HSpaceHolderDeleter<tType>>*>( ptr_._shared )->DELETER.mem() );
	}

	template<typename tType>
	static void do_make_pointer_post( HPointer<tType>& ptr_ ) {
		ptr_._object = static_cast<HSharedDeleter<tType, HSpaceHolderDeleter<tType>>*>( ptr_._shared )->DELETER.mem();
		ptr_._shared->_object = ptr_._object;
		HPointer<tType>::initialize_from_this( ptr_._object, ptr_, 0 );
		return;
	}

	template<typename allocator_t, typename tType>
	static tType* do_allocate_pointer_pre( allocator_t const& allocator_, HPointer<tType>& ptr_ ) {
		typedef typename allocator_t::template rebind<HSharedDeleterAllocator<tType, HSpaceHolderDeleter<tType>, allocator_t>>::other allocator_type;
		allocator_type allocator( allocator_ );
		ptr_._shared = allocator.allocate( 1 );
		new ( ptr_._shared ) HSharedDeleterAllocatorImpl<tType, HSpaceHolderDeleter<tType>, allocator_t>( HSpaceHolderDeleter<tType>(), allocator, static_cast<tType*>( nullptr ) );
		return ( static_cast<HSharedDeleterAllocatorImpl<tType, HSpaceHolderDeleter<tType>, allocator_t>*>( ptr_._shared )->DELETER.mem() );
	}

	template<typename allocator_t, typename tType>
	static void do_allocate_pointer_post( HPointer<tType>& ptr_ ) {
		ptr_._object = static_cast<HSharedDeleterAllocatorImpl<tType, HSpaceHolderDeleter<tType>, allocator_t>*>( ptr_._shared )->DELETER.mem();
		ptr_._shared->_object = ptr_._object;
		HPointer<tType>::initialize_from_this( ptr_._object, ptr_, 0 );
		return;
	}

};

template<typename tType>
class HPointerBase {
public:
	typedef HSharedBase<tType> shared_t;
	typedef typename shared_t::value_type value_type;
	typedef typename shared_t::reference reference;
	typedef typename shared_t::const_reference const_reference;
	typedef typename shared_t::pointer pointer;
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
	 * failure an object pointee must still be nullptr.
	 */
protected:
	shared_t* _shared;
	value_type* _object;
public:
	HPointerBase( void )
		: _shared( nullptr )
		, _object( nullptr ) {
		return;
	}
	HPointerBase( shared_t* shared_, value_type* object_ )
		: _shared( shared_ )
		, _object( object_ ) {
		return;
	}
	HPointerBase( HPointerBase const& ) = delete;
	HPointerBase& operator = ( HPointerBase const& ) = delete;
protected:
	~HPointerBase( void ) {
		if ( ! _object && _shared ) {
			/* This path is reached only when tType constructor throws
			 * in make_pointer() helper function.
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
		_object = nullptr;
		return;
	}
	template<typename type, typename alien_t>
	void acquire( HPointerBase<alien_t> const& from ) {
		typedef typename HPointerBase<alien_t>::pointer alien_pointer;
		HPointerBase const& alien = reinterpret_cast<HPointerBase const&>( from );
		if ( ( &alien != this ) && ( _shared != alien._shared ) ) {
			M_ASSERT( ( ! ( _shared && alien._shared ) )
				|| ( ( _shared && alien._shared )
					&& ( _object != static_cast<alien_pointer>( static_cast<void*>( alien._object ) ) ) ) );
			if ( _shared ) {
				release<type>();
			}
			if ( alien._shared && ( alien._shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) ) {
				alien._shared->inc_reference_counter( static_cast<type*>( nullptr ) );
				_shared = alien._shared;
				assign( _object, static_cast<alien_pointer>( static_cast<void*>( alien._object ) ) );
			} else {
				_shared = nullptr;
				_object = nullptr;
			}
		}
		return;
	}
	template<typename type>
	bool release( void ) throw() {
		M_ASSERT( _shared && _object );
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
	void assign( pointer& to, pointer from ) {
		to = from;
		return;
	}
	template<typename alien_t>
	void assign( pointer& to, alien_t* from ) {
		to = dynamic_cast<pointer>( from );
		return;
	}
	friend struct pointer_helper;
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
class HPointerObserver final : public HPointerBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic pop
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
public:
	typedef HSharedBase<tType> shared_t;
	HPointerObserver( void )
		: HPointerBase<tType>() {
		return;
	}
	HPointerObserver( HPointerObserver const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	template<typename alien_t>
	HPointerObserver( HPointerObserver<alien_t> const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	HPointerObserver( HPointerObserver&& other_ ) noexcept
		: HPointerBase<tType>() {
		swap( other_ );
		return;
	}
	HPointerObserver( HPointer<tType> const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::false_type>( pointer_ );
		return;
	}
	~HPointerObserver( void ) {
		/* The make_pointer() helper part.
		 */
		if ( this->_object ) {
			reset();
		}
	}
	HPointerObserver& operator = ( HPointerObserver const& pointer_ ) {
		this->template acquire<trait::false_type>( pointer_ );
		return ( *this );
	}
	HPointerObserver& operator = ( HPointerObserver&& other_ ) noexcept {
		if ( &other_ != this ) {
			swap( other_ );
			other_.reset();
		}
		return ( *this );
	}
	template<typename alien_t>
	HPointerObserver& operator = ( HPointerObserver<alien_t> const& pointer_ ) {
		this->template acquire<trait::false_type>( pointer_ );
		return ( *this );
	}
	void reset( void ) {
		this->template do_reset<trait::false_type>();
	}
	void swap( HPointerObserver& p ) {
		/*
		 * Both fields are POD types (pointers: tType*, HSharedBase*)
		 * so they do not have a specialized implementation
		 * and we can explicitly request yaal generic implementation.
		 */
		yaal::swap( this->_shared, p._shared );
		yaal::swap( this->_object, p._object );
		return;
	}
};

/*! \brief Smart pointer, reference counting implementation.
 *
 * \tparam tType - object type which life time will be guarded.
 * \tparam pointer_type_t - select pointer type, one of HPointerScalar or HPointerArray.
 * \tparam access_type_t - defines pointer kind, one of HPointerStrict or HPointerWeak.
 */
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
template<typename tType>
class HPointer final : public HPointerBase<tType> {
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic pop
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */
public:
	typedef typename HPointerBase<tType>::value_type value_type;
	typedef typename HPointerBase<tType>::reference reference;
	typedef typename HPointerBase<tType const>::const_reference const_reference;
	typedef pointer_helper::HSpaceHolderDeleter<tType> space_holder_deleter_t;
	template<typename allocator_t>
	struct allocated_shared {
		typedef HSharedDeleterAllocator<tType, space_holder_deleter_t, allocator_t> type;
		static int const size = static_cast<int>( sizeof ( type ) );
	};
	HPointer( void )
		: HPointerBase<tType>() {
		return;
	}
	HPointer( std::nullptr_t )
		: HPointerBase<tType>() {
	}
	template<typename real_t>
	explicit HPointer( real_t* pointer_ )
		: HPointerBase<tType>(
			pointer_ ? new ( memory::yaal ) HShared<tType, real_t>( pointer_ ) : nullptr,
			pointer_
		) {
		M_ASSERT( pointer_ );
		initialize_from_this( pointer_, *this, 0 );
		return;
	}
	template<typename real_t, typename deleter_t>
	explicit HPointer( real_t* pointer_,  deleter_t deleter_ )
		: HPointerBase<tType>( pointer_ ? new ( memory::yaal ) HSharedDeleter<tType, deleter_t>( deleter_, pointer_ ) : nullptr, pointer_ ) {
		M_ASSERT( pointer_ );
		initialize_from_this( pointer_, *this, 0 );
		return;
	}
	template<typename real_t, typename deleter_t, typename allocator_t>
	explicit HPointer( real_t* pointer_,  deleter_t deleter_, allocator_t allocator_ )
		: HPointerBase<tType>() {
		M_ASSERT( pointer_ );
		typedef typename allocator_t::template rebind<HSharedDeleterAllocator<tType, deleter_t, allocator_t>>::other allocator_type;
		allocator_type allocator( allocator_ );
		this->_shared = allocator.allocate( 1 );
		new ( this->_shared ) HSharedDeleterAllocatorImpl<tType, deleter_t, allocator_type>( deleter_, allocator, pointer_ );
		this->_object = pointer_;
		initialize_from_this( pointer_, *this, 0 );
		return;
	}
	HPointer( HPointer const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	HPointer( HPointerObserver<tType> const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	template<typename alien_t>
	HPointer( HPointer<alien_t> const& pointer_ )
		: HPointerBase<tType>() {
		this->template acquire<trait::true_type>( pointer_ );
		return;
	}
	HPointer( HPointer&& other_ ) noexcept
		: HPointerBase<tType>() {
		swap( other_ );
		return;
	}
	~HPointer( void ) {
		/* The make_pointer() helper part.
		 */
		if ( this->_object ) {
			reset();
		}
	}
	HPointer& operator = ( HPointer const& pointer_ ) {
		this->template acquire<trait::true_type>( pointer_ );
		return ( *this );
	}
	HPointer& operator = ( HPointer&& other_ ) noexcept {
		swap( other_ );
		other_.reset();
		return ( *this );
	}
	template<typename alien_t>
	HPointer& operator = ( HPointer<alien_t> const& pointer_ ) {
		this->template acquire<trait::true_type>( pointer_ );
		return ( *this );
	}
	void swap( HPointer& p ) {
		/*
		 * Both fields are POD types (pointers: tType*, HSharedBase*)
		 * so they do not have a specialized implementation
		 * and we can explicitly request yaal generic implementation.
		 */
		yaal::swap( this->_shared, p._shared );
		yaal::swap( this->_object, p._object );
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
		static_assert( !trait::is_array<tType>::value, "indirection operator is called for an array" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_object) );
	}
	reference operator* ( void ) {
		static_assert( !trait::is_array<tType>::value, "indirection operator is called for an array" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( *(this->_object) );
	}
	const_reference operator[] ( int index_ ) const {
		static_assert( trait::is_array<tType>::value, "array subscript operator is called for a scalar" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		M_ASSERT( index_ >= 0 );
		return ( this->_object[ index_ ] );
	}
	reference operator[] ( int index_ ) {
		static_assert( trait::is_array<tType>::value, "array subscript operator is called for a scalar" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		M_ASSERT( index_ >= 0 );
		return ( this->_object[ index_ ] );
	}

	template<typename alien_t>
	bool operator == ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object == static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator == ( alien_t const* pointer_ ) const {
		return ( this->_object == pointer_ );
	}
	template<typename alien_t>
	bool operator != ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object != static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator != ( alien_t const* pointer_ ) const {
		return ( this->_object != pointer_ );
	}

	template<typename alien_t>
	bool operator < ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object < static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator < ( alien_t const* pointer_ ) const {
		return ( this->_object < pointer_ );
	}
	template<typename alien_t>
	bool operator > ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object > static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator > ( alien_t const* pointer_ ) const {
		return ( this->_object > pointer_ );
	}

	template<typename alien_t>
	bool operator <= ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object <= static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator <= ( alien_t const* pointer_ ) const {
		return ( this->_object <= pointer_ );
	}
	template<typename alien_t>
	bool operator >= ( HPointer<alien_t> const& pointer_ ) const {
		HPointer const* alien = reinterpret_cast<HPointer const*>( &pointer_ );
		return ( this->_object >= static_cast<alien_t const*>( static_cast<void const*>( alien->_object ) ) );
	}
	template<typename alien_t>
	bool operator >= ( alien_t const* pointer_ ) const {
		return ( this->_object >= pointer_ );
	}

	tType const* operator->( void ) const {
		static_assert( !trait::is_array<tType>::value, "structure dereference operator is called for an array" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_object );
	}
	value_type* operator->( void ) {
		static_assert( !trait::is_array<tType>::value, "structure dereference operator is called for an array" );
		M_ASSERT( this->_shared && ( this->_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::HOLDER ] > 0 ) );
		return ( this->_object );
	}
	tType const* raw( void ) const {
		return ( this->_object );
	}
	value_type* raw( void ) {
		return ( this->_object );
	}
	value_type const* get( void ) const {
		return ( this->_object );
	}
	value_type* get( void ) {
		return ( this->_object );
	}
	bool operator! ( void ) const {
		return ( ! this->_object );
	}
private:
	friend struct pointer_helper;
	template<typename ptr_t, typename X = typename trait::enable_if<has_initialize_observer<tType, ptr_t>::value>::type>
	static void initialize_from_this( value_type* obj, ptr_t const& ptr, int ) {
		obj->initialize_observer( ptr );
		return;
	}
	template<typename ptr_t>
	static void initialize_from_this( value_type*, ptr_t const&, long ) {
		return;
	}
};

template<typename tType, typename real_t>
class HShared : protected HSharedBase<tType> {
	HShared( real_t* object_ )
		: HSharedBase<tType>( object_ ) {
	}
	virtual ~HShared( void ) {}
	virtual void do_delete( void ) override {
		delete_pointee( static_cast<typename trait::ternary<trait::is_array<tType>::value, trait::true_type, trait::false_type>::type*>( nullptr ) );
		this->_object = nullptr;
	}
	void delete_pointee( trait::false_type* ) {
		M_SAFE( delete static_cast<real_t*>( this->_object ) );
	}
	void delete_pointee( trait::true_type* ) {
		M_SAFE( delete [] static_cast<real_t*>( this->_object ) );
	}
	friend struct pointer_helper;
	template<typename>
	friend class HPointer;
};

template<typename tType, typename deleter_t>
class HSharedDeleter : protected HSharedBase<tType> {
	deleter_t DELETER;
	HSharedDeleter( deleter_t const& deleter_, tType* object_ )
		: HSharedBase<tType>( object_ )
		, DELETER( deleter_ ) {
	}
	virtual void do_delete( void ) override {
		DELETER( HSharedBase<tType>::_object );
		HSharedBase<tType>::_object = nullptr;
	}
	friend struct pointer_helper;
	template<typename>
	friend class HPointer;
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
	friend struct pointer_helper;
	template<typename>
	friend class HPointer;
};

template<typename tType, typename deleter_t, typename allocator_t>
class HSharedDeleterAllocatorImpl : protected HSharedDeleterAllocator<tType, deleter_t, allocator_t> {
	typedef HSharedDeleterAllocator<tType, deleter_t, allocator_t> base_type;
	template<typename input_allocator>
	HSharedDeleterAllocatorImpl( deleter_t const& deleter_, input_allocator const& allocator_, tType* object_ )
		: base_type( deleter_, allocator_, object_ ) {
	}
	virtual void destroy( void ) override {
		typedef typename allocator_t::template rebind<HSharedDeleterAllocatorImpl>::other allocator_type;
		allocator_type( this->_allocator ).deallocate( this, 1 );
	}
	friend struct pointer_helper;
	template<typename>
	friend class HPointer;
};

/*! \brief Interface for concept of getting smart pointer from `this'.
 */
template<typename tType>
class HPointerFromThisInterface {
protected:
	typedef HPointer<tType> ptr_t;
	typedef HPointerObserver<tType> observer_t;
	observer_t _selfObserver;
public:
	HPointerFromThisInterface( void )
		: _selfObserver() {
		return;
	}
	virtual ~HPointerFromThisInterface( void ) {}
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
	template<typename T>
	friend class HPointer;
	template<typename T, typename U>
	friend struct has_initialize_observer;
};

template<typename alien_t, typename tType>
bool operator == ( alien_t const* pointer_, HPointer<tType> const& smartPointer_ ) {
	return ( smartPointer_ == pointer_ );
}

template<typename alien_t, typename tType>
bool operator != ( alien_t const* pointer_, HPointer<tType> const& smartPointer_ ) {
	return ( smartPointer_ != pointer_ );
}

template<typename to_t, typename from_t>
typename yaal::hcore::HPointer<to_t> pointer_static_cast( HPointer<from_t> const& from_ ) {
	return ( pointer_helper::do_static_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t>
typename yaal::hcore::HPointer<to_t> pointer_dynamic_cast( HPointer<from_t> const& from_ ) {
	return ( pointer_helper::do_dynamic_cast<to_t>( from_ ) );
}

template<typename to_t, typename from_t>
typename yaal::hcore::HPointer<to_t> pointer_const_cast( HPointer<from_t> const& from_ ) {
	return ( pointer_helper::do_const_cast<to_t>( from_ ) );
}

template<typename tType, typename... arg_t>
HPointer<tType> make_pointer( arg_t&&... arg_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_make_pointer_pre( ptr ) );
	new ( p ) tType( yaal::forward<arg_t>( arg_ )... );
	pointer_helper::do_make_pointer_post( ptr );
	return ( ptr );
}

template<typename allocator_t, typename tType, typename... arg_t>
HPointer<tType> allocate_pointer( allocator_t const& allocator_, arg_t&&... arg_ ) {
	HPointer<tType> ptr;
	tType* p( pointer_helper::do_allocate_pointer_pre( allocator_, ptr ) );
	new ( p ) tType( yaal::forward<arg_t>( arg_ )... );
	pointer_helper::do_allocate_pointer_post<allocator_t>( ptr );
	return ( ptr );
}

template<typename tType>
inline void swap( yaal::hcore::HPointer<tType>& a, yaal::hcore::HPointer<tType>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/objectfactory.hxx
 * \brief Declaration of HObjectFactory class.
 */

#ifndef YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

enum class POOL_TYPE {
	SCALAR,
	COLLECTION,
	CLASSLESS
};

class HPoolHolderInterface {
public:
	virtual ~HPoolHolderInterface( void ) {}
};

template<int const size>
class HPoolHolder : public HPoolHolderInterface {
public:
	typedef yaal::hcore::HPool<size> pool_t;
private:
	pool_t _pool;
public:
	HPoolHolder( void )
		: _pool() {
	}
	pool_t& get( void ) {
		return ( _pool );
	}
};

template<typename T>
struct pool_type_info {
	typedef yaal::tools::huginn::HValueReference<T> object_ptr_t;
	typedef typename object_ptr_t::template allocated_shared<allocator::shared_pool<T>> shared_t;
	typedef HPoolHolder<shared_t::size> pool_holder_t;
	typedef typename pool_holder_t::pool_t pool_t;
	typedef allocator::shared_pool<typename shared_t::type> allocator_t;
};

class HObjectFactoryBase {
protected:
	typedef yaal::hcore::HResource<HPoolHolderInterface> pool_holder_ptr_t;
	typedef yaal::hcore::HArray<pool_holder_ptr_t> pool_holders_t;
	typedef yaal::hcore::HHashMap<int, HPoolHolderInterface*> pools_t;
	HRuntime* _runtime;
	pool_holders_t _memoryPoolsStore;
	pools_t _memoryPools;
public:
	HObjectFactoryBase( HRuntime* );
	virtual ~HObjectFactoryBase( void ) {}
	template<typename T>
	typename pool_type_info<T>::allocator_t get_allocator( void ) {
		typedef typename pool_type_info<T>::pool_holder_t pool_holder_t;
		typedef typename pool_type_info<T>::shared_t shared_t;
		pools_t::iterator it( _memoryPools.find( shared_t::size + 0 ) );
		HPoolHolderInterface* phRaw( nullptr );
		if ( it != _memoryPools.end() ) {
			phRaw = it->second;
		} else {
			pool_holder_ptr_t ph( yaal::hcore::make_resource<pool_holder_t>() );
			phRaw = ph.raw();
			_memoryPoolsStore.emplace_back( yaal::move( ph ) );
			_memoryPools.insert( yaal::hcore::make_pair( shared_t::size + 0, phRaw ) );
		}
		return ( static_cast<pool_holder_t*>( phRaw )->get() );
	}
private:
	template<typename T>
	friend class HObjectPoolBase;
private:
	HObjectFactoryBase( HObjectFactoryBase const& ) = delete;
	HObjectFactoryBase& operator = ( HObjectFactoryBase const& ) = delete;
};


template<typename T>
class HObjectPoolBase {
protected:
	typedef typename pool_type_info<T>::allocator_t allocator_t;
	allocator_t _allocator;
public:
	HObjectPoolBase( HObjectFactoryBase* objectFactory_ )
		: _allocator( objectFactory_->get_allocator<T>() ) {
	}
};

template<typename T, POOL_TYPE const = POOL_TYPE::SCALAR>
class HObjectPool;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
template<typename T>
class HObjectPool<T, POOL_TYPE::SCALAR> : public HObjectPoolBase<T> {
#pragma GCC diagnostic pop
	HHuginn::HClass const* _class;
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	HObjectPool( HObjectFactoryBase* objectFactory_, HHuginn::HClass const* class_ )
		: base_type( objectFactory_ )
		, _class( class_ ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( huginn::allocate_value<allocator_t, T>( this->_allocator, _class, yaal::forward<arg_t>( arg_ )... ) );
	}
private:
	HObjectPool( HObjectPool const& ) = delete;
	HObjectPool& operator = ( HObjectPool const& ) = delete;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
template<typename T>
class HObjectPool<T, POOL_TYPE::COLLECTION> : public HObjectPoolBase<T> {
#pragma GCC diagnostic pop
	HHuginn::HClass const* _class;
	typename T::pool_t _nodePool;
	typename T::allocator_t _nodeAllocator;
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	HObjectPool( HObjectFactoryBase* objectFactory_, HHuginn::HClass const* class_ )
		: base_type( objectFactory_ )
		, _class( class_ )
		, _nodePool()
		, _nodeAllocator( _nodePool ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( huginn::allocate_value<allocator_t, T>( this->_allocator, _class, _nodeAllocator, yaal::forward<arg_t>( arg_ )... ) );
	}
private:
	HObjectPool( HObjectPool const& ) = delete;
	HObjectPool& operator = ( HObjectPool const& ) = delete;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
template<typename T>
class HObjectPool<T, POOL_TYPE::CLASSLESS> : public HObjectPoolBase<T> {
#pragma GCC diagnostic pop
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	HObjectPool( HObjectFactoryBase* objectFactory_ )
		: base_type( objectFactory_ ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( huginn::allocate_value<allocator_t, T>( this->_allocator, yaal::forward<arg_t>( arg_ )... ) );
	}
};

class HObjectFactory final : public HObjectFactoryBase {
	HObjectPool<HHuginn::HTaggedValue, POOL_TYPE::CLASSLESS> _taggedValuePool;
	HObjectPool<HHuginn::HObject, POOL_TYPE::CLASSLESS> _objectPool;
	/* Language semantics classes. */
	HHuginn::class_t _none;
	HHuginn::class_t _observer;
	HHuginn::class_t _reference;
	HHuginn::class_t _functionReference;
	HHuginn::class_t _objectReference;
	HHuginn::class_t _method;
	HHuginn::class_t _unboundMethod;
	HHuginn::class_t _boundMethod;
	HHuginn::class_t _variadicParameters;
	HHuginn::class_t _namedParameters;
	/* Language semantics pools. */
	HObjectPool<HHuginn::HReference> _referencePool;
	HObjectPool<HHuginn::HFunctionReference> _functionReferencePool;
	HObjectPool<HHuginn::HClass::HMethod> _methodPool;
	HObjectPool<HHuginn::HClass::HUnboundMethod> _unboundMethodPool;
	HObjectPool<HHuginn::HClass::HBoundMethod> _boundMethodPool;
	/* Built-in classes. */
	HHuginn::class_t _boolean;
	HHuginn::class_t _integer;
	HHuginn::class_t _string;
	HHuginn::class_t _real;
	HHuginn::class_t _number;
	HHuginn::class_t _character;
	HHuginn::class_t _tuple;
	HHuginn::class_t _list;
	HHuginn::class_t _deque;
	HHuginn::class_t _dict;
	HHuginn::class_t _order;
	HHuginn::class_t _lookup;
	HHuginn::class_t _set;
	HHuginn::class_t _blob;
	HHuginn::class_t _exception;
	HHuginn::class_t _stackFrameInfo;
	HHuginn::class_t _runtimeException;
	HHuginn::class_t _conversionException;
	HHuginn::class_t _arithmeticException;
	HHuginn::value_t _true;
	HHuginn::value_t _false;
	/* Explicit pools. */
	HObjectPool<HHuginn::HString> _stringPool;
	HObjectPool<HHuginn::HInteger> _integerPool;
	HObjectPool<HHuginn::HReal> _realPool;
	HObjectPool<HHuginn::HNumber> _numberPool;
	HObjectPool<HHuginn::HCharacter> _characterPool;
	HObjectPool<HHuginn::HTuple> _tuplePool;
	HObjectPool<HHuginn::HList> _listPool;
	HObjectPool<HHuginn::HDeque> _dequePool;
	HObjectPool<HHuginn::HDict, POOL_TYPE::COLLECTION> _dictPool;
	HObjectPool<HHuginn::HLookup, POOL_TYPE::COLLECTION> _lookupPool;
	HObjectPool<HHuginn::HOrder, POOL_TYPE::COLLECTION> _orderPool;
	HObjectPool<HHuginn::HSet, POOL_TYPE::COLLECTION> _setPool;
	HObjectPool<HHuginn::HBlob> _blobPool;
public:
	HObjectFactory( HRuntime* );
	void register_builtin_classes( void );
	HRuntime& runtime( void ) const {
		return ( *_runtime );
	}
	HHuginn::value_t const& true_value( void ) const {
		return ( _true );
	}
	HHuginn::value_t const& false_value( void ) const {
		return ( _false );
	}
	HHuginn::value_t create_integer( HHuginn::HInteger::value_type value_ ) const {
		return ( _integerPool.create( value_ ) );
	}
	HHuginn::value_t create_string( yaal::hcore::HString const& value_ = yaal::hcore::HString() ) const {
		return ( _stringPool.create( value_ ) );
	}
	HHuginn::value_t create_string( yaal::hcore::HString&& value_ ) const {
		return ( _stringPool.create( yaal::move( value_ ) ) );
	}
	HHuginn::value_t create_real( HHuginn::HReal::value_type value_ ) const {
		return ( _realPool.create( value_ ) );
	}
	HHuginn::value_t create_number( HHuginn::HNumber::value_type const& value_ ) const {
		return ( _numberPool.create( value_ ) );
	}
	HHuginn::value_t create_number( HHuginn::HNumber::value_type&& value_ ) const {
		return ( _numberPool.create( yaal::move( value_ ) ) );
	}
	HHuginn::value_t create_character( HHuginn::HCharacter::value_type value_ ) const {
		return ( _characterPool.create( value_ ) );
	}
	HHuginn::value_t create_tuple( HHuginn::values_t&& values_ = HHuginn::values_t() ) const {
		return ( _tuplePool.create( yaal::move( values_ ) ) );
	}
	HHuginn::value_t create_list( HHuginn::values_t&& values_ = HHuginn::values_t() ) const {
		return ( _listPool.create( yaal::move( values_ ) ) );
	}
	HHuginn::value_t create_deque( HHuginn::HDeque::values_t&& values_ = HHuginn::HDeque::values_t() ) const {
		return ( _dequePool.create( yaal::move( values_ ) ) );
	}
	HHuginn::value_t create_dict( void ) const {
		return ( _dictPool.create() );
	}
	HHuginn::value_t create_order( void ) const {
		return ( _orderPool.create() );
	}
	HHuginn::value_t create_lookup( void ) const {
		return ( _lookupPool.create() );
	}
	HHuginn::value_t create_set( void ) const {
		return ( _setPool.create() );
	}
	HHuginn::value_t create_blob( yaal::hcore::HChunk&& chunk_ ) const {
		return ( _blobPool.create( yaal::move( chunk_ ) ) );
	}
	HHuginn::value_t create_reference( HHuginn::value_t& value_ ) const {
		return ( _referencePool.create( value_ ) );
	}
	HHuginn::value_t create_tagged_value( HHuginn::value_t const& value_, HHuginn::HClass const* tag_ ) const {
		return ( _taggedValuePool.create( value_, tag_ ) );
	}
	HHuginn::value_t create_function_reference( HHuginn::identifier_id_t identifierId_, HHuginn::function_t const& function_, yaal::hcore::HString const& doc_, HHuginn::HClass const* juncture_ = nullptr ) const {
		return ( _functionReferencePool.create( identifierId_, function_, doc_, juncture_ ) );
	}
	template<typename... T>
	HHuginn::value_t create_method( T&&... args_ ) {
		return (
			_methodPool.create( yaal::hcore::call( yaal::forward<T>( args_ )..., yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4 ) )
		);
	}
	HHuginn::value_t create_method_raw( HHuginn::function_t const& method_ ) const {
		return ( _methodPool.create( method_ ) );
	}
	HHuginn::value_t create_unbound_method( HHuginn::HClass const* juncture_, HHuginn::function_t const& method_ ) const {
		return ( _unboundMethodPool.create( juncture_, method_ ) );
	}
	HHuginn::value_t create_bound_method( HHuginn::function_t const& method_, HHuginn::value_t const& object_ ) const {
		return ( _boundMethodPool.create( method_, object_ ) );
	}
	HHuginn::value_t create_object( HHuginn::HClass const* class_, HHuginn::HObject::fields_t const& fields_ ) const {
		return ( _objectPool.create( class_, fields_ ) );
	}
	HHuginn::HClass const* none_class( void ) const {
		return ( _none.raw() );
	}
	HHuginn::HClass const* observer_class( void ) const {
		return ( _observer.raw() );
	}
	HHuginn::HClass const* reference_class( void ) const {
		return ( _reference.raw() );
	}
	HHuginn::HClass const* function_reference_class( void ) const {
		return ( _functionReference.raw() );
	}
	HHuginn::HClass const* object_reference_class( void ) const {
		return ( _objectReference.raw() );
	}
	HHuginn::HClass const* method_class( void ) const {
		return ( _method.raw() );
	}
	HHuginn::HClass const* unbound_method_class( void ) const {
		return ( _unboundMethod.raw() );
	}
	HHuginn::HClass const* bound_method_class( void ) const {
		return ( _boundMethod.raw() );
	}
	HHuginn::HClass const* variadic_parameters_class( void ) const {
		return ( _variadicParameters.raw() );
	}
	HHuginn::HClass const* named_parameters_class( void ) const {
		return ( _namedParameters.raw() );
	}
	HHuginn::HClass const* integer_class( void ) const {
		return ( _integer.raw() );
	}
	HHuginn::HClass const* real_class( void ) const {
		return ( _real.raw() );
	}
	HHuginn::HClass const* string_class( void ) const {
		return ( _string.raw() );
	}
	HHuginn::HClass const* number_class( void ) const {
		return ( _number.raw() );
	}
	HHuginn::HClass const* character_class( void ) const {
		return ( _character.raw() );
	}
	HHuginn::HClass const* boolean_class( void ) const {
		return ( _boolean.raw() );
	}
	HHuginn::HClass const* tuple_class( void ) const {
		return ( _tuple.raw() );
	}
	HHuginn::HClass const* list_class( void ) const {
		return ( _list.raw() );
	}
	HHuginn::HClass const* deque_class( void ) const {
		return ( _deque.raw() );
	}
	HHuginn::HClass const* dict_class( void ) const {
		return ( _dict.raw() );
	}
	HHuginn::HClass const* lookup_class( void ) const {
		return ( _lookup.raw() );
	}
	HHuginn::HClass const* order_class( void ) const {
		return ( _order.raw() );
	}
	HHuginn::HClass const* set_class( void ) const {
		return ( _set.raw() );
	}
	HHuginn::HClass const* blob_class( void ) const {
		return ( _blob.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exception.raw() );
	}
	HHuginn::HClass const* stack_frame_info_class( void ) const {
		return ( _stackFrameInfo.raw() );
	}
	HHuginn::HClass const* runtime_exception_class( void ) const {
		return ( _runtimeException.raw() );
	}
	HHuginn::HClass const* conversion_exception_class( void ) const {
		return ( _conversionException.raw() );
	}
	HHuginn::HClass const* arithmetic_exception_class( void ) const {
		return ( _arithmeticException.raw() );
	}
	template<typename T, typename... args_t>
	HHuginn::value_t create( args_t&&... args_ )  {
		typedef typename pool_type_info<T>::allocator_t allocator_t;
		return ( huginn::allocate_value<allocator_t, T>( get_allocator<T>(), yaal::forward<args_t>( args_ )... ) );
	}
private:
	HObjectFactory( HObjectFactory const& ) = delete;
	HObjectFactory& operator = ( HObjectFactory const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED */


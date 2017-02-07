/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  objectfactory.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

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

template<typename T>
class HObjectPoolBase {
protected:
	typedef yaal::hcore::HPointer<T> object_ptr_t;
	typedef typename object_ptr_t::template allocated_shared<allocator::shared_pool<T>> shared_t;
	typedef yaal::hcore::HPool<shared_t::size> pool_t;
	typedef allocator::shared_pool<typename shared_t::type> allocator_t;
	pool_t _pool;
	allocator_t _allocator;
public:
	HObjectPoolBase( void )
		: _pool()
		, _allocator( _pool ) {
	}
};

template<typename T, POOL_TYPE const = POOL_TYPE::SCALAR>
class HObjectPool;

template<typename T>
class HObjectPool<T, POOL_TYPE::SCALAR> : public HObjectPoolBase<T> {
	HHuginn::HClass const* _class;
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	HObjectPool( HHuginn::HClass const* class_ )
		: _class( class_ ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( yaal::hcore::allocate_pointer<allocator_t, T>( this->_allocator, _class, yaal::forward<arg_t>( arg_ )... ) );
	}
private:
	HObjectPool( HObjectPool const& ) = delete;
	HObjectPool& operator = ( HObjectPool const& ) = delete;
};

template<typename T>
class HObjectPool<T, POOL_TYPE::COLLECTION> : public HObjectPoolBase<T> {
	HHuginn::HClass const* _class;
	typename T::pool_t _nodePool;
	typename T::allocator_t _nodeAllocator;
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	HObjectPool( HHuginn::HClass const* class_ )
		: _class( class_ )
		, _nodePool()
		, _nodeAllocator( _nodePool ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( yaal::hcore::allocate_pointer<allocator_t, T>( this->_allocator, _class, _nodeAllocator, yaal::forward<arg_t>( arg_ )... ) );
	}
private:
	HObjectPool( HObjectPool const& ) = delete;
	HObjectPool& operator = ( HObjectPool const& ) = delete;
};

template<typename T>
class HObjectPool<T, POOL_TYPE::CLASSLESS> : public HObjectPoolBase<T> {
public:
	typedef HObjectPoolBase<T> base_type;
	typedef typename base_type::allocator_t allocator_t;
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( yaal::hcore::allocate_pointer<allocator_t, T>( this->_allocator, yaal::forward<arg_t>( arg_ )... ) );
	}
};

class HObjectFactory final {
	HRuntime* _runtime;
	HHuginn::class_t _boolean;
	HHuginn::class_t _integer;
	HHuginn::class_t _string;
	HHuginn::class_t _real;
	HHuginn::class_t _number;
	HHuginn::class_t _character;
	HHuginn::class_t _list;
	HHuginn::class_t _deque;
	HHuginn::class_t _dict;
	HHuginn::class_t _order;
	HHuginn::class_t _lookup;
	HHuginn::class_t _set;
	HHuginn::class_t _exception;
	HHuginn::class_t _conversionException;
	HHuginn::class_t _arithmeticException;
	/* Pools */
	HObjectPool<HHuginn::HString> _stringPool;
	HObjectPool<HHuginn::HInteger> _integerPool;
	HObjectPool<HHuginn::HBoolean> _booleanPool;
	HObjectPool<HHuginn::HReal> _realPool;
	HObjectPool<HHuginn::HNumber> _numberPool;
	HObjectPool<HHuginn::HCharacter> _characterPool;
	HObjectPool<HHuginn::HList> _listPool;
	HObjectPool<HHuginn::HDeque> _dequePool;
	HObjectPool<HHuginn::HDict, POOL_TYPE::COLLECTION> _dictPool;
	HObjectPool<HHuginn::HLookup, POOL_TYPE::COLLECTION> _lookupPool;
	HObjectPool<HHuginn::HOrder, POOL_TYPE::COLLECTION> _orderPool;
	HObjectPool<HHuginn::HSet, POOL_TYPE::COLLECTION> _setPool;
	HObjectPool<HHuginn::HReference, POOL_TYPE::CLASSLESS> _referencePool;
	HObjectPool<HHuginn::HFunctionReference, POOL_TYPE::CLASSLESS> _functionReferencePool;
	HObjectPool<HHuginn::HClass::HBoundMethod, POOL_TYPE::CLASSLESS> _boundMethodPool;
	HObjectPool<HHuginn::HObject, POOL_TYPE::CLASSLESS> _objectPool;
public:
	HObjectFactory( HRuntime* );
	void register_builtin_classes( void );
	HRuntime& runtime( void ) const {
		return ( *_runtime );
	}
	HHuginn::value_t create_boolean( HHuginn::HBoolean::value_type value_ ) const {
		return ( _booleanPool.create( value_ ) );
	}
	HHuginn::value_t create_integer( HHuginn::HInteger::value_type value_ ) const {
		return ( _integerPool.create( value_ ) );
	}
	HHuginn::value_t create_string( yaal::hcore::HString const& value_ = yaal::hcore::HString() ) const {
		return ( _stringPool.create( value_ ) );
	}
	HHuginn::value_t create_real( HHuginn::HReal::value_type value_ ) const {
		return ( _realPool.create( value_ ) );
	}
	HHuginn::value_t create_number( HHuginn::HNumber::value_type const& value_ ) const {
		return ( _numberPool.create( value_ ) );
	}
	HHuginn::value_t create_character( HHuginn::HCharacter::value_type value_ ) const {
		return ( _characterPool.create( value_ ) );
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
	HHuginn::value_t create_reference( HHuginn::value_t& value_ ) const {
		return ( _referencePool.create( value_ ) );
	}
	HHuginn::value_t create_function_reference( HHuginn::identifier_id_t identifierId_, HHuginn::function_t const& function_, yaal::hcore::HString const& doc_ ) const {
		return ( _functionReferencePool.create( identifierId_, function_, doc_ ) );
	}
	HHuginn::value_t create_bound_method( HHuginn::function_t const& method_, HHuginn::value_t const& object_ ) const {
		return ( _boundMethodPool.create( method_, object_ ) );
	}
	HHuginn::value_t create_object( HHuginn::HClass const* class_, HHuginn::HObject::fields_t const& fields_ ) const {
		return ( _objectPool.create( class_, fields_ ) );
	}
	HHuginn::HClass const* boolean_class( void ) const {
		return ( _boolean.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exception.raw() );
	}
	HHuginn::HClass const* conversion_exception_class( void ) const {
		return ( _conversionException.raw() );
	}
	HHuginn::HClass const* arithmetic_exception_class( void ) const {
		return ( _arithmeticException.raw() );
	}
private:
	HObjectFactory( HObjectFactory const& ) = delete;
	HObjectFactory& operator = ( HObjectFactory const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBJECTFACTORY_HXX_INCLUDED */


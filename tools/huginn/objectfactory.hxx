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

template<typename T>
class HObjectPool final {
private:
	typedef yaal::hcore::HPointer<T> object_ptr_t;
	typedef typename object_ptr_t::template allocated_shared<allocator::shared_pool<T>> shared_t;
	typedef yaal::hcore::HPool<shared_t::size> pool_t;
	typedef allocator::shared_pool<typename shared_t::type> allocator_t;
	pool_t _pool;
	allocator_t _allocator;
	HHuginn::HClass const* _class;
public:
	HObjectPool( HHuginn::HClass const* class_ = nullptr )
		: _pool()
		, _allocator( _pool )
		, _class( class_ ) {
		return;
	}
	template<typename... arg_t>
	HHuginn::value_t create( arg_t&&... arg_ ) const {
		return ( yaal::hcore::allocate_pointer<allocator_t, T>( _allocator, _class, yaal::forward<arg_t>( arg_ )... ) );
	}
	template<typename... arg_t>
	HHuginn::value_t create_nc( arg_t&&... arg_ ) const {
		return ( yaal::hcore::allocate_pointer<allocator_t, T>( _allocator, yaal::forward<arg_t>( arg_ )... ) );
	}
private:
	HObjectPool( HObjectPool const& ) = delete;
	HObjectPool& operator = ( HObjectPool const& ) = delete;
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
	HObjectPool<HHuginn::HDict> _dictPool;
	HObjectPool<HHuginn::HLookup> _lookupPool;
	HObjectPool<HHuginn::HOrder> _orderPool;
	HObjectPool<HHuginn::HSet> _setPool;
	HObjectPool<HHuginn::HReference> _referencePool;
	HObjectPool<HHuginn::HFunctionReference> _functionReferencePool;
	HObjectPool<HHuginn::HClass::HBoundMethod> _boundMethodPool;
	HObjectPool<HHuginn::HObject> _objectPool;
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
	HHuginn::value_t create_list( HHuginn::values_t const& values_ = HHuginn::values_t() ) const {
		return ( _listPool.create( values_ ) );
	}
	HHuginn::value_t create_deque( HHuginn::HDeque::values_t const& values_ = HHuginn::HDeque::values_t() ) const {
		return ( _dequePool.create( values_ ) );
	}
	HHuginn::value_t create_dict( void ) const {
		return ( _dictPool.create() );
	}
	HHuginn::value_t create_dict( HHuginn::HDict::values_t const& data_, HHuginn::HClass const* keyType_ ) const {
		return ( _dictPool.create( data_, keyType_ ) );
	}
	HHuginn::value_t create_order( void ) const {
		return ( _orderPool.create() );
	}
	HHuginn::value_t create_order( HHuginn::HOrder::values_t const& data_, HHuginn::HClass const* keyType_ ) const {
		return ( _orderPool.create( data_, keyType_ ) );
	}
	HHuginn::value_t create_lookup( void ) const {
		return ( _lookupPool.create() );
	}
	HHuginn::value_t create_lookup( HHuginn::HLookup::values_t const& data_ ) const {
		return ( _lookupPool.create( data_ ) );
	}
	HHuginn::value_t create_set( void ) const {
		return ( _setPool.create() );
	}
	HHuginn::value_t create_set( HHuginn::HSet::values_t const& data_ ) const {
		return ( _setPool.create( data_ ) );
	}
	HHuginn::value_t create_reference( HHuginn::value_t& value_ ) const {
		return ( _referencePool.create_nc( value_ ) );
	}
	HHuginn::value_t create_function_reference( HHuginn::identifier_id_t identifierId_, HHuginn::function_t const& function_, yaal::hcore::HString const& doc_ ) const {
		return ( _functionReferencePool.create_nc( identifierId_, function_, doc_ ) );
	}
	HHuginn::value_t create_bound_method( HHuginn::HClass::HMethod const& method_, HHuginn::value_t const& object_ ) const {
		return ( _boundMethodPool.create_nc( method_, object_ ) );
	}
	HHuginn::value_t create_object( HHuginn::HClass const* class_ ) const {
		return ( _objectPool.create_nc( class_ ) );
	}
	HHuginn::value_t create_object( HHuginn::HClass const* class_, HHuginn::HObject::fields_t const& fields_ ) const {
		return ( _objectPool.create_nc( class_, fields_ ) );
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


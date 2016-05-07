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

class HObjectFactory final {
	HHuginn* _huginn;
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
	/* string pool */
	typedef yaal::hcore::HPointer<HHuginn::HString> string_ptr_t;
	typedef string_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HString>> shared_string_t;
	typedef yaal::hcore::HPool<shared_string_t::size> string_pool_t;
	typedef allocator::shared_pool<shared_string_t::type> string_allocator_t;
	string_pool_t _stringPool;
	string_allocator_t _stringAllocator;
	/* integer pool */
	typedef yaal::hcore::HPointer<HHuginn::HInteger> integer_ptr_t;
	typedef integer_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HInteger>> shared_integer_t;
	typedef yaal::hcore::HPool<shared_integer_t::size> integer_pool_t;
	typedef allocator::shared_pool<shared_integer_t::type> integer_allocator_t;
	integer_pool_t _integerPool;
	integer_allocator_t _integerAllocator;
	/* boolean pool */
	typedef yaal::hcore::HPointer<HHuginn::HBoolean> boolean_ptr_t;
	typedef boolean_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HBoolean>> shared_boolean_t;
	typedef yaal::hcore::HPool<shared_boolean_t::size> boolean_pool_t;
	typedef allocator::shared_pool<shared_boolean_t::type> boolean_allocator_t;
	boolean_pool_t _booleanPool;
	boolean_allocator_t _booleanAllocator;
	/* real pool */
	typedef yaal::hcore::HPointer<HHuginn::HReal> real_ptr_t;
	typedef real_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HReal>> shared_real_t;
	typedef yaal::hcore::HPool<shared_real_t::size> real_pool_t;
	typedef allocator::shared_pool<shared_real_t::type> real_allocator_t;
	real_pool_t _realPool;
	real_allocator_t _realAllocator;
	/* number pool */
	typedef yaal::hcore::HPointer<HHuginn::HNumber> number_ptr_t;
	typedef number_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HNumber>> shared_number_t;
	typedef yaal::hcore::HPool<shared_number_t::size> number_pool_t;
	typedef allocator::shared_pool<shared_number_t::type> number_allocator_t;
	number_pool_t _numberPool;
	number_allocator_t _numberAllocator;
	/* character pool */
	typedef yaal::hcore::HPointer<HHuginn::HCharacter> character_ptr_t;
	typedef character_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HCharacter>> shared_character_t;
	typedef yaal::hcore::HPool<shared_character_t::size> character_pool_t;
	typedef allocator::shared_pool<shared_character_t::type> character_allocator_t;
	character_pool_t _characterPool;
	character_allocator_t _characterAllocator;
	/* reference pool */
	typedef yaal::hcore::HPointer<HHuginn::HReference> reference_ptr_t;
	typedef reference_ptr_t::allocated_shared<allocator::shared_pool<HHuginn::HReference>> shared_reference_t;
	typedef yaal::hcore::HPool<shared_reference_t::size> reference_pool_t;
	typedef allocator::shared_pool<shared_reference_t::type> reference_allocator_t;
	reference_pool_t _referencePool;
	reference_allocator_t _referenceAllocator;
public:
	HObjectFactory( HHuginn* );
	void register_builtin_classes( void );
	HHuginn& huginn( void ) const {
		return ( *_huginn );
	}
	HHuginn::value_t create_boolean( HHuginn::HBoolean::value_type value_ ) const {
		return ( yaal::hcore::allocate_pointer<boolean_allocator_t, HHuginn::HBoolean>( _booleanAllocator, _boolean.raw(), value_ ) );
	}
	HHuginn::value_t create_integer( HHuginn::HInteger::value_type value_ ) const {
		return ( yaal::hcore::allocate_pointer<integer_allocator_t, HHuginn::HInteger>( _integerAllocator, _integer.raw(), value_ ) );
	}
	HHuginn::value_t create_string( yaal::hcore::HString const& value_ = yaal::hcore::HString() ) const {
		return ( yaal::hcore::allocate_pointer<string_allocator_t, HHuginn::HString>( _stringAllocator, _string.raw(), value_ ) );
	}
	HHuginn::value_t create_real( HHuginn::HReal::value_type value_ ) const {
		return ( yaal::hcore::allocate_pointer<real_allocator_t, HHuginn::HReal>( _realAllocator, _real.raw(), value_ ) );
	}
	HHuginn::value_t create_number( HHuginn::HNumber::value_type const& value_ ) const {
		return ( yaal::hcore::allocate_pointer<number_allocator_t, HHuginn::HNumber>( _numberAllocator, _number.raw(), value_ ) );
	}
	HHuginn::value_t create_character( HHuginn::HCharacter::value_type value_ ) const {
		return ( yaal::hcore::allocate_pointer<character_allocator_t, HHuginn::HCharacter>( _characterAllocator, _character.raw(), value_ ) );
	}
	HHuginn::value_t create_list( HHuginn::values_t const& values_ = HHuginn::values_t() ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HList>( _list.raw(), values_ ) );
	}
	HHuginn::value_t create_deque( HHuginn::HDeque::values_t const& values_ = HHuginn::HDeque::values_t() ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDeque>( _deque.raw(), values_ ) );
	}
	HHuginn::value_t create_dict( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDict>( _dict.raw() ) );
	}
	HHuginn::value_t create_dict( HHuginn::HDict::values_t const& data_, HHuginn::HClass const* keyType_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HDict>( _dict.raw(), data_, keyType_ ) );
	}
	HHuginn::value_t create_order( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HOrder>( _order.raw() ) );
	}
	HHuginn::value_t create_order( HHuginn::HOrder::values_t const& data_, HHuginn::HClass const* keyType_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HOrder>( _order.raw(), data_, keyType_ ) );
	}
	HHuginn::value_t create_lookup( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HLookup>( _lookup.raw() ) );
	}
	HHuginn::value_t create_lookup( HHuginn::HLookup::values_t const& data_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HLookup>( _lookup.raw(), data_ ) );
	}
	HHuginn::value_t create_set( void ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HSet>( _set.raw() ) );
	}
	HHuginn::value_t create_set( HHuginn::HSet::values_t const& data_ ) const {
		return ( yaal::hcore::make_pointer<HHuginn::HSet>( _set.raw(), data_ ) );
	}
	HHuginn::value_t create_reference( HHuginn::value_t& value_ ) const {
		return ( yaal::hcore::allocate_pointer<reference_allocator_t, HHuginn::HReference>( _referenceAllocator, value_ ) );
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


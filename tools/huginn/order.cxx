/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  order.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "compiler.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HOrderIterator : public HIteratorInterface {
	HHuginn::HOrder::values_t* _order;
	HHuginn::HOrder::values_t::iterator _it;
public:
	HOrderIterator( HHuginn::HOrder::values_t* order_ )
		: _order( order_ ), _it( order_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _order->end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HOrderIterator( HOrderIterator const& ) = delete;
	HOrderIterator& operator = ( HOrderIterator const& ) = delete;
};

namespace order {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.add", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_->raw() ) );
	M_ASSERT( o != nullptr );
	o->insert( values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.has_key", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_->raw() ) );
	M_ASSERT( o != nullptr );
	bool hasKey( o->has_key( values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.erase", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_->raw() ) );
	M_ASSERT( o != nullptr );
	o->erase( values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.clear", values_, 0, 0, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_->raw() ) );
	M_ASSERT( o != nullptr );
	o->value().clear();
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::ORDER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::ORDER ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "add",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::add, _1, _2, _3, _4 ) ) },
				{ "has_key", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::has_key, _1, _2, _3, _4 ) ) },
				{ "erase",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::erase, _1, _2, _3, _4 ) ) },
				{ "clear",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::clear, _1, _2, _3, _4 ) ) }
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HOrder::HOrder( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _data( &value_builtin::less_low )
	, _keyType( &huginn::_noneClass_ ) {
	return;
}

HHuginn::HOrder::HOrder( HHuginn::HClass const* class_, values_t const& data_, HHuginn::HClass const* keyType_ )
	: HIterable( class_ ),
	_data(),
	_keyType( keyType_ ) {
	HRuntime* huginn( class_->runtime() );
	for ( values_t::value_type const& v : data_ ) {
		_data.insert( _data.end(), v->clone( huginn ) );
	}
	return;
}

int long HHuginn::HOrder::do_size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HOrder::verify_key_type( HHuginn::HHuginn::HClass const* typeId_, int position_ ) const {
	if ( ( _keyType->type_id() != TYPE::NONE ) && ( typeId_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types.", position_ );
	}
	if ( ! OCompiler::is_comparable( typeId_->type_id() ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( typeId_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

bool HHuginn::HOrder::has_key( HHuginn::value_t const& value_, int position_ ) const {
	M_PROLOG
	verify_key_type( value_->get_class(), position_ );
	return ( _data.find( value_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HOrder::erase( HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( value_->get_class(), position_ );
	_data.erase( value_ );
	return;
	M_EPILOG
}

void HHuginn::HOrder::insert( HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( value_->get_class(), position_ );
	_data.insert( value_ );
	_keyType = value_->get_class();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HOrder::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HOrderIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HOrder::do_clone( HRuntime* runtime_ ) const {
	return ( runtime_->object_factory()->create_order( _data, _keyType ) );
}

}

}


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
#include "iterator.hxx"
#include "compiler.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HHuginn::type_t const HHuginn::TYPE::ORDER( HHuginn::HType::register_type( "order", nullptr ) );

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
	virtual HHuginn::value_t do_value( void ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _order->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HOrderIterator( HOrderIterator const& ) = delete;
	HOrderIterator& operator = ( HOrderIterator const& ) = delete;
};

namespace order {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.add", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_ ) );
	M_ASSERT( o != nullptr );
	o->insert( values_[0], position_ );
	return ( object_->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.has_key", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_ ) );
	M_ASSERT( o != nullptr );
	bool hasKey( o->has_key( values_[0], position_ ) );
	return ( make_pointer<HHuginn::HBoolean>( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.erase", values_, 1, 1, position_ );
	HHuginn::HOrder* o( dynamic_cast<HHuginn::HOrder*>( object_ ) );
	M_ASSERT( o != nullptr );
	M_ASSERT( !! o->get_pointer() );
	o->erase( values_[0], position_ );
	return ( o->get_pointer() );
	M_EPILOG
}

}

HHuginn::HClass _orderClass_(
	nullptr,
	HHuginn::TYPE::ORDER,
	nullptr,
	/* methods */ {
		"add",
		"has_key",
		"erase"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::add, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::has_key, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &order::erase, _1, _2, _3, _4 ) )
	}
);

}

HHuginn::HOrder::HOrder( void )
	: HIterable( &huginn::_orderClass_ )
	, _data( &value_builtin::less_low )
	, _keyType( HHuginn::TYPE::NONE ) {
	return;
}

HHuginn::HOrder::HOrder( values_t const& data_, type_t keyType_ )
	: HIterable( &huginn::_orderClass_ ),
	_data( data_ ),
	_keyType( keyType_ ) {
	return;
}

int long HHuginn::HOrder::size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HOrder::verify_key_type( HHuginn::type_t type_, int position_ ) const {
	if ( ( _keyType != TYPE::NONE ) && ( type_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types.", position_ );
	}
	if ( ! OCompiler::is_comparable( type_ ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( type_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

bool HHuginn::HOrder::has_key( HHuginn::value_t const& value_, int position_ ) const {
	M_PROLOG
	verify_key_type( value_->type(), position_ );
	return ( _data.find( value_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HOrder::erase( HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( value_->type(), position_ );
	_data.erase( value_ );
	return;
	M_EPILOG
}

void HHuginn::HOrder::insert( HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( value_->type(), position_ );
	_data.insert( value_ );
	_keyType = value_->type();
	return;
	M_EPILOG
}

HHuginn::HOrder::values_t const& HHuginn::HOrder::value( void ) const {
	return ( _data );
}

HHuginn::HOrder::values_t& HHuginn::HOrder::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HOrder::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HOrderIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HOrder::do_clone( void ) const {
	return ( make_pointer<HOrder>( _data, _keyType ) );
}

}

}


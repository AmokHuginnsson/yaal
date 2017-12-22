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
	virtual bool do_is_valid( huginn::HThread*, int ) override {
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

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.insert", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HHuginn::HOrder*>( object_->raw() )->insert( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.has_key", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	bool hasKey( static_cast<HHuginn::HOrder*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HHuginn::HOrder*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HHuginn::HOrder*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	verify_signature( "order.update", values_, { HHuginn::TYPE::ORDER }, thread_, position_ );
	HHuginn::HOrder& l( *static_cast<HHuginn::HOrder*>( object_->raw() ) );
	HHuginn::HOrder const& r( *static_cast<HHuginn::HOrder const*>( values_[0].raw() ) );
	if ( r.key_type()->type_id() != HHuginn::TYPE::NONE ) {
		l.verify_key_type( thread_, r.key_type(), position_ );
	}
	HHuginn::HOrder::values_t& lv( l.value() );
	HHuginn::HOrder::values_t const& rv( r.value() );
	l.anchor( thread_, position_ );
	for ( HHuginn::HOrder::values_t::const_iterator it( rv.begin() ), end( rv.end() ); it != end; ++ it ) {
		lv.insert( *it );
	}
	l.detach();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	HHuginn::HOrder::values_t const& values( static_cast<HHuginn::HOrder*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::ORDER ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += value_builtin::hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	verify_signature( "order.equals", values_, { HHuginn::TYPE::ORDER }, thread_, position_ );
	HHuginn::HOrder::values_t const& l( static_cast<HHuginn::HOrder*>( object_->raw() )->value() );
	HHuginn::HOrder::values_t const& r( static_cast<HHuginn::HOrder const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( HHuginn::HOrder::values_t::const_iterator lit( l.begin() ), rit( r.begin() ), end( l.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		equal = value_builtin::equals( thread_, *lit, *rit, position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::ORDER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::ORDER ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "insert",  objectFactory_->create_method( hcore::call( &order::insert, _1, _2, _3, _4 ) ),  "( *elem* ) - insert given element *elem* into an `order`" },
				{ "has_key", objectFactory_->create_method( hcore::call( &order::has_key, _1, _2, _3, _4 ) ), "( *elem* ) - tell if given element *elem* is in the `order`" },
				{ "erase",   objectFactory_->create_method( hcore::call( &order::erase, _1, _2, _3, _4 ) ),   "( *elem* ) - remove given element *elem* from the `order`" },
				{ "clear",   objectFactory_->create_method( hcore::call( &order::clear, _1, _2, _3, _4 ) ),   "erase `order`'s content, `order` becomes empty" },
				{ "add",     objectFactory_->create_method( hcore::call( &order::update, _1, _2, _3, _4 ) ),  "( *other* ) - update content of this `order` with values added from *other* `order`" },
				{ "update",  objectFactory_->create_method( hcore::call( &order::update, _1, _2, _3, _4 ) ),  "( *other* ) - update content of this `order` with values from *other* `order`" },
				{ "hash",    objectFactory_->create_method( hcore::call( &order::hash, _1, _2, _3, _4 ) ),    "calculate hash value for this `order`" },
				{ "equals",  objectFactory_->create_method( hcore::call( &order::equals, _1, _2, _3, _4 ) ),  "( *other* ) - test if *other* `order` has the same content" }
			},
			"The `order` is a collection of sorted values of uniform types. It supports operations of addition, search and element removal."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HOrder::HOrder( HHuginn::HClass const* class_, allocator_t const& allocator_ )
	: HIterable( class_ )
	, _helper()
	, _data( _helper, allocator_ )
	, _keyType( &huginn::_noneClass_ ) {
	return;
}

int long HHuginn::HOrder::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHuginn::HOrder::verify_key_type( huginn::HThread* thread_, HHuginn::HHuginn::HClass const* keyType_, int position_ ) const {
	if ( ( _keyType->type_id() != TYPE::NONE ) && ( keyType_ != _keyType ) ) {
		throw HHuginnRuntimeException(
			"Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	if ( ! OCompiler::is_comparable( keyType_->type_id() ) ) {
		throw HHuginnRuntimeException(
			"Key type `"_ys.append( keyType_->name() ).append( "' is not a comparable." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return;
}

bool HHuginn::HOrder::has_key( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) const {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	bool has( _data.find( value_ ) != _data.end() );
	_helper.detach();
	return ( has );
	M_EPILOG
}

void HHuginn::HOrder::erase( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	_data.erase( value_ );
	_helper.detach();
	return;
	M_EPILOG
}

void HHuginn::HOrder::insert( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	_data.insert( value_ );
	_helper.detach();
	_keyType = value_->get_class();
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HOrder::key_type( void ) const {
	return ( _keyType );
}

HHuginn::HIterable::HIterator HHuginn::HOrder::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HOrderIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HOrder::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_order() );
	HHuginn::HOrder* order( static_cast<HHuginn::HOrder*>( res.raw() ) );
	values_t&  data( order->value() );
	order->_keyType = _keyType;
	order->anchor( thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert( data.end(), v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	order->detach();
	return ( res );
}

}

}


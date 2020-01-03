/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "order.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "iterator.hxx"
#include "compiler.hxx"
#include "instruction.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace order {

class HOrderIterator : public HNotifableIterator {
	HOrder::values_t* _order;
	HOrder::values_t::iterator _it;
public:
	HOrderIterator( HOrder* owner_ )
		: HNotifableIterator( owner_ )
		, _order( &owner_->value() )
		, _it( _order->begin() ) {
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
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _order->end();
	}
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HOrderIterator( HOrderIterator const& ) = delete;
	HOrderIterator& operator = ( HOrderIterator const& ) = delete;
};

class HOrderReverseIterator : public HNotifableIterator {
	HOrder::values_t* _order;
	HOrder::values_t::reverse_iterator _it;
public:
	HOrderReverseIterator( HOrder* owner_ )
		: HNotifableIterator( owner_ )
		, _order( &owner_->value() )
		, _it( _order->rbegin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _it != _order->rend() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _order->rend();
	}
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.raw().node_id() );
	}
private:
	HOrderReverseIterator( HOrderReverseIterator const& ) = delete;
	HOrderReverseIterator& operator = ( HOrderReverseIterator const& ) = delete;
};

class HReversedOrder : public huginn::HIterable {
	HHuginn::value_t _order;
public:
	HReversedOrder( HClass const* class_, HHuginn::value_t const& order_ )
		: HIterable( class_ )
		, _order( order_ ) {
		M_ASSERT( _order->type_id() == HHuginn::TYPE::ORDER );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectfactory_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				objectfactory_,
				"ReversedOrderView",
				"The `ReversedOrderView` class represents *lazy* *iterable* reversed view of a `order`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HOrder const*>( _order.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return ( make_pointer<HOrderReverseIterator>( static_cast<HOrder*>( _order.raw() ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedOrder>( HIterable::get_class(), _order ) );
	}
};

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.insert", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HOrder*>( object_->raw() )->insert( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.has_key", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	bool hasKey( static_cast<HOrder*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->runtime().boolean_value( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HOrder*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "order.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	static_cast<HOrder*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::ORDER );
	verify_signature( "order.update", values_, { HHuginn::TYPE::ORDER }, thread_, position_ );
	(*object_)->operator_add( thread_, *object_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

class HOrderClass : public HClass {
public:
	typedef HOrderClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _reversedOrderClass;
public:
	HOrderClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::ORDER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::ORDER ) ),
			"The `order` is a collection of sorted values of uniform types. It supports operations of addition, search and element removal.",
			&builtin::order
		)
		, _reversedOrderClass() {
		HHuginn::field_definitions_t fd{
			{ "insert",  objectFactory_->create_method( &order::insert ),  "( *elem* ) - insert given element *elem* into an `order`" },
			{ "has_key", objectFactory_->create_method( &order::has_key ), "( *elem* ) - tell if given element *elem* is in the `order`" },
			{ "erase",   objectFactory_->create_method( &order::erase ),   "( *elem* ) - remove given element *elem* from the `order`" },
			{ "clear",   objectFactory_->create_method( &order::clear ),   "erase `order`'s content, `order` becomes empty" },
			{ "update",  objectFactory_->create_method( &order::update ),  "( *other* ) - update content of this `order` with values from *other* `order`" }
		};
		redefine( nullptr, fd );
		_reversedOrderClass = add_class_as_type_reference( this, HReversedOrder::get_class( runtime_, objectFactory_, this ), HClass::MEMBER_TYPE::STATIC );
		return;
	}
	HClass const* reversed_order_class( void ) const {
		return ( _reversedOrderClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _reversedOrderClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HOrderClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::ORDER );
	HOrderClass const* lc( static_cast<HOrderClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedOrder>( lc->reversed_order_class(), value_ ) );
	M_EPILOG
}

}

HOrder::HOrder( HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper( &instruction::less )
	, _data( _helper, allocator_ )
	, _keyType( nullptr ) {
	return;
}

int long HOrder::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HOrder::verify_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) const {
	M_ASSERT( keyType_ );
	if ( _keyType && ( keyType_ != _keyType ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return;
}

void HOrder::update_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) {
	verify_key_type( thread_, keyType_, position_ );
	_keyType = keyType_;
	return;
}

bool HOrder::has_key( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) const {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HOrder> ag( *this, thread_, position_ );
	return ( _data.find( value_ ) != _data.end() );
	M_EPILOG
}

void HOrder::erase( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HOrder> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( value_ ) );
	if ( it != _data.end() ) {
		skip( thread_, it.node_id(), position_ );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

void HOrder::insert( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	update_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HOrder> ag( *this, thread_, position_ );
	_data.insert( value_ );
	return;
	M_EPILOG
}

void HOrder::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	_keyType = nullptr;
	return;
	M_EPILOG
}

HClass const* HOrder::key_type( void ) const {
	return ( _keyType );
}

HIterable::iterator_t HOrder::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<huginn::order::HOrderIterator>( this ) );
}

HHuginn::value_t HOrder::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_order() );
	HOrder* order( static_cast<HOrder*>( res.raw() ) );
	values_t&  data( order->value() );
	order->_keyType = _keyType;
	HAnchorGuard<HOrder> ag( *order, thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert( data.end(), v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( res );
}

bool HOrder::do_operator_equals( huginn::HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	HOrder const& otherValue( *static_cast<HOrder const*>( other_.raw() ) );
	HOrder::values_t const& otherData( otherValue.value() );
	bool equal( ( _data.get_size() == otherData.get_size() ) && ( _keyType == otherValue._keyType ) );
	for ( HOrder::values_t::const_iterator lit( _data.begin() ), rit( otherData.begin() ), end( _data.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		HHuginn::value_t const& v( *lit );
		equal = v->operator_equals( thread_, v, *rit, position_ );
	}
	return ( equal );
}

void HOrder::do_operator_add( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	HOrder const& otherValue( *static_cast<HOrder const*>( other_.raw() ) );
	if ( otherValue.key_type()->type_id() != HHuginn::TYPE::NONE ) {
		update_key_type( thread_, otherValue.key_type(), position_ );
	}
	HOrder::values_t const& otherData( otherValue.value() );
	HAnchorGuard<HOrder> ag( *this, thread_, position_ );
	for ( HOrder::values_t::const_iterator it( otherData.begin() ), end( otherData.end() ); it != end; ++ it ) {
		_data.insert( *it );
	}
	return;
}

bool HOrder::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( has_key( thread_, other_, position_ ) );
}

hash_value_t HOrder::do_operator_hash( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	hash_value_t hashValue( static_cast<hash_value_t>( HHuginn::TYPE::ORDER ) );
	for ( HHuginn::value_t const& v : _data ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( hashValue );
}

yaal::hcore::HString HOrder::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "order()" );
	}
	hcore::HString str( "order(" );
	bool next( false );
	for ( HHuginn::value_t const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->code( thread_, v, cycleTracker_, position_ ) );
	}
	str.append( ")" );
	return ( str );
}

yaal::hcore::HString HOrder::do_to_string( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "order()" );
	}
	hcore::HString str( "order(" );
	bool next( false );
	for ( HHuginn::value_t const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->to_string( thread_, v, cycleTracker_, position_ ) );
	}
	str.append( ")" );
	return ( str );
}

}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "order.hxx"
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

namespace order {

class HOrderIterator : public HNotifableIterator {
	HHuginn::HOrder::values_t* _order;
	HHuginn::HOrder::values_t::iterator _it;
public:
	HOrderIterator( HHuginn::HOrder* owner_ )
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
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HOrderIterator( HOrderIterator const& ) = delete;
	HOrderIterator& operator = ( HOrderIterator const& ) = delete;
};

class HOrderReverseIterator : public HNotifableIterator {
	HHuginn::HOrder::values_t* _order;
	HHuginn::HOrder::values_t::reverse_iterator _it;
public:
	HOrderReverseIterator( HHuginn::HOrder* owner_ )
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
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.base().node_id() );
	}
private:
	HOrderReverseIterator( HOrderReverseIterator const& ) = delete;
	HOrderReverseIterator& operator = ( HOrderReverseIterator const& ) = delete;
};

class HReversedOrder : public HHuginn::HIterable {
	HHuginn::value_t _order;
public:
	HReversedOrder( HHuginn::HClass const* class_, HHuginn::value_t const& order_ )
		: HIterable( class_ )
		, _order( order_ ) {
		M_ASSERT( _order->type_id() == HHuginn::TYPE::ORDER );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				"ReversedOrderView",
				"The `ReversedOrderView` class represents *lazy* *iterable* reversed view of a `order`."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HOrder const*>( _order.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HOrderReverseIterator( static_cast<HHuginn::HOrder*>( _order.raw() ) ) );
		return ( HIterator( yaal::move( impl ) ) );
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
	static_cast<HHuginn::HOrder*>( object_->raw() )->clear();
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
	HAnchorGuard<HHuginn::HOrder> ag( l, thread_, position_ );
	for ( HHuginn::HOrder::values_t::const_iterator it( rv.begin() ), end( rv.end() ); it != end; ++ it ) {
		lv.insert( *it );
	}
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

class HOrderClass : public HHuginn::HClass {
public:
	typedef HOrderClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _reversedOrderClass;
public:
	HOrderClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HHuginn::HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::ORDER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::ORDER ) ),
			"The `order` is a collection of sorted values of uniform types. It supports operations of addition, search and element removal.",
			&huginn_builtin::order
		)
		, _reversedOrderClass( HReversedOrder::get_class( runtime_ ) ) {
		HHuginn::field_definitions_t fd{
			{ "insert",  objectFactory_->create_method( &order::insert ),  "( *elem* ) - insert given element *elem* into an `order`" },
			{ "has_key", objectFactory_->create_method( &order::has_key ), "( *elem* ) - tell if given element *elem* is in the `order`" },
			{ "erase",   objectFactory_->create_method( &order::erase ),   "( *elem* ) - remove given element *elem* from the `order`" },
			{ "clear",   objectFactory_->create_method( &order::clear ),   "erase `order`'s content, `order` becomes empty" },
			{ "add",     objectFactory_->create_method( &order::update ),  "( *other* ) - update content of this `order` with values added from *other* `order`" },
			{ "update",  objectFactory_->create_method( &order::update ),  "( *other* ) - update content of this `order` with values from *other* `order`" },
			{ "hash",    objectFactory_->create_method( &order::hash ),    "calculate hash value for this `order`" },
			{ "equals",  objectFactory_->create_method( &order::equals ),  "( *other* ) - test if *other* `order` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HHuginn::HClass const* reversed_order_class( void ) const {
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

}

HHuginn::HOrder::HOrder( HHuginn::HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper( &value_builtin::less )
	, _data( _helper, allocator_ )
	, _keyType( nullptr ) {
	return;
}

int long HHuginn::HOrder::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHuginn::HOrder::verify_key_type( huginn::HThread* thread_, HHuginn::HHuginn::HClass const* keyType_, int position_ ) const {
	if ( _keyType && ( keyType_ != _keyType ) ) {
		throw HHuginnRuntimeException(
			"Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	if ( ! is_comparable( keyType_ ) ) {
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
	HAnchorGuard<HHuginn::HOrder> ag( *this, thread_, position_ );
	return ( _data.find( value_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HOrder::erase( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HHuginn::HOrder> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( value_ ) );
	if ( it != _data.end() ) {
		invalidate( it.node_id() );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

void HHuginn::HOrder::insert( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HHuginn::HOrder> ag( *this, thread_, position_ );
	_data.insert( value_ );
	_keyType = value_->get_class();
	return;
	M_EPILOG
}

void HHuginn::HOrder::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HOrder::key_type( void ) const {
	return ( _keyType );
}

HHuginn::HIterable::HIterator HHuginn::HOrder::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::order::HOrderIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HOrder::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_order() );
	HHuginn::HOrder* order( static_cast<HHuginn::HOrder*>( res.raw() ) );
	values_t&  data( order->value() );
	order->_keyType = _keyType;
	HAnchorGuard<HHuginn::HOrder> ag( *order, thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert( data.end(), v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( res );
}

}

}


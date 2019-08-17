/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "heap.hxx"
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

namespace heap {

class HHeapIterator : public HNotifableIterator {
	HHeap const* _owner;
	HHeap::values_t _heap;
public:
	HHeapIterator( HHeap* owner_ )
		: HNotifableIterator( owner_ )
		, _owner( owner_ )
		, _heap( owner_->value() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _heap.top() );
	}
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( ! _heap.is_empty() );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		if ( ! _heap.is_empty() ) {
			HAnchorGuard<HHeap> ag( *_owner, thread_, position_ );
			_heap.pop();
		}
	}
	virtual void do_invalidate( void ) override {
		_heap.clear();
	}
	virtual void do_skip( HThread* thread_, int position_ ) override {
		HAnchorGuard<HHeap> ag( *_owner, thread_, position_ );
		_heap.pop();
	}
	virtual void const* do_id( void ) const override {
		return ( _heap.top().raw() );
	}
private:
	HHeapIterator( HHeapIterator const& ) = delete;
	HHeapIterator& operator = ( HHeapIterator const& ) = delete;
};

inline HHuginn::value_t push( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "heap.push", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	static_cast<HHeap*>( object_->raw() )->push( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "heap.pop", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	return ( static_cast<HHeap*>( object_->raw() )->pop( thread_, position_ ) );
	M_EPILOG
}

inline HHuginn::value_t top( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "heap.top", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	return ( static_cast<HHeap*>( object_->raw() )->top( thread_, position_ ) );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "heap.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	static_cast<HHeap*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	verify_signature( "heap.update", values_, { HHuginn::TYPE::HEAP }, thread_, position_ );
	HHeap& l( *static_cast<HHeap*>( object_->raw() ) );
	HHeap const& r( *static_cast<HHeap const*>( values_[0].raw() ) );
	if ( r.key_type()->type_id() != HHuginn::TYPE::NONE ) {
		l.update_key_type( thread_, r.key_type(), position_ );
	}
	HHeap::values_t& lv( l.value() );
	HHeap::values_t rv( r.value() );
	HAnchorGuard<HHeap> agL( l, thread_, position_ );
	HAnchorGuard<HHeap> agR( r, thread_, position_ );
	while ( ! rv.is_empty() ) {
		lv.push( rv.top() );
		rv.pop();
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "heap.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	HHeap& heap( *static_cast<HHeap*>( object_->raw() ) );
	HHeap::values_t values( static_cast<HHeap*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::HEAP ) );
	HAnchorGuard<HHeap> ag( heap, thread_, position_ );
	while ( ! values.is_empty() ) {
		hashValue *= 3;
		HHuginn::value_t const& v( values.top() );
		hashValue += v->operator_hash( thread_, v, position_ );
		values.pop();
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::HEAP );
	verify_signature( "heap.equals", values_, { HHuginn::TYPE::HEAP }, thread_, position_ );
	HHeap const& l( *static_cast<HHeap*>( object_->raw() ) );
	HHeap const& r( *static_cast<HHeap const*>( values_[0].raw() ) );
	HHeap::values_t ld( l.value() );
	HHeap::values_t rd( r.value() );
	bool equal( ( ld.get_size() == rd.get_size() ) && ( l.key_type() == r.key_type() ) );
	HAnchorGuard<HHeap> agL( l, thread_, position_ );
	HAnchorGuard<HHeap> agR( r, thread_, position_ );
	while ( equal && ! ld.is_empty() ) {
		HHuginn::value_t const& v( ld.top() );
		equal = v->operator_equals( thread_, v, rd.top(), position_ );
		ld.pop();
		rd.pop();
	}
	return ( thread_->runtime().boolean_value( equal ) );
	M_EPILOG
}

class HHeapClass : public HClass {
public:
	typedef HHeapClass this_type;
	typedef HClass base_type;
public:
	HHeapClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::HEAP ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::HEAP ) ),
			"The `heap` is a collection of sorted values of uniform types. It supports operations of addition, search and element removal.",
			&builtin::heap
		) {
		HHuginn::field_definitions_t fd{
			{ "push",   objectFactory_->create_method( &heap::push ),   "( *elem* ) - push given element *elem* onto a `heap`" },
			{ "pop",    objectFactory_->create_method( &heap::pop ),    "remove (and retrieve) top element from the `heap`" },
			{ "top",    objectFactory_->create_method( &heap::top ),    "inspect the top element on the `heap`" },
			{ "clear",  objectFactory_->create_method( &heap::clear ),  "clear `heap`'s content, `heap` becomes empty" },
			{ "add",    objectFactory_->create_method( &heap::update ), "( *other* ) - update content of this `heap` with values added from *other* `heap`" },
			{ "update", objectFactory_->create_method( &heap::update ), "( *other* ) - update content of this `heap` with values from *other* `heap`" },
			{ "hash",   objectFactory_->create_method( &heap::hash ),   "calculate hash value for this `heap`" },
			{ "equals", objectFactory_->create_method( &heap::equals ), "( *other* ) - test if *other* `heap` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HHeapClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

}

HHeap::HHeap( HClass const* class_ )
	: HInvalidatingIterable( class_ )
	, _helper( &instruction::less )
	, _data( _helper )
	, _keyType( nullptr ) {
	return;
}

int long HHeap::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHeap::verify_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) const {
	M_ASSERT( keyType_ );
	if ( _keyType && ( keyType_ != _keyType ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	if ( ! is_comparable( keyType_ ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Key type `"_ys.append( keyType_->name() ).append( "` is not a comparable." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return;
}

void HHeap::update_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) {
	verify_key_type( thread_, keyType_, position_ );
	_keyType = keyType_;
	return;
}

HHuginn::value_t HHeap::pop( huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	if ( _data.is_empty() ) {
		throw HHuginn::HHuginnRuntimeException(
			"The `heap` is empty.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HHuginn::value_t v( _data.top() );
	skip( thread_, v.raw(), position_ );
	HAnchorGuard<HHeap> ag( *this, thread_, position_ );
	_data.pop();
	return ( v );
	M_EPILOG
}

HHuginn::value_t HHeap::top( huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	if ( _data.is_empty() ) {
		throw HHuginn::HHuginnRuntimeException(
			"The `heap` is empty.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( _data.top() );
	M_EPILOG
}

void HHeap::push( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	update_key_type( thread_, value_->get_class(), position_ );
	HAnchorGuard<HHeap> ag( *this, thread_, position_ );
	_data.push( value_ );
	return;
	M_EPILOG
}

void HHeap::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	_keyType = nullptr;
	return;
	M_EPILOG
}

HClass const* HHeap::key_type( void ) const {
	return ( _keyType );
}

HIterable::iterator_t HHeap::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<huginn::heap::HHeapIterator>( this ) );
}

HHuginn::value_t HHeap::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_heap() );
	HHeap* heap( static_cast<HHeap*>( res.raw() ) );
	values_t&  data( heap->value() );
	heap->_keyType = _keyType;
	values_t values( _data );
	HAnchorGuard<HHeap> agDst( *heap, thread_, position_ );
	HAnchorGuard<HHeap> agSrc( *this, thread_, position_ );
	while ( ! values.is_empty() ) {
		HHuginn::value_t const& v( values.top() );
		data.push( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
		values.pop();
	}
	return ( res );
}

yaal::hcore::HString HHeap::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "heap()" );
	}
	hcore::HString str( "heap(" );
	values_t values( _data );
	bool next( false );
	HAnchorGuard<HHeap> ag( *this, thread_, position_ );
	while ( ! values.is_empty() ) {
		HHuginn::value_t const& v( values.top() );
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->code( thread_, v, cycleTracker_, position_ ) );
		values.pop();
	}
	str.append( ")" );
	return ( str );
}

yaal::hcore::HString HHeap::do_to_string( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "heap()" );
	}
	hcore::HString str( "heap(" );
	values_t values( _data );
	bool next( false );
	HAnchorGuard<HHeap> ag( *this, thread_, position_ );
	while ( ! values.is_empty() ) {
		HHuginn::value_t const& v( values.top() );
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->to_string( thread_, v, cycleTracker_, position_ ) );
		values.pop();
	}
	str.append( ")" );
	return ( str );
}

}

}

}


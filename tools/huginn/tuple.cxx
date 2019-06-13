/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tuple.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "objectfactory.hxx"
#include "instruction.hxx"
#include "tools/xmath.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace tuple {

class HTupleIterator : public HIteratorInterface {
	huginn::HTuple* _tuple;
	int long _index;
public:
	HTupleIterator( huginn::HTuple* tuple_ )
		: _tuple( tuple_ )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _tuple->get( _index ) );
	}
	virtual bool do_is_valid( huginn::HThread* thread_, int position_ ) override {
		return ( _index < _tuple->size( thread_, position_ ) );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HTupleIterator( HTupleIterator const& ) = delete;
	HTupleIterator& operator = ( HTupleIterator const& ) = delete;
};

class HTupleReverseIterator : public HIteratorInterface {
	huginn::HTuple* _tuple;
	int long _index;
public:
	HTupleReverseIterator( HThread* thread_, huginn::HTuple* tuple_, int position_ )
		: _tuple( tuple_ )
		, _index( tuple_->size( thread_, position_ ) - 1 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _tuple->get( _index ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _index >= 0 );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HTupleReverseIterator( HTupleReverseIterator const& ) = delete;
	HTupleReverseIterator& operator = ( HTupleReverseIterator const& ) = delete;
};

class HReversedTuple : public huginn::HIterable {
	HHuginn::value_t _tuple;
public:
	HReversedTuple( HClass const* class_, HHuginn::value_t const& tuple_ )
		: HIterable( class_ )
		, _tuple( tuple_ ) {
		M_ASSERT( _tuple->type_id() == HHuginn::TYPE::TUPLE );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				"ReversedTupleView",
				"The `ReversedTupleView` class represents *lazy* *iterable* reversed view of a `tuple`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HTuple const*>( _tuple.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread* thread_, int position_ ) override {
		return ( make_pointer<HTupleReverseIterator>( thread_, static_cast<huginn::HTuple*>( _tuple.raw() ), position_ ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedTuple>( HIterable::get_class(), _tuple ) );
	}
};

inline HHuginn::value_t find( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	char const name[] = "tuple.find";
	verify_arg_count( name, values_, 1, 3, thread_, position_ );
	int noArg( static_cast<int>( values_.get_size() ) );
	int long start( 0 );
	int long stop( -1 );
	huginn::HTuple& l( *static_cast<huginn::HTuple*>( object_->raw() ) );
	int long size( l.value().get_size() );
	if ( noArg > 1 ) {
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		start = xmath::clip( start, safe_int::cast<int long>( get_integer( values_[1] ) ), size );
	}
	if ( noArg > 2 ) {
		verify_arg_type( name, values_, 2, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
		stop = xmath::clip( start, safe_int::cast<int long>( get_integer( values_[2] ) ), size );
	}
	return ( thread_->object_factory().create_integer( l.find( thread_, position_, values_[0], start, stop ) ) );
	M_EPILOG
}

inline HHuginn::value_t add( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "tuple.add", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	huginn::HTuple::values_t& dst( static_cast<huginn::HTuple*>( object_->raw() )->value() );
	huginn::HTuple::values_t const& src( static_cast<huginn::HTuple const*>( values_[0].raw() )->value() );
	dst.insert( dst.end(), src.begin(), src.end() );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "tuple.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	huginn::HTuple::values_t const& values( static_cast<huginn::HTuple*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::TUPLE ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline bool less_impl( huginn::HThread* thread_, HHuginn::value_t const& l_, HHuginn::value_t const& r_, int position_ ) {
	huginn::HTuple::values_t const& l( static_cast<huginn::HTuple const*>( l_.raw() )->value() );
	huginn::HTuple::values_t const& r( static_cast<huginn::HTuple const*>( r_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( l.begin(), l.end(), r.begin(), r.end(), cref( lessHelper ) ) );
}

inline HHuginn::value_t less( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.less", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t less_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.less_or_equal", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.greater", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.greater_or_equal", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.equals", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	huginn::HTuple::values_t const& l( static_cast<huginn::HTuple*>( object_->raw() )->value() );
	huginn::HTuple::values_t const& r( static_cast<huginn::HTuple const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		HHuginn::value_t const& lv( l[i] );
		HHuginn::value_t const& rv( r[i] );
		equal = ( lv->type_id() == rv->type_id() ) && lv->operator_equals( thread_, lv, rv, position_ );
	}
	return ( thread_->runtime().boolean_value( equal ) );
	M_EPILOG
}

class HTupleClass : public HClass {
public:
	typedef HTupleClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _reversedTupleClass;
public:
	HTupleClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::TUPLE ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::TUPLE ) ),
			"The `tuple` is a collection type that is used to represent and operate on `tuple` of values. "
			"It supports basic subscript and range operators.",
			&builtin::tuple
		)
		, _reversedTupleClass( HReversedTuple::get_class( runtime_, this ) ) {
		HHuginn::field_definitions_t fd{
			{ "find",   objectFactory_->create_method( &tuple::find ),   "( *elem*[, *start*[, *stop*]] ) - get index of first *elem*ent of the `tuple` not before *start* and before *stop*, return -1 if not found" },
			{ "add",    objectFactory_->create_method( &tuple::add ),    "( *other* ) - append all elements from *other* `tuple` at the end of this `tuple`" },
			{ "hash",   objectFactory_->create_method( &tuple::hash ),   "calculate hash value for this `tuple`" },
			{ "less",   objectFactory_->create_method( &tuple::less ),   "( *other* ) - test if this `tuple` comes lexicographically before *other* `tuple`" },
			{ "less_or_equal",    objectFactory_->create_method( &tuple::less_or_equal ),    "( *other* ) - test if this `tuple` is equal to or comes lexicographically before *other* `tuple`" },
			{ "greater",          objectFactory_->create_method( &tuple::greater ),          "( *other* ) - test if this `tuple` comes lexicographically after *other* `tuple`" },
			{ "greater_or_equal", objectFactory_->create_method( &tuple::greater_or_equal ), "( *other* ) - test if this `tuple` is equal to or comes lexicographically after *other* `tuple`" },
			{ "equals", objectFactory_->create_method( &tuple::equals ), "( *other* ) - test if *other* `tuple` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HClass const* reversed_tuple_class( void ) const {
		return ( _reversedTupleClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _reversedTupleClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HTupleClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::TUPLE );
	HTupleClass const* lc( static_cast<HTupleClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedTuple>( lc->reversed_tuple_class(), value_ ) );
	M_EPILOG
}


}

HTuple::HTuple( HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

int long HTuple::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

int long HTuple::find( huginn::HThread* thread_, int position_, HHuginn::value_t const& val_, int long start_, int long stop_ ) const {
	M_PROLOG
	if ( stop_ < 0 ) {
		stop_ = _data.get_size();
	}
	values_t::const_iterator it(
		yaal::find_if(
			_data.cbegin() + start_,
			_data.cbegin() + stop_,
			[thread_, &val_, position_]( HHuginn::value_t const& elem_ ) {
				return ( val_->operator_equals( thread_, val_, elem_, position_ ) );
			}
		)
	);
	int long pos( distance( _data.cbegin(), it ) );
	return ( pos != stop_ ? pos : npos );
	M_EPILOG
}

HHuginn::value_t huginn::HTuple::get( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HIterable::iterator_t HTuple::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<tuple::HTupleIterator>( this ) );
}

HHuginn::value_t HTuple::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	values_t data;
	data.reserve( _data.get_size() );
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_tuple( yaal::move( data ) ) );
}

bool HTuple::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( find( thread_, position_, other_ ) != huginn::HTuple::npos );
}

}

}

}


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
			{ "find", objectFactory_->create_method( &tuple::find ), "( *elem*[, *start*[, *stop*]] ) - get index of first *elem*ent of the `tuple` not before *start* and before *stop*, return -1 if not found" }
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

HHuginn::value_t huginn::HTuple::get( int long long index_ ) const {
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

void HTuple::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	M_PROLOG
	huginn::HTuple::values_t const& src( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	_data.insert( _data.end(), src.begin(), src.end() );
	return;
	M_EPILOG
}

int long HTuple::do_operator_hash( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	M_PROLOG
	int long hashValue( static_cast<int long>( HHuginn::TYPE::TUPLE ) );
	for ( HHuginn::value_t const& v : _data ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( hashValue );
	M_EPILOG
}

bool HTuple::do_operator_less( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HTuple::values_t const& other( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( _data.begin(), _data.end(), other.begin(), other.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HTuple::do_operator_less_or_equal( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HTuple::values_t const& other( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( ! lexicographical_compare( other.begin(), other.end(), _data.begin(), _data.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HTuple::do_operator_greater( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HTuple::values_t const& other( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( other.begin(), other.end(), _data.begin(), _data.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HTuple::do_operator_greater_or_equal( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HTuple::values_t const& other( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( ! lexicographical_compare( _data.begin(), _data.end(), other.begin(), other.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HTuple::do_operator_equals( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HTuple::values_t const& other( static_cast<huginn::HTuple const*>( other_.raw() )->value() );
	int long s( _data.get_size() );
	bool equal( s == other.get_size() );
	for ( int long i( 0 ); equal && ( i < s ); ++ i ) {
		HHuginn::value_t const& lv( _data[i] );
		HHuginn::value_t const& rv( other[i] );
		equal = ( lv->type_id() == rv->type_id() ) && lv->operator_equals( thread_, lv, rv, position_ );
	}
	return ( equal );
	M_EPILOG
}

HHuginn::value_t HTuple::do_operator_subscript( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& index_, int position_ ) const {
	int long long index( extract_index( thread_, index_, position_ ) );
	M_ASSERT( ( index >= 0 ) && ( index < _data.get_size() ) );
	return ( _data[static_cast<int>( index )] );
}

void HTuple::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const&, HHuginn::value_t&&, int position_ ) {
	throw HHuginn::HHuginnRuntimeException( "`tuple` does not support item assignment.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::value_t HTuple::do_operator_range( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& from_, HHuginn::value_t const& to_, HHuginn::value_t const& step_, int position_ ) const {
	HIterable::ORange range( extract_range( thread_, from_, to_, step_, position_ ) );
	HHuginn::values_t v;
	if ( range.valid ) {
		if ( range.step > 0 ) {
			for ( int long i( range.from ); i < range.to; i += range.step ) {
				v.push_back( get( i ) );
			}
		} else {
			for ( int long i( range.from ); i > range.to; i += range.step ) {
				v.push_back( get( i ) );
			}
		}
	}
	return ( thread_->object_factory().create_tuple( yaal::move( v ) ) );
}

}

}

}


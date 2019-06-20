/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "deque.hxx"
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

namespace deque {

class HDequeIterator : public HIteratorInterface {
	huginn::HDeque* _deque;
	int _index;
public:
	HDequeIterator( huginn::HDeque* deque_ )
		: _deque( deque_ )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _deque->get( _index ) );
	}
	virtual bool do_is_valid( HThread* thread_ , int position_ ) override {
		return ( _index < _deque->size( thread_, position_ ) );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HDequeIterator( HDequeIterator const& ) = delete;
	HDequeIterator& operator = ( HDequeIterator const& ) = delete;
};

class HDequeReverseIterator : public HIteratorInterface {
	huginn::HDeque* _deque;
	int long _index;
public:
	HDequeReverseIterator( HThread* thread_, huginn::HDeque* deque_, int position_ )
		: _deque( deque_ )
		, _index( deque_->size( thread_, position_ ) - 1 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _deque->get( _index ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _index >= 0 );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HDequeReverseIterator( HDequeReverseIterator const& ) = delete;
	HDequeReverseIterator& operator = ( HDequeReverseIterator const& ) = delete;
};

class HReversedDeque : public huginn::HIterable {
	HHuginn::value_t _deque;
public:
	HReversedDeque( HClass const* class_, HHuginn::value_t const& deque_ )
		: HIterable( class_ )
		, _deque( deque_ ) {
		M_ASSERT( _deque->type_id() == HHuginn::TYPE::DEQUE );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				"ReversedDequeView",
				"The `ReversedDequeView` class represents *lazy* *iterable* reversed view of a `deque`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HDeque const*>( _deque.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread* thread_, int position_ ) override {
		return ( make_pointer<HDequeReverseIterator>( thread_, static_cast<huginn::HDeque*>( _deque.raw() ), position_ ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedDeque>( HIterable::get_class(), _deque ) );
	}
};

inline HHuginn::value_t push( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.push", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<huginn::HDeque*>( object_->raw() )->push_back( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<huginn::HDeque*>( object_->raw() )->pop_back();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t append( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "deque.append";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t(
		verify_arg_type(
			name, values_, 0, {
				HHuginn::TYPE::TUPLE, HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::ORDER, HHuginn::TYPE::SET
			}, ARITY::UNARY, thread_, position_
		)
	);
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	huginn::HDeque::values_t& dst( static_cast<huginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			huginn::HTuple::values_t const& src( static_cast<huginn::HTuple const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			huginn::HList::values_t const& src( static_cast<huginn::HList const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			huginn::HDeque::values_t const& src( static_cast<huginn::HDeque const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			huginn::HOrder::values_t const& src( static_cast<huginn::HOrder const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			huginn::HSet::values_t const& src( static_cast<huginn::HSet const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t prepend( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "deque.prepend";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t(
		verify_arg_type(
			name, values_, 0, {
				HHuginn::TYPE::TUPLE, HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::ORDER, HHuginn::TYPE::SET
			}, ARITY::UNARY, thread_, position_
		)
	);
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	huginn::HDeque::values_t& dst( static_cast<huginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			huginn::HTuple::values_t const& src( static_cast<huginn::HTuple const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			huginn::HList::values_t const& src( static_cast<huginn::HList const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			huginn::HDeque::values_t const& src( static_cast<huginn::HDeque const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			huginn::HOrder::values_t const& src( static_cast<huginn::HOrder const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			huginn::HSet::values_t const& src( static_cast<huginn::HSet const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "deque.insert", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::UNKNOWN }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	huginn::HDeque::values_t& dst( static_cast<huginn::HDeque*>( object_->raw() )->value() );
	huginn::HInteger::value_type pos( get_integer( values_[0] ) );
	if ( ( pos < 0 ) || ( pos > dst.get_size() ) ) {
		throw HHuginn::HHuginnRuntimeException( "invalid insertion position: "_ys.append( pos ), thread_->current_frame()->file_id(), position_ );
	}
	dst.insert( dst.begin() + static_cast<int long>( pos ), values_[1] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t push_front( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.push_front", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<huginn::HDeque*>( object_->raw() )->push_front( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop_front( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop_front", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<huginn::HDeque*>( object_->raw() )->pop_front();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<huginn::HDeque*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t find( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	char const name[] = "deque.find";
	verify_arg_count( name, values_, 1, 3, thread_, position_ );
	int noArg( static_cast<int>( values_.get_size() ) );
	int long start( 0 );
	int long stop( -1 );
	huginn::HDeque& l( *static_cast<huginn::HDeque*>( object_->raw() ) );
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

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	huginn::HDeque::values_t const& values( static_cast<huginn::HDeque*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::DEQUE ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline bool less_impl( huginn::HThread* thread_, HHuginn::value_t const& l_, HHuginn::value_t const& r_, int position_ ) {
	huginn::HDeque::values_t const& l( static_cast<huginn::HDeque const*>( l_.raw() )->value() );
	huginn::HDeque::values_t const& r( static_cast<huginn::HDeque const*>( r_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( l.begin(), l.end(), r.begin(), r.end(), cref( lessHelper ) ) );
}

inline HHuginn::value_t less( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.less", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t less_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.less_or_equal", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.greater", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.greater_or_equal", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.equals", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	huginn::HDeque::values_t const& l( static_cast<huginn::HDeque*>( object_->raw() )->value() );
	huginn::HDeque::values_t const& r( static_cast<huginn::HDeque const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		HHuginn::value_t const& lv( l[i] );
		HHuginn::value_t const& rv( r[i] );
		equal = ( lv->type_id() == rv->type_id() ) && lv->operator_equals( thread_, lv, rv, position_ );
	}
	return ( thread_->runtime().boolean_value( equal ) );
	M_EPILOG
}

class HDequeClass : public HClass {
public:
	typedef HDequeClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _reversedDequeClass;
public:
	HDequeClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::DEQUE ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::DEQUE ) ),
			"The `deque` is a collection type that is used to represent and operate on deque of values. "
			"It supports basic subscript and range operators. "
			"It also supports efficient operations of addition and removal of its elements at its both ends.",
			&builtin::deque
		)
		, _reversedDequeClass( HReversedDeque::get_class( runtime_, this ) ) {
		HHuginn::field_definitions_t fd{
			{ "push",       objectFactory_->create_method( &deque::push ),       "( *elem* ) - add new *elem* at the (right/back) end of the `deque`, `deque` grows in size by 1" },
			{ "pop",        objectFactory_->create_method( &deque::pop ),        "remove last element from the deque, deque shrinks by 1" },
			{ "push_front", objectFactory_->create_method( &deque::push_front ), "( *elem* ) - add new *elem* at the (left/front) beginning of the `deque`, `deque` grows in size by 1" },
			{ "pop_front",  objectFactory_->create_method( &deque::pop_front ),  "remove first element from the `deque`, `deque` shrinks by 1" },
			{ "add",        objectFactory_->create_method( &deque::append ),     "( *other* ) - append all elements from *other* collection at the end of  `deque`" },
			{ "append",     objectFactory_->create_method( &deque::append ),     "( *other* ) - append all elements from *other* collection at the end of  `deque`" },
			{ "prepend",    objectFactory_->create_method( &deque::prepend ),    "( *other* ) - prepend all elements from *other* collection in front of  `deque`" },
			{ "insert",     objectFactory_->create_method( &deque::insert ),     "( *index*, *elem* ) - insert given *elem*ent at given *index*" },
			{ "clear",      objectFactory_->create_method( &deque::clear ),      "erase `deque`'s content, `deque` becomes empty" },
			{ "find",       objectFactory_->create_method( &deque::find ),       "( *elem*[, *start*[, *stop*]] ) - get index of first *elem*ent of the `deque` not before *start* and before *stop*, return -1 if not found" },
			{ "hash",       objectFactory_->create_method( &deque::hash ),       "calculate hash value for  `deque`" },
			{ "less",       objectFactory_->create_method( &deque::less ),       "( *other* ) - test if  `deque` comes lexicographically before *other* `deque`" },
			{ "less_or_equal",    objectFactory_->create_method( &deque::less_or_equal ),    "( *other* ) - test if this `deque` is equal to or comes lexicographically before *other* `deque`" },
			{ "greater",          objectFactory_->create_method( &deque::greater ),          "( *other* ) - test if this `deque` comes lexicographically after *other* `deque`" },
			{ "greater_or_equal", objectFactory_->create_method( &deque::greater_or_equal ), "( *other* ) - test if this `deque` is equal to or comes lexicographically after *other* `deque`" },
			{ "equals",     objectFactory_->create_method( &deque::equals ),     "( *other* ) - test if *other* `deque` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HClass const* reversed_deque_class( void ) const {
		return ( _reversedDequeClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _reversedDequeClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HDequeClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::DEQUE );
	HDequeClass const* dc( static_cast<HDequeClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedDeque>( dc->reversed_deque_class(), value_ ) );
	M_EPILOG
}

}

huginn::HDeque::HDeque( HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

void huginn::HDeque::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

void huginn::HDeque::pop_back( void ) {
	M_PROLOG
	_data.pop_back();
	return;
	M_EPILOG
}

void huginn::HDeque::push_front( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_front( value_ );
	return;
	M_EPILOG
}

void huginn::HDeque::pop_front( void ) {
	M_PROLOG
	_data.pop_front();
	return;
	M_EPILOG
}

int long huginn::HDeque::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void huginn::HDeque::clear( void ) {
	M_PROLOG
	_data.clear();
	return;
	M_EPILOG
}

int long huginn::HDeque::find( huginn::HThread* thread_, int position_, HHuginn::value_t const& val_, int long start_, int long stop_ ) const {
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

HHuginn::value_t huginn::HDeque::get( int long long index_ ) const {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

huginn::HIterable::iterator_t huginn::HDeque::do_iterator( huginn::HThread*, int ) {
	return ( hcore::make_pointer<deque::HDequeIterator>( this ) );
}

HHuginn::value_t huginn::HDeque::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	values_t data;
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_deque( yaal::move( data ) ) );
}

bool HDeque::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( find( thread_, position_, other_ ) != huginn::HDeque::npos );
}

HHuginn::value_t HDeque::do_operator_subscript( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& index_, int position_ ) const {
	int long long index( extract_index( thread_, index_, position_ ) );
	M_ASSERT( ( index >= 0 ) && ( index < _data.get_size() ) );
	return ( _data[static_cast<int>( index )] );
}

void HDeque::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& index_, HHuginn::value_t&& value_, int position_ ) {
	int long long index( extract_index( thread_, index_, position_ ) );
	M_ASSERT( ( index >= 0 ) && ( index < _data.get_size() ) );
	_data[static_cast<int>( index )] = yaal::move( value_ );
}

HHuginn::value_t HDeque::do_operator_range( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& from_, HHuginn::value_t const& to_, HHuginn::value_t const& step_, int position_ ) const {
	HIterable::ORange range( extract_range( thread_, from_, to_, step_, position_ ) );
	HDeque::values_t v;
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
	return ( thread_->object_factory().create_deque( yaal::move( v ) ) );
}

}

}

}


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
	HHuginn::HDeque* _deque;
	int _index;
public:
	HDequeIterator( HHuginn::HDeque* deque_ )
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
	HHuginn::HDeque* _deque;
	int long _index;
public:
	HDequeReverseIterator( HThread* thread_, HHuginn::HDeque* deque_, int position_ )
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

class HReversedDeque : public HHuginn::HIterable {
	HHuginn::value_t _deque;
public:
	HReversedDeque( HHuginn::HClass const* class_, HHuginn::value_t const& deque_ )
		: HIterable( class_ )
		, _deque( deque_ ) {
		M_ASSERT( _deque->type_id() == HHuginn::TYPE::DEQUE );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
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
		return ( safe_int::cast<int long>( static_cast<HHuginn::HDeque const*>( _deque.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread* thread_, int position_ ) override {
		return ( make_pointer<HDequeReverseIterator>( thread_, static_cast<HHuginn::HDeque*>( _deque.raw() ), position_ ) );
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
	static_cast<HHuginn::HDeque*>( object_->raw() )->push_back( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->pop_back();
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
	HHuginn::HDeque::values_t& dst( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			HHuginn::HTuple::values_t const& src( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			HHuginn::HList::values_t const& src( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			HHuginn::HDeque::values_t const& src( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			HHuginn::HOrder::values_t const& src( static_cast<HHuginn::HOrder const*>( values_[0].raw() )->value() );
			dst.insert( dst.end(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			HHuginn::HSet::values_t const& src( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
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
	HHuginn::HDeque::values_t& dst( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			HHuginn::HTuple::values_t const& src( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			HHuginn::HList::values_t const& src( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			HHuginn::HDeque::values_t const& src( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			HHuginn::HOrder::values_t const& src( static_cast<HHuginn::HOrder const*>( values_[0].raw() )->value() );
			dst.insert( dst.begin(), src.begin(), src.end() );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			HHuginn::HSet::values_t const& src( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
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
	HHuginn::HDeque::values_t& dst( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	HHuginn::HInteger::value_type pos( get_integer( values_[0] ) );
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
	static_cast<HHuginn::HDeque*>( object_->raw() )->push_front( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop_front( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop_front", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->pop_front();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->clear();
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
	HHuginn::HDeque& l( *static_cast<HHuginn::HDeque*>( object_->raw() ) );
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
	HHuginn::HDeque::values_t const& values( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::DEQUE ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += instruction::hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline HHuginn::value_t less( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.less", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	HHuginn::HDeque::values_t const& l( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	HHuginn::HDeque::values_t const& r( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
	HHuginn::HValueCompareHelper lessHelper( &instruction::less );
	lessHelper.anchor( thread_, position_ );
	bool res( lexicographical_compare( l.begin(), l.end(), r.begin(), r.end(), cref( lessHelper ) ) );
	return ( thread_->runtime().boolean_value( res ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.equals", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	HHuginn::HDeque::values_t const& l( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	HHuginn::HDeque::values_t const& r( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		equal = instruction::equals( thread_, l[i], r[i], position_ );
	}
	return ( thread_->runtime().boolean_value( equal ) );
	M_EPILOG
}

class HDequeClass : public HHuginn::HClass {
public:
	typedef HDequeClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _reversedDequeClass;
public:
	HDequeClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HHuginn::HClass(
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
			{ "equals",     objectFactory_->create_method( &deque::equals ),     "( *other* ) - test if *other* `deque` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HHuginn::HClass const* reversed_deque_class( void ) const {
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

}

HHuginn::HDeque::HDeque( HHuginn::HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

void HHuginn::HDeque::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

void HHuginn::HDeque::pop_back( void ) {
	M_PROLOG
	_data.pop_back();
	return;
	M_EPILOG
}

void HHuginn::HDeque::push_front( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_front( value_ );
	return;
	M_EPILOG
}

void HHuginn::HDeque::pop_front( void ) {
	M_PROLOG
	_data.pop_front();
	return;
	M_EPILOG
}

int long HHuginn::HDeque::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHuginn::HDeque::clear( void ) {
	M_PROLOG
	_data.clear();
	return;
	M_EPILOG
}

int long HHuginn::HDeque::find( huginn::HThread* thread_, int position_, HHuginn::value_t const& val_, int long start_, int long stop_ ) const {
	M_PROLOG
	if ( stop_ < 0 ) {
		stop_ = _data.get_size();
	}
	values_t::const_iterator it(
		yaal::find_if(
			_data.cbegin() + start_,
			_data.cbegin() + stop_,
			[thread_, &val_, position_]( HHuginn::value_t const& elem_ ) {
				return ( instruction::equals( thread_, val_, elem_, position_ ) );
			}
		)
	);
	int long pos( distance( _data.cbegin(), it ) );
	return ( pos != stop_ ? pos : npos );
	M_EPILOG
}

HHuginn::value_t HHuginn::HDeque::get( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::value_t& HHuginn::HDeque::get_ref( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::HIterable::iterator_t HHuginn::HDeque::do_iterator( huginn::HThread*, int ) {
	return ( hcore::make_pointer<deque::HDequeIterator>( this ) );
}

HHuginn::value_t HHuginn::HDeque::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	values_t data;
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_deque( yaal::move( data ) ) );
}

}

}


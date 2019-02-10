/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "list.hxx"
#include "helper.hxx"
#include "iterator.hxx"
#include "instruction.hxx"
#include "builtin.hxx"
#include "runtime.hxx"
#include "objectfactory.hxx"
#include "tools/xmath.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace list {

class HListIterator : public HIteratorInterface {
	HHuginn::HList* _list;
	int long _index;
public:
	HListIterator( HHuginn::HList* list_ )
		: _list( list_ )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( huginn::HThread* thread_, int position_ ) override {
		return ( _index < _list->size( thread_, position_ ) );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HListIterator( HListIterator const& ) = delete;
	HListIterator& operator = ( HListIterator const& ) = delete;
};

class HListReverseIterator : public HIteratorInterface {
	HHuginn::HList* _list;
	int long _index;
public:
	HListReverseIterator( HThread* thread_, HHuginn::HList* list_, int position_ )
		: _list( list_ )
		, _index( list_->size( thread_, position_ ) - 1 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _index >= 0 );
	}
	virtual void do_next( HThread*, int ) override {
		-- _index;
	}
private:
	HListReverseIterator( HListReverseIterator const& ) = delete;
	HListReverseIterator& operator = ( HListReverseIterator const& ) = delete;
};

class HReversedList : public HHuginn::HIterable {
	HHuginn::value_t _list;
public:
	HReversedList( HHuginn::HClass const* class_, HHuginn::value_t const& list_ )
		: HIterable( class_ )
		, _list( list_ ) {
		M_ASSERT( _list->type_id() == HHuginn::TYPE::LIST );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				"ReversedListView",
				"The `ReversedListView` class represents *lazy* *iterable* reversed view of a `list`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HList const*>( _list.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread* thread_, int position_ ) override {
		return ( make_pointer<HListReverseIterator>( thread_, static_cast<HHuginn::HList*>( _list.raw() ), position_ ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedList>( HIterable::get_class(), _list ) );
	}
};

inline HHuginn::value_t push( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.push", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	static_cast<HHuginn::HList*>( object_->raw() )->push_back( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t append( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "list.append";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t(
		verify_arg_type(
			name, values_, 0, {
				HHuginn::TYPE::TUPLE, HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::ORDER, HHuginn::TYPE::SET
			}, ARITY::UNARY, thread_, position_
		)
	);
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	HHuginn::HList::values_t& dst( static_cast<HHuginn::HList*>( object_->raw() )->value() );
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

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "list.insert", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::UNKNOWN }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	HHuginn::HList::values_t& dst( static_cast<HHuginn::HList*>( object_->raw() )->value() );
	HHuginn::HInteger::value_type pos( get_integer( values_[0] ) );
	if ( ( pos < 0 ) || ( pos > dst.get_size() ) ) {
		throw HHuginn::HHuginnRuntimeException( "invalid insertion position: "_ys.append( pos ), thread_->current_frame()->file_id(), position_ );
	}
	dst.insert( dst.begin() + static_cast<int long>( pos ), values_[1] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t resize( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "list.resize", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::UNKNOWN }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	HHuginn::HList::values_t& dst( static_cast<HHuginn::HList*>( object_->raw() )->value() );
	HHuginn::HInteger::value_type size( get_integer( values_[0] ) );
	if ( size < 0 ) {
		throw HHuginn::HHuginnRuntimeException( "invalid new size: "_ys.append( size ), thread_->current_frame()->file_id(), position_ );
	}
	int long oldSize( dst.get_size() );
	dst.resize( static_cast<int long>( size ) );
	HHuginn::value_t& templ( values_[1] );
	for ( int long i( oldSize ); i < size; ++ i ) {
		dst[i] = templ->clone( thread_, &templ, position_ );
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.pop", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	static_cast<HHuginn::HList*>( object_->raw() )->pop_back();
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t sort( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "list.sort";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	HHuginn::value_t key;
	if ( values_.get_size() > 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::FUNCTION_REFERENCE, ARITY::MULTIPLE, thread_, position_ );
		key = values_[0];
	}
	HHuginn::HList::values_t& data( static_cast<HHuginn::HList*>( object_->raw() )->value() );
	if ( ! key ) {
		HHuginn::HValueCompareHelper less( &instruction::checked_less );
		less.anchor( thread_, position_ );
		sort( data.begin(), data.end(), cref( less ) );
	} else {
		HHuginn::function_t k( static_cast<HHuginn::HFunctionReference*>( key.raw() )->function() );
		yaal::sort(
			data.begin(), data.end(),
			[&k, &thread_, &position_]( HHuginn::value_t const& l_, HHuginn::value_t const& r_ ) {
				return (
					instruction::checked_less(
						thread_,
						k( thread_, nullptr, HArguments( thread_, l_ ), position_ ),
						k( thread_, nullptr, HArguments( thread_, r_ ), position_ ),
						position_
					)
				);
			}
		);
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	static_cast<HHuginn::HList*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t find( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	char const name[] = "list.find";
	verify_arg_count( name, values_, 1, 3, thread_, position_ );
	int noArg( static_cast<int>( values_.get_size() ) );
	int long start( 0 );
	int long stop( -1 );
	HHuginn::HList& l( *static_cast<HHuginn::HList*>( object_->raw() ) );
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
	verify_arg_count( "list.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	HHuginn::HList::values_t const& values( static_cast<HHuginn::HList*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::LIST ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += instruction::hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline bool less_impl( huginn::HThread* thread_, HHuginn::value_t const& l_, HHuginn::value_t const& r_, int position_ ) {
	HHuginn::HList::values_t const& l( static_cast<HHuginn::HList const*>( l_.raw() )->value() );
	HHuginn::HList::values_t const& r( static_cast<HHuginn::HList const*>( r_.raw() )->value() );
	HHuginn::HValueCompareHelper lessHelper( &instruction::less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( l.begin(), l.end(), r.begin(), r.end(), cref( lessHelper ) ) );
}

inline HHuginn::value_t less( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	verify_signature( "list.less", values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t less_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	verify_signature( "list.less_or_equal", values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	verify_signature( "list.greater", values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	return ( thread_->runtime().boolean_value( less_impl( thread_, values_[0], *object_, position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t greater_or_equal( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	verify_signature( "list.greater_or_equal", values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	return ( thread_->runtime().boolean_value( ! less_impl( thread_, *object_, values_[0], position_ ) ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	verify_signature( "list.equals", values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	HHuginn::HList::values_t const& l( static_cast<HHuginn::HList*>( object_->raw() )->value() );
	HHuginn::HList::values_t const& r( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		equal = instruction::equals( thread_, l[i], r[i], position_ );
	}
	return ( thread_->runtime().boolean_value( equal ) );
	M_EPILOG
}

class HListClass : public HHuginn::HClass {
public:
	typedef HListClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _reversedListClass;
public:
	HListClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HHuginn::HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::LIST ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::LIST ) ),
			"The `list` is a collection type that is used to represent and operate on `list` of values. "
			"It supports basic subscript and range operators. "
			"It also supports efficient operations of addition and removal of its elements from its (right) end.",
			&builtin::list
		)
		, _reversedListClass( HReversedList::get_class( runtime_, this ) ) {
		HHuginn::field_definitions_t fd{
			{ "push",   objectFactory_->create_method( &list::push ),   "( *elem* ) - add new *elem* at the end of the `list`, `list` grows in size by 1" },
			{ "pop",    objectFactory_->create_method( &list::pop ),    "remove last element from the `list`, `list` shrinks by 1" },
			{ "add",    objectFactory_->create_method( &list::append ), "( *other* ) - append all elements from *other* collection at the end of this `list`" },
			{ "append", objectFactory_->create_method( &list::append ), "( *other* ) - append all elements from *other* collection at the end of this `list`" },
			{ "insert", objectFactory_->create_method( &list::insert ), "( *index*, *elem* ) - insert given *elem*ent at given *index*" },
			{ "resize", objectFactory_->create_method( &list::resize ), "( *size*, *elem* ) - resize `list` to given *size* optionally filling new elements with **copies** of value *elem*" },
			{ "clear",  objectFactory_->create_method( &list::clear ),  "erase `list`'s content, `list` becomes empty" },
			{ "find",   objectFactory_->create_method( &list::find ),   "( *elem*[, *start*[, *stop*]] ) - get index of first *elem*ent of the `list` not before *start* and before *stop*, return -1 if not found" },
			{ "sort",   objectFactory_->create_method( &list::sort ),   "( [*callable*] ) - in-place sort this `list`, using *callable* to retrieve keys for element comparison" },
			{ "hash",   objectFactory_->create_method( &list::hash ),   "calculate hash value for this `list`" },
			{ "less",   objectFactory_->create_method( &list::less ),   "( *other* ) - test if this `list` comes lexicographically before *other* `list`" },
			{ "less_or_equal",    objectFactory_->create_method( &list::less_or_equal ),    "( *other* ) - test if this `list` is equal to or comes lexicographically before *other* `list`" },
			{ "greater",          objectFactory_->create_method( &list::greater ),          "( *other* ) - test if this `list` comes lexicographically after *other* `list`" },
			{ "greater_or_equal", objectFactory_->create_method( &list::greater_or_equal ), "( *other* ) - test if this `list` is equal to or comes lexicographically after *other* `list`" },
			{ "equals", objectFactory_->create_method( &list::equals ), "( *other* ) - test if *other* `list` has the same content" }
		};
		redefine( nullptr, fd );
		return;
	}
	HHuginn::HClass const* reversed_list_class( void ) const {
		return ( _reversedListClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _reversedListClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	return ( make_pointer<HListClass>( runtime_, objectFactory_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::LIST );
	HListClass const* lc( static_cast<HListClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedList>( lc->reversed_list_class(), value_ ) );
	M_EPILOG
}

}

}

HHuginn::HList::HList( HHuginn::HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

void HHuginn::HList::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

void HHuginn::HList::pop_back( void ) {
	M_PROLOG
	_data.pop_back();
	return;
	M_EPILOG
}

int long HHuginn::HList::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHuginn::HList::clear( void ) {
	M_PROLOG
	_data.clear();
	return;
	M_EPILOG
}

int long HHuginn::HList::find( huginn::HThread* thread_, int position_, HHuginn::value_t const& val_, int long start_, int long stop_ ) const {
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

HHuginn::value_t HHuginn::HList::get( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::value_t& HHuginn::HList::get_ref( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::HIterable::iterator_t HHuginn::HList::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<huginn::list::HListIterator>( this ) );
}

HHuginn::value_t HHuginn::HList::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	values_t data;
	data.reserve( _data.get_size() );
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_list( yaal::move( data ) ) );
}

}

}


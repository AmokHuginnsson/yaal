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
	huginn::HList* _list;
	int long _index;
public:
	HListIterator( huginn::HList* list_ )
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
	huginn::HList* _list;
	int long _index;
public:
	HListReverseIterator( HThread* thread_, huginn::HList* list_, int position_ )
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

class HReversedList : public huginn::HIterable {
	HHuginn::value_t _list;
public:
	HReversedList( HClass const* class_, HHuginn::value_t const& list_ )
		: HIterable( class_ )
		, _list( list_ ) {
		M_ASSERT( _list->type_id() == HHuginn::TYPE::LIST );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
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
		return ( safe_int::cast<int long>( static_cast<huginn::HList const*>( _list.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread* thread_, int position_ ) override {
		return ( make_pointer<HListReverseIterator>( thread_, static_cast<huginn::HList*>( _list.raw() ), position_ ) );
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
	static_cast<huginn::HList*>( object_->raw() )->push_back( values_[0] );
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
	huginn::HList::values_t& dst( static_cast<huginn::HList*>( object_->raw() )->value() );
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

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "list.insert", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::UNKNOWN }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LIST );
	huginn::HList::values_t& dst( static_cast<huginn::HList*>( object_->raw() )->value() );
	huginn::HInteger::value_type pos( get_integer( values_[0] ) );
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
	huginn::HList::values_t& dst( static_cast<huginn::HList*>( object_->raw() )->value() );
	huginn::HInteger::value_type size( get_integer( values_[0] ) );
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
	static_cast<huginn::HList*>( object_->raw() )->pop_back();
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
	huginn::HList::values_t& data( static_cast<HList*>( object_->raw() )->value() );
	if ( ! key ) {
		HValueCompareHelper less( &instruction::checked_less );
		less.anchor( thread_, position_ );
		sort( data.begin(), data.end(), cref( less ) );
	} else {
		HHuginn::function_t k( static_cast<HFunctionReference*>( key.raw() )->function() );
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
	static_cast<huginn::HList*>( object_->raw() )->clear();
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
	huginn::HList& l( *static_cast<huginn::HList*>( object_->raw() ) );
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
	huginn::HList::values_t const& values( static_cast<huginn::HList*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::LIST ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

class HListClass : public HClass {
public:
	typedef HListClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _reversedListClass;
public:
	HListClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_
	) : HClass(
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
			{ "append", objectFactory_->create_method( &list::append ), "( *other* ) - append all elements from *other* collection at the end of this `list`" },
			{ "insert", objectFactory_->create_method( &list::insert ), "( *index*, *elem* ) - insert given *elem*ent at given *index*" },
			{ "resize", objectFactory_->create_method( &list::resize ), "( *size*, *elem* ) - resize `list` to given *size* optionally filling new elements with **copies** of value *elem*" },
			{ "clear",  objectFactory_->create_method( &list::clear ),  "erase `list`'s content, `list` becomes empty" },
			{ "find",   objectFactory_->create_method( &list::find ),   "( *elem*[, *start*[, *stop*]] ) - get index of first *elem*ent of the `list` not before *start* and before *stop*, return -1 if not found" },
			{ "sort",   objectFactory_->create_method( &list::sort ),   "( [*callable*] ) - in-place sort this `list`, using *callable* to retrieve keys for element comparison" }
		};
		redefine( nullptr, fd );
		return;
	}
	HClass const* reversed_list_class( void ) const {
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

huginn::HList::HList( HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

void huginn::HList::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

void huginn::HList::pop_back( void ) {
	M_PROLOG
	_data.pop_back();
	return;
	M_EPILOG
}

int long huginn::HList::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void huginn::HList::clear( void ) {
	M_PROLOG
	_data.clear();
	return;
	M_EPILOG
}

int long huginn::HList::find( huginn::HThread* thread_, int position_, HHuginn::value_t const& val_, int long start_, int long stop_ ) const {
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

HHuginn::value_t huginn::HList::get( int long long index_ ) const {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

huginn::HIterable::iterator_t huginn::HList::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<huginn::list::HListIterator>( this ) );
}

HHuginn::value_t huginn::HList::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	values_t data;
	data.reserve( _data.get_size() );
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_list( yaal::move( data ) ) );
}

bool HList::do_operator_less( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HList::values_t const& other( static_cast<huginn::HList const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( _data.begin(), _data.end(), other.begin(), other.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HList::do_operator_less_or_equal( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HList::values_t const& other( static_cast<huginn::HList const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( ! lexicographical_compare( other.begin(), other.end(), _data.begin(), _data.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HList::do_operator_greater( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HList::values_t const& other( static_cast<huginn::HList const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( lexicographical_compare( other.begin(), other.end(), _data.begin(), _data.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HList::do_operator_greater_or_equal( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HList::values_t const& other( static_cast<huginn::HList const*>( other_.raw() )->value() );
	HValueCompareHelper lessHelper( &instruction::checked_less );
	lessHelper.anchor( thread_, position_ );
	return ( ! lexicographical_compare( _data.begin(), _data.end(), other.begin(), other.end(), cref( lessHelper ) ) );
	M_EPILOG
}

bool HList::do_operator_equals( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HList::values_t const& other( static_cast<huginn::HList const*>( other_.raw() )->value() );
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

bool HList::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( find( thread_, position_, other_ ) != huginn::HList::npos );
}

void HList::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	M_PROLOG
	huginn::HList::values_t const& src( static_cast<huginn::HList const*>( other_.raw() )->value() );
	_data.insert( _data.end(), src.begin(), src.end() );
	return;
	M_EPILOG
}

HHuginn::value_t HList::do_operator_subscript( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& index_, int position_ ) const {
	int long long index( extract_index( thread_, index_, position_ ) );
	M_ASSERT( ( index >= 0 ) && ( index < _data.get_size() ) );
	return ( _data[static_cast<int>( index )] );
}

void HList::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& index_, HHuginn::value_t&& value_, int position_ ) {
	int long long index( extract_index( thread_, index_, position_ ) );
	M_ASSERT( ( index >= 0 ) && ( index < _data.get_size() ) );
	_data[static_cast<int>( index )] = yaal::move( value_ );
}

HHuginn::value_t HList::do_operator_range( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& from_, HHuginn::value_t const& to_, HHuginn::value_t const& step_, int position_ ) const {
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
	return ( thread_->object_factory().create_list( yaal::move( v ) ) );
}

int long HList::do_operator_hash( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	M_PROLOG
	int long hashValue( static_cast<int long>( HHuginn::TYPE::LIST ) );
	for ( HHuginn::value_t const& v : _data ) {
		hashValue *= 3;
		hashValue += v->operator_hash( thread_, v, position_ );
	}
	return ( hashValue );
	M_EPILOG
}

yaal::hcore::HString HList::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	hcore::HString str( "[" );
	bool next( false );
	for ( HHuginn::value_t const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->code( thread_, v, cycleTracker_, position_ ) );
	}
	str.append( "]" );
	return ( str );
}

yaal::hcore::HString HList::do_to_string( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	hcore::HString str( "[" );
	bool next( false );
	for ( HHuginn::value_t const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v->to_string( thread_, v, cycleTracker_, position_ ) );
	}
	str.append( "]" );
	return ( str );
}

}

}

}


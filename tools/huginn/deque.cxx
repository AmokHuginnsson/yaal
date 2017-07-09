/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  deque.cxx - this file is integral part of `yaal' project.

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
#include "helper.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

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

namespace deque {

inline HHuginn::value_t push( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.push", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->push_back( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->pop_back();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t append( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "deque.append";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t( verify_arg_type( name, values_, 0, { HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::ORDER, HHuginn::TYPE::SET }, ARITY::UNARY, thread_, position_ ) );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	HHuginn::HDeque::values_t& dst( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
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

inline HHuginn::value_t prepend( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "deque.prepend";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t( verify_arg_type( name, values_, 0, { HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::ORDER, HHuginn::TYPE::SET }, ARITY::UNARY, thread_, position_ ) );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	HHuginn::HDeque::values_t& dst( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	switch( t.get() ) {
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

inline HHuginn::value_t insert( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
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

inline HHuginn::value_t push_front( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.push_front", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->push_front( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop_front( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop_front", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->pop_front();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	static_cast<HHuginn::HDeque*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DEQUE );
	verify_signature( "deque.equals", values_, { HHuginn::TYPE::DEQUE }, thread_, position_ );
	HHuginn::HDeque::values_t const& l( static_cast<HHuginn::HDeque*>( object_->raw() )->value() );
	HHuginn::HDeque::values_t const& r( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		equal = value_builtin::equals( thread_, l[i], r[i], position_ );
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
			type_id( HHuginn::TYPE::DEQUE ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::DEQUE ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "push",       objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::push, _1, _2, _3, _4 ) ),       "( *elem* ) - add new *elem* at the (right/back) end of the `deque`, `deque` grows in size by 1" },
				{ "pop",        objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::pop, _1, _2, _3, _4 ) ),        "remove last element from the deque, deque shrinks by 1" },
				{ "push_front", objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::push_front, _1, _2, _3, _4 ) ), "( *elem* ) - add new *elem* at the (left/front) beginning of the `deque`, `deque` grows in size by 1" },
				{ "pop_front",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::pop_front, _1, _2, _3, _4 ) ),  "remove first element from the `deque`, `deque` shrinks by 1" },
				{ "add",        objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::append, _1, _2, _3, _4 ) ),     "( *other* ) - append all elements from *other* collection at the end of this `deque`" },
				{ "append",     objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::append, _1, _2, _3, _4 ) ),     "( *other* ) - append all elements from *other* collection at the end of this `deque`" },
				{ "prepend",    objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::prepend, _1, _2, _3, _4 ) ),    "( *other* ) - prepend all elements from *other* collection in front of this `deque`" },
				{ "insert",     objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::insert, _1, _2, _3, _4 ) ),     "( *index*, *elem* ) - insert given *elem*ent at given *index*" },
				{ "clear",      objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::clear, _1, _2, _3, _4 ) ),      "erase `deque`'s content, `deque` becomes empty" },
				{ "equals",     objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &deque::equals, _1, _2, _3, _4 ) ),     "( *other* ) - test if *other* `deque` has the same content" }
			},
			"The `deque` is a collection type that is used to represent and operate on list of values. "
			"It supports basic subscript and range operators. "
			"It also supports efficient operations of addition and removal of its elements at its both ends."
		)
	);
	return ( c );
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
	_data.clear();
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

HHuginn::HIterable::HIterator HHuginn::HDeque::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HDequeIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HDeque::do_clone( huginn::HThread* thread_, int position_ ) const {
	values_t data;
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_deque( yaal::move( data ) ) );
}

}

}


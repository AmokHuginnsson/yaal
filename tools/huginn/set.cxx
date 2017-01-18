/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  set.cxx - this file is integral part of `yaal' project.

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

class HSetIterator : public HIteratorInterface {
	HHuginn::HSet::values_t* _set;
	HHuginn::HSet::values_t::iterator _it;
public:
	HSetIterator( HHuginn::HSet::values_t* set_ )
		: _set( set_ ), _it( set_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _set->end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HSetIterator( HSetIterator const& ) = delete;
	HSetIterator& operator = ( HSetIterator const& ) = delete;
};

namespace set {

inline HHuginn::value_t add( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.add", values_, 1, 1, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	static_cast<HHuginn::HSet*>( object_->raw() )->insert( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.has_key", values_, 1, 1, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	bool hasKey( static_cast<HHuginn::HSet*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.erase", values_, 1, 1, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	static_cast<HHuginn::HSet*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.clear", values_, 0, 0, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	static_cast<HHuginn::HSet*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	verify_signature( "set.update", values_, { HHuginn::TYPE::SET }, position_ );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( object_->raw() ) );
	HHuginn::HSet::values_t& l( s->value() );
	HHuginn::HSet::values_t const& r( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
	s->anchor( thread_, position_ );
	for ( HHuginn::HSet::values_t::const_iterator it( r.begin() ), end( r.end() ); it != end; ++ it ) {
		l.insert( *it );
	}
	s->detach();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::SET );
	verify_signature( "set.equals", values_, { HHuginn::TYPE::SET }, position_ );
	HHuginn::HSet::values_t const& l( static_cast<HHuginn::HSet*>( object_->raw() )->value() );
	HHuginn::HSet::values_t const& r( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( HHuginn::HSet::values_t::const_iterator lit( l.begin() ), rit( r.begin() ), end( l.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		equal = value_builtin::equals( thread_, *lit, *rit, position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::SET ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::SET ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "add",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::add, _1, _2, _3, _4 ) ),     "( *elem* ) - add given element *elem* to a `set`" },
				{ "has_key", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::has_key, _1, _2, _3, _4 ) ), "( *elem* ) - tell if given element *elem* is present in the `set`" },
				{ "erase",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::erase, _1, _2, _3, _4 ) ),   "( *elem* ) - remove given element *elem* from the `set`" },
				{ "clear",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::clear, _1, _2, _3, _4 ) ),   "erase `set`'s content, `set` becomes empty" },
				{ "update",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::update, _1, _2, _3, _4 ) ),  "( *other* ) - update content of this `set` with values from *other* `set`" },
				{ "equals",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::equals, _1, _2, _3, _4 ) ),  "( *other* ) - test if *other* `set` has the same content" }
			},
			"The `set` is a collection of unique elements of varying types. It supports operation of element insertion, removal and search."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HSet::HSet( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _helper()
	, _data( _helper, _helper ) {
	return;
}

int long HHuginn::HSet::do_size( void ) const {
	return ( _data.get_size() );
}

bool HHuginn::HSet::has_key( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	_helper.anchor( thread_, position_ );
	bool has( _data.find( key_ ) != _data.end() );
	_helper.detach();
	return ( has );
	M_EPILOG
}

void HHuginn::HSet::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	_helper.anchor( thread_, position_ );
	_data.erase( key_ );
	_helper.detach();
	return;
	M_EPILOG
}

void HHuginn::HSet::insert( huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	_helper.anchor( thread_, position_ );
	_data.insert( value_ );
	_helper.detach();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HSet::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HSetIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HSet::do_clone( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	HHuginn::value_t res( thread_->runtime().object_factory()->create_set() );
	HSet* set( static_cast<HSet*>( res.raw() ) );
	values_t& data( set->value() );
	set->anchor( thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert( v->clone( thread_, position_ ) );
	}
	set->detach();
	return ( res );
	M_EPILOG
}

}

}


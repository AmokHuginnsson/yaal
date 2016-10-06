/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  list.cxx - this file is integral part of `yaal' project.

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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HListIterator : public HIteratorInterface {
	HHuginn::HList* _list;
	int _index;
public:
	HListIterator( HHuginn::HList* list_ )
		: _list( list_ ),
		_index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _list->size() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HListIterator( HListIterator const& ) = delete;
	HListIterator& operator = ( HListIterator const& ) = delete;
};

namespace list {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.add", values_, 1, 1, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	l->push_back( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.pop", values_, 0, 0, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	l->pop_back();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.clear", values_, 0, 0, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	l->clear();
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::LIST ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::LIST ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "add",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::add, _1, _2, _3, _4 ) ), "( *elem* ) - add new *elem* at the end of the list, list grows in size by 1" },
				{ "pop",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::pop, _1, _2, _3, _4 ) ), "remove last element from the list, list shrinks by 1" },
				{ "clear", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::clear, _1, _2, _3, _4 ) ), "erase list's content, list becomes empty" }
			},
			"The `list` is a collection type that is used to represent and operate on list of values. "
			"It supports basic subscript and range operators. "
			"It also supports efficient operations of addition and removal of its elements from its (right) end."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HList::HList( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _data() {
	return;
}

HHuginn::HList::HList( HHuginn::HClass const* class_, values_t const& data_ )
	: HIterable( class_ )
	, _data() {
	_data.reserve( data_.get_size() );
	HRuntime* runtime( class_->runtime() );
	for ( values_t::value_type const& v : data_ ) {
		_data.push_back( v->clone( runtime ) );
	}
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

int long HHuginn::HList::do_size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HList::clear( void ) {
	_data.clear();
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

HHuginn::HIterable::HIterator HHuginn::HList::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HListIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HList::do_clone( HRuntime* runtime_ ) const {
	return ( runtime_->object_factory()->create_list( _data ) );
}

}

}


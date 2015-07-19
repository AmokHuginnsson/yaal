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
#include "iterator.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HHuginn::type_t const HHuginn::TYPE::LIST( HHuginn::HType::register_type( "list", nullptr ) );

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
	virtual HHuginn::value_t do_value( void ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _list->size() );
	}
	virtual void do_next( void ) override {
		++ _index;
	}
private:
	HListIterator( HListIterator const& ) = delete;
	HListIterator& operator = ( HListIterator const& ) = delete;
};

namespace list {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.push", values_, 1, 1, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_ ) );
	M_ASSERT( l != nullptr );
	l->push_back( values_[0] );
	M_ASSERT( !! l->get_pointer() );
	return ( object_->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.pop", values_, 0, 0, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_ ) );
	M_ASSERT( l != nullptr );
	l->pop_back();
	M_ASSERT( !! l->get_pointer() );
	return ( object_->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "list.clear", values_, 0, 0, position_ );
	HHuginn::HList* l( dynamic_cast<HHuginn::HList*>( object_ ) );
	M_ASSERT( l != nullptr );
	l->clear();
	M_ASSERT( !! l->get_pointer() );
	return ( object_->get_pointer() );
	M_EPILOG
}

}

HHuginn::HClass _listClass_(
	nullptr,
	HHuginn::TYPE::LIST,
	nullptr,
	/* methods */ {
		"add",
		"pop",
		"clear"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::add, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::pop, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &list::clear, _1, _2, _3, _4 ) )
	}
);

}

HHuginn::HList::HList( void )
	: HIterable( &huginn::_listClass_ )
	, _data() {
	return;
}

HHuginn::HList::HList( values_t const& data_ )
	: HIterable( &huginn::_listClass_ )
	, _data( data_ ) {
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

int long HHuginn::HList::size( void ) const {
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

HHuginn::value_t HHuginn::HList::get_ref( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( make_pointer<HReference>( _data[static_cast<int>( index_ )] ) );
	M_EPILOG
}

HHuginn::HList::values_t const& HHuginn::HList::value( void ) const {
	return ( _data );
}

HHuginn::HList::values_t& HHuginn::HList::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HList::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HListIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HList::do_clone( void ) const {
	return ( make_pointer<HList>( _data ) );
}

}

}


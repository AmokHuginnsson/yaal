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
#include "iterator.hxx"
#include "helper.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HHuginn::type_t const HHuginn::TYPE::DEQUE( HHuginn::HType::register_type( "deque", nullptr ) );

namespace huginn {

class HDequeIterator : public HIteratorInterface {
	HHuginn::HDeque* _deque;
	int _index;
public:
	HDequeIterator( HHuginn::HDeque* deque_ )
		: _deque( deque_ ),
		_index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _deque->get( _index ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _deque->size() );
	}
	virtual void do_next( void ) override {
		++ _index;
	}
private:
	HDequeIterator( HDequeIterator const& ) = delete;
	HDequeIterator& operator = ( HDequeIterator const& ) = delete;
};

namespace deque {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.add", values_, 1, 1, position_ );
	HHuginn::HDeque* d( dynamic_cast<HHuginn::HDeque*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->push_back( values_[0] );
	M_ASSERT( !! d->get_pointer() );
	return ( d->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t pop( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop", values_, 0, 0, position_ );
	HHuginn::HDeque* d( dynamic_cast<HHuginn::HDeque*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->pop_back();
	M_ASSERT( !! d->get_pointer() );
	return ( d->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t add_front( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.add_front", values_, 1, 1, position_ );
	HHuginn::HDeque* d( dynamic_cast<HHuginn::HDeque*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->push_front( values_[0] );
	M_ASSERT( !! d->get_pointer() );
	return ( d->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t pop_front( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.pop_front", values_, 0, 0, position_ );
	HHuginn::HDeque* d( dynamic_cast<HHuginn::HDeque*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->pop_front();
	M_ASSERT( !! d->get_pointer() );
	return ( d->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "deque.clear", values_, 0, 0, position_ );
	HHuginn::HDeque* d( dynamic_cast<HHuginn::HDeque*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->clear();
	M_ASSERT( !! d->get_pointer() );
	return ( d->get_pointer() );
	M_EPILOG
}

HHuginn::class_t get_class( void ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			nullptr,
			HHuginn::TYPE::DEQUE,
			nullptr,
			HHuginn::HClass::field_names_t{
				"add",
				"pop",
				"add_front",
				"pop_front",
				"clear"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &deque::add, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &deque::pop, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &deque::add_front, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &deque::pop_front, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &deque::clear, _1, _2, _3, _4 ) )
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HDeque::HDeque( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _data() {
	return;
}

HHuginn::HDeque::HDeque( HHuginn::HClass const* class_, values_t const& data_ )
	: HIterable( class_ )
	, _data( data_ ) {
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

int long HHuginn::HDeque::do_size( void ) const {
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

HHuginn::value_t HHuginn::HDeque::get_ref( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( make_pointer<HReference>( _data[static_cast<int>( index_ )] ) );
	M_EPILOG
}

HHuginn::HDeque::values_t const& HHuginn::HDeque::value( void ) const {
	return ( _data );
}

HHuginn::HDeque::values_t& HHuginn::HDeque::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HDeque::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HDequeIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HDeque::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_deque( _data ) );
}

}

}


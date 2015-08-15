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
#include "iterator.hxx"
#include "compiler.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HHuginn::type_t const HHuginn::TYPE::SET( HHuginn::HType::register_type( "set", nullptr ) );

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
	virtual HHuginn::value_t do_value( void ) override {
		return ( *_it );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _set->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HSetIterator( HSetIterator const& ) = delete;
	HSetIterator& operator = ( HSetIterator const& ) = delete;
};

namespace set {

inline HHuginn::value_t add( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.add", values_, 1, 1, position_ );
	HHuginn::HSet* s( dynamic_cast<HHuginn::HSet*>( object_ ) );
	M_ASSERT( s != nullptr );
	s->insert( values_[0] );
	return ( object_->get_pointer() );
	M_EPILOG
}

inline HHuginn::value_t has_key( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.has_key", values_, 1, 1, position_ );
	HHuginn::HSet* s( dynamic_cast<HHuginn::HSet*>( object_ ) );
	M_ASSERT( s != nullptr );
	bool hasKey( s->has_key( values_[0] ) );
	return ( make_pointer<HHuginn::HBoolean>( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "set.erase", values_, 1, 1, position_ );
	HHuginn::HSet* s( dynamic_cast<HHuginn::HSet*>( object_ ) );
	M_ASSERT( s != nullptr );
	M_ASSERT( !! s->get_pointer() );
	s->erase( values_[0] );
	return ( s->get_pointer() );
	M_EPILOG
}

}

HHuginn::HClass _setClass_(
	nullptr,
	HHuginn::TYPE::SET,
	nullptr,
	/* methods */ {
		"add",
		"has_key",
		"erase"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::add, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::has_key, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &set::erase, _1, _2, _3, _4 ) )
	}
);

}

HHuginn::HSet::HSet( void )
	: HIterable( &huginn::_setClass_ )
	, _data( &value_builtin::hash, &value_builtin::key_equals ) {
	return;
}

HHuginn::HSet::HSet( values_t const& data_ )
	: HIterable( &huginn::_setClass_ )
	, _data( data_ ) {
	return;
}

int long HHuginn::HSet::do_size( void ) const {
	return ( _data.get_size() );
}

bool HHuginn::HSet::has_key( HHuginn::value_t const& key_ ) const {
	M_PROLOG
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HSet::erase( HHuginn::value_t const& key_ ) {
	M_PROLOG
	_data.erase( key_ );
	return;
	M_EPILOG
}

void HHuginn::HSet::insert( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.insert( value_ );
	return;
	M_EPILOG
}

HHuginn::HSet::values_t const& HHuginn::HSet::value( void ) const {
	return ( _data );
}

HHuginn::HSet::values_t& HHuginn::HSet::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HSet::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HSetIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HSet::do_clone( void ) const {
	return ( make_pointer<HSet>( _data ) );
}

}

}


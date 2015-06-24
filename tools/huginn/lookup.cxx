/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  lookup.cxx - this file is integral part of `yaal' project.

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

HHuginn::type_t const HHuginn::TYPE::LOOKUP( HHuginn::HType::register_type( "lookup", nullptr ) );

namespace huginn {

class HLookupIterator : public HIteratorInterface {
	HHuginn::HLookup::values_t* _lookup;
	HHuginn::HLookup::values_t::iterator _it;
public:
	HLookupIterator( HHuginn::HLookup::values_t* lookup_ )
		: _lookup( lookup_ ), _it( lookup_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _lookup->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HLookupIterator( HLookupIterator const& ) = delete;
	HLookupIterator& operator = ( HLookupIterator const& ) = delete;
};

namespace lookup {

inline HHuginn::value_t has_key( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.has_key", values_, 1, 1, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_ ) );
	M_ASSERT( l != nullptr );
	bool hasKey( l->has_key( values_[0] ) );
	return ( make_pointer<HHuginn::HBoolean>( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.get", values_, 1, 2, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_ ) );
	M_ASSERT( l != nullptr );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( l->try_get( values_[0], v ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = l->get( values_[0], position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.erase", values_, 1, 1, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_ ) );
	M_ASSERT( l != nullptr );
	M_ASSERT( !! l->get_pointer() );
	l->erase( values_[0] );
	return ( l->get_pointer() );
	M_EPILOG
}

}

HHuginn::HClass _lookupClass_(
	nullptr,
	HHuginn::TYPE::LOOKUP,
	nullptr,
	/* methods */ {
		"has_key",
		"get",
		"erase"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::has_key, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::get, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::erase, _1, _2, _3, _4 ) )
	}
);

}

HHuginn::HLookup::HLookup( void )
	: HIterable( &huginn::_lookupClass_ )
	, _data( &value_builtin::hash, &value_builtin::key_equals ) {
	return;
}

HHuginn::HLookup::HLookup( values_t const& data_ )
	: HIterable( &huginn::_lookupClass_ ),
	_data( data_ ) {
	return;
}

int long HHuginn::HLookup::size( void ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HHuginn::HLookup::get( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `lookup'.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HLookup::try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_ ) {
	M_PROLOG
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HHuginn::HLookup::has_key( HHuginn::value_t const& key_ ) const {
	M_PROLOG
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HLookup::erase( HHuginn::value_t const& key_ ) {
	M_PROLOG
	_data.erase( key_ );
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HLookup::get_ref( HHuginn::value_t const& key_ ) {
	M_PROLOG
	return ( make_pointer<HReference>( _data[key_] ) );
	M_EPILOG
}

void HHuginn::HLookup::insert( HHuginn::value_t const& key_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.insert( make_pair( key_, value_ ) );
	return;
	M_EPILOG
}

HHuginn::HLookup::values_t const& HHuginn::HLookup::value( void ) const {
	return ( _data );
}

HHuginn::HLookup::values_t& HHuginn::HLookup::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HLookup::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HLookupIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HLookup::do_clone( void ) const {
	return ( make_pointer<HLookup>( _data ) );
}

}

}


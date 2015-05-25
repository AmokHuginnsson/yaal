/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  map.cxx - this file is integral part of `yaal' project.

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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HMapIterator : public HIteratorInterface {
	HHuginn::HMap::values_t* _map;
	HHuginn::HMap::values_t::iterator _it;
public:
	HMapIterator( HHuginn::HMap::values_t* map_ )
		: _map( map_ ), _it( map_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _map->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HMapIterator( HMapIterator const& ) = delete;
	HMapIterator& operator = ( HMapIterator const& ) = delete;
};

namespace map {

inline HHuginn::value_t has_key( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			"map.has_key() expects exactly one parameter, got: "_ys
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::HMap* m( dynamic_cast<HHuginn::HMap*>( object_ ) );
	M_ASSERT( m != nullptr );
	bool hasKey( m->has_key( values_[0], position_ ) );
	return ( make_pointer<HHuginn::HBoolean>( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			"map.has_key() expects exactly one parameter, got: "_ys
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::HMap* m( dynamic_cast<HHuginn::HMap*>( object_ ) );
	M_ASSERT( m != nullptr );
	M_ASSERT( !! m->get_pointer() );
	m->erase( values_[0], position_ );
	return ( m->get_pointer() );
	M_EPILOG
}

}

HHuginn::HClass _mapClass_(
	nullptr,
	HHuginn::TYPE::MAP,
	nullptr,
	/* methods */ {
		"has_key",
		"erase"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &map::has_key, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &map::erase, _1, _2, _3, _4 ) )
	}
);

}

HHuginn::HMap::HMap( void )
	: HIterable( &huginn::_mapClass_ )
	, _data( &value_builtin::less_low )
	, _keyType( HHuginn::TYPE::NONE ) {
	return;
}

HHuginn::HMap::HMap( values_t const& data_, type_t keyType_ )
	: HIterable( &huginn::_mapClass_ ),
	_data( data_ ),
	_keyType( keyType_ ) {
	return;
}

int long HHuginn::HMap::size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HMap::verify_key_type( HHuginn::type_t type_, int position_ ) const {
	if ( ( _keyType != TYPE::NONE ) && ( type_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types.", position_ );
	}
	if ( ! OCompiler::is_comparable( type_ ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( type_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

HHuginn::value_t HHuginn::HMap::get( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in map.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HMap::has_key( HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HMap::erase( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	_data.erase( key_ );
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HMap::get_ref( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	_keyType = key_->type();
	return ( make_pointer<HReference>( _data[key_] ) );
	M_EPILOG
}

void HHuginn::HMap::insert( HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	_data.insert( make_pair( key_, value_ ) );
	_keyType = key_->type();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HMap::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HMapIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HMap::do_clone( void ) const {
	return ( make_pointer<HMap>( _data, _keyType ) );
}

}

}


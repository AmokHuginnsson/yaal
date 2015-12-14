/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  class.cxx - this file is integral part of `yaal' project.

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
#include "keyword.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HHuginn::HClass::HClass(
	HHuginn* huginn_,
	type_t type_,
	HClass const* super_,
	field_names_t const& fieldNames_,
	values_t const& fieldDefinitions_
) : _type( type_ )
	, _super( super_ )
	, _fieldNames( fieldNames_ )
	, _fieldIndexes( super_ ? super_->_fieldIndexes : field_indexes_t() )
	, _fieldDefinitions( super_ ? super_->_fieldDefinitions : values_t() )
	, _huginn( huginn_ ) {
	M_PROLOG
	M_ASSERT( fieldNames_.get_size() == fieldDefinitions_.get_size() );
	int definitionIdx( 0 );
	for ( yaal::hcore::HString const& n : fieldNames_ ) {
		field_indexes_t::const_iterator fi( _fieldIndexes.insert( make_pair( n, static_cast<int>( _fieldIndexes.get_size() ) ) ).first );
		if ( fi->second >= _fieldDefinitions.get_size() ) {
			_fieldDefinitions.resize( fi->second + 1 );
		}
		_fieldDefinitions[fi->second] = fieldDefinitions_[definitionIdx];
		++ definitionIdx;
	}
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HClass::super( void ) const {
	return ( _super );
}

yaal::hcore::HString const& HHuginn::HClass::name( void ) const {
	return ( _type->name() );
}

HHuginn::type_t HHuginn::HClass::type( void ) const {
	return ( _type );
}

HHuginn::field_names_t const& HHuginn::HClass::field_names( void ) const {
	return ( _fieldNames );
}

int HHuginn::HClass::field_index( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	field_indexes_t::const_iterator it( _fieldIndexes.find( name_ ) );
	return ( it != _fieldIndexes.end() ? it->second : -1 );
	M_EPILOG
}

HHuginn::function_t const& HHuginn::HClass::function( int index_ ) const {
	M_PROLOG
	M_ASSERT( dynamic_cast<HMethod const*>( _fieldDefinitions[index_].raw() ) != nullptr );
	return ( static_cast<HMethod const*>( _fieldDefinitions[index_].raw() )->function() );
	M_EPILOG
}

HHuginn* HHuginn::HClass::huginn( void ) const {
	return ( _huginn );
}

HHuginn::value_t HHuginn::HClass::create_instance( huginn::HThread* thread_, value_t*, values_t const& values_, int position_ ) const {
	M_PROLOG
	return ( do_create_instance( thread_, values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::do_create_instance( huginn::HThread* thread_, values_t const& values_, int position_ ) const {
	M_PROLOG
	value_t v( make_pointer<HObject>( this ) );
	int constructorIdx( field_index( KEYWORD::CONSTRUCTOR ) );
	if ( constructorIdx >= 0 ) {
		function( constructorIdx )( thread_, &v, values_, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::values_t HHuginn::HClass::get_defaults( void ) const {
	M_PROLOG
	values_t defaults;
	for ( value_t const& v : _fieldDefinitions ) {
		defaults.push_back( v->clone( _huginn ) );
	}
	return ( defaults );
	M_EPILOG
}

bool HHuginn::HClass::is_kind_of( yaal::hcore::HString const& typeName_ ) const {
	M_PROLOG
	return ( ( typeName_ == _type->name() ) || ( _super ? _super->is_kind_of( typeName_ ) : false ) );
	M_EPILOG
}

HHuginn::HObject::HObject( HClass const* class_ )
	: HValue( class_->type() )
	, _class( class_ )
	, _fields( class_->get_defaults() ) {
}

HHuginn::HObject::HObject( HClass const* class_, fields_t const& fields_ )
	: HValue( class_->type() )
	, _class( class_ )
	, _fields( fields_ ) {
}

}

}


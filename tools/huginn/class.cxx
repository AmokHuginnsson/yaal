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
#include "runtime.hxx"
#include "keyword.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "compiler.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HHuginn::HClass::HClass(
	HRuntime* runtime_,
	type_id_t typeId_,
	identifier_id_t identifierId_,
	HClass const* super_,
	field_definitions_t const& fieldDefinitions_,
	yaal::hcore::HString const& doc_
) : _typeId( typeId_ )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _fieldIdentifiers()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _runtime( runtime_ ) {
	M_PROLOG
	redefine( super_, fieldDefinitions_ );
	return;
	M_EPILOG
}

HHuginn::HClass::HClass(
	HHuginn::TYPE typeTag_,
	HHuginn::identifier_id_t identifierId_,
	yaal::hcore::HString const& doc_
) : _typeId( huginn::type_id( typeTag_ ) )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _fieldIdentifiers()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _runtime( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HHuginn::HClass::redefine( HClass const* super_, field_definitions_t const& fieldDefinitions_ ) {
	M_PROLOG
	_super = super_;
	_fieldIdentifiers.clear();
	_fieldIndexes = ( _super ? _super->_fieldIndexes : field_indexes_t() );
	_fieldDefinitions = ( _super ? _super->_fieldDefinitions : values_t() );
	_fieldDescriptions = ( _super ? _super->_fieldDescriptions : field_descriptions_t() );
	for ( field_definitions_t::value_type const& fd : fieldDefinitions_ ) {
		identifier_id_t identifierId( _runtime->identifier_id( fd.name() ) );
		_fieldIdentifiers.emplace_back( identifierId );
		field_indexes_t::const_iterator fi(
			_fieldIndexes.insert( make_pair( identifierId, static_cast<int>( _fieldIndexes.get_size() ) ) ).first
		);
		if ( fi->second >= _fieldDefinitions.get_size() ) {
			_fieldDefinitions.resize( fi->second + 1 );
			_fieldDescriptions.resize( fi->second + 1 );
		}
		_fieldDefinitions[fi->second] = fd.value();
		_fieldDescriptions[fi->second] = fd.doc();
	}
	return;
	M_EPILOG
}

void HHuginn::HClass::update_runtime( huginn::HRuntime* runtime_ ) {
	_runtime = runtime_;
	return;
}

int HHuginn::HClass::field_index( identifier_id_t identifierId_ ) const {
	M_PROLOG
	field_indexes_t::const_iterator it( _fieldIndexes.find( identifierId_ ) );
	return ( it != _fieldIndexes.end() ? it->second : -1 );
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::HClass::name( void ) const {
	M_PROLOG
	return ( _runtime ? _runtime->identifier_name( _identifierId ) : type_name( _typeId ) );
	M_EPILOG
}

HHuginn::function_t const& HHuginn::HClass::function( int index_ ) const {
	M_PROLOG
	M_ASSERT( dynamic_cast<HMethod const*>( _fieldDefinitions[index_].raw() ) != nullptr );
	return ( static_cast<HMethod const*>( _fieldDefinitions[index_].raw() )->function() );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::create_instance( huginn::HThread* thread_, value_t*, values_t const& values_, int position_ ) const {
	M_PROLOG
	return ( do_create_instance( thread_, values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::do_create_instance( huginn::HThread* thread_, values_t const& values_, int position_ ) const {
	M_PROLOG
	value_t v( thread_->runtime().object_factory()->create_object( this ) );
	int constructorIdx( field_index( KEYWORD::CONSTRUCTOR_IDENTIFIER ) );
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
		defaults.push_back( v->clone( _runtime ) );
	}
	return ( defaults );
	M_EPILOG
}

bool HHuginn::HClass::is_kind_of( identifier_id_t identifierId_ ) const {
	M_PROLOG
	return ( ( identifierId_ == _identifierId ) || ( _super ? _super->is_kind_of( identifierId_ ) : false ) );
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::HClass::doc( void ) const {
	return ( _doc );
}

yaal::hcore::HString const& HHuginn::HClass::doc( identifier_id_t method_ ) const {
	M_PROLOG
	int fieldIndex( field_index( method_ ) );
	if ( fieldIndex < 0 ) {
		throw HHuginnException( "Invalid method identifier id." );
	}
	return ( _fieldDescriptions[ fieldIndex ] );
	M_EPILOG
}

}

}


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
	yaal::hcore::HString const& doc_,
	TYPE type_,
	HClass const* origin_,
	create_instance_t createInstance_
) : _typeId( typeId_ )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _createInstance(
		createInstance_
			? createInstance_
			: (
				super_ && ( super_->_type == TYPE::BUILTIN ) && ( type_ == TYPE::USER )
					? &HClass::create_instance_default
					: &HClass::create_instance_default
			)
	)
	, _fieldIdentifiers()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( type_ )
	, _origin( origin_ )
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
	, _createInstance( &HClass::create_instance_default )
	, _fieldIdentifiers()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( TYPE::BUILTIN )
	, _origin( nullptr )
	, _runtime( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
HHuginn::value_t HHuginn::HClass::base_class_not_initialized( huginn::HThread* thread_, value_t*, values_t&, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException(
		"Base class `"_ys
			.append( _super->name() )
			.append( "' is not initialized." )
		,
		thread_->current_frame()->file_id(),
		position_
	);
}
#pragma GCC diagnostic pop

bool HHuginn::HClass::has_builtin_base( void ) const {
	return ( _super && ( _super->_type == TYPE::BUILTIN ) && ( _type == TYPE::USER ) );
}

void HHuginn::HClass::redefine( HClass const* super_, field_definitions_t const& fieldDefinitions_ ) {
	M_PROLOG
	_super = super_;
	_fieldIdentifiers.clear();
	_fieldIndexes = ( _super ? _super->_fieldIndexes : field_indexes_t() );
	_fieldDefinitions = ( _super ? _super->_fieldDefinitions : values_t() );
	_fieldDescriptions = ( _super ? _super->_fieldDescriptions : field_descriptions_t() );
	if ( has_builtin_base() ) {
		for ( HHuginn::value_t& v : _fieldDefinitions ) {
			v = _runtime->object_factory()->create_method(
				hcore::call(
					&HHuginn::HClass::base_class_not_initialized,
					this, _1, _2, _3, _4
				)
			);
		}
	}
	for ( field_definitions_t::value_type const& fd : fieldDefinitions_ ) {
		add_member( fd );
	}
	return;
	M_EPILOG
}

void HHuginn::HClass::add_member( HHuginn::HFieldDefinition const& fd_ ) {
	M_PROLOG
	identifier_id_t identifierId( _runtime->identifier_id( fd_.name() ) );
	_fieldIdentifiers.emplace_back( identifierId );
	field_indexes_t::const_iterator fi(
		_fieldIndexes.insert( make_pair( identifierId, static_cast<int>( _fieldIndexes.get_size() ) ) ).first
	);
	if ( fi->second >= _fieldDefinitions.get_size() ) {
		_fieldDefinitions.resize( fi->second + 1 );
		_fieldDescriptions.resize( fi->second + 1 );
	}
	_fieldDefinitions[fi->second] = fd_.value();
	_fieldDescriptions[fi->second] = fd_.doc();
	return;
	M_EPILOG
}

void HHuginn::HClass::update_runtime( huginn::HRuntime* runtime_ ) {
	_runtime = runtime_;
	return;
}

void HHuginn::HClass::set_origin( HClass const* origin_ ) {
	M_ASSERT( ! _origin && origin_ );
	_origin = origin_;
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

void HHuginn::HClass::finalize_registration( huginn::HRuntime* runtime_ ) {
	M_PROLOG
	do_finalize_registration( runtime_ );
	return;
	M_EPILOG
}

void HHuginn::HClass::do_finalize_registration( huginn::HRuntime* ) {
}

HHuginn::value_t HHuginn::HClass::create_instance_default( HClass const* class_, huginn::HThread* thread_, values_t& values_, int position_ ) {
	return ( class_->do_create_instance( thread_, values_, position_ ) );
}

HHuginn::value_t HHuginn::HClass::create_instance( huginn::HThread* thread_, value_t*, values_t& values_, int position_ ) const {
	M_PROLOG
	return ( _createInstance( this, thread_, values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::access_violation( huginn::HThread* thread_, value_t*, values_t&, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException(
		"Explicit construction of class `"_ys.append( name() ).append( "' objects (instances) is forbidden." ),
		thread_->current_frame()->file_id(),
		position_
	);
	M_EPILOG
}

HHuginn::function_t HHuginn::HClass::constructor_function( HHuginn::ACCESS access_ ) const {
	M_PROLOG
	return (
		hcore::call(
			access_ == HHuginn::ACCESS::PUBLIC
				? &HHuginn::HClass::create_instance
				: &HHuginn::HClass::access_violation,
			this, _1, _2, _3, _4
		)
	);
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::constructor( HHuginn::ACCESS access_ ) const {
	M_PROLOG
	HHuginn::identifier_id_t identifier( identifier_id() );
	HHuginn::function_t func( constructor_function( access_ ) );
	HHuginn::value_t ctor(
		_runtime->object_factory()->create_function_reference(
			identifier,
			func,
			"automatic constructor for class: `"_ys.append( _runtime->identifier_name( identifier ) ).append( "`" )
		)
	);
	return ( ctor );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::do_create_instance( huginn::HThread* thread_, values_t& values_, int position_ ) const {
	M_PROLOG
	value_t v( thread_->runtime().object_factory()->create_object( this, get_defaults( thread_, position_ ) ) );
	int constructorIdx( field_index( KEYWORD::CONSTRUCTOR_IDENTIFIER ) );
	if ( constructorIdx >= 0 ) {
		function( constructorIdx )( thread_, &v, values_, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::values_t HHuginn::HClass::get_defaults( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	values_t defaults;
	for ( value_t const& v : _fieldDefinitions ) {
		defaults.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return ( defaults );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::get_default( huginn::HThread* thread_, int index_, int position_ ) const {
	M_PROLOG
	HHuginn::value_t const& v( _fieldDefinitions[index_] );
	return ( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
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


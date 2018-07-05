/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	yaal::hcore::HString const& doc_,
	HHuginn::ACCESS access_,
	TYPE type_,
	HClass const* origin_,
	create_instance_t createInstance_
)	: _runtime( runtime_ )
	, _typeId( typeId_ )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _createInstance( createInstance_ ? createInstance_ : &HClass::create_instance_default )
	, _fieldIdentifiers()
	, _staticFieldIndexes()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _constructor( make_constructor( runtime_->object_factory(), access_ ) )
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( type_ )
	, _origin( origin_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

/* ReverseTupleView, ReverseListView, ReverseDequeView and so on */
HHuginn::HClass::HClass(
	HRuntime* runtime_,
	char const* name_,
	char const* doc_,
	HClass const* origin_
) : _runtime( runtime_ )
	, _typeId( runtime_->new_type_id() )
	, _identifierId( runtime_->identifier_id( name_ ) )
	, _super( nullptr )
	, _createInstance( &HClass::create_instance_default )
	, _fieldIdentifiers()
	, _staticFieldIndexes()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _constructor()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( TYPE::BUILTIN )
	, _origin( origin_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

/* *none_type*, *function_reference*, *observer* and so on */
HHuginn::HClass::HClass(
	HRuntime* runtime_,
	HHuginn::TYPE typeTag_,
	HHuginn::identifier_id_t identifierId_,
	char const* doc_
) : _runtime( runtime_ )
	, _typeId( huginn::type_id( typeTag_ ) )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _createInstance( &HClass::create_instance_default )
	, _fieldIdentifiers()
	, _staticFieldIndexes()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _constructor()
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( TYPE::BUILTIN )
	, _origin( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

/* from HObjectFactory constructor */
HHuginn::HClass::HClass(
	HRuntime* runtime_,
	HObjectFactory* objectFactory_,
	type_id_t typeId_,
	identifier_id_t identifierId_,
	yaal::hcore::HString const& doc_,
	constructor_t constructor_
) : _runtime( runtime_ )
	, _typeId( typeId_ )
	, _identifierId( identifierId_ )
	, _super( nullptr )
	, _createInstance( &HClass::create_instance_default )
	, _fieldIdentifiers()
	, _staticFieldIndexes()
	, _fieldIndexes()
	, _fieldDefinitions()
	, _constructor( make_constructor( objectFactory_, constructor_ ) )
	, _fieldDescriptions()
	, _doc( doc_ )
	, _type( TYPE::BUILTIN )
	, _origin( nullptr ) {
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
			v = _runtime->create_method(
				&HHuginn::HClass::base_class_not_initialized,
				this
			);
		}
	}
	for ( field_definitions_t::value_type const& fd : fieldDefinitions_ ) {
		add_member( fd );
	}
	return;
	M_EPILOG
}

void HHuginn::HClass::add_member( HHuginn::HFieldDefinition const& fd_, MEMBER_TYPE memberType_ ) {
	M_PROLOG
	identifier_id_t identifierId( _runtime->identifier_id( fd_.name() ) );
	_fieldIdentifiers.emplace_back( identifierId );
	int fieldCount( static_cast<int>( _fieldIndexes.get_size() + _staticFieldIndexes.get_size() ) );
	if (
		( ( memberType_ == MEMBER_TYPE::INSTANCE ) && _staticFieldIndexes.count( identifierId ) )
		|| ( ( memberType_ == MEMBER_TYPE::STATIC ) && _fieldIndexes.count( identifierId ) )
	) {
		throw HHuginnException( "Static and Instance members conflict." );
	}
	field_indexes_t::const_iterator fi(
		memberType_ == MEMBER_TYPE::INSTANCE
			? _fieldIndexes.insert( make_pair( identifierId, fieldCount ) ).first
			: _staticFieldIndexes.insert( make_pair( identifierId, fieldCount ) ).first
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

HHuginn::ACCESS HHuginn::HClass::access( void ) const {
	M_PROLOG
	return (
		( ! _origin && ! is_meta_class( this ) )
			? HHuginn::ACCESS::PUBLIC
			: HHuginn::ACCESS::PRIVATE
	);
	M_EPILOG
}

int HHuginn::HClass::field_index( identifier_id_t const& identifierId_, MEMBER_TYPE memberType_ ) const {
	M_PROLOG
	int fieldIndex( -1 );
	field_indexes_t::const_iterator it( _fieldIndexes.find( identifierId_ ) );
	if ( it != _fieldIndexes.end() ) {
		fieldIndex = it->second;
	} else if (
		( memberType_ == MEMBER_TYPE::STATIC )
		&& ( (	it = _staticFieldIndexes.find( identifierId_ ) ) != _staticFieldIndexes.end() )
	) {
		fieldIndex = it->second;
	}
	return ( fieldIndex );
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
	if ( ! _constructor ) {
		_constructor = make_constructor( runtime_->object_factory(), HHuginn::ACCESS::PRIVATE );
	}
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

HHuginn::value_t HHuginn::HClass::make_constructor( HObjectFactory* objectFactory_, HHuginn::ACCESS access_ ) const {
	M_PROLOG
	HHuginn::function_t func(
		hcore::call(
			access_ == HHuginn::ACCESS::PUBLIC
				? &HHuginn::HClass::create_instance
				: &HHuginn::HClass::access_violation,
			this, _1, _2, _3, _4
		)
	);
	HHuginn::identifier_id_t identifier( identifier_id() );
	HHuginn::value_t ctor(
		objectFactory_->create_function_reference(
			identifier,
			func,
			"automatic constructor for class: `"_ys.append( _runtime->identifier_name( identifier ) ).append( "`" )
		)
	);
	return ( ctor );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::make_constructor( HObjectFactory* objectFactory_, constructor_t constructor_ ) const {
	M_PROLOG
	HHuginn::function_t func( hcore::call( constructor_, _1, _2, _3, _4 ) );
	HHuginn::identifier_id_t identifier( identifier_id() );
	HHuginn::value_t ctor(
		objectFactory_->create_function_reference(
			identifier,
			func,
			"construct an instance of "_ys.append( a_type_name( this ) ).append( " type" )
		)
	);
	return ( ctor );
	M_EPILOG
}

HHuginn::function_t HHuginn::HClass::constructor_function( void ) const {
	M_PROLOG
	return ( static_cast<HFunctionReference const*>( _constructor.raw() )->function() );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::do_create_instance( huginn::HThread* thread_, values_t& values_, int position_ ) const {
	M_PROLOG
	value_t v( thread_->runtime().object_factory()->create_object( this, get_defaults( thread_, position_ ) ) );
	int constructorIdx( field_index( KEYWORD::CONSTRUCTOR_IDENTIFIER ) );
	if ( constructorIdx >= 0 ) {
		function( constructorIdx )( thread_, &v, values_, position_ );
	} else {
		HHuginn::HObject& o( *static_cast<HHuginn::HObject*>( v.raw() ) );
		int vi( 0 );
		int const valueCount( static_cast<int>( values_.get_size() ) );
		int const fieldCount( static_cast<int>( _fieldDefinitions.get_size() ) );
		for ( int fi( 0 ); ( fi < fieldCount ) && ( vi < valueCount ); ++ fi ) {
			HHuginn::value_t& f( o.field_ref( fi ) );
			HHuginn::type_id_t t( f->type_id() );
			if ( t != HHuginn::TYPE::METHOD ) {
				f = values_[vi];
				++ vi;
			}
		}
		if ( vi < valueCount ) {
			throw HHuginn::HHuginnRuntimeException(
				"Too many arguments for class initializer, expected at most: "_ys.append( vi ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return ( v );
	M_EPILOG
}

HHuginn::values_t HHuginn::HClass::get_defaults( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	values_t defaults;
	for ( value_t const& v : _fieldDefinitions ) {
		defaults.push_back(
			( v->type_id() != HHuginn::TYPE::METHOD ) && ( ! is_enum_class( v ) )
				? v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ )
				: v
		);
	}
	return ( defaults );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::get_default( huginn::HThread* thread_, int index_, int position_ ) const {
	M_PROLOG
	HHuginn::value_t const& v( _fieldDefinitions[index_] );
	return (
		( v->type_id() != HHuginn::TYPE::METHOD ) && ( ! is_enum_class( v ) )
			? v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ )
			: v
	);
	M_EPILOG
}

bool HHuginn::HClass::is_kind_of( identifier_id_t const& identifierId_ ) const {
	M_PROLOG
	return ( ( identifierId_ == _identifierId ) || ( _super ? _super->is_kind_of( identifierId_ ) : false ) );
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::HClass::doc( void ) const {
	return ( _doc );
}

yaal::hcore::HString const& HHuginn::HClass::doc( identifier_id_t member_ ) const {
	M_PROLOG
	int fieldIndex( field_index( member_, MEMBER_TYPE::STATIC ) );
	if ( fieldIndex < 0 ) {
		throw HHuginnException( "Invalid member identifier id." );
	}
	return ( _fieldDescriptions[ fieldIndex ] );
	M_EPILOG
}

}

}


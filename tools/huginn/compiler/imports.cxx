/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/huginn/compiler.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

void OCompiler::build_import_name( yaal::hcore::HString const& name_, executing_parser::range_t ) {
	M_PROLOG
	if ( ! _moduleName.is_empty() ) {
		_moduleName.append( "." );
	}
	_moduleName.append( name_ );
	return;
	M_EPILOG
}

void OCompiler::set_import_name( executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_restricted( _moduleName ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( _moduleName ).append( "' is a restricted name." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t importIdentifier( _runtime->identifier_id( _moduleName ) );
	_moduleName.clear();
	check_name_import( importIdentifier, range_ );
	check_name_enum( importIdentifier, true, range_ );
	check_name_class( importIdentifier, false, range_ );
	check_name_function( importIdentifier, range_ );
	_importInfo._package = importIdentifier;
	_importInfo._position = range_.start();
	return;
	M_EPILOG
}

HHuginn::identifier_id_t OCompiler::prep_import_result( yaal::hcore::HString const& name_, HHuginn::SYMBOL_KIND kind_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t importResultIdentifier( _runtime->identifier_id( name_ ) );
	check_name_import( importResultIdentifier, range_ );
	check_name_enum( importResultIdentifier, true, range_ );
	check_name_class( importResultIdentifier, true, range_ );
	if ( ! _isIncremental || ( kind_ == HHuginn::SYMBOL_KIND::PACKAGE ) ) {
		check_name_function( importResultIdentifier, range_ );
	}
	_usedIdentifiers[importResultIdentifier].write( range_.start(), kind_ );
	return ( importResultIdentifier );
	M_EPILOG
}

void OCompiler::set_import_alias( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	_importInfo._alias = prep_import_result( name_, HHuginn::SYMBOL_KIND::PACKAGE, range_ );
	if ( _introspector ) {
		_introspector->symbol( { _runtime->identifier_name( _importInfo._package ), name_ }, HHuginn::SYMBOL_KIND::PACKAGE, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::add_imported_symbol( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( name_ != "*" ) {
		_importInfo._importedSymbols.push_back( prep_import_result( name_, HHuginn::SYMBOL_KIND::UNKNOWN, range_ ) );
		if ( _introspector ) {
			_introspector->symbol( { _runtime->identifier_name( _importInfo._package ), name_ }, HHuginn::SYMBOL_KIND::PACKAGE, _fileId, range_.start() );
		}
	}
	return;
	M_EPILOG
}

void OCompiler::commit_import( executing_parser::range_t ) {
	M_PROLOG
	_submittedImports.emplace_back( yaal::move( _importInfo ) );
	return;
	M_EPILOG
}

}

}

}


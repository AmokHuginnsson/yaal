/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hhuginn.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>
#include <cstring>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hlog.hxx"
#include "tools/stringalgo.hxx"
#include "tools/huginn/source.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/objectfactory.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/expression.hxx"
#include "tools/huginn/scope.hxx"
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/keyword.hxx"
#include "tools/huginn/value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

#if 0
/*
 * Sample code in huginn.
 */

sum( a, b ) {
	return ( a + b );
}

main( args ) {
	a = integer( args[0] );
	b = integer( args[1] );
	return ( sum( 3 + a, sum( 4, ( b + 1 ) * 2 ) ) );
}

#endif

namespace {
char const DEFAULT_PATHS[] = ".:" DATADIR "/huginn";
char const* const MODULE_PATHS_RAW( ::getenv( "HUGINNPATH" ) );
HString MODULE_PATHS_S( MODULE_PATHS_RAW ? hcore::to_string( MODULE_PATHS_RAW ).append( ":" ).append( DEFAULT_PATHS ) : DEFAULT_PATHS );

}
HHuginn::paths_t const HHuginn::MODULE_PATHS( string::split<HHuginn::paths_t>( MODULE_PATHS_S, ":", HTokenizer::SKIP_EMPTY ) );

static int const DEFAULT_MAX_CALL_STACK( 1024 );
int _huginnMaxCallStack_( DEFAULT_MAX_CALL_STACK );

namespace huginn {

HHuginn::identifier_id_t const TYPE_NONE_IDENTIFIER( 24 );
HHuginn::identifier_id_t const TYPE_OBSERVER_IDENTIFIER( 25 );
HHuginn::identifier_id_t const TYPE_REFERENCE_IDENTIFIER( 26 );
HHuginn::identifier_id_t const TYPE_FUNCTION_REFERENCE_IDENTIFIER( 27 );
HHuginn::identifier_id_t const TYPE_OBJECT_REFERENCE_IDENTIFIER( 28 );
HHuginn::identifier_id_t const TYPE_METHOD_IDENTIFIER( 29 );
HHuginn::identifier_id_t const TYPE_BOUND_METHOD_IDENTIFIER( 30 );
HHuginn::identifier_id_t const TYPE_VARIADIC_PARAMETERS_IDENTIFIER( 31 );
HHuginn::identifier_id_t const TYPE_NAMED_PARAMETERS_IDENTIFIER( 32 );
HHuginn::identifier_id_t const TYPE_UNKNOWN_IDENTIFIER( 33 );
HHuginn::HClass const _noneClass_( HHuginn::TYPE::NONE, TYPE_NONE_IDENTIFIER, "A type of `none` value." );
HHuginn::HClass const _observerClass_( HHuginn::TYPE::OBSERVER, TYPE_OBSERVER_IDENTIFIER, "The `*observer*` is a type representing a reference cycle breaking, non-owning weak \"pointer\" to a value." );
HHuginn::HClass const _referenceClass_( HHuginn::TYPE::REFERENCE, TYPE_REFERENCE_IDENTIFIER, "Write only reference. Allows assign operator to work." );
HHuginn::HClass const _functionReferenceClass_( HHuginn::TYPE::FUNCTION_REFERENCE, TYPE_FUNCTION_REFERENCE_IDENTIFIER, "The `*function_reference*` is a Huginn's way of providing information about a value's *runtime* type." );
HHuginn::HClass const _objectReferenceClass_( HHuginn::TYPE::OBJECT_REFERENCE, TYPE_OBJECT_REFERENCE_IDENTIFIER, "The `*object_reference*` is a up-casting reference allowing to access super class methods." );
HHuginn::HClass const _methodClass_( HHuginn::TYPE::METHOD, TYPE_METHOD_IDENTIFIER, "Unbound method." );
HHuginn::HClass const _boundMethodClass_( HHuginn::TYPE::BOUND_METHOD, TYPE_BOUND_METHOD_IDENTIFIER, "A reference to a callable method with a valid runtime value bound to it." );
HHuginn::HClass const _variadicParametersClass_( HHuginn::TYPE::VARIADIC_PARAMETERS, TYPE_VARIADIC_PARAMETERS_IDENTIFIER, "Variadic parameters pack." );
HHuginn::HClass const _namedParametersClass_( HHuginn::TYPE::NAMED_PARAMETERS, TYPE_NAMED_PARAMETERS_IDENTIFIER, "Named parameters pack." );
HHuginn::HClass const _unknownClass_( HHuginn::TYPE::UNKNOWN, TYPE_UNKNOWN_IDENTIFIER, "An erroneous unknown type." );

char const* _errMsgHHuginn_[ 10 ] = {
	_( "Operands are not summable: " ),
	_( "Operands are not subtractable: " ),
	_( "Operands are not multipliable: " ),
	_( "Operands are not divisible: " ),
	_( "Operands are not exponentiationable: " ),
	_( "Operands are not comparable: " ),
	_( "Operand is not a numeric value: " ),
	_( "Operands are not boolean values: " ),
	_( "Operand is not a boolean value: " ),
	_( "Subscript is not an integer: " )
};

}

HHuginn::compiler_setup_t const HHuginn::COMPILER::DEFAULT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_STRICT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_SLOPPY = HHuginn::compiler_setup_t::new_flag();

HHuginn::HHuginnRuntimeException::HHuginnRuntimeException( yaal::hcore::HString const& message_, int fileId_, int position_ )
	: _message( message_ )
	, _fileId( fileId_ )
	, _position( position_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HHuginnRuntimeException::message( void ) const {
	return ( _message );
}

int HHuginn::HHuginnRuntimeException::position( void ) const {
	return ( _position );
}

int HHuginn::HHuginnRuntimeException::file_id( void ) const {
	return ( _fileId );
}

HIntrospecteeInterface::HCallSite::HCallSite( yaal::hcore::HString const& file_, int line_, int column_, yaal::hcore::HString const& context_ )
	: _file( file_ )
	, _line( line_ )
	, _column( column_ )
	, _context( context_ ) {
}

yaal::hcore::HString const& HIntrospecteeInterface::HCallSite::file( void ) const {
	return ( _file );
}

int HIntrospecteeInterface::HCallSite::line( void ) const {
	return ( _line );
}

int HIntrospecteeInterface::HCallSite::column( void ) const {
	return ( _column );
}

yaal::hcore::HString const& HIntrospecteeInterface::HCallSite::context( void ) const {
	return ( _context );
}

HIntrospecteeInterface::HVariableView::HVariableView( yaal::hcore::HString const& name_, HHuginn::value_t const& view_ )
	: _name( name_ )
	, _view( view_ ) {
}

yaal::hcore::HString const& HIntrospecteeInterface::HVariableView::name( void ) const {
	return ( _name );
}

HHuginn::value_t HIntrospecteeInterface::HVariableView::value( void ) const {
	return ( _view );
}

HIntrospecteeInterface::call_stack_t HIntrospecteeInterface::get_call_stack( void ) {
	M_PROLOG
	return ( do_get_call_stack() );
	M_EPILOG
}

HIntrospecteeInterface::variable_views_t HIntrospecteeInterface::get_locals( int frameNo_ ) {
	M_PROLOG
	return ( do_get_locals( frameNo_ ) );
	M_EPILOG
}

void HIntrospectorInterface::introspect( yaal::tools::HIntrospecteeInterface& introspectee_ ) {
	M_PROLOG
	do_introspect( introspectee_ );
	return;
	M_EPILOG
}

HHuginn::HObjectReference::HObjectReference( value_t const& value_, int upCastLevel_, bool upCast_, int fileId_, int position_ )
	: HValue( &_objectReferenceClass_ )
	, _object( value_ )
	, _class( nullptr ) {
	M_PROLOG
	HObject const* o( dynamic_cast<HObject const*>( _object.raw() ) );
	M_ASSERT( o );
	HClass const* c( o->get_class() );
	for ( int i( 0 ); i < ( upCastLevel_ + ( upCast_ ? 1 : 0 ) ); ++ i ) {
		HClass const* s( c->super() );
		if ( ! s ) {
			throw HHuginnRuntimeException(
				"`"_ys.append( c->name() ).append( "' does not have superclass." ),
				fileId_,
				position_
			);
		}
		c = s;
	}
	_class = c;
	return;
	M_EPILOG
}

HHuginn::HObjectReference::HObjectReference( value_t const& value_, HClass const* class_ )
	: HValue( &_objectReferenceClass_ )
	, _object( value_ )
	, _class( class_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HHuginn::HObjectReference::field_index( identifier_id_t identifierId_ ) const {
	M_PROLOG
	HObject const* o( static_cast<HObject const*>( _object.raw() ) );
	return ( _class != o->get_class() ? _class->field_index( identifierId_ ) : o->field_index( identifierId_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObjectReference::field( huginn::HThread* thread_, int index_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( _object.raw() ) );
	value_t v;
	if ( o->get_class()->is_overridden( _class, index_ ) ) {
		v = _class->get_default( thread_, index_, position_ );
	} else {
		v = o->field_ref( index_ );
	}
	if ( v->type_id() == TYPE::METHOD ) {
		v = _class->runtime()->object_factory()->create_bound_method(
			static_cast<HClass::HMethod*>( v.raw() )->function(),
			_object
		);
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObjectReference::do_clone( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HObjectReference>( _object->clone( thread_, position_ ), _class ) );
	M_EPILOG
}

HHuginn::HTaggedValue::HTaggedValue( value_t const& value_, HClass const* class_ )
	: HValue( class_ )
	, _value( value_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HTaggedValue::do_clone( huginn::HThread*, int ) const {
	M_PROLOG
	M_ASSERT( 0 && "cloning tagged value"[0] );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( HHuginn::value_t() );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
	M_EPILOG
}

HHuginn::flag_t HHuginn::_grammarVerified{ false };

void HHuginn::disable_grammar_verification( void ) {
	_grammarVerified.store( true );
}

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY )
	, _runtime( make_resource<HRuntime>( this ) )
	, _sources()
	, _compiler( make_resource<OCompiler>( _runtime.raw() ) )
	, _engine( make_engine( _runtime.raw() ), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
	, _errorMessage()
	, _errorPosition( INVALID_POSITION )
	, _errorFileId( INVALID_FILE_ID )
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr )
	, _logStream()
	, _logStreamRaw( &hcore::log ) {
	M_PROLOG
	_sources.emplace_back( make_resource<HSource>() );
	_grammarVerified.store( true );
	_runtime->register_builtins();
	return;
	M_EPILOG
}

HHuginn::HHuginn( huginn::HRuntime* runtime_ )
	: _state( STATE::EMPTY )
	, _runtime( make_resource<HRuntime>( this ) )
	, _sources()
	, _compiler( make_resource<OCompiler>( _runtime.raw() ) )
	, _engine( make_engine( runtime_ ), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
	, _errorMessage()
	, _errorPosition( INVALID_POSITION )
	, _errorFileId( INVALID_FILE_ID )
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr )
	, _logStream()
	, _logStreamRaw( &hcore::log ) {
	M_PROLOG
	_sources.emplace_back( make_resource<HSource>() );
	_grammarVerified.store( true );
	_runtime->register_builtins();
	return;
	M_EPILOG
}

void HHuginn::reset( int undoSteps_ ) {
	M_PROLOG
	_errorPosition = INVALID_POSITION;
	_errorFileId = INVALID_FILE_ID;
	_errorMessage.clear();
	_compiler->reset( undoSteps_ );
	_compiler->_fileId = INVALID_FILE_ID;
	_sources[0] = make_resource<HSource>();
	_runtime->reset();
	_state = STATE::EMPTY;
	return;
	M_EPILOG
}

HHuginn::~HHuginn( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HHuginn::set_max_call_stack_size( int maxCallStackSize_ ) {
	_runtime->set_max_call_stack_size( maxCallStackSize_ );
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, yaal::hcore::HString const& name_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_sources.front()->load( stream_, name_, skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_sources.front()->load( stream_, hcore::HString(), skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::preprocess( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::LOADED, "Program source must be loaded before preprocessing." );
	_sources.front()->preprocess();
	_state = STATE::PREPROCESSED;
	return;
	M_EPILOG
}

bool HHuginn::parse( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PREPROCESSED, "Preprocessor step is required before parsing." );
	bool ok( _engine.parse( _sources.front()->begin(), _sources.front()->end() ) );
	if ( ! ok ) {
		_errorMessage = _engine.error_messages()[0];
		_errorPosition = _engine.error_position();
		_errorFileId = MAIN_FILE_ID;
	} else {
		_state = STATE::PARSED;
	}
	return ( ok );
	M_EPILOG
}

HHuginn::HClass const* HHuginn::commit_class( identifier_id_t identifierId_ ) {
	M_PROLOG
	class_t cls( _runtime->get_class( identifierId_ ) );
	if ( _compiler->_submittedClasses.count( identifierId_ ) > 0 ) {
		yaal::hcore::HString const& name( _runtime->identifier_name( identifierId_ ) );
		OCompiler::OClassContext* cc( _compiler->_submittedClasses.at( identifierId_ ).get() );
		M_ASSERT( cc->_classIdentifier == identifierId_ );
		for ( OCompiler::submitted_imports_t::value_type const& i : _compiler->_submittedImports ) {
			if ( identifierId_ == i._package ) {
				throw HHuginnRuntimeException( "Package of the same name `"_ys.append( name ).append( "' is already imported." ), MAIN_FILE_ID, cc->_position.get() );
			} else if ( identifierId_ == i._alias ) {
				throw HHuginnRuntimeException( "Package alias of the same name `"_ys.append( name ).append( "' is already defined." ), MAIN_FILE_ID, cc->_position.get() );
			}
		}
		if ( _runtime->get_function( identifierId_ ) && ! cls ) {
			throw HHuginnRuntimeException( "Function of the same name `"_ys.append( name ).append( "' is already defined." ), MAIN_FILE_ID, cc->_position.get() );
		}
		HClass const* super( nullptr );
		if ( cc->_baseName != INVALID_IDENTIFIER ) {
			super = commit_class( cc->_baseName );
		}
		field_definitions_t fieldDefinitions;
		huginn::HThread t( _runtime.raw(), hcore::HThread::get_current_thread_id() );
		t.create_function_frame( nullptr, nullptr, 0 );
		HFrame* frame( t.current_frame() );
		for ( int i( 0 ), size( static_cast<int>( cc->_fieldNames.get_size() ) ); i < size; ++ i ) {
			OCompiler::OClassContext::expressions_t::const_iterator f( cc->_fieldDefinitions.find( i ) );
			if ( f != cc->_fieldDefinitions.end() ) {
				f->second->execute( &t );
				fieldDefinitions.emplace_back( cc->_fieldNames[i], frame->result(), cc->_docs.at( i ) );
			} else {
				OCompiler::OClassContext::methods_t::const_iterator m( cc->_methods.find( i ) );
				M_ASSERT( m != cc->_methods.end() );
				fieldDefinitions.emplace_back( cc->_fieldNames[i], _runtime->object_factory()->create<HClass::HMethod>( m->second ), cc->_docs.at( i ) );
			}
		}
		t.pop_frame();
		if ( ! cls ) {
			cls = _runtime->create_class( identifierId_, super, fieldDefinitions, cc->_doc ? *cc->_doc : "" );
			_runtime->register_class_low( cls, ACCESS::PUBLIC );
		} else {
			cls->redefine( super, fieldDefinitions );
		}
		_compiler->_submittedClasses.erase( identifierId_ );
	}
	return ( cls.raw() );
	M_EPILOG
}

void HHuginn::finalize_compilation( paths_t const& paths_, compiler_setup_t compilerSetup_ ) {
	M_PROLOG
	for ( OCompiler::submitted_imports_t::value_type const& i : _compiler->_submittedImports ) {
		_runtime->register_package( i._package, i._alias, paths_, compilerSetup_, i._position );
	}
	typedef HArray<identifier_id_t> identifiers_t;
	identifiers_t classIdentifiers;
	for ( OCompiler::submitted_classes_t::value_type const& sc : _compiler->_submittedClasses ) {
		_compiler->track_name_cycle( sc.first );
		classIdentifiers.push_back( sc.first );
	}
	for ( identifier_id_t const& id : classIdentifiers ) {
		commit_class( id );
	}
	M_ASSERT( _compiler->_submittedClasses.is_empty() );
	if ( compilerSetup_ & COMPILER::BE_STRICT ) {
		_compiler->detect_misuse();
	}
	_compiler->resolve_symbols();
	return;
	M_EPILOG
}

void HHuginn::register_class( class_t class_, ACCESS classConstructorAccess_ ) {
	M_PROLOG
	M_ENSURE( _state != STATE::COMPILED );
	_runtime->register_class_low( class_, classConstructorAccess_ );
	OCompiler::OIdentifierUse& ciu( _compiler->_usedIdentifiers[class_->identifier_id()] );
	ciu.write( 0, OCompiler::OIdentifierUse::TYPE::CLASS );
	ciu.read( 0 );
	for ( identifier_id_t id : class_->field_identifiers() ) {
		OCompiler::OIdentifierUse& iu( _compiler->_usedIdentifiers[id] );
		iu.write( 0, OCompiler::OIdentifierUse::TYPE::FIELD );
		iu.read( 0 );
	}
	return;
	M_EPILOG
}

void HHuginn::register_function( identifier_id_t id_ ) {
	M_PROLOG
	OCompiler::OIdentifierUse& iu( _compiler->_usedIdentifiers[id_] );
	iu.write( 0, OCompiler::OIdentifierUse::TYPE::FUNCTION );
	iu.read( 0 );
	return;
	M_EPILOG
}

bool HHuginn::compile( paths_t const& paths_, compiler_setup_t compilerSetup_, HIntrospectorInterface* introspector_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED, "Parsing step is required before compilation." );
	if ( ( compilerSetup_ & COMPILER::BE_STRICT ) && ( compilerSetup_ & COMPILER::BE_SLOPPY ) ) {
		throw HHuginnException( "BE_STRICT and BE_SLOPPY flags are mutually exclusive." );
	}
	if ( ! ( ( compilerSetup_ & COMPILER::BE_STRICT ) || ( compilerSetup_ & COMPILER::BE_SLOPPY ) ) ) {
		compilerSetup_ |= COMPILER::BE_STRICT;
	}
	bool ok( false );
	int mainStatementCount( _compiler->_mainStatementCount );
	try {
		_compiler->set_setup( compilerSetup_, introspector_ );
		_engine.execute( &( _compiler->_classNoter ) );
		_compiler->_classNoter._passThrough = true;
		_engine.execute();
		finalize_compilation( paths_, compilerSetup_ );
		_state = STATE::COMPILED;
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		_compiler->_mainStatementCount = mainStatementCount;
		_errorMessage = e.message();
		_errorPosition = e.position();
		_errorFileId = e.file_id();
	}
	return ( ok );
	M_EPILOG
}

bool HHuginn::compile( compiler_setup_t compilerSetup_, HIntrospectorInterface* introspector_ ) {
	M_PROLOG
	return ( compile( paths_t(), compilerSetup_, introspector_ ) );
	M_EPILOG
}

bool HHuginn::execute( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED, "Program must be compiled before execution." );
	bool ok( false );
	try {
		_runtime->execute();
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		_errorMessage = e.message();
		_errorPosition = e.position();
		_errorFileId = e.file_id();
	}
	return ( ok );
	M_EPILOG
}

yaal::hcore::HString HHuginn::source_name( int fileId_ ) const {
	M_PROLOG
	return ( _sources[fileId_]->name() );
	M_EPILOG
}

yaal::hcore::HString HHuginn::where( int fileId_, int position_ ) const {
	M_PROLOG
	hcore::HString w( source_name( fileId_ ) );
	HHuginn::HErrorCoordinate ec( get_coordinate( fileId_, position_ ) );
	w.append( ":" ).append( ec.line() ).append( ":" ).append( ec.column() );
	return ( w );
	M_EPILOG
}

int HHuginn::error_position( void ) const {
	M_PROLOG
	return ( _sources[_errorFileId]->error_position( _errorPosition ) );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::error_coordinate( void ) const {
	M_PROLOG
	return ( _sources[_errorFileId]->error_coordinate( error_position() ) );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::get_coordinate( int fileId_, int position_ ) const {
	M_PROLOG
	return ( _sources[fileId_]->error_coordinate( _sources[fileId_]->error_position( position_ ) ) );
	M_EPILOG
}

yaal::hcore::HString HHuginn::error_message( void ) const {
	M_PROLOG
	hcore::HString message( _sources[_errorFileId]->name() );
	HErrorCoordinate coord( error_coordinate() );
	if ( ! _errorMessage.is_empty() ) {
		message
			.append( ':' )
			.append( coord.line() )
			.append( ':' )
			.append( coord.column() )
			.append( ": " )
			.append( _errorMessage );
	} else {
		message.append( ": no error" );
	}
	return ( message );
	M_EPILOG
}

char const* HHuginn::error_message( int code_ ) const {
	return ( ::error_message( code_ ) );
}

yaal::hcore::HString HHuginn::get_snippet( int from_, int len_ ) const {
	M_PROLOG
	return ( _sources.front()->get_snippet( from_, len_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::get_comment( int pos_ ) const {
	M_PROLOG
	return ( _sources.front()->get_comment( pos_ ) );
	M_EPILOG
}

void HHuginn::set_input_stream( yaal::hcore::HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	_inputStream = stream_;
	_inputStreamRaw = _inputStream.raw();
	return;
	M_EPILOG
}

void HHuginn::set_input_stream( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_inputStream.reset();
	_inputStreamRaw = &stream_;
	return;
	M_EPILOG
}

void HHuginn::set_output_stream( yaal::hcore::HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	_outputStream = stream_;
	_outputStreamRaw = _outputStream.raw();
	return;
	M_EPILOG
}

void HHuginn::set_output_stream( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_outputStream.reset();
	_outputStreamRaw = &stream_;
	return;
	M_EPILOG
}

void HHuginn::set_error_stream( yaal::hcore::HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	_errorStream = stream_;
	_errorStreamRaw = _errorStream.raw();
	return;
	M_EPILOG
}

void HHuginn::set_error_stream( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_errorStream.reset();
	_errorStreamRaw = &stream_;
	return;
	M_EPILOG
}

void HHuginn::set_log_stream( yaal::hcore::HStreamInterface::ptr_t stream_ ) {
	M_PROLOG
	_logStream = stream_;
	_logStreamRaw = _logStream.raw();
	return;
	M_EPILOG
}

void HHuginn::set_log_stream( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_logStream.reset();
	_logStreamRaw = &stream_;
	return;
	M_EPILOG
}

yaal::hcore::HStreamInterface& HHuginn::input_stream( void ) {
	M_PROLOG
	M_ENSURE( _inputStreamRaw );
	return ( *_inputStreamRaw );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HHuginn::output_stream( void ) {
	M_PROLOG
	M_ENSURE( _outputStreamRaw );
	return ( *_outputStreamRaw );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HHuginn::error_stream( void ) {
	M_PROLOG
	M_ENSURE( _errorStreamRaw );
	return ( *_errorStreamRaw );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HHuginn::log_stream( void ) {
	M_PROLOG
	M_ENSURE( _logStreamRaw );
	return ( *_logStreamRaw );
	M_EPILOG
}

void HHuginn::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	_runtime->add_argument( arg_ );
	return;
	M_EPILOG
}

void HHuginn::clear_arguments( void ) {
	M_PROLOG
	_runtime->clear_arguments();
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::result( void ) const {
	return ( _runtime->result() );
}

void HHuginn::dump_preprocessed_source( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	_sources.front()->dump_preprocessed( stream_ );
	return;
	M_EPILOG
}

HRuntime const& HHuginn::runtime( void ) const {
	return ( *_runtime );
}

void HHuginn::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	_runtime->dump_vm_state( stream_ );
	return;
	M_EPILOG
}

void HHuginn::dump_docs( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	_runtime->dump_docs( stream_ );
	return;
	M_EPILOG
}

HHuginn::HValue::HValue( HClass const* class_ )
	: _class( class_ ) {
	return;
}

int HHuginn::HValue::field_index( identifier_id_t identifierId_ ) const {
	M_PROLOG
	return ( _class->field_index( identifierId_ ) );
	M_EPILOG
}

bool HHuginn::HValue::is_kind_of( HHuginn::identifier_id_t typeName_ ) const {
	M_PROLOG
	return ( _class->is_kind_of( typeName_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HValue::do_field( HHuginn::value_t const& object_, int index_ ) const {
	M_PROLOG
	value_t const& f( _class->field( index_ ) );
	M_ASSERT( f->type_id() == TYPE::METHOD );
	return (
		_class->runtime()->object_factory()->create_bound_method(
			static_cast<HClass::HMethod const*>( f.raw() )->function(),
			object_
		)
	);
	M_EPILOG
}

HHuginn::value_t HHuginn::HValue::clone( huginn::HThread* thread_, int position_ ) const {
	return ( do_clone( thread_, position_ ) );
}

HHuginn::value_t HHuginn::HValue::do_clone( huginn::HThread* thread_, int ) const {
	M_ASSERT( _class->type_id() == TYPE::NONE );
	return ( thread_->runtime().none_value() );
}

HHuginn::HObserver::HObserver( HHuginn::value_t const& value_ )
	: HValue( &_observerClass_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HObserver::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HObserver::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->object_factory().create<HObserver>( _value ) );
}

HHuginn::HReference::HReference( HHuginn::value_t& value_ )
	: HValue( &_referenceClass_ ), _value( value_ ) {
	return;
}

HHuginn::value_t& HHuginn::HReference::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HReference::do_clone( huginn::HThread*, int ) const {
	M_ASSERT( 0 && "cloning reference"[0] );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( HHuginn::value_t() );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HHuginn::HIterable::HIterable( HClass const* class_ )
	: HValue( class_ ) {
	return;
}

HHuginn::HIterable::HIterator HHuginn::HIterable::iterator( huginn::HThread* thread_, int position_ ) {
	return ( do_iterator( thread_, position_ ) );
}

int long HHuginn::HIterable::size( huginn::HThread* thread_, int position_ ) const {
	return ( do_size( thread_, position_ ) );
}

HHuginn::HTernaryEvaluator::HTernaryEvaluator(
	expression_t const& condition_,
	expression_t const& ifTrue_,
	expression_t const& ifFalse_
) : HValue( &_unknownClass_ ),
	_condition( condition_ ),
	_ifTrue( ifTrue_ ),
	_ifFalse( ifFalse_ ) {
	return;
}

HHuginn::value_t HHuginn::HTernaryEvaluator::execute( huginn::HThread* thread_ ) {
	M_PROLOG
	_condition->execute( thread_ );
	HFrame* f( thread_->current_frame() );
	value_t v( f->result() );
	if ( v->type_id() != TYPE::BOOLEAN ) {
		throw HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( v->get_class()->name() ), f->file_id(), _condition->position() );
	}
	if ( static_cast<HBoolean*>( v.raw() )->value() ) {
		_ifTrue->execute( thread_ );
	} else {
		_ifFalse->execute( thread_ );
	}
	return ( f->result() );
	M_EPILOG
}

HHuginn::value_t HHuginn::HTernaryEvaluator::do_clone( huginn::HThread*, int ) const {
	M_ASSERT( 0 && "cloning ternary evaluator"[0] );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( HHuginn::value_t() );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HHuginn::HFunctionReference::HFunctionReference(
	identifier_id_t identifierId_,
	function_t const& function_,
	yaal::hcore::HString const& doc_
) : HValue( &_functionReferenceClass_ )
	, _identifierId( identifierId_ )
	, _function( function_ )
	, _doc( doc_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunctionReference::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->runtime().object_factory()->create_function_reference( _identifierId, _function, _doc ) );
}

yaal::hcore::HString const& HHuginn::HFunctionReference::doc( void ) const {
	return ( _doc );
}

HHuginn::HClass::HMethod::HMethod(
	function_t const& function_
) : HValue( &_methodClass_ )
	, _function( function_ ) {
	return;
}

HHuginn::HClass::HMethod::HMethod(
	HHuginn::HClass const* class_,
	function_t const& function_
) : HValue( class_ )
	, _function( function_ ) {
	return;
}

HHuginn::function_t const& HHuginn::HClass::HMethod::function( void ) const {
	return ( _function );
}

HHuginn::value_t HHuginn::HClass::HMethod::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->object_factory().create<HMethod>( _function ) );
}

HHuginn::HClass::HBoundMethod::HBoundMethod( HHuginn::function_t const& method_, HHuginn::value_t const& object_ )
	: HMethod( &_boundMethodClass_, method_ )
	, _objectHolder( object_ ) {
	return;
}

HHuginn::value_t HHuginn::HClass::HBoundMethod::call( huginn::HThread* thread_, values_t& arguments_, int position_ ) {
	return ( _function( thread_, &_objectHolder, arguments_, position_ ) );
}

HHuginn::value_t HHuginn::HClass::HBoundMethod::do_clone( huginn::HThread* thread_, int position_ ) const {
	return (
		thread_->runtime().object_factory()->create_bound_method(
			static_cast<HMethod const*>( this )->function(),
			_objectHolder->clone( thread_, position_ )
		)
	);
}

yaal::hcore::HString to_string( HHuginn::value_t const& value_, HHuginn* huginn_ ) {
	HResource<huginn::HThread> threadHolder;
	HStatement stmt( HStatement::statement_id_t( 0 ), 0, 0 );
	if ( huginn_ ) {
		threadHolder.reset( new huginn::HThread( const_cast<HRuntime*>( &huginn_->runtime() ), hcore::HThread::get_current_thread_id() ) );
		threadHolder->create_function_frame( &stmt, nullptr, 0 );
	}
	HString s;
	try {
		s = value_builtin::string_representation( threadHolder.raw(), value_, 0 );
	} catch ( HHuginn::HHuginnRuntimeException const& e ) {
		s = e.message();
	} catch ( HException const& e ) {
		s = e.what();
	}
	return ( s );
}

yaal::hcore::HString to_string( HHuginn::value_t const& value_ ) {
	return ( to_string( value_, nullptr ) );
}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>
#include <cstring>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"
#include "hcore/hfile.hxx"
#include "hcore/system.hxx"
#include "hcore/hlog.hxx"
#include "tools/stringalgo.hxx"
#include "tools/filesystem.hxx"
#include "tools/huginn/source.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/objectfactory.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/expression.hxx"
#include "tools/huginn/scope.hxx"
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/keyword.hxx"
#include "tools/huginn/instruction.hxx"
#include "tools/huginn/iterable.hxx"
#include "tools/huginn/objectreference.hxx"
#include "tools/huginn/observer.hxx"
#include "tools/huginn/ternaryevaluator.hxx"

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

char const DEFAULT_PATHS[] = ".:" LIBDIR "/huginn:" DATADIR "/huginn";
char const* const MODULE_PATHS_RAW( ::getenv( "HUGINNPATH" ) );
hcore::HString MODULE_PATHS_S(
	( MODULE_PATHS_RAW ? hcore::to_string( MODULE_PATHS_RAW ).append( ":" ) : hcore::to_string( "" ) )
		.append( DEFAULT_PATHS )
		.append( ":" )
		.append( filesystem::dirname( system::get_self_exec_path() ) )
		.append( "/packages" )
);

}

HHuginn::paths_t const HHuginn::MODULE_PATHS( string::split<HHuginn::paths_t>( MODULE_PATHS_S, ":", HTokenizer::SKIP_EMPTY ) );

static int const DEFAULT_MAX_CALL_STACK( 1024 );
int _huginnMaxCallStack_( DEFAULT_MAX_CALL_STACK );

namespace huginn {

char const* _errMsgHHuginn_[ 11 ] = {
	_( "Operands are not summable: " ),
	_( "Operands are not subtractable: " ),
	_( "Operands are not multipliable: " ),
	_( "Operands are not divisible: " ),
	_( "Operands are not exponentiationable: " ),
	_( "Operands are not comparable: " ),
	_( "Operand is not a numeric value: " ),
	_( "Operands are not boolean values: " ),
	_( "Operand is not a boolean value: " ),
	_( "Subscript is not an integer: " ),
	_( "Operand is not a collection type: " )
};

}

HHuginn::compiler_setup_t const HHuginn::COMPILER::DEFAULT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_STRICT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_SLOPPY = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::OPTIMIZE = HHuginn::compiler_setup_t::new_flag();

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

HHuginn::HCallSite::HCallSite( yaal::hcore::HString const& file_, int line_, int column_, yaal::hcore::HString const& context_ )
	: _file( file_ )
	, _coordinate( line_, column_ )
	, _context( context_ ) {
}

yaal::hcore::HString const& HHuginn::HCallSite::file( void ) const {
	return ( _file );
}

int HHuginn::HCallSite::line( void ) const {
	return ( _coordinate.line() );
}

int HHuginn::HCallSite::column( void ) const {
	return ( _coordinate.column() );
}

yaal::hcore::HString const& HHuginn::HCallSite::context( void ) const {
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

HHuginn::call_stack_t HIntrospecteeInterface::get_call_stack( void ) {
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

void HIntrospectorInterface::symbol( symbols_t const& symbol_, HHuginn::SYMBOL_KIND symbolKind_, int fileId_, int position_ ) {
	M_PROLOG
	do_symbol( symbol_, symbolKind_, fileId_, position_ );
	return;
	M_EPILOG
}

HHuginn::HReferenceTracker::HReferenceTracker( void )
	: _observers() {
	return;
}

void HHuginn::HReferenceTracker::notify( HHuginn::HNotifableReference* ref_ ) {
	M_PROLOG
	_observers.push_back( ref_ );
	return;
	M_EPILOG
}

void HHuginn::HReferenceTracker::forget( HHuginn::HNotifableReference* ref_ ) {
	M_PROLOG
	_observers.erase( find( _observers.begin(), _observers.end(), ref_ ) );
	return;
	M_EPILOG
}

void HHuginn::HReferenceTracker::invalidate( ids_t& ids_ ) {
	M_PROLOG
	sort( ids_.begin(), ids_.end() );
	for ( HHuginn::HNotifableReference* nr : _observers ) {
		M_ASSERT( dynamic_cast<huginn::HIdentifableReference*>( nr ) );
		HIdentifableReference* ir( static_cast<huginn::HIdentifableReference*>( nr ) );
		ids_t::const_iterator it( lower_bound( ids_.begin(), ids_.end(), ir->id() ) );
		if ( ( it != ids_.end() ) && ( *it == ir->id() ) ) {
			ir->invalidate();
		}
	}
	return;
	M_EPILOG
}

void HHuginn::HReferenceTracker::invalidate( void ) {
	M_PROLOG
	for ( HHuginn::HNotifableReference* nr : _observers ) {
		nr->invalidate();
	}
	return;
	M_EPILOG
}

HInvalidatingIterable::HInvalidatingIterable( huginn::HClass const* class_ )
	: HIterable( class_ )
	, HReferenceTracker() {
	return;
}

void HInvalidatingIterable::skip( huginn::HThread* thread_, void const* id_, int position_ ) {
	M_PROLOG
	for ( HHuginn::HNotifableReference* nr : _observers ) {
		M_ASSERT( dynamic_cast<huginn::HSkippingIterator*>( nr ) );
		HSkippingIterator* si( static_cast<huginn::HSkippingIterator*>( nr ) );
		if ( si->id() == id_ ) {
			si->skip( thread_, position_ );
		}
	}
	return;
	M_EPILOG
}

void HInvalidatingIterable::skip( huginn::HThread* thread_, int long from_, int long to_, int position_ ) {
	M_PROLOG
	for ( HHuginn::HNotifableReference* nr : _observers ) {
		M_ASSERT( dynamic_cast<huginn::HBacktrackingIterator*>( nr ) );
		HBacktrackingIterator* si( static_cast<huginn::HBacktrackingIterator*>( nr ) );
		si->skip( thread_, from_, to_, position_ );
	}
	return;
	M_EPILOG
}

void HInvalidatingIterable::backtrack( huginn::HThread* thread_, int long index_, int position_ ) {
	M_PROLOG
	for ( HHuginn::HNotifableReference* nr : _observers ) {
		M_ASSERT( dynamic_cast<huginn::HBacktrackingIterator*>( nr ) );
		static_cast<huginn::HBacktrackingIterator*>( nr )->backtrack( thread_, index_, position_ );
	}
	return;
	M_EPILOG
}

HObjectReference::HObjectReference( huginn::HClass const* class_, value_t const& value_, int upCastLevel_, int fileId_, int position_ )
	: HValue( class_ )
	, _value( value_ )
	, _referenceClass( nullptr ) {
	M_PROLOG
	HObject const* o( dynamic_cast<HObject const*>( _value.raw() ) );
	M_ASSERT( o );
	HClass const* c( o->get_class() );
	for ( int i( 0 ); i < ( upCastLevel_ + 1 ); ++ i ) {
		HClass const* s( c->super() );
		if ( ! s ) {
			throw HHuginn::HHuginnRuntimeException(
				"`"_ys.append( c->name() ).append( "` does not have superclass." ),
				fileId_,
				position_
			);
		}
		c = s;
	}
	_referenceClass = c;
	return;
	M_EPILOG
}

HObjectReference::HObjectReference( huginn::HClass const* class_, value_t const& value_, HClass const* referenceClass_ )
	: HValue( class_ )
	, _value( value_ )
	, _referenceClass( referenceClass_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HObjectReference::field_index( HHuginn::identifier_id_t const& identifierId_ ) const {
	M_PROLOG
	HValue const* v( _value.raw() );
	return ( _referenceClass != v->get_class() ? _referenceClass->field_index( identifierId_ ) : v->field_index( identifierId_ ) );
	M_EPILOG
}

HHuginn::value_t HObjectReference::field( huginn::HThread* thread_, int index_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( _value.raw() ) );
	value_t v;
	if ( o->get_class()->is_overridden( _referenceClass, index_ ) ) {
		v = _referenceClass->get_default( thread_, index_, position_ );
	} else {
		v = o->field_ref( index_ );
	}
	if ( v->type_id() == HHuginn::TYPE::METHOD ) {
		v = _referenceClass->runtime()->object_factory()->create_bound_method(
			static_cast<HClass::HMethod*>( v.raw() )->function(),
			_value
		);
	}
	return v;
	M_EPILOG
}

HHuginn::value_t HObjectReference::do_clone( huginn::HThread* thread_, HHuginn::value_t* object_, int position_ ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HObjectReference>( get_class(), _value->clone( thread_, object_, position_ ), _referenceClass ) );
	M_EPILOG
}

HTaggedValue::HTaggedValue( value_t const& value_, HClass const* class_ )
	: HValue( class_ )
	, _value( value_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HHuginn::value_t HTaggedValue::do_clone( huginn::HThread*, HHuginn::value_t*, int ) const {
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

HHuginn::HHuginn( compiler_setup_t compilerSetup_ )
	: _state( STATE::EMPTY )
	, _runtime( make_resource<HRuntime>( this ) )
	, _sources()
	, _compiler( make_resource<OCompiler>( _runtime.raw() ) )
	, _engine(
		make_engine( _runtime.raw(), compilerSetup_ ),
		_grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR
	)
	, _errorMessage()
	, _errorPosition( INVALID_POSITION )
	, _errorFileId( INVALID_FILE_ID )
	, _trace()
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr )
	, _logStream()
	, _logStreamRaw( &hcore::log ) {
	M_PROLOG
	_sources.emplace_back( make_pointer<HSource>() );
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
	, _engine(
		make_engine( runtime_, runtime_->compiler_setup() ),
		_grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR
	)
	, _errorMessage()
	, _errorPosition( INVALID_POSITION )
	, _errorFileId( INVALID_FILE_ID )
	, _trace()
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr )
	, _logStream()
	, _logStreamRaw( &hcore::log ) {
	M_PROLOG
	_sources.emplace_back( make_pointer<HSource>() );
	_grammarVerified.store( true );
	_runtime->register_builtins();
	return;
	M_EPILOG
}

void HHuginn::reset( int undoSteps_ ) {
	M_PROLOG
	_trace.clear();
	_errorPosition = INVALID_POSITION;
	_errorFileId = INVALID_FILE_ID;
	_errorMessage.clear();
	_compiler->reset( undoSteps_ );
	_sources.front()->reset();
	_runtime->reset();
	_state = STATE::EMPTY;
	return;
	M_EPILOG
}

void HHuginn::finalize( void ) {
	M_PROLOG
	_runtime->finalize();
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

huginn::HSource& HHuginn::current_source( void ) {
	M_PROLOG
	int fileId( _compiler->_fileId );
	return (
		( fileId == INVALID_FILE_ID )
		|| (
			( fileId == MAIN_FILE_ID )
			&& ( ( _state == STATE::PARSED ) || ( _state == STATE::COMPILED ) || ( _state == STATE::RUNNING ) )
		)
			? *_sources.front()
			: (
				( ( _state == STATE::LOADED ) || ( _state == STATE::PREPROCESSED ) || ( _state == STATE::PARSED ) )
					? *_sources.back()
					: *_sources[fileId]
			)
	);
	M_EPILOG
}

huginn::HSource const & HHuginn::current_source( void ) const {
	M_PROLOG
	return ( const_cast<HHuginn*>( this )->current_source() );
	M_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, yaal::hcore::HString const& name_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	current_source().load( stream_, name_, skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	current_source().load( stream_, hcore::HString(), skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::preprocess( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::LOADED, "Program source must be loaded before preprocessing." );
	current_source().preprocess();
	_state = STATE::PREPROCESSED;
	return;
	M_EPILOG
}

bool HHuginn::parse( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PREPROCESSED, "Preprocessor step is required before parsing." );
	bool ok( _engine.parse( current_source().begin(), current_source().end() ) );
	if ( ! ok ) {
		_errorMessage = _engine.error_messages()[0];
		_errorPosition = _engine.error_position();
		_errorFileId = _compiler->_isModule ? static_cast<int>( _sources.get_size() ) - 1 : MAIN_FILE_ID;
	} else {
		_state = STATE::PARSED;
	}
	return ok;
	M_EPILOG
}

huginn::HClass const* HHuginn::commit_class( identifier_id_t identifierId_ ) {
	M_PROLOG
	class_t cls( _runtime->get_class( identifierId_ ) );
	if ( _compiler->_submittedClasses.count( identifierId_ ) > 0 ) {
		OCompiler::OClassContext* cc( _compiler->_submittedClasses.at( identifierId_ ).get() );
		M_ASSERT( cc->_classIdentifier == identifierId_ );
		HClass const* super( nullptr );
		if ( cc->_baseName != IDENTIFIER::INVALID ) {
			HHuginn::class_t c( _runtime->get_class( cc->_baseName ) );
			super = ! c ? commit_class( cc->_baseName ) : c.raw();
		}
		if ( ! cls ) {
			cls = _runtime->create_class( identifierId_, cc->_doc ? *cc->_doc : "", ACCESS::PUBLIC, HClass::TYPE::USER );
		}
		_runtime->register_class( cls, VISIBILITY::GLOBAL );
		field_definitions_t fieldDefinitions;
		huginn::HThread t( _runtime.raw(), hcore::HThread::get_current_thread_id() );
		t.create_frame( nullptr, nullptr, 0 );
		for ( int i( 0 ), size( static_cast<int>( cc->_fieldNames.get_size() ) ); i < size; ++ i ) {
			OCompiler::OClassContext::expressions_t::const_iterator f( cc->_fieldDefinitions.find( i ) );
			if ( f != cc->_fieldDefinitions.end() ) {
				fieldDefinitions.emplace_back( cc->_fieldNames[i], f->second->evaluate( &t ), cc->_docs.at( i ) );
			} else {
				OCompiler::OClassContext::methods_t::const_iterator m( cc->_methods.find( i ) );
				M_ASSERT( m != cc->_methods.end() );
				fieldDefinitions.emplace_back( cc->_fieldNames[i], _runtime->object_factory()->create_method_raw( m->second ), cc->_docs.at( i ) );
			}
		}
		t.pop_frame();
		cls->redefine( super, fieldDefinitions );
		_compiler->_submittedClasses.erase( identifierId_ );
	}
	return ( cls.raw() );
	M_EPILOG
}

void HHuginn::finalize_compilation( void ) {
	M_PROLOG
	for ( OCompiler::submitted_imports_t::value_type const& i : _compiler->_submittedImports ) {
		if ( i._alias != IDENTIFIER::INVALID ) {
			_runtime->register_package( i._package, i._alias, i._position );
		} else {
			_runtime->import_symbols( i._package, i._importedSymbols, i._position );
		}
	}
	for ( OCompiler::submitted_enums_t::value_type const& e : _compiler->_submittedEnums ) {
		M_ASSERT( is_enum_class( e ) );
		_runtime->register_global( e->get_class()->identifier_id(), e );
	}
	identifiers_t classIdentifiers;
	for ( OCompiler::submitted_classes_t::value_type const& sc : _compiler->_submittedClasses ) {
		_compiler->track_name_cycle( sc.first );
		classIdentifiers.push_back( sc.first );
	}
	for ( identifier_id_t const& id : classIdentifiers ) {
		commit_class( id );
	}
	M_ASSERT( _compiler->_submittedClasses.is_empty() );
	if ( _runtime->compiler_setup() & COMPILER::BE_STRICT ) {
		_compiler->detect_misuse();
	}
	_compiler->resolve_symbols();
	return;
	M_EPILOG
}

void HHuginn::register_class( class_t class_, VISIBILITY classVisibility_ ) {
	M_PROLOG
	M_ENSURE( _state != STATE::COMPILED );
	_runtime->register_class( class_, classVisibility_ );
	if ( classVisibility_ != VISIBILITY::HIDDEN ) {
		OCompiler::OIdentifierUse& ciu( _compiler->_usedIdentifiers[class_->identifier_id()] );
		ciu.write( 0, SYMBOL_KIND::CLASS );
		ciu.read( 0 );
		for ( identifier_id_t id : class_->field_identifiers() ) {
			OCompiler::OIdentifierUse& iu( _compiler->_usedIdentifiers[id] );
			iu.write( 0, SYMBOL_KIND::FIELD );
			iu.read( 0 );
		}
	}
	return;
	M_EPILOG
}

void HHuginn::register_function( yaal::hcore::HString const& name_, function_t&& func_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted name." ), MAIN_FILE_ID, 0 );
	}
	identifier_id_t id( _runtime->identifier_id( name_ ) );
	_runtime->register_function( id, func_, doc_ );
	register_function( id );
	return;
	M_EPILOG
}

void HHuginn::register_function( identifier_id_t id_ ) {
	M_PROLOG
	OCompiler::OIdentifierUse& iu( _compiler->_usedIdentifiers[id_] );
	iu.write( 0, SYMBOL_KIND::FUNCTION );
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
	int mainStatementCount( _compiler->_mainCompiledStatementCount );
	try {
		_compiler->set_setup(
			introspector_,
			count_if(
				_sources.begin(),
				_sources.end(),
				[this]( source_t const& src_ ) {
					return ( src_->name() == current_source().name() );
				}
			) > 1
		);
		_runtime->set_setup( paths_, compilerSetup_ );
		_engine.execute( &( _compiler->_classNoter ) );
		_compiler->_classNoter.set_pass( OCompiler::HMultiPassDispatcher::PASS::FINAL );
		_engine.execute();
		finalize_compilation();
		_state = STATE::COMPILED;
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		consume_error( e, mainStatementCount );
	}
	return ok;
	M_EPILOG
}

bool HHuginn::compile( compiler_setup_t compilerSetup_, HIntrospectorInterface* introspector_ ) {
	M_PROLOG
	return ( compile( paths_t(), compilerSetup_, introspector_ ) );
	M_EPILOG
}

int HHuginn::new_statement_count( void ) const {
	M_PROLOG
	M_ENSURE( _compiler->_isIncremental, "Statement count query makes sense only in incremental mode." );
	M_ENSURE( _state == STATE::COMPILED, "Program must be compiled and not executed for querying new statement count." );
	return ( _compiler->_mainCompiledStatementCount - _compiler->_mainExecutedStatementCount );
	M_EPILOG
}

bool HHuginn::execute( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED, "Program must be compiled before execution." );
	HScopedValueReplacement<STATE> state( _state, STATE::RUNNING );
	bool ok( false );
	try {
		_runtime->execute();
		_compiler->_mainExecutedStatementCount = _compiler->_mainCompiledStatementCount;
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		consume_error( e, _compiler->_mainExecutedStatementCount );
	}
	_outputStreamRaw->flush();
	_errorStreamRaw->flush();
	return ok;
	M_EPILOG
}

HHuginn::value_t HHuginn::call( yaal::hcore::HString const& name_, HHuginn::values_t const& argv_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED, "Program must be compiled before calling a function." );
	HScopedValueReplacement<STATE> state( _state, STATE::RUNNING );
	HHuginn::value_t res;
	try {
		res = _runtime->call( name_, argv_ );
		_compiler->_mainExecutedStatementCount = _compiler->_mainCompiledStatementCount;
	} catch ( HHuginnRuntimeException const& e ) {
		consume_error( e, _compiler->_mainExecutedStatementCount );
	}
	_outputStreamRaw->flush();
	_errorStreamRaw->flush();
	return res;
	M_EPILOG
}

void HHuginn::consume_error( HHuginnRuntimeException const& ex_, int statementCount_ ) {
	M_PROLOG
	_compiler->_mainCompiledStatementCount = statementCount_;
	_errorMessage = ex_.message();
	_errorPosition = ex_.position();
	_errorFileId = ex_.file_id();
	_trace = _runtime->trace();
	return;
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
	HHuginn::HCoordinate ec( get_coordinate( fileId_, position_ ) );
	w.append( ":" ).append( ec.line() ).append( ":" ).append( ec.column() );
	return w;
	M_EPILOG
}

int HHuginn::error_position( void ) const {
	M_PROLOG
	return ( _errorFileId >= 0 ? _sources[_errorFileId]->real_position( _errorPosition ) : 0 );
	M_EPILOG
}

int HHuginn::file_id( void ) const {
	M_PROLOG
	return ( static_cast<int>( _sources.get_size() ) - 1 );
	M_EPILOG
}

HHuginn::HCoordinate HHuginn::error_coordinate( void ) const {
	M_PROLOG
	return ( _errorFileId >= 0 ? _sources[_errorFileId]->get_coordinate( error_position() ) : HCoordinate( 0, 0 ) );
	M_EPILOG
}

int HHuginn::real_position( int fileId_, int position_ ) const {
	M_PROLOG
	return ( _sources[fileId_]->real_position( position_ ) );
	M_EPILOG
}

HHuginn::HCoordinate HHuginn::get_coordinate( int fileId_, int position_ ) const {
	M_PROLOG
	return ( _sources[fileId_]->get_coordinate( real_position( fileId_, position_ ) ) );
	M_EPILOG
}

yaal::hcore::HString HHuginn::error_message( void ) const {
	M_PROLOG
	hcore::HString message( _errorFileId >= 0 ? _sources[_errorFileId]->name() : "" );
	HCoordinate coord( error_coordinate() );
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
	return message;
	M_EPILOG
}

char const* HHuginn::error_message( int code_ ) const {
	return ( ::error_message( code_ ) );
}

HHuginn::call_stack_t const& HHuginn::trace( void ) const {
	return ( _trace );
}

yaal::hcore::time::duration_t HHuginn::execution_time( void ) const {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED );
	return ( _runtime->execution_time() );
	M_EPILOG
}

yaal::hcore::HString HHuginn::get_snippet( int from_, int len_ ) const {
	M_PROLOG
	return ( current_source().get_snippet( from_, len_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::get_comment( int pos_ ) const {
	M_PROLOG
	return ( current_source().get_comment( pos_ ) );
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
	if ( &stream_ == _inputStreamRaw ) {
		return;
	}
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
	if ( &stream_ == _outputStreamRaw ) {
		return;
	}
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
	if ( &stream_ == _errorStreamRaw ) {
		return;
	}
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

HHuginn::value_t HHuginn::value( int long long val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_integer( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( code_point_t val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_character( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( double long val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_real( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( yaal::hcore::HNumber&& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_number( yaal::move( val_ ) ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( yaal::hcore::HNumber const& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_number( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( yaal::hcore::HString&& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_string( yaal::move( val_ ) ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( yaal::hcore::HString const& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_string( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( bool val_ ) {
	M_PROLOG
	return ( _runtime->boolean_value( val_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( void const* val_ ) {
	M_PROLOG
	M_ENSURE( ! val_ );
	return ( _runtime->none_value() );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( values_t&& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_list( yaal::move( val_ ) ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::value( yaal::hcore::HDeque<value_t>&& val_ ) {
	M_PROLOG
	return ( _runtime->object_factory()->create_deque( yaal::move( val_ ) ) );
	M_EPILOG
}

void HHuginn::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	_runtime->add_argument( arg_ );
	return;
	M_EPILOG
}

void HHuginn::add_argument( value_t const& arg_ ) {
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
	current_source().dump_preprocessed( stream_ );
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

HObserver::HObserver( huginn::HClass const* class_, HHuginn::value_t const& value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HObserver::value( void ) const {
	return ( _value );
}

HHuginn::value_t HObserver::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->object_factory().create<HObserver>( get_class(), _value ) );
}

HTernaryEvaluator::HTernaryEvaluator(
	HHuginn::expression_t const& condition_,
	HHuginn::expression_t const& ifTrue_,
	HHuginn::expression_t const& ifFalse_
) : HValue( nullptr ),
	_condition( condition_ ),
	_ifTrue( ifTrue_ ),
	_ifFalse( ifFalse_ ) {
	return;
}

HHuginn::value_t HTernaryEvaluator::execute( huginn::HThread* thread_ ) {
	M_PROLOG
	;
	HFrame* f( thread_->current_frame() );
	value_t v( _condition->evaluate( thread_ ) );
	if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( v->get_class()->name() ), f->file_id(), _condition->position() );
	}
	if ( static_cast<HBoolean*>( v.raw() )->value() ) {
		v = _ifTrue->evaluate( thread_ );
	} else {
		v = _ifFalse->evaluate( thread_ );
	}
	return v;
	M_EPILOG
}

HHuginn::value_t HTernaryEvaluator::do_clone( huginn::HThread*, HHuginn::value_t*, int ) const {
	M_ASSERT( 0 && "cloning ternary evaluator"[0] );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( HHuginn::value_t() );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

namespace {

yaal::hcore::HString string_form(
	HHuginn::value_t const& value_,
	yaal::hcore::HString ( HValue::* stringifier_ )( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const,
	HRuntime const* runtime_
) {
	HResource<huginn::HThread> threadHolder;
	HStatement stmt( HHuginn::statement_id_t( 0 ), 0, { 0, 0 } );
	if ( runtime_ ) {
		threadHolder.reset( new huginn::HThread( const_cast<HRuntime*>( runtime_ ), hcore::HThread::get_current_thread_id() ) );
		threadHolder->create_frame( &stmt, nullptr, 0 );
	}
	hcore::HString s;
	try {
		HCycleTracker cycleTracker;
		s = ( value_.raw()->*stringifier_ )( threadHolder.raw(), value_, cycleTracker, 0 );
	} catch ( HHuginn::HHuginnRuntimeException const& e ) {
		s = e.message();
	} catch ( hcore::HException const& e ) {
		s = e.what();
	}
	return s;
}

}

yaal::hcore::HString code( HHuginn::value_t const& value_, HHuginn* huginn_ ) {
	return ( string_form( value_, &HValue::code, huginn_ ? &huginn_->runtime() : nullptr ) );
}

yaal::hcore::HString code( HHuginn::value_t const& value_ ) {
	return ( string_form( value_, &HValue::code, nullptr ) );
}

yaal::hcore::HString to_string( HHuginn::value_t const& value_, HHuginn* huginn_ ) {
	return ( string_form( value_, &HValue::to_string, huginn_ ? &huginn_->runtime() : nullptr ) );
}

yaal::hcore::HString to_string( HHuginn::value_t const& value_ ) {
	return ( string_form( value_, &HValue::to_string, nullptr ) );
}

}

}


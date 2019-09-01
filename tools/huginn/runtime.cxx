/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "runtime.hxx"
#include "keyword.hxx"
#include "objectfactory.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "builtin.hxx"
#include "helper.hxx"
#include "compiler.hxx"
#include "packagefactory.hxx"
#include "tools/util.hxx"
#include "tools/stringalgo.hxx"
#include "tools/tools.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HRuntime::HRuntime( HHuginn* huginn_ )
	: _huginn( huginn_ )
	, _idGenerator( type_id( HHuginn::TYPE::NOT_BOOLEAN ).get() )
	, _identifierIds( {
			{ KEYWORD::CONSTRUCTOR, IDENTIFIER::KEYWORD::CONSTRUCTOR },
			{ KEYWORD::DESTRUCTOR, IDENTIFIER::KEYWORD::DESTRUCTOR },
			{ KEYWORD::THIS, IDENTIFIER::KEYWORD::THIS },
			{ KEYWORD::SUPER, IDENTIFIER::KEYWORD::SUPER },
			{ KEYWORD::ASSERT, IDENTIFIER::KEYWORD::ASSERT },
			{ BUILTIN::INTEGER, IDENTIFIER::BUILTIN::INTEGER },
			{ BUILTIN::REAL, IDENTIFIER::BUILTIN::REAL },
			{ BUILTIN::NUMBER, IDENTIFIER::BUILTIN::NUMBER },
			{ BUILTIN::STRING, IDENTIFIER::BUILTIN::STRING },
			{ BUILTIN::CHARACTER, IDENTIFIER::BUILTIN::CHARACTER },
			{ BUILTIN::BOOLEAN, IDENTIFIER::BUILTIN::BOOLEAN },
			{ BUILTIN::TUPLE, IDENTIFIER::BUILTIN::TUPLE },
			{ BUILTIN::LIST, IDENTIFIER::BUILTIN::LIST },
			{ BUILTIN::DEQUE, IDENTIFIER::BUILTIN::DEQUE },
			{ BUILTIN::DICT, IDENTIFIER::BUILTIN::DICT },
			{ BUILTIN::LOOKUP, IDENTIFIER::BUILTIN::LOOKUP },
			{ BUILTIN::ORDER, IDENTIFIER::BUILTIN::ORDER },
			{ BUILTIN::SET, IDENTIFIER::BUILTIN::SET },
			{ BUILTIN::HEAP, IDENTIFIER::BUILTIN::HEAP },
			{ BUILTIN::BLOB, IDENTIFIER::BUILTIN::BLOB },
			{ BUILTIN::TYPE_NONE, IDENTIFIER::BUILTIN::TYPE_NONE },
			{ BUILTIN::TYPE_OBSERVER, IDENTIFIER::BUILTIN::TYPE_OBSERVER },
			{ BUILTIN::TYPE_REFERENCE, IDENTIFIER::BUILTIN::TYPE_REFERENCE },
			{ BUILTIN::TYPE_FUNCTION_REFERENCE, IDENTIFIER::BUILTIN::TYPE_FUNCTION_REFERENCE },
			{ BUILTIN::TYPE_OBJECT_REFERENCE, IDENTIFIER::BUILTIN::TYPE_OBJECT_REFERENCE },
			{ BUILTIN::TYPE_METHOD, IDENTIFIER::BUILTIN::TYPE_METHOD },
			{ BUILTIN::TYPE_UNBOUND_METHOD, IDENTIFIER::BUILTIN::TYPE_UNBOUND_METHOD },
			{ BUILTIN::TYPE_BOUND_METHOD, IDENTIFIER::BUILTIN::TYPE_BOUND_METHOD },
			{ BUILTIN::TYPE_VARIADIC_PARAMETERS, IDENTIFIER::BUILTIN::TYPE_VARIADIC_PARAMETERS },
			{ BUILTIN::TYPE_NAMED_PARAMETERS, IDENTIFIER::BUILTIN::TYPE_NAMED_PARAMETERS },
			{ BUILTIN::SIZE, IDENTIFIER::BUILTIN::SIZE },
			{ BUILTIN::TYPE, IDENTIFIER::BUILTIN::TYPE },
			{ BUILTIN::COPY, IDENTIFIER::BUILTIN::COPY },
			{ BUILTIN::OBSERVE, IDENTIFIER::BUILTIN::OBSERVE },
			{ BUILTIN::USE, IDENTIFIER::BUILTIN::USE },
			{ BUILTIN::SQUARE_ROOT, IDENTIFIER::BUILTIN::SQUARE_ROOT },
			{ BUILTIN::N_ARY_SUMMATION, IDENTIFIER::BUILTIN::N_ARY_SUMMATION },
			{ BUILTIN::N_ARY_PRODUCT, IDENTIFIER::BUILTIN::N_ARY_PRODUCT },
			{ INTERFACE::CLONE,            IDENTIFIER::INTERFACE::CLONE },
			{ INTERFACE::GET_SIZE,         IDENTIFIER::INTERFACE::GET_SIZE },
			{ INTERFACE::SUBSCRIPT,        IDENTIFIER::INTERFACE::SUBSCRIPT },
			{ INTERFACE::SET_SUBSCRIPT,    IDENTIFIER::INTERFACE::SET_SUBSCRIPT },
			{ INTERFACE::MEMBER,           IDENTIFIER::INTERFACE::MEMBER },
			{ INTERFACE::SET_MEMBER,       IDENTIFIER::INTERFACE::SET_MEMBER },
			{ INTERFACE::ITERATOR,         IDENTIFIER::INTERFACE::ITERATOR },
			{ INTERFACE::IS_VALID,         IDENTIFIER::INTERFACE::IS_VALID },
			{ INTERFACE::NEXT,             IDENTIFIER::INTERFACE::NEXT },
			{ INTERFACE::VALUE,            IDENTIFIER::INTERFACE::VALUE },
			{ INTERFACE::CALL,             IDENTIFIER::INTERFACE::CALL },
			{ INTERFACE::HASH,             IDENTIFIER::INTERFACE::HASH },
			{ INTERFACE::EQUALS,           IDENTIFIER::INTERFACE::EQUALS },
			{ INTERFACE::LESS,             IDENTIFIER::INTERFACE::LESS },
			{ INTERFACE::GREATER,          IDENTIFIER::INTERFACE::GREATER },
			{ INTERFACE::LESS_OR_EQUAL,    IDENTIFIER::INTERFACE::LESS_OR_EQUAL },
			{ INTERFACE::GREATER_OR_EQUAL, IDENTIFIER::INTERFACE::GREATER_OR_EQUAL },
			{ INTERFACE::CONTAINS,         IDENTIFIER::INTERFACE::CONTAINS },
			{ INTERFACE::ADD,              IDENTIFIER::INTERFACE::ADD },
			{ INTERFACE::SUBTRACT,         IDENTIFIER::INTERFACE::SUBTRACT },
			{ INTERFACE::MULTIPLY,         IDENTIFIER::INTERFACE::MULTIPLY },
			{ INTERFACE::DIVIDE,           IDENTIFIER::INTERFACE::DIVIDE },
			{ INTERFACE::MODULO,           IDENTIFIER::INTERFACE::MODULO },
			{ INTERFACE::POWER,            IDENTIFIER::INTERFACE::POWER },
			{ INTERFACE::MODULUS,          IDENTIFIER::INTERFACE::MODULUS },
			{ INTERFACE::NEGATE,           IDENTIFIER::INTERFACE::NEGATE },
			{ INTERFACE::CODE,             IDENTIFIER::INTERFACE::CODE },
			{ INTERFACE::TO_STRING,        IDENTIFIER::INTERFACE::TO_STRING },
			{ INTERFACE::TO_INTEGER,       IDENTIFIER::INTERFACE::TO_INTEGER },
			{ INTERFACE::TO_REAL,          IDENTIFIER::INTERFACE::TO_REAL },
			{ INTERFACE::TO_NUMBER,        IDENTIFIER::INTERFACE::TO_NUMBER },
			{ INTERFACE::TO_CHARACTER,     IDENTIFIER::INTERFACE::TO_CHARACTER },
			{ INTERFACE::TO_BOOLEAN,       IDENTIFIER::INTERFACE::TO_BOOLEAN },
			{ STANDARD_FUNCTIONS::MAIN,    IDENTIFIER::STANDARD_FUNCTIONS::MAIN },
			{ BUILTIN::TYPE_UNKNOWN,       IDENTIFIER::BUILTIN::TYPE_UNKNOWN }
		} )
	, _identifierNames( {
			KEYWORD::CONSTRUCTOR,
			KEYWORD::DESTRUCTOR,
			KEYWORD::THIS,
			KEYWORD::SUPER,
			KEYWORD::ASSERT,
			BUILTIN::INTEGER,
			BUILTIN::REAL,
			BUILTIN::NUMBER,
			BUILTIN::STRING,
			BUILTIN::CHARACTER,
			BUILTIN::BOOLEAN,
			BUILTIN::TUPLE,
			BUILTIN::LIST,
			BUILTIN::DEQUE,
			BUILTIN::DICT,
			BUILTIN::LOOKUP,
			BUILTIN::ORDER,
			BUILTIN::SET,
			BUILTIN::HEAP,
			BUILTIN::BLOB,
			BUILTIN::TYPE_NONE,
			BUILTIN::TYPE_OBSERVER,
			BUILTIN::TYPE_REFERENCE,
			BUILTIN::TYPE_FUNCTION_REFERENCE,
			BUILTIN::TYPE_OBJECT_REFERENCE,
			BUILTIN::TYPE_METHOD,
			BUILTIN::TYPE_UNBOUND_METHOD,
			BUILTIN::TYPE_BOUND_METHOD,
			BUILTIN::TYPE_VARIADIC_PARAMETERS,
			BUILTIN::TYPE_NAMED_PARAMETERS,
			BUILTIN::SIZE,
			BUILTIN::TYPE,
			BUILTIN::COPY,
			BUILTIN::OBSERVE,
			BUILTIN::USE,
			BUILTIN::SQUARE_ROOT,
			BUILTIN::N_ARY_SUMMATION,
			BUILTIN::N_ARY_PRODUCT,
			INTERFACE::CLONE,
			INTERFACE::GET_SIZE,
			INTERFACE::SUBSCRIPT,
			INTERFACE::SET_SUBSCRIPT,
			INTERFACE::MEMBER,
			INTERFACE::SET_MEMBER,
			INTERFACE::ITERATOR,
			INTERFACE::IS_VALID,
			INTERFACE::NEXT,
			INTERFACE::VALUE,
			INTERFACE::CALL,
			INTERFACE::HASH,
			INTERFACE::EQUALS,
			INTERFACE::LESS,
			INTERFACE::GREATER,
			INTERFACE::LESS_OR_EQUAL,
			INTERFACE::GREATER_OR_EQUAL,
			INTERFACE::CONTAINS,
			INTERFACE::ADD,
			INTERFACE::SUBTRACT,
			INTERFACE::MULTIPLY,
			INTERFACE::DIVIDE,
			INTERFACE::MODULO,
			INTERFACE::POWER,
			INTERFACE::MODULUS,
			INTERFACE::NEGATE,
			INTERFACE::CODE,
			INTERFACE::TO_STRING,
			INTERFACE::TO_INTEGER,
			INTERFACE::TO_REAL,
			INTERFACE::TO_NUMBER,
			INTERFACE::TO_CHARACTER,
			INTERFACE::TO_BOOLEAN,
			STANDARD_FUNCTIONS::MAIN,
			BUILTIN::TYPE_UNKNOWN
		} )
	, _objectFactory( make_pointer<HObjectFactory>( this ) )
	, _none( &_objectFactory->none_value() )
	, _true( &_objectFactory->true_value() )
	, _false( &_objectFactory->false_value() )
	, _threads()
	, _dependencies()
	, _functionIds()
	, _classes()
	, _valuesStore( make_pointer<values_store_t>() )
	, _globalDefinitions()
	, _argv( _objectFactory->create_list() )
	, _result( _objectFactory->none_value() )
	, _incrementalFrame()
	, _maxLocalVariableCount( 0 )
	, _maxCallStackSize( _huginnMaxCallStack_ )
	, _modulePaths()
	, _compilerSetup( HHuginn::COMPILER::DEFAULT )
	, _trace() {
}

HRuntime::~HRuntime( void ) {
	M_PROLOG
	finalize();
	return;
	M_DESTRUCTOR_EPILOG
}

void HRuntime::finalize( void ) {
	M_PROLOG
	if ( ! _objectFactory.unique() ) {
		return;
	}
	if ( ! _result ) {
		return;
	}
	_incrementalFrame.reset();
	HObjectFactory::long_lived_t longLived;
	for ( class_t const& c : _dependencies ) {
		for ( HHuginn::value_t const& f : c->field_definitions() ) {
			longLived.insert( f.raw() );
		}
	}
	for ( HHuginn::value_t const& v : *_valuesStore ) {
		longLived.insert( v.raw() );
	}
	_result.reset();
	_argv.reset();
	util::HScopeExitCall sec( hcore::call( &threads_t::clear, &_threads ) );
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	_objectFactory->cleanup( longLived );
	_trace = t->trace();
	t->flush_runtime_exception();
	return;
	M_EPILOG
}

void HRuntime::reset( void ) {
	M_PROLOG
	_result.reset();
	static_cast<huginn::HList*>( _argv.raw() )->clear();
	_threads.clear();
	return;
	M_EPILOG
}

void HRuntime::copy_text( HRuntime& source_ ) {
	M_PROLOG
	M_ASSERT( &source_ != this );
	bool submoduleContext( _globalDefinitions.at( IDENTIFIER::KEYWORD::ASSERT ) != source_._globalDefinitions.at( IDENTIFIER::KEYWORD::ASSERT ) );
	_idGenerator = max( _idGenerator, source_._idGenerator );
	_identifierIds = source_._identifierIds;
	_identifierNames = source_._identifierNames;
	_incrementalFrame = source_._incrementalFrame;
	/*
	 * Order matters.
	 * Must be the same as order of destruction from HRuntime class destructor.
	 */
	_result = source_._result;
	_incrementalFrame = source_._incrementalFrame;
	_argv = source_._argv;
	_valuesStore = source_._valuesStore;
	if ( submoduleContext ) {
		_classes.clear();
	}
	_functionIds = source_._functionIds;
	_dependencies = source_._dependencies;
	_threads = source_._threads;
	_true = source_._true;
	_false = source_._false;
	_none = source_._none;
	_objectFactory = source_._objectFactory;
	_maxLocalVariableCount = max( _maxLocalVariableCount, source_._maxLocalVariableCount );
	_maxCallStackSize = source_._maxCallStackSize;
	_modulePaths = source_._modulePaths;
	_compilerSetup = source_._compilerSetup;
	_trace = source_._trace;
	fix_references();
	/* Update global references in sub-module. */
	if ( submoduleContext ) {
		for ( global_definitions_t::value_type& gc : _globalDefinitions ) {
			gc.second = source_._globalDefinitions.at( gc.first );
			class_t c( source_.get_class( gc.first ) );
			if ( !! c ) {
				_classes.insert( make_pair( gc.first, c ) );
			}
		}
	}
	return;
	M_EPILOG
}

void HRuntime::fix_references( void ) {
	M_PROLOG
	for ( dependencies_t::value_type& c : _dependencies ) {
		_huginn->register_class( c );
		c->update_runtime( this );
	}
	return;
	M_EPILOG
}

huginn::HThread* HRuntime::current_thread( void ) {
	M_PROLOG
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	threads_t::iterator t( _threads.find( threadId ) );
	return ( t != _threads.end() ? t->second.raw() : nullptr );
	M_EPILOG
}

int HRuntime::file_id( void ) {
	M_PROLOG
	huginn::HThread* t( current_thread() );
	return ( t ? t->current_frame()->file_id() : _huginn->file_id() );
	M_EPILOG
}

HHuginn::value_t HRuntime::result( void ) const {
	return ( _result );
}

HHuginn::value_t const* HRuntime::get_global( identifier_id_t identifierId_ ) const {
	M_PROLOG
	global_definitions_t::const_iterator it( _globalDefinitions.find( identifierId_ ) );
	return ( ( it != _globalDefinitions.end() ) ? it->second : nullptr );
	M_EPILOG
}

HHuginn::class_t HRuntime::get_class( identifier_id_t identifierId_ ) const {
	M_PROLOG
	classes_t::const_iterator ci( _classes.find( identifierId_ ) );
	return ( ci != _classes.end() ? ci->second : class_t() );
	M_EPILOG
}

HClass const* HRuntime::get_class( void const* id_ ) const {
	M_PROLOG
	function_ids_t::const_iterator it( _functionIds.find( id_ ) );
	return ( it != _functionIds.end() ? static_cast<HClass const*>( id_ ) : nullptr );
	M_EPILOG
}

void HRuntime::set_max_local_variable_count( int maxLocalVariableCount_ ) {
	M_PROLOG
	M_ASSERT( maxLocalVariableCount_ >= 0 );
	_maxLocalVariableCount = maxLocalVariableCount_;
	return;
	M_EPILOG
}

void HRuntime::set_max_call_stack_size( int maxCallStackSize_ ) {
	M_PROLOG
	static int const MIN_CALL_STACK_SIZE( 128 );
	if ( maxCallStackSize_ < MIN_CALL_STACK_SIZE ) {
		throw HHuginnException( "Invalid max call stack size: "_ys.append( maxCallStackSize_ ) );
	}
	_maxCallStackSize = maxCallStackSize_;
	return;
	M_EPILOG
}

void HRuntime::register_class( class_t class_, HHuginn::VISIBILITY classConstructorVisibility_ ) {
	M_PROLOG
	if ( _functionIds.insert( class_.raw() ).second ) {
		_dependencies.push_back( class_ );
	}
	/*
	 * Classes from submodules shall not be added to global classes set
	 * so that it is possible to have classes with _the same name_ in different
	 * submodules and (one) in global scope.
	 */
	if ( classConstructorVisibility_ == HHuginn::VISIBILITY::GLOBAL ) {
		_classes.insert( make_pair( class_->identifier_id(), class_ ) );
		if ( ! is_enum_class( class_.raw() ) ) {
			_globalDefinitions.insert( make_pair( class_->identifier_id(), &class_->constructor() ) );
		}
	}
	class_->finalize_registration( this );
	return;
	M_EPILOG
}

void HRuntime::drop_class( identifier_id_t identifier_ ) {
	M_PROLOG
	classes_t::iterator it( _classes.find( identifier_ ) );
	if ( it != _classes.end() ) {
		HHuginn::type_id_t t( it->second->type_id() );
		_functionIds.erase( it->second.raw() );
		_globalDefinitions.erase( identifier_ );
		_dependencies.erase(
			yaal::remove_if(
				_dependencies.begin(),
				_dependencies.end(),
				[&t]( class_t const& class_ ) {
					return ( class_->type_id() == t );
				}
			),
			_dependencies.end()
		);
		_classes.erase( it );
	}
	return;
	M_EPILOG
}

void HRuntime::register_function( identifier_id_t identifier_, function_t function_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	register_global( identifier_, _objectFactory->create_function_reference( identifier_, function_, doc_ ) );
	return;
	M_EPILOG
}

void HRuntime::drop_global( identifier_id_t identifier_ ) {
	M_PROLOG
	global_definitions_t::iterator gcIt( _globalDefinitions.find( identifier_ ) );
	if ( gcIt != _globalDefinitions.end() ) {
		for ( values_store_t::iterator it( _valuesStore->begin() ), end( _valuesStore->end() ); it != end; ++ it ) {
			if ( it->raw() == gcIt->second->raw() ) {
				_valuesStore->erase( it );
				break;
			}
		}
		_globalDefinitions.erase( gcIt );
	}
	return;
	M_EPILOG
}

void HRuntime::register_global( identifier_id_t identifier_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	global_definitions_t::iterator gcIt( _globalDefinitions.find( identifier_ ) );
	if ( gcIt != _globalDefinitions.end() ) {
		for ( values_store_t::iterator it( _valuesStore->begin() ), end( _valuesStore->end() ); it != end; ++ it ) {
			if ( it->raw() == gcIt->second->raw() ) {
				*it = value_;
				break;
			}
		}
	} else {
		_valuesStore->push_back( value_ );
		_globalDefinitions.insert( make_pair( identifier_, &_valuesStore->back() ) );
	}
	return;
	M_EPILOG
}

HHuginn::value_t HRuntime::find_package( identifier_id_t identifier_ ) const {
	M_PROLOG
	HHuginn::value_t package;
	for ( values_store_t::value_type const& v : *_valuesStore ) {
		if ( v->get_class()->identifier_id() == identifier_ ) {
			package = v;
			break;
		}
	}
	return ( package );
	M_EPILOG
}

void HRuntime::register_package(
	identifier_id_t package_,
	identifier_id_t alias_,
	int position_
) {
	M_PROLOG
	HHuginn::value_t package( find_package( package_ ) );
	if ( ! package ) {
		package = HPackageFactory::get_instance().create_package( this, identifier_name( package_ ), HHuginn::VISIBILITY::PACKAGE, position_ );
	}
	register_global( alias_, package );
	return;
	M_EPILOG
}

namespace {

HHuginn::value_t bound_method_call(
	HClass::HMethod* method_,
	HHuginn::value_t subject_,
	huginn::HThread* thread_,
	HHuginn::value_t*,
	HHuginn::values_t& values_,
	int position_
) {
	return ( method_->function()( thread_, &subject_, values_, position_ ) );
}

}

void HRuntime::import_symbols( identifier_id_t package_, HHuginn::identifiers_t const& importedSymbols_, int position_ ) {
	M_PROLOG
	HHuginn::value_t package( find_package( package_ ) );
	bool firstImported( false );
	if ( ! package ) {
		package = HPackageFactory::get_instance().create_package( this, identifier_name( package_ ), HHuginn::VISIBILITY::HIDDEN, position_ );
		firstImported = true;
	}
	HClass const* packageClass( package->get_class() );
	HHuginn::identifiers_t const* importedSymbols(
		! importedSymbols_.is_empty()
			? &importedSymbols_
			: &packageClass->field_identifiers()
	);
	for ( HHuginn::identifier_id_t id : *importedSymbols ) {
		int idx( package->field_index( id ) );
		if ( idx < 0 ) {
			throw HHuginn::HHuginnRuntimeException(
				"Symbol `"_ys
					.append( identifier_name( id ) )
					.append( "` does not exist in `" )
					.append( identifier_name( package_ ) )
					.append( "` package." ),
				MAIN_FILE_ID,
				position_
			);
		}
		HHuginn::value_t v( package->field( idx ) );
		HHuginn::type_id_t t( v->type_id() );
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			register_global( id, v );
		} else if ( t == HHuginn::TYPE::METHOD ) {
			HHuginn::value_t f(
				_objectFactory->create_function_reference(
					id,
					hcore::call(
						&bound_method_call,
						static_cast<HClass::HMethod*>( v.raw() ),
						package,
						_1, _2, _3, _4
					),
					packageClass->doc( id )
				)
			);
			register_global( id, f );
		} else if ( is_enum_class( v ) ) {
			register_global( id, v );
		} else if ( type_tag( v->type_id() ) < HHuginn::TYPE::UNKNOWN ) {
			register_global( id, v );
		}
	}
	if ( firstImported ) {
		_valuesStore->push_back( package );
	}
	return;
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class(
	identifier_id_t identifier_,
	yaal::hcore::HString const& doc_,
	HHuginn::ACCESS access_,
	HClass::TYPE type_,
	HClass const* origin_,
	HClass::create_instance_t createInstance_
) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HClass>(
			this,
			type_id_t( new_type_id() ),
			identifier_,
			doc_,
			access_,
			type_,
			origin_,
			createInstance_
		)
	);
	return ( c );
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class(
	yaal::hcore::HString const& name_,
	yaal::hcore::HString const& doc_,
	HHuginn::ACCESS access_,
	HClass::TYPE type_,
	HClass const* origin_,
	HClass::create_instance_t createInstance_
) {
	M_PROLOG
	return (
		create_class( identifier_id( name_ ), doc_, access_, type_, origin_, createInstance_ )
	);
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( class_constructor_t const& classConstructor_ ) {
	M_PROLOG
	class_t c( classConstructor_( new_type_id() ) );
	return ( c );
	M_EPILOG
}

HHuginn::identifier_id_t HRuntime::try_identifier_id( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	M_ASSERT( _identifierIds.get_size() == _identifierNames.get_size() );
	identifier_ids_t::const_iterator it( _identifierIds.find( name_ ) );
	return ( it != _identifierIds.end() ? it->second : IDENTIFIER::INVALID );
	M_EPILOG
}

HRuntime::type_id_t HRuntime::new_type_id( void ) {
	M_PROLOG
	type_id_t tid( _idGenerator );
	++ _idGenerator;
	return ( tid );
	M_EPILOG
}

HHuginn::identifier_id_t HRuntime::identifier_id( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	M_ASSERT( _identifierIds.get_size() == _identifierNames.get_size() );
	identifier_id_t id( try_identifier_id( name_ ) );
	if ( id == IDENTIFIER::INVALID ) {
		id = identifier_id_t( static_cast<identifier_id_t::value_type>( _identifierIds.get_size() ) );
		_identifierIds.insert( make_pair( name_, id ) );
		_identifierNames.emplace_back( name_ );
	}
	M_ASSERT( _identifierIds.get_size() == _identifierNames.get_size() );
	return ( id );
	M_EPILOG
}

yaal::hcore::HString const& HRuntime::identifier_name( identifier_id_t id_ ) const {
	M_PROLOG
	static yaal::hcore::HString const INVALID_NAME_IDENTIFIER = "*invalid_identifier*";
	return ( id_ != IDENTIFIER::INVALID ? _identifierNames[id_.get()] : INVALID_NAME_IDENTIFIER );
	M_EPILOG
}

void HRuntime::execute( void ) {
	M_PROLOG
	util::HScopeExitCall sec( hcore::call( &threads_t::clear, &_threads ) );
	HHuginn::values_t args;
	args.reserve( max_local_variable_count() );
	huginn::HList& argv( *static_cast<huginn::HList*>( _argv.raw() ) );
	if ( argv.size( nullptr, 0 ) > 0 ) {
		args.push_back( _argv );
	}
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	_result = call( IDENTIFIER::STANDARD_FUNCTIONS::MAIN, args, 0 );
	_trace = t->trace();
	t->flush_runtime_exception();
	return;
	M_EPILOG
}

HHuginn::class_t HRuntime::make_package( yaal::hcore::HString const& name_, HRuntime const& context_ ) {
	M_PROLOG
	hcore::HString doc( _huginn->get_comment( 1 ) );
	HHuginn::class_t cls( create_class( name_, ! doc.is_empty() ? doc : "The `"_ys.append( name_ ).append( "` is an user defined submodule." ), HHuginn::ACCESS::PRIVATE ) );
	HHuginn::field_definitions_t fds;
	for ( classes_t::value_type const& c : _classes ) {
		if (
			! is_enum_class( c.second.raw() )
			&& ( context_._classes.count( c.first ) == 0 )
			&& ! c.second->origin()
			&& ! context_.find_package( c.first )
		) {
			fds.emplace_back(
				identifier_name( c.first ),
				create_method( &package::instance, c.second.raw() ),
				"access class "_ys.append( identifier_name( c.first ) ).append( " imported in submodule" )
			);
		}
	}
	typedef HHashSet<HHuginn::type_id_t> type_set_t;
	type_set_t previouslyImportedPackages;
	for ( global_definitions_t::value_type const& gd : context_._globalDefinitions ) {
		previouslyImportedPackages.insert( (*gd.second)->type_id() );
	}
	for ( global_definitions_t::value_type const& gd : _globalDefinitions ) {
		bool isEnum( is_enum_class( gd.second ) );
		if (
			( gd.first < IDENTIFIER::STANDARD_FUNCTIONS::MAIN )
			|| ( ! isEnum && ( _classes.count( gd.first ) > 0 ) )
			|| is_restricted( identifier_name( gd.first ) )
		) {
			continue;
		}
		if ( (*gd.second)->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			huginn::HFunctionReference const* fr( static_cast<huginn::HFunctionReference const*>( gd.second->raw() ) );
			fds.emplace_back(
				identifier_name( gd.first ),
				_objectFactory->create_method_raw( fr->function() ),
				! fr->doc().is_empty() ? fr->doc() : "access function "_ys.append( identifier_name( gd.first ) ).append( " imported in submodule" )
			);
		} else if ( isEnum ) {
			fds.emplace_back(
				identifier_name( gd.first ),
				*gd.second,
				"access enum "_ys.append( identifier_name( gd.first ) ).append( " imported in submodule" )
			);
		} else if ( !! find_package( (*gd.second)->get_class()->identifier_id() ) && ( previouslyImportedPackages.count( (*gd.second)->type_id() ) == 0 ) ) {
			fds.emplace_back(
				identifier_name( gd.first ),
				create_method( &package::value, *gd.second, identifier_name( gd.first ) ),
				"access package "_ys.append( (*gd.second)->get_class()->name() ).append( " imported in submodule" )
			);
		}
	}
	for ( classes_t::value_type& m : _classes ) {
		if ( ! m.second->origin() && ( context_._classes.find( m.first ) == context_._classes.end() )) {
			m.second->set_origin( cls.raw() );
		}
	}
	cls->redefine( nullptr, fds );
	return ( cls );
	M_EPILOG
}

void HRuntime::set_incremental_frame( huginn::HThread::frame_t const& frame_ ) {
	M_PROLOG
	_incrementalFrame = frame_;
	return;
	M_EPILOG
}

huginn::HThread::frame_t const& HRuntime::incremental_frame( void ) const {
	M_PROLOG
	return ( _incrementalFrame );
	M_EPILOG
}

HHuginn::value_t HRuntime::call( yaal::hcore::HString const& name_, HHuginn::values_t const& values_ ) {
	M_PROLOG
	HHuginn::identifier_id_t identifier( try_identifier_id( name_ ) );
	if ( identifier == IDENTIFIER::INVALID ) {
		throw HHuginn::HHuginnRuntimeException( "Symbol `"_ys.append( name_ ).append( "` is not defined." ), MAIN_FILE_ID, 0 );
	}
	util::HScopeExitCall sec( hcore::call( &threads_t::clear, &_threads ) );
	HHuginn::values_t args( values_ );
	args.reserve( max_local_variable_count() );
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	_result = call( identifier, args, 0 );
	_trace = t->trace();
	t->flush_runtime_exception();
	return ( _result );
	M_EPILOG
}

HHuginn::value_t HRuntime::call( identifier_id_t identifier_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	value_t res;
	global_definitions_t::iterator gcIt( _globalDefinitions.find( identifier_ ) );
	if ( gcIt != _globalDefinitions.end() ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		if ( (*gcIt->second)->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			res = static_cast<huginn::HFunctionReference const*>( gcIt->second->raw() )->function()( t->second.raw(), nullptr, values_, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "Symbol `"_ys.append( identifier_name( identifier_ ) ).append( "` is not callable." ), MAIN_FILE_ID, position_ );
		}
	} else {
		throw HHuginn::HHuginnRuntimeException( "Function `"_ys.append( identifier_name( identifier_ ) ).append( "(...)` is not defined." ), MAIN_FILE_ID, position_ );
	}
	return ( res );
	M_EPILOG
}

void HRuntime::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	huginn::HList& argv( *static_cast<huginn::HList*>( _argv.raw() ) );
	argv.push_back( _objectFactory->create_string( arg_ ) );
	return;
	M_EPILOG
}

void HRuntime::add_argument( HHuginn::value_t const& arg_ ) {
	M_PROLOG
	huginn::HList& argv( *static_cast<huginn::HList*>( _argv.raw() ) );
	argv.push_back( arg_ );
	return;
	M_EPILOG
}

void HRuntime::clear_arguments( void ) {
	M_PROLOG
	huginn::HList& argv( *static_cast<huginn::HList*>( _argv.raw() ) );
	argv.clear();
	return;
	M_EPILOG
}

void HRuntime::register_builtin_function( yaal::hcore::HString const& name_, function_t&& function_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	identifier_id_t id( identifier_id( name_ ) );
	_huginn->register_function( id );
	HHuginn::value_t f( _objectFactory->create_function_reference( id, yaal::move( function_ ), doc_ ) );
	_valuesStore->push_back( f );
	_globalDefinitions.insert( make_pair( id, &_valuesStore->back() ) );
	return;
	M_EPILOG
}

void HRuntime::register_builtins( void ) {
	M_PROLOG
	M_ENSURE( ( identifier_id( KEYWORD::CONSTRUCTOR ) == IDENTIFIER::KEYWORD::CONSTRUCTOR ) && ( identifier_name( IDENTIFIER::KEYWORD::CONSTRUCTOR ) == KEYWORD::CONSTRUCTOR ) );
	M_ENSURE( ( identifier_id( KEYWORD::DESTRUCTOR ) == IDENTIFIER::KEYWORD::DESTRUCTOR ) && ( identifier_name( IDENTIFIER::KEYWORD::DESTRUCTOR ) == KEYWORD::DESTRUCTOR ) );
	M_ENSURE( ( identifier_id( KEYWORD::THIS ) == IDENTIFIER::KEYWORD::THIS ) && ( identifier_name( IDENTIFIER::KEYWORD::THIS ) == KEYWORD::THIS ) );
	M_ENSURE( ( identifier_id( KEYWORD::SUPER ) == IDENTIFIER::KEYWORD::SUPER ) && ( identifier_name( IDENTIFIER::KEYWORD::SUPER ) == KEYWORD::SUPER ) );
	M_ENSURE( ( identifier_id( KEYWORD::ASSERT ) == IDENTIFIER::KEYWORD::ASSERT ) && ( identifier_name( IDENTIFIER::KEYWORD::ASSERT ) == KEYWORD::ASSERT ) );
	M_ENSURE( ( identifier_id( BUILTIN::INTEGER ) == IDENTIFIER::BUILTIN::INTEGER ) && ( identifier_name( IDENTIFIER::BUILTIN::INTEGER ) == BUILTIN::INTEGER ) );
	M_ENSURE( ( identifier_id( BUILTIN::REAL ) == IDENTIFIER::BUILTIN::REAL ) && ( identifier_name( IDENTIFIER::BUILTIN::REAL ) == BUILTIN::REAL ) );
	M_ENSURE( ( identifier_id( BUILTIN::NUMBER ) == IDENTIFIER::BUILTIN::NUMBER ) && ( identifier_name( IDENTIFIER::BUILTIN::NUMBER ) == BUILTIN::NUMBER ) );
	M_ENSURE( ( identifier_id( BUILTIN::STRING ) == IDENTIFIER::BUILTIN::STRING ) && ( identifier_name( IDENTIFIER::BUILTIN::STRING ) == BUILTIN::STRING ) );
	M_ENSURE( ( identifier_id( BUILTIN::CHARACTER ) == IDENTIFIER::BUILTIN::CHARACTER ) && ( identifier_name( IDENTIFIER::BUILTIN::CHARACTER ) == BUILTIN::CHARACTER ) );
	M_ENSURE( ( identifier_id( BUILTIN::BOOLEAN ) == IDENTIFIER::BUILTIN::BOOLEAN ) && ( identifier_name( IDENTIFIER::BUILTIN::BOOLEAN ) == BUILTIN::BOOLEAN ) );
	M_ENSURE( ( identifier_id( BUILTIN::TUPLE ) == IDENTIFIER::BUILTIN::TUPLE ) && ( identifier_name( IDENTIFIER::BUILTIN::TUPLE ) == BUILTIN::TUPLE ) );
	M_ENSURE( ( identifier_id( BUILTIN::LIST ) == IDENTIFIER::BUILTIN::LIST ) && ( identifier_name( IDENTIFIER::BUILTIN::LIST ) == BUILTIN::LIST ) );
	M_ENSURE( ( identifier_id( BUILTIN::DEQUE ) == IDENTIFIER::BUILTIN::DEQUE ) && ( identifier_name( IDENTIFIER::BUILTIN::DEQUE ) == BUILTIN::DEQUE ) );
	M_ENSURE( ( identifier_id( BUILTIN::DICT ) == IDENTIFIER::BUILTIN::DICT ) && ( identifier_name( IDENTIFIER::BUILTIN::DICT ) == BUILTIN::DICT ) );
	M_ENSURE( ( identifier_id( BUILTIN::LOOKUP ) == IDENTIFIER::BUILTIN::LOOKUP ) && ( identifier_name( IDENTIFIER::BUILTIN::LOOKUP ) == BUILTIN::LOOKUP ) );
	M_ENSURE( ( identifier_id( BUILTIN::ORDER ) == IDENTIFIER::BUILTIN::ORDER ) && ( identifier_name( IDENTIFIER::BUILTIN::ORDER ) == BUILTIN::ORDER ) );
	M_ENSURE( ( identifier_id( BUILTIN::SET ) == IDENTIFIER::BUILTIN::SET ) && ( identifier_name( IDENTIFIER::BUILTIN::SET ) == BUILTIN::SET ) );
	M_ENSURE( ( identifier_id( BUILTIN::HEAP ) == IDENTIFIER::BUILTIN::HEAP ) && ( identifier_name( IDENTIFIER::BUILTIN::HEAP ) == BUILTIN::HEAP ) );
	M_ENSURE( ( identifier_id( BUILTIN::BLOB ) == IDENTIFIER::BUILTIN::BLOB ) && ( identifier_name( IDENTIFIER::BUILTIN::BLOB ) == BUILTIN::BLOB ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_NONE ) == IDENTIFIER::BUILTIN::TYPE_NONE ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_NONE ) == BUILTIN::TYPE_NONE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_OBSERVER ) == IDENTIFIER::BUILTIN::TYPE_OBSERVER ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_OBSERVER ) == BUILTIN::TYPE_OBSERVER ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_REFERENCE ) == IDENTIFIER::BUILTIN::TYPE_REFERENCE ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_REFERENCE ) == BUILTIN::TYPE_REFERENCE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_FUNCTION_REFERENCE ) == IDENTIFIER::BUILTIN::TYPE_FUNCTION_REFERENCE ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_FUNCTION_REFERENCE ) == BUILTIN::TYPE_FUNCTION_REFERENCE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_OBJECT_REFERENCE ) == IDENTIFIER::BUILTIN::TYPE_OBJECT_REFERENCE ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_OBJECT_REFERENCE ) == BUILTIN::TYPE_OBJECT_REFERENCE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_METHOD ) == IDENTIFIER::BUILTIN::TYPE_METHOD ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_METHOD ) == BUILTIN::TYPE_METHOD ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_UNBOUND_METHOD ) == IDENTIFIER::BUILTIN::TYPE_UNBOUND_METHOD ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_UNBOUND_METHOD ) == BUILTIN::TYPE_UNBOUND_METHOD ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_BOUND_METHOD ) == IDENTIFIER::BUILTIN::TYPE_BOUND_METHOD ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_BOUND_METHOD ) == BUILTIN::TYPE_BOUND_METHOD ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_VARIADIC_PARAMETERS ) == IDENTIFIER::BUILTIN::TYPE_VARIADIC_PARAMETERS ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_VARIADIC_PARAMETERS ) == BUILTIN::TYPE_VARIADIC_PARAMETERS ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_NAMED_PARAMETERS ) == IDENTIFIER::BUILTIN::TYPE_NAMED_PARAMETERS ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_NAMED_PARAMETERS ) == BUILTIN::TYPE_NAMED_PARAMETERS ) );
	M_ENSURE( ( identifier_id( BUILTIN::SIZE ) == IDENTIFIER::BUILTIN::SIZE ) && ( identifier_name( IDENTIFIER::BUILTIN::SIZE ) == BUILTIN::SIZE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE ) == IDENTIFIER::BUILTIN::TYPE ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE ) == BUILTIN::TYPE ) );
	M_ENSURE( ( identifier_id( BUILTIN::COPY ) == IDENTIFIER::BUILTIN::COPY ) && ( identifier_name( IDENTIFIER::BUILTIN::COPY ) == BUILTIN::COPY ) );
	M_ENSURE( ( identifier_id( BUILTIN::OBSERVE ) == IDENTIFIER::BUILTIN::OBSERVE ) && ( identifier_name( IDENTIFIER::BUILTIN::OBSERVE ) == BUILTIN::OBSERVE ) );
	M_ENSURE( ( identifier_id( BUILTIN::USE ) == IDENTIFIER::BUILTIN::USE ) && ( identifier_name( IDENTIFIER::BUILTIN::USE ) == BUILTIN::USE ) );
	M_ENSURE( ( identifier_id( BUILTIN::SQUARE_ROOT ) == IDENTIFIER::BUILTIN::SQUARE_ROOT ) && ( identifier_name( IDENTIFIER::BUILTIN::SQUARE_ROOT ) == BUILTIN::SQUARE_ROOT ) );
	M_ENSURE( ( identifier_id( BUILTIN::N_ARY_SUMMATION ) == IDENTIFIER::BUILTIN::N_ARY_SUMMATION ) && ( identifier_name( IDENTIFIER::BUILTIN::N_ARY_SUMMATION ) == BUILTIN::N_ARY_SUMMATION ) );
	M_ENSURE( ( identifier_id( BUILTIN::N_ARY_PRODUCT ) == IDENTIFIER::BUILTIN::N_ARY_PRODUCT ) && ( identifier_name( IDENTIFIER::BUILTIN::N_ARY_PRODUCT ) == BUILTIN::N_ARY_PRODUCT ) );
	M_ENSURE( ( identifier_id( INTERFACE::CLONE ) == IDENTIFIER::INTERFACE::CLONE ) && ( identifier_name( IDENTIFIER::INTERFACE::CLONE ) == INTERFACE::CLONE ) );
	M_ENSURE( ( identifier_id( INTERFACE::GET_SIZE ) == IDENTIFIER::INTERFACE::GET_SIZE ) && ( identifier_name( IDENTIFIER::INTERFACE::GET_SIZE ) == INTERFACE::GET_SIZE ) );
	M_ENSURE( ( identifier_id( INTERFACE::SUBSCRIPT ) == IDENTIFIER::INTERFACE::SUBSCRIPT ) && ( identifier_name( IDENTIFIER::INTERFACE::SUBSCRIPT ) == INTERFACE::SUBSCRIPT ) );
	M_ENSURE( ( identifier_id( INTERFACE::SET_SUBSCRIPT ) == IDENTIFIER::INTERFACE::SET_SUBSCRIPT ) && ( identifier_name( IDENTIFIER::INTERFACE::SET_SUBSCRIPT ) == INTERFACE::SET_SUBSCRIPT ) );
	M_ENSURE( ( identifier_id( INTERFACE::MEMBER ) == IDENTIFIER::INTERFACE::MEMBER ) && ( identifier_name( IDENTIFIER::INTERFACE::MEMBER ) == INTERFACE::MEMBER ) );
	M_ENSURE( ( identifier_id( INTERFACE::SET_MEMBER ) == IDENTIFIER::INTERFACE::SET_MEMBER ) && ( identifier_name( IDENTIFIER::INTERFACE::SET_MEMBER ) == INTERFACE::SET_MEMBER ) );
	M_ENSURE( ( identifier_id( INTERFACE::ITERATOR ) == IDENTIFIER::INTERFACE::ITERATOR ) && ( identifier_name( IDENTIFIER::INTERFACE::ITERATOR ) == INTERFACE::ITERATOR ) );
	M_ENSURE( ( identifier_id( INTERFACE::IS_VALID ) == IDENTIFIER::INTERFACE::IS_VALID ) && ( identifier_name( IDENTIFIER::INTERFACE::IS_VALID ) == INTERFACE::IS_VALID ) );
	M_ENSURE( ( identifier_id( INTERFACE::NEXT ) == IDENTIFIER::INTERFACE::NEXT ) && ( identifier_name( IDENTIFIER::INTERFACE::NEXT ) == INTERFACE::NEXT ) );
	M_ENSURE( ( identifier_id( INTERFACE::VALUE ) == IDENTIFIER::INTERFACE::VALUE ) && ( identifier_name( IDENTIFIER::INTERFACE::VALUE ) == INTERFACE::VALUE ) );
	M_ENSURE( ( identifier_id( INTERFACE::CALL ) == IDENTIFIER::INTERFACE::CALL ) && ( identifier_name( IDENTIFIER::INTERFACE::CALL ) == INTERFACE::CALL ) );
	M_ENSURE( ( identifier_id( INTERFACE::HASH ) == IDENTIFIER::INTERFACE::HASH ) && ( identifier_name( IDENTIFIER::INTERFACE::HASH ) == INTERFACE::HASH ) );
	M_ENSURE( ( identifier_id( INTERFACE::EQUALS ) == IDENTIFIER::INTERFACE::EQUALS ) && ( identifier_name( IDENTIFIER::INTERFACE::EQUALS ) == INTERFACE::EQUALS ) );
	M_ENSURE( ( identifier_id( INTERFACE::LESS ) == IDENTIFIER::INTERFACE::LESS ) && ( identifier_name( IDENTIFIER::INTERFACE::LESS ) == INTERFACE::LESS ) );
	M_ENSURE( ( identifier_id( INTERFACE::GREATER ) == IDENTIFIER::INTERFACE::GREATER ) && ( identifier_name( IDENTIFIER::INTERFACE::GREATER ) == INTERFACE::GREATER ) );
	M_ENSURE( ( identifier_id( INTERFACE::LESS_OR_EQUAL ) == IDENTIFIER::INTERFACE::LESS_OR_EQUAL ) && ( identifier_name( IDENTIFIER::INTERFACE::LESS_OR_EQUAL ) == INTERFACE::LESS_OR_EQUAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::GREATER_OR_EQUAL ) == IDENTIFIER::INTERFACE::GREATER_OR_EQUAL ) && ( identifier_name( IDENTIFIER::INTERFACE::GREATER_OR_EQUAL ) == INTERFACE::GREATER_OR_EQUAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::CONTAINS ) == IDENTIFIER::INTERFACE::CONTAINS ) && ( identifier_name( IDENTIFIER::INTERFACE::CONTAINS ) == INTERFACE::CONTAINS ) );
	M_ENSURE( ( identifier_id( INTERFACE::ADD ) == IDENTIFIER::INTERFACE::ADD ) && ( identifier_name( IDENTIFIER::INTERFACE::ADD ) == INTERFACE::ADD ) );
	M_ENSURE( ( identifier_id( INTERFACE::SUBTRACT ) == IDENTIFIER::INTERFACE::SUBTRACT ) && ( identifier_name( IDENTIFIER::INTERFACE::SUBTRACT ) == INTERFACE::SUBTRACT ) );
	M_ENSURE( ( identifier_id( INTERFACE::MULTIPLY ) == IDENTIFIER::INTERFACE::MULTIPLY ) && ( identifier_name( IDENTIFIER::INTERFACE::MULTIPLY ) == INTERFACE::MULTIPLY ) );
	M_ENSURE( ( identifier_id( INTERFACE::DIVIDE ) == IDENTIFIER::INTERFACE::DIVIDE ) && ( identifier_name( IDENTIFIER::INTERFACE::DIVIDE ) == INTERFACE::DIVIDE ) );
	M_ENSURE( ( identifier_id( INTERFACE::MODULO ) == IDENTIFIER::INTERFACE::MODULO ) && ( identifier_name( IDENTIFIER::INTERFACE::MODULO ) == INTERFACE::MODULO ) );
	M_ENSURE( ( identifier_id( INTERFACE::POWER ) == IDENTIFIER::INTERFACE::POWER ) && ( identifier_name( IDENTIFIER::INTERFACE::POWER ) == INTERFACE::POWER ) );
	M_ENSURE( ( identifier_id( INTERFACE::MODULUS ) == IDENTIFIER::INTERFACE::MODULUS ) && ( identifier_name( IDENTIFIER::INTERFACE::MODULUS ) == INTERFACE::MODULUS ) );
	M_ENSURE( ( identifier_id( INTERFACE::NEGATE ) == IDENTIFIER::INTERFACE::NEGATE ) && ( identifier_name( IDENTIFIER::INTERFACE::NEGATE ) == INTERFACE::NEGATE ) );
	M_ENSURE( ( identifier_id( INTERFACE::CODE ) == IDENTIFIER::INTERFACE::CODE ) && ( identifier_name( IDENTIFIER::INTERFACE::CODE ) == INTERFACE::CODE ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_STRING ) == IDENTIFIER::INTERFACE::TO_STRING ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_STRING ) == INTERFACE::TO_STRING ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_INTEGER ) == IDENTIFIER::INTERFACE::TO_INTEGER ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_INTEGER ) == INTERFACE::TO_INTEGER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_REAL ) == IDENTIFIER::INTERFACE::TO_REAL ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_REAL ) == INTERFACE::TO_REAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_NUMBER ) == IDENTIFIER::INTERFACE::TO_NUMBER ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_NUMBER ) == INTERFACE::TO_NUMBER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_CHARACTER ) == IDENTIFIER::INTERFACE::TO_CHARACTER ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_CHARACTER ) == INTERFACE::TO_CHARACTER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_BOOLEAN ) == IDENTIFIER::INTERFACE::TO_BOOLEAN ) && ( identifier_name( IDENTIFIER::INTERFACE::TO_BOOLEAN ) == INTERFACE::TO_BOOLEAN ) );
	M_ENSURE( ( identifier_id( STANDARD_FUNCTIONS::MAIN ) == IDENTIFIER::STANDARD_FUNCTIONS::MAIN ) && ( identifier_name( IDENTIFIER::STANDARD_FUNCTIONS::MAIN ) == STANDARD_FUNCTIONS::MAIN ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE_UNKNOWN ) == IDENTIFIER::BUILTIN::TYPE_UNKNOWN ) && ( identifier_name( IDENTIFIER::BUILTIN::TYPE_UNKNOWN ) == BUILTIN::TYPE_UNKNOWN ) );
	register_builtin_function( BUILTIN::SIZE, hcore::call( &builtin::size, _1, _2, _3, _4 ), "( *expr* ) - get a size of given expression *expr*, e.g: a number of elements in a collection" );
	register_builtin_function( BUILTIN::TYPE, hcore::call( &builtin::type, _1, _2, _3, _4 ), "( *expr* ) - get a type of given expression *expr*" );
	register_builtin_function( BUILTIN::COPY, hcore::call( &builtin::copy, _1, _2, _3, _4 ), "( *ref* ) - make a deep copy of a value given given by *ref*" );
	register_builtin_function( BUILTIN::OBSERVE, hcore::call( &builtin::observe, _1, _2, _3, _4 ), "( *ref* ) - create an `*observer*` for a value given by *ref*" );
	register_builtin_function( BUILTIN::USE, hcore::call( &builtin::use, _1, _2, _3, _4 ), "( *observer* ) - get a reference to a value from given *observer*" );
	register_builtin_function( BUILTIN::SQUARE_ROOT, hcore::call( &builtin::square_root, BUILTIN::SQUARE_ROOT, _1, _2, _3, _4 ), "( *value* ) - calculate square root of given *value*" );
	register_builtin_function( BUILTIN::N_ARY_SUMMATION, hcore::call( &builtin::n_ary_summation, _1, _2, _3, _4 ), "( *iterable* ) - calculate a sum of elements from *iterable*" );
	register_builtin_function( BUILTIN::N_ARY_PRODUCT, hcore::call( &builtin::n_ary_product, _1, _2, _3, _4 ), "( *iterable* ) - calculate a product of elements from *iterable*" );
	register_builtin_function( "print", hcore::call( &builtin::print, _1, _2, _3, _4 ), "( *str* ) - print a message given by *str* to interpreter's standard output" );
	register_builtin_function( "input", hcore::call( &builtin::input, _1, _2, _3, _4 ), "read a line of text from interpreter's standard input" );
	register_builtin_function( KEYWORD::ASSERT, hcore::call( &builtin::assert, _1, _2, _3, _4 ), "( *condition*[, *message*] ) - ensure *condition* is met or bailout with *message*" );
	_objectFactory->register_builtin_classes();
	return;
	M_EPILOG
}

namespace {

void sort_identifiers( HHuginn::identifiers_t& identifiers_, HRuntime const* runtime_ ) {
	sort(
		identifiers_.begin(),
		identifiers_.end(),
		[runtime_]( HHuginn::identifier_id_t const& l, HHuginn::identifier_id_t const& r ) {
			return ( runtime_->identifier_name( l ) < runtime_->identifier_name( r ) );
		}
	);
}

}

inline void dump_class_info( HRuntime const* runtime_, HClass const& huginnClass_, yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	stream_ << ( is_enum_class( &huginnClass_ ) ? "enum: " : "class: " );
	stream_ << ( ( huginnClass_.access() == HHuginn::ACCESS::PUBLIC ) ? "" : "-" );
	stream_ << full_class_name( *runtime_, &huginnClass_, false );
	if ( huginnClass_.super() ) {
		stream_ << " : " << huginnClass_.super()->name();
	}
	HHuginn::field_identifiers_t newFields( huginnClass_.field_identifiers() );
	typedef HStack<HClass const*> hierarchy_t;
	hierarchy_t hierarchy;
	HClass const* super( huginnClass_.super() );
	while ( super ) {
		hierarchy.push( super );
		super = super->super();
	}
	HHuginn::field_identifiers_t derivedFields;
	while ( ! hierarchy.is_empty() ) {
		for ( HHuginn::identifier_id_t f : hierarchy.top()->field_identifiers() ) {
			if ( find( derivedFields.begin(), derivedFields.end(), f ) == derivedFields.end() ) {
				derivedFields.push_back( f );
			}
		}
		hierarchy.pop();
	}
	HHuginn::field_identifiers_t overriddenFields;
	for ( HHuginn::identifier_id_t f : derivedFields ) {
		if ( find( newFields.begin(), newFields.end(), f ) != newFields.end() ) {
			overriddenFields.push_back( f );
		}
	}

	HHuginn::field_identifiers_t::iterator endDerived( derivedFields.end() );
	HHuginn::field_identifiers_t::iterator endNew( newFields.end() );
	for ( HHuginn::identifier_id_t f : overriddenFields ) {
		endDerived = remove( derivedFields.begin(), endDerived, f );
		endNew = remove( newFields.begin(), endNew, f );
	}
	newFields.erase( endNew, newFields.end() );
	derivedFields.erase( endDerived, derivedFields.end() );
	stream_ << " {";
	bool next( false );
	bool verbose( _debugLevel_ >= DEBUG_LEVEL::VERBOSE_MESSAGES );
	sort_identifiers( derivedFields, runtime_ );
	for ( HHuginn::identifier_id_t f : derivedFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime_->identifier_name( f ) << ( verbose ? "(derived)" : "" );
	}
	sort_identifiers( overriddenFields, runtime_ );
	for ( HHuginn::identifier_id_t f : overriddenFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime_->identifier_name( f ) << ( verbose ? "(overridden)" : "" );
	}
	sort_identifiers( newFields, runtime_ );
	for ( HHuginn::identifier_id_t f : newFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime_->identifier_name( f ) << ( verbose ? "(new)" : "" );
	}
	stream_ << ( next ? " " : "" ) << "}" << endl;
	return;
	M_EPILOG
}

namespace {

huginn::classes_t class_list( HRuntime::dependencies_t const& dependencies_ ) {
	M_PROLOG
	huginn::classes_t classes;
	for ( HRuntime::dependencies_t::value_type const& c : dependencies_ ) {
		classes.push_back( c.raw() );
	}
	sort(
		classes.begin(),
		classes.end(),
		[]( HClass const* l, HClass const* r ) {
			return ( l->name() < r->name() );
		}
	);
	classes.erase(
		unique(
			classes.begin(),
			classes.end(),
			[]( HClass const* l, HClass const* r ) {
				return ( l->type_id() == r->type_id() );
			}
		),
		classes.end()
	);
	return ( classes );
	M_EPILOG
}

}

void HRuntime::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _huginn->source_name( MAIN_FILE_ID ) << "`" << endl;
	for ( global_definitions_t::value_type const& gd : _globalDefinitions ) {
		HHuginn::value_t const& v( *gd.second );
		if ( ( v->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) || is_enum_class( v ) ) {
			continue;
		}
		stream_ << "package: " << identifier_name( gd.first ) << " = " << v->get_class()->name() << endl;
	}
	huginn::classes_t classes( class_list( _dependencies ) );
	for ( HClass const* c : classes ) {
		if ( ! is_enum_class( c ) ) {
			dump_class_info( this, *c, stream_ );
		}
	}
	for ( HClass const* c : classes ) {
		if ( is_enum_class( c ) ) {
			dump_class_info( this, *c, stream_ );
		}
	}
	HHuginn::identifiers_t identifiers;
	for ( global_definitions_t::value_type const& f : _globalDefinitions ) {
		if ( ( (*f.second)->type_id() != HHuginn::TYPE::FUNCTION_REFERENCE ) || !! get_class( f.first ) ) {
			continue;
		}
		identifiers.push_back( f.first );
	}
	sort_identifiers( identifiers, this );
	for ( HHuginn::identifier_id_t const& id : identifiers ) {
		yaal::hcore::HString const& name( identifier_name( id ) );
		stream_ << "function: " << name;
		if ( _debugLevel_ >= DEBUG_LEVEL::VERBOSE_MESSAGES ) {
			if ( _builtin_.count( name ) > 0 ) {
				stream_ <<" (builtin)";
			}
			if ( _standardLibrary_.count( name ) > 0 ) {
				stream_ <<" (standard library)";
			}
		}
		stream_ << endl;
	}
	return;
	M_EPILOG
}

void HRuntime::dump_docs( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	huginn::classes_t classes( class_list( _dependencies ) );
	for ( huginn::classes_t::value_type const& c : classes ) {
		HClass const& cls( *c );
		hcore::HString cn( full_class_name( *this, &cls, false ) );
		if ( ! cls.doc().is_empty() ) {
			stream_ << cn << ":" << cls.doc() << endl;
		}
		for ( HHuginn::field_identifiers_t::value_type const& f : cls.field_identifiers() ) {
			hcore::HString const& doc( cls.doc( f ) );
			if ( ! doc.is_empty() ) {
				stream_ << cn << "." << identifier_name( f ) << ":";
				int fi( cls.field_index( f, HClass::MEMBER_TYPE::STATIC ) );
				HHuginn::type_id_t tid( cls.field( fi )->type_id() );
				if ( ( doc.front() != '(' ) && ( tid == HHuginn::TYPE::METHOD ) ) {
					stream_ << "() - ";
				}
				stream_ << doc << endl;
			}
		}
	}
	for ( global_definitions_t::value_type const& gd : _globalDefinitions ) {
		if ( ( (*gd.second)->type_id() != HHuginn::TYPE::FUNCTION_REFERENCE ) || ( _classes.count( gd.first ) > 0 ) ) {
			continue;
		}
		huginn::HFunctionReference const& f( *static_cast<huginn::HFunctionReference const*>( gd.second->raw() ) );
		if ( ! f.doc().is_empty() ) {
			stream_ << identifier_name( gd.first ) << ":" << f.doc() << endl;
		}
	}
	return;
	M_EPILOG
}

yaal::hcore::HString HRuntime::suggestion( HHuginn::identifier_id_t identifier_ ) const {
	M_PROLOG
	hcore::HString identifier( identifier_name( identifier_ ) );
	hcore::HString s;
	int minDiff( meta::max_signed<int>::value );
	for ( yaal::hcore::HString const& i : _identifierNames ) {
		int diff( tools::string::distance( i, identifier, tools::string::DISTANCE_METRIC::QWERTY ) );
		if ( ( diff > 0 ) && ( diff < minDiff ) ) {
			minDiff = diff;
			s = i;
		}
	}
	return ( s );
	M_EPILOG
}

yaal::hcore::HString HRuntime::suggestion( HClass const* class_, HHuginn::identifier_id_t identifierId_ ) const {
	M_PROLOG
	hcore::HString identifierName( identifier_name( identifierId_ ) );
	hcore::HString s;
	int minDiff( meta::max_signed<int>::value );
	while ( class_ ) {
		for ( HHuginn::identifier_id_t identifierId : class_->field_identifiers() ) {
			hcore::HString const& name( identifier_name( identifierId ) );
			int diff( tools::string::distance( name, identifierName, tools::string::DISTANCE_METRIC::QWERTY ) );
			if ( ( diff > 0 ) && ( diff < minDiff ) ) {
				minDiff = diff;
				s = name;
			}
		}
		class_ = class_->super();
	}
	return ( s );
	M_EPILOG
}

yaal::hcore::HString const& HRuntime::function_name( void const* id_ ) const {
	M_PROLOG
	static yaal::hcore::HString const unknown( "unknown" );
	yaal::hcore::HString const* name( &unknown );
	bool found( false );
	for ( values_store_t::value_type const& f : *_valuesStore ) {
		if ( f->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			huginn::HFunctionReference const* fr( static_cast<huginn::HFunctionReference const*>( f.raw() ) );
			if ( fr->function().id() == id_ ) {
				name = &identifier_name( fr->identifier_id() );
				found = true;
				break;
			}
		}
	}
	if ( ! found ) {
		for ( HHuginn::class_t const& c : _dependencies ) {
			if ( static_cast<huginn::HFunctionReference const*>( c->constructor().raw() )->function().id() == id_ ) {
				name = &c->name();
			}
		}
	}
	return ( *name );
	M_EPILOG
}

yaal::hcore::HString const* HRuntime::package_name( HClass const* class_ ) const {
	M_PROLOG
	yaal::hcore::HString const* name( nullptr );
	for ( global_definitions_t::value_type const& gd : _globalDefinitions ) {
		if ( (*gd.second)->get_class() == class_ ) {
			name = &identifier_name( gd.first );
			break;
		}
	}
	return ( name );
	M_EPILOG
}

HRuntime::global_definitions_t const& HRuntime::globals( void ) const {
	return ( _globalDefinitions );
}

HHuginn::call_stack_t HRuntime::get_call_stack( huginn::HThread* thread_ ) {
	M_PROLOG
	HHuginn::call_stack_t callStack;
	HFrame* f( thread_->current_frame() );
	int position( f->position() );
	while ( f ) {
		if ( f->type() == HFrame::TYPE::FUNCTION ) {
			int fileId( f->file_id() );
			HHuginn::HCoordinate coord( _huginn->get_coordinate( fileId, position ) );
			HFunction const* func( static_cast<HFunction const*>( f->statement() ) );
			callStack.emplace_back( _huginn->source_name( fileId ), coord.line(), coord.column(), identifier_name( func->name() ) );
			position = INVALID_POSITION;
		}
		f = f->parent();
		if ( f && ( position == INVALID_POSITION ) ) {
			position = f->position();
			if ( ( position == INVALID_POSITION ) && ( f->statement() ) ) {
				position = f->statement()->position();
			}
		}
	}
	return ( callStack );
	M_EPILOG
}

HHuginn::call_stack_t HRuntime::do_get_call_stack( void ) {
	M_PROLOG
	return ( get_call_stack( current_thread() ) );
	M_EPILOG
}

HHuginn::call_stack_t const& HRuntime::trace( void ) const {
	return ( _trace );
}

HIntrospecteeInterface::variable_views_t HRuntime::get_locals( HThread* thread_, int frameNo_ ) {
	M_PROLOG
	variable_views_t variableViews;
	HFrame* f( thread_->current_frame() );
	while ( frameNo_ > 0 ) {
		while ( f ) {
			HFrame::TYPE t( f->type() );
			f = f->parent();
			if ( t == HFrame::TYPE::FUNCTION ) {
				break;
			}
		}
		-- frameNo_;
	}
	while ( f ) {
		HHuginn::values_t& variableValues( f->variable_values() );
		HFrame::identifiers_t const& variableIdentifiers( f->variable_identifiers() );
		M_ASSERT( variableIdentifiers.get_size() == variableValues.get_size() );
		for ( int i( 0 ), COUNT( static_cast<int>( variableValues.get_size() ) ); i < COUNT; ++ i ) {
			variableViews.emplace_back( identifier_name( variableIdentifiers[ i ] ), variableValues[ i ] );
		}
		if ( f->type() == HFrame::TYPE::FUNCTION ) {
			break;
		}
		f = f->parent();
	}
	return ( variableViews );
	M_EPILOG
}

HIntrospecteeInterface::variable_views_t HRuntime::do_get_locals( int frameNo_ ) {
	M_PROLOG
	return ( get_locals( current_thread(), frameNo_ ) );
	M_EPILOG
}

void HRuntime::set_setup( HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t setup_ ) {
	M_PROLOG
	_modulePaths = paths_;
	_compilerSetup = setup_;
	return;
	M_EPILOG
}

HHuginn::paths_t const& HRuntime::module_paths( void ) const {
	return ( _modulePaths );
}

HHuginn::compiler_setup_t HRuntime::compiler_setup( void ) const {
	return ( _compilerSetup );
}

}

}

}

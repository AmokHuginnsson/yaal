/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "runtime.hxx"
#include "keyword.hxx"
#include "objectfactory.hxx"
#include "thread.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"
#include "compiler.hxx"
#include "packagefactory.hxx"
#include "tools/util.hxx"
#include "tools/streamtools.hxx"
#include "tools/stringalgo.hxx"
#include "tools/tools.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::identifier_id_t const INVALID_IDENTIFIER( -1 );

HRuntime::HRuntime( HHuginn* huginn_ )
	: _huginn( huginn_ )
	, _idGenerator( static_cast<type_id_t::value_type>( HHuginn::TYPE::NOT_BOOLEAN ) )
	, _identifierIds( {
			{ KEYWORD::CONSTRUCTOR, KEYWORD::CONSTRUCTOR_IDENTIFIER },
			{ KEYWORD::DESTRUCTOR, KEYWORD::DESTRUCTOR_IDENTIFIER },
			{ KEYWORD::THIS, KEYWORD::THIS_IDENTIFIER },
			{ KEYWORD::SUPER, KEYWORD::SUPER_IDENTIFIER },
			{ KEYWORD::ASSERT, KEYWORD::ASSERT_IDENTIFIER },
			{ BUILTIN::INTEGER, BUILTIN::INTEGER_IDENTIFIER },
			{ BUILTIN::REAL, BUILTIN::REAL_IDENTIFIER },
			{ BUILTIN::NUMBER, BUILTIN::NUMBER_IDENTIFIER },
			{ BUILTIN::STRING, BUILTIN::STRING_IDENTIFIER },
			{ BUILTIN::CHARACTER, BUILTIN::CHARACTER_IDENTIFIER },
			{ BUILTIN::BOOLEAN, BUILTIN::BOOLEAN_IDENTIFIER },
			{ BUILTIN::SIZE, BUILTIN::SIZE_IDENTIFIER },
			{ BUILTIN::TYPE, BUILTIN::TYPE_IDENTIFIER },
			{ BUILTIN::COPY, BUILTIN::COPY_IDENTIFIER },
			{ BUILTIN::OBSERVE, BUILTIN::OBSERVE_IDENTIFIER },
			{ BUILTIN::USE, BUILTIN::USE_IDENTIFIER },
			{ BUILTIN::TUPLE, BUILTIN::TUPLE_IDENTIFIER },
			{ BUILTIN::LIST, BUILTIN::LIST_IDENTIFIER },
			{ BUILTIN::DEQUE, BUILTIN::DEQUE_IDENTIFIER },
			{ BUILTIN::DICT, BUILTIN::DICT_IDENTIFIER },
			{ BUILTIN::LOOKUP, BUILTIN::LOOKUP_IDENTIFIER },
			{ BUILTIN::ORDER, BUILTIN::ORDER_IDENTIFIER },
			{ BUILTIN::SET, BUILTIN::SET_IDENTIFIER },
			{ BUILTIN::BLOB, BUILTIN::BLOB_IDENTIFIER },
			{ INTERFACE::CLONE,            INTERFACE::CLONE_IDENTIFIER },
			{ INTERFACE::GET_SIZE,         INTERFACE::GET_SIZE_IDENTIFIER },
			{ INTERFACE::ITERATOR,         INTERFACE::ITERATOR_IDENTIFIER },
			{ INTERFACE::IS_VALID,         INTERFACE::IS_VALID_IDENTIFIER },
			{ INTERFACE::NEXT,             INTERFACE::NEXT_IDENTIFIER },
			{ INTERFACE::VALUE,            INTERFACE::VALUE_IDENTIFIER },
			{ INTERFACE::HASH,             INTERFACE::HASH_IDENTIFIER },
			{ INTERFACE::EQUALS,           INTERFACE::EQUALS_IDENTIFIER },
			{ INTERFACE::LESS,             INTERFACE::LESS_IDENTIFIER },
			{ INTERFACE::GREATER,          INTERFACE::GREATER_IDENTIFIER },
			{ INTERFACE::LESS_OR_EQUAL,    INTERFACE::LESS_OR_EQUAL_IDENTIFIER },
			{ INTERFACE::GREATER_OR_EQUAL, INTERFACE::GREATER_OR_EQUAL_IDENTIFIER },
			{ INTERFACE::ADD,              INTERFACE::ADD_IDENTIFIER },
			{ INTERFACE::SUBTRACT,         INTERFACE::SUBTRACT_IDENTIFIER },
			{ INTERFACE::MULTIPLY,         INTERFACE::MULTIPLY_IDENTIFIER },
			{ INTERFACE::DIVIDE,           INTERFACE::DIVIDE_IDENTIFIER },
			{ INTERFACE::MODULO,           INTERFACE::MODULO_IDENTIFIER },
			{ INTERFACE::POWER,            INTERFACE::POWER_IDENTIFIER },
			{ INTERFACE::MODULUS,          INTERFACE::MODULUS_IDENTIFIER },
			{ INTERFACE::NEGATE,           INTERFACE::NEGATE_IDENTIFIER },
			{ INTERFACE::TO_STRING,        INTERFACE::TO_STRING_IDENTIFIER },
			{ INTERFACE::TO_INTEGER,       INTERFACE::TO_INTEGER_IDENTIFIER },
			{ INTERFACE::TO_REAL,          INTERFACE::TO_REAL_IDENTIFIER },
			{ INTERFACE::TO_NUMBER,        INTERFACE::TO_NUMBER_IDENTIFIER },
			{ INTERFACE::TO_CHARACTER,     INTERFACE::TO_CHARACTER_IDENTIFIER },
			{ INTERFACE::TO_BOOLEAN,       INTERFACE::TO_BOOLEAN_IDENTIFIER },
			{ STANDARD_FUNCTIONS::MAIN, STANDARD_FUNCTIONS::MAIN_IDENTIFIER },
			{ _noneClass_.name(), TYPE_NONE_IDENTIFIER },
			{ _observerClass_.name(), TYPE_OBSERVER_IDENTIFIER },
			{ _referenceClass_.name(), TYPE_REFERENCE_IDENTIFIER },
			{ _functionReferenceClass_.name(), TYPE_FUNCTION_REFERENCE_IDENTIFIER },
			{ _objectReferenceClass_.name(), TYPE_OBJECT_REFERENCE_IDENTIFIER },
			{ _methodClass_.name(), TYPE_METHOD_IDENTIFIER },
			{ _unboundMethodClass_.name(), TYPE_UNBOUND_METHOD_IDENTIFIER },
			{ _boundMethodClass_.name(), TYPE_BOUND_METHOD_IDENTIFIER },
			{ _variadicParametersClass_.name(), TYPE_VARIADIC_PARAMETERS_IDENTIFIER },
			{ _namedParametersClass_.name(), TYPE_NAMED_PARAMETERS_IDENTIFIER },
			{ _unknownClass_.name(), TYPE_UNKNOWN_IDENTIFIER }
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
			BUILTIN::SIZE,
			BUILTIN::TYPE,
			BUILTIN::COPY,
			BUILTIN::OBSERVE,
			BUILTIN::USE,
			BUILTIN::TUPLE,
			BUILTIN::LIST,
			BUILTIN::DEQUE,
			BUILTIN::DICT,
			BUILTIN::LOOKUP,
			BUILTIN::ORDER,
			BUILTIN::SET,
			BUILTIN::BLOB,
			INTERFACE::CLONE,
			INTERFACE::GET_SIZE,
			INTERFACE::ITERATOR,
			INTERFACE::IS_VALID,
			INTERFACE::NEXT,
			INTERFACE::VALUE,
			INTERFACE::HASH,
			INTERFACE::EQUALS,
			INTERFACE::LESS,
			INTERFACE::GREATER,
			INTERFACE::LESS_OR_EQUAL,
			INTERFACE::GREATER_OR_EQUAL,
			INTERFACE::ADD,
			INTERFACE::SUBTRACT,
			INTERFACE::MULTIPLY,
			INTERFACE::DIVIDE,
			INTERFACE::MODULO,
			INTERFACE::POWER,
			INTERFACE::MODULUS,
			INTERFACE::NEGATE,
			INTERFACE::TO_STRING,
			INTERFACE::TO_INTEGER,
			INTERFACE::TO_REAL,
			INTERFACE::TO_NUMBER,
			INTERFACE::TO_CHARACTER,
			INTERFACE::TO_BOOLEAN,
			STANDARD_FUNCTIONS::MAIN,
			_noneClass_.name(),
			_observerClass_.name(),
			_referenceClass_.name(),
			_functionReferenceClass_.name(),
			_objectReferenceClass_.name(),
			_methodClass_.name(),
			_unboundMethodClass_.name(),
			_boundMethodClass_.name(),
			_variadicParametersClass_.name(),
			_namedParametersClass_.name(),
			_unknownClass_.name()
		} )
	, _objectFactory( new HObjectFactory( this ) )
	, _none( _objectFactory->create<HHuginn::HValue>( &_noneClass_ ) )
	, _true( _objectFactory->create_boolean( true ) )
	, _false( _objectFactory->create_boolean( false ) )
	, _threads()
	, _functionsStore()
	, _functionsAvailable()
	, _dependencies()
	, _classes()
	, _packages()
	, _argv( _objectFactory->create_list() )
	, _result()
	, _incrementalFrame()
	, _maxLocalVariableCount( 0 )
	, _maxCallStackSize( _huginnMaxCallStack_ ) {
}

void HRuntime::reset( void ) {
	M_PROLOG
	_result.reset();
	static_cast<HHuginn::HList*>( _argv.raw() )->clear();
	_threads.clear();
	_idGenerator = static_cast<type_id_t::value_type>( HHuginn::TYPE::NOT_BOOLEAN );
	return;
	M_EPILOG
}

void HRuntime::copy_text( HRuntime& source_ ) {
	M_PROLOG
	M_ASSERT( &source_ != this );
	_idGenerator = source_._idGenerator;
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
	_packages = source_._packages;
	_classes = source_._classes;
	_dependencies = source_._dependencies;
	using yaal::swap;
	_functionsStore = yaal::move( source_._functionsStore );
	_threads = source_._threads;
	_false = source_._false;
	_true = source_._true;
	_none = source_._none;
	_objectFactory = source_._objectFactory;
	_maxLocalVariableCount = source_._maxLocalVariableCount;
	_maxCallStackSize = source_._maxCallStackSize;
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

HHuginn::value_t HRuntime::result( void ) const {
	return ( _result );
}

HHuginn::value_t* HRuntime::get_function( identifier_id_t identifierId_, bool fromAll_ ) {
	M_PROLOG
	HHuginn::value_t* f( nullptr );
	if ( fromAll_ || ( _functionsAvailable.count( identifierId_ ) > 0 ) ) {
		f = &_functionsStore.at( identifierId_ );
	}
	return ( f );
	M_EPILOG
}

HHuginn::class_t HRuntime::get_class( identifier_id_t identifierId_ ) {
	M_PROLOG
	classes_t::const_iterator ci( _classes.find( identifierId_ ) );
	return ( ci != _classes.end() ? ci->second : class_t() );
	M_EPILOG
}

HHuginn::value_t* HRuntime::get_package( identifier_id_t identifierId_ ) {
	M_PROLOG
	HHuginn::value_t* v( nullptr );
	packages_t::iterator it( _packages.find( identifierId_ ) );
	if ( it != _packages.end() ) {
		v = &( it->second );
	}
	return ( v );
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

void HRuntime::register_class( class_t class_, HHuginn::ACCESS classConstructorAccess_, HHuginn::VISIBILITY classConstructorVisibility_ ) {
	M_PROLOG
	if ( _classes.insert( make_pair( class_->identifier_id(), class_ ) ).second ) {
		_dependencies.push_back( class_ );
		HHuginn::identifier_id_t identifier( class_->identifier_id() );
		_functionsStore.insert( make_pair( identifier, class_->constructor( classConstructorAccess_ ) ) );
	}
	if ( classConstructorVisibility_ == HHuginn::VISIBILITY::GLOBAL ) {
		_functionsAvailable.insert( class_->identifier_id() );
	}
	class_->finalize_registration( this );
	return;
	M_EPILOG
}

void HRuntime::drop_class( identifier_id_t identifier_ ) {
	M_PROLOG
	_functionsAvailable.erase( identifier_ );
	_functionsStore.erase( identifier_ );
	_dependencies.erase(
		yaal::remove_if(
			_dependencies.begin(),
			_dependencies.end(),
			[&identifier_]( class_t const& class_ ) {
				return ( class_->identifier_id() == identifier_ );
			}
		),
		_dependencies.end()
	);
	_classes.erase( identifier_ );
	return;
	M_EPILOG
}

void HRuntime::register_function( identifier_id_t identifier_, function_t function_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	HHuginn::value_t& f( _functionsStore[ identifier_ ] );
	if ( ! f ) {
		f = _objectFactory->create_function_reference( identifier_, function_, doc_ );
	} else {
		static_cast<HHuginn::HFunctionReference*>( f.raw() )->reset( function_ );
	}
	_functionsAvailable.insert( identifier_ );
	return;
	M_EPILOG
}

void HRuntime::drop_function( identifier_id_t identifier_ ) {
	M_PROLOG
	_functionsAvailable.erase( identifier_ );
	_functionsStore.erase( identifier_ );
	return;
	M_EPILOG
}

void HRuntime::register_package( identifier_id_t package_, identifier_id_t alias_, HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t compilerSetup_, int position_ ) {
	M_PROLOG
	HHuginn::value_t package;
	for ( packages_t::value_type const& p : _packages ) {
		if ( p.second->get_class()->identifier_id() == package_ ) {
			package = p.second;
			break;
		}
	}
	if ( ! package ) {
		package = HPackageFactory::get_instance().create_package( this, paths_, compilerSetup_, identifier_name( package_ ), position_ );
	}
	_packages.insert( make_pair( alias_, package ) );
	return;
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class(
	identifier_id_t identifier_,
	HHuginn::HClass const* base_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass::TYPE type_,
	HHuginn::HClass const* origin_,
	HHuginn::HClass::create_instance_t createInstance_
) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			this,
			type_id_t( _idGenerator ),
			identifier_,
			base_,
			doc_,
			type_,
			origin_,
			createInstance_
		)
	);
	++ _idGenerator;
	return ( c );
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class(
	yaal::hcore::HString const& name_,
	HHuginn::HClass const* base_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass::TYPE type_,
	HHuginn::HClass const* origin_,
	HHuginn::HClass::create_instance_t createInstance_
) {
	M_PROLOG
	return (
		create_class( identifier_id( name_ ), base_, doc_, type_, origin_, createInstance_ )
	);
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( class_constructor_t const& classConstructor_ ) {
	M_PROLOG
	type_id_t id( _idGenerator );
	++ _idGenerator;
	class_t c( classConstructor_( id ) );
	return ( c );
	M_EPILOG
}

HHuginn::identifier_id_t HRuntime::identifier_id( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	M_ASSERT( _identifierIds.get_size() == _identifierNames.get_size() );
	identifier_id_t id;
	identifier_ids_t::const_iterator it( _identifierIds.find( name_ ) );
	if ( it != _identifierIds.end() ) {
		id = it->second;
	} else {
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
	static yaal::hcore::HString const INVALID_IDENTIFIER_NAME = "*invalid_identifier*";
	return ( id_ != INVALID_IDENTIFIER ? _identifierNames[id_.get()] : INVALID_IDENTIFIER_NAME );
	M_EPILOG
}

void HRuntime::execute( void ) {
	M_PROLOG
	util::HScopeExitCall sec( hcore::call( &threads_t::clear, &_threads ) );
	values_t args;
	args.reserve( max_local_variable_count() );
	HHuginn::HList& argv( *static_cast<HHuginn::HList*>( _argv.raw() ) );
	if ( argv.size( nullptr, 0 ) > 0 ) {
		args.push_back( _argv );
	}
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	_result = call( STANDARD_FUNCTIONS::MAIN_IDENTIFIER, args, 0 );
	t->flush_runtime_exception();
	return;
	M_EPILOG
}

namespace {
namespace package {

HHuginn::value_t value( HHuginn::value_t value_, HUTF8String name_, HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 0, 0, thread_, position_ );
	return ( value_ );
	M_EPILOG
}

HHuginn::value_t instance( HHuginn::HClass const* class_, HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	return ( class_->create_instance( thread_, object_, values_, position_ ) );
	M_EPILOG
}

}
}

HHuginn::class_t HRuntime::make_package( yaal::hcore::HString const& name_, HRuntime const& context_ ) {
	M_PROLOG
	HString doc( _huginn->get_comment( 1 ) );
	HHuginn::class_t cls( create_class( name_, nullptr, ! doc.is_empty() ? doc : "The `"_ys.append( name_ ).append( "` is an user defined submodule." ) ) );
	HHuginn::field_definitions_t fds;
	/* Erasing from lookup invalidated .end() */
	for ( packages_t::iterator it( _packages.begin() ); it != _packages.end(); ) {
		if ( context_._packages.find( it->first ) == context_._packages.end() ) {
			fds.emplace_back(
				identifier_name( it->first ),
				create_method( &package::value, it->second, identifier_name( it->first ) ),
				"access package "_ys.append( it->second->get_class()->name() ).append( " imported in submodule" )
			);
			it = _packages.erase( it );
		} else {
			++ it;
		}
	}
	for ( classes_t::value_type const& c : _classes ) {
		if ( context_._classes.find( c.first ) == context_._classes.end() ) {
			fds.emplace_back(
				identifier_name( c.first ),
				create_method( &package::instance, c.second.raw() ),
				"access class "_ys.append( identifier_name( c.first ) ).append( " imported in submodule" )
			);
		}
	}
	for ( functions_available_t::value_type const& fi : _functionsAvailable ) {
		if ( context_._functionsAvailable.find( fi ) == context_._functionsAvailable.end() ) {
			HHuginn::HFunctionReference const* fr( static_cast<HHuginn::HFunctionReference const*>( _functionsStore.at( fi ).raw() ) );
			fds.emplace_back(
				identifier_name( fi ),
				_objectFactory->create_method_raw( fr->function() ),
				! fr->doc().is_empty() ? fr->doc() : "access function "_ys.append( identifier_name( fi ) ).append( " imported in submodule" )
			);
		}
	}
	for ( classes_t::value_type& m : _classes ) {
		if ( ! m.second->origin() && ( context_._classes.find( m.first ) == context_._classes.end() )) {
			m.second->set_origin( cls.raw() );
		}
	}
	cls->redefine( nullptr, fds );
	_huginn->register_class( cls );
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

HHuginn::value_t HRuntime::call( identifier_id_t identifier_, values_t& values_, int position_ ) {
	M_PROLOG
	value_t res;
	if ( _functionsAvailable.count( identifier_ ) > 0 ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = static_cast<HHuginn::HFunctionReference*>( _functionsStore.at( identifier_ ).raw() )->function()( t->second.raw(), nullptr, values_, position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Function `"_ys.append( identifier_name( identifier_ ) ).append( "(...)' is not defined." ), MAIN_FILE_ID, position_ );
	}
	return ( res );
	M_EPILOG
}

void HRuntime::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	HHuginn::HList& argv( *static_cast<HHuginn::HList*>( _argv.raw() ) );
	argv.push_back( _objectFactory->create_string( arg_ ) );
	return;
	M_EPILOG
}

void HRuntime::clear_arguments( void ) {
	M_PROLOG
	HHuginn::HList& argv( *static_cast<HHuginn::HList*>( _argv.raw() ) );
	argv.clear();
	return;
	M_EPILOG
}

namespace huginn_builtin {

HHuginn::value_t string( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string", values_, 1, 1, thread_, position_ );
	return ( value_builtin::string( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t integer( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "integer", values_, 1, 1, thread_, position_ );
	return ( value_builtin::integer( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t real( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "real", values_, 1, 1, thread_, position_ );
	return ( value_builtin::real( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t boolean( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "boolean", values_, 1, 1, thread_, position_ );
	return ( value_builtin::boolean( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t character( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "character", values_, 1, 1, thread_, position_ );
	return ( value_builtin::character( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t number( HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "number", values_, 1, 1, thread_, position_ );
	return ( value_builtin::number( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t size( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "size", values_, 1, 1, thread_, position_ );
	HHuginn::value_t const& val( values_.front() );
	HHuginn::HValue const* v( val.raw() );
	int long long s( 0 );
	HHuginn::HIterable const* iterable( dynamic_cast<HHuginn::HIterable const*>( v ) );
	if ( iterable ) {
		s = iterable->size( thread_, position_ );
	} else if ( v->type_id() == HHuginn::TYPE::BLOB ) {
		s = static_cast<HHuginn::HBlob const*>( v )->get_size();
	} else if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v ) ) {
		HHuginn::value_t res( o->call_method( thread_, val, INTERFACE::GET_SIZE_IDENTIFIER, HArguments( thread_ ), position_ ) );
		if ( res->type_id() != HHuginn::TYPE::INTEGER ) {
			throw HHuginn::HHuginnRuntimeException(
				"User supplied `get_size' method returned an invalid type "_ys
					.append( a_type_name( res->get_class() ) )
					.append( " instead of an `integer'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		s = get_integer( res );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Getting size of "_ys.append( a_type_name( v->get_class() ) ).append( "s is not supported." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( thread_->object_factory().create_integer( s ) );
	M_EPILOG
}

HHuginn::value_t type( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, thread_, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	HHuginn::identifier_id_t id( v->get_class()->identifier_id() );
	HHuginn::value_t* f( thread_->runtime().get_function( id, true ) );
	return ( *f );
	M_EPILOG
}

HHuginn::value_t copy( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, thread_, position_ );
	HHuginn::value_t& v( values_.front() );
	return ( v->clone( thread_, &v, position_ ) );
	M_EPILOG
}

HHuginn::value_t observe( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "observe", values_, 1, 1, thread_, position_ );
	HHuginn::value_t const& v( values_.front() );
	if ( v->type_id() == HHuginn::TYPE::OBSERVER ) {
		throw HHuginn::HHuginnRuntimeException(
			"Making an *observer* out of an *observer*.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( thread_->object_factory().create<HHuginn::HObserver>( v ) );
	M_EPILOG
}

HHuginn::value_t use( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "use", values_, { HHuginn::TYPE::OBSERVER }, thread_, position_ );
	HHuginn::HObserver const* o( static_cast<HHuginn::HObserver const*>( values_.front().raw() ) );
	HHuginn::value_t v( o->value() );
	if ( !v ) {
		v = thread_->runtime().none_value();
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t tuple( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::values_t v( values_ );
	return ( thread_->object_factory().create_tuple( yaal::move( v ) ) );
	M_EPILOG
}

HHuginn::value_t list( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_list() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t deque( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_deque() );
	HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		d->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t dict( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_dict() );
	M_EPILOG
}

HHuginn::value_t order( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_order() );
	HHuginn::HOrder* o( static_cast<HHuginn::HOrder*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		o->insert( thread_, e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t lookup( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_lookup() );
	M_EPILOG
}

HHuginn::value_t set( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_set() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( thread_, e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t blob( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "blob", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HHuginn::HInteger::value_type s( get_integer( values_[0] ) );
	if ( s <= 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid `blob` size requested: "_ys.append( s ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	int long rs( 0 );
	try {
		rs = safe_int::cast<int long>( s );
	} catch ( ... ) {
		throw HHuginn::HHuginnRuntimeException(
			"Requested `blob` size is too big: "_ys.append( s ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HChunk c;
	try {
		c.realloc( rs, HChunk::STRATEGY::EXACT );
	} catch ( HException const& e ) {
		thread_->raise( thread_->object_factory().runtime_exception_class(), e.what(), position_ );
	}
	return ( thread_->object_factory().create_blob( yaal::move( c ) ) );
	M_EPILOG
}

HHuginn::value_t print( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "print", values_, 1, 1, thread_, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	yaal::hcore::HStreamInterface& out( thread_->runtime().huginn()->output_stream() );
	HHuginn::type_id_t typeId( v->type_id() );
	if ( typeId == HHuginn::TYPE::INTEGER ) {
		out << static_cast<HHuginn::HInteger const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::REAL ) {
		out << static_cast<HHuginn::HReal const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::STRING ) {
		out << static_cast<HHuginn::HString const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::NUMBER ) {
		out << static_cast<HHuginn::HNumber const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::BOOLEAN ) {
		out << static_cast<HHuginn::HBoolean const*>( v )->value();
	} else if ( typeId == HHuginn::TYPE::CHARACTER ) {
		out << static_cast<HHuginn::HCharacter const*>( v )->value();
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Printing `"_ys.append( v->get_class()->name() ).append( "'s is not supported." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	out << flush;
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t input( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "input", values_, 0, 0, thread_, position_ );
	yaal::hcore::HString l;
	int len( static_cast<int>( thread_->runtime().huginn()->input_stream().read_until( l, HStreamInterface::eols, false ) ) );
	return ( len > 0 ? thread_->object_factory().create_string( l ) : thread_->runtime().none_value() );
	M_EPILOG
}

inline HHuginn::value_t assert( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "assert";
	verify_arg_count( name, values_, 2, 3, thread_, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::BOOLEAN, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
	if ( ! get_boolean( values_[0] ) ) {
		int argc( static_cast<int>( values_.get_size() ) );
		HString message( get_string( values_[argc - 1] ) );
		if ( argc > 2 ) {
			message.append( " " ).append( get_string( values_[1] ) );
		}
		throw HHuginn::HHuginnRuntimeException( message, thread_->current_frame()->file_id(), position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t invalid_instance( yaal::hcore::HString const&, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t invalid_instance( yaal::hcore::HString const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t&, int position_ ) {
	M_PROLOG
	/*
	 * __attribute__(( noreturn )) causes problems with clang.
	 * Hence this workaround.
	 * Condition in following if shall always evaluate to `true'.
	 */
	if ( position_ >= 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Direct creation of instances of `"_ys.append( name_ ).append( "' is not allowed." ), thread_->current_frame()->file_id(), position_ );
	}
	return ( HHuginn::value_t() );
	M_EPILOG
}

}

void HRuntime::register_builtin_function( yaal::hcore::HString const& name_, function_t&& function_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	identifier_id_t id( identifier_id( name_ ) );
	_huginn->register_function( id );
	_functionsStore.insert( make_pair( id, _objectFactory->create_function_reference( id, yaal::move( function_ ), doc_ ) ) );
	_functionsAvailable.insert( id );
	return;
	M_EPILOG
}

namespace {
HHuginn::HClass const* _coreClasses_[11];
}

void HRuntime::register_builtins( void ) {
	M_PROLOG
	static volatile bool once( false );
	if ( ! once ) {
		once = true;
		HHuginn::HClass const* coreClassesInit[] = {
			&_noneClass_,
			&_observerClass_,
			&_referenceClass_,
			&_functionReferenceClass_,
			&_objectReferenceClass_,
			&_methodClass_,
			&_unboundMethodClass_,
			&_boundMethodClass_,
			&_variadicParametersClass_,
			&_namedParametersClass_,
			&_unknownClass_
		};
		static_assert( sizeof ( coreClassesInit ) == sizeof ( _coreClasses_ ), "invalid core classes initializer size" );
		copy( begin( coreClassesInit ), end( coreClassesInit ), begin( _coreClasses_ ) );
	}
	M_ENSURE( ( identifier_id( KEYWORD::CONSTRUCTOR ) == KEYWORD::CONSTRUCTOR_IDENTIFIER ) && ( identifier_name( KEYWORD::CONSTRUCTOR_IDENTIFIER ) == KEYWORD::CONSTRUCTOR ) );
	M_ENSURE( ( identifier_id( KEYWORD::DESTRUCTOR ) == KEYWORD::DESTRUCTOR_IDENTIFIER ) && ( identifier_name( KEYWORD::DESTRUCTOR_IDENTIFIER ) == KEYWORD::DESTRUCTOR ) );
	M_ENSURE( ( identifier_id( KEYWORD::THIS ) == KEYWORD::THIS_IDENTIFIER ) && ( identifier_name( KEYWORD::THIS_IDENTIFIER ) == KEYWORD::THIS ) );
	M_ENSURE( ( identifier_id( KEYWORD::SUPER ) == KEYWORD::SUPER_IDENTIFIER ) && ( identifier_name( KEYWORD::SUPER_IDENTIFIER ) == KEYWORD::SUPER ) );
	M_ENSURE( ( identifier_id( KEYWORD::ASSERT ) == KEYWORD::ASSERT_IDENTIFIER ) && ( identifier_name( KEYWORD::ASSERT_IDENTIFIER ) == KEYWORD::ASSERT ) );
	M_ENSURE( ( identifier_id( BUILTIN::INTEGER ) == BUILTIN::INTEGER_IDENTIFIER ) && ( identifier_name( BUILTIN::INTEGER_IDENTIFIER ) == BUILTIN::INTEGER ) );
	M_ENSURE( ( identifier_id( BUILTIN::REAL ) == BUILTIN::REAL_IDENTIFIER ) && ( identifier_name( BUILTIN::REAL_IDENTIFIER ) == BUILTIN::REAL ) );
	M_ENSURE( ( identifier_id( BUILTIN::NUMBER ) == BUILTIN::NUMBER_IDENTIFIER ) && ( identifier_name( BUILTIN::NUMBER_IDENTIFIER ) == BUILTIN::NUMBER ) );
	M_ENSURE( ( identifier_id( BUILTIN::STRING ) == BUILTIN::STRING_IDENTIFIER ) && ( identifier_name( BUILTIN::STRING_IDENTIFIER ) == BUILTIN::STRING ) );
	M_ENSURE( ( identifier_id( BUILTIN::CHARACTER ) == BUILTIN::CHARACTER_IDENTIFIER ) && ( identifier_name( BUILTIN::CHARACTER_IDENTIFIER ) == BUILTIN::CHARACTER ) );
	M_ENSURE( ( identifier_id( BUILTIN::BOOLEAN ) == BUILTIN::BOOLEAN_IDENTIFIER ) && ( identifier_name( BUILTIN::BOOLEAN_IDENTIFIER ) == BUILTIN::BOOLEAN ) );
	M_ENSURE( ( identifier_id( BUILTIN::SIZE ) == BUILTIN::SIZE_IDENTIFIER ) && ( identifier_name( BUILTIN::SIZE_IDENTIFIER ) == BUILTIN::SIZE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TYPE ) == BUILTIN::TYPE_IDENTIFIER ) && ( identifier_name( BUILTIN::TYPE_IDENTIFIER ) == BUILTIN::TYPE ) );
	M_ENSURE( ( identifier_id( BUILTIN::COPY ) == BUILTIN::COPY_IDENTIFIER ) && ( identifier_name( BUILTIN::COPY_IDENTIFIER ) == BUILTIN::COPY ) );
	M_ENSURE( ( identifier_id( BUILTIN::OBSERVE ) == BUILTIN::OBSERVE_IDENTIFIER ) && ( identifier_name( BUILTIN::OBSERVE_IDENTIFIER ) == BUILTIN::OBSERVE ) );
	M_ENSURE( ( identifier_id( BUILTIN::USE ) == BUILTIN::USE_IDENTIFIER ) && ( identifier_name( BUILTIN::USE_IDENTIFIER ) == BUILTIN::USE ) );
	M_ENSURE( ( identifier_id( BUILTIN::TUPLE ) == BUILTIN::TUPLE_IDENTIFIER ) && ( identifier_name( BUILTIN::TUPLE_IDENTIFIER ) == BUILTIN::TUPLE ) );
	M_ENSURE( ( identifier_id( BUILTIN::LIST ) == BUILTIN::LIST_IDENTIFIER ) && ( identifier_name( BUILTIN::LIST_IDENTIFIER ) == BUILTIN::LIST ) );
	M_ENSURE( ( identifier_id( BUILTIN::DEQUE ) == BUILTIN::DEQUE_IDENTIFIER ) && ( identifier_name( BUILTIN::DEQUE_IDENTIFIER ) == BUILTIN::DEQUE ) );
	M_ENSURE( ( identifier_id( BUILTIN::DICT ) == BUILTIN::DICT_IDENTIFIER ) && ( identifier_name( BUILTIN::DICT_IDENTIFIER ) == BUILTIN::DICT ) );
	M_ENSURE( ( identifier_id( BUILTIN::LOOKUP ) == BUILTIN::LOOKUP_IDENTIFIER ) && ( identifier_name( BUILTIN::LOOKUP_IDENTIFIER ) == BUILTIN::LOOKUP ) );
	M_ENSURE( ( identifier_id( BUILTIN::ORDER ) == BUILTIN::ORDER_IDENTIFIER ) && ( identifier_name( BUILTIN::ORDER_IDENTIFIER ) == BUILTIN::ORDER ) );
	M_ENSURE( ( identifier_id( BUILTIN::SET ) == BUILTIN::SET_IDENTIFIER ) && ( identifier_name( BUILTIN::SET_IDENTIFIER ) == BUILTIN::SET ) );
	M_ENSURE( ( identifier_id( BUILTIN::BLOB ) == BUILTIN::BLOB_IDENTIFIER ) && ( identifier_name( BUILTIN::BLOB_IDENTIFIER ) == BUILTIN::BLOB ) );
	M_ENSURE( ( identifier_id( INTERFACE::CLONE ) == INTERFACE::CLONE_IDENTIFIER ) && ( identifier_name( INTERFACE::CLONE_IDENTIFIER ) == INTERFACE::CLONE ) );
	M_ENSURE( ( identifier_id( INTERFACE::GET_SIZE ) == INTERFACE::GET_SIZE_IDENTIFIER ) && ( identifier_name( INTERFACE::GET_SIZE_IDENTIFIER ) == INTERFACE::GET_SIZE ) );
	M_ENSURE( ( identifier_id( INTERFACE::ITERATOR ) == INTERFACE::ITERATOR_IDENTIFIER ) && ( identifier_name( INTERFACE::ITERATOR_IDENTIFIER ) == INTERFACE::ITERATOR ) );
	M_ENSURE( ( identifier_id( INTERFACE::IS_VALID ) == INTERFACE::IS_VALID_IDENTIFIER ) && ( identifier_name( INTERFACE::IS_VALID_IDENTIFIER ) == INTERFACE::IS_VALID ) );
	M_ENSURE( ( identifier_id( INTERFACE::NEXT ) == INTERFACE::NEXT_IDENTIFIER ) && ( identifier_name( INTERFACE::NEXT_IDENTIFIER ) == INTERFACE::NEXT ) );
	M_ENSURE( ( identifier_id( INTERFACE::VALUE ) == INTERFACE::VALUE_IDENTIFIER ) && ( identifier_name( INTERFACE::VALUE_IDENTIFIER ) == INTERFACE::VALUE ) );
	M_ENSURE( ( identifier_id( INTERFACE::HASH ) == INTERFACE::HASH_IDENTIFIER ) && ( identifier_name( INTERFACE::HASH_IDENTIFIER ) == INTERFACE::HASH ) );
	M_ENSURE( ( identifier_id( INTERFACE::EQUALS ) == INTERFACE::EQUALS_IDENTIFIER ) && ( identifier_name( INTERFACE::EQUALS_IDENTIFIER ) == INTERFACE::EQUALS ) );
	M_ENSURE( ( identifier_id( INTERFACE::LESS ) == INTERFACE::LESS_IDENTIFIER ) && ( identifier_name( INTERFACE::LESS_IDENTIFIER ) == INTERFACE::LESS ) );
	M_ENSURE( ( identifier_id( INTERFACE::GREATER ) == INTERFACE::GREATER_IDENTIFIER ) && ( identifier_name( INTERFACE::GREATER_IDENTIFIER ) == INTERFACE::GREATER ) );
	M_ENSURE( ( identifier_id( INTERFACE::LESS_OR_EQUAL ) == INTERFACE::LESS_OR_EQUAL_IDENTIFIER ) && ( identifier_name( INTERFACE::LESS_OR_EQUAL_IDENTIFIER ) == INTERFACE::LESS_OR_EQUAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::GREATER_OR_EQUAL ) == INTERFACE::GREATER_OR_EQUAL_IDENTIFIER ) && ( identifier_name( INTERFACE::GREATER_OR_EQUAL_IDENTIFIER ) == INTERFACE::GREATER_OR_EQUAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::ADD ) == INTERFACE::ADD_IDENTIFIER ) && ( identifier_name( INTERFACE::ADD_IDENTIFIER ) == INTERFACE::ADD ) );
	M_ENSURE( ( identifier_id( INTERFACE::SUBTRACT ) == INTERFACE::SUBTRACT_IDENTIFIER ) && ( identifier_name( INTERFACE::SUBTRACT_IDENTIFIER ) == INTERFACE::SUBTRACT ) );
	M_ENSURE( ( identifier_id( INTERFACE::MULTIPLY ) == INTERFACE::MULTIPLY_IDENTIFIER ) && ( identifier_name( INTERFACE::MULTIPLY_IDENTIFIER ) == INTERFACE::MULTIPLY ) );
	M_ENSURE( ( identifier_id( INTERFACE::DIVIDE ) == INTERFACE::DIVIDE_IDENTIFIER ) && ( identifier_name( INTERFACE::DIVIDE_IDENTIFIER ) == INTERFACE::DIVIDE ) );
	M_ENSURE( ( identifier_id( INTERFACE::MODULO ) == INTERFACE::MODULO_IDENTIFIER ) && ( identifier_name( INTERFACE::MODULO_IDENTIFIER ) == INTERFACE::MODULO ) );
	M_ENSURE( ( identifier_id( INTERFACE::POWER ) == INTERFACE::POWER_IDENTIFIER ) && ( identifier_name( INTERFACE::POWER_IDENTIFIER ) == INTERFACE::POWER ) );
	M_ENSURE( ( identifier_id( INTERFACE::MODULUS ) == INTERFACE::MODULUS_IDENTIFIER ) && ( identifier_name( INTERFACE::MODULUS_IDENTIFIER ) == INTERFACE::MODULUS ) );
	M_ENSURE( ( identifier_id( INTERFACE::NEGATE ) == INTERFACE::NEGATE_IDENTIFIER ) && ( identifier_name( INTERFACE::NEGATE_IDENTIFIER ) == INTERFACE::NEGATE ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_STRING ) == INTERFACE::TO_STRING_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_STRING_IDENTIFIER ) == INTERFACE::TO_STRING ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_INTEGER ) == INTERFACE::TO_INTEGER_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_INTEGER_IDENTIFIER ) == INTERFACE::TO_INTEGER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_REAL ) == INTERFACE::TO_REAL_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_REAL_IDENTIFIER ) == INTERFACE::TO_REAL ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_NUMBER ) == INTERFACE::TO_NUMBER_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_NUMBER_IDENTIFIER ) == INTERFACE::TO_NUMBER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_CHARACTER ) == INTERFACE::TO_CHARACTER_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_CHARACTER_IDENTIFIER ) == INTERFACE::TO_CHARACTER ) );
	M_ENSURE( ( identifier_id( INTERFACE::TO_BOOLEAN ) == INTERFACE::TO_BOOLEAN_IDENTIFIER ) && ( identifier_name( INTERFACE::TO_BOOLEAN_IDENTIFIER ) == INTERFACE::TO_BOOLEAN ) );
	M_ENSURE( ( identifier_id( _noneClass_.name() ) == TYPE_NONE_IDENTIFIER ) && ( identifier_name( TYPE_NONE_IDENTIFIER ) == _noneClass_.name() ) );
	M_ENSURE( ( identifier_id( _observerClass_.name() ) == TYPE_OBSERVER_IDENTIFIER ) && ( identifier_name( TYPE_OBSERVER_IDENTIFIER ) == _observerClass_.name() ) );
	M_ENSURE( ( identifier_id( _referenceClass_.name() ) == TYPE_REFERENCE_IDENTIFIER ) && ( identifier_name( TYPE_REFERENCE_IDENTIFIER ) == _referenceClass_.name() ) );
	M_ENSURE( ( identifier_id( _functionReferenceClass_.name() ) == TYPE_FUNCTION_REFERENCE_IDENTIFIER ) && ( identifier_name( TYPE_FUNCTION_REFERENCE_IDENTIFIER ) == _functionReferenceClass_.name() ) );
	M_ENSURE( ( identifier_id( _objectReferenceClass_.name() ) == TYPE_OBJECT_REFERENCE_IDENTIFIER ) && ( identifier_name( TYPE_OBJECT_REFERENCE_IDENTIFIER ) == _objectReferenceClass_.name() ) );
	M_ENSURE( ( identifier_id( _methodClass_.name() ) == TYPE_METHOD_IDENTIFIER ) && ( identifier_name( TYPE_METHOD_IDENTIFIER ) == _methodClass_.name() ) );
	M_ENSURE( ( identifier_id( _unboundMethodClass_.name() ) == TYPE_UNBOUND_METHOD_IDENTIFIER ) && ( identifier_name( TYPE_UNBOUND_METHOD_IDENTIFIER ) == _unboundMethodClass_.name() ) );
	M_ENSURE( ( identifier_id( _boundMethodClass_.name() ) == TYPE_BOUND_METHOD_IDENTIFIER ) && ( identifier_name( TYPE_BOUND_METHOD_IDENTIFIER ) == _boundMethodClass_.name() ) );
	M_ENSURE( ( identifier_id( _variadicParametersClass_.name() ) == TYPE_VARIADIC_PARAMETERS_IDENTIFIER ) && ( identifier_name( TYPE_VARIADIC_PARAMETERS_IDENTIFIER ) == _variadicParametersClass_.name() ) );
	M_ENSURE( ( identifier_id( _namedParametersClass_.name() ) == TYPE_NAMED_PARAMETERS_IDENTIFIER ) && ( identifier_name( TYPE_NAMED_PARAMETERS_IDENTIFIER ) == _namedParametersClass_.name() ) );
	M_ENSURE( ( identifier_id( _unknownClass_.name() ) == TYPE_UNKNOWN_IDENTIFIER ) && ( identifier_name( TYPE_UNKNOWN_IDENTIFIER ) == _unknownClass_.name() ) );
	M_ENSURE( ( identifier_id( STANDARD_FUNCTIONS::MAIN ) == STANDARD_FUNCTIONS::MAIN_IDENTIFIER ) && ( identifier_name( STANDARD_FUNCTIONS::MAIN_IDENTIFIER ) == STANDARD_FUNCTIONS::MAIN ) );
	register_builtin_function( type_name( HHuginn::TYPE::INTEGER ), hcore::call( &huginn_builtin::integer, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `integer` type" );
	register_builtin_function( type_name( HHuginn::TYPE::REAL ), hcore::call( &huginn_builtin::real, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `real`" );
	register_builtin_function( type_name( HHuginn::TYPE::STRING ), hcore::call( &huginn_builtin::string, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `string`" );
	register_builtin_function( type_name( HHuginn::TYPE::NUMBER ), hcore::call( &huginn_builtin::number, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `number`" );
	register_builtin_function( type_name( HHuginn::TYPE::BOOLEAN ), hcore::call( &huginn_builtin::boolean, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `boolean`" );
	register_builtin_function( type_name( HHuginn::TYPE::CHARACTER ), hcore::call( &huginn_builtin::character, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `character`" );
	register_builtin_function( BUILTIN::SIZE, hcore::call( &huginn_builtin::size, _1, _2, _3, _4 ), "( *expr* ) - get a size of given expression *expr*, e.g: a number of elements in a collection" );
	register_builtin_function( BUILTIN::TYPE, hcore::call( &huginn_builtin::type, _1, _2, _3, _4 ), "( *expr* ) - get a type of given expression *expr*" );
	register_builtin_function( BUILTIN::COPY, hcore::call( &huginn_builtin::copy, _1, _2, _3, _4 ), "( *ref* ) - make a deep copy of a value given given by *ref*" );
	register_builtin_function( BUILTIN::OBSERVE, hcore::call( &huginn_builtin::observe, _1, _2, _3, _4 ), "( *ref* ) - create an `*observer*` for a value given by *ref*" );
	register_builtin_function( BUILTIN::USE, hcore::call( &huginn_builtin::use, _1, _2, _3, _4 ), "( *observer* ) - get a reference to a value from given *observer*" );
	register_builtin_function( type_name( HHuginn::TYPE::TUPLE ), hcore::call( &huginn_builtin::tuple, _1, _2, _3, _4 ), "([ *items...* ]) - create a `tuple` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::LIST ), hcore::call( &huginn_builtin::list, _1, _2, _3, _4 ), "([ *items...* ]) - create a `list` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::DEQUE ), hcore::call( &huginn_builtin::deque, _1, _2, _3, _4 ), "([ *items...* ]) - create a `deque` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::DICT ), hcore::call( &huginn_builtin::dict, _1, _2, _3, _4 ), "create empty a `dict` object" );
	register_builtin_function( type_name( HHuginn::TYPE::ORDER ), hcore::call( &huginn_builtin::order, _1, _2, _3, _4 ), "([ *items...* ]) - create an `order` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::LOOKUP ), hcore::call( &huginn_builtin::lookup, _1, _2, _3, _4 ), "create empty a `lookup` object" );
	register_builtin_function( type_name( HHuginn::TYPE::SET ), hcore::call( &huginn_builtin::set, _1, _2, _3, _4 ), "([ *items...* ]) - create a `set` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::BLOB ), hcore::call( &huginn_builtin::blob, _1, _2, _3, _4 ), "( *size* ) - create a `blob` of *size* bytes size" );
	register_builtin_function( "print", hcore::call( &huginn_builtin::print, _1, _2, _3, _4 ), "( *str* ) - print a message given by *str* to interpreter's standard output" );
	register_builtin_function( "input", hcore::call( &huginn_builtin::input, _1, _2, _3, _4 ), "read a line of text from interpreter's standard input" );
	register_builtin_function( KEYWORD::ASSERT, hcore::call( &huginn_builtin::assert, _1, _2, _3, _4 ), "( *condition*[, *message*] ) - ensure *condition* is met or bailout with *message*" );
	for ( HHuginn::HClass const* c : _coreClasses_ ) {
		register_builtin_function( c->name(), hcore::call( &huginn_builtin::invalid_instance, c->name(), _1, _2, _3, _4 ), "" );
	}
	_objectFactory->register_builtin_classes();
	return;
	M_EPILOG
}

inline yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, HHuginn::HClass const& huginnClass_ ) {
	M_PROLOG
	HRuntime const* runtime( huginnClass_.runtime() );
	stream_ << "class: " << huginnClass_.name();
	if ( huginnClass_.super() ) {
		stream_ << " : " << huginnClass_.super()->name();
	}
	HHuginn::field_identifiers_t newFields( huginnClass_.field_identifiers() );
	typedef HStack<HHuginn::HClass const*> hierarchy_t;
	hierarchy_t hierarchy;
	HHuginn::HClass const* super( huginnClass_.super() );
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
	for ( HHuginn::identifier_id_t f : derivedFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime->identifier_name( f ) << ( verbose ? "(derived)" : "" );
	}
	for ( HHuginn::identifier_id_t f : overriddenFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime->identifier_name( f ) << ( verbose ? "(overridden)" : "" );
	}
	for ( HHuginn::identifier_id_t f : newFields ) {
		if ( next ) {
			stream_ << ",";
		}
		next = true;
		stream_ << " " << runtime->identifier_name( f ) << ( verbose ? "(new)" : "" );
	}
	stream_ << ( next ? " " : "" ) << "}";
	return ( stream_ );
	M_EPILOG
}

void HRuntime::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _huginn->source_name( MAIN_FILE_ID ) << "'" << endl;
	for ( packages_t::value_type const& p : _packages ) {
		stream_ << "package: " << identifier_name( p.first ) << " = " << p.second->get_class()->name() << endl;
	}
	for ( HHuginn::HClass const* c : _coreClasses_ ) {
		stream_ << "class: " << c->name() << " {}" << endl;
	}
	for ( classes_t::value_type const& c : _classes ) {
		stream_ << *c.second << endl;
	}
	for ( functions_available_t::value_type const& f : _functionsAvailable ) {
		yaal::hcore::HString const& name( identifier_name( f ) );
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
	for ( HHuginn::HClass const* c : _coreClasses_ ) {
		if ( ! c->doc().is_empty() ) {
			stream_ << c->name() << ":" << c->doc() << endl;
		}
	}
	for ( classes_t::value_type const& c : _classes ) {
		HHuginn::HClass const& cls( *c.second );
		if ( ! cls.doc().is_empty() ) {
			stream_ << cls.name() << ":" << cls.doc() << endl;
		}
		for ( HHuginn::field_identifiers_t::value_type const& f : cls.field_identifiers() ) {
			HString const& doc( cls.doc( f ) );
			if ( ! doc.is_empty() ) {
				stream_ << cls.name() << "." << identifier_name( f ) << ":";
				int fi( cls.field_index( f, HHuginn::HClass::MEMBER_TYPE::STATIC ) );
				HHuginn::type_id_t tid( cls.field( fi )->type_id() );
				if ( ( doc.front() != '(' ) && ( tid == HHuginn::TYPE::METHOD ) ) {
					stream_ << "() - ";
				}
				stream_ << doc << endl;
			}
		}
	}
	for ( functions_available_t::value_type const& fa : _functionsAvailable ) {
		HHuginn::HFunctionReference const& f( *static_cast<HHuginn::HFunctionReference const*>( _functionsStore.at( fa ).raw() ) );
		if ( ! f.doc().is_empty() && ( ( _classes.count( fa ) == 0 ) || _classes.at( fa )->doc().is_empty() ) ) {
			stream_ << identifier_name( fa ) << ":" << f.doc() << endl;
		}
	}
	return;
	M_EPILOG
}

yaal::hcore::HString HRuntime::suggestion( HHuginn::identifier_id_t identifier_ ) const {
	M_PROLOG
	HString identifier( identifier_name( identifier_ ) );
	HString s;
	int minDiff( meta::max_signed<int>::value );
	for ( yaal::hcore::HString const& i : _identifierNames ) {
		int diff( string::distance( i, identifier, string::DISTANCE_METRIC::QWERTY ) );
		if ( ( diff > 0 ) && ( diff < minDiff ) ) {
			minDiff = diff;
			s = i;
		}
	}
	return ( s );
	M_EPILOG
}

yaal::hcore::HString HRuntime::suggestion( HHuginn::HClass const* class_, HHuginn::identifier_id_t identifierId_ ) const {
	M_PROLOG
	HString identifierName( identifier_name( identifierId_ ) );
	HString s;
	int minDiff( meta::max_signed<int>::value );
	while ( class_ ) {
		for ( HHuginn::identifier_id_t identifierId : class_->field_identifiers() ) {
			HString const& name( identifier_name( identifierId ) );
			int diff( string::distance( name, identifierName, string::DISTANCE_METRIC::QWERTY ) );
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
	for ( functions_store_t::value_type const& f : _functionsStore ) {
		if ( static_cast<HHuginn::HFunctionReference const*>( f.second.raw() )->function().id() == id_ ) {
			name = &identifier_name( f.first );
			break;
		}
	}
	return ( *name );
	M_EPILOG
}

yaal::hcore::HString const& HRuntime::package_name( HHuginn::HClass const* class_ ) const {
	M_PROLOG
	static yaal::hcore::HString const unknown( "unknown" );
	yaal::hcore::HString const* name( &unknown );
	for ( packages_t::value_type const& p : _packages ) {
		if ( p.second->get_class() == class_ ) {
			name = &identifier_name( p.first );
			break;
		}
	}
	return ( *name );
	M_EPILOG
}

HIntrospecteeInterface::call_stack_t HRuntime::get_call_stack( huginn::HThread* thread_ ) {
	M_PROLOG
	call_stack_t callStack;
	HFrame* f( thread_->current_frame() );
	int position( f->position() );
	while ( f ) {
		if ( f->type() == HFrame::TYPE::FUNCTION ) {
			int fileId( f->file_id() );
			HHuginn::HErrorCoordinate ec( _huginn->get_coordinate( fileId, position ) );
			HFunction const* func( static_cast<HFunction const*>( f->statement() ) );
			callStack.emplace_back( _huginn->source_name( fileId ), ec.line(), ec.column(), identifier_name( func->name() ) );
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

HIntrospecteeInterface::call_stack_t HRuntime::do_get_call_stack( void ) {
	M_PROLOG
	return ( get_call_stack( current_thread() ) );
	M_EPILOG
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

}

}

}

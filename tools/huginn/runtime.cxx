/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  runtime.cxx - this file is integral part of `yaal' project.

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
			{ type_name( HHuginn::TYPE::INTEGER ), BUILTIN::INTEGER_IDENTIFIER },
			{ type_name( HHuginn::TYPE::REAL ), BUILTIN::REAL_IDENTIFIER },
			{ type_name( HHuginn::TYPE::NUMBER ), BUILTIN::NUMBER_IDENTIFIER },
			{ type_name( HHuginn::TYPE::STRING ), BUILTIN::STRING_IDENTIFIER },
			{ type_name( HHuginn::TYPE::CHARACTER ), BUILTIN::CHARACTER_IDENTIFIER },
			{ type_name( HHuginn::TYPE::BOOLEAN ), BUILTIN::BOOLEAN_IDENTIFIER },
			{ BUILTIN::SIZE, BUILTIN::SIZE_IDENTIFIER },
			{ BUILTIN::TYPE, BUILTIN::TYPE_IDENTIFIER },
			{ BUILTIN::COPY, BUILTIN::COPY_IDENTIFIER },
			{ BUILTIN::OBSERVE, BUILTIN::OBSERVE_IDENTIFIER },
			{ BUILTIN::USE, BUILTIN::USE_IDENTIFIER },
			{ type_name( HHuginn::TYPE::LIST ), BUILTIN::LIST_IDENTIFIER },
			{ type_name( HHuginn::TYPE::DEQUE ), BUILTIN::DEQUE_IDENTIFIER },
			{ type_name( HHuginn::TYPE::DICT ), BUILTIN::DICT_IDENTIFIER },
			{ type_name( HHuginn::TYPE::LOOKUP ), BUILTIN::LOOKUP_IDENTIFIER },
			{ type_name( HHuginn::TYPE::ORDER ), BUILTIN::ORDER_IDENTIFIER },
			{ type_name( HHuginn::TYPE::SET ), BUILTIN::SET_IDENTIFIER },
			{ STANDARD_FUNCTIONS::MAIN, STANDARD_FUNCTIONS::MAIN_IDENTIFIER },
			{ _noneClass_.name(), TYPE_NONE_IDENTIFIER },
			{ _observerClass_.name(), TYPE_OBSERVER_IDENTIFIER },
			{ _referenceClass_.name(), TYPE_REFERENCE_IDENTIFIER },
			{ _functionReferenceClass_.name(), TYPE_FUNCTION_REFERENCE_IDENTIFIER },
			{ _objectReferenceClass_.name(), TYPE_OBJECT_REFERENCE_IDENTIFIER },
			{ _methodClass_.name(), TYPE_METHOD_IDENTIFIER },
			{ _boundMethodClass_.name(), TYPE_BOUND_METHOD_IDENTIFIER },
			{ _unknownClass_.name(), TYPE_UNKNOWN_IDENTIFIER }
		} )
	, _identifierNames( {
			KEYWORD::CONSTRUCTOR,
			KEYWORD::DESTRUCTOR,
			KEYWORD::THIS,
			KEYWORD::SUPER,
			KEYWORD::ASSERT,
			type_name( HHuginn::TYPE::INTEGER ),
			type_name( HHuginn::TYPE::REAL ),
			type_name( HHuginn::TYPE::NUMBER ),
			type_name( HHuginn::TYPE::STRING ),
			type_name( HHuginn::TYPE::CHARACTER ),
			type_name( HHuginn::TYPE::BOOLEAN ),
			BUILTIN::SIZE,
			BUILTIN::TYPE,
			BUILTIN::COPY,
			BUILTIN::OBSERVE,
			BUILTIN::USE,
			type_name( HHuginn::TYPE::LIST ),
			type_name( HHuginn::TYPE::DEQUE ),
			type_name( HHuginn::TYPE::DICT ),
			type_name( HHuginn::TYPE::LOOKUP ),
			type_name( HHuginn::TYPE::ORDER ),
			type_name( HHuginn::TYPE::SET ),
			STANDARD_FUNCTIONS::MAIN,
			_noneClass_.name(),
			_observerClass_.name(),
			_referenceClass_.name(),
			_functionReferenceClass_.name(),
			_objectReferenceClass_.name(),
			_methodClass_.name(),
			_boundMethodClass_.name(),
			_unknownClass_.name()
		} )
	, _objectFactory( new HObjectFactory( this ) )
	, _none( make_pointer<HHuginn::HValue>( &_noneClass_ ) )
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
	, _maxLocalVariableCount( 0 ) {
}

void HRuntime::reset( void ) {
	M_PROLOG
	_result.reset();
	_argv->clear();
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

huginn::HFrame* HRuntime::current_frame( void ) {
	M_PROLOG
	return ( current_thread()->current_frame() );
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

void HRuntime::register_class_low( class_t class_, HHuginn::ACCESS classConstructorAccess_ ) {
	M_PROLOG
	if ( _classes.insert( make_pair( class_->identifier_id(), class_ ) ).second ) {
		_dependencies.push_back( class_ );
		HHuginn::identifier_id_t identifier( class_->identifier_id() );
		HHuginn::function_t function( hcore::call( classConstructorAccess_ != HHuginn::ACCESS::PRIVATE ? &HHuginn::HClass::create_instance : &HHuginn::HClass::access_violation, class_.raw(), _1, _2, _3, _4 ) );
		HHuginn::value_t functionReference( _objectFactory->create_function_reference( identifier, function, "automatic constructor for class: `"_ys.append( identifier_name( identifier ) ).append( "`" ) ) );
		_functionsStore.insert( make_pair( identifier, functionReference ) );
	}
	if ( classConstructorAccess_ == HHuginn::ACCESS::PUBLIC ) {
		_functionsAvailable.insert( class_->identifier_id() );
	}
	return;
	M_EPILOG
}

void HRuntime::register_function( identifier_id_t identifier_, function_t function_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	_functionsStore[ identifier_ ] = _objectFactory->create_function_reference( identifier_, function_, doc_ );
	_functionsAvailable.insert( identifier_ );
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

HHuginn::class_t HRuntime::create_class( identifier_id_t identifier_, HHuginn::HClass const* base_, field_definitions_t const& fieldDefinitions_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			this,
			type_id_t( _idGenerator ),
			identifier_,
			base_,
			fieldDefinitions_,
			doc_
		)
	);
	++ _idGenerator;
	return ( c );
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( yaal::hcore::HString const& name_, HHuginn::HClass const* base_, field_definitions_t const& fieldDefinitions_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	return ( create_class( identifier_id( name_ ), base_, fieldDefinitions_, doc_ ) );
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( class_constructor_t const& classConstructor_ ) {
	M_PROLOG
	class_t c( classConstructor_( type_id_t( _idGenerator ) ) );
	++ _idGenerator;
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
	if ( _argv->size() > 0 ) {
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

HHuginn::value_t value( HHuginn::value_t value_, HString name_, HThread*, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, position_ );
	return ( value_ );
	M_EPILOG
}

HHuginn::value_t instance( HHuginn::HClass const* class_, HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	return ( class_->create_instance( thread_, object_, values_, position_ ) );
	M_EPILOG
}

}
}

HHuginn::class_t HRuntime::make_package( yaal::hcore::HString const& name_, HRuntime const& context_ ) {
	M_PROLOG
	HHuginn::field_definitions_t fds;
	/* Erasing from lookup invalidated .end() */
	for ( packages_t::iterator it( _packages.begin() ); it != _packages.end(); ) {
		if ( context_._packages.find( it->first ) == context_._packages.end() ) {
			fds.emplace_back(
				identifier_name( it->first ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &package::value, it->second, identifier_name( it->first ), _1, _2, _3, _4 ) ),
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
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &package::instance, c.second.raw(), _1, _2, _3, _4 ) ),
				"access class "_ys.append( identifier_name( c.first ) ).append( " imported in submodule" )
			);
		}
	}
	for ( functions_available_t::value_type const& fi : _functionsAvailable ) {
		if ( context_._functionsAvailable.find( fi ) == context_._functionsAvailable.end() ) {
			HHuginn::HFunctionReference const* fr( static_cast<HHuginn::HFunctionReference const*>( _functionsStore.at( fi ).raw() ) );
			fds.emplace_back(
				identifier_name( fi ),
				make_pointer<HHuginn::HClass::HMethod>( fr->function() ),
				! fr->doc().is_empty() ? fr->doc() : "access function "_ys.append( identifier_name( fi ) ).append( " imported in submodule" )
			);
		}
	}
	HString doc( _huginn->get_comment( 1 ) );
	HHuginn::class_t c( create_class( name_, nullptr, fds, ! doc.is_empty() ? doc : "The `"_ys.append( name_ ).append( "` is an user defined submodule." ) ) );
	_huginn->register_class( c );
	return ( c );
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

HHuginn::value_t HRuntime::call( identifier_id_t identifier_, values_t const& values_, int position_ ) {
	M_PROLOG
	value_t res;
	if ( _functionsAvailable.count( identifier_ ) > 0 ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = static_cast<HHuginn::HFunctionReference*>( _functionsStore.at( identifier_ ).raw() )->function()( t->second.raw(), nullptr, values_, position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Function `"_ys.append( identifier_name( identifier_ ) ).append( "(...)' is not defined." ), position_ );
	}
	return ( res );
	M_EPILOG
}

void HRuntime::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	_argv->push_back( _objectFactory->create_string( arg_ ) );
	return;
	M_EPILOG
}

void HRuntime::clear_arguments( void ) {
	M_PROLOG
	_argv->clear();
	return;
	M_EPILOG
}

namespace huginn_builtin {

HHuginn::value_t string( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "string", values_, 1, 1, position_ );
	return ( value_builtin::string( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t integer( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "integer", values_, 1, 1, position_ );
	return ( value_builtin::integer( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t real( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "real", values_, 1, 1, position_ );
	return ( value_builtin::real( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t boolean( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "boolean", values_, 1, 1, position_ );
	return ( value_builtin::boolean( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t character( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "character", values_, 1, 1, position_ );
	return ( value_builtin::character( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t number( HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "number", values_, 1, 1, position_ );
	return ( value_builtin::number( thread_, values_[0], position_ ) );
	M_EPILOG
}

HHuginn::value_t size( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "size", values_, 1, 1, position_ );
	HHuginn::value_t const& val( values_.front() );
	HHuginn::HValue const* v( val.raw() );
	int long long s( 0 );
	HHuginn::HIterable const* iterable( dynamic_cast<HHuginn::HIterable const*>( v ) );
	if ( iterable ) {
		s = iterable->size();
	} else {
		if ( HHuginn::HObject const* o = dynamic_cast<HHuginn::HObject const*>( v ) ) {
			s = get_integer( value_builtin::integer( thread_, o->call_method( thread_, val, "get_size", HHuginn::values_t(), position_ ), position_ ) );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Getting size of `"_ys.append( v->get_class()->name() ).append( "'s is not supported." ),
				position_
			);
		}
	}
	return ( thread_->object_factory().create_integer( s ) );
	M_EPILOG
}

HHuginn::value_t type( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	HHuginn::identifier_id_t id( v->get_class()->identifier_id() );
	HHuginn::value_t* f( thread_->runtime().get_function( id, true ) );
	return ( *f );
	M_EPILOG
}

HHuginn::value_t copy( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( v->clone( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t observe( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "observe", values_, 1, 1, position_ );
	HHuginn::value_t const& v( values_.front() );
	if ( v->type_id() == HHuginn::TYPE::OBSERVER ) {
		throw HHuginn::HHuginnRuntimeException(
			"Making an *observer* out of an *observer*.",
			position_
		);
	}
	return ( make_pointer<HHuginn::HObserver>( v ) );
	M_EPILOG
}

HHuginn::value_t use( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( "use", values_, { HHuginn::TYPE::OBSERVER }, position_ );
	HHuginn::HObserver const* o( static_cast<HHuginn::HObserver const*>( values_.front().raw() ) );
	HHuginn::value_t v( o->value() );
	if ( !v ) {
		v = thread_->runtime().none_value();
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t list( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_list() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t deque( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_deque() );
	HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		d->push_back( e );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t dict( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict", values_, 0, 0, position_ );
	return ( thread_->object_factory().create_dict() );
	M_EPILOG
}

HHuginn::value_t order( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_order() );
	HHuginn::HOrder* o( static_cast<HHuginn::HOrder*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		o->insert( e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t lookup( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup", values_, 0, 0, position_ );
	return ( thread_->object_factory().create_lookup() );
	M_EPILOG
}

HHuginn::value_t set( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_set() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( thread_, e, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t print( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "print", values_, 1, 1, position_ );
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
			position_
		);
	}
	out << flush;
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t input( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "input", values_, 0, 0, position_ );
	yaal::hcore::HString l;
	int len( static_cast<int>( thread_->runtime().huginn()->input_stream().read_until( l, HStreamInterface::eols, false ) ) );
	return ( len > 0 ? thread_->object_factory().create_string( l ) : thread_->runtime().none_value() );
	M_EPILOG
}

inline HHuginn::value_t assert( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "assert";
	verify_arg_count( name, values_, 2, 3, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::BOOLEAN, ARITY::MULTIPLE, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, position_ );
	if ( ! get_boolean( values_[0] ) ) {
		int argc( static_cast<int>( values_.get_size() ) );
		HString message( get_string( values_[argc - 1] ) );
		if ( argc > 2 ) {
			message.append( " " ).append( get_string( values_[1] ) );
		}
		throw HHuginn::HHuginnRuntimeException( message, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t invalid_instance( yaal::hcore::HString const&, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t invalid_instance( yaal::hcore::HString const& name_, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int position_ ) {
	M_PROLOG
	/*
	 * __attribute__(( noreturn )) causes problems with clang.
	 * Hence this workaround.
	 * Condition in following if shall always evaluate to `true'.
	 */
	if ( position_ >= 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Direct creation of instances of `"_ys.append( name_ ).append( "' is not allowed." ), position_ );
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
HHuginn::HClass const* _coreClasses_[8];
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
			&_boundMethodClass_,
			&_unknownClass_
		};
		static_assert( sizeof ( coreClassesInit ) == sizeof ( _coreClasses_ ), "invalid core classes initializer size" );
		copy( begin( coreClassesInit ), end( coreClassesInit ), begin( _coreClasses_ ) );
	}
	M_ENSURE( identifier_id( KEYWORD::CONSTRUCTOR ) == KEYWORD::CONSTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::DESTRUCTOR ) == KEYWORD::DESTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::THIS ) == KEYWORD::THIS_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::SUPER ) == KEYWORD::SUPER_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::ASSERT ) == KEYWORD::ASSERT_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::INTEGER ) ) == BUILTIN::INTEGER_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::REAL ) ) == BUILTIN::REAL_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::NUMBER ) ) == BUILTIN::NUMBER_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::STRING ) ) == BUILTIN::STRING_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::CHARACTER ) ) == BUILTIN::CHARACTER_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::BOOLEAN ) ) == BUILTIN::BOOLEAN_IDENTIFIER );
	M_ENSURE( identifier_id( BUILTIN::SIZE ) == BUILTIN::SIZE_IDENTIFIER );
	M_ENSURE( identifier_id( BUILTIN::TYPE ) == BUILTIN::TYPE_IDENTIFIER );
	M_ENSURE( identifier_id( BUILTIN::COPY ) == BUILTIN::COPY_IDENTIFIER );
	M_ENSURE( identifier_id( BUILTIN::OBSERVE ) == BUILTIN::OBSERVE_IDENTIFIER );
	M_ENSURE( identifier_id( BUILTIN::USE ) == BUILTIN::USE_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::LIST ) ) == BUILTIN::LIST_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::DEQUE ) ) == BUILTIN::DEQUE_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::DICT ) ) == BUILTIN::DICT_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::LOOKUP ) ) == BUILTIN::LOOKUP_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::ORDER ) ) == BUILTIN::ORDER_IDENTIFIER );
	M_ENSURE( identifier_id( type_name( HHuginn::TYPE::SET ) ) == BUILTIN::SET_IDENTIFIER );
	M_ENSURE( identifier_id( _noneClass_.name() ) == TYPE_NONE_IDENTIFIER );
	M_ENSURE( identifier_id( _observerClass_.name() ) == TYPE_OBSERVER_IDENTIFIER );
	M_ENSURE( identifier_id( _referenceClass_.name() ) == TYPE_REFERENCE_IDENTIFIER );
	M_ENSURE( identifier_id( _functionReferenceClass_.name() ) == TYPE_FUNCTION_REFERENCE_IDENTIFIER );
	M_ENSURE( identifier_id( _objectReferenceClass_.name() ) == TYPE_OBJECT_REFERENCE_IDENTIFIER );
	M_ENSURE( identifier_id( _methodClass_.name() ) == TYPE_METHOD_IDENTIFIER );
	M_ENSURE( identifier_id( _boundMethodClass_.name() ) == TYPE_BOUND_METHOD_IDENTIFIER );
	M_ENSURE( identifier_id( _unknownClass_.name() ) == TYPE_UNKNOWN_IDENTIFIER );
	M_ENSURE( identifier_id( STANDARD_FUNCTIONS::MAIN ) == STANDARD_FUNCTIONS::MAIN_IDENTIFIER );
	register_builtin_function( type_name( HHuginn::TYPE::INTEGER ), hcore::call( &huginn_builtin::integer, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `integer` type" );
	register_builtin_function( type_name( HHuginn::TYPE::REAL ), hcore::call( &huginn_builtin::real, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `real`" );
	register_builtin_function( type_name( HHuginn::TYPE::STRING ), hcore::call( &huginn_builtin::string, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `string`" );
	register_builtin_function( type_name( HHuginn::TYPE::NUMBER ), hcore::call( &huginn_builtin::number, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `number`" );
	register_builtin_function( type_name( HHuginn::TYPE::BOOLEAN ), hcore::call( &huginn_builtin::boolean, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `boolean`" );
	register_builtin_function( type_name( HHuginn::TYPE::CHARACTER ), hcore::call( &huginn_builtin::character, _1, _2, _3, _4 ), "( *val* ) - convert *val* value to `character`" );
	register_builtin_function( BUILTIN::SIZE, hcore::call( &huginn_builtin::size, _1, _2, _3, _4 ), "( *expr* ) - get size of given expression *expr*, e.g: a number of elements in a collection" );
	register_builtin_function( BUILTIN::TYPE, hcore::call( &huginn_builtin::type, _1, _2, _3, _4 ), "( *expr* ) - get type of given expression *expr*" );
	register_builtin_function( BUILTIN::COPY, hcore::call( &huginn_builtin::copy, _1, _2, _3, _4 ), "( *ref* ) - make a deep copy of a value given given by *ref*" );
	register_builtin_function( BUILTIN::OBSERVE, hcore::call( &huginn_builtin::observe, _1, _2, _3, _4 ), "( *ref* ) - create an `*observer*` for a value given by *ref*" );
	register_builtin_function( BUILTIN::USE, hcore::call( &huginn_builtin::use, _1, _2, _3, _4 ), "( *observer* ) - get a reference to a value from given *observer*" );
	register_builtin_function( type_name( HHuginn::TYPE::LIST ), hcore::call( &huginn_builtin::list, _1, _2, _3, _4 ), "([ *items...* ]) - create `list` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::DEQUE ), hcore::call( &huginn_builtin::deque, _1, _2, _3, _4 ), "([ *items...* ]) - create `deque` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::DICT ), hcore::call( &huginn_builtin::dict, _1, _2, _3, _4 ), "create empty `dict` object" );
	register_builtin_function( type_name( HHuginn::TYPE::ORDER ), hcore::call( &huginn_builtin::order, _1, _2, _3, _4 ), "([ *items...* ]) - create `order` collection from *items...*" );
	register_builtin_function( type_name( HHuginn::TYPE::LOOKUP ), hcore::call( &huginn_builtin::lookup, _1, _2, _3, _4 ), "create empty `lookup` object" );
	register_builtin_function( type_name( HHuginn::TYPE::SET ), hcore::call( &huginn_builtin::set, _1, _2, _3, _4 ), "([ *items...* ]) - create `set` collection from *items...*" );
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
	stream_ << "Huginn VM state for `" << _huginn->source_name() << "'" << endl;
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
			if ( ! cls.doc( f ).is_empty() ) {
				stream_ << cls.name() << "." << identifier_name( f ) << ":" << cls.doc( f ) << endl;
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
		int diff( string::distance::levenshtein_damerau( i, identifier ) );
		if ( ( diff > 0 ) && ( diff < minDiff ) ) {
			minDiff = diff;
			s = i;
		}
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

}

}

}


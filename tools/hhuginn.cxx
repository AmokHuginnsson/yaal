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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"
#include "hcore/hfile.hxx"
#include "tools/huginn/source.hxx"
#include "tools/huginn/objectfactory.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/expression.hxx"
#include "tools/huginn/value_builtin.hxx"
#include "tools/huginn/scope.hxx"
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/keyword.hxx"
#include "tools/huginn/packagefactory.hxx"
#include "streamtools.hxx"

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

namespace huginn {

HHuginn::HClass const _noneClass_( HHuginn::TYPE::NONE );
HHuginn::HClass const _observerClass_( HHuginn::TYPE::OBSERVER );
HHuginn::HClass const _referenceClass_( HHuginn::TYPE::REFERENCE );
HHuginn::HClass const _functionReferenceClass_( HHuginn::TYPE::FUNCTION_REFERENCE );
HHuginn::HClass const _objectReferenceClass_( HHuginn::TYPE::OBJECT_REFERENCE );
HHuginn::HClass const _methodClass_( HHuginn::TYPE::METHOD );
HHuginn::HClass const _unknownClass_( HHuginn::TYPE::UNKNOWN );

char const* _errMsgHHuginn_[ 10 ] = {
	_( "Operands are not summable: " ),
	_( "Operands are not substractable: " ),
	_( "Operands are not multipliable: " ),
	_( "Operands are not divisible: " ),
	_( "Operands are not exponentionable." ),
	_( "Operands are not comparable." ),
	_( "Operands are not boolean values." ),
	_( "Operand is not a boolean value." ),
	_( "Subscript is not an integer." )
};

namespace exception {

extern HHuginn::class_t _exceptionClass_;

}

}

HHuginn::compiler_setup_t const HHuginn::COMPILER::DEFAULT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_STRICT = HHuginn::compiler_setup_t::new_flag();
HHuginn::compiler_setup_t const HHuginn::COMPILER::BE_SLOPPY = HHuginn::compiler_setup_t::new_flag();

HHuginn::HHuginnRuntimeException::HHuginnRuntimeException( yaal::hcore::HString const& message_, int position_ )
	: _message( message_ )
	, _position( position_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HHuginnRuntimeException::message( void ) const {
	return ( _message );
}

int HHuginn::HHuginnRuntimeException::position( void ) const {
	return ( _position );
}

HHuginn::HObjectReference::HObjectReference( value_t const& value_, int upCastLevel_, bool upCast_, int position_ )
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
				"`"_ys.append( s->name() ).append( "' does not have superclass." ),
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

HHuginn::value_t HHuginn::HObjectReference::field( int index_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( _object.raw() ) );
	value_t v;
	if ( _class != o->get_class() ) {
		v = _class->get_defaults()[index_];
	} else {
		v = o->field_ref( index_ );
	}
	if ( v->type_id() == TYPE::METHOD ) {
		v = make_pointer<HClass::HBoundMethod>( *static_cast<HClass::HMethod*>( v.raw() ), _object );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObjectReference::do_clone( HHuginn* huginn_ ) const {
	M_PROLOG
	return ( make_pointer<HObjectReference>( _object->clone( huginn_ ), _class ) );
	M_EPILOG
}

HHuginn::flag_t HHuginn::_grammarVerified{ false };

void HHuginn::disable_grammar_verification( void ) {
	_grammarVerified.store( true );
}

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY )
	, _idGenerator( static_cast<type_id_t::value_type>( TYPE::NOT_BOOLEAN ) )
	, _identifierIds( {
			{ KEYWORD::CONSTRUCTOR, KEYWORD::CONSTRUCTOR_IDENTIFIER },
			{ KEYWORD::DESTRUCTOR, KEYWORD::DESTRUCTOR_IDENTIFIER },
			{ KEYWORD::THIS, KEYWORD::THIS_IDENTIFIER },
			{ KEYWORD::SUPER, KEYWORD::SUPER_IDENTIFIER },
			{ KEYWORD::ASSERT, KEYWORD::ASSERT_IDENTIFIER }
		} )
	, _identifierNames( {
			KEYWORD::CONSTRUCTOR,
			KEYWORD::DESTRUCTOR,
			KEYWORD::THIS,
			KEYWORD::SUPER,
			KEYWORD::ASSERT
		} )
	, _objectFactory( new HObjectFactory( this ) )
	, _none( make_pointer<HHuginn::HValue>( &_noneClass_ ) )
	, _true( _objectFactory->create_boolean( true ) )
	, _false( _objectFactory->create_boolean( false ) )
	, _classes()
	, _functions()
	, _source( make_resource<HSource>() )
	, _compiler( make_resource<OCompiler>( this ) )
	, _engine( make_engine(), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
	, _threads()
	, _packages()
	, _argv( _objectFactory->create_list() )
	, _result()
	, _errorMessage()
	, _errorPosition( -1 )
	, _maxLocalVariableCount( 0 )
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr ) {
	M_PROLOG
	_grammarVerified.store( true );
	register_builtins();
	_objectFactory->register_builtin_classes();
	return;
	M_EPILOG
}

HHuginn::~HHuginn( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, yaal::hcore::HString const& name_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_source->load( stream_, name_, skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_, int skippedLines_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_source->load( stream_, hcore::HString(), skippedLines_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::preprocess( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::LOADED );
	_source->preprocess();
	_state = STATE::PREPROCESSED;
	return;
	M_EPILOG
}

bool HHuginn::parse( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PREPROCESSED );
	bool ok( _engine.parse( _source->begin(), _source->end() ) );
	if ( ! ok ) {
		_errorMessage = _engine.error_messages()[0];
		_errorPosition = _engine.error_position();
	} else {
		_state = STATE::PARSED;
	}
	return ( ok );
	M_EPILOG
}

void HHuginn::set_max_local_variable_count( int maxLocalVariableCount_ ) {
	M_PROLOG
	M_ASSERT( maxLocalVariableCount_ >= 0 );
	_maxLocalVariableCount = maxLocalVariableCount_;
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::commit_class( identifier_id_t identifierId_ ) {
	M_PROLOG
	yaal::hcore::HString const& name( identifier_name( identifierId_ ) );
	HClass const* c( nullptr );
	classes_t::const_iterator it( _classes.find( identifierId_ ) );
	OCompiler::OClassContext* cc( _compiler->_submittedClasses.at( identifierId_ ).get() );
	for ( packages_t::value_type const& p : _packages ) {
		if ( identifierId_ == p.second->get_class()->identifier_id() ) {
			throw HHuginnRuntimeException( "Package of the same name `"_ys.append( name ).append( "' is already imported." ), cc->_position.get() );
		}
	}
	if ( _packages.count( identifierId_ ) > 0 ) {
		throw HHuginnRuntimeException( "Package alias of the same name `"_ys.append( name ).append( "' is already defined." ), cc->_position.get() );
	}
	if ( it != _classes.end() ) {
		c = it->second.get();
	} else {
		_compiler->track_name_cycle( identifierId_ );
		if ( _functions.count( identifierId_ ) > 0 ) {
			throw HHuginnRuntimeException( "Function of the same name `"_ys.append( name ).append( "' is already defined." ), cc->_position.get() );
		}
		HClass const* super( nullptr );
		if ( cc->_baseName != INVALID_IDENTIFIER ) {
			super = commit_class( cc->_baseName );
		}
		field_definitions_t fieldDefinitions;
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator ti( _threads.find( threadId ) );
		M_ASSERT( ti != _threads.end() );
		huginn::HThread* t( ti->second.raw() );
		HFrame* frame( t->current_frame() );
		for ( int i( 0 ), size( static_cast<int>( cc->_fieldNames.get_size() ) ); i < size; ++ i ) {
			OCompiler::OClassContext::expressions_t::const_iterator f( cc->_fieldDefinitions.find( i ) );
			if ( f != cc->_fieldDefinitions.end() ) {
				f->second->execute( t );
				fieldDefinitions.emplace_back( cc->_fieldNames[i], frame->result() );
			} else {
				OCompiler::OClassContext::methods_t::const_iterator m( cc->_methods.find( i ) );
				M_ASSERT( m != cc->_methods.end() );
				fieldDefinitions.emplace_back( cc->_fieldNames[i], make_pointer<HClass::HMethod>( m->second ) );
			}
		}
		c = _classes.insert( make_pair( identifierId_, make_pointer<HClass>( this, type_id_t( _idGenerator ), cc->_classIdentifier, super, fieldDefinitions ) ) ).first->second.get();
		_functions.insert( make_pair( identifierId_, hcore::call( &HHuginn::HClass::create_instance, c, _1, _2, _3, _4 ) ) );
		++ _idGenerator;
	}
	return ( c );
	M_EPILOG
}

void HHuginn::register_class( class_t class_ ) {
	M_PROLOG
	M_ENSURE( _state != STATE::COMPILED );
	_classes.insert( make_pair( class_->identifier_id(), class_ ) );
	_functions.insert( make_pair( class_->identifier_id(), hcore::call( &HHuginn::HClass::create_instance, class_.raw(), _1, _2, _3, _4 ) ) );
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

HHuginn::identifier_id_t HHuginn::identifier_id( yaal::hcore::HString const& name_ ) {
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

yaal::hcore::HString const& HHuginn::identifier_name( identifier_id_t id_ ) const {
	M_PROLOG
	static yaal::hcore::HString const INVALID_IDENTIFIER_NAME = "*invalid_identifier*";
	return ( id_ != INVALID_IDENTIFIER ? _identifierNames[id_.get()] : INVALID_IDENTIFIER_NAME );
	M_EPILOG
}

void HHuginn::finalize_compilation( compiler_setup_t compilerSetup_ ) {
	M_PROLOG
	for ( OCompiler::submitted_imports_t::value_type i : _compiler->_submittedImports ) {
		_packages.insert( make_pair( i.second, HPackageFactoryInstance::get_instance().create_package( this, identifier_name( i.first ) ) ) );
	}
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	t->create_function_frame( INVALID_STATEMENT_IDENTIFIER, nullptr, 0 );
	for ( OCompiler::submitted_classes_t::value_type const& sc : _compiler->_submittedClasses ) {
		commit_class( sc.first );
	}
	t->pop_frame();
	_compiler->_submittedClasses.clear();
	if ( compilerSetup_ & COMPILER::BE_STRICT ) {
		_compiler->detect_misuse();
	}
	_compiler->resolve_symbols();
	_threads.clear();
	return;
	M_EPILOG
}

bool HHuginn::compile( compiler_setup_t compilerSetup_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED );
	if ( ( compilerSetup_ & COMPILER::BE_STRICT ) && ( compilerSetup_ & COMPILER::BE_SLOPPY ) ) {
		throw HHuginnException( "BE_STRICT and BE_SLOPPY flags are mutually exclusive." );
	}
	if ( ! ( ( compilerSetup_ & COMPILER::BE_STRICT ) || ( compilerSetup_ & COMPILER::BE_SLOPPY ) ) ) {
		compilerSetup_ |= COMPILER::BE_STRICT;
	}
	bool ok( false );
	try {
		_compiler->set_setup( compilerSetup_ );
		_engine.execute();
		finalize_compilation( compilerSetup_ );
		_state = STATE::COMPILED;
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		_errorMessage = e.message();
		_errorPosition = e.position();
	}
	return ( ok );
	M_EPILOG
}

bool HHuginn::execute( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED );
	values_t args;
	if ( _argv->size() > 0 ) {
		args.push_back( _argv );
	}
	bool ok( false );
	try {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
		_result = call( "main", args, 0 );
		t->flush_runtime_exception();
		ok = true;
	} catch ( HHuginnRuntimeException const& e ) {
		_errorMessage = e.message();
		_errorPosition = e.position();
	}
	_threads.clear();
	return ( ok );
	M_EPILOG
}

HHuginn::value_t HHuginn::call( yaal::hcore::HString const& name_, values_t const& values_, int position_ ) {
	M_PROLOG
	functions_t::const_iterator f( _functions.find( identifier_id( name_ ) ) );
	value_t res;
	if ( f != _functions.end() ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = f->second( t->second.raw(), nullptr, values_, position_ );
	} else {
		throw HHuginnRuntimeException( "Function `"_ys.append( name_ ).append( "(...)' is not defined." ), position_ );
	}
	return ( res );
	M_EPILOG
}

huginn::HThread* HHuginn::current_thread( void ) {
	M_PROLOG
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	threads_t::iterator t( _threads.find( threadId ) );
	return ( t != _threads.end() ? t->second.raw() : nullptr );
	M_EPILOG
}

huginn::HFrame* HHuginn::current_frame( void ) {
	M_PROLOG
	return ( current_thread()->current_frame() );
	M_EPILOG
}

HHuginn::value_t HHuginn::result( void ) const {
	return ( _result );
}

HHuginn::function_t* HHuginn::get_function( identifier_id_t identifierId_ ) {
	M_PROLOG
	HHuginn::function_t* f( nullptr );
	functions_t::iterator fi( _functions.find( identifierId_ ) );
	if ( fi != _functions.end() ) {
		f = &( fi->second );
	}
	return ( f );
	M_EPILOG
}

HHuginn::class_t HHuginn::get_class( identifier_id_t identifierId_ ) {
	M_PROLOG
	classes_t::const_iterator ci( _classes.find( identifierId_ ) );
	return ( ci != _classes.end() ? ci->second : class_t() );
	M_EPILOG
}

HHuginn::value_t* HHuginn::get_package( identifier_id_t identifierId_ ) {
	M_PROLOG
	HHuginn::value_t* v( nullptr );
	packages_t::iterator it( _packages.find( identifierId_ ) );
	if ( it != _packages.end() ) {
		v = &( it->second );
	}
	return ( v );
	M_EPILOG
}

yaal::hcore::HString HHuginn::source_name( void ) const {
	M_PROLOG
	return ( _source->name() );
	M_EPILOG
}

yaal::hcore::HString HHuginn::where( int position_ ) const {
	M_PROLOG
	hcore::HString w( source_name() );
	HHuginn::HErrorCoordinate ec( get_coordinate( position_ ) );
	w.append( ":" ).append( ec.line() ).append( ":" ).append( ec.column() );
	return ( w );
	M_EPILOG
}

int HHuginn::error_position( void ) const {
	M_PROLOG
	return ( _source->error_position( _errorPosition ) );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::error_coordinate( void ) const {
	M_PROLOG
	return ( _source->error_coordinate( error_position() ) );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::get_coordinate( int position_ ) const {
	M_PROLOG
	return ( _source->error_coordinate( _source->error_position( position_ ) ) );
	M_EPILOG
}

yaal::hcore::HString HHuginn::error_message( void ) const {
	M_PROLOG
	hcore::HString message( _source->name() );
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
	return ( _source->get_snippet( from_, len_ ) );
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

void HHuginn::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	_argv->push_back( _objectFactory->create_string( arg_ ) );
	return;
	M_EPILOG
}

void HHuginn::clear_arguments( void ) {
	M_PROLOG
	_argv->clear();
	return;
	M_EPILOG
}

void HHuginn::dump_preprocessed_source( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_source->dump_preprocessed( stream_ );
	return;
	M_EPILOG
}

inline yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, HHuginn::HClass const& huginnClass_ ) {
	M_PROLOG
	HHuginn const* huginn( huginnClass_.huginn() );
	stream_ << huginnClass_.name();
	if ( huginnClass_.super() ) {
		stream_ << " : " << huginnClass_.super()->name();
	}
	HHuginn::field_identifiers_t newFields( huginnClass_.field_identifiers() );
	typedef HStack<HHuginn::HClass const*> hierarhy_t;
	hierarhy_t hierarhy;
	HHuginn::HClass const* super( huginnClass_.super() );
	while ( super ) {
		hierarhy.push( super );
		super = super->super();
	}
	HHuginn::field_identifiers_t derivedFields;
	while ( ! hierarhy.is_empty() ) {
		for ( HHuginn::identifier_id_t f : hierarhy.top()->field_identifiers() ) {
			if ( find( derivedFields.begin(), derivedFields.end(), f ) == derivedFields.end() ) {
				derivedFields.push_back( f );
			}
		}
		hierarhy.pop();
	}
	HHuginn::field_identifiers_t overridenFields;
	for ( HHuginn::identifier_id_t f : derivedFields ) {
		if ( find( newFields.begin(), newFields.end(), f ) != newFields.end() ) {
			overridenFields.push_back( f );
		}
	}

	HHuginn::field_identifiers_t::iterator endDerived( derivedFields.end() );
	HHuginn::field_identifiers_t::iterator endNew( newFields.end() );
	for ( HHuginn::identifier_id_t f : overridenFields ) {
		endDerived = remove( derivedFields.begin(), endDerived, f );
		endNew = remove( newFields.begin(), endNew, f );
	}
	newFields.erase( endNew, newFields.end() );
	derivedFields.erase( endDerived, derivedFields.end() );
	for ( HHuginn::identifier_id_t f : derivedFields ) {
		stream_ << "\n" << "  " << huginn->identifier_name( f ) << " (derived)";
	}
	for ( HHuginn::identifier_id_t f : overridenFields ) {
		stream_ << "\n" << "  " << huginn->identifier_name( f ) << " (overriden)";
	}
	for ( HHuginn::identifier_id_t f : newFields ) {
		stream_ << "\n" << "  " << huginn->identifier_name( f ) << " (new)";
	}
	return ( stream_ );
	M_EPILOG
}

void HHuginn::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _source->name() << "'" << endl << "classes:" << endl;
	for ( classes_t::value_type const& c : _classes ) {
		stream_ << *c.second << endl;
	}
	stream_ << "functions:" << endl;
	for ( functions_t::value_type const& f : _functions ) {
		yaal::hcore::HString const& name( identifier_name( f.first ) );
		stream_ << name;
		if ( _builtin_.count( name ) > 0 ) {
			stream_ <<" (builtin)";
		}
		if ( _standardLibrary_.count( name ) > 0 ) {
			stream_ <<" (standard library)";
		}
		stream_ << endl;
	}
	return;
	M_EPILOG
}

void HHuginn::register_function( identifier_id_t identifier_, function_t function_ ) {
	M_PROLOG
	_functions.insert( make_pair( identifier_, function_ ) );
	return;
	M_EPILOG
}

HHuginn::class_t HHuginn::create_class( yaal::hcore::HString const& name_, HClass const* base_, field_definitions_t const& fieldDefinitions_ ) {
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			this,
			type_id_t( _idGenerator ),
			identifier_id( name_ ),
			base_,
			fieldDefinitions_
		)
	);
	++ _idGenerator;
	return ( c );
}

HHuginn::class_t HHuginn::create_class( class_constructor_t const& classConstructor_ ) {
	class_t c( classConstructor_( type_id_t( _idGenerator ) ) );
	++ _idGenerator;
	return ( c );
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

HHuginn::value_t HHuginn::HValue::do_field( HHuginn::value_t const& object_, int index_ ) const {
	M_PROLOG
	value_t const& f( _class->field( index_ ) );
	M_ASSERT( f->type_id() == TYPE::METHOD );
	return (
		pointer_static_cast<HHuginn::HValue>(
			make_pointer<HClass::HBoundMethod>( *static_cast<HClass::HMethod const*>( f.raw() ), object_ )
		)
	);
	M_EPILOG
}

HHuginn::value_t HHuginn::HValue::clone( HHuginn* huginn_ ) const {
	return ( do_clone( huginn_ ) );
}

HHuginn::value_t HHuginn::HValue::do_clone( HHuginn* huginn_ ) const {
	M_ASSERT( _class->type_id() == TYPE::NONE );
	return ( huginn_->none_value() );
}

HHuginn::HObserver::HObserver( HHuginn::value_t const& value_ )
	: HValue( &_observerClass_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HObserver::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HObserver::do_clone( HHuginn* ) const {
	return ( make_pointer<HObserver>( _value ) );
}

HHuginn::HReference::HReference( HHuginn::value_t& value_ )
	: HValue( &_referenceClass_ ), _value( value_ ) {
	return;
}

HHuginn::value_t& HHuginn::HReference::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HReference::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_reference( _value ) );
}

HHuginn::HIterable::HIterable( HClass const* class_ )
	: HValue( class_ ) {
	return;
}

HHuginn::HIterable::HIterator HHuginn::HIterable::iterator( void ) {
	return ( do_iterator() );
}

int long HHuginn::HIterable::size( void ) const {
	return ( do_size() );
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
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], _condition->position() );
	}
	if ( static_cast<HBoolean*>( v.raw() )->value() ) {
		_ifTrue->execute( thread_ );
	} else {
		_ifFalse->execute( thread_ );
	}
	return ( f->result() );
	M_EPILOG
}

HHuginn::value_t HHuginn::HTernaryEvaluator::do_clone( HHuginn* M_NDEBUG_CODE( huginn_ ) ) const {
	M_ASSERT( 0 && "cloning ternary evaluator"[0] );
#ifdef NDEBUG
	return ( huginn_->none_value() );
#endif /* #ifdef NDEBUG */
}

HHuginn::HFunctionReference::HFunctionReference(
	identifier_id_t identifierId_,
	function_t const& function_
) : HValue( &_functionReferenceClass_ ),
	_identifierId( identifierId_ ),
	_function( function_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunctionReference::do_clone( HHuginn* ) const {
	return ( make_pointer<HFunctionReference>( _identifierId, _function ) );
}

HHuginn::HClass::HMethod::HMethod(
	function_t const& function_
) : HValue( &_methodClass_ )
	, _function( function_ ) {
	return;
}

HHuginn::function_t const& HHuginn::HClass::HMethod::function( void ) const {
	return ( _function );
}

HHuginn::value_t HHuginn::HClass::HMethod::do_clone( HHuginn* ) const {
	return ( make_pointer<HMethod>( _function ) );
}

HHuginn::HClass::HBoundMethod::HBoundMethod( HMethod const& method_, HHuginn::value_t const& object_ )
	: HMethod( method_.function() )
	, _objectHolder( object_ ) {
	return;
}

HHuginn::value_t* HHuginn::HClass::HBoundMethod::object( void ) {
	return ( &_objectHolder );
}

HHuginn::value_t HHuginn::HClass::HBoundMethod::do_clone( HHuginn* ) const {
	return ( make_pointer<HBoundMethod>( *static_cast<HMethod const*>( this ), _objectHolder ) );
}

namespace huginn_builtin {

inline HHuginn::value_t convert( HHuginn::TYPE toType_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( type_name( toType_ ), values_, 1, 1, position_ );
	HHuginn::value_t res;
	if ( toType_ == HHuginn::TYPE::INTEGER ) {
		res = value_builtin::integer( thread_, values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::REAL ) {
		res = value_builtin::real( thread_, values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::STRING ) {
		res = value_builtin::string( thread_, values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::NUMBER ) {
		res = value_builtin::number( thread_, values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::BOOLEAN ) {
		res = value_builtin::boolean( thread_, values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::CHARACTER ) {
		res = value_builtin::character( thread_, values_.front(), position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Conversion to `"_ys.append( type_name( toType_ ) ).append( "' is not supported." ),
			position_
		);
	}
	return ( res );
	M_EPILOG
}

inline HHuginn::value_t size( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "size", values_, 1, 1, position_ );
	HHuginn::value_t const& val( values_.front() );
	HHuginn::HValue const* v( val.raw() );
	int long long s( 0 );
	HHuginn::type_id_t typeId( v->type_id() );
	HHuginn::value_t res;
	if ( typeId == HHuginn::TYPE::STRING ) {
		s = static_cast<HHuginn::HString const*>( v )->value().get_length();
	} else if ( typeId == HHuginn::TYPE::LIST ) {
		s = static_cast<HHuginn::HList const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::DEQUE ) {
		s = static_cast<HHuginn::HDeque const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::DICT ) {
		s = static_cast<HHuginn::HDict const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::ORDER ) {
		s = static_cast<HHuginn::HOrder const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::SET ) {
		s = static_cast<HHuginn::HSet const*>( v )->size();
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

inline HHuginn::value_t type( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( thread_->object_factory().create_string( v->get_class()->name() ) );
	M_EPILOG
}

inline HHuginn::value_t copy( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( v->clone( &(thread_->huginn()) ) );
	M_EPILOG
}

inline HHuginn::value_t observe( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "observe", values_, 1, 1, position_ );
	HHuginn::value_t const& v( values_.front() );
	if ( v->type_id() == HHuginn::TYPE::OBSERVER ) {
		throw HHuginn::HHuginnRuntimeException(
			"Making observer out of observer.",
			position_
		);
	}
	return ( make_pointer<HHuginn::HObserver>( v ) );
	M_EPILOG
}

inline HHuginn::value_t use( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	static char const name[] = "use";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::OBSERVER, true, position_ );
	HHuginn::HObserver const* o( static_cast<HHuginn::HObserver const*>( values_.front().raw() ) );
	HHuginn::value_t v( o->value() );
	if ( !v ) {
		v = thread_->huginn().none_value();
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t list( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_list() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t deque( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_deque() );
	HHuginn::HDeque* d( static_cast<HHuginn::HDeque*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		d->push_back( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t dict( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict", values_, 0, 0, position_ );
	return ( thread_->object_factory().create_dict() );
	M_EPILOG
}

inline HHuginn::value_t order( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_order() );
	HHuginn::HOrder* o( static_cast<HHuginn::HOrder*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		o->insert( e, position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t lookup( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup", values_, 0, 0, position_ );
	return ( thread_->object_factory().create_lookup() );
	M_EPILOG
}

inline HHuginn::value_t set( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_set() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t print( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "print", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	yaal::hcore::HStreamInterface& out( thread_->huginn().output_stream() );
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
	return ( thread_->huginn().none_value() );
	M_EPILOG
}

inline HHuginn::value_t input( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "input", values_, 0, 0, position_ );
	yaal::hcore::HString l;
	int len( static_cast<int>( thread_->huginn().input_stream().read_until( l, HStreamInterface::eols, false ) ) );
	return ( len > 0 ? thread_->object_factory().create_string( l ) : thread_->huginn().none_value() );
	M_EPILOG
}

inline HHuginn::value_t assert( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "assert";
	verify_arg_count( name, values_, 2, 3, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::BOOLEAN, false, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
	if ( ! get_boolean( values_[0] ) ) {
		int argc( static_cast<int>( values_.get_size() ) );
		HString message( get_string( values_[argc - 1] ) );
		if ( argc > 2 ) {
			message.append( " " ).append( get_string( values_[1] ) );
		}
		throw HHuginn::HHuginnRuntimeException( message, position_ );
	}
	return ( thread_->huginn().none_value() );
	M_EPILOG
}

}

void HHuginn::register_builtin_function( char const* name_, function_t&& function_ ) {
	M_PROLOG
	identifier_id_t id( identifier_id( name_ ) );
	OCompiler::OIdentifierUse& iu( _compiler->_usedIdentifiers[id] );
	iu.write( 0, OCompiler::OIdentifierUse::TYPE::FUNCTION );
	iu.read( 0 );
	_functions.insert( make_pair( id, yaal::move( function_ ) ) );
	return;
	M_EPILOG
}

void HHuginn::register_builtins( void ) {
	M_PROLOG
	M_ENSURE( _state != STATE::COMPILED );
	M_ENSURE( identifier_id( KEYWORD::CONSTRUCTOR ) == KEYWORD::CONSTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::DESTRUCTOR ) == KEYWORD::DESTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::THIS ) == KEYWORD::THIS_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::SUPER ) == KEYWORD::SUPER_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::ASSERT ) == KEYWORD::ASSERT_IDENTIFIER );
	register_builtin_function( "integer", hcore::call( &huginn_builtin::convert, TYPE::INTEGER, _1, _2, _3, _4 ) );
	register_builtin_function( "real", hcore::call( &huginn_builtin::convert, TYPE::REAL, _1, _2, _3, _4 ) );
	register_builtin_function( "string", hcore::call( &huginn_builtin::convert, TYPE::STRING, _1, _2, _3, _4 ) );
	register_builtin_function( "number", hcore::call( &huginn_builtin::convert, TYPE::NUMBER, _1, _2, _3, _4 ) );
	register_builtin_function( "boolean", hcore::call( &huginn_builtin::convert, TYPE::BOOLEAN, _1, _2, _3, _4 ) );
	register_builtin_function( "character", hcore::call( &huginn_builtin::convert, TYPE::CHARACTER, _1, _2, _3, _4 ) );
	register_builtin_function( "size", hcore::call( &huginn_builtin::size, _1, _2, _3, _4 ) );
	register_builtin_function( "type", hcore::call( &huginn_builtin::type, _1, _2, _3, _4 ) );
	register_builtin_function( "copy", hcore::call( &huginn_builtin::copy, _1, _2, _3, _4 ) );
	register_builtin_function( "observe", hcore::call( &huginn_builtin::observe, _1, _2, _3, _4 ) );
	register_builtin_function( "use", hcore::call( &huginn_builtin::use, _1, _2, _3, _4 ) );
	register_builtin_function( "list", hcore::call( &huginn_builtin::list, _1, _2, _3, _4 ) );
	register_builtin_function( "deque", hcore::call( &huginn_builtin::deque, _1, _2, _3, _4 ) );
	register_builtin_function( "dict", hcore::call( &huginn_builtin::dict, _1, _2, _3, _4 ) );
	register_builtin_function( "order", hcore::call( &huginn_builtin::order, _1, _2, _3, _4 ) );
	register_builtin_function( "lookup", hcore::call( &huginn_builtin::lookup, _1, _2, _3, _4 ) );
	register_builtin_function( "set", hcore::call( &huginn_builtin::set, _1, _2, _3, _4 ) );
	register_builtin_function( "print", hcore::call( &huginn_builtin::print, _1, _2, _3, _4 ) );
	register_builtin_function( "input", hcore::call( &huginn_builtin::input, _1, _2, _3, _4 ) );
	register_builtin_function( "assert", hcore::call( &huginn_builtin::assert, _1, _2, _3, _4 ) );
	return;
	M_EPILOG
}

}

}


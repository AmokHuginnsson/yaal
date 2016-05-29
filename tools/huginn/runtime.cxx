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
#include "tools/util.hxx"
#include "tools/streamtools.hxx"
#include "tools/stringalgo.hxx"

#include "hcore/hfile.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HRuntime::HRuntime( HHuginn* huginn_ )
	: _huginn( huginn_ )
	, _idGenerator( static_cast<type_id_t::value_type>( HHuginn::TYPE::NOT_BOOLEAN ) )
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
	, _threads()
	, _packages()
	, _argv( _objectFactory->create_list() )
	, _result()
	, _maxLocalVariableCount( 0 ) {
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

HHuginn::function_t* HRuntime::get_function( identifier_id_t identifierId_ ) {
	M_PROLOG
	HHuginn::function_t* f( nullptr );
	functions_t::iterator fi( _functions.find( identifierId_ ) );
	if ( fi != _functions.end() ) {
		f = &( fi->second );
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

void HRuntime::register_class_low( class_t class_ ) {
	M_PROLOG
	_classes.insert( make_pair( class_->identifier_id(), class_ ) );
	_functions.insert( make_pair( class_->identifier_id(), hcore::call( &HHuginn::HClass::create_instance, class_.raw(), _1, _2, _3, _4 ) ) );
	return;
	M_EPILOG
}

void HRuntime::register_function( identifier_id_t identifier_, function_t function_ ) {
	M_PROLOG
	_functions.insert( make_pair( identifier_, function_ ) );
	return;
	M_EPILOG
}

void HRuntime::register_package( identifier_id_t identifier_, value_t package_ ) {
	M_PROLOG
	_packages.insert( make_pair( identifier_, package_ ) );
	return;
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( identifier_id_t identifier_, HHuginn::HClass const* base_, field_definitions_t const& fieldDefinitions_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			this,
			type_id_t( _idGenerator ),
			identifier_,
			base_,
			fieldDefinitions_
		)
	);
	++ _idGenerator;
	return ( c );
	M_EPILOG
}

HHuginn::class_t HRuntime::create_class( yaal::hcore::HString const& name_, HHuginn::HClass const* base_, field_definitions_t const& fieldDefinitions_ ) {
	M_PROLOG
	return ( create_class( identifier_id( name_ ), base_, fieldDefinitions_ ) );
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
	_result = call( "main", args, 0 );
	t->flush_runtime_exception();
	return;
	M_EPILOG
}

HHuginn::value_t HRuntime::call( yaal::hcore::HString const& name_, values_t const& values_, int position_ ) {
	M_PROLOG
	functions_t::const_iterator f( _functions.find( identifier_id( name_ ) ) );
	value_t res;
	if ( f != _functions.end() ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = f->second( t->second.raw(), nullptr, values_, position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException( "Function `"_ys.append( name_ ).append( "(...)' is not defined." ), position_ );
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
	} else if ( typeId == HHuginn::TYPE::LOOKUP ) {
		s = static_cast<HHuginn::HLookup const*>( v )->size();
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

HHuginn::value_t type( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( thread_->object_factory().create_string( v->get_class()->name() ) );
	M_EPILOG
}

HHuginn::value_t copy( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( v->clone( &(thread_->runtime()) ) );
	M_EPILOG
}

HHuginn::value_t observe( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
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

HHuginn::value_t use( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	static char const name[] = "use";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::OBSERVER, true, position_ );
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

HHuginn::value_t set( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( thread_->object_factory().create_set() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( e );
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
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

}

void HRuntime::register_builtin_function( char const* name_, function_t&& function_ ) {
	M_PROLOG
	identifier_id_t id( identifier_id( name_ ) );
	_huginn->register_function( id );
	_functions.insert( make_pair( id, yaal::move( function_ ) ) );
	return;
	M_EPILOG
}

void HRuntime::register_builtins( void ) {
	M_PROLOG
	M_ENSURE( identifier_id( KEYWORD::CONSTRUCTOR ) == KEYWORD::CONSTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::DESTRUCTOR ) == KEYWORD::DESTRUCTOR_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::THIS ) == KEYWORD::THIS_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::SUPER ) == KEYWORD::SUPER_IDENTIFIER );
	M_ENSURE( identifier_id( KEYWORD::ASSERT ) == KEYWORD::ASSERT_IDENTIFIER );
	register_builtin_function( "integer", hcore::call( &huginn_builtin::integer, _1, _2, _3, _4 ) );
	register_builtin_function( "real", hcore::call( &huginn_builtin::real, _1, _2, _3, _4 ) );
	register_builtin_function( "string", hcore::call( &huginn_builtin::string, _1, _2, _3, _4 ) );
	register_builtin_function( "number", hcore::call( &huginn_builtin::number, _1, _2, _3, _4 ) );
	register_builtin_function( "boolean", hcore::call( &huginn_builtin::boolean, _1, _2, _3, _4 ) );
	register_builtin_function( "character", hcore::call( &huginn_builtin::character, _1, _2, _3, _4 ) );
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
	_objectFactory->register_builtin_classes();
	return;
	M_EPILOG
}

inline yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, HHuginn::HClass const& huginnClass_ ) {
	M_PROLOG
	HRuntime const* runtime( huginnClass_.runtime() );
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
		stream_ << "\n" << "  " << runtime->identifier_name( f ) << " (derived)";
	}
	for ( HHuginn::identifier_id_t f : overridenFields ) {
		stream_ << "\n" << "  " << runtime->identifier_name( f ) << " (overriden)";
	}
	for ( HHuginn::identifier_id_t f : newFields ) {
		stream_ << "\n" << "  " << runtime->identifier_name( f ) << " (new)";
	}
	return ( stream_ );
	M_EPILOG
}

void HRuntime::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _huginn->source_name() << "'" << endl << "classes:" << endl;
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
	for ( functions_t::value_type const& f : _functions ) {
		if ( f.second.id() == id_ ) {
			cerr << "id_ " << id_ << endl;
			cerr << "f = " << f.second.id() << endl;
			cerr << "this = " << this << endl;
			cerr << "number = " << reinterpret_cast<void*>( &value_builtin::number ) << endl;
			cerr << "boolean =  " << reinterpret_cast<void*>( &value_builtin::boolean ) << endl;
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


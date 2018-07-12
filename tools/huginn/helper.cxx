/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "helper.hxx"
#include "thread.hxx"
#include "keyword.hxx"
#include "operator.hxx"
#include "value_builtin.hxx"
#include "exception.hxx"
#include "tools/util.hxx"
#include "tools/hstringstream.hxx"
#include "tools/streamtools.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

HHuginn::HValueHashHelper::HValueHashHelper( void )
	: _thread( nullptr )
	, _position( 0 ) {
}

int long HHuginn::HValueHashHelper::operator()( HHuginn::value_t const& value_ ) const {
	M_ASSERT( _thread != nullptr );
	return ( huginn::value_builtin::hash( _thread, value_, _position ) );
}

bool HHuginn::HValueHashHelper::operator()( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) const {
	M_ASSERT( _thread != nullptr );
	return ( ( v1_->type_id() == v2_->type_id() ) && huginn::value_builtin::equals( _thread, v1_, v2_, _position ) );
}

HHuginn::HValueCompareHelper::HValueCompareHelper( compare_t compare_ )
	: _thread( nullptr )
	, _position( 0 )
	, _compare( compare_ ) {
}

bool HHuginn::HValueCompareHelper::operator()( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) const {
	M_ASSERT( _thread != nullptr );
	M_ASSERT( v1_->type_id() == v2_->type_id() );
	return ( _compare( _thread, v1_, v2_, _position ) );
}

namespace huginn {

bool is_keyword( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( _keywords_.count( name_ ) > 0 );
	M_EPILOG
}

bool is_builtin( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) );
	M_EPILOG
}

bool is_restricted( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) || ( _standardLibrary_.count( name_ ) > 0 ) );
	M_EPILOG
}

HCycleTracker::HCycleTracker( void )
	: _valueNoter() {
}

bool HCycleTracker::track( HHuginn::value_t const& value_ ) {
	M_PROLOG
	HHuginn::type_id_t t( value_->type_id() );
	return (
		( t == HHuginn::TYPE::NONE )
		|| ( t == HHuginn::TYPE::BOOLEAN )
		|| ( t == HHuginn::TYPE::FUNCTION_REFERENCE )
		|| ( t == HHuginn::TYPE::UNBOUND_METHOD )
		|| _valueNoter.insert( value_.raw() ).second
	);
	M_EPILOG
}

void HCycleTracker::check( HHuginn::value_t const& value_, int fileId_, int position_ ) {
	M_PROLOG
	HHuginn::type_id_t t( value_->type_id() );
	if (
		( t != HHuginn::TYPE::NONE )
		&& ( t != HHuginn::TYPE::BOOLEAN )
		&& ( t != HHuginn::TYPE::FUNCTION_REFERENCE )
		&& ( t != HHuginn::TYPE::UNBOUND_METHOD )
		&& ! _valueNoter.insert( value_.raw() ).second ) {
		throw HHuginn::HHuginnRuntimeException(
			"Cycle detected on type: "_ys.append( value_->get_class()->name() ),
			fileId_,
			position_
		);
	}
	return;
	M_EPILOG
}

void HCycleTracker::done( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_valueNoter.erase( value_.raw() );
	return;
	M_EPILOG
}

void operands_type_mismatch( char const* op_, HHuginn::HClass const* c1_, HHuginn::HClass const* c2_, int fileId_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "' do not match: " )
		.append( a_type_name( c1_ ) )
		.append( " vs " )
		.append( a_type_name( c2_ ) )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, fileId_, pos_ );
}

HHuginn::class_t add_class_as_member(
	HHuginn::HClass* juncture_,
	HHuginn::class_t const& class_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass::MEMBER_TYPE memberType_
) {
	M_PROLOG
	HHuginn::value_t member(
		juncture_->runtime()->object_factory()->create_method_raw( class_->constructor_function() )
	);
	juncture_->add_member(
		HHuginn::HFieldDefinition(
			class_->name(), member, doc_
		),
		memberType_
	);
	return ( class_ );
	M_EPILOG
}

enumeration::HEnumerationClass::ptr_t add_enumeration_as_member(
	HHuginn::HClass* juncture_,
	enumeration::HEnumerationClass::ptr_t const& class_,
	yaal::hcore::HString const& doc_,
	HHuginn::HClass::MEMBER_TYPE memberType_
) {
	M_PROLOG
	HHuginn::value_t member(
		juncture_->runtime()->object_factory()->create<HHuginn::HValue>( class_.raw() )
	);
	juncture_->add_member(
		HHuginn::HFieldDefinition(
			class_->name(), member, doc_
		),
		memberType_
	);
	return ( class_ );
	M_EPILOG
}

HHuginn::class_t class_exception( HHuginn::HClass* package_ ) {
	HString name( package_->name() );
	HString exName( name );
	exName.append( "Exception" );
	return (
		add_class_as_member(
			package_,
			exception::create_class(
				package_->runtime(),
				exName,
				"The `"_ys.append( exName ).append( "` exception type for `" ).append( name ).append( "` package." )
			),
			"( *message* ) - create instance of "_ys.append( exName ).append( " with given *message*" )
		)
	);
}

void verify_arg_count( char const* name_, HHuginn::values_t& values_, int min_, int max_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	int argCount( static_cast<int>( values_.get_size() ) );
	if ( min_ == max_ ) {
		if ( argCount != min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
					.append( name_ ).append( "()', expected exactly: " )
					.append( min_ ).append( ", got: " )
					.append( argCount ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		if ( argCount < min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
					.append( name_ ).append( "()', expected at least: " )
					.append( min_ ).append( ", got: " )
					.append( argCount ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		} else if ( argCount > max_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
					.append( name_ ).append( "()', expected at most: " )
					.append( max_ ).append( ", got: " )
					.append( argCount ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return;
	M_EPILOG
}

yaal::hcore::HString a_type_name( HHuginn::TYPE type_ ) {
	M_PROLOG
	hcore::HString tn( type_name( type_ ) );
	hcore::HString atn( article( tn ) );
	atn.append( " `" ).append( tn ).append( "'" );
	return ( atn );
	M_EPILOG
}

yaal::hcore::HString a_type_name( HHuginn::HClass const* class_ ) {
	M_PROLOG
	hcore::HString const& cn( class_->name() );
	hcore::HString atn( article( cn ) );
	atn.append( " `" ).append( cn ).append( "'" );
	return ( atn );
	M_EPILOG
}

namespace {

void fail_arg_type( char const*, HHuginn::values_t&, int, yaal::hcore::HString const&, ARITY, huginn::HThread*, int ) __attribute__((noreturn));
void fail_arg_type(
	char const* name_,
	HHuginn::values_t& values_,
	int no_,
	yaal::hcore::HString const& reqName_,
	ARITY argsArity_,
	huginn::HThread* thread_,
	int position_
) {
	M_PROLOG
	HString no;
	if ( argsArity_ == ARITY::MULTIPLE ) {
		no = util::ordinal( no_ + 1 ).append( " " );
	}
	throw HHuginn::HHuginnRuntimeException(
		""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be " )
			.append( reqName_ )
			.append( ", not " )
			.append( a_type_name( values_[no_]->get_class() ) )
			.append( "." ),
		thread_->current_frame()->file_id(),
		position_
	);
	M_EPILOG
}

}

void verify_arg_type(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, HHuginn::TYPE type_, ARITY argsArity_, huginn::HThread* thread_, int position_ ) {
	if ( values_[no_]->type_id() != type_ ) {
		fail_arg_type( name_, values_, no_, a_type_name( type_ ), argsArity_, thread_, position_ );
	}
}

void verify_arg_type(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, HHuginn::HClass const* class_, ARITY argsArity_, huginn::HThread* thread_, int position_ ) {
	if ( values_[no_]->get_class() != class_ ) {
		fail_arg_type( name_, values_, no_, a_type_name( class_ ), argsArity_, thread_, position_ );
	}
}

void verify_signature( char const* name_, HHuginn::values_t& values_, types_t const& types_, huginn::HThread* thread_, int position_ ) {
	int const COUNT( static_cast<int>( types_.get_size() ) );
	verify_arg_count( name_, values_, COUNT, COUNT, thread_, position_ );
	ARITY arity( COUNT == 1 ? ARITY::UNARY : ARITY::MULTIPLE );
	for ( int i( 0 ); i < COUNT; ++ i ) {
		if ( types_[i] != HHuginn::TYPE::UNKNOWN ) {
			verify_arg_type( name_, values_, i, types_[i], arity, thread_, position_ );
		}
	}
}

void verify_signature_by_class( char const* name_, HHuginn::values_t& values_, classes_t const& classes_, huginn::HThread* thread_, int position_ ) {
	int const COUNT( static_cast<int>( classes_.get_size() ) );
	verify_arg_count( name_, values_, COUNT, COUNT, thread_, position_ );
	ARITY arity( COUNT == 1 ? ARITY::UNARY : ARITY::MULTIPLE );
	for ( int i( 0 ); i < COUNT; ++ i ) {
		if ( classes_[i]->type_id() != HHuginn::TYPE::UNKNOWN ) {
			verify_arg_type( name_, values_, i, classes_[i], arity, thread_, position_ );
		}
	}
}

HHuginn::type_id_t verify_arg_type(
	char const* name_,
	HHuginn::values_t& values_,
	int no_,
	types_t const& types_,
	ARITY argsArity_,
	huginn::HThread* thread_,
	int position_
) {
	M_PROLOG
	HHuginn::type_id_t realType( values_[no_]->type_id() );
	if ( find( types_.begin(), types_.end(), realType ) == types_.end() ) {
		HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		HString reqName;
		for ( HHuginn::TYPE t : types_ ) {
			if ( ! reqName.is_empty() ) {
				reqName.append( ", " );
			}
			reqName.append( a_type_name( t ) );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
				.append( "() " )
				.append( no )
				.append( "argument must be one of" )
				.append( " {" )
				.append( reqName )
				.append( "}, not " )
				.append( a_type_name( values_[no_]->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( realType );
	M_EPILOG
}

HHuginn::type_id_t verify_arg_numeric(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, ARITY argsArity_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	HHuginn::type_id_t t( values_[no_]->type_id() );
	if ( ( t != HHuginn::TYPE::NUMBER ) && ( t != HHuginn::TYPE::REAL ) ) {
		HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
				.append( "() " )
				.append( no )
				.append( "argument must be a numeric type, either a `number' or a `real', not " )
				.append( a_type_name( values_[no_]->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( t );
	M_EPILOG
}

HHuginn::type_id_t verify_arg_collection(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, ARITY argsArity_, ONTICALLY ontically_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	static HHuginn::TYPE const material[] = {
		HHuginn::TYPE::TUPLE,
		HHuginn::TYPE::LIST,
		HHuginn::TYPE::DEQUE,
		HHuginn::TYPE::SET,
		HHuginn::TYPE::ORDER,
		HHuginn::TYPE::DICT,
		HHuginn::TYPE::LOOKUP,
		HHuginn::TYPE::STRING
	};
	HHuginn::TYPE t( static_cast<HHuginn::TYPE>( values_[no_]->type_id().get() ) );
	if ( ! dynamic_cast<HHuginn::HIterable const*>( values_[no_].raw() )
		|| ( ( ontically_ == ONTICALLY::MATERIALIZED ) && ( find( begin( material ), end( material ), t ) == end( material ) ) ) ) {
		HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
				.append( "() " )
				.append( no )
				.append( "argument must be a" )
				.append( ( ontically_ == ONTICALLY::MATERIALIZED ) ? " materialized" : "" )
				.append( " collection type, not " )
				.append( a_type_name( values_[no_]->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( values_[no_]->type_id() );
	M_EPILOG
}

template<typename collection_t>
HHuginn::type_id_t verify_arg_collection_value_type_low(
	char const* name_,
	collection_t const& collection_,
	types_t const& requiredTypes_,
	UNIFORMITY uniformity_,
	huginn::HThread* thread_,
	int position_
) {
	HHuginn::type_id_t type( type_id( HHuginn::TYPE::UNKNOWN ) );
	HHuginn::type_id_t curType( type_id( HHuginn::TYPE::UNKNOWN ) );
	bool first( true );
	int long long pos( 0 );
	for ( HHuginn::value_t const& v : collection_.value() ) {
		curType = v->type_id();
		if ( find( requiredTypes_.begin(), requiredTypes_.end(), curType ) == requiredTypes_.end() ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( name_ )
					.append( "() a collection contains value of an unexpected type: " )
					.append( a_type_name( v->get_class() ) )
					.append( ", at position: " )
					.append( pos ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		if ( first ) {
			type = curType;
			first = false;
		} else if ( uniformity_ == UNIFORMITY::REQUIRED ) {
			if ( curType != type ) {
				throw HHuginn::HHuginnRuntimeException(
					hcore::to_string( name_ )
						.append( "() a collection is not uniformly typed: " )
						.append( a_type_name( v->get_class() ) )
						.append( ", at position: " )
						.append( pos ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
		}
		++ pos;
	}
	return ( type );
}

HHuginn::type_id_t verify_arg_collection_value_type(
	char const* name_,
	HHuginn::values_t& values_,
	int no_,
	ARITY argsArity_,
	types_t const& requiredTypes_,
	UNIFORMITY uniformity_,
	huginn::HThread* thread_,
	int position_
) {
	verify_arg_collection( name_, values_, no_, argsArity_, ONTICALLY::MATERIALIZED, thread_, position_ );
	HHuginn::type_id_t type( type_id( HHuginn::TYPE::UNKNOWN ) );
	switch ( values_[no_]->type_id().get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HTuple const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HList const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HDeque const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HSet const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			HHuginn::HOrder::values_t const& o( static_cast<HHuginn::HOrder const*>( values_[no_].raw() )->value() );
			if ( ! o.is_empty() ) {
				HHuginn::HClass const* c( (*o.begin())->get_class() );
				type = c->type_id();
				if ( find( requiredTypes_.begin(), requiredTypes_.end(), type ) == requiredTypes_.end() ) {
					throw HHuginn::HHuginnRuntimeException(
						hcore::to_string( name_ )
							.append( "() a collection contains value of an unexpected type: " )
							.append( a_type_name( c ) )
							.append( "." ),
						thread_->current_frame()->file_id(),
						position_
					);
				}
			}
		} break;
		default: {
			M_ASSERT( !"Invalid code path - invalid collection type"[0] );
		}
	}
	return ( type );
}

bool is_numeric( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		( t == HHuginn::TYPE::INTEGER )
		|| ( t == HHuginn::TYPE::REAL )
		|| ( t == HHuginn::TYPE::NUMBER )
	);
}

bool is_collection( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		( t == HHuginn::TYPE::TUPLE )
		|| ( t == HHuginn::TYPE::LIST )
		|| ( t == HHuginn::TYPE::DEQUE )
		|| ( t == HHuginn::TYPE::DICT )
		|| ( t == HHuginn::TYPE::LOOKUP )
		|| ( t == HHuginn::TYPE::ORDER )
		|| ( t == HHuginn::TYPE::SET )
	);
}

bool is_collection_like( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( is_collection( class_ ) || ( t == HHuginn::TYPE::STRING ) || ( t == HHuginn::TYPE::UNKNOWN ) );
}

bool is_comparable( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		is_numeric( class_ )
		|| ( t == HHuginn::TYPE::STRING )
		|| ( t == HHuginn::TYPE::CHARACTER )
		|| ( t == HHuginn::TYPE::FUNCTION_REFERENCE )
		|| ( t == HHuginn::TYPE::TUPLE )
		|| ( t == HHuginn::TYPE::LIST )
		|| ( t == HHuginn::TYPE::DEQUE )
	);
}

bool is_boolean_congruent( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		( t == HHuginn::TYPE::BOOLEAN )
		|| ( t == HHuginn::TYPE::UNKNOWN )
		|| ( t == HHuginn::TYPE::REFERENCE )
	);
}

bool is_unknown( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( ( t == HHuginn::TYPE::NOT_BOOLEAN ) || ( t == HHuginn::TYPE::UNKNOWN ) || ( t == HHuginn::TYPE::REFERENCE ) );
}

bool is_numeric_congruent( HHuginn::HClass const* class_ ) {
	return ( is_numeric( class_ ) || is_unknown( class_ ) );
}

bool is_comparable_congruent( HHuginn::HClass const* class_ ) {
	return ( is_comparable( class_ ) || is_unknown( class_ ) );
}

bool is_reference_congruent( HHuginn::HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( ( t == HHuginn::TYPE::REFERENCE ) || ( t == HHuginn::TYPE::UNKNOWN ) );
}

bool is_integer_congruent( HHuginn::HClass const* class_ ) {
	return ( ( class_ && ( class_->type_id() == HHuginn::TYPE::INTEGER ) ) || is_unknown( class_ ) );
}

bool is_summable( HHuginn::HClass const* class_ ) {
	return (
		is_numeric_congruent( class_ )
		|| ( class_ && ( class_->type_id() == HHuginn::TYPE::STRING ) )
		|| is_collection( class_ )
	);
}

bool is_meta_class( HHuginn::HClass const* class_ ) {
	HHuginn::identifier_id_t id( class_->identifier_id() );
	return ( ( id >= BUILTIN::TYPE_NONE_IDENTIFIER ) && ( id <= BUILTIN::TYPE_NAMED_PARAMETERS_IDENTIFIER ) );
}

bool are_congruous( HHuginn::HClass const* c1_, HHuginn::HClass const* c2_ ) {
	HHuginn::type_id_t t1( c1_ ? c1_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	HHuginn::type_id_t t2( c2_ ? c2_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	bool congruous(
		( t1 == t2 )
		|| ( t1 == HHuginn::TYPE::UNKNOWN )
		|| ( t2 == HHuginn::TYPE::UNKNOWN )
		|| ( t1 == HHuginn::TYPE::REFERENCE )
		|| ( t2 == HHuginn::TYPE::REFERENCE )
	);
	if ( ! congruous ) {
		congruous = ( ( t1 != HHuginn::TYPE::BOOLEAN ) && ( t2 == HHuginn::TYPE::NOT_BOOLEAN ) )
			|| ( ( t2 != HHuginn::TYPE::BOOLEAN ) && ( t1 == HHuginn::TYPE::NOT_BOOLEAN ) );
	}
	return ( congruous );
}

HHuginn::HString::value_type const& get_string( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HString const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HString const*>( value_.raw() )->value() );
}

HHuginn::HInteger::value_type get_integer( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HInteger const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
}

HHuginn::HReal::value_type get_real( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HReal const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HReal const*>( value_.raw() )->value() );
}

HHuginn::HNumber::value_type const& get_number( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HNumber const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HNumber const*>( value_.raw() )->value() );
}

HHuginn::HBoolean::value_type get_boolean( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HBoolean const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HBoolean const*>( value_.raw() )->value() );
}

HHuginn::HCharacter::value_type get_character( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HCharacter const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HCharacter const*>( value_.raw() )->value() );
}

HHuginn::HList::values_t const& get_list( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HList const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HList const*>( value_.raw() )->value() );
}

HHuginn::HEnumeral::value_type get_enumeral( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HEnumeral const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HEnumeral const*>( value_.raw() )->value() );
}

HHuginn::HString::value_type const& get_string( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HString const*>( value_ ) );
	return ( static_cast<HHuginn::HString const*>( value_ )->value() );
}

HHuginn::HInteger::value_type get_integer( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HInteger const*>( value_ ) );
	return ( static_cast<HHuginn::HInteger const*>( value_ )->value() );
}

HHuginn::HReal::value_type get_real( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HReal const*>( value_ ) );
	return ( static_cast<HHuginn::HReal const*>( value_ )->value() );
}

HHuginn::HNumber::value_type const& get_number( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HNumber const*>( value_ ) );
	return ( static_cast<HHuginn::HNumber const*>( value_ )->value() );
}

HHuginn::HBoolean::value_type get_boolean( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HBoolean const*>( value_ ) );
	return ( static_cast<HHuginn::HBoolean const*>( value_ )->value() );
}

HHuginn::HCharacter::value_type get_character( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HCharacter const*>( value_ ) );
	return ( static_cast<HHuginn::HCharacter const*>( value_ )->value() );
}

HHuginn::HEnumeral::value_type get_enumeral( HHuginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HEnumeral const*>( value_ ) );
	return ( static_cast<HHuginn::HEnumeral const*>( value_ )->value() );
}

template<>
double long get_by_type<double long>( HHuginn::value_t const& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::REAL );
	return ( static_cast<HHuginn::HReal const*>( value_.raw() )->value() );
}

template<>
double long get_by_type<double long>( HHuginn::value_t& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::REAL );
	return ( static_cast<HHuginn::HReal*>( value_.raw() )->value() );
}

template<>
yaal::hcore::HNumber const& get_by_type<yaal::hcore::HNumber const&>( HHuginn::value_t const& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::NUMBER );
	return ( static_cast<HHuginn::HNumber const*>( value_.raw() )->value() );
}

template<>
yaal::hcore::HNumber& get_by_type<yaal::hcore::HNumber&>( HHuginn::value_t& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::NUMBER );
	return ( static_cast<HHuginn::HNumber*>( value_.raw() )->value() );
}

yaal::hcore::HString const& type_name( HHuginn::TYPE type_ ) {
	static HString const NAME_BOOLEAN   = BUILTIN::BOOLEAN;
	static HString const NAME_INTEGER   = BUILTIN::INTEGER;
	static HString const NAME_REAL      = BUILTIN::REAL;
	static HString const NAME_STRING    = BUILTIN::STRING;
	static HString const NAME_NUMBER    = BUILTIN::NUMBER;
	static HString const NAME_CHARACTER = BUILTIN::CHARACTER;
	static HString const NAME_TUPLE     = BUILTIN::TUPLE;
	static HString const NAME_LIST      = BUILTIN::LIST;
	static HString const NAME_DEQUE     = BUILTIN::DEQUE;
	static HString const NAME_DICT      = BUILTIN::DICT;
	static HString const NAME_LOOKUP    = BUILTIN::LOOKUP;
	static HString const NAME_ORDER     = BUILTIN::ORDER;
	static HString const NAME_SET       = BUILTIN::SET;
	static HString const NAME_BLOB      = BUILTIN::BLOB;
	static HString const NAME_NONE                = BUILTIN::TYPE_NONE;
	static HString const NAME_OBSERVER            = BUILTIN::TYPE_OBSERVER;
	static HString const NAME_REFERENCE           = BUILTIN::TYPE_REFERENCE;
	static HString const NAME_FUNCTION_REFERENCE  = BUILTIN::TYPE_FUNCTION_REFERENCE;
	static HString const NAME_OBJECT_REFERENCE    = BUILTIN::TYPE_OBJECT_REFERENCE;
	static HString const NAME_METHOD              = BUILTIN::TYPE_METHOD;
	static HString const NAME_UNBOUND_METHOD      = BUILTIN::TYPE_UNBOUND_METHOD;
	static HString const NAME_BOUND_METHOD        = BUILTIN::TYPE_BOUND_METHOD;
	static HString const NAME_VARIADIC_PARAMETERS = BUILTIN::TYPE_VARIADIC_PARAMETERS;
	static HString const NAME_NAMED_PARAMETERS    = BUILTIN::TYPE_NAMED_PARAMETERS;
	static HString const NAME_UNKNOWN             = BUILTIN::TYPE_UNKNOWN;
	HString const* s( &NAME_UNKNOWN );
	switch ( type_ ) {
		case ( HHuginn::TYPE::NONE ):               s = &NAME_NONE;               break;
		case ( HHuginn::TYPE::BOOLEAN ):            s = &NAME_BOOLEAN;            break;
		case ( HHuginn::TYPE::INTEGER ):            s = &NAME_INTEGER;            break;
		case ( HHuginn::TYPE::REAL ):               s = &NAME_REAL;               break;
		case ( HHuginn::TYPE::STRING ):             s = &NAME_STRING;             break;
		case ( HHuginn::TYPE::NUMBER ):             s = &NAME_NUMBER;             break;
		case ( HHuginn::TYPE::CHARACTER ):          s = &NAME_CHARACTER;          break;
		case ( HHuginn::TYPE::TUPLE ):              s = &NAME_TUPLE;              break;
		case ( HHuginn::TYPE::LIST ):               s = &NAME_LIST;               break;
		case ( HHuginn::TYPE::DICT ):               s = &NAME_DICT;               break;
		case ( HHuginn::TYPE::DEQUE ):              s = &NAME_DEQUE;              break;
		case ( HHuginn::TYPE::ORDER ):              s = &NAME_ORDER;              break;
		case ( HHuginn::TYPE::LOOKUP ):             s = &NAME_LOOKUP;             break;
		case ( HHuginn::TYPE::SET ):                s = &NAME_SET;                break;
		case ( HHuginn::TYPE::BLOB ):               s = &NAME_BLOB;               break;
		case ( HHuginn::TYPE::OBSERVER ):           s = &NAME_OBSERVER;           break;
		case ( HHuginn::TYPE::REFERENCE ):          s = &NAME_REFERENCE;          break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): s = &NAME_FUNCTION_REFERENCE; break;
		case ( HHuginn::TYPE::OBJECT_REFERENCE ):   s = &NAME_OBJECT_REFERENCE;   break;
		case ( HHuginn::TYPE::METHOD ):             s = &NAME_METHOD;             break;
		case ( HHuginn::TYPE::UNBOUND_METHOD ):     s = &NAME_UNBOUND_METHOD;     break;
		case ( HHuginn::TYPE::BOUND_METHOD ):       s = &NAME_BOUND_METHOD;       break;
		case ( HHuginn::TYPE::VARIADIC_PARAMETERS ):s = &NAME_VARIADIC_PARAMETERS;break;
		case ( HHuginn::TYPE::NAMED_PARAMETERS ):   s = &NAME_NAMED_PARAMETERS;   break;
		case ( HHuginn::TYPE::UNKNOWN ):            s = &NAME_UNKNOWN;            break;
		case ( HHuginn::TYPE::NOT_BOOLEAN ):        s = &NAME_UNKNOWN;            break;
	}
	return ( *s );
}

char const* op_to_symbol_str( OPERATOR o_ ) {
	char const* str( "" );
	switch ( o_ ) {
		case ( OPERATOR::PLUS ):               str = "+";      break;
		case ( OPERATOR::MINUS ):              str = "-";      break;
		case ( OPERATOR::MULTIPLY ):           str = "*";      break;
		case ( OPERATOR::DIVIDE ):             str = "/";      break;
		case ( OPERATOR::MODULO ):             str = "%";      break;
		case ( OPERATOR::POWER ):              str = "^";      break;
		case ( OPERATOR::POWER_TERM ):         str = "^;";     break;
		case ( OPERATOR::FACTORIAL ):          str = "x!";     break;
		case ( OPERATOR::MODULUS ):            str = "|...|";  break;
		case ( OPERATOR::PARENTHESIS ):        str = "(...)";  break;
		case ( OPERATOR::ASSIGN ):             str = "=";      break;
		case ( OPERATOR::ASSIGN_PACK ):        str = ",=";     break;
		case ( OPERATOR::ASSIGN_TERM ):        str = "=;";     break;
		case ( OPERATOR::PLUS_ASSIGN ):        str = "+=";     break;
		case ( OPERATOR::MINUS_ASSIGN ):       str = "-=";     break;
		case ( OPERATOR::MULTIPLY_ASSIGN ):    str = "*=";     break;
		case ( OPERATOR::DIVIDE_ASSIGN ):      str = "/=";     break;
		case ( OPERATOR::MODULO_ASSIGN ):      str = "%=";     break;
		case ( OPERATOR::POWER_ASSIGN ):       str = "^=";     break;
		case ( OPERATOR::SUBSCRIPT ):          str = "[...]";  break;
		case ( OPERATOR::RANGE ):              str = "[::]";   break;
		case ( OPERATOR::SUBSCRIPT_ARGUMENT ): str = "[xxx]";  break;
		case ( OPERATOR::NEGATE ):             str = "-x";     break;
		case ( OPERATOR::FUNCTION_CALL ):      str = "f(,,,)"; break;
		case ( OPERATOR::FUNCTION_ARGUMENT ):  str = "f(arg)"; break;
		case ( OPERATOR::MEMBER_ACCESS ):      str = "o.memb"; break;
		case ( OPERATOR::EQUALS ):             str = "==";     break;
		case ( OPERATOR::NOT_EQUALS ):         str = "≠";      break;
		case ( OPERATOR::LESS ):               str = "<";      break;
		case ( OPERATOR::GREATER ):            str = ">";      break;
		case ( OPERATOR::LESS_OR_EQUAL ):      str = "≤";      break;
		case ( OPERATOR::GREATER_OR_EQUAL ):   str = "≥";      break;
		case ( OPERATOR::IS_ELEMENT_OF ):      str = "∈";      break;
		case ( OPERATOR::BOOLEAN_AND ):        str = "⋀";      break;
		case ( OPERATOR::BOOLEAN_OR ):         str = "⋁";      break;
		case ( OPERATOR::BOOLEAN_XOR ):        str = "⊕";      break;
		case ( OPERATOR::BOOLEAN_NOT ):        str = "¬x";     break;
		case ( OPERATOR::TERNARY ):            str = "?:";     break;
		case ( OPERATOR::MAKE_DICT ):          str = "{k:v}";  break;
		case ( OPERATOR::MAKE_LOOKUP ):        str = "[k:v]";  break;
		case ( OPERATOR::NONE ): break;
	}
	return ( str );
}

char const* op_to_str( OPERATOR o_ ) {
	char const* str( "" );
	switch ( o_ ) {
		case ( OPERATOR::NOT_EQUALS ):         str = "!=";     break;
		case ( OPERATOR::LESS_OR_EQUAL ):      str = "<=";     break;
		case ( OPERATOR::GREATER_OR_EQUAL ):   str = ">=";     break;
		case ( OPERATOR::BOOLEAN_AND ):        str = "&&";     break;
		case ( OPERATOR::BOOLEAN_OR ):         str = "||";     break;
		case ( OPERATOR::BOOLEAN_XOR ):        str = "^^";     break;
		case ( OPERATOR::BOOLEAN_NOT ):        str = "!x";     break;
		default: str = op_to_symbol_str( o_ );
	}
	return ( str );
}

yaal::hcore::HString int_to_str( HHuginn::HInteger::value_type int_, BASE base_, bool prefix_ ) {
	M_PROLOG
	HStringStream ss;
	switch ( base_ ) {
		case ( BASE::DEC ): ss << int_; break;
		case ( BASE::HEX ): ss << ( prefix_ ? "0x" : "" ) << hex << int_; break;
		case ( BASE::OCT ): ss << ( prefix_ ? "0o"  : "" ) << oct << int_; break;
		case ( BASE::BIN ): {
			HStringStream bs;
			bs << bin << int_;
			HString s( bs.string() );
			s.trim_left( "0" );
			if ( s.is_empty() ) {
				s.assign( "0" );
			}
			ss << ( prefix_ ? "0b" : "" ) << s;
		} break;
	}
	return ( ss.string() );
	M_EPILOG
}

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

}

}


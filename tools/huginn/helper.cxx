/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "helper.hxx"
#include "thread.hxx"
#include "keyword.hxx"
#include "operator.hxx"
#include "instruction.hxx"
#include "iterator.hxx"
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

HHuginn::HValueHashHelper::size_type HHuginn::HValueHashHelper::operator()( HHuginn::value_t const& value_ ) const {
	M_ASSERT( _thread != nullptr );
	return ( value_->operator_hash( _thread, value_, _position ) );
}

bool HHuginn::HValueHashHelper::operator()( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) const {
	M_ASSERT( _thread != nullptr );
	return ( ( v1_->type_id() == v2_->type_id() ) && v1_->operator_equals( _thread, v1_, v2_, _position ) );
}

HHuginn::HValueCompareHelper::HValueCompareHelper( compare_t compare_ )
	: _thread( nullptr )
	, _position( 0 )
	, _compare( compare_ ) {
}

bool HHuginn::HValueCompareHelper::operator()( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) const {
	M_ASSERT( _thread != nullptr );
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

void operands_type_mismatch( char const* op_, HClass const* c1_, HClass const* c2_, int fileId_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "` do not match: " )
		.append( a_type_name( c1_ ) )
		.append( " vs " )
		.append( a_type_name( c2_ ) )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, fileId_, pos_ );
}

HHuginn::class_t add_class_as_member(
	HClass* juncture_,
	HHuginn::class_t const& class_,
	yaal::hcore::HString const& doc_,
	HClass::MEMBER_TYPE memberType_
) {
	M_PROLOG
	juncture_->add_member(
		HHuginn::HFieldDefinition(
			class_->name(), class_->constructor(), doc_
		),
		memberType_
	);
	return ( class_ );
	M_EPILOG
}

enumeration::HEnumerationClass::ptr_t add_enumeration_as_member(
	HClass* juncture_,
	enumeration::HEnumerationClass::ptr_t const& class_,
	yaal::hcore::HString const& doc_,
	HClass::MEMBER_TYPE memberType_
) {
	M_PROLOG
	HHuginn::value_t member(
		juncture_->runtime()->object_factory()->create<enumeration::HEnumeration>( class_.raw() )
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

HHuginn::class_t class_exception( HClass* package_, HClass const* base_ ) {
	hcore::HString name( package_->name() );
	hcore::HString exName( name );
	exName.append( "Exception" );
	return (
		add_class_as_member(
			package_,
			exception::create_class(
				package_->runtime(),
				exName,
				"The `"_ys.append( exName ).append( "` exception type for `" ).append( name ).append( "` package." ),
				HHuginn::VISIBILITY::PACKAGE,
				base_
			),
			"( *message* ) - create instance of "_ys.append( exName ).append( " with given *message*" )
		)
	);
}

HHuginn::class_t create_class(
	HRuntime* runtime_,
	yaal::hcore::HString const& name_,
	HHuginn::field_definitions_t const& descriptions_,
	yaal::hcore::HString const& doc_,
	HHuginn::VISIBILITY visibility_,
	HClass const* origin_
) {
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( name_ ) );
	HHuginn::class_t c( runtime_ && ! origin_ ? runtime_->get_class( classIdentifier ) : nullptr );
	if ( ! c ) {
		c =	runtime_->create_class(
			classIdentifier,
			doc_,
			HHuginn::ACCESS::PRIVATE,
			HClass::TYPE::BUILTIN
		);
		c->redefine( nullptr, descriptions_ );
		c->set_origin( origin_ );
		runtime_->huginn()->register_class( c, visibility_ );
	}
	return ( c );
}

void verify_arg_count( char const* name_, HHuginn::values_t& values_, int min_, int max_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	int argCount( static_cast<int>( values_.get_size() ) );
	if ( min_ == max_ ) {
		if ( argCount != min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
					.append( name_ ).append( "()`, expected exactly: " )
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
					.append( name_ ).append( "()`, expected at least: " )
					.append( min_ ).append( ", got: " )
					.append( argCount ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		} else if ( argCount > max_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
					.append( name_ ).append( "()`, expected at most: " )
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

yaal::hcore::HString full_class_name( huginn::HRuntime const& runtime_, HClass const* class_, bool preserveAliases_ ) {
	M_PROLOG
	HClass const* origin( class_->origin() );
	hcore::HString const* originName( preserveAliases_ ? runtime_.package_name( origin ) : nullptr );
	hcore::HString cn;
	if ( originName ) {
		cn.append( *originName ).append( "." );
	} else if (
		origin && (
			origin->origin()
			|| !! runtime_.get_class( origin->identifier_id() )
			|| ! preserveAliases_
		)
	) {
		cn.append( full_class_name( runtime_, origin, preserveAliases_ ) ).append( "." );
	}
	cn.append( class_->name() );
	return ( cn );
	M_EPILOG
}

yaal::hcore::HString full_class_name( huginn::HRuntime const& runtime_, HHuginn::value_t const& value_, bool preserveAliases_ ) {
	M_PROLOG
	return ( full_class_name( runtime_, value_->get_class(), preserveAliases_ ) );
	M_EPILOG
}

yaal::hcore::HString a_type_name( HHuginn::TYPE type_ ) {
	M_PROLOG
	hcore::HString tn( type_name( type_ ) );
	hcore::HString atn( article( tn ) );
	atn.append( " `" ).append( tn ).append( "`" );
	return ( atn );
	M_EPILOG
}

yaal::hcore::HString a_type_name( HClass const* class_ ) {
	M_PROLOG
	hcore::HString const& cn( class_->name() );
	hcore::HString atn( article( cn ) );
	atn.append( " `" ).append( cn ).append( "`" );
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
	hcore::HString no;
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
	int no_, HClass const* class_, ARITY argsArity_, huginn::HThread* thread_, int position_ ) {
	if ( values_[no_]->get_class() != class_ ) {
		fail_arg_type( name_, values_, no_, a_type_name( class_ ), argsArity_, thread_, position_ );
	}
}

void verify_signature( char const* name_, HHuginn::values_t& values_, int optionalCount_, types_t const& types_, huginn::HThread* thread_, int position_ ) {
	int const COUNT( static_cast<int>( types_.get_size() ) );
	verify_arg_count( name_, values_, COUNT - optionalCount_, COUNT, thread_, position_ );
	int argCount( static_cast<int>( values_.get_size() ) );
	ARITY arity( argCount == 1 ? ARITY::UNARY : ARITY::MULTIPLE );
	for ( int i( 0 ); i < argCount; ++ i ) {
		if ( types_[i] != HHuginn::TYPE::UNKNOWN ) {
			verify_arg_type( name_, values_, i, types_[i], arity, thread_, position_ );
		}
	}
}

void verify_signature_by_class( char const* name_, HHuginn::values_t& values_, int optionalCount_, classes_t const& classes_, huginn::HThread* thread_, int position_ ) {
	int const COUNT( static_cast<int>( classes_.get_size() ) );
	verify_arg_count( name_, values_, COUNT - optionalCount_, COUNT, thread_, position_ );
	int argCount( static_cast<int>( values_.get_size() ) );
	ARITY arity( argCount == 1 ? ARITY::UNARY : ARITY::MULTIPLE );
	for ( int i( 0 ); i < argCount; ++ i ) {
		if ( classes_[i] != nullptr ) {
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
		hcore::HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		hcore::HString reqName;
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
		hcore::HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
				.append( "() " )
				.append( no )
				.append( "argument must be a numeric type, either a `number` or a `real`, not " )
				.append( a_type_name( values_[no_]->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( t );
	M_EPILOG
}

HHuginn::value_t verify_arg_callable(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, ARITY argsArity_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( values_[no_] );
	HHuginn::type_id_t t( v->type_id() );
	do {
		if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			break;
		}
		if ( t == HHuginn::TYPE::UNBOUND_METHOD ) {
			break;
		}
		if ( t == HHuginn::TYPE::BOUND_METHOD ) {
			break;
		}
		int fi( v->field_index( IDENTIFIER::INTERFACE::CALL ) );
		if ( fi >= 0 ) {
			v = v->field( v, fi );
			if ( v->type_id() == HHuginn::TYPE::BOUND_METHOD ) {
				break;
			}
		}

		hcore::HString no;
		if ( argsArity_ == ARITY::MULTIPLE ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
				.append( "() " )
				.append( no )
				.append( "argument must be a callable type, either a `*function_reference*` or an `*unbound_method*` or a `*bound_method*` or implementing a `call()` method, not " )
				.append( a_type_name( values_[no_]->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	} while ( false );
	return ( v );
	M_EPILOG
}

void not_a_collection( huginn::HThread*, char const*, HClass const*, int, ARITY, char const*, int ) __attribute__((noreturn));
inline void not_a_collection( huginn::HThread* thread_, char const* name_, HClass const* class_, int no_, ARITY argsArity_, char const* extraMsg_, int position_ ) {
	hcore::HString no;
	if ( argsArity_ == ARITY::MULTIPLE ) {
		no = util::ordinal( no_ + 1 ).append( " " );
	}
	throw HHuginn::HHuginnRuntimeException(
		""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be a" )
			.append( extraMsg_ )
			.append( " collection type, not " )
			.append( a_type_name( class_ ) )
			.append( "." ),
		thread_->current_frame()->file_id(),
		position_
	);
}

HHuginn::type_id_t verify_arg_materialized_collection(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, ARITY argsArity_, huginn::HThread* thread_, int position_
) {
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
	HHuginn::value_t const& v( values_[no_] );
	HHuginn::type_id_t t( v->type_id() );
	auto end( yaal::end( material ) );
	if ( find( begin( material ), end, type_tag( t ) ) == end ) {
		not_a_collection( thread_, name_, v->get_class(), no_, argsArity_, " materialized", position_ );
	}
	return ( t );
	M_EPILOG
}

HHuginn::value_t verify_arg_virtual_collection(
	char const* name_,
	HHuginn::values_t& values_,
	int no_, ARITY argsArity_, huginn::HThread* thread_, int position_
) {
	M_PROLOG
	HHuginn::value_t v( values_[no_] );
	do {
		if ( dynamic_cast<huginn::HIterable const*>( v.raw() ) ) {
			break;
		}
		if (
			dynamic_cast<HObject*>( v.raw() )
			&& ( v->field_index( IDENTIFIER::INTERFACE::ITERATOR ) >= 0 )
		) {
			v = thread_->object_factory().create<HIterableAdaptor>( thread_->object_factory().iterable_adaptor_class(), v );
			break;
		}
		not_a_collection( thread_, name_, v->get_class(), no_, argsArity_, "", position_ );
	} while ( false );
	return ( v );
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
	verify_arg_materialized_collection( name_, values_, no_, argsArity_, thread_, position_ );
	HHuginn::type_id_t type( type_id( HHuginn::TYPE::UNKNOWN ) );
	switch ( values_[no_]->type_id().get() ) {
		case ( static_cast<int>( HHuginn::TYPE::TUPLE ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<huginn::HTuple const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<huginn::HList const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<huginn::HDeque const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<huginn::HSet const*>( values_[no_].raw() ), requiredTypes_, uniformity_, thread_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::ORDER ) ): {
			huginn::HOrder::values_t const& o( static_cast<huginn::HOrder const*>( values_[no_].raw() )->value() );
			if ( ! o.is_empty() ) {
				HClass const* c( (*o.begin())->get_class() );
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

bool is_numeric( HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		( t == HHuginn::TYPE::INTEGER )
		|| ( t == HHuginn::TYPE::REAL )
		|| ( t == HHuginn::TYPE::NUMBER )
	);
}

bool is_collection( HClass const* class_ ) {
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

bool is_collection_like( HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( is_collection( class_ ) || ( t == HHuginn::TYPE::STRING ) || ( t == HHuginn::TYPE::UNKNOWN ) || is_enum_class( class_ ) );
}

bool is_comparable( HClass const* class_ ) {
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

bool is_boolean_congruent( HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return (
		( t == HHuginn::TYPE::BOOLEAN )
		|| ( t == HHuginn::TYPE::UNKNOWN )
		|| ( t == HHuginn::TYPE::REFERENCE )
	);
}

bool is_unknown( HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( ( t == HHuginn::TYPE::NOT_BOOLEAN ) || ( t == HHuginn::TYPE::UNKNOWN ) || ( t == HHuginn::TYPE::REFERENCE ) );
}

bool is_numeric_congruent( HClass const* class_ ) {
	return ( is_numeric( class_ ) || is_unknown( class_ ) );
}

bool is_comparable_congruent( HClass const* class_ ) {
	return ( is_comparable( class_ ) || is_unknown( class_ ) );
}

bool is_reference_congruent( HClass const* class_ ) {
	HHuginn::type_id_t t( class_ ? class_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
	return ( ( t == HHuginn::TYPE::REFERENCE ) || ( t == HHuginn::TYPE::UNKNOWN ) );
}

bool is_integer_congruent( HClass const* class_ ) {
	return ( ( class_ && ( class_->type_id() == HHuginn::TYPE::INTEGER ) ) || is_unknown( class_ ) );
}

bool is_summable( HClass const* class_ ) {
	return (
		is_numeric_congruent( class_ )
		|| ( class_ && ( class_->type_id() == HHuginn::TYPE::STRING ) )
		|| is_collection( class_ )
	);
}

bool is_meta_class( HClass const* class_ ) {
	HHuginn::identifier_id_t id( class_->identifier_id() );
	return ( ( id >= IDENTIFIER::BUILTIN::TYPE_NONE ) && ( id <= IDENTIFIER::BUILTIN::TYPE_NAMED_PARAMETERS ) );
}

bool are_congruous( HClass const* c1_, HClass const* c2_ ) {
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

HString::value_type const& get_string( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HString const*>( value_.raw() ) );
	return ( static_cast<HString const*>( value_.raw() )->value() );
}

huginn::HInteger::value_type get_integer( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<huginn::HInteger const*>( value_.raw() ) );
	return ( static_cast<huginn::HInteger const*>( value_.raw() )->value() );
}

HReal::value_type get_real( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HReal const*>( value_.raw() ) );
	return ( static_cast<HReal const*>( value_.raw() )->value() );
}

HNumber::value_type const& get_number( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HNumber const*>( value_.raw() ) );
	return ( static_cast<HNumber const*>( value_.raw() )->value() );
}

HBoolean::value_type get_boolean( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HBoolean const*>( value_.raw() ) );
	return ( static_cast<HBoolean const*>( value_.raw() )->value() );
}

HCharacter::value_type get_character( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HCharacter const*>( value_.raw() ) );
	return ( static_cast<HCharacter const*>( value_.raw() )->value() );
}

huginn::HList::values_t const& get_list( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<huginn::HList const*>( value_.raw() ) );
	return ( static_cast<huginn::HList const*>( value_.raw() )->value() );
}

HEnumeral::value_type get_enumeral( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HEnumeral const*>( value_.raw() ) );
	return ( static_cast<HEnumeral const*>( value_.raw() )->value() );
}

HString::value_type const& get_string( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HString const*>( value_ ) );
	return ( static_cast<HString const*>( value_ )->value() );
}

huginn::HInteger::value_type get_integer( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<huginn::HInteger const*>( value_ ) );
	return ( static_cast<huginn::HInteger const*>( value_ )->value() );
}

HReal::value_type get_real( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HReal const*>( value_ ) );
	return ( static_cast<HReal const*>( value_ )->value() );
}

HNumber::value_type const& get_number( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HNumber const*>( value_ ) );
	return ( static_cast<HNumber const*>( value_ )->value() );
}

HBoolean::value_type get_boolean( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HBoolean const*>( value_ ) );
	return ( static_cast<HBoolean const*>( value_ )->value() );
}

HCharacter::value_type get_character( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HCharacter const*>( value_ ) );
	return ( static_cast<HCharacter const*>( value_ )->value() );
}

HEnumeral::value_type get_enumeral( huginn::HValue const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HEnumeral const*>( value_ ) );
	return ( static_cast<HEnumeral const*>( value_ )->value() );
}

template<>
double long get_by_type<double long>( HHuginn::value_t const& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::REAL );
	return ( static_cast<HReal const*>( value_.raw() )->value() );
}

template<>
double long get_by_type<double long>( HHuginn::value_t& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::REAL );
	return ( static_cast<HReal*>( value_.raw() )->value() );
}

template<>
yaal::hcore::HNumber const& get_by_type<yaal::hcore::HNumber const&>( HHuginn::value_t const& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::NUMBER );
	return ( static_cast<HNumber const*>( value_.raw() )->value() );
}

template<>
yaal::hcore::HNumber& get_by_type<yaal::hcore::HNumber&>( HHuginn::value_t& value_ ) {
	M_ENSURE( value_->type_id() == HHuginn::TYPE::NUMBER );
	return ( static_cast<HNumber*>( value_.raw() )->value() );
}

yaal::hcore::HString const& type_name( HHuginn::TYPE type_ ) {
	static hcore::HString const NAME_BOOLEAN   = BUILTIN::BOOLEAN;
	static hcore::HString const NAME_INTEGER   = BUILTIN::INTEGER;
	static hcore::HString const NAME_REAL      = BUILTIN::REAL;
	static hcore::HString const NAME_STRING    = BUILTIN::STRING;
	static hcore::HString const NAME_NUMBER    = BUILTIN::NUMBER;
	static hcore::HString const NAME_CHARACTER = BUILTIN::CHARACTER;
	static hcore::HString const NAME_TUPLE     = BUILTIN::TUPLE;
	static hcore::HString const NAME_LIST      = BUILTIN::LIST;
	static hcore::HString const NAME_DEQUE     = BUILTIN::DEQUE;
	static hcore::HString const NAME_DICT      = BUILTIN::DICT;
	static hcore::HString const NAME_LOOKUP    = BUILTIN::LOOKUP;
	static hcore::HString const NAME_ORDER     = BUILTIN::ORDER;
	static hcore::HString const NAME_SET       = BUILTIN::SET;
	static hcore::HString const NAME_BLOB      = BUILTIN::BLOB;
	static hcore::HString const NAME_NONE                = BUILTIN::TYPE_NONE;
	static hcore::HString const NAME_OBSERVER            = BUILTIN::TYPE_OBSERVER;
	static hcore::HString const NAME_REFERENCE           = BUILTIN::TYPE_REFERENCE;
	static hcore::HString const NAME_FUNCTION_REFERENCE  = BUILTIN::TYPE_FUNCTION_REFERENCE;
	static hcore::HString const NAME_OBJECT_REFERENCE    = BUILTIN::TYPE_OBJECT_REFERENCE;
	static hcore::HString const NAME_METHOD              = BUILTIN::TYPE_METHOD;
	static hcore::HString const NAME_UNBOUND_METHOD      = BUILTIN::TYPE_UNBOUND_METHOD;
	static hcore::HString const NAME_BOUND_METHOD        = BUILTIN::TYPE_BOUND_METHOD;
	static hcore::HString const NAME_VARIADIC_PARAMETERS = BUILTIN::TYPE_VARIADIC_PARAMETERS;
	static hcore::HString const NAME_NAMED_PARAMETERS    = BUILTIN::TYPE_NAMED_PARAMETERS;
	static hcore::HString const NAME_UNKNOWN             = BUILTIN::TYPE_UNKNOWN;
	hcore::HString const* s( &NAME_UNKNOWN );
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
		case ( OPERATOR::IS_NOT_ELEMENT_OF ):  str = "∉";      break;
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

yaal::hcore::HString int_to_str( huginn::HInteger::value_type int_, BASE base_, bool prefix_ ) {
	M_PROLOG
	HStringStream ss;
	switch ( base_ ) {
		case ( BASE::DEC ): ss << int_; break;
		case ( BASE::HEX ): ss << ( prefix_ ? "0x" : "" ) << hex << int_; break;
		case ( BASE::OCT ): ss << ( prefix_ ? "0o"  : "" ) << oct << int_; break;
		case ( BASE::BIN ): {
			HStringStream bs;
			bs << bin << int_;
			hcore::HString s( bs.string() );
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

HHuginn::value_t instance( HClass const* class_, HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	return ( class_->create_instance( thread_, object_, values_, position_ ) );
	M_EPILOG
}

}

}

}

}


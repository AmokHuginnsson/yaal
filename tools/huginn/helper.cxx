/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  helper.cxx - this file is integral part of `yaal' project.

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
#include "helper.hxx"
#include "thread.hxx"
#include "keyword.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

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

void operands_type_mismatch( char const* op_, HHuginn::type_id_t t1_, HHuginn::type_id_t t2_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "' do not match: " )
		.append( type_name( t1_ ) )
		.append( " vs " )
		.append( type_name( t2_ ) )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, pos_ );
}

void verify_arg_count( yaal::hcore::HString const& name_, HHuginn::values_t const& values_, int min_, int max_, int position_ ) {
	M_PROLOG
	int argCount( static_cast<int>( values_.get_size() ) );
	if ( min_ == max_ ) {
		if ( argCount != min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
				.append( name_ ).append( "()', expected exactly: " )
				.append( min_ ).append( ", got: " )
				.append( argCount ).append( "." ),
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
				position_
			);
		} else if ( argCount > max_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
				.append( name_ ).append( "()', expected at most: " )
				.append( max_ ).append( ", got: " )
				.append( argCount ).append( "." ),
				position_
			);
		}
	}
	return;
	M_EPILOG
}

namespace {
void verify_arg_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_,
	HHuginn::type_id_t type_,
	yaal::hcore::HString const& reqName_,
	bool oneArg_,
	int position_
) {
	M_PROLOG
	if ( values_[no_]->type_id() != type_ ) {
		HString no;
		if ( ! oneArg_ ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		HString const& realName( values_[no_]->get_class()->name() );
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be " )
			.append( _vowel_.has( reqName_[0] ) ? "an" : "a" )
			.append( " `" )
			.append( reqName_ )
			.append( "', not " )
			.append( _vowel_.has( realName[0] ) ? "an" : "a" )
			.append( " `" )
			.append( realName )
			.append( "'." ),
			position_
		);
	}
	return;
	M_EPILOG
}
}

void verify_arg_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_, HHuginn::TYPE type_, bool oneArg_, int position_ ) {
	verify_arg_type( name_, values_, no_, HHuginn::type_id_t( static_cast<HHuginn::type_id_t::value_type>( type_ ) ), type_name( type_ ), oneArg_, position_ );
}

void verify_arg_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_, HHuginn::HClass const* class_, bool oneArg_, int position_ ) {
	verify_arg_type( name_, values_, no_, class_->type_id(), class_->name(), oneArg_, position_ );
}

HHuginn::type_id_t verify_arg_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_,
	types_t const& types_,
	bool oneArg_,
	int position_
) {
	M_PROLOG
	HHuginn::type_id_t realType( values_[no_]->type_id() );
	if ( find( types_.begin(), types_.end(), realType ) == types_.end() ) {
		HString no;
		if ( ! oneArg_ ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		HString const& realName( values_[no_]->get_class()->name() );
		HString reqName;
		for ( HHuginn::TYPE t : types_ ) {
			if ( ! reqName.is_empty() ) {
				reqName.append( ", " );
			}
			reqName.append( type_name( t ) );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be one of" )
			.append( " {" )
			.append( reqName )
			.append( "}, not " )
			.append( _vowel_.has( realName[0] ) ? "an" : "a" )
			.append( " `" )
			.append( realName )
			.append( "'." ),
			position_
		);
	}
	return ( realType );
	M_EPILOG
}

HHuginn::type_id_t verify_arg_numeric(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_, bool oneArg_, int position_ ) {
	M_PROLOG
	HHuginn::type_id_t t( values_[no_]->type_id() );
	if ( ( t != HHuginn::TYPE::NUMBER ) && ( t != HHuginn::TYPE::REAL ) ) {
		HString no;
		if ( ! oneArg_ ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be a numeric type, either `number' or `real', not a " )
			.append( values_[no_]->get_class()->name() )
			.append( "'." ),
			position_
		);
	}
	return ( t );
	M_EPILOG
}

HHuginn::type_id_t verify_arg_collection(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_, bool oneArg_, bool materialized_, int position_ ) {
	M_PROLOG
	static HHuginn::TYPE const material[] = {
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
		|| ( materialized_ && ( find( begin( material ), end( material ), t ) == end( material ) ) ) ) {
		HString no;
		if ( ! oneArg_ ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be a" )
			.append( materialized_ ? " materialized" : "" )
			.append( " collection type, not a " )
			.append( values_[no_]->get_class()->name() )
			.append( "'." ),
			position_
		);
	}
	return ( values_[no_]->type_id() );
	M_EPILOG
}

template<typename collection_t>
HHuginn::type_id_t verify_arg_collection_value_type_low(
	yaal::hcore::HString const& name_,
	collection_t const& collection_,
	types_t const& requiredTypes_,
	bool uniform_,
	int position_
) {
	HHuginn::type_id_t type( _unknownClass_.type_id() );
	HHuginn::type_id_t curType( _unknownClass_.type_id() );
	bool first( true );
	int long long pos( 0 );
	for ( HHuginn::value_t const& v : collection_.value() ) {
		curType = v->type_id();
		if ( find( requiredTypes_.begin(), requiredTypes_.end(), curType ) == requiredTypes_.end() ) {
			throw HHuginn::HHuginnRuntimeException( to_string( name_ ).append( "() a collection contains value of an unexpected type: " ).append( v->get_class()->name() ).append( ", at position: " ).append( pos ), position_ );
		}
		if ( first ) {
			type = curType;
			first = false;
		} else if ( uniform_ ) {
			if ( curType != type ) {
				throw HHuginn::HHuginnRuntimeException( to_string( name_ ).append( "() a collection is not uniformly typed: " ).append( v->get_class()->name() ).append( ", at position: " ).append( pos ), position_ );
			}
		}
		++ pos;
	}
	return ( type );
}

HHuginn::type_id_t verify_arg_collection_value_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_,
	bool oneArg_,
	types_t const& requiredTypes_,
	bool uniform_,
	int position_
) {
	verify_arg_collection( name_, values_, no_, oneArg_, true, position_ );
	HHuginn::type_id_t type( _unknownClass_.type_id() );
	switch ( values_[no_]->type_id().get() ) {
		case ( static_cast<int>( HHuginn::TYPE::LIST ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HList const*>( values_[no_].raw() ), requiredTypes_, uniform_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::DEQUE ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HDeque const*>( values_[no_].raw() ), requiredTypes_, uniform_, position_ );
		} break;
		case ( static_cast<int>( HHuginn::TYPE::SET ) ): {
			type = verify_arg_collection_value_type_low( name_, *static_cast<HHuginn::HSet const*>( values_[no_].raw() ), requiredTypes_, uniform_, position_ );
		} break;
	}
	return ( type );
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
	static HString const NAME_NONE = "*none*";
	static HString const NAME_BOOLEAN = "boolean";
	static HString const NAME_INTEGER = "integer";
	static HString const NAME_REAL = "real";
	static HString const NAME_STRING = "string";
	static HString const NAME_NUMBER = "number";
	static HString const NAME_CHARACTER = "character";
	static HString const NAME_LIST = "list";
	static HString const NAME_DICT = "dict";
	static HString const NAME_DEQUE = "deque";
	static HString const NAME_ORDER = "order";
	static HString const NAME_LOOKUP = "lookup";
	static HString const NAME_SET = "set";
	static HString const NAME_OBSERVER = "*observer*";
	static HString const NAME_REFERENCE = "*reference*";
	static HString const NAME_FUNCTION_REFERENCE = "*function_reference*";
	static HString const NAME_OBJECT_REFERENCE = "*object_reference*";
	static HString const NAME_METHOD = "*method*";
	static HString const NAME_BOUND_METHOD = "*bound_method*";
	static HString const NAME_UNKNOWN = "*unknown*";
	HString const* s( &NAME_UNKNOWN );
	switch ( type_ ) {
		case ( HHuginn::TYPE::NONE ):               s = &NAME_NONE;               break;
		case ( HHuginn::TYPE::BOOLEAN ):            s = &NAME_BOOLEAN;            break;
		case ( HHuginn::TYPE::INTEGER ):            s = &NAME_INTEGER;            break;
		case ( HHuginn::TYPE::REAL ):               s = &NAME_REAL;               break;
		case ( HHuginn::TYPE::STRING ):             s = &NAME_STRING;             break;
		case ( HHuginn::TYPE::NUMBER ):             s = &NAME_NUMBER;             break;
		case ( HHuginn::TYPE::CHARACTER ):          s = &NAME_CHARACTER;          break;
		case ( HHuginn::TYPE::LIST ):               s = &NAME_LIST;               break;
		case ( HHuginn::TYPE::DICT ):               s = &NAME_DICT;               break;
		case ( HHuginn::TYPE::DEQUE ):              s = &NAME_DEQUE;              break;
		case ( HHuginn::TYPE::ORDER ):              s = &NAME_ORDER;              break;
		case ( HHuginn::TYPE::LOOKUP ):             s = &NAME_LOOKUP;             break;
		case ( HHuginn::TYPE::SET ):                s = &NAME_SET;                break;
		case ( HHuginn::TYPE::OBSERVER ):           s = &NAME_OBSERVER;           break;
		case ( HHuginn::TYPE::REFERENCE ):          s = &NAME_REFERENCE;          break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): s = &NAME_FUNCTION_REFERENCE; break;
		case ( HHuginn::TYPE::OBJECT_REFERENCE ):   s = &NAME_OBJECT_REFERENCE;   break;
		case ( HHuginn::TYPE::METHOD ):             s = &NAME_METHOD;             break;
		case ( HHuginn::TYPE::BOUND_METHOD ):       s = &NAME_BOUND_METHOD;       break;
		case ( HHuginn::TYPE::UNKNOWN ):            s = &NAME_UNKNOWN;            break;
		case ( HHuginn::TYPE::NOT_BOOLEAN ):        s = &NAME_UNKNOWN;            break;
	}
	return ( *s );
}

char const* op_to_str( OPERATOR o_ ) {
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
		case ( OPERATOR::ABSOLUTE ):           str = "|...|";  break;
		case ( OPERATOR::PARENTHESIS ):        str = "(...)";  break;
		case ( OPERATOR::ASSIGN ):             str = "=";      break;
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
		case ( OPERATOR::NOT_EQUALS ):         str = "!=";     break;
		case ( OPERATOR::LESS ):               str = "<";      break;
		case ( OPERATOR::GREATER ):            str = ">";      break;
		case ( OPERATOR::LESS_OR_EQUAL ):      str = "<=";     break;
		case ( OPERATOR::GREATER_OR_EQUAL ):   str = ">=";     break;
		case ( OPERATOR::BOOLEAN_AND ):        str = "&&";     break;
		case ( OPERATOR::BOOLEAN_OR ):         str = "||";     break;
		case ( OPERATOR::BOOLEAN_XOR ):        str = "^^";     break;
		case ( OPERATOR::BOOLEAN_NOT ):        str = "!x";     break;
		case ( OPERATOR::TERNARY ):            str = "?:";     break;
		case ( OPERATOR::MAKE_DICT ):          str = "{k:v}";  break;
		case ( OPERATOR::NONE ): break;
	}
	return ( str );
}

}

}

}


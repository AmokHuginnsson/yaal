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

/*
 * Built-in type dictionary must be declared before first use of register_type().
 */
HHuginn::HType::id_generator_t HHuginn::HType::_idGenerator{ 0 };
HHuginn::HType::type_dict_t HHuginn::HType::_builtin{};

HHuginn::type_t const HHuginn::TYPE::NONE( HHuginn::HType::register_type( "none", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::INTEGER( HHuginn::HType::register_type( "integer", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::REAL( HHuginn::HType::register_type( "real", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::NUMBER( HHuginn::HType::register_type( "number", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::BOOLEAN( HHuginn::HType::register_type( "boolean", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::CHARACTER( HHuginn::HType::register_type( "character", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::REFERENCE( HHuginn::HType::register_type( "*reference*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::FUNCTION_REFERENCE( HHuginn::HType::register_type( "*function_reference*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::OBJECT_REFERENCE( HHuginn::HType::register_type( "*object_reference*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::METHOD( HHuginn::HType::register_type( "*method*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::UNKNOWN( HHuginn::HType::register_type( "*unknown*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::NOT_BOOLEAN( HHuginn::HType::register_type( "*not_boolean*", nullptr ) );

namespace {
}

namespace huginn {

char const* _errMsgHHuginn_[ 10 ] = {
	_( "Operands are not summable." ),
	_( "Operands are not substractable." ),
	_( "Operands are not multipliable." ),
	_( "Operands are not divisible." ),
	_( "Operands are not exponentionable." ),
	_( "Operands are not comparable." ),
	_( "Operands are not boolean values." ),
	_( "Operand is not a boolean value." ),
	_( "Subscript is not an integer." )
};

HHuginn::value_t _none_ = make_pointer<HHuginn::HValue>( HHuginn::TYPE::NONE );
HHuginn::value_t _true_ = make_pointer<HHuginn::HBoolean>( true );
HHuginn::value_t _false_ = make_pointer<HHuginn::HBoolean>( false );

namespace exception {

extern HHuginn::class_t _class_;

}

}

executing_parser::HRule HHuginn::make_engine( void ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	hcore::HString identifier( YAAL_REGEX_WORD_START "[a-zA-Z_][a-zA-Z0-9_]*" YAAL_REGEX_WORD_END );
	HRule expression( "expression", e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::mark_expression_position, _compiler.get(), _1 ) ) );
	HRule absoluteValue( "absoluteValue",
		constant( '|', e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) )
		>> expression
		>> constant( '|', e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::ABSOLUTE, _1 ) ) ) );
	HRule parenthesis( "parenthesis",
		constant( '(', e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) )
		>> expression
		>> constant( ')', e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::PARENTHESIS, _1 ) ) ) );
	HRule arg(
		"argument",
		expression,
		HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_ARGUMENT, _1 ) )
	);
	HRule argList( "argList", arg >> ( * ( ',' >> arg ) ) );
	HRule functionCallOperator(
		"functionCallOperator",
		constant( '(', HRuleBase::action_position_t( hcore::call( &OCompiler::start_function_call, _compiler.get(), _1 ) ) )
		>> -argList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &OCompiler::close_function_call, _compiler.get(), _1 ) ) ),
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule stringLiteral(
		"stringLiteral",
		string_literal[e_p::HStringLiteral::action_string_position_t( hcore::call( &OCompiler::defer_store_string, _compiler.get(), _1, _2 ) )]
	);
	HRule literalNone(
		"none",
		constant( KEYWORD::NONE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _none_, _1 ) ) )
	);
	HRule booleanLiteralTrue(
		"true",
		constant( KEYWORD::TRUE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _true_, _1 ) ) )
	);
	HRule booleanLiteralFalse(
		"false",
		constant( KEYWORD::FALSE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _false_, _1 ) ) )
	);
	HRule numberLiteral(
		"numberLiteral",
		constant( '$' ) >> real[e_p::HReal::action_string_position_t( hcore::call( &OCompiler::defer_store_number, _compiler.get(), _1, _2 ) )]
	);
	HRule listLiteral(
		"listLiteral",
		constant(
			'[',
			HRuleBase::action_position_t( hcore::call( &OCompiler::defer_call, _compiler.get(), "list", _1 ) )
		) >> -argList >> ']',
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule dictLiteralElement(
		"dictLiteralElement",
		arg >> ':' >>	arg
	);
	HRule dictLiteral(
		"dictLiteral",
		constant(
			'{',
			HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -( dictLiteralElement >> *( ',' >> dictLiteralElement ) ) >> '}',
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_DICT, _1 ) )
	);
	HRule parameter(
		"parameter",
		regex(
			"parameterIdentifier",
			identifier,
			HRegex::action_string_position_t( hcore::call( &OCompiler::add_paramater, _compiler.get(), _1, _2 ) )
		) >> -( constant( '=' ) >> HRule( expression, HRuleBase::action_position_t( hcore::call( &OCompiler::add_default_value, _compiler.get(), _1 ) ) ) )
	);
	HRule nameList(
		"nameList",
		parameter >> ( * ( ',' >> parameter ) )
	);
	HRule scope( "scope" );
	HRule lambda(
		"lambda",
		constant(
			'@',
			e_p::HRegex::action_position_t( hcore::call( &OCompiler::set_lambda_name, _compiler.get(), _1 ) )
		) >> '(' >> -nameList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &OCompiler::verify_default_argument, _compiler.get(), _1 ) ) )
		>> scope,
		HRuleBase::action_position_t( hcore::call( &OCompiler::create_lambda, _compiler.get(), _1 ) )
	);
	HRule rangeOper(
		"rangeOper",
		constant(
			':',
			e_p::HCharacter::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::SUBSCRIPT_ARGUMENT, _1 ) )
		)
	);
	HRule subscriptOperator(
		"subscriptOperator",
		constant(
			'[',
			e_p::HCharacter::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::SUBSCRIPT, _1 ) )
		) >> ( ( ( rangeOper >> -arg ) | ( arg >> -( rangeOper >> -arg ) ) ) >> -( rangeOper >> -arg ) ) >> ']',
		e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::SUBSCRIPT, _1 ) )
	);
	HRule reference(
		regex(
			"reference",
			identifier,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &OCompiler::defer_get_reference, _compiler.get(), _1, _2 ) )
		)
	);
	HRule memberAccess(
		"memberAccess",
		constant( '.' )[HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::MEMBER_ACCESS, _1 ) )] >> regex(
			"member",
			identifier,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &OCompiler::defer_get_field_reference, _compiler.get(), _1, _2 ) )
		),
		e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MEMBER_ACCESS, _1 ) )
	);
	HRule dereference( "dereference", *( subscriptOperator | functionCallOperator | memberAccess ) );
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_position_t( hcore::call( &OCompiler::defer_store_real, _compiler.get(), _1, _2 ) )]
		| numberLiteral
		| integer[e_p::HInteger::action_int_long_long_position_t( hcore::call( &OCompiler::defer_store_integer, _compiler.get(), _1, _2 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_position_t( hcore::call( &OCompiler::defer_store_character, _compiler.get(), _1, _2 ) )]
		| ( listLiteral >> -( subscriptOperator >> dereference ) )
		| ( dictLiteral >> -( subscriptOperator >> dereference ) )
		| literalNone | booleanLiteralTrue | booleanLiteralFalse
		| ( reference >> dereference )
		| ( stringLiteral >> -subscriptOperator )
		| ( lambda >> -( functionCallOperator >> dereference ) )
	);
	HRule negation(
		"negation",
		( constant( '-' )[HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::NEGATE, _1 ) )] >> atom )[
			e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::NEGATE, _1 ) )
		] | atom
	);
	HRule booleanNot(
		"booleanNot", (
			constant( '!' )[HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::BOOLEAN_NOT, _1 ) )]
			>> negation
		)[
			e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_NOT, _1 ) )
		] | negation
	);
	HRule power(
		"power",
		booleanNot >> (
			* ( constant( '^', e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) ) >> booleanNot )
		),
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::POWER, _1 ) )
	);
	HRule multiplication(
		"multiplication",
		power >> (
			* (
					(
					characters(
						"*/%", e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) )
					) >> power
				)[e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MULTIPLY, _1 ) )]
			)
		)
	);
	HRule sum(
		"sum",
		multiplication >> (
			* (
					(
					characters(
						"+-", e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) )
					) >> multiplication
				)[e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::PLUS, _1 ) )]
			)
		)
	);
	HRule compare(
		"compare",
		sum >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( "<=" ) | ">=" | "<" | ">"
				)[e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> sum
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::LESS, _1 ) )]
		)
	);
	HRule equality(
		"equality",
		compare >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( "==" ) | "!="
				)[e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> compare
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::EQUALS, _1 ) )]
		)
	);
	/*
	 * ( a == b ) && ( c == d ) || ( e == f ) && ( g == h )
	 */
	HRule booleanAnd(
		"booleanAnd",
		equality[e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) )] >> *(
			/* compare action */ (
				constant( "&&" )[e_p::HString::action_position_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )]
				>> equality
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::commit_boolean, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )
	);
	HRule booleanOr(
		"booleanOr",
		HRule( booleanAnd, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) ) ) >> *(
			/* compare action */ (
				constant( "||" )[e_p::HString::action_position_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )]
				>> booleanAnd
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::commit_boolean, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )
	);
	HRule booleanXor(
		"booleanXor",
		booleanOr >> -(
			/* compare action */ (
				constant( "^^" )[e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> booleanOr
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_XOR, _1 ) )]
		)
	);
	HRule ternary(
		"ternary",
		HRule( booleanXor, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) ) ) >> -(
			/* ternary action */ (
				constant( '?' )[e_p::HString::action_position_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::TERNARY, _1 ) )]
				>> expression
				>> constant( ':' )[e_p::HString::action_position_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::TERNARY, _1 ) )]
				>> expression
			)[HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::TERNARY, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::commit_ternary, _compiler.get(), _1 ) )
	);
	HRule value( "value", ternary );
	HRule subscript( "subscript", ( reference >> +( subscriptOperator | functionCallOperator | memberAccess ) ) );
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	expression %= HRule(
		* (
			/* make reference */ (
				subscript[e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::make_reference, _compiler.get(), _1 ) )]
				| regex(
					"variableSetter",
					identifier,
					e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_make_variable, _compiler.get(), _1, _2 ) )
				)
			) >>
			constant( '=' )[e_p::HCharacter::action_character_position_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) )] ^ '='
		) >> value,
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::ASSIGN, _1 ) )
	);
	HRule expressionStatement(
		"expressionStatement",
		HRule( expression, HRuleBase::action_position_t( hcore::call( &OCompiler::commit_expression, _compiler.get(), _1 ) ) ) >> ';'
	);
	HRule catchStatement(
		"catchStatement",
		constant( KEYWORD::CATCH ) >> '(' >>
		regex(
			"exceptionType",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_type_name, _compiler.get(), _1, _2 ) )
		) >>
		regex(
			"exceptionVariable",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_identifier, _compiler.get(), _1, _2 ) )
		) >> ')' >> scope[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_catch, _compiler.get(), _1 ) )]
	);
	HRule tryCatchStatement(
		"tryCatchStatement",
		constant( KEYWORD::TRY ) >> scope >> +catchStatement
	);
	HRule ifClause(
		"ifClause",
		e_p::constant( KEYWORD::IF ) >> '(' >> expression >> ')' >> scope,
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule ifStatement(
		"ifStatement",
		ifClause >>
		*( constant( KEYWORD::ELSE ) >> ifClause ) >>
		-( ( constant( KEYWORD::ELSE ) >> scope )[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )] )
	);
	HRule continueStatement( "continueStatement", constant( KEYWORD::CONTINUE ) >> ';' );
	HRule throwStatement(
		"throwStatement",
		constant( KEYWORD::THROW ) >> expression >> ';'
	);
	/*
	 * TODO:
	 * Allow `break' statement to have literal integer argument
	 * telling how many scopes given `break' should break,
	 * i.e.:
	 * break ( 2 ); // <--- break two levels of nested scopes.
	 */
	HRule breakStatement( "breakStatement", constant( KEYWORD::BREAK ) >> ';' );
	HRule whileStatement(
		"whileStatement",
		constant(
			KEYWORD::WHILE,
			bound_call<e_p::HRuleBase::action_position_t>( &OCompiler::inc_loop_count, _compiler.get(), _1 )
		) >> '(' >> expression >> ')' >> scope
	);
	HRule forIdentifier(
		regex(
			"forIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_identifier, _compiler.get(), _1, _2 ) )
		)
	);
	HRule forStatement(
		"forStatement",
		constant(
			KEYWORD::FOR,
			bound_call<e_p::HRuleBase::action_position_t>( &OCompiler::inc_loop_count, _compiler.get(), _1 )
		) >> '(' >> forIdentifier >> ':' >> expression >> ')' >> scope
	);
	HRule caseStatement(
		"caseStatement",
		constant( KEYWORD::CASE ) >> '(' >> expression >> ')' >> ':'
		>> scope >> -( breakStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )] ),
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule defaultStatement( "defaultStatement", constant( KEYWORD::DEFAULT ) >> ':' >> scope );
	HRule switchStatement(
		"switchStatement",
		constant(
			KEYWORD::SWITCH,
			bound_call<e_p::HRuleBase::action_position_t>( &OCompiler::inc_loop_switch_count, _compiler.get(), _1 )
		) >> '(' >> expression >> ')' >>
		constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> +caseStatement >>
		-( defaultStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )] ) >> '}'
	);
	HRule returnStatement( "returnStatement", constant( KEYWORD::RETURN ) >> -( '(' >> expression >> ')' ) >> ';' );
	HRule statement( "statement",
		ifStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_if_statement, _compiler.get(), _1 ) )]
		| whileStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_while_statement, _compiler.get(), _1 ) )]
		| forStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_for_statement, _compiler.get(), _1 ) )]
		| switchStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_switch_statement, _compiler.get(), _1 ) )]
		| tryCatchStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_try_catch_statement, _compiler.get(), _1 ) )]
		| throwStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_throw_statement, _compiler.get(), _1 ) )]
		| breakStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )]
		| continueStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_continue_statement, _compiler.get(), _1 ) )]
		| returnStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_return_statement, _compiler.get(), _1 ) )]
		| expressionStatement
		| scope[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_scope, _compiler.get(), _1 ) )]
	);
	scope %= ( constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> *statement >> '}' );
	HRule functionDefinition(
		"functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_function_name, _compiler.get(), _1, _2 ) )
		) >> '(' >> -nameList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &OCompiler::verify_default_argument, _compiler.get(), _1 ) ) )
		>> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::create_function, this, _1 ) )
	);
	HRule field(
		"field",
		regex(
			"fieldIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_field_name, _compiler.get(), _1, _2 ) )
		) >> '=' >> HRule( expression, HRuleBase::action_position_t( hcore::call( &OCompiler::add_field_definition, _compiler.get(), _1 ) ) ) >> ';'
	);
	HRule classDefinition(
		"classDefinition",
		constant( KEYWORD::CLASS ) >> regex(
			"classIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_class_name, _compiler.get(), _1, _2 ) )
		) >> -(
			':' >> regex(
				"baseIdentifier",
				identifier,
				e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_base_name, _compiler.get(), _1, _2 ) )
			)
		) >> '{' >> +( field | functionDefinition ) >> '}',
		HRuleBase::action_position_t( hcore::call( &OCompiler::submit_class, _compiler.get(), _1 ) )
	);
	HRule importStatement(
		"importStatement",
		constant( "import" ) >> regex(
			"packageName",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_import_name, _compiler.get(), _1, _2 ) )
		) >> "as" >> regex(
			"importName",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_import_alias, _compiler.get(), _1, _2 ) )
		) >> ';',
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_import, _compiler.get(), _1 ) )
	);
	HRule huginnGrammar( "huginnGrammar", + ( classDefinition | functionDefinition | importStatement ) );
	return ( huginnGrammar );
	M_EPILOG
}

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

HHuginn::HType::HType( yaal::hcore::HString const& name_, int id_ )
	: _name( name_ )
	, _id( id_ ) {
	return;
}

HHuginn::type_t HHuginn::HType::register_type( yaal::hcore::HString const& name_, HHuginn* huginn_ ) {
	type_dict_t& typeDict( huginn_ ? huginn_->_userTypeDict : _builtin );
	if ( ( _builtin.count( name_ ) != 0 )
		|| ( huginn_ && ( huginn_->_userTypeDict.count( name_ ) != 0 ) ) ) {
		throw HHuginnException( "Registering existing type: `"_ys.append( name_ ).append( "'." ) );
	}
	HType::type_holder_t t( new HType( name_, huginn_ ? huginn_->_idGenerator ++ : _idGenerator ++ ) );
	type_dict_t::iterator it( typeDict.insert( make_pair<yaal::hcore::HString const, HType::type_holder_t>( name_, yaal::move( t ) ) ).first );
	return ( it->second.get() );
}

int HHuginn::HType::builtin_type_count( void ) {
	return ( _idGenerator );
}

HHuginn::HClass::HClass(
	HHuginn* huginn_,
	type_t type_,
	HClass const* super_,
	field_names_t const& fieldNames_,
	values_t const& fieldDefinitions_
) : _type( type_ )
	, _super( super_ )
	, _fieldNames( fieldNames_ )
	, _fieldIndexes( super_ ? super_->_fieldIndexes : field_indexes_t() )
	, _fieldDefinitions( super_ ? super_->_fieldDefinitions : values_t() )
	, _huginn( huginn_ ) {
	M_PROLOG
	M_ASSERT( fieldNames_.get_size() == fieldDefinitions_.get_size() );
	int definitionIdx( 0 );
	for ( yaal::hcore::HString const& n : fieldNames_ ) {
		field_indexes_t::const_iterator fi( _fieldIndexes.insert( make_pair( n, static_cast<int>( _fieldIndexes.get_size() ) ) ).first );
		if ( fi->second >= _fieldDefinitions.get_size() ) {
			_fieldDefinitions.resize( fi->second + 1 );
		}
		_fieldDefinitions[fi->second] = fieldDefinitions_[definitionIdx];
		++ definitionIdx;
	}
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HClass::super( void ) const {
	return ( _super );
}

yaal::hcore::HString const& HHuginn::HClass::name( void ) const {
	return ( _type->name() );
}

HHuginn::type_t HHuginn::HClass::type( void ) const {
	return ( _type );
}

HHuginn::HClass::field_names_t const& HHuginn::HClass::field_names( void ) const {
	return ( _fieldNames );
}

int HHuginn::HClass::field_index( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	field_indexes_t::const_iterator it( _fieldIndexes.find( name_ ) );
	return ( it != _fieldIndexes.end() ? it->second : -1 );
	M_EPILOG
}

HHuginn::function_t const& HHuginn::HClass::function( int index_ ) const {
	M_PROLOG
	M_ASSERT( dynamic_cast<HMethod const*>( _fieldDefinitions[index_].raw() ) != nullptr );
	return ( static_cast<HMethod const*>( _fieldDefinitions[index_].raw() )->function() );
	M_EPILOG
}

HHuginn* HHuginn::HClass::huginn( void ) const {
	return ( _huginn );
}

HHuginn::value_t HHuginn::HClass::create_instance( huginn::HThread* thread_, HObject*, values_t const& values_, int position_ ) const {
	M_PROLOG
	return ( do_create_instance( thread_, values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HClass::do_create_instance( huginn::HThread* thread_, values_t const& values_, int position_ ) const {
	M_PROLOG
	value_t v( make_pointer<HObject>( this ) );
	int constructorIdx( field_index( KEYWORD::CONSTRUCTOR ) );
	if ( constructorIdx >= 0 ) {
		function( constructorIdx )( thread_, static_cast<HObject*>( v.raw() ), values_, position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::values_t HHuginn::HClass::get_defaults( void ) const {
	M_PROLOG
	values_t defaults;
	for ( value_t const& v : _fieldDefinitions ) {
		defaults.push_back( v->clone() );
	}
	return ( defaults );
	M_EPILOG
}

bool HHuginn::HClass::is_kind_of( yaal::hcore::HString const& typeName_ ) const {
	M_PROLOG
	return ( ( typeName_ == _type->name() ) || ( _super ? _super->is_kind_of( typeName_ ) : false ) );
	M_EPILOG
}

HHuginn::HObject::HObject( HClass const* class_ )
	: HValue( class_->type() )
	, _class( class_ )
	, _fields( class_->get_defaults() ) {
	reset_methods();
}

HHuginn::HObject::HObject( HClass const* class_, fields_t const& fields_ )
	: HValue( class_->type() )
	, _class( class_ )
	, _fields( fields_ ) {
	reset_methods();
}

HHuginn::HObject::~HObject( void ) {
	M_PROLOG
	huginn::HThread* t( _class->huginn() ? _class->huginn()->current_thread() : nullptr );
	if ( t && ! t->has_runtime_exception() ) {
		int destructorIdx( _class->field_index( KEYWORD::DESTRUCTOR ) );
		HClass const* c( _class->super() );
		if ( destructorIdx >= 0 ) {
			try {
				static_cast<HClass::HMethod const*>(
					_fields[destructorIdx].raw()
				)->function()( t, this, values_t{}, 0 );
			} catch ( HHuginnRuntimeException const& e ) {
				t->break_execution( HFrame::STATE::RUNTIME_EXCEPTION );
				t->set_exception( e.message(), e.position() );
				c = nullptr;
			}
		}
		while ( c ) {
			destructorIdx = c->field_index( KEYWORD::DESTRUCTOR );
			if ( destructorIdx >= 0 ) {
				try {
					c->function( destructorIdx )( t, this, values_t{}, 0 );
				} catch ( HHuginnRuntimeException const& e ) {
					t->break_execution( HFrame::STATE::RUNTIME_EXCEPTION );
					t->set_exception( e.message(), e.position() );
					break;
				}
			}
			c = c->super();
		}
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HHuginn::HObject::reset_methods( void ) {
	M_PROLOG
	for ( value_t& v : _fields ) {
		if ( v->type() == TYPE::METHOD ) {
			static_cast<HClass::HMethod*>( v.raw() )->set_object( this );
		}
	}
	return;
	M_EPILOG
}

int HHuginn::HObject::field_index( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _class->field_index( name_ ) );
	M_EPILOG
}

HHuginn::value_t& HHuginn::HObject::field( int index_ ) {
	M_PROLOG
	return ( _fields[index_] );
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HObject::get_class( void ) const {
	M_PROLOG
	return ( _class );
	M_EPILOG
}

bool HHuginn::HObject::is_kind_of( yaal::hcore::HString const& typeName_ ) const {
	M_PROLOG
	return ( _class->is_kind_of( typeName_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObject::do_clone( void ) const {
	M_PROLOG
	values_t fields;
	for ( value_t const& v : _fields ) {
		fields.push_back( v->clone() );
	}
	return ( make_pointer<HObject>( _class, fields ) );
	M_EPILOG
}

HHuginn::HObjectReference::HObjectReference( value_t const& value_, int upCastLevel_, bool upCast_, int position_ )
	: HValue( TYPE::OBJECT_REFERENCE )
	, _object( value_ )
	, _class( nullptr ) {
	M_PROLOG
	HObject const* o( dynamic_cast<HObject const*>( _object.raw() ) );
	M_ASSERT( o );
	HClass const* c( o->get_class() );
	for ( int i( 0 ); i < ( upCastLevel_ + ( upCast_ ? 1 : 0 ) ); ++ i ) {
		HClass const* s( c->super() );
		if ( ! s ) {
			throw HHuginnRuntimeException( "`"_ys.append( s->type()->name() ).append( "' does not have superclass." ), position_ );
		}
		c = s;
	}
	_class = c;
	return;
	M_EPILOG
}

HHuginn::HObjectReference::HObjectReference( value_t const& value_, HClass const* class_ )
	: HValue( TYPE::OBJECT_REFERENCE )
	, _object( value_ )
	, _class( class_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HHuginn::HObjectReference::field_index( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	HObject const* o( static_cast<HObject const*>( _object.raw() ) );
	return ( _class != o->get_class() ? o->field_index( name_ ) : o->field_index( name_ ) );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObjectReference::field( int index_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( _object.raw() ) );
	value_t* f( nullptr );
	values_t v;
	if ( _class != o->get_class() ) {
		v = _class->get_defaults();
		f = &v[index_];
	} else {
		f = &o->field( index_ );
	}
	HClass::HMethod* m( dynamic_cast<HClass::HMethod*>( f->raw() ) );
	if ( m ) {
		m->set_object( o );
	}
	return ( *f );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObjectReference::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HObjectReference>( _object->clone(), _class ) );
	M_EPILOG
}

HHuginn::flag_t HHuginn::_grammarVerified{ false };

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY )
	, _idGenerator{ HType::builtin_type_count() }
	, _userTypeDict()
	, _classes()
	, _functions()
	, _source( make_resource<HSource>() )
	, _compiler( make_resource<OCompiler>( this ) )
	, _engine( make_engine(), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
	, _threads()
	, _packages()
	, _argv( new ( memory::yaal ) HList() )
	, _result()
	, _errorMessage()
	, _errorPosition( -1 )
	, _inputStream()
	, _inputStreamRaw( &cin )
	, _outputStream()
	, _outputStreamRaw( &cout )
	, _errorStream()
	, _errorStreamRaw( &cerr ) {
	M_PROLOG
	_grammarVerified.store( true );
	register_builtins();
	return;
	M_EPILOG
}

HHuginn::~HHuginn( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_source->load( stream_ );
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
	_state = STATE::PARSED;
	bool ok( _engine.parse( _source->begin(), _source->end() ) );
	if ( ! ok ) {
		_errorMessage = _engine.error_messages()[0];
		_errorPosition = _engine.error_position();
	}
	return ( ok );
	M_EPILOG
}

HHuginn::HClass const* HHuginn::commit_class( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HClass const* c( nullptr );
	classes_t::const_iterator it( _classes.find( name_ ) );
	if ( it != _classes.end() ) {
		c = it->second.get();
	} else {
		OCompiler::OClassContext* cc( _compiler->_submittedClasses.at( name_ ).get() );
		_compiler->track_name_cycle( name_ );
		if ( _functions.count( name_ ) > 0 ) {
			throw HHuginnRuntimeException( "Function of the same name `"_ys.append( name_ ).append( "' is already defined." ), cc->_position.get() );
		}
		HClass const* super( nullptr );
		if ( ! cc->_baseName.is_empty() ) {
			super = commit_class( cc->_baseName );
		}
		values_t fieldDefinitions;
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator ti( _threads.find( threadId ) );
		M_ASSERT( ti != _threads.end() );
		huginn::HThread* t( ti->second.raw() );
		HFrame* frame( t->current_frame() );
		for ( int i( 0 ), size( static_cast<int>( cc->_fieldNames.get_size() ) ); i < size; ++ i ) {
			OCompiler::OClassContext::expressions_t::const_iterator f( cc->_fieldDefinitions.find( i ) );
			if ( f != cc->_fieldDefinitions.end() ) {
				f->second->execute( t );
				fieldDefinitions.push_back( frame->result() );
			} else {
				OCompiler::OClassContext::methods_t::const_iterator m( cc->_methods.find( i ) );
				M_ASSERT( m != cc->_methods.end() );
				fieldDefinitions.push_back( make_pointer<HClass::HMethod>( m->second ) );
			}
		}
		type_t type( HType::register_type( cc->_className, this ) );
		c = _classes.insert( make_pair( name_, make_pointer<HClass>( this, type, super, cc->_fieldNames, fieldDefinitions ) ) ).first->second.get();
	}
	return ( c );
	M_EPILOG
}

void HHuginn::register_class( class_t class_ ) {
	M_PROLOG
	_classes.insert( make_pair( class_->type()->name(), class_ ) );
	return;
	M_EPILOG
}

void HHuginn::finalize_compilation( void ) {
	M_PROLOG
	register_class( exception::_class_ );
	for ( OCompiler::submitted_imports_t::value_type i : _compiler->_submittedImports ) {
		_packages.insert( make_pair( i.second, HPackageFactoryInstance::get_instance().create_package( this, i.first ) ) );
	}
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	huginn::HThread* t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first->second.get() );
	t->create_function_frame( nullptr, 0 );
	for ( OCompiler::submitted_classes_t::value_type const& sc : _compiler->_submittedClasses ) {
		commit_class( sc.first );
	}
	t->pop_frame();
	return;
	M_EPILOG
}

bool HHuginn::compile( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED );
	_state = STATE::COMPILED;
	bool ok( false );
	try {
		_engine.execute();
		finalize_compilation();
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
		_result = call( "main", args, 0 );
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		t->second->flush_runtime_exception();
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
	functions_t::const_iterator f( _functions.find( name_ ) );
	value_t res;
	if ( f != _functions.end() ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = f->second( t->second.raw(), nullptr, values_, position_ );
	} else {
		throw HHuginnRuntimeException( "function `"_ys.append( name_ ).append( "' is not defined" ), position_ );
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

HHuginn::function_t HHuginn::get_function( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HHuginn::function_t f;
	functions_t::const_iterator fi( _functions.find( name_ ) );
	if ( fi != _functions.end() ) {
		f = fi->second;
	} else {
		classes_t::const_iterator ci( _classes.find( name_ ) );
		if ( ci != _classes.end() ) {
			f = hcore::call( &HHuginn::HClass::create_instance, ci->second.raw(), _1, _2, _3, _4 );
		}
	}
	return ( f );
	M_EPILOG
}

HHuginn::value_t HHuginn::get_package( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HHuginn::value_t v;
	packages_t::iterator it( _packages.find( name_ ) );
	if ( it != _packages.end() ) {
		v = it->second;
	}
	return ( v );
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
	_argv->push_back( make_pointer<HString>( arg_ ) );
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
	stream_ << huginnClass_.name();
	if ( huginnClass_.super() ) {
		stream_ << " : " << huginnClass_.super()->name();
	}
	HHuginn::HClass::field_names_t newFields( huginnClass_.field_names() );
	typedef HStack<HHuginn::HClass const*> hierarhy_t;
	hierarhy_t hierarhy;
	HHuginn::HClass const* super( huginnClass_.super() );
	while ( super ) {
		hierarhy.push( super );
		super = super->super();
	}
	HHuginn::HClass::field_names_t derivedFields;
	while ( ! hierarhy.is_empty() ) {
		for ( yaal::hcore::HString const& f : hierarhy.top()->field_names() ) {
			if ( find( derivedFields.begin(), derivedFields.end(), f ) == derivedFields.end() ) {
				derivedFields.push_back( f );
			}
		}
		hierarhy.pop();
	}
	HHuginn::HClass::field_names_t overridenFields;
	for ( yaal::hcore::HString const& f : derivedFields ) {
		if ( find( newFields.begin(), newFields.end(), f ) != newFields.end() ) {
			overridenFields.push_back( f );
		}
	}

	HHuginn::HClass::field_names_t::iterator endDerived( derivedFields.end() );
	HHuginn::HClass::field_names_t::iterator endNew( newFields.end() );
	for ( yaal::hcore::HString const& f : overridenFields ) {
		endDerived = remove( derivedFields.begin(), endDerived, f );
		endNew = remove( newFields.begin(), endNew, f );
	}
	newFields.erase( endNew, newFields.end() );
	derivedFields.erase( endDerived, derivedFields.end() );
	for ( yaal::hcore::HString const& f : derivedFields ) {
		stream_ << "\n" << "  " << f << " (derived)";
	}
	for ( yaal::hcore::HString const& f : overridenFields ) {
		stream_ << "\n" << "  " << f << " (overriden)";
	}
	for ( yaal::hcore::HString const& f : newFields ) {
		stream_ << "\n" << "  " << f << " (new)";
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
		stream_ << f.first;
		if ( _builtin_.count( f.first ) > 0 ) {
			stream_ <<" (builtin)";
		}
		if ( _standardLibrary_.count( f.first ) > 0 ) {
			stream_ <<" (standard library)";
		}
		stream_ << endl;
	}
	return;
	M_EPILOG
}

void HHuginn::create_function( executing_parser::position_t position_ ) {
	M_PROLOG
	if ( ! _compiler->_classContext ) {
		OCompiler::OFunctionContext& fc( _compiler->f() );
		_functions.insert( make_pair<hcore::HString const, function_t>( yaal::move( fc._functionName ), _compiler->create_function( position_ ) ) );
		_compiler->pop_function_context();
	} else {
		_compiler->add_method( position_ );
	}
	return;
	M_EPILOG
}

HHuginn::HValue::HValue( type_t type_ )
	: _type( type_ ) {
	return;
}

HHuginn::type_t HHuginn::HValue::type( void ) const {
	return ( _type );
}

HHuginn::value_t HHuginn::HValue::clone( void ) const {
	return ( do_clone() );
}

HHuginn::value_t HHuginn::HValue::do_clone( void ) const {
	M_ASSERT( _type == TYPE::NONE );
	return ( _none_ );
}

HHuginn::HReference::HReference( HHuginn::value_t& value_ )
	: HValue( TYPE::REFERENCE ), _value( value_ ) {
	return;
}

HHuginn::value_t& HHuginn::HReference::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HReference::do_clone( void ) const {
	return ( make_pointer<HReference>( _value ) );
}

HHuginn::HReal::HReal( double long value_ )
	: HValue( TYPE::REAL ), _value( value_ ) {
	return;
}

double long HHuginn::HReal::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HReal::do_clone( void ) const {
	return ( make_pointer<HReal>( _value ) );
}

HHuginn::HBoolean::HBoolean( bool value_ )
	: HValue( TYPE::BOOLEAN ), _value( value_ ) {
	return;
}

bool HHuginn::HBoolean::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HBoolean::do_clone( void ) const {
	return ( make_pointer<HBoolean>( _value ) );
}

HHuginn::HInteger::HInteger( int long long value_ )
	: HValue( TYPE::INTEGER ), _value( value_ ) {
	return;
}

int long long HHuginn::HInteger::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HInteger::do_clone( void ) const {
	return ( make_pointer<HInteger>( _value ) );
}

HHuginn::HNumber::HNumber( yaal::hcore::HNumber const& value_ )
	: HValue( TYPE::NUMBER ), _value( value_ ) {
	return;
}

yaal::hcore::HNumber const& HHuginn::HNumber::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HNumber::do_clone( void ) const {
	return ( make_pointer<HNumber>( _value ) );
}

HHuginn::HCharacter::HCharacter( char value_ )
	: HValue( TYPE::CHARACTER ), _value( value_ ) {
	return;
}

char HHuginn::HCharacter::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HCharacter::do_clone( void ) const {
	return ( make_pointer<HCharacter>( _value ) );
}

HHuginn::HIterable::HIterable( HClass const* class_ )
	: HObject( class_ ) {
	return;
}

HHuginn::HIterable::HIterator HHuginn::HIterable::iterator( void ) {
	return ( do_iterator() );
}

HHuginn::HTernaryEvaluator::HTernaryEvaluator(
	expression_t const& condition_,
	expression_t const& ifTrue_,
	expression_t const& ifFalse_
) : HValue( TYPE::UNKNOWN ),
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
	if ( v->type() != TYPE::BOOLEAN ) {
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

HHuginn::value_t HHuginn::HTernaryEvaluator::do_clone( void ) const {
	M_ASSERT( 0 && "cloning ternary evaluator"[0] );
#ifdef NDEBUG
	return ( _none_ );
#endif /* #ifdef NDEBUG */
}

HHuginn::HFunctionReference::HFunctionReference(
	yaal::hcore::HString const& name_,
	function_t const& function_
) : HValue( TYPE::FUNCTION_REFERENCE ),
	_name( name_ ),
	_function( function_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HFunctionReference::name( void ) const {
	return ( _name );
}

HHuginn::function_t const& HHuginn::HFunctionReference::function( void ) const {
	return ( _function );
}

HHuginn::value_t HHuginn::HFunctionReference::do_clone( void ) const {
	return ( make_pointer<HFunctionReference>( _name, _function ) );
}

HHuginn::HClass::HMethod::HMethod(
	function_t const& function_
) : HValue( TYPE::METHOD )
	, _function( function_ )
	, _object( nullptr ) {
	return;
}

HHuginn::function_t const& HHuginn::HClass::HMethod::function( void ) const {
	return ( _function );
}

HHuginn::HObject* HHuginn::HClass::HMethod::object( void ) const {
	return ( _object );
}

void HHuginn::HClass::HMethod::set_object( yaal::tools::HHuginn::HObject* object_ ) {
	_object = object_;
	return;
}

HHuginn::value_t HHuginn::HClass::HMethod::do_clone( void ) const {
	return ( make_pointer<HMethod>( _function ) );
}

namespace huginn_builtin {

inline HHuginn::value_t convert( HHuginn::type_t toType_, huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( toType_->name(), values_, 1, 1, position_ );
	HHuginn::value_t res;
	if ( toType_ == HHuginn::TYPE::INTEGER ) {
		res = value_builtin::integer( values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::REAL ) {
		res = value_builtin::real( values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::STRING ) {
		res = value_builtin::string( values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::NUMBER ) {
		res = value_builtin::number( values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::BOOLEAN ) {
		res = value_builtin::boolean( values_.front(), position_ );
	} else if ( toType_ == HHuginn::TYPE::CHARACTER ) {
		res = value_builtin::character( values_.front(), position_ );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Conversion to `"_ys.append( toType_->name() ).append( "' is not supported." ),
			position_
		);
	}
	return ( res );
	M_EPILOG
}

inline HHuginn::value_t size( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "size", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	int long long s( 0 );
	HHuginn::type_t typeId( v->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		s = static_cast<HHuginn::HString const*>( v )->value().get_length();
	} else if ( typeId == HHuginn::TYPE::LIST ) {
		s = static_cast<HHuginn::HList const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::DICT ) {
		s = static_cast<HHuginn::HDict const*>( v )->size();
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Getting size of `"_ys.append( v->type()->name() ).append( "'s is not supported." ),
			position_
		);
	}
	return ( make_pointer<HHuginn::HInteger>( s ) );
	M_EPILOG
}

inline HHuginn::value_t type( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "type", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( make_pointer<HHuginn::HString>( v->type()->name() ) );
	M_EPILOG
}

inline HHuginn::value_t copy( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "copy", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	return ( v->clone() );
	M_EPILOG
}

inline HHuginn::value_t list( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( make_pointer<HHuginn::HList>() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t dict( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict", values_, 0, 0, position_ );
	return ( make_pointer<HHuginn::HDict>() );
	M_EPILOG
}

inline HHuginn::value_t order( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( make_pointer<HHuginn::HOrder>() );
	HHuginn::HOrder* o( static_cast<HHuginn::HOrder*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		o->insert( e, position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t lookup( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup", values_, 0, 0, position_ );
	return ( make_pointer<HHuginn::HLookup>() );
	M_EPILOG
}

inline HHuginn::value_t set( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( make_pointer<HHuginn::HSet>() );
	HHuginn::HSet* s( static_cast<HHuginn::HSet*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		s->insert( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t print( huginn::HThread* thread_, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "print", values_, 1, 1, position_ );
	HHuginn::HValue const* v( values_.front().raw() );
	yaal::hcore::HStreamInterface& out( thread_->huginn().output_stream() );
	HHuginn::type_t typeId( v->type() );
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
			"Printing `"_ys.append( v->type()->name() ).append( "'s is not supported." ),
			position_
		);
	}
	out << flush;
	return ( _none_ );
	M_EPILOG
}

inline HHuginn::value_t input( huginn::HThread* thread_, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "input", values_, 0, 0, position_ );
	yaal::hcore::HString l;
	thread_->huginn().input_stream().read_until( l );
	return ( make_pointer<HHuginn::HString>( l ) );
	M_EPILOG
}

inline HHuginn::value_t assert( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "assert";
	verify_arg_count( name, values_, 2, 3, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::BOOLEAN, false, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
	if ( ! get_boolean( values_[0] ) ) {
		throw HHuginn::HHuginnRuntimeException( get_string( values_[values_.get_size() - 1] ), position_ );
	}
	return ( _none_ );
	M_EPILOG
}

}

void HHuginn::register_builtins( void ) {
	M_PROLOG
	_functions.insert( make_pair<yaal::hcore::HString const>( "integer", hcore::call( &huginn_builtin::convert, TYPE::INTEGER, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "real", hcore::call( &huginn_builtin::convert, TYPE::REAL, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "string", hcore::call( &huginn_builtin::convert, TYPE::STRING, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "number", hcore::call( &huginn_builtin::convert, TYPE::NUMBER, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "boolean", hcore::call( &huginn_builtin::convert, TYPE::BOOLEAN, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "character", hcore::call( &huginn_builtin::convert, TYPE::CHARACTER, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "size", hcore::call( &huginn_builtin::size, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "type", hcore::call( &huginn_builtin::type, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "copy", hcore::call( &huginn_builtin::copy, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "list", hcore::call( &huginn_builtin::list, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "dict", hcore::call( &huginn_builtin::dict, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "order", hcore::call( &huginn_builtin::order, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "lookup", hcore::call( &huginn_builtin::lookup, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "set", hcore::call( &huginn_builtin::set, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "print", hcore::call( &huginn_builtin::print, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "input", hcore::call( &huginn_builtin::input, _1, _2, _3, _4 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "assert", hcore::call( &huginn_builtin::assert, _1, _2, _3, _4 ) ) );
	return;
	M_EPILOG
}

}

}


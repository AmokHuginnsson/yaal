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
HHuginn::type_t const HHuginn::TYPE::STRING( HHuginn::HType::register_type( "string", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::NUMBER( HHuginn::HType::register_type( "number", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::BOOLEAN( HHuginn::HType::register_type( "boolean", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::CHARACTER( HHuginn::HType::register_type( "character", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::LIST( HHuginn::HType::register_type( "list", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::MAP( HHuginn::HType::register_type( "map", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::REFERENCE( HHuginn::HType::register_type( "*reference*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::FUNCTION_REFERENCE( HHuginn::HType::register_type( "*function_reference*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::UNKNOWN( HHuginn::HType::register_type( "*unknown*", nullptr ) );
HHuginn::type_t const HHuginn::TYPE::NOT_BOOLEAN( HHuginn::HType::register_type( "*not_boolean*", nullptr ) );

namespace {

typedef yaal::hcore::HHashSet<yaal::hcore::HString>  words_t;

words_t _directives_ = {{
	"import"
}};

words_t _keywords_ = {{
	"break",
	"case",
	"catch",
	"class",
	"continue",
	"default",
	"else",
	"for",
	"if",
	"nil",
	"none",
	"null",
	"return",
	"switch",
	"this",
	"throw",
	"while"
}};

words_t _builtin_ = {{
	"boolean",
	"character",
	"integer",
	"list",
	"map",
	"number",
	"real",
	"string",
	"type"
}};

words_t _standardLibrary_ = {{
	"file",
	"print"
}};

bool is_restricted( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) || ( _standardLibrary_.count( name_ ) > 0 ) );
	M_EPILOG
}

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

}

HHuginn::value_t _none_ = make_pointer<HHuginn::HValue>( HHuginn::TYPE::NONE );

void operands_type_mismatch( char const*, HHuginn::type_t, HHuginn::type_t, int ) __attribute__(( noreturn ));

executing_parser::HRule HHuginn::make_engine( void ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	hcore::HString identifier( YAAL_REGEX_WORD_START "[a-zA-Z_][a-zA-Z0-9_]*" YAAL_REGEX_WORD_END );
	HRule expression( "expression", e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::mark_expression_position, &_compiler, _1 ) ) );
	HRule absoluteValue( "absoluteValue",
		constant( '|', e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) ) )
		>> expression
		>> constant( '|', e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::ABSOLUTE, _1 ) ) ) );
	HRule parenthesis( "parenthesis",
		constant( '(', e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) ) )
		>> expression
		>> constant( ')', e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::PARENTHESIS, _1 ) ) ) );
	HRule arg(
		"argument",
		expression,
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::FUNCTION_ARGUMENT, _1 ) )
	);
	HRule argList( "argList", arg >> ( * ( ',' >> arg ) ) );
	HRule functionCallOperator(
		"functionCallOperator",
		constant(
			'(',
			HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -argList >> ')',
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule stringLiteral(
		"stringLiteral",
		string_literal[e_p::HStringLiteral::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_store_string, &_compiler, _1, _2 ) )]
	);
	HRule literalNone(
		"none",
		constant( "none", e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_store_none, &_compiler, _1 ) ) )
	);
	HRule booleanLiteralTrue(
		"true",
		constant( "true", e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_store_boolean, &_compiler, true, _1 ) ) )
	);
	HRule booleanLiteralFalse(
		"false",
		constant( "false", e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_store_boolean, &_compiler, false, _1 ) ) )
	);
	HRule numberLiteral(
		"numberLiteral",
		constant( '$' ) >> real[e_p::HReal::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_store_number, &_compiler, _1, _2 ) )]
	);
	HRule listLiteral(
		"listLiteral",
		constant(
			'[',
			HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_call, &_compiler, "list", _1 ) )
		) >> -argList >> ']',
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule mapLiteralElement(
		"mapLiteralElement",
		arg >> ':' >>	arg
	);
	HRule mapLiteral(
		"mapLiteral",
		constant(
			'{',
			HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -( mapLiteralElement >> *( ',' >> mapLiteralElement ) ) >> '}',
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::MAKE_MAP, _1 ) )
	);
	HRule parameter(
		"parameter",
		regex(
			"parameterIdentifier",
			identifier,
			HRegex::action_string_position_t( hcore::call( &HHuginn::OCompiler::add_paramater, &_compiler, _1, _2 ) )
		) >> -( constant( '=' ) >> HRule( expression, HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::add_default_value, &_compiler, _1 ) ) ) )
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
			e_p::HRegex::action_position_t( hcore::call( &HHuginn::OCompiler::set_lambda_name, &_compiler, _1 ) )
		) >> '(' >> -nameList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::verify_default_argument, &_compiler, _1 ) ) )
		>> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::create_lambda, &_compiler, _1 ) )
	);
	HRule rangeOper(
		"rangeOper",
		constant(
			':',
			e_p::HCharacter::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::SUBSCRIPT_ARGUMENT, _1 ) )
		)
	);
	HRule subscriptOperator(
		"subscriptOperator",
		constant(
			'[',
			e_p::HCharacter::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::SUBSCRIPT, _1 ) )
		) >> ( ( ( rangeOper >> -arg ) | ( arg >> -( rangeOper >> -arg ) ) ) >> -( rangeOper >> -arg ) ) >> ']',
		e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::SUBSCRIPT, _1 ) )
	);
	HRule reference(
		regex(
			"reference",
			identifier,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_get_reference, &_compiler, _1, _2 ) )
		)
	);
	HRule dereference( "dereference", ( reference >> *( subscriptOperator | functionCallOperator ) ) );
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_position_t( hcore::call( &HHuginn::OCompiler::defer_store_real, &_compiler, _1, _2 ) )]
		| numberLiteral
		| integer[e_p::HInteger::action_int_long_long_position_t( hcore::call( &HHuginn::OCompiler::defer_store_integer, &_compiler, _1, _2 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_store_character, &_compiler, _1, _2 ) )]
		| ( listLiteral >> -( subscriptOperator >> *( subscriptOperator | functionCallOperator ) ) )
		| ( mapLiteral >> -( subscriptOperator >> *( subscriptOperator | functionCallOperator ) ) )
		| literalNone | booleanLiteralTrue | booleanLiteralFalse
		| dereference
		| ( stringLiteral >> -subscriptOperator )
		| ( lambda >> -( functionCallOperator >> *( subscriptOperator | functionCallOperator ) ) )
	);
	HRule negation(
		"negation",
		( constant( '-' )[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::NEGATE, _1 ) )] >> atom )[
			e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::NEGATE, _1 ) )
		] | atom
	);
	HRule booleanNot(
		"booleanNot", (
			constant( '!' )[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_oper_direct, &_compiler, OPERATOR::BOOLEAN_NOT, _1 ) )]
			>> negation
		)[
			e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::BOOLEAN_NOT, _1 ) )
		] | negation
	);
	HRule power(
		"power",
		booleanNot >> (
			* ( constant( '^', e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) ) ) >> booleanNot )
		),
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::POWER, _1 ) )
	);
	HRule multiplication(
		"multiplication",
		power >> (
			* (
					(
					characters(
						"*/%", e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) )
					) >> power
				)[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::MULTIPLY, _1 ) )]
			)
		)
	);
	HRule sum(
		"sum",
		multiplication >> (
			* (
					(
					characters(
						"+-", e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) )
					) >> multiplication
				)[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::PLUS, _1 ) )]
			)
		)
	);
	HRule compare(
		"compare",
		sum >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( "<=" ) | ">=" | "<" | ">"
				)[e_p::HString::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1, _2 ) )]
				>> sum
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::LESS, _1 ) )]
		)
	);
	HRule equality(
		"equality",
		compare >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( "==" ) | "!="
				)[e_p::HString::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1, _2 ) )]
				>> compare
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::EQUALS, _1 ) )]
		)
	);
	/*
	 * ( a == b ) && ( c == d ) || ( e == f ) && ( g == h )
	 */
	HRule booleanAnd(
		"booleanAnd",
		equality[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::start_subexpression, &_compiler, _1 ) )] >> *(
			/* compare action */ (
				constant( "&&" )[e_p::HString::action_position_t( hcore::call( &HHuginn::OCompiler::add_subexpression, &_compiler, OPERATOR::BOOLEAN_AND, _1 ) )]
				>> equality
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::BOOLEAN_AND, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_boolean, &_compiler, OPERATOR::BOOLEAN_AND, _1 ) )
	);
	HRule booleanOr(
		"booleanOr",
		HRule( booleanAnd, e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::start_subexpression, &_compiler, _1 ) ) ) >> *(
			/* compare action */ (
				constant( "||" )[e_p::HString::action_position_t( hcore::call( &HHuginn::OCompiler::add_subexpression, &_compiler, OPERATOR::BOOLEAN_OR, _1 ) )]
				>> booleanAnd
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::BOOLEAN_OR, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_boolean, &_compiler, OPERATOR::BOOLEAN_OR, _1 ) )
	);
	HRule booleanXor(
		"booleanXor",
		booleanOr >> -(
			/* compare action */ (
				constant( "^^" )[e_p::HString::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1, _2 ) )]
				>> booleanOr
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::BOOLEAN_XOR, _1 ) )]
		)
	);
	HRule ternary(
		"ternary",
		HRule( booleanXor, e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::start_subexpression, &_compiler, _1 ) ) ) >> -(
			/* ternary action */ (
				constant( '?' )[e_p::HString::action_position_t( hcore::call( &HHuginn::OCompiler::add_subexpression, &_compiler, OPERATOR::TERNARY, _1 ) )]
				>> expression
				>> constant( ':' )[e_p::HString::action_position_t( hcore::call( &HHuginn::OCompiler::add_subexpression, &_compiler, OPERATOR::TERNARY, _1 ) )]
				>> expression
			)[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::TERNARY, _1 ) )]
		),
		e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_ternary, &_compiler, _1 ) )
	);
	HRule value( "value", ternary );
	HRule subscript( "subscript", ( reference >> +( subscriptOperator | functionCallOperator ) ) );
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	expression %= HRule(
		* (
			/* make reference */ (
				subscript[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::make_reference, &_compiler, _1 ) )]
				| regex(
					"variableSetter",
					identifier,
					e_p::HString::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_make_variable, &_compiler, _1, _2 ) )
				)
			) >>
			constant( '=' )[e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) )] ^ '='
		) >> value,
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::ASSIGN, _1 ) )
	);
	HRule expressionStatement(
		"expressionStatement",
		HRule( expression, HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_expression, &_compiler, _1 ) ) ) >> ';'
	);
	HRule ifClause(
		"ifClause",
		e_p::constant( "if" ) >> '(' >> expression >> ')' >> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_if_clause, &_compiler, _1 ) )
	);
	HRule ifStatement(
		"ifStatement",
		ifClause >>
		*( constant( "else" ) >> ifClause ) >>
		-( ( constant( "else" ) >> scope )[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_else_clause, &_compiler, _1 ) )] )
	);
	HRule continueStatement( "continueStatement", constant( "continue" ) >> ';' );
	/*
	 * TODO:
	 * Allow `break' statement to have literal integer argument
	 * telling how many scopes given `break' should break,
	 * i.e.:
	 * break ( 2 ); // <--- break two levels of nested scopes.
	 */
	HRule breakStatement( "breakStatement", constant( "break" ) >> ';' );
	HRule whileStatement( "whileStatement", constant( "while" ) >> '(' >> expression >> ')' >> scope );
	HRule forIdentifier(
		regex(
			"forIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &HHuginn::OCompiler::set_for_identifier, &_compiler, _1, _2 ) )
		)
	);
	HRule forStatement( "forStatement", constant( "for" ) >> '(' >> forIdentifier >> ':' >> expression >> ')' >> scope );
	HRule caseStatement(
		"caseStatement",
		constant( "case" ) >> '(' >> expression >> ')' >> ':' >> scope >> -( breakStatement[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::add_break_statement, &_compiler, _1 ) )] ),
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_if_clause, &_compiler, _1 ) )
	);
	HRule defaultStatement( "defaultStatement", constant( "default" ) >> ':' >> scope );
	HRule switchStatement(
		"switchStatement",
		constant( "switch" ) >> '(' >> expression >> ')' >>
		constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, &_compiler, _1 ) ) ) >> +caseStatement >>
		-( defaultStatement[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_else_clause, &_compiler, _1 ) )] ) >> '}'
	);
	HRule returnStatement( "returnStatement", constant( "return" ) >> -( '(' >> expression >> ')' ) >> ';' );
	HRule statement( "statement",
		ifStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_if_statement, &_compiler, _1 ) )]
		| whileStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_while_statement, &_compiler, _1 ) )]
		| forStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_for_statement, &_compiler, _1 ) )]
		| switchStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_switch_statement, &_compiler, _1 ) )]
		| breakStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_break_statement, &_compiler, _1 ) )]
		| continueStatement
		| returnStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_return_statement, &_compiler, _1 ) )]
		| expressionStatement
		| scope[HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::commit_scope, &_compiler, _1 ) )]
	);
	scope %= ( constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, &_compiler, _1 ) ) ) >> *statement >> '}' );
	HRule functionDefinition( "functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &HHuginn::OCompiler::set_function_name, &_compiler, _1, _2 ) )
		) >> '(' >> -nameList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::verify_default_argument, &_compiler, _1 ) ) )
		>> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::create_function, this, _1 ) )
	);
	HRule field( "field", regex( "fieldIdentifier", identifier ) >> '=' >> expression >> ';' );
	HRule classDefinition(
		"classDefinition",
		constant( "class" ) >> regex( "classIdentifier", identifier )
		>> -( ':' >> regex( "baseIdentifier", identifier ) )
		>> '{' >> +( field | functionDefinition ) >> '}'
	);
	HRule huginnGrammar( "huginnGrammar", + ( classDefinition | functionDefinition ) );
	return ( huginnGrammar );
	M_EPILOG
}

HHuginn::OCompiler::OScopeContext::OScopeContext( HHuginn* huginn_ )
	: _huginn( huginn_ ),
	_scope( make_pointer<HScope>() ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, make_pointer<HExpression>() );
	return;
}

HHuginn::OCompiler::OScopeContext::OScopeContext( HHuginn* huginn_, scope_t const& scope_, expression_t const& expression_ )
	: _huginn( huginn_ ),
	_scope( scope_ ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, expression_ );
	return;
}

void HHuginn::OCompiler::OScopeContext::clear( void ) {
	M_PROLOG
	_scope = make_pointer<HScope>();
	_expressionsStack.clear();
	_expressionsStack.emplace( 1, make_pointer<HExpression>() );
	return;
	M_EPILOG
}

HHuginn::expression_t const& HHuginn::OCompiler::OScopeContext::expression( void ) const {
	return ( _expressionsStack.top().back() );
}

HHuginn::expression_t& HHuginn::OCompiler::OScopeContext::expression( void ) {
	return ( _expressionsStack.top().back() );
}

HHuginn::OCompiler::OCompilationFrame::OCompilationFrame( HHuginn* huginn_ )
	: _context( huginn_ ),
	_contextsChain(),
	_else(),
	_forIdentifier(),
	_forPosition( 0 ) {
	return;
}

void HHuginn::OCompiler::OCompilationFrame::clear( void ) {
	M_PROLOG
	_contextsChain.clear();
	_else.reset();
	_forIdentifier.clear();
	_forPosition = 0;
	return;
	M_EPILOG
}

HHuginn::OCompiler::OFunctionContext::OFunctionContext( HHuginn* huginn_ )
	: _functionName(),
	_parameters(),
	_defaultValues(),
	_lastDefaultValuePosition( -1 ),
	_compilationStack(),
	_operations(),
	_valueTypes(),
	_lastDereferenceOperator( OPERATOR::NONE ) {
	_compilationStack.emplace( huginn_ );
	return;
}

HHuginn::OCompiler::OCompiler( HHuginn* huginn_ )
	: _functionContexts(),
	_huginn( huginn_ ) {
	return;
}

HHuginn::OCompiler::OFunctionContext& HHuginn::OCompiler::f( void ) {
	return ( _functionContexts.top() );
}

HHuginn::scope_t& HHuginn::OCompiler::current_scope( void ) {
	return ( f()._compilationStack.top()._context._scope );
}

HHuginn::expression_t& HHuginn::OCompiler::current_expression( void ) {
	return ( f()._compilationStack.top()._context.expression() );
}

void HHuginn::OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	_functionContexts.emplace( _huginn );
	if ( is_restricted( name_ ) ) {
		throw HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ), position_.get() );
	}
	f()._functionName = name_;
	return;
	M_EPILOG
}

void HHuginn::OCompiler::set_lambda_name( executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::HErrorCoordinate ec( _huginn->get_coordinate( position_.get() ) );
	_functionContexts.emplace( _huginn );
	f()._functionName.assign( "@" ).append( ec.line() ).append( ":" ).append( ec.column() );
	return;
	M_EPILOG
}

HHuginn::function_t HHuginn::OCompiler::create_function( executing_parser::position_t ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	M_ASSERT( ! fc._functionName.is_empty() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	function_t fun(
		hcore::call(
			&HFunction::execute,
			make_pointer<HFunction>(
				fc._functionName,
				fc._parameters,
				yaal::move( current_scope() ),
				fc._defaultValues
			),
			_1, _2, _3
		)
	);
	fc._compilationStack.pop();
	M_ASSERT( fc._compilationStack.get_size() == 1 );
	fc._compilationStack.top().clear();
	fc._parameters.clear();
	fc._defaultValues.clear();
	fc._lastDefaultValuePosition = -1;
	return ( fun );
	M_EPILOG
}

void HHuginn::OCompiler::create_lambda( executing_parser::position_t position_ ) {
	M_PROLOG
	function_t fun( create_function( position_ ) );
	value_t fRef( make_pointer<HFunctionReference>( f()._functionName, fun ) );
	_functionContexts.pop();
	defer_store_direct( fRef, position_ );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::verify_default_argument( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int lastDefaultValuePosition( fc._lastDefaultValuePosition );
	if ( ( lastDefaultValuePosition >= 0 ) && ( lastDefaultValuePosition < static_cast<int>( fc._parameters.get_size() - 1 ) ) ) {
		throw HHuginnRuntimeException( "Missing default argument.", position_.get() );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_paramater( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	verify_default_argument( position_ );
	f()._parameters.push_back( name_ );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::create_scope( executing_parser::position_t ) {
	M_PROLOG
	f()._compilationStack.emplace( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_scope( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	scope_t scope( current_scope() );
	fc._compilationStack.pop();
	M_ASSERT( ! fc._compilationStack.is_empty() );
	current_scope()->add_statement( scope );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::reset_expression( void ) {
	M_PROLOG
	current_expression() = make_pointer<HExpression>();
	OFunctionContext& fc( f() );
	M_ASSERT( fc._operations.is_empty() );
	fc._valueTypes.clear();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::start_subexpression( executing_parser::position_t position_ ) {
	M_PROLOG
	f()._compilationStack.top()._context._expressionsStack.emplace( 1, make_pointer<HExpression>( position_.get() ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_subexpression( OPERATOR op_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._compilationStack.top()._context._expressionsStack.top().emplace_back( make_pointer<HExpression>( position_.get() ) );
	fc._operations.emplace( op_, position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_boolean( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._compilationStack.top()._context._expressionsStack.top().get_size() > 1 ) {
		value_t And( make_pointer<HBooleanEvaluator>( fc._compilationStack.top()._context._expressionsStack.top(), operator_ ) );
		fc._compilationStack.top()._context._expressionsStack.pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == TYPE::BOOLEAN );
		fc._valueTypes.pop();
		defer_store_direct( And, position_ );
		current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
		defer_action( operator_ == OPERATOR::BOOLEAN_AND ? &HExpression::boolean_and : &HExpression::boolean_or, position_ );
	} else {
		expression_t e( fc._compilationStack.top()._context.expression() );
		fc._compilationStack.top()._context._expressionsStack.pop();
		fc._compilationStack.top()._context.expression()->merge( *e );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_ternary( executing_parser::position_t position_ ) {
	OFunctionContext& fc( f() );
	expressions_t const& exprs( fc._compilationStack.top()._context._expressionsStack.top() );
	if ( exprs.get_size() > 1 ) {
		M_ASSERT( exprs.get_size() == 3 );
		value_t ternary( make_pointer<HTernaryEvaluator>( exprs[0], exprs[1], exprs[2] ) );
		fc._compilationStack.top()._context._expressionsStack.pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == TYPE::UNKNOWN );
		fc._valueTypes.pop();
		defer_store_direct( ternary, position_ );
		current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), OPERATOR::TERNARY, _1, position_.get() ) );
		defer_action( &HExpression::ternary, position_ );
	} else {
		expression_t e( fc._compilationStack.top()._context.expression() );
		fc._compilationStack.top()._context._expressionsStack.pop();
		fc._compilationStack.top()._context.expression()->merge( *e );
	}
}

void HHuginn::OCompiler::add_return_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HReturn>( current_expression() ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_break_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HBreak>() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_while_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	scope_t scope( current_scope() );
	fc._compilationStack.pop();
	current_scope()->add_statement( make_pointer<HWhile>( current_expression(), scope ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::set_for_identifier( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OCompilationFrame& cf( f()._compilationStack.top() );
	cf._forIdentifier = name_;
	cf._forPosition = position_.get();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_for_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	scope_t scope( current_scope() );
	fc._compilationStack.pop();
	OCompilationFrame const& cf( fc._compilationStack.top() );
	current_scope()->add_statement( make_pointer<HFor>( cf._forIdentifier, current_expression(), scope, cf._forPosition ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_if_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	scope_t scope( current_scope() );
	fc._compilationStack.pop();
	fc._compilationStack.top()._contextsChain.emplace_back( _huginn, scope, current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_else_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	scope_t scope( current_scope() );
	fc._compilationStack.pop();
	fc._compilationStack.top()._else = scope;
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_if_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	statement_t ifStatement( make_pointer<HIf>( fc._compilationStack.top()._contextsChain, fc._compilationStack.top()._else ) );
	fc._compilationStack.top()._contextsChain.clear();
	fc._compilationStack.top()._else.reset();
	current_scope()->add_statement( ifStatement );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_switch_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	OCompilationFrame::contexts_t contexts( yaal::move( fc._compilationStack.top()._contextsChain ) );
	scope_t Default( fc._compilationStack.top()._else );
	fc._compilationStack.pop();
	statement_t switchStatement(
		make_pointer<HSwitch>(
			current_expression(),
			contexts,
			Default
		)
	);
	current_scope()->add_statement( switchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_expression( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_default_value( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	fc._lastDefaultValuePosition = static_cast<int>( fc._parameters.get_size() - 1 );
	fc._defaultValues.push_back( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::mark_expression_position( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_expression()->set_position( position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_oper( char operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::OPERATOR o( HHuginn::OPERATOR::NONE );
	switch ( operator_ ) {
		case ( '+' ): o = HHuginn::OPERATOR::PLUS;        break;
		case ( '-' ): o = HHuginn::OPERATOR::MINUS;       break;
		case ( '*' ): o = HHuginn::OPERATOR::MULTIPLY;    break;
		case ( '/' ): o = HHuginn::OPERATOR::DIVIDE;      break;
		case ( '%' ): o = HHuginn::OPERATOR::MODULO;      break;
		case ( '^' ): o = HHuginn::OPERATOR::POWER;       break;
		case ( '(' ): o = HHuginn::OPERATOR::PARENTHESIS; break;
		case ( '|' ): o = HHuginn::OPERATOR::ABSOLUTE;    break;
		case ( '=' ): o = HHuginn::OPERATOR::ASSIGN;      break;
		case ( '!' ): o = HHuginn::OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), o, _1, position_.get() ) );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_str_oper( yaal::hcore::HString const& operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, HHuginn::OPERATOR> operator_lookup_t;
	static operator_lookup_t const operatorLookup( {
		{ "==", HHuginn::OPERATOR::EQUALS },
		{ "!=", HHuginn::OPERATOR::NOT_EQUALS },
		{ "<=", HHuginn::OPERATOR::LESS_OR_EQUAL },
		{ ">=", HHuginn::OPERATOR::GREATER_OR_EQUAL },
		{ "&&", HHuginn::OPERATOR::BOOLEAN_AND },
		{ "||", HHuginn::OPERATOR::BOOLEAN_OR },
		{ "^^", HHuginn::OPERATOR::BOOLEAN_XOR },
		{ "<", HHuginn::OPERATOR::LESS },
		{ ">", HHuginn::OPERATOR::GREATER }
	} );
	OPERATOR o( operatorLookup.at( operator_ ) );
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), o, _1, position_.get() ) );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_oper_direct( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
	f()._operations.emplace( operator_, position_.get() );
	return;
	M_EPILOG
}

bool HHuginn::OCompiler::is_numeric( type_t type_ ) {
	return ( ( type_ == TYPE::INTEGER ) || ( type_ == TYPE::REAL ) || ( type_ == TYPE::NUMBER ) );
}

bool HHuginn::OCompiler::is_comparable( type_t type_ ) {
	return (
		is_numeric( type_ )
		|| ( type_ == TYPE::STRING )
		|| ( type_ == TYPE::CHARACTER )
	);
}

bool HHuginn::OCompiler::is_boolean_congruent( type_t type_ ) {
	return ( ( type_ == TYPE::BOOLEAN ) || ( type_ == TYPE::UNKNOWN ) || ( type_ == TYPE::REFERENCE ) );
}

bool HHuginn::OCompiler::is_unknown( type_t type_ ) {
	return ( ( type_ == TYPE::NOT_BOOLEAN ) || ( type_ == TYPE::UNKNOWN ) || ( type_ == TYPE::REFERENCE ) );
}

bool HHuginn::OCompiler::is_numeric_congruent( type_t type_ ) {
	return ( is_numeric( type_ ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_comparable_congruent( type_t type_ ) {
	return ( is_comparable( type_ ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_reference_congruent( type_t type_ ) {
	return ( ( type_ == TYPE::REFERENCE ) || ( type_ == TYPE::UNKNOWN ) );
}

bool HHuginn::OCompiler::is_integer_congruent( type_t type_ ) {
	return ( ( type_ == TYPE::INTEGER ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_summable( type_t type_ ) {
	return ( is_numeric_congruent( type_ ) || ( type_ == TYPE::STRING ) );
}

bool HHuginn::OCompiler::are_congruous( type_t t1_, type_t t2_ ) {
	bool congruous(
		( t1_ == t2_ )
		|| ( t1_ == TYPE::UNKNOWN )
		|| ( t2_ == TYPE::UNKNOWN )
		|| ( t1_ == TYPE::REFERENCE )
		|| ( t2_ == TYPE::REFERENCE )
	);
	if ( ! congruous ) {
		congruous = ( ( t1_ != TYPE::BOOLEAN ) && ( t2_ == TYPE::NOT_BOOLEAN ) )
			|| ( ( t2_ != TYPE::BOOLEAN ) && ( t1_ == TYPE::NOT_BOOLEAN ) );
	}
	return ( congruous );
}

HHuginn::type_t HHuginn::OCompiler::congruent( type_t t1_, type_t t2_ ) {
	type_t t( TYPE::NOT_BOOLEAN );
	if ( t1_ == t2_ ) {
		if ( ( t1_ != TYPE::UNKNOWN ) && ( t1_ != TYPE::REFERENCE ) ) {
			t = t1_;
		}
	} else if ( ! ( is_unknown( t1_ ) && is_unknown( t2_ ) ) ) {
		if ( is_unknown( t1_ ) ) {
			t = t2_;
		} else {
			t = t1_;
		}
	}
	return ( t );
}

void HHuginn::OCompiler::dispatch_plus( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::PLUS ) || ( o == OPERATOR::MINUS ) );
	defer_action( o == OPERATOR::PLUS ? &HExpression::plus : &HExpression::minus, position_ );
	fc._operations.pop();
	type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_summable( t1 ) && is_summable( t2 ) ) ) {
		throw HHuginnRuntimeException( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB], p );
	}
	fc._valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_mul( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::MULTIPLY ) || ( o == OPERATOR::DIVIDE ) || ( o == OPERATOR::MODULO ) );
	defer_action( o == OPERATOR::MULTIPLY ? &HExpression::mul : ( o == OPERATOR::DIVIDE ? &HExpression::div : &HExpression::mod ), position_ );
	fc._operations.pop();
	type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV], p );
	}
	fc._valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_power( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	defer_action( &HExpression::power, position_ );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::POWER ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		type_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		type_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
			throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP], p );
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_compare( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::LESS ) || ( o == OPERATOR::GREATER ) || ( o == OPERATOR::LESS_OR_EQUAL ) || ( o == OPERATOR::GREATER_OR_EQUAL ) );
	switch ( o ) {
		case ( OPERATOR::LESS ):             { defer_action( &HExpression::less, position_ );             } break;
		case ( OPERATOR::GREATER ):          { defer_action( &HExpression::greater, position_ );          } break;
		case ( OPERATOR::LESS_OR_EQUAL ):    { defer_action( &HExpression::less_or_equal, position_ );    } break;
		case ( OPERATOR::GREATER_OR_EQUAL ): { defer_action( &HExpression::greater_or_equal, position_ ); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_comparable_congruent( t1 ) && is_comparable_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP], p );
	}
	fc._valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_boolean( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	int p( po._position );
	if ( !! action_ ) {
		defer_action( action_, position_ );
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( is_boolean_congruent( t1 ) && is_boolean_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], p );
	}
	fc._valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_ternary( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::TERNARY );
	fc._operations.pop();
	M_ASSERT( fc._operations.top()._operator == OPERATOR::TERNARY );
	int p( fc._operations.top()._position );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 3 );
	fc._valueTypes.pop();
	fc._valueTypes.pop();
	type_t t0( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! is_boolean_congruent( t0 ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
	}
	fc._valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_equals( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::EQUALS ) || ( o == OPERATOR::NOT_EQUALS ) );
	defer_action( o == OPERATOR::EQUALS ? &HExpression::equals : &HExpression::not_equals, position_ );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( ( o == OPERATOR::EQUALS ? "==" : "!=" ), t1, t2, p );
	}
	fc._valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_assign( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	defer_action( &HExpression::set_variable, position_ );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::ASSIGN ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		type_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		type_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( ! is_reference_congruent( t2 ) ) {
			throw HHuginnRuntimeException( "Setting a non reference location.", p );
		}
		fc._valueTypes.push( t1 );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_subscript( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPERATOR op( fc._operations.top()._operator );
	int range( 0 );
	bool nonInteger( false );
	int p( position_.get() );
	while ( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
		if ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) {
			++ range;
		} else {
			if ( ! ( nonInteger || is_integer_congruent( fc._valueTypes.top() ) || ( fc._valueTypes.top() == TYPE::NONE ) ) ) {
				nonInteger = true;
				p = fc._operations.top()._position;
			}
			fc._valueTypes.pop();
		}
		fc._operations.pop();
		op = fc._operations.top()._operator;
	}
	if ( ( range > 0 ) && nonInteger ) {
		throw HHuginnRuntimeException( "Range specifier is not an integer.", p );
	}
	M_ASSERT( fc._operations.top()._operator == OPERATOR::SUBSCRIPT );
	current_expression()->add_execution_step( hcore::call( &HExpression::subscript, current_expression().raw(), HExpression::SUBSCRIPT::VALUE, _1, position_.get() ) );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 1 );
	fc._valueTypes.pop();
	fc._valueTypes.push( TYPE::REFERENCE );
	fc._lastDereferenceOperator = OPERATOR::SUBSCRIPT;
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_function_call( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) ) {
		fc._operations.pop();
		fc._valueTypes.pop();
	}
	if ( action_ == &HExpression::function_call ) {
		fc._valueTypes.pop();
	}
	fc._valueTypes.push( TYPE::UNKNOWN );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::FUNCTION_CALL );
	defer_action( action_, position_ );
	fc._operations.pop();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_action( OPERATOR oper_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._lastDereferenceOperator = OPERATOR::NONE;
	OPositionedOperator po( ! fc._operations.is_empty() ? fc._operations.top() : OPositionedOperator( OPERATOR::NONE, 0 ) );
	OPERATOR o( po._operator );
	int p( po._position );
	switch ( oper_ ) {
		case ( OPERATOR::PLUS ):     { dispatch_plus( position_ );  } break;
		case ( OPERATOR::MULTIPLY ): { dispatch_mul( position_ );   } break;
		case ( OPERATOR::POWER ):    { dispatch_power( position_ ); } break;
		case ( OPERATOR::NEGATE ): {
			M_ASSERT( o == OPERATOR::NEGATE );
			M_ASSERT( ! fc._valueTypes.is_empty() );
			defer_action( &HExpression::negate, position_ );
			fc._operations.pop();
			if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
				throw HHuginnRuntimeException( "Operand is not a numeric value.", p );
			}
		} break;
		case ( OPERATOR::SUBSCRIPT ):     { dispatch_subscript( position_ );     } break;
		case ( OPERATOR::ASSIGN ):        { dispatch_assign( position_ );        } break;
		case ( OPERATOR::FUNCTION_CALL ): { dispatch_function_call( &HExpression::function_call, position_ ); } break;
		case ( OPERATOR::MAKE_MAP ):      { dispatch_function_call( &HExpression::make_map, position_ ); } break;
		case ( OPERATOR::PARENTHESIS ):
		case ( OPERATOR::ABSOLUTE ): {
			M_ASSERT( ( o == OPERATOR::ABSOLUTE ) || ( o == OPERATOR::PARENTHESIS ) );
			defer_action( &HExpression::close_parenthesis, position_ );
			fc._operations.pop();
			if ( o == OPERATOR::ABSOLUTE ) {
				M_ASSERT( ! fc._valueTypes.is_empty() );
				if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
					throw HHuginnRuntimeException( "Operand is not a numeric value.", p );
				}
			}
		} break;
		case ( OPERATOR::EQUALS ): { dispatch_equals( position_ );  } break;
		case ( OPERATOR::LESS ):   { dispatch_compare( position_ ); } break;
		case ( OPERATOR::BOOLEAN_AND ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_AND );
			dispatch_boolean( nullptr, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_OR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_OR );
			dispatch_boolean( nullptr, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_XOR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_XOR );
			dispatch_boolean( &HExpression::boolean_xor, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_NOT ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_NOT );
			M_ASSERT( ! fc._valueTypes.is_empty() );
			defer_action( &HExpression::boolean_not, position_ );
			fc._operations.pop();
			if ( ! is_boolean_congruent( fc._valueTypes.top() ) ) {
				throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
			}
		} break;
		case ( OPERATOR::TERNARY ): { dispatch_ternary(); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_action( expression_action_t const& expressionAction_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( expressionAction_, current_expression().raw(), _1, position_.get() ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_call( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	defer_get_reference( name_, position_ );
	defer_oper_direct( OPERATOR::FUNCTION_CALL, position_ );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::make_reference( executing_parser::position_t position_ ) {
	M_PROLOG
	if ( f()._lastDereferenceOperator != OPERATOR::SUBSCRIPT ) {
		throw HHuginnRuntimeException( "Assignment to function result.", position_.get() );
	}
	current_expression()->pop_execution_step();
	current_expression()->add_execution_step( hcore::call( &HExpression::subscript, current_expression().raw(), HExpression::SUBSCRIPT::REFERENCE, _1, position_.get() ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_get_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::get_reference, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_make_variable( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::make_variable, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_direct( value_t const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( value_->type() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_real( double long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_real, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::REAL );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_integer( int long long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_integer, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::INTEGER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_string( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_string, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::STRING );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_number( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_number, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::NUMBER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_character( char value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_character, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::CHARACTER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_boolean( bool value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_boolean, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_none( executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_none, current_expression().raw(), _1, position_.get() ) );
	f()._valueTypes.push( TYPE::NONE );
	return;
	M_EPILOG
}

HHuginn::HHuginnRuntimeException::HHuginnRuntimeException( yaal::hcore::HString const& message_, int position_ )
	: _message( message_ ),
	_position( position_ ) {
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

HHuginn::flag_t HHuginn::_grammarVerified{ false };

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY )
	, _idGenerator{ HType::builtin_type_count() }
	, _userTypeDict()
	, _classes()
	, _functions()
	, _engine( make_engine(), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
	, _source( make_resource<HSource>() )
	, _compiler( this )
	, _threads()
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

bool HHuginn::compile( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED );
	_state = STATE::COMPILED;
	bool ok( false );
	try {
		_engine.execute();
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
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	threads_t::iterator t( _threads.insert( make_pair( threadId, make_pointer<huginn::HThread>( this, threadId ) ) ).first );
	values_t args;
	if ( _argv->size() > 0 ) {
		args.push_back( _argv );
	}
	bool ok( false );
	try {
		_result = call( "main", args, 0 );
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
		res = f->second( t->second.raw(), values_, position_ );
	} else {
		throw HHuginnRuntimeException( "function `"_ys.append( name_ ).append( "' is not defined" ), position_ );
	}
	return ( res );
	M_EPILOG
}

huginn::HFrame* HHuginn::current_frame( void ) {
	M_PROLOG
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	threads_t::iterator t( _threads.find( threadId ) );
	M_ASSERT( t != _threads.end() );
	return ( t->second->current_frame() );
	M_EPILOG
}

HHuginn::value_t HHuginn::result( void ) const {
	return ( _result );
}

HHuginn::function_t HHuginn::get_function( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	functions_t::const_iterator it( _functions.find( name_ ) );
	return ( it != _functions.end() ? it->second : function_t() );
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

void HHuginn::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _source->name() << "':\nfunctions:" << endl;
	for ( functions_t::value_type const& f : _functions ) {
		stream_ << f.first << endl;
	}
	return;
	M_EPILOG
}

void HHuginn::create_function( executing_parser::position_t position_ ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( _compiler.f() );
	_functions.insert( make_pair<hcore::HString const, function_t>( yaal::move( fc._functionName ), _compiler.create_function( position_ ) ) );
	_compiler._functionContexts.pop();
	return;
	M_EPILOG
}

HHuginn::HValue::HValue( type_t type_ )
	: _type( type_ ), _methods() {
	return;
}

HHuginn::type_t HHuginn::HValue::type( void ) const {
	return ( _type );
}

HHuginn::HReference::HReference( HHuginn::value_t& value_ )
	: HValue( TYPE::REFERENCE ), _value( value_ ) {
	return;
}

HHuginn::value_t& HHuginn::HReference::value( void ) const {
	return ( _value );
}

HHuginn::HReal::HReal( double long value_ )
	: HValue( TYPE::REAL ), _value( value_ ) {
	return;
}

double long HHuginn::HReal::value( void ) const {
	return ( _value );
}

HHuginn::HBoolean::HBoolean( bool value_ )
	: HValue( TYPE::BOOLEAN ), _value( value_ ) {
	return;
}

bool HHuginn::HBoolean::value( void ) const {
	return ( _value );
}

HHuginn::HInteger::HInteger( int long long value_ )
	: HValue( TYPE::INTEGER ), _value( value_ ) {
	return;
}

int long long HHuginn::HInteger::value( void ) const {
	return ( _value );
}

HHuginn::HNumber::HNumber( yaal::hcore::HNumber const& value_ )
	: HValue( TYPE::NUMBER ), _value( value_ ) {
	return;
}

yaal::hcore::HNumber const& HHuginn::HNumber::value( void ) const {
	return ( _value );
}

HHuginn::HCharacter::HCharacter( char value_ )
	: HValue( TYPE::CHARACTER ), _value( value_ ) {
	return;
}

char HHuginn::HCharacter::value( void ) const {
	return ( _value );
}

namespace {

class HIteratorInterface {
public:
	virtual ~HIteratorInterface( void ) {
		return;
	}
	HHuginn::value_t value( void ) {
		return ( do_value() );
	}
	bool is_valid( void ) {
		return ( do_is_valid() );
	}
	void next( void ) {
		do_next();
	}
protected:
	virtual HHuginn::value_t do_value( void ) = 0;
	virtual bool do_is_valid( void ) = 0;
	virtual void do_next( void ) = 0;
};

class HStringIterator : public HIteratorInterface {
	HHuginn::HString* _string;
	int _index;
public:
	HStringIterator( HHuginn::HString* string_ )
		: _string( string_ ),
		_index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( make_pointer<HHuginn::HCharacter>( _string->value()[ _index ] ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _string->value().get_size() );
	}
	virtual void do_next( void ) override {
		++ _index;
	}
private:
	HStringIterator( HStringIterator const& ) = delete;
	HStringIterator& operator = ( HStringIterator const& ) = delete;
};

class HListIterator : public HIteratorInterface {
	HHuginn::HList* _list;
	int _index;
public:
	HListIterator( HHuginn::HList* list_ )
		: _list( list_ ),
		_index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _index < _list->size() );
	}
	virtual void do_next( void ) override {
		++ _index;
	}
private:
	HListIterator( HListIterator const& ) = delete;
	HListIterator& operator = ( HListIterator const& ) = delete;
};

class HMapIterator : public HIteratorInterface {
	HHuginn::HMap::values_t* _map;
	HHuginn::HMap::values_t::iterator _it;
public:
	HMapIterator( HHuginn::HMap::values_t* map_ )
		: _map( map_ ), _it( map_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _map->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HMapIterator( HMapIterator const& ) = delete;
	HMapIterator& operator = ( HMapIterator const& ) = delete;
};

}

class HHuginn::HIterable::HIterator {
public:
	typedef HResource<HIteratorInterface> iterator_implementation_t;
private:
	iterator_implementation_t _impl;
public:
	HIterator( iterator_implementation_t&& impl_ )
		: _impl( yaal::move( impl_ ) ) {
		return;
	}
	value_t value( void ) {
		return ( _impl->value() );
	}
	bool is_valid( void ) {
		return ( _impl->is_valid() );
	}
	void next( void ) {
		_impl->next();
	}
};

HHuginn::HIterable::HIterable( type_t type_ )
	: HValue( type_ ) {
	return;
}

HHuginn::HIterable::HIterator HHuginn::HIterable::iterator( void ) {
	return ( do_iterator() );
}

HHuginn::HString::HString( yaal::hcore::HString const& value_ )
	: HIterable( TYPE::STRING ),
	_value( value_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HString::value( void ) const {
	return ( _value );
}

yaal::hcore::HString& HHuginn::HString::value( void ) {
	return ( _value );
}

HHuginn::HIterable::HIterator HHuginn::HString::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStringIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::HList::HList( void )
	: HIterable( TYPE::LIST ), _data() {
	return;
}

void HHuginn::HList::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

int long HHuginn::HList::size( void ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HHuginn::HList::get( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::value_t HHuginn::HList::get_ref( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( make_pointer<HReference>( _data[static_cast<int>( index_ )] ) );
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HList::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HListIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::HMap::HMap( void )
	: HIterable( TYPE::MAP ),
	_data( &value_builtin::less_low ),
	_keyType( HHuginn::TYPE::NONE ) {
	return;
}

int long HHuginn::HMap::size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HMap::verify_key_type( HHuginn::type_t type_, int position_ ) {
	if ( ( _keyType != TYPE::NONE ) && ( type_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types.", position_ );
	}
	if ( ! HHuginn::OCompiler::is_comparable( type_ ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( type_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

HHuginn::value_t HHuginn::HMap::get( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in map.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

HHuginn::value_t HHuginn::HMap::get_ref( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	_keyType = key_->type();
	return ( make_pointer<HReference>( _data[key_] ) );
	M_EPILOG
}

void HHuginn::HMap::insert( HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->type(), position_ );
	_data.insert( make_pair( key_, value_ ) );
	_keyType = key_->type();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HMap::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HMapIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::HBooleanEvaluator::HBooleanEvaluator( expressions_t const& expressions_, OPERATOR operator_ )
	: HValue( TYPE::BOOLEAN ),
	_expressions( expressions_ ),
	_operator( operator_ ) {
	return;
}

bool HHuginn::HBooleanEvaluator::execute( huginn::HThread* thread_ ) {
	M_PROLOG
	bool all( true );
	HFrame* f( thread_->current_frame() );
	for ( expression_t const& e : _expressions ) {
		e->execute( thread_ );
		if ( ! thread_->can_continue() ) {
			break;
		}
		value_t result( f->result() );
		if ( result->type() != TYPE::BOOLEAN ) {
			throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], e->position() );
		}
		bool v( static_cast<HBoolean*>( result.raw() )->value() );
		if ( ( ( _operator == OPERATOR::BOOLEAN_AND ) && ! v ) || ( ( _operator == OPERATOR::BOOLEAN_OR ) && v ) ) {
			all = false;
			break;
		}
	}
	bool ret( _operator == OPERATOR::BOOLEAN_AND ? all : ! all );
	return ( ret );
	M_EPILOG
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

HHuginn::HScope::HScope( void )
	: _statements() {
	return;
}

void HHuginn::HScope::add_statement( statement_t statement_ ) {
	M_PROLOG
	_statements.emplace_back( statement_ );
	return;
	M_EPILOG
}

void HHuginn::HScope::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_scope_frame();
	for ( HHuginn::statement_t const& s : _statements ) {
		s->execute( thread_ );
		M_ASSERT( thread_->current_frame()->values().is_empty() );
		M_ASSERT( thread_->current_frame()->operations().is_empty() );
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HHuginn::HReturn::HReturn( HHuginn::expression_t const& expression_ )
	: _expression( expression_ ) {
	return;
}

void HHuginn::HReturn::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	if ( !! _expression ) {
		_expression->execute( thread_ );
	}
	thread_->break_execution( huginn::HFrame::STATE::RETURN, thread_->current_frame()->result() );
	return;
	M_EPILOG
}

HHuginn::HBreak::HBreak( void ) {
	return;
}

void HHuginn::HBreak::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->break_execution( huginn::HFrame::STATE::BREAK );
	return;
	M_EPILOG
}

HHuginn::HIf::HIf(
	if_clauses_t const& ifClause_,
	scope_t const& elseClause_
) : HStatement(),
	_ifClauses( ifClause_ ),
	_elseClause( elseClause_ ) {
	return;
}

void HHuginn::HIf::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_scope_frame();
	HFrame* f( thread_->current_frame() );
	bool done( false );
	for ( if_clauses_t::const_iterator it( _ifClauses.begin() ), end( _ifClauses.end() );
		( it != end ) && ! done && thread_->can_continue(); ++ it ) {
		it->expression()->execute( thread_ );
		if ( thread_->can_continue() ) {
			value_t v( f->result() );
			M_ASSERT( v->type() == TYPE::BOOLEAN );
			if ( static_cast<HBoolean*>( v.raw() )->value() ) {
				done = true;
				it->_scope->execute( thread_ );
			}
		} else {
			break;
		}
	}
	if ( ! done && thread_->can_continue() && !! _elseClause ) {
		_elseClause->execute( thread_ );
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HHuginn::HSwitch::HSwitch(
	expression_t const& expression_,
	cases_t const& cases_,
	scope_t const& default_
) : HStatement(),
	_expression( expression_ ),
	_cases( cases_ ),
	_default( default_ ) {
	return;
}

void HHuginn::HSwitch::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame();
	HFrame* f( thread_->current_frame() );
	_expression->execute( thread_ );
	if ( thread_->can_continue() ) {
		value_t v( f->result() );
		if ( ! OCompiler::is_comparable( v->type() ) ) {
			throw HHuginnRuntimeException( "Expression does not evaluate to comparable type.", _expression->position() );
		}
		bool matched( false );
		for ( cases_t::const_iterator it( _cases.begin() ), end( _cases.end() );
			( it != end ) && thread_->can_continue(); ++ it ) {
			if ( ! matched ) {
				it->expression()->execute( thread_ );
			}
			if ( thread_->can_continue() ) {
				if ( ! matched ) {
					if ( v->type() != f->result()->type() ) {
						throw HHuginnRuntimeException( "Case type does not match switch type.", it->expression()->position() );
					}
				}
				if ( matched || value_builtin::equals( v, f->result(), it->expression()->position() ) ) {
					matched = true;
					it->_scope->execute( thread_ );
				}
			} else {
				break;
			}
		}
		if ( thread_->can_continue() && !! _default ) {
			_default->execute( thread_ );
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HHuginn::HWhile::HWhile( expression_t const& condition_, scope_t const& loop_ )
	: HStatement(),
	_condition( condition_ ),
	_loop( loop_ ) {
	return;
}

void HHuginn::HWhile::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame();
	HFrame* f( thread_->current_frame() );
	while ( thread_->can_continue() ) {
		_condition->execute( thread_ );
		if ( thread_->can_continue() ) {
			value_t v( f->result() );
			M_ASSERT( v->type() == TYPE::BOOLEAN );
			if ( static_cast<HBoolean*>( v.raw() )->value() ) {
				_loop->execute( thread_ );
			} else {
				break;
			}
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HHuginn::HFor::HFor( yaal::hcore::HString const& variableName_,
	expression_t const& source_,
	scope_t const& loop_,
	int position_ )
	: HStatement(),
	_variableName( variableName_ ),
	_source( source_ ),
	_loop( loop_ ),
	_position( position_ ) {
	return;
}

void HHuginn::HFor::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame();
	HFrame* f( thread_->current_frame() );
	_source->execute( thread_ );
	if ( thread_->can_continue() ) {
		value_t source( f->result() );
		HIterable* coll( dynamic_cast<HIterable*>( source.raw() ) );
		if ( ! coll ) {
			throw HHuginnRuntimeException( "`For' source is not an iterable.", _source->position() );
		}
		HIterable::HIterator it( coll->iterator() );
		while ( thread_->can_continue() && it.is_valid() ) {
			value_t v( it.value() );
			f->set_variable( _variableName, v, _position );
			_loop->execute( thread_ );
			it.next();
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

HHuginn::HFunction::HFunction( yaal::hcore::HString const& name_,
	parameter_names_t const& parameterNames_,
	HHuginn::scope_t const& scope_,
	expressions_t const& defaultValues_ )
	: _name( name_ ),
	_parameterNames( parameterNames_ ),
	_defaultValues( defaultValues_ ),
	_scope( scope_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunction::execute( huginn::HThread* thread_, values_t const& values_, int position_ ) const {
	M_PROLOG
	if ( values_.get_size() < ( _parameterNames.get_size() - _defaultValues.get_size() ) ) {
		throw HHuginnRuntimeException(
			"Mismatching number of parameters in call to: `"_ys
			.append( _name ).append( "', expected at least: " )
			.append( _parameterNames.get_size() - _defaultValues.get_size() ).append( ", got: " )
			.append( values_.get_size() ).append( "." ),
			position_
		);
	}
	thread_->create_function_frame();
	HFrame* f( thread_->current_frame() );
	for (
		int i( 0 ),
			NAME_COUNT( static_cast<int>( _parameterNames.get_size() ) ),
			VALUE_COUNT( static_cast<int>( values_.get_size() ) ),
			DEFAULT_VALUE_COUNT( static_cast<int>( _defaultValues.get_size() ) );
		i < NAME_COUNT;
		++ i
	) {
		if ( i < VALUE_COUNT ) {
			thread_->current_frame()->set_variable( _parameterNames[i], values_[i], position_ );
		} else {
			int defaultValueIndex( i - ( NAME_COUNT - DEFAULT_VALUE_COUNT ) );
			_defaultValues[defaultValueIndex]->execute( thread_ );
			if ( ! thread_->can_continue() ) {
				break;
			}
			thread_->current_frame()->set_variable( _parameterNames[i], f->result(), position_ );
		}
	}
	if ( thread_->can_continue() ) {
		_scope->execute( thread_ );
	}
	HHuginn::value_t res( thread_->current_frame()->result() );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
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

namespace huginn_builtin {

inline HHuginn::value_t convert( HHuginn::type_t toType_, huginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			""_ys
			.append( toType_->name() )
			.append( "() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
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

inline HHuginn::value_t size( huginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			""_ys
			.append( "size() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::HValue const* v( values_.front().raw() );
	int long long s( 0 );
	HHuginn::type_t typeId( v->type() );
	if ( typeId == HHuginn::TYPE::STRING ) {
		s = static_cast<HHuginn::HString const*>( v )->value().get_length();
	} else if ( typeId == HHuginn::TYPE::LIST ) {
		s = static_cast<HHuginn::HList const*>( v )->size();
	} else if ( typeId == HHuginn::TYPE::MAP ) {
		s = static_cast<HHuginn::HMap const*>( v )->size();
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Getting size of `"_ys.append( v->type()->name() ).append( "'s is not supported." ),
			position_
		);
	}
	return ( make_pointer<HHuginn::HInteger>( s ) );
	M_EPILOG
}

inline HHuginn::value_t type( huginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException( ""_ys
			.append( "type() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::HValue const* v( values_.front().raw() );
	return ( make_pointer<HHuginn::HString>( v->type()->name() ) );
	M_EPILOG
}

inline HHuginn::value_t list( huginn::HThread*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( make_pointer<HHuginn::HList>() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t map( huginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( ! values_.is_empty() ) {
		throw HHuginn::HHuginnRuntimeException( ""_ys
			.append( "map() does not expect any parameters, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
	return ( make_pointer<HHuginn::HMap>() );
	M_EPILOG
}

inline HHuginn::value_t print( huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException( ""_ys
			.append( "print() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
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

inline HHuginn::value_t input( huginn::HThread* thread_, HHuginn::values_t const&, int ) {
	M_PROLOG
	yaal::hcore::HString l;
	thread_->huginn().input_stream().read_until( l );
	return ( make_pointer<HHuginn::HString>( l ) );
	M_EPILOG
}

}

void HHuginn::register_builtins( void ) {
	M_PROLOG
	_functions.insert( make_pair<yaal::hcore::HString const>( "integer", hcore::call( &huginn_builtin::convert, TYPE::INTEGER, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "real", hcore::call( &huginn_builtin::convert, TYPE::REAL, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "string", hcore::call( &huginn_builtin::convert, TYPE::STRING, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "number", hcore::call( &huginn_builtin::convert, TYPE::NUMBER, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "boolean", hcore::call( &huginn_builtin::convert, TYPE::BOOLEAN, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "character", hcore::call( &huginn_builtin::convert, TYPE::CHARACTER, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "size", hcore::call( &huginn_builtin::size, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "type", hcore::call( &huginn_builtin::type, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "list", hcore::call( &huginn_builtin::list, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "map", hcore::call( &huginn_builtin::map, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "print", hcore::call( &huginn_builtin::print, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "input", hcore::call( &huginn_builtin::input, _1, _2, _3 ) ) );
	return;
	M_EPILOG
}

void operands_type_mismatch( char const* op_, HHuginn::type_t t1_, HHuginn::type_t t2_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "' do not match: " )
		.append( t1_->name() )
		.append( " vs " )
		.append( t2_->name() )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, pos_ );
}

}

}


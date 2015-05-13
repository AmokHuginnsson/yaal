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

}

namespace huginn {

bool is_restricted( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) || ( _standardLibrary_.count( name_ ) > 0 ) );
	M_EPILOG
}

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
		constant(
			'(',
			HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -argList >> ')',
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule stringLiteral(
		"stringLiteral",
		string_literal[e_p::HStringLiteral::action_string_position_t( hcore::call( &OCompiler::defer_store_string, _compiler.get(), _1, _2 ) )]
	);
	HRule literalNone(
		"none",
		constant( "none", e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_none, _compiler.get(), _1 ) ) )
	);
	HRule booleanLiteralTrue(
		"true",
		constant( "true", e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_boolean, _compiler.get(), true, _1 ) ) )
	);
	HRule booleanLiteralFalse(
		"false",
		constant( "false", e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_boolean, _compiler.get(), false, _1 ) ) )
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
	HRule mapLiteralElement(
		"mapLiteralElement",
		arg >> ':' >>	arg
	);
	HRule mapLiteral(
		"mapLiteral",
		constant(
			'{',
			HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -( mapLiteralElement >> *( ',' >> mapLiteralElement ) ) >> '}',
		HRuleBase::action_position_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_MAP, _1 ) )
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
	HRule dereference( "dereference", ( reference >> *( subscriptOperator | functionCallOperator ) ) );
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_position_t( hcore::call( &OCompiler::defer_store_real, _compiler.get(), _1, _2 ) )]
		| numberLiteral
		| integer[e_p::HInteger::action_int_long_long_position_t( hcore::call( &OCompiler::defer_store_integer, _compiler.get(), _1, _2 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_position_t( hcore::call( &OCompiler::defer_store_character, _compiler.get(), _1, _2 ) )]
		| ( listLiteral >> -( subscriptOperator >> *( subscriptOperator | functionCallOperator ) ) )
		| ( mapLiteral >> -( subscriptOperator >> *( subscriptOperator | functionCallOperator ) ) )
		| literalNone | booleanLiteralTrue | booleanLiteralFalse
		| dereference
		| ( stringLiteral >> -subscriptOperator )
		| ( lambda >> -( functionCallOperator >> *( subscriptOperator | functionCallOperator ) ) )
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
	HRule subscript( "subscript", ( reference >> +( subscriptOperator | functionCallOperator ) ) );
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
	HRule ifClause(
		"ifClause",
		e_p::constant( "if" ) >> '(' >> expression >> ')' >> scope,
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule ifStatement(
		"ifStatement",
		ifClause >>
		*( constant( "else" ) >> ifClause ) >>
		-( ( constant( "else" ) >> scope )[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )] )
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
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_for_identifier, _compiler.get(), _1, _2 ) )
		)
	);
	HRule forStatement( "forStatement", constant( "for" ) >> '(' >> forIdentifier >> ':' >> expression >> ')' >> scope );
	HRule caseStatement(
		"caseStatement",
		constant( "case" ) >> '(' >> expression >> ')' >> ':' >> scope >> -( breakStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )] ),
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule defaultStatement( "defaultStatement", constant( "default" ) >> ':' >> scope );
	HRule switchStatement(
		"switchStatement",
		constant( "switch" ) >> '(' >> expression >> ')' >>
		constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> +caseStatement >>
		-( defaultStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )] ) >> '}'
	);
	HRule returnStatement( "returnStatement", constant( "return" ) >> -( '(' >> expression >> ')' ) >> ';' );
	HRule statement( "statement",
		ifStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_if_statement, _compiler.get(), _1 ) )]
		| whileStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_while_statement, _compiler.get(), _1 ) )]
		| forStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_for_statement, _compiler.get(), _1 ) )]
		| switchStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_switch_statement, _compiler.get(), _1 ) )]
		| breakStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )]
		| continueStatement
		| returnStatement[HRuleBase::action_position_t( hcore::call( &OCompiler::add_return_statement, _compiler.get(), _1 ) )]
		| expressionStatement
		| scope[HRuleBase::action_position_t( hcore::call( &OCompiler::commit_scope, _compiler.get(), _1 ) )]
	);
	scope %= ( constant( '{', HRuleBase::action_position_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> *statement >> '}' );
	HRule functionDefinition( "functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_function_name, _compiler.get(), _1, _2 ) )
		) >> '(' >> -nameList >>
		constant( ')', HRuleBase::action_position_t( hcore::call( &OCompiler::verify_default_argument, _compiler.get(), _1 ) ) )
		>> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::create_function, this, _1 ) )
	);
	HRule field( "field", regex( "fieldIdentifier", identifier ) >> '=' >> expression >> ';' );
	HRule classDefinition(
		"classDefinition",
		constant( "class" ) >> regex(
			"classIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_class_name, _compiler.get(), _1, _2 ) )
		) >> -( ':' >> regex( "baseIdentifier", identifier ) )
		>> '{' >> +( field | functionDefinition ) >> '}'
	);
	HRule huginnGrammar( "huginnGrammar", + ( classDefinition | functionDefinition ) );
	return ( huginnGrammar );
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
	, _source( make_resource<HSource>() )
	, _compiler( make_resource<OCompiler>( this ) )
	, _engine( make_engine(), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR )
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
	OCompiler::OFunctionContext& fc( _compiler->f() );
	_functions.insert( make_pair<hcore::HString const, function_t>( yaal::move( fc._functionName ), _compiler->create_function( position_ ) ) );
	_compiler->_functionContexts.pop();
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
	if ( ! OCompiler::is_comparable( type_ ) ) {
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

}

}


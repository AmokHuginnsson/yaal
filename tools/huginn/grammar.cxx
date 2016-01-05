/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  grammar.cxx - this file is integral part of `yaal' project.

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
#include "tools/hhuginn.hxx"
#include "keyword.hxx"
#include "compiler.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

executing_parser::HRule HHuginn::make_engine( void ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	hcore::HString identifierPattern( YAAL_REGEX_WORD_START "[a-zA-Z_][a-zA-Z0-9_]*" YAAL_REGEX_WORD_END );
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
		constant( KEYWORD::NONE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _none, _1 ) ) )
	);
	HRule booleanLiteralTrue(
		"true",
		constant( KEYWORD::TRUE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _true, _1 ) ) )
	);
	HRule booleanLiteralFalse(
		"false",
		constant( KEYWORD::FALSE, e_p::HRuleBase::action_position_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), _false, _1 ) ) )
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
			identifierPattern,
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
			identifierPattern,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &OCompiler::defer_get_reference, _compiler.get(), _1, _2 ) )
		)
	);
	HRule memberAccess(
		"memberAccess",
		constant( '.' )[HRuleBase::action_position_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::MEMBER_ACCESS, _1 ) )] >> regex(
			"member",
			identifierPattern,
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
					identifierPattern,
					e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_make_variable, _compiler.get(), _1, _2 ) )
				)
			) >>
			( constant( "=" ) | "+=" | "-=" | "*=" | "/=" | "%=" | "^=" )[
				e_p::HString::action_string_position_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )
			] ^ '='
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
			identifierPattern,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_type_name, _compiler.get(), _1, _2 ) )
		) >>
		regex(
			"exceptionVariable",
			identifierPattern,
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
			identifierPattern,
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
			identifierPattern,
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
			identifierPattern,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_field_name, _compiler.get(), _1, _2 ) )
		) >> '=' >> HRule( expression, HRuleBase::action_position_t( hcore::call( &OCompiler::add_field_definition, _compiler.get(), _1 ) ) ) >> ';'
	);
	HRule classDefinition(
		"classDefinition",
		constant( KEYWORD::CLASS ) >> regex(
			"classIdentifier",
			identifierPattern,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_class_name, _compiler.get(), _1, _2 ) )
		) >> -(
			':' >> regex(
				"baseIdentifier",
				identifierPattern,
				e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_base_name, _compiler.get(), _1, _2 ) )
			)
		) >> '{' >> +( field | functionDefinition ) >> '}',
		HRuleBase::action_position_t( hcore::call( &OCompiler::submit_class, _compiler.get(), _1 ) )
	);
	HRule importStatement(
		"importStatement",
		constant( "import" ) >> regex(
			"packageName",
			identifierPattern,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_import_name, _compiler.get(), _1, _2 ) )
		) >> "as" >> regex(
			"importName",
			identifierPattern,
			e_p::HRegex::action_string_position_t( hcore::call( &OCompiler::set_import_alias, _compiler.get(), _1, _2 ) )
		) >> ';',
		HRuleBase::action_position_t( hcore::call( &OCompiler::commit_import, _compiler.get(), _1 ) )
	);
	HRule huginnGrammar( "huginnGrammar", + ( classDefinition | functionDefinition | importStatement ) );
	return ( huginnGrammar );
	M_EPILOG
}

}

}


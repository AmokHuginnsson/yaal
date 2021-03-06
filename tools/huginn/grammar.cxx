/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "keyword.hxx"
#include "compiler.hxx"
#include "tools/hidentifierlexer.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

class HNumberParser : public executing_parser::HReal {
public:
	typedef HNumberParser this_type;
	typedef HReal base_type;
public:
	HNumberParser( action_number_range_t const& action_ )
		: HReal( action_ ) {
		return;
	}
	HNumberParser( HNumberParser const& numberParser_ )
		: HReal( numberParser_ ) {
		return;
	}
protected:
	virtual ptr_t do_clone( void ) const override {
		M_PROLOG
		return ( make_pointer<HNumberParser>( *this ) );
		M_EPILOG
	}
	virtual yaal::hcore::HUTF8String::const_iterator do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const override {
		M_PROLOG
		if ( first_ == last_ ) {
			return first_;
		}
		if ( *first_ != '$' ) {
			report_error( executingParser_, first_, "expected a number" );
			return first_;
		}
		hcore::HUTF8String::const_iterator scan( first_ );
		++ scan;
		hcore::HUTF8String::const_iterator scanned( HReal::do_parse( executingParser_, scan, last_ ) );
		return ( scanned != scan ? scanned : first_ );
		M_EPILOG
	}
	virtual void do_describe( executing_parser::HRuleDescription& rd_, executing_parser::rule_use_t const& ) const override {
		M_PROLOG
		rd_.desc( "number" );
		return;
		M_EPILOG
	}
private:
	HNumberParser& operator = ( HNumberParser const& ) = delete;
};

executing_parser::HRule HHuginn::make_engine( HRuntime* runtime_, compiler_setup_t compilerSetup_ ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	HRule expression( "expression", e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::mark_expression_position, _compiler.get(), _1 ) ) );
	HRule modulus( "modulus",
		constant( '|', e_p::HCharacter::action_character_range_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) )
		>> expression
		>> constant( '|', e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MODULUS, _1 ) ) ) );
	HRule parenthesis( "parenthesis",
		constant( '(', e_p::HCharacter::action_character_range_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) )
		>> expression
		>> constant( ')', e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::PARENTHESIS, _1 ) ) ) );
	HRule arg(
		"argument",
		expression,
		HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_ARGUMENT, _1 ) )
	);
	HRule unbound(
		"unbound",
		constant( '~', e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::note_function_argument, _compiler.get(), OCompiler::IMPLICIT_UNBOUND_INDEX + 0, _1 ) ) )
		>> -integer[e_p::HInteger::action_int_range_t( hcore::call( &OCompiler::index_unbound, _compiler.get(), _1, _2 ) )]
	);
	/*
	 * b = ( 1, 2, 3 );
	 * g = foo( a, ~, b..., c );
	 * h = foo( a, b..., ~, c );
	 * g( 7 ) ~~~ foo( a, 7, 1, 2, 3, c )
	 * h( 7 ) ~~~ foo( a, 1, 2, 3, 7, c )
	 */
	HRule functionArgument(
		"functionArgument", (
			( arg ^ ':' ) >> -constant( "..." )[HRuleBase::action_range_t( hcore::call( &OCompiler::unpack_variadic_parameters, _compiler.get(), _1 ) )]
		)[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::note_function_argument, _compiler.get(), OCompiler::BOUND_INDEX + 0, _1 ) )] | unbound
	);
	HRule unpackedNamedParameters(
		"unpackedNamedParameters",
		arg >> constant( ":::" )[HRuleBase::action_range_t( hcore::call( &OCompiler::repack_named_parameters, _compiler.get(), _1 ) )]
	);
	HRule argList( "argList", functionArgument >> *( ',' >> functionArgument ) );
	HRule parameterName(
		identifier(
			"parameterName",
			e_p::HStringLiteral::action_string_range_t( hcore::call( &OCompiler::defer_store_string, _compiler.get(), _1, _2 ) )
		),
		HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_ARGUMENT, _1 ) )
	);
	HRule namedParameter(
		"namedParameter",
		parameterName >> ':' >> functionArgument
	);
	HRule namedParameters(
		"namedParameters",
		HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		>> namedParameter >> *( ',' >> namedParameter )
		>> HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_LOOKUP, _1 ) )
		>> HRuleBase::action_range_t( hcore::call( &OCompiler::repack_named_parameters, _compiler.get(), _1 ) ),
		HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_ARGUMENT, _1 ) )
	);
	HRule repackedNamedParameter(
		"repackedNamedParameter",
		namedParameters | unpackedNamedParameters
	);
	HRule repackedNamedParameters(
		"repackedNamedParameters",
		repackedNamedParameter >> *( ',' >> repackedNamedParameter )
	);
	HRule functionCallOperator(
		"functionCallOperator",
		constant( '(', HRuleBase::action_range_t( hcore::call( &OCompiler::start_function_call, _compiler.get(), _1 ) ) )
		>> -( ( argList >> -( ',' >> repackedNamedParameters ) ) | repackedNamedParameters ) >>
		constant( ')', HRuleBase::action_range_t( hcore::call( &OCompiler::close_function_call, _compiler.get(), _1 ) ) ),
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_PARTIAL, _1 ) )
	);
	HRule stringLiteral(
		"stringLiteral",
		string_literal[e_p::HStringLiteral::action_string_range_t( hcore::call( &OCompiler::defer_store_string, _compiler.get(), _1, _2 ) )]
	);
	HRule literalNone(
		"none",
		constant( KEYWORD::NONE, e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), runtime_ ? runtime_->none_value() : value_t(), _1 ) ) )
	);
	HRule booleanLiteralTrue(
		"true",
		constant( KEYWORD::TRUE, e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), runtime_ ? runtime_->true_value() : value_t(), _1 ) ) )
	);
	HRule booleanLiteralFalse(
		"false",
		constant( KEYWORD::FALSE, e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::defer_store_direct, _compiler.get(), runtime_ ? runtime_->false_value() : value_t(), _1 ) ) )
	);
	HRule tupleLiteral(
		"tupleLiteral",
		constant(
			'(',
			HRuleBase::action_range_t( hcore::call( &OCompiler::defer_call, _compiler.get(), "tuple", _1 ) )
		) >> -argList >> -constant( ',' ) >> ')',
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_PARTIAL, _1 ) )
	);
	HRule listLiteral(
		"listLiteral",
		constant(
			'[',
			HRuleBase::action_range_t( hcore::call( &OCompiler::defer_call, _compiler.get(), "list", _1 ) )
		) >> -argList >> ']',
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_PARTIAL, _1 ) )
	);
	HRule dictLiteralElement(
		"dictLiteralElement",
		arg >> ':' >> arg
	);
	HRule dictLiteral(
		"dictLiteral",
		constant(
			'[',
			HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -( dictLiteralElement >> *( ',' >> dictLiteralElement ) ) >> ']',
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_DICT, _1 ) )
	);
	HRule lookupLiteral(
		"lookupLiteral",
		constant(
			'{',
			HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::FUNCTION_CALL, _1 ) )
		) >> -( dictLiteralElement >> *( ',' >> dictLiteralElement ) ) >> '}',
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_LOOKUP, _1 ) )
	);
	HRule setLiteral(
		"setLiteral",
		constant(
			'{',
			HRuleBase::action_range_t( hcore::call( &OCompiler::defer_call, _compiler.get(), "set", _1 ) )
		) >> arg >> *( ',' >> arg ) >> '}',
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MAKE_PARTIAL, _1 ) )
	);
	HIdentifierLexer parameterIdentifier(
		identifier(
			"parameterIdentifier",
			HRegex::action_string_range_t( hcore::call( &OCompiler::add_parameter, _compiler.get(), _1, _2 ) )
		)
	);
	HRule val( "value" );
	HRule parameter(
		"parameter",
		( parameterIdentifier ^ ( constant( { "...", ":::" } ) ) )
		>> -( constant( '=' ) >> HRule( val, HRuleBase::action_range_t( hcore::call( &OCompiler::add_default_value, _compiler.get(), _1 ) ) ) ),
		HRuleBase::action_range_t( hcore::call( &OCompiler::verify_default_argument, _compiler.get(), _1 ) )
	);
	HRule variadicParameter(
		"variadicParameter",
		parameterIdentifier >> "...",
		HRuleBase::action_t( hcore::call( &OCompiler::mark_variadic, _compiler.get() ) )
	);
	HRule namedParameterCapture(
		"namedParameterCapture",
		parameterIdentifier >> ":::",
		HRuleBase::action_t( hcore::call( &OCompiler::mark_named_parameter_capture, _compiler.get() ) )
	);
	HRule nameList(
		"nameList",
		parameter >> ( * ( ',' >> parameter ) )
	);
	HRule statement( "statement" );
	HRule callable(
		"callable",
		constant( '(' ) >> -( ( nameList >> -( ',' >> variadicParameter ) >> -( ',' >> namedParameterCapture ) ) | ( variadicParameter >> -( ',' >> namedParameterCapture ) ) | namedParameterCapture ) >> ')'
		>> '{' >> *statement >> '}'
	);
	/*
	 * There are two kinds of lambdas in Huginn language:
	 * 1. Pure lambda functions (called Lambda(s) for short)
	 * 2. Referentially opaque lambda functions (called Closure(s) for short).
	 *
	 * Implementation of Lambdas is trivial, lambda body is turned into ordinary
	 * function during compilation time and stored as single value in run-time.
	 *
	 * Closures require different approach, i.e.: all "captures" must be used
	 * twice, once in call/use context, and once in lambda body context.
	 * Hence each Closure constitutes a separate class which instance is created
	 * on Closure definition site.
	 */
	HRule capture(
		"capture",
		identifier(
			"captureIdentifier",
			HRegex::action_string_range_t( hcore::call( &OCompiler::add_capture, _compiler.get(), _1, _2 ) )
		) >> -( ':' >> expression ),
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_capture, _compiler.get(), _1 ) )
	);
	HRule captureList(
		"captureList",
		capture >> ( * ( ',' >> capture ) )
	);
	HRule lambda(
		"lambda",
		( '@' >> -( '[' >> captureList >> ']' ) )[
			HIdentifierLexer::action_range_t( hcore::call( &OCompiler::set_lambda_name, _compiler.get(), _1 ) )
		] >> callable,
		HRuleBase::action_range_t( hcore::call( &OCompiler::create_lambda, _compiler.get(), _1 ) )
	);
	HRule rangeOper(
		"rangeOper",
		constant(
			':',
			e_p::HCharacter::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::SUBSCRIPT_ARGUMENT, _1 ) )
		)
	);
	HRule subscriptOperator(
		"subscriptOperator",
		constant(
			'[',
			e_p::HCharacter::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::SUBSCRIPT, _1 ) )
		) >> ( ( ( rangeOper >> -arg ) | ( arg >> -( rangeOper >> -arg ) ) ) >> -( rangeOper >> -arg ) ) >> ']',
		e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::SUBSCRIPT, _1 ) )
	);
	HIdentifierLexer reference(
		identifier(
			"reference",
			e_p::HStringLiteral::action_string_range_t( hcore::call( &OCompiler::defer_get_reference, _compiler.get(), _1, _2 ) )
		)
	);
	HRule memberAccess(
		"memberAccess",
		constant( '.', HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::MEMBER_ACCESS, _1 ) ) ) >> identifier(
			"member",
			e_p::HStringLiteral::action_string_range_t( hcore::call( &OCompiler::defer_get_field_reference, _compiler.get(), _1, _2 ) )
		),
		e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MEMBER_ACCESS, _1 ) )
	);
	HRule dereference( "dereference", *( subscriptOperator | functionCallOperator | memberAccess ) );
	HNumberParser number( e_p::HReal::action_number_range_t( hcore::call( &OCompiler::defer_store_number, _compiler.get(), _1, _2 ) ) );
	HRule atom( "atom",
		modulus
		| parenthesis >> -( ( subscriptOperator | memberAccess ) >> dereference )
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_range_t( hcore::call( &OCompiler::defer_store_real, _compiler.get(), _1, _2 ) )]
		| integer[e_p::HInteger::action_int_long_long_range_t( hcore::call( &OCompiler::defer_store_integer, _compiler.get(), _1, _2 ) )]
		| (
			( number
				| character_literal[e_p::HCharacterLiteral::action_character_range_t( hcore::call( &OCompiler::defer_store_character, _compiler.get(), _1, _2 ) )] )
			>> -( memberAccess >> functionCallOperator )
		)
		| ( ( tupleLiteral | listLiteral | dictLiteral | lookupLiteral | stringLiteral ) >> -( ( subscriptOperator | memberAccess ) >> dereference ) )
		| ( setLiteral >> -( memberAccess >> dereference ) )
		| literalNone | booleanLiteralTrue | booleanLiteralFalse
		| ( reference >> dereference )
		| ( lambda >> -( functionCallOperator >> dereference ) )
	);
	HRule factorial(
		"factorial",
		atom >> -( ( ( constant( '!' ) & "==" ) | ( constant( '!' ) ^ '=' ) )[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::FACTORIAL, _1 ) )] )
	);
	HRule negation( "negation" );
	HRule power(
		"power",
		factorial >> (
			* ( constant( '^', e_p::HCharacter::action_character_range_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) ) ) >> negation )
		),
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::POWER, _1 ) )
	);
	negation %= (
		( constant( '-', HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::NEGATE, _1 ) ) ) >> negation )[
			e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::NEGATE, _1 ) )
		] | power
	);
	HRule multiplication(
		"multiplication",
		negation >> (
			* (
					(
					characters(
						"*/%", e_p::HCharacter::action_character_range_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) )
					) >> negation
				)[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::MULTIPLY, _1 ) )]
			)
		)
	);
	HRule sum(
		"sum",
		multiplication >> (
			* (
					(
					characters(
						"+-", e_p::HCharacter::action_character_range_t( hcore::call( &OCompiler::defer_oper, _compiler.get(), _1, _2 ) )
					) >> multiplication
				)[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::PLUS, _1 ) )]
			)
		)
	);
	HRule compare(
		"compare",
		sum >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( { "<=", ">=", "<", ">", "≤", "≥", "∈", "∉" } )
				)[e_p::HString::action_string_range_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> sum
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::LESS, _1 ) )]
		)
	);
	HRule equality(
		"equality",
		compare >> -(
			/* compare action */ (
				/* comparator operator */ (
					constant( { "==", "!=", "≠" } )
				)[e_p::HString::action_string_range_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> compare
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::EQUALS, _1 ) )]
		)
	);
	HRule booleanNot(
		"booleanNot", (
			characters( "!¬", HRuleBase::action_range_t( hcore::call( &OCompiler::defer_oper_direct, _compiler.get(), OPERATOR::BOOLEAN_NOT, _1 ) ) )
			>> equality
		)[
			e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_NOT, _1 ) )
		] | equality
	);
	/*
	 * ( a == b ) && ( c == d ) || ( e == f ) && ( g == h )
	 */
	HRule booleanAnd(
		"booleanAnd",
		booleanNot[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) )] >> *(
			/* boolean action */ (
				/* boolean operator */ (
					constant( { "&&", "⋀" } )
				)[e_p::HString::action_range_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )]
				>> booleanNot
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )]
		),
		e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::commit_boolean, _compiler.get(), OPERATOR::BOOLEAN_AND, _1 ) )
	);
	HRule booleanOr(
		"booleanOr",
		HRule( booleanAnd, e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) ) ) >> *(
			/* boolean action */ (
				/* boolean operator */ (
					constant( { "||", "⋁" } )
				)[e_p::HString::action_range_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )]
				>> booleanAnd
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )]
		),
		e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::commit_boolean, _compiler.get(), OPERATOR::BOOLEAN_OR, _1 ) )
	);
	HRule booleanXor(
		"booleanXor",
		booleanOr >> -(
			/* boolean action */ (
				/* boolean operator */ (
					constant( { "^^", "⊕" } )
				)[e_p::HString::action_string_range_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )]
				>> booleanOr
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::BOOLEAN_XOR, _1 ) )]
		)
	);
	HRule ternary(
		"ternary",
		HRule( booleanXor, e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::start_subexpression, _compiler.get(), _1 ) ) ) >> -(
			/* ternary action */ (
				constant( '?', e_p::HString::action_range_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::TERNARY, _1 ) ) )
				>> expression
				>> constant( ':', e_p::HString::action_range_t( hcore::call( &OCompiler::add_subexpression, _compiler.get(), OPERATOR::TERNARY, _1 ) ) )
				>> expression
			)[HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::TERNARY, _1 ) )]
		),
		e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::commit_ternary, _compiler.get(), _1 ) )
	);
	val %= ternary;
	HRule subscript( "subscript", ( reference >> +( subscriptOperator | functionCallOperator | memberAccess ) ) );
	HRule assignable(
		"assignable",
		subscript[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::make_reference, _compiler.get(), _1 ) )]
		| identifier(
			"variableSetter",
			e_p::HString::action_string_range_t( hcore::call( &OCompiler::defer_make_variable, _compiler.get(), _1, _2 ) )
		)
	);
	HRule assignablePack(
		"assignablePack",
		HRuleBase::action_range_t( hcore::call( &OCompiler::start_assignable, _compiler.get(), _1 ) )
		>> assignable >> * ( ',' >> assignable ) >>
		HRuleBase::action_range_t( hcore::call( &OCompiler::finish_assignable, _compiler.get(), _1 ) )
	);
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	expression %= HRule(
		* ( assignablePack >> constant( { "=", "+=", "-=", "*=", "/=", "%=", "^=" } )[
				e_p::HString::action_string_range_t( hcore::call( &OCompiler::defer_str_oper, _compiler.get(), _1, _2 ) )
			] ^ '='
		) >> val,
		HRuleBase::action_range_t( hcore::call( &OCompiler::dispatch_action, _compiler.get(), OPERATOR::ASSIGN, _1 ) )
	);
	HRule expressionStatement(
		"expressionStatement",
		HRule( expression, HRuleBase::action_range_t( hcore::call( &OCompiler::commit_expression, _compiler.get(), _1 ) ) ) >> ';'
	);
	HRule scope( "scope", constant( '{', HRuleBase::action_range_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> *statement >> '}' );
	HRule catchStatement(
		"catchStatement",
		constant( KEYWORD::CATCH ) >> '(' >>
		identifier(
			"exceptionType",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::start_catch_statement, _compiler.get(), _1, _2 ) )
		) >> assignable[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::commit_catch_control_variable, _compiler.get(), _1 ) )] >> ')' >>
		scope[HRuleBase::action_range_t( hcore::call( &OCompiler::commit_catch, _compiler.get(), _1 ) )]
	);
	HRule tryCatchStatement(
		"tryCatchStatement",
		constant( KEYWORD::TRY ) >> scope[HRuleBase::action_range_t( hcore::call( &OCompiler::commit_try, _compiler.get(), _1 ) )] >> +catchStatement
	);
	HRule ifClause(
		"ifClause",
		e_p::constant(
			KEYWORD::IF,
			bound_call<e_p::HRuleBase::action_range_t>( &OCompiler::start_if_statement, _compiler.get(), _1 )
		) >> '(' >> expression >> ')' >> scope,
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule ifStatement(
		"ifStatement",
		ifClause >>
		*( constant( KEYWORD::ELSE ) >> ifClause ) >>
		-(
			/* `else' clause */ (
				constant( KEYWORD::ELSE ) >> scope
			)[
				HRuleBase::action_range_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )
			]
		)
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
			bound_call<e_p::HRuleBase::action_range_t>( &OCompiler::start_while_statement, _compiler.get(), _1 )
		) >> '(' >> expression >> ')' >> scope
	);
	HRule forCtrlVar(
		"forCtrlVar",
		assignable[e_p::HRuleBase::action_range_t( hcore::call( &OCompiler::save_control_variable, _compiler.get(), _1 ) )]
	);
	HRule forStatement(
		"forStatement",
		constant(
			KEYWORD::FOR,
			bound_call<e_p::HRuleBase::action_range_t>( &OCompiler::start_for_statement, _compiler.get(), _1 )
		) >> '(' >> forCtrlVar >> * ( ',' >> forCtrlVar ) >> ':' >> expression >> ')' >> scope
	);
	HRule caseStatement(
		"caseStatement",
		constant( KEYWORD::CASE ) >> '(' >> expression >> ')' >> ':'
		>> scope >> -( breakStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )] ),
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_if_clause, _compiler.get(), _1 ) )
	);
	HRule defaultStatement( "defaultStatement", constant( KEYWORD::DEFAULT ) >> ':' >> scope );
	HRule switchStatement(
		"switchStatement",
		constant(
			KEYWORD::SWITCH,
			bound_call<e_p::HRuleBase::action_range_t>( &OCompiler::start_switch_statement, _compiler.get(), _1 )
		) >> '(' >> expression >> ')' >>
		constant( '{', HRuleBase::action_range_t( hcore::call( &OCompiler::create_scope, _compiler.get(), _1 ) ) ) >> +caseStatement >>
		-( defaultStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::commit_else_clause, _compiler.get(), _1 ) )] ) >> '}'
	);
	HRule returnStatement( "returnStatement", constant( KEYWORD::RETURN ) >> -( '(' >> expression >> ')' ) >> ';' );
	HSkip assertStatement( skip( constant( KEYWORD::ASSERT ) >> functionCallOperator >> ';' ) );
	statement %= (
		ifStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_if_statement, _compiler.get(), _1 ) )]
		| whileStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_while_statement, _compiler.get(), _1 ) )]
		| forStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_for_statement, _compiler.get(), _1 ) )]
		| switchStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_switch_statement, _compiler.get(), _1 ) )]
		| tryCatchStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_try_catch_statement, _compiler.get(), _1 ) )]
		| throwStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_throw_statement, _compiler.get(), _1 ) )]
		| breakStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_break_statement, _compiler.get(), _1 ) )]
		| continueStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_continue_statement, _compiler.get(), _1 ) )]
		| returnStatement[HRuleBase::action_range_t( hcore::call( &OCompiler::add_return_statement, _compiler.get(), _1 ) )]
		| ( ( compilerSetup_ & COMPILER::OPTIMIZE ) ? &assertStatement : nullptr )
		| expressionStatement
		| scope[HRuleBase::action_range_t( hcore::call( &OCompiler::commit_scope, _compiler.get(), _1 ) )]
	);
	HRule functionDefinition(
		"functionDefinition",
		identifier(
			"functionDefinitionIdentifier",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_function_name, _compiler.get(), _1, _2 ) )
		) >> callable,
		HRuleBase::action_range_t( hcore::call( &OCompiler::create_function, _compiler.get(), _1 ) )
	);
	HRule field(
		"field",
		identifier(
			"fieldIdentifier",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_field_name, _compiler.get(), _1, _2 ) )
		) >> '=' >> HRule( expression, HRuleBase::action_range_t( hcore::call( &OCompiler::add_field_definition, _compiler.get(), _1 ) ) ) >> ';'
	);
	HRule classDefinition(
		"classDefinition",
		constant( KEYWORD::CLASS ) >> identifier(
			"classIdentifier",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::HMultiPassDispatcher::set_class_name, &(_compiler->_classNoter), _1, _2 ) )
		) >> -(
			':' >> identifier(
				"baseIdentifier",
				HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_base_name, _compiler.get(), _1, _2 ) )
			)
		) >> '{' >> +( field | functionDefinition ) >> '}',
		HRuleBase::action_range_t( hcore::call( &OCompiler::submit_class, _compiler.get(), _1 ) )
	);
	HRule enumeral(
		"enumeral",
		identifier(
			"fieldIdentifier",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_field_name, _compiler.get(), _1, _2 ) )
		)
	);
	HRule enumDefinition(
		"enumDefinition",
		constant( KEYWORD::ENUM ) >> identifier(
			"enumIdentifier",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_enum_name, _compiler.get(), _1, _2 ) )
		) >> '{' >> enumeral >> *( ',' >> enumeral ) >> '}',
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_enum, _compiler.get(), _1 ) )
	);
	HIdentifierLexer moduleName(
		identifier(
			"moduleName",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::build_import_name, _compiler.get(), _1, _2 ) )
		)
	);
	HRule packageName(
		"packageName",
		moduleName >> *( '.' >> moduleName ),
		HRule::action_range_t( hcore::call( &OCompiler::set_import_name, _compiler.get(), _1 ) )
	);
	HRule importStatement(
		"importStatement",
		constant( "import" ) >> packageName >> "as" >> identifier(
			"importName",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::set_import_alias, _compiler.get(), _1, _2 ) )
		) >> ';',
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_import, _compiler.get(), _1 ) )
	);
	HIdentifierLexer importedSymbol(
		identifier(
			"importedSymbol",
			HIdentifierLexer::action_string_range_t( hcore::call( &OCompiler::add_imported_symbol, _compiler.get(), _1, _2 ) )
		)
	);
	HRule fromStatement(
		"fromStatement",
		constant( "from" ) >> packageName >> "import" >> (
			( importedSymbol >> *( ',' >> importedSymbol ) )
			| constant( "*", e_p::HString::action_string_range_t( hcore::call( &OCompiler::add_imported_symbol, _compiler.get(), _1, _2 ) ) )
		) >> ';',
		HRuleBase::action_range_t( hcore::call( &OCompiler::commit_import, _compiler.get(), _1 ) )
	);
	HRule huginnGrammar( "huginnGrammar", + ( classDefinition | functionDefinition | enumDefinition | importStatement | fromStatement ) );
	return huginnGrammar;
	M_EPILOG
}

}

}


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
#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"
#include "streamtools.hxx"
#include "hcore/hfile.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {
static char const NEWLINE = '\n';
}

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

HHuginn::value_t _none_ = make_pointer<HHuginn::HValue>();

namespace ERR_CODE {
enum {
	OP_TYPES_NOT_MATCH = 0,
	OP_NOT_SUM,
	OP_NOT_SUB,
	OP_NOT_MUL,
	OP_NOT_DIV,
	OP_NOT_EXP,
	OP_NOT_CMP,
	OPS_NOT_BOOL,
	OP_NOT_BOOL,
	IDX_NOT_INT
};
}

char const* _errMsgHHuginn_[ 10 ] = {
	_( "Operand types do not match." ),
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

executing_parser::HRule HHuginn::make_engine( void ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	hcore::HString identifier( "\\<[a-zA-Z_][a-zA-Z0-9_]*\\>" );
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
	HRule functionCall(
		"functionCall",
		regex(
			"functionCallIdentifier",
			identifier,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_function_call, &_compiler, _1, _2 ) )
		) >> '(' >> -argList >> ')',
		HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::FUNCTION_CALL, _1 ) )
	);
	HRule variableGetter(
		regex(
			"variableGetter",
			identifier,
			e_p::HStringLiteral::action_string_position_t( hcore::call( &HHuginn::OCompiler::defer_get_variable, &_compiler, _1, _2 ) )
		)
	);
	HRule subscript( "subscript" );
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
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_position_t( hcore::call( &HHuginn::OCompiler::defer_store_real, &_compiler, _1, _2 ) )]
		| numberLiteral
		| integer[e_p::HInteger::action_int_long_long_position_t( hcore::call( &HHuginn::OCompiler::defer_store_integer, &_compiler, _1, _2 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_store_character, &_compiler, _1, _2 ) )]
		| subscript[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::dereference, _1 ) )]
		| stringLiteral
		| functionCall
		| literalNone | booleanLiteralTrue | booleanLiteralFalse
		| variableGetter
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
	HRule value( "value", booleanXor );
	subscript %= (
		/* subscript base */ ( functionCall | variableGetter | stringLiteral ) >>
		/* repeat at least once */ + (
			/* subscript operator it self */ (
				constant(
					'[',
					e_p::HCharacter::action_character_position_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1, _2 ) )
				) >> expression >> ']'
			)[e_p::HRuleBase::action_position_t( hcore::call( &HHuginn::OCompiler::dispatch_action, &_compiler, OPERATOR::SUBSCRIPT, _1 ) )]
		)
	);
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	expression %= HRule(
		* (
			/* make reference */ (
				subscript
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
	HRule scope( "scope" );
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
	HRule parameter(
		regex(
			"parameter",
			identifier,
			HRegex::action_string_position_t( hcore::call( &HHuginn::OCompiler::add_paramater, &_compiler, _1, _2 ) )
		)
	);
	HRule nameList( "nameList", parameter >> ( * ( ',' >> parameter ) ) );
	HRule functionDefinition( "functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_position_t( hcore::call( &HHuginn::OCompiler::set_function_name, &_compiler, _1, _2 ) )
		) >> '(' >> -nameList >> ')' >> scope,
		HRuleBase::action_position_t( hcore::call( &HHuginn::create_function, this, _1 ) )
	);
	HRule literal(
		"literal",
		real
		| numberLiteral
		| integer
		| character_literal
		| stringLiteral
		| literalNone
		| booleanLiteralTrue
		| booleanLiteralFalse
	);
	HRule field( "field", regex( "fieldIdentifier", identifier ) >> '=' >> literal >> ';' );
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

class HPrepocessor {
public:
	typedef HPrepocessor this_type;
	class HIterator;
private:
	yaal::hcore::HString::const_iterator _beg;
	yaal::hcore::HString::const_iterator _end;
public:
	HPrepocessor( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ )
		: _beg( first_ ), _end( last_ ) {
		return;
	}
	HIterator begin( void ) const;
	HIterator end( void ) const;
};

class HPrepocessor::HIterator : public yaal::hcore::iterator_interface<char const, iterator_category::forward> {
public:
	typedef HIterator this_type;
	typedef yaal::hcore::iterator_interface<char const, iterator_category::forward> base_type;
private:
	static char const COMMENT_START_CHAR1 = '/';
	static char const COMMENT_START_CHAR2 = '*';
	static char const COMMENT_START_CHAR2ALT = '/';
	static char const COMMENT_STOP_CHAR1 = '*';
	static char const COMMENT_STOP_CHAR2 = '/';
	static char const ESCAPE = '\\';
	static char const DOUBLE_QUOTE = '"';
	static char const SINGLE_QUOTE = '\'';
	struct STATE {
		typedef enum {
			NORMAL,
			IN_COMMENT,
			IN_SINGLE_QUOTE,
			IN_DOUBLE_QUOTE
		} state_t;
	};
	HPrepocessor const* _owner;
	yaal::hcore::HString::const_iterator _cur;
	STATE::state_t _state;
	bool _escape; /*!< Is a QUOTE substate. */
public:
	HIterator( void )
		: _owner( NULL ), _cur(), _state( STATE::NORMAL ), _escape( false ) {
	}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner ), _cur( it_._cur ), _state( it_._state ), _escape( it_._escape ) {
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			HIterator tmp( it_ );
			swap( tmp );
		}
		return ( *this );
	}
	void swap( HIterator& it_ ) {
		if ( &it_ != this ) {
			using yaal::swap;
			swap( _owner, it_._owner );
			swap( _cur, it_._cur );
			swap( _state, it_._state );
			swap( _escape, it_._escape );
		}
		return;
	}
	HIterator& operator ++ ( void ) {
		++ _cur;
		if ( _cur != _owner->_end ) {
			make_readable();
		}
		return ( *this );
	}
	HIterator operator ++ ( int ) const {
		HIterator it( *this );
		++ it;
		return ( it );
	}
	bool operator == ( HIterator const& it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._cur == _cur );
	}
	bool operator != ( HIterator const& it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._cur != _cur );
	}
	value_type const& operator * ( void ) const {
		M_ASSERT( _owner );
		return ( *_cur );
	}
	yaal::hcore::HString::const_iterator raw( void ) const {
		return ( _cur );
	}
private:
	HIterator( HPrepocessor const* owner_, yaal::hcore::HString::const_iterator pos_ )
		: _owner( owner_ ), _cur( pos_ ), _state( STATE::NORMAL ), _escape( false ) {
		make_readable();
		return;
	}
	friend class HPrepocessor;
	void make_readable( void );
	yaal::hcore::HString::const_iterator try_skip_whitespace( yaal::hcore::HString::const_iterator, char );
	yaal::hcore::HString::const_iterator skip_whitespace( yaal::hcore::HString::const_iterator );
	yaal::hcore::HString::const_iterator skip_comment( yaal::hcore::HString::const_iterator );
};

/*
 * These cannot be declared inside HPrepocessor body because HIterator is not fully declared there.
 */
HPrepocessor::HIterator HPrepocessor::begin( void ) const {
	int len( static_cast<int>( _end - _beg ) );
	char const hashBang[] = "#!";
	int const hashBangLen( static_cast<int>( sizeof ( hashBang ) - 1 ) );
	int offset( 0 );
	if ( ( len >= hashBangLen ) && ! strncmp( _beg, hashBang, hashBangLen ) ) {
		char const* nl( static_cast<char const*>( ::memchr( _beg, NEWLINE, static_cast<size_t>( len ) ) ) );
		if ( nl ) {
			offset = static_cast<int>( nl + 1 - _beg );
		}
	}
	return ( HIterator( this, _beg + offset ) );
}

HPrepocessor::HIterator HPrepocessor::end( void ) const {
	return ( HIterator( this, _end ) );
}

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::skip_comment( yaal::hcore::HString::const_iterator pos_ ) {
	yaal::hcore::HString::const_iterator pos( pos_ );
	yaal::hcore::HString::const_iterator goodPos( pos_ );
	while ( true ) {
		if ( *pos == COMMENT_START_CHAR1 ) {
			/* we are possibly in comment, lets check next character */
			++ pos;
			if ( pos == _owner->_end ) {
				/* We are at end of source, we should not update _cur */
				pos = goodPos;
				break;
			} else if ( *pos == COMMENT_START_CHAR2 ) {
#if 0
				we are in /* ... */ comment
#endif /* #if 0 */
				++ pos;
				while ( pos != _owner->_end ) {
					/* we look for end of comment */
					if ( *pos == COMMENT_STOP_CHAR1 ) {
						/* possibly we found first character of end of comment */
						++ pos;
						if ( pos != _owner->_end ) {
							if ( *pos == COMMENT_STOP_CHAR2 ) {
								/* we found end of comment. */
								++ pos;
								break;
							}
						} else {
							/* unfinished comment */
							break;
						}
					}
					++ pos;
				}
				if ( pos == _owner->_end ) {
					break;
				}
				goodPos = pos;
				continue;
			} else if ( *pos == COMMENT_START_CHAR2ALT ) {
				/* We are in single line comment. */
				++ pos;
				while ( pos != _owner->_end ) {
					if ( *pos == NEWLINE ) {
						break;
					}
					++ pos;
				}
				if ( pos == _owner->_end ) {
					break;
				}
				goodPos = pos;
				continue;
			} else {
				pos = goodPos;
			}
		}
		break;
	}
	return ( pos );
}

void HPrepocessor::HIterator::make_readable( void ) {
	/*
	 * Make_readable always starts in NORMAL state or in one of the QUOTEs,
	 * and always ends in NORMAL state or in one of the QUOTEs.
	 */
	yaal::hcore::HString::const_iterator pos( _cur );
	if ( _state == STATE::NORMAL ) {
		pos = skip_comment( pos );
		if ( *pos == SINGLE_QUOTE ) {
			_state = STATE::IN_SINGLE_QUOTE;
		} else if ( *pos == DOUBLE_QUOTE ) {
			_state = STATE::IN_DOUBLE_QUOTE;
		}
	} else {
		/* We are in quote state. */
		if ( _escape ) {
			_escape = false;
		} else if ( *pos == ESCAPE ) {
			_escape = true;
		} else {
			if ( ( ( _state == STATE::IN_SINGLE_QUOTE ) && ( *pos == SINGLE_QUOTE ) )
					|| ( ( _state == STATE::IN_DOUBLE_QUOTE ) && ( *pos == DOUBLE_QUOTE ) ) ) {
				pos = try_skip_whitespace( pos, *pos );
			}
		}
	}
	_cur = pos;
	return;
}

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::skip_whitespace( yaal::hcore::HString::const_iterator pos_ ) {
	yaal::hcore::HString::const_iterator pos( pos_ );
	while ( pos != _owner->_end ) {
		if ( ! is_whitespace( *pos ) ) {
			break;
		}
		++ pos;
	}
	return ( pos );
}

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::try_skip_whitespace( yaal::hcore::HString::const_iterator pos_, char quote_ ) {
	yaal::hcore::HString::const_iterator pos( pos_ );
	/* We are at closing quote character. */
	++ pos;
	while ( pos != _owner->_end ) {
		yaal::hcore::HString::const_iterator orig( pos );
		pos = skip_whitespace( pos );
		if ( pos == _owner->_end ) {
			pos = pos_;
			break;
		}
		pos = skip_comment( pos );
		if ( pos == _owner->_end ) {
			pos = pos_;
			break;
		}
		if ( pos == orig ) {
			break;
		}
	}
	if ( ( pos != _owner->_end ) && ( *pos == quote_ ) ) {
		++ pos;
	} else {
		pos = pos_;
		_state = STATE::NORMAL;
	}
	return ( pos );
}

HHuginn::HSource::HSource( void )
	: _name(),
	_orig(),
	_origSize( 0 ),
	_preprocessed(),
	_preprocessedSize( 0 ),
	_skips() {
	return;
}

void HHuginn::HSource::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int nRead( 0 );
	int block( 0 );
	M_ASSERT( _origSize == 0 );
	do {
		_orig.realloc( ( block + 1 ) * PAGE_SIZE );
		nRead = static_cast<int>( stream_.read( _orig.get<char>() + block * PAGE_SIZE, PAGE_SIZE ) );
		M_ENSURE( nRead >= 0 );
		_origSize += nRead;
		++ block;
	} while ( nRead == PAGE_SIZE );
	_name = get_stream_id( &stream_ );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HHuginn::HSource::name( void ) const {
	M_PROLOG
	return ( _name );
	M_EPILOG
}

/*
 * Strip C-style comments, concatenate literal strings.
 */
void HHuginn::HSource::preprocess( void ) {
	M_PROLOG
	_preprocessed.realloc( _origSize );
	HPrepocessor pp( _orig.get<char>(), _orig.get<char>() + _origSize );
	char* dst( _preprocessed.get<char>() );
	_skips[0] = 0;
	int pos( -1 );
	int skipsTotal( 0 );
	for ( HPrepocessor::HIterator it( pp.begin() ), endIt( pp.end() ); it != endIt; ++ it, ++ dst ) {
		int newPos( static_cast<int>( it.raw() - _orig.get<char>() ) );
		if ( newPos > ( pos + 1 ) ) {
			skipsTotal += ( newPos - ( pos + 1 ) );
			_skips[static_cast<int>( dst - _preprocessed.get<char>() )] = skipsTotal;
		}
		pos = newPos;
		*dst = *it;
	}
	_preprocessedSize = static_cast<int>( dst - _preprocessed.get<char>() );
	return;
	M_EPILOG
}

int HHuginn::HSource::error_position( int position_ ) const {
	M_PROLOG
	skips_t::const_iterator it( _skips.upper_bound( position_ ) );
	-- it;
	M_ENSURE( it != _skips.end() );
	int errorPosition( it->second + position_ );
	return ( errorPosition );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::HSource::error_coordinate( int position_ ) const {
	M_PROLOG
	char const* src( _orig.get<char>() );
	/* +1 because we count lines starting from 1 (not starting from 0) */
	int line( static_cast<int>( count( src, src + position_, NEWLINE ) ) + 1 );
	int lastNewlinePosition( 0 );
	if ( line > 1 ) {
		char const* nl( static_cast<char const*>( ::memrchr( src, NEWLINE, static_cast<size_t>( position_ ) ) ) );
		M_ASSERT( nl );
		int nlPos( static_cast<int>( nl - src ) );
		++ nlPos;
		lastNewlinePosition = nlPos;
	}
	int column( ( position_ - lastNewlinePosition ) + 1 );
	return ( HErrorCoordinate( line, column ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HHuginn::HSource::begin( void ) const {
	M_PROLOG
	return ( _preprocessed.get<char const>() );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HHuginn::HSource::end( void ) const {
	M_PROLOG
	return ( _preprocessed.get<char const>() + _preprocessedSize );
	M_EPILOG
}

void HHuginn::HSource::dump_preprocessed( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int totalWritten( 0 );
	do {
		int nWritten( static_cast<int>( stream_.write( _preprocessed.get<char>() + totalWritten,
						_preprocessedSize - totalWritten >= PAGE_SIZE ? PAGE_SIZE : _preprocessedSize - totalWritten ) ) );
		M_ENSURE( nWritten >= 0 );
		totalWritten += nWritten;
	} while ( totalWritten < _preprocessedSize );
	return;
	M_EPILOG
}

HHuginn::OCompiler::OContext::OContext( HHuginn* huginn_ )
	: _scope( make_pointer<HScope>() ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, make_pointer<HExpression>( huginn_ ) );
	return;
}

HHuginn::OCompiler::OContext::OContext( scope_t const& scope_, expression_t const& expression_ )
	: _scope( scope_ ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, expression_ );
	return;
}

HHuginn::expression_t const& HHuginn::OCompiler::OContext::expression( void ) const {
	return ( _expressionsStack.top().back() );
}

HHuginn::expression_t& HHuginn::OCompiler::OContext::expression( void ) {
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

HHuginn::OCompiler::OCompiler( HHuginn* huginn_ )
	: _huginn( huginn_ ),
	_functionName(),
	_parameters(),
	_compilationStack(),
	_operations(),
	_valueTypes() {
	return;
}

HHuginn::scope_t& HHuginn::OCompiler::current_scope( void ) {
	return ( _compilationStack.top()._context._scope );
}

HHuginn::expression_t& HHuginn::OCompiler::current_expression( void ) {
	return ( _compilationStack.top()._context.expression() );
}

void HHuginn::OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ), position_.get() );
	}
	_functionName = name_;
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_paramater( yaal::hcore::HString const& name_, executing_parser::position_t ) {
	M_PROLOG
	_parameters.push_back( name_ );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::create_scope( executing_parser::position_t ) {
	M_PROLOG
	_compilationStack.emplace( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_scope( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	scope_t scope( current_scope() );
	_compilationStack.pop();
	M_ASSERT( ! _compilationStack.is_empty() );
	current_scope()->add_statement( scope );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::reset_expression( void ) {
	M_PROLOG
	current_expression() = make_pointer<HExpression>( _huginn );
	M_ASSERT( _operations.is_empty() );
	_valueTypes.clear();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::start_subexpression( executing_parser::position_t position_ ) {
	M_PROLOG
	_compilationStack.top()._context._expressionsStack.emplace( 1, make_pointer<HExpression>( _huginn, position_.get() ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_subexpression( OPERATOR op_, executing_parser::position_t position_ ) {
	M_PROLOG
	_compilationStack.top()._context._expressionsStack.top().emplace_back( make_pointer<HExpression>( _huginn, position_.get() ) );
	_operations.emplace( op_, position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_boolean( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( _compilationStack.top()._context._expressionsStack.top().get_size() > 1 ) {
		value_t And( make_pointer<HBooleanEvaluator>( _compilationStack.top()._context._expressionsStack.top(), operator_ ) );
		_compilationStack.top()._context._expressionsStack.pop();
		defer_store_direct( And, position_ );
		current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
		defer_action( operator_ == OPERATOR::BOOLEAN_AND ? &HExpression::boolean_and : &HExpression::boolean_or, position_.get() );
	} else {
		expression_t e( _compilationStack.top()._context.expression() );
		_compilationStack.top()._context._expressionsStack.pop();
		_compilationStack.top()._context.expression()->merge( *e );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_return_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HReturn>( current_expression() ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_break_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HBreak>() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_while_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	scope_t scope( current_scope() );
	_compilationStack.pop();
	current_scope()->add_statement( make_pointer<HWhile>( current_expression(), scope ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::set_for_identifier( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OCompilationFrame& f( _compilationStack.top() );
	f._forIdentifier = name_;
	f._forPosition = position_.get();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_for_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	scope_t scope( current_scope() );
	_compilationStack.pop();
	OCompilationFrame const& f( _compilationStack.top() );
	current_scope()->add_statement( make_pointer<HFor>( f._forIdentifier, current_expression(), scope, f._forPosition ) );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_if_clause( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	scope_t scope( current_scope() );
	_compilationStack.pop();
	_compilationStack.top()._contextsChain.emplace_back( scope, current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_else_clause( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	scope_t scope( current_scope() );
	_compilationStack.pop();
	_compilationStack.top()._else = scope;
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_if_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	statement_t ifStatement( make_pointer<HIf>( _compilationStack.top()._contextsChain, _compilationStack.top()._else ) );
	_compilationStack.top()._contextsChain.clear();
	_compilationStack.top()._else.reset();
	current_scope()->add_statement( ifStatement );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_switch_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
	OCompilationFrame::contexts_t contexts( yaal::move( _compilationStack.top()._contextsChain ) );
	scope_t Default( _compilationStack.top()._else );
	_compilationStack.pop();
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
	M_ASSERT( ! _compilationStack.is_empty() );
	current_scope()->add_statement( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::mark_expression_position( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! _compilationStack.is_empty() );
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
		case ( '[' ): o = HHuginn::OPERATOR::SUBSCRIPT;   break;
		case ( '!' ): o = HHuginn::OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), o, _1, position_.get() ) );
	_operations.emplace( o, position_.get() );
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
	_operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_oper_direct( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
	_operations.emplace( operator_, position_.get() );
	return;
	M_EPILOG
}

bool HHuginn::OCompiler::is_numeric( TYPE type_ ) {
	return ( ( type_ == TYPE::INTEGER ) || ( type_ == TYPE::REAL ) || ( type_ == TYPE::NUMBER ) );
}

bool HHuginn::OCompiler::is_comparable( TYPE type_ ) {
	return (
		is_numeric( type_ )
		|| ( type_ == TYPE::STRING )
		|| ( type_ == TYPE::CHARACTER )
	);
}

bool HHuginn::OCompiler::is_boolean_congruent( TYPE type_ ) {
	return ( ( type_ == TYPE::BOOLEAN ) || ( type_ == TYPE::UNKNOWN ) || ( type_ == TYPE::REFERENCE ) );
}

bool HHuginn::OCompiler::is_unknown( TYPE type_ ) {
	return ( ( type_ == TYPE::NOT_BOOLEAN ) || ( type_ == TYPE::UNKNOWN ) || ( type_ == TYPE::REFERENCE ) );
}

bool HHuginn::OCompiler::is_numeric_congruent( TYPE type_ ) {
	return ( is_numeric( type_ ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_comparable_congruent( TYPE type_ ) {
	return ( is_comparable( type_ ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_reference_congruent( TYPE type_ ) {
	return ( ( type_ == TYPE::REFERENCE ) || ( type_ == TYPE::UNKNOWN ) );
}

bool HHuginn::OCompiler::is_integer_congruent( TYPE type_ ) {
	return ( ( type_ == TYPE::INTEGER ) || is_unknown( type_ ) );
}

bool HHuginn::OCompiler::is_summable( TYPE type_ ) {
	return ( is_numeric_congruent( type_ ) || ( type_ == TYPE::STRING ) );
}

bool HHuginn::OCompiler::are_congruous( TYPE t1_, TYPE t2_ ) {
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

HHuginn::TYPE HHuginn::OCompiler::congruent( TYPE t1_, TYPE t2_ ) {
	TYPE t( TYPE::NOT_BOOLEAN );
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
	M_ASSERT( ! _operations.is_empty() );
	M_ASSERT( _valueTypes.get_size() >= 2 );
	OPositionedOperator po( _operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::PLUS ) || ( o == OPERATOR::MINUS ) );
	defer_action( o == OPERATOR::PLUS ? &HExpression::plus : &HExpression::minus, position_ );
	_operations.pop();
	TYPE t1( _valueTypes.top() );
	_valueTypes.pop();
	TYPE t2( _valueTypes.top() );
	_valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_summable( t1 ) && is_summable( t2 ) ) ) {
		throw HHuginnRuntimeException( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB], p );
	}
	_valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_mul( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! _operations.is_empty() );
	M_ASSERT( _valueTypes.get_size() >= 2 );
	OPositionedOperator po( _operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::MULTIPLY ) || ( o == OPERATOR::DIVIDE ) || ( o == OPERATOR::MODULO ) );
	defer_action( o == OPERATOR::MULTIPLY ? &HExpression::mul : ( o == OPERATOR::DIVIDE ? &HExpression::div : &HExpression::mod ), position_ );
	_operations.pop();
	TYPE t1( _valueTypes.top() );
	_valueTypes.pop();
	TYPE t2( _valueTypes.top() );
	_valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV], p );
	}
	_valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_power( executing_parser::position_t position_ ) {
	M_PROLOG
	defer_action( &HExpression::power, position_ );
	while ( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::POWER ) ) {
		M_ASSERT( _valueTypes.get_size() >= 2 );
		int p( _operations.top()._position );
		_operations.pop();
		TYPE t1( _valueTypes.top() );
		_valueTypes.pop();
		TYPE t2( _valueTypes.top() );
		_valueTypes.pop();
		if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
			throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP], p );
		}
		_valueTypes.push( congruent( t1, t2 ) );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_compare( executing_parser::position_t position_ ) {
	M_PROLOG
	OPositionedOperator po( _operations.top() );
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
	_operations.pop();
	M_ASSERT( _valueTypes.get_size() >= 2 );
	TYPE t1( _valueTypes.top() );
	_valueTypes.pop();
	TYPE t2( _valueTypes.top() );
	_valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_comparable_congruent( t1 ) && is_comparable_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP], p );
	}
	_valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_boolean( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OPositionedOperator po( _operations.top() );
	int p( po._position );
	if ( action_ ) {
		defer_action( action_, position_ );
	}
	_operations.pop();
	M_ASSERT( _valueTypes.get_size() >= 2 );
	TYPE t1( _valueTypes.top() );
	_valueTypes.pop();
	TYPE t2( _valueTypes.top() );
	_valueTypes.pop();
	if ( ! ( is_boolean_congruent( t1 ) && is_boolean_congruent( t2 ) ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], p );
	}
	_valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_equals( executing_parser::position_t position_ ) {
	M_PROLOG
	OPositionedOperator po( _operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::EQUALS ) || ( o == OPERATOR::NOT_EQUALS ) );
	defer_action( o == OPERATOR::EQUALS ? &HExpression::equals : &HExpression::not_equals, position_ );
	_operations.pop();
	M_ASSERT( _valueTypes.get_size() >= 2 );
	TYPE t1( _valueTypes.top() );
	_valueTypes.pop();
	TYPE t2( _valueTypes.top() );
	_valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	_valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_assign( executing_parser::position_t position_ ) {
	M_PROLOG
	defer_action( &HExpression::set_variable, position_ );
	while ( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::ASSIGN ) ) {
		M_ASSERT( _valueTypes.get_size() >= 2 );
		int p( _operations.top()._position );
		_operations.pop();
		TYPE t1( _valueTypes.top() );
		_valueTypes.pop();
		TYPE t2( _valueTypes.top() );
		_valueTypes.pop();
		if ( ! is_reference_congruent( t2 ) ) {
			throw HHuginnRuntimeException( "Setting a non reference location.", p );
		}
		_valueTypes.push( t1 );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_subscript( executing_parser::position_t position_ ) {
	M_PROLOG
	OPositionedOperator po( _operations.top() );
	int p( po._position );
	M_ASSERT( po._operator == OPERATOR::SUBSCRIPT );
	defer_action( &HExpression::subscript, position_ );
	_operations.pop();
	M_ASSERT( _valueTypes.get_size() >= 2 );
	if ( ! is_integer_congruent( _valueTypes.top() ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT], p );
	}
	_valueTypes.pop();
	_valueTypes.pop();
	_valueTypes.push( TYPE::REFERENCE );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_function_call( executing_parser::position_t position_ ) {
	M_PROLOG
	defer_action( &HExpression::function_call_exec, position_ );
	while ( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) ) {
		_operations.pop();
		_valueTypes.pop();
	}
	M_ASSERT( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::FUNCTION_CALL ) );
	int p( _operations.top()._position );
	_operations.pop();
	if ( _valueTypes.top() != TYPE::FUNCTION_REFERENCE ) {
		throw HHuginnRuntimeException( "Not a function.", p );
	}
	_valueTypes.pop();
	_valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::dispatch_action( OPERATOR oper_, executing_parser::position_t position_ ) {
	M_PROLOG
	OPositionedOperator po( ! _operations.is_empty() ? _operations.top() : OPositionedOperator( OPERATOR::NONE, 0 ) );
	OPERATOR o( po._operator );
	int p( po._position );
	switch ( oper_ ) {
		case ( OPERATOR::PLUS ):     { dispatch_plus( position_ );  } break;
		case ( OPERATOR::MULTIPLY ): { dispatch_mul( position_ );   } break;
		case ( OPERATOR::POWER ):    { dispatch_power( position_ ); } break;
		case ( OPERATOR::NEGATE ): {
			M_ASSERT( o == OPERATOR::NEGATE );
			M_ASSERT( ! _valueTypes.is_empty() );
			defer_action( &HExpression::negate, position_ );
			_operations.pop();
			if ( ! is_numeric_congruent( _valueTypes.top() ) ) {
				throw HHuginnRuntimeException( "Operand is not a numeric value.", p );
			}
		} break;
		case ( OPERATOR::SUBSCRIPT ): { dispatch_subscript( position_ ); } break;
		case ( OPERATOR::ASSIGN ): { dispatch_assign( position_ ); } break;
		case ( OPERATOR::FUNCTION_CALL ): { dispatch_function_call( position_ ); } break;
		case ( OPERATOR::PARENTHESIS ):
		case ( OPERATOR::ABSOLUTE ): {
			M_ASSERT( ( o == OPERATOR::ABSOLUTE ) || ( o == OPERATOR::PARENTHESIS ) );
			defer_action( &HExpression::close_parenthesis, position_ );
			_operations.pop();
			if ( o == OPERATOR::ABSOLUTE ) {
				M_ASSERT( ! _valueTypes.is_empty() );
				if ( ! is_numeric_congruent( _valueTypes.top() ) ) {
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
			M_ASSERT( ! _valueTypes.is_empty() );
			defer_action( &HExpression::boolean_not, position_ );
			_operations.pop();
			if ( ! is_boolean_congruent( _valueTypes.top() ) ) {
				throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
			}
		} break;
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

void HHuginn::OCompiler::defer_function_call( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::function_call, current_expression().raw(), value_, _1, position_.get() ) );
	_operations.emplace( OPERATOR::FUNCTION_CALL, position_.get() );
	_valueTypes.push( TYPE::FUNCTION_REFERENCE );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_get_variable( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::get_variable, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_make_variable( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::make_variable, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_direct( value_t const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( value_->type() );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_real( double long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_real, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::REAL );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_integer( int long long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_integer, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::INTEGER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_string( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_string, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::STRING );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_number( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_number, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::NUMBER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_character( char value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_character, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::CHARACTER );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_boolean( bool value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_boolean, current_expression().raw(), value_, _1, position_.get() ) );
	_valueTypes.push( TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_none( executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_none, current_expression().raw(), _1, position_.get() ) );
	_valueTypes.push( TYPE::NONE );
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

HHuginn::flag_t HHuginn::_grammarVerified{ false };

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY ),
	_functions(),
	_engine( make_engine(), _grammarVerified.load() ? HExecutingParser::INIT_MODE::TRUST_GRAMMAR : HExecutingParser::INIT_MODE::VERIFY_GRAMMAR ),
	_source(),
	_compiler( this ),
	_threads(),
	_argv( new ( memory::yaal ) HList() ),
	_result(),
	_errorMessage(),
	_errorPosition( -1 ),
	_inputStream(),
	_inputStreamRaw( &cin ),
	_outputStream(),
	_outputStreamRaw( &cout ),
	_errorStream(),
	_errorStreamRaw( &cerr ) {
	M_PROLOG
	_grammarVerified.store( true );
	register_builtins();
	return;
	M_EPILOG
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	_source.load( stream_ );
	_state = STATE::LOADED;
	return;
	M_EPILOG
}

void HHuginn::preprocess( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::LOADED );
	_source.preprocess();
	_state = STATE::PREPROCESSED;
	return;
	M_EPILOG
}

bool HHuginn::parse( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PREPROCESSED );
	_state = STATE::PARSED;
	bool ok( _engine.parse( _source.begin(), _source.end() ) );
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
	threads_t::iterator t( _threads.insert( make_pair( threadId, make_pointer<HThread>( this, threadId ) ) ).first );
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

HHuginn::HFrame* HHuginn::current_frame( void ) {
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

int HHuginn::error_position( void ) const {
	M_PROLOG
	return ( _source.error_position( _errorPosition ) );
	M_EPILOG
}

HHuginn::HErrorCoordinate HHuginn::error_coordinate( void ) const {
	M_PROLOG
	return ( _source.error_coordinate( error_position() ) );
	M_EPILOG
}

yaal::hcore::HString HHuginn::error_message( void ) const {
	M_PROLOG
	hcore::HString message( _source.name() );
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

char const* HHuginn::HSource::error_message( int code_ ) const {
	return ( ::error_message( code_ ) );
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

HHuginn::value_t HHuginn::returned_value( void ) const {
	M_PROLOG
	return ( value_t() );
	M_EPILOG
}

void HHuginn::dump_preprocessed_source( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	_source.dump_preprocessed( stream_ );
	return;
	M_EPILOG
}

void HHuginn::dump_vm_state( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	stream_ << "Huginn VM state for `" << _source.name() << "':\nfunctions:" << endl;
	for ( functions_t::value_type const& f : _functions ) {
		stream_ << f.first << endl;
	}
	return;
	M_EPILOG
}

void HHuginn::create_function( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! _compiler._functionName.is_empty() );
	M_ASSERT( ! _compiler._compilationStack.is_empty() );
	function_t f( hcore::call( &HFunction::execute, make_pointer<HFunction>( _compiler._functionName, _compiler._parameters, yaal::move( _compiler.current_scope() ) ), _1, _2, _3 ) );
	_functions.insert( make_pair<hcore::HString const, function_t>( yaal::move( _compiler._functionName ), yaal::move( f ) ) );
	_compiler._compilationStack.pop();
	M_ASSERT( _compiler._compilationStack.is_empty() );
	_compiler._parameters.clear();
	_compiler._functionName.clear();
	return;
	M_EPILOG
}

HHuginn::HThread::HThread( HHuginn* huginn_, yaal::hcore::HThread::id_t id_ )
	: _frames(), _id( id_ ), _huginn( huginn_ ) {
	return;
}

yaal::hcore::HThread::id_t HHuginn::HThread::id( void ) const {
	return ( _id );
}

HHuginn::HFrame* HHuginn::HThread::current_frame( void ) {
	M_PROLOG
	return ( ! _frames.is_empty() ? _frames.top().raw() : nullptr );
	M_EPILOG
}

HHuginn::HFrame const* HHuginn::HThread::current_frame( void ) const {
	M_PROLOG
	return ( ! _frames.is_empty() ? _frames.top().raw() : nullptr );
	M_EPILOG
}

void HHuginn::HThread::create_function_frame( void ) {
	M_PROLOG
	HHuginn::HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, true, false ) );
	return;
	M_EPILOG
}

void HHuginn::HThread::create_loop_frame( void ) {
	M_PROLOG
	HHuginn::HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, false, true ) );
	return;
	M_EPILOG
}

void HHuginn::HThread::create_scope_frame( void ) {
	M_PROLOG
	HHuginn::HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, false, false ) );
	return;
	M_EPILOG
}

void HHuginn::HThread::pop_frame( void ) {
	M_PROLOG
	_frames.pop();
	return;
	M_EPILOG
}

bool HHuginn::HThread::can_continue( void ) const {
	M_PROLOG
	M_ASSERT( current_frame() );
	return ( current_frame()->can_continue() );
	M_EPILOG
}

void HHuginn::HThread::break_execution( HFrame::STATE state_, HHuginn::value_t const& value_, int level_ ) {
	M_PROLOG
	M_ASSERT( ( state_ == HHuginn::HFrame::STATE::RETURN ) || ( state_ == HHuginn::HFrame::STATE::BREAK ) );
	int level( 0 );
	HFrame* f( current_frame() );
	HFrame* target( f );
	int no( f->number() );
	while ( f ) {
		f->break_execution( state_ );
		target = f;
		if ( f->is_loop() ) {
			++ level;
		}
		f = f->parent();
		if ( ! f ) {
			break;
		} else if ( f->number() != no ) {
			break;
		} else if ( ( state_ == HHuginn::HFrame::STATE::BREAK ) && ( level > level_ ) ) {
			break;
		}
	}
	if ( target ) {
		target->set_result( value_ );
	}
	return;
	M_EPILOG
}

HHuginn& HHuginn::HThread::huginn( void ) {
	return ( *_huginn );
}

HHuginn::HFrame::HFrame( HThread* thread_, HFrame* parent_, bool bump_, bool loop_ )
	: _thread( thread_ ),
	_parent( parent_ ),
	_variables(),
	_operations(),
	_values(),
	_result(),
	_number( parent_ ? ( parent_->_number + ( bump_ ? 1 : 0 ) ) : 1 ),
	_loop( loop_ ),
	_state( STATE::NORMAL ) {
	return;
}

HHuginn::HThread* HHuginn::HFrame::thread( void ) const {
	return ( _thread );
}

int HHuginn::HFrame::number( void ) const {
	return ( _number );
}

HHuginn::HFrame* HHuginn::HFrame::parent( void ) {
	return ( _parent );
}

bool HHuginn::HFrame::is_loop( void ) const {
	return ( _loop );
}

bool HHuginn::HFrame::can_continue( void ) const {
	return ( _state == STATE::NORMAL );
}

void HHuginn::HFrame::break_execution( STATE state_ ) {
	_state = state_;
	return;
}

void HHuginn::HFrame::set_result( HHuginn::value_t const& result_ ) {
	M_PROLOG
	_result = result_;
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HFrame::result( void ) const {
	return ( _result );
}

void HHuginn::HFrame::reset( void ) {
	M_PROLOG
	_operations.clear();
	_values.clear();
	return;
	M_EPILOG
}

HHuginn::value_t& HHuginn::HFrame::get_variable( yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	variables_t::iterator it( _variables.find( name_ ) );
	HHuginn::value_t* v( nullptr );
	if ( it != _variables.end() ) {
		v = &it->second;
	} else if ( _parent && ( _parent->_number == _number ) ) {
		v = &_parent->get_variable( name_, position_ );
	} else {
		throw HHuginnRuntimeException( "variable `"_ys.append( name_ ).append( "' does not exist" ), position_ );
	}
	return ( *v );
	M_EPILOG
}

void HHuginn::HFrame::set_variable( yaal::hcore::HString const& name_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	value_t ref( make_variable( name_, position_ ) );
	static_cast<HReference*>( ref.raw() )->value() = value_;
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HFrame::make_variable( yaal::hcore::HString const& name_, int ) {
	M_PROLOG
	HFrame* f( this );
	variables_t::iterator it;
	while ( f ) {
		it = f->_variables.find( name_ );
		if ( it != f->_variables.end() ) {
			break;
		}
		if ( f->_parent && ( f->_parent->_number == _number ) ) {
			f = f->_parent;
		} else {
			f = nullptr;
		}
	}
	if ( ! f ) {
		it = _variables.insert( make_pair( name_, _none_ ) ).first;
	}
	return ( make_pointer<HReference>( it->second ) );
	M_EPILOG
}

HHuginn::operations_t& HHuginn::HFrame::operations( void ) {
	return ( _operations );
}

HHuginn::HFrame::values_t& HHuginn::HFrame::values( void ) {
	return ( _values );
}

HHuginn::HValue::HValue( void )
	: _type( TYPE::NONE ), _methods() {
	return;
}

HHuginn::HValue::HValue( TYPE type_ )
	: _type( type_ ), _methods() {
	return;
}

HHuginn::TYPE HHuginn::HValue::type( void ) const {
	return ( _type );
}

yaal::hcore::HString const& HHuginn::HValue::type_name( TYPE type_ ) {
	static yaal::hcore::HString const names[] = {
		"none",
		"boolean",
		"integer",
		"real",
		"string",
		"number",
		"character",
		"list",
		"map",
		"reference",
		"function_reference",
		"unknown",
		"not-boolean"
	};
	return ( names[static_cast<int>( type_ )] );
}

HHuginn::value_t HHuginn::HValue::subscript( HHuginn::value_t& base_, HHuginn::value_t const& index_, int position_ ) {
	M_ASSERT( index_->type() == TYPE::INTEGER );
	TYPE baseType( base_->type() );
	HInteger const* i( static_cast<HInteger const*>( index_.raw() ) );
	int long long index( i->value() );
	value_t res;
	if ( ( baseType != TYPE::LIST ) && ( baseType != TYPE::STRING ) ) {
		throw HHuginnRuntimeException( "Subscript is not supported on `"_ys.append( type_name( baseType ) ).append( "'." ), position_ );
	} else if ( baseType == TYPE::LIST ) {
		HList* l( static_cast<HList*>( base_.raw() ) );
		if ( ( index < 0 ) || ( index >= l->size() ) ) {
			throw HHuginnRuntimeException( "Bad index.", position_ );
		}
		res = l->get( index );
	} else {
		M_ASSERT( baseType == TYPE::STRING );
		HString* s( static_cast<HString*>( base_.raw() ) );
		if ( ( index < 0 ) || ( index >= s->value().get_length() ) ) {
			throw HHuginnRuntimeException( "Bad index.", position_ );
		}
		res = make_pointer<HCharacter>( s->value()[static_cast<int>( index )] );
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::add( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() + static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( static_cast<HReal const*>( v1_.raw() )->value() + static_cast<HReal const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::STRING ): {
			res = make_pointer<HString>( static_cast<HString const*>( v1_.raw() )->value() + static_cast<HString const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( static_cast<HNumber const*>( v1_.raw() )->value() + static_cast<HNumber const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `+' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::sub( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() - static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( static_cast<HReal const*>( v1_.raw() )->value() - static_cast<HReal const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( static_cast<HNumber const*>( v1_.raw() )->value() - static_cast<HNumber const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `-' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::mul( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() * static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( static_cast<HReal const*>( v1_.raw() )->value() * static_cast<HReal const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( static_cast<HNumber const*>( v1_.raw() )->value() * static_cast<HNumber const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `*' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::div( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() / static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( static_cast<HReal const*>( v1_.raw() )->value() / static_cast<HReal const*>( v2_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( static_cast<HNumber const*>( v1_.raw() )->value() / static_cast<HNumber const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `/' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::mod( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() % static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `%' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::pow( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( ::powl( static_cast<HReal const*>( v1_.raw() )->value(), static_cast<HReal const*>( v2_.raw() )->value() ) );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( static_cast<HNumber const*>( v1_.raw() )->value() ^ static_cast<HNumber const*>( v2_.raw() )->value().to_integer() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `^' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::abs( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::INTEGER ): {
			int long long v( static_cast<HInteger const*>( v_.raw() )->value() );
			if ( v >= 0 ) {
				res = v_;
			} else {
				res = make_pointer<HInteger>( -v );
			}
		} break;
		case ( TYPE::REAL ): {
			double long v( static_cast<HReal const*>( v_.raw() )->value() );
			if ( v >= 0 ) {
				res = v_;
			} else {
				res = make_pointer<HReal>( -v );
			}
		} break;
		case ( TYPE::NUMBER ): {
			yaal::hcore::HNumber const& v( static_cast<HNumber const*>( v_.raw() )->value() );
			if ( v >= 0 ) {
				res = v_;
			} else {
				res = make_pointer<HNumber>( -v );
			}
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no |.| operator for `"_ys.append( type_name( v_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::neg( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( -static_cast<HInteger const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HReal>( -static_cast<HReal const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HNumber>( -static_cast<HNumber const*>( v_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `negate` operator for `"_ys.append( type_name( v_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

bool HHuginn::HValue::equals( value_t const& v1_, value_t const& v2_, int position_ ) {
	bool noneOperand( ( v1_->type() == TYPE::NONE ) || ( v2_->type() == TYPE::NONE ) );
	M_ASSERT( noneOperand || ( v1_->type() == v2_->type() ) );
	bool res( false );
	if ( ! noneOperand ) {
		switch ( v1_->type() ) {
			case ( TYPE::INTEGER ): {
				res = static_cast<HInteger const*>( v1_.raw() )->value() == static_cast<HInteger const*>( v2_.raw() )->value();
			} break;
			case ( TYPE::REAL ): {
				res = static_cast<HReal const*>( v1_.raw() )->value() == static_cast<HReal const*>( v2_.raw() )->value();
			} break;
			case ( TYPE::STRING ): {
				res = static_cast<HString const*>( v1_.raw() )->value() == static_cast<HString const*>( v2_.raw() )->value();
			} break;
			case ( TYPE::NUMBER ): {
				res = static_cast<HNumber const*>( v1_.raw() )->value() == static_cast<HNumber const*>( v2_.raw() )->value();
			} break;
			case ( TYPE::CHARACTER ): {
				res = static_cast<HCharacter const*>( v1_.raw() )->value() == static_cast<HCharacter const*>( v2_.raw() )->value();
			} break;
			case ( TYPE::BOOLEAN ): {
				res = static_cast<HBoolean const*>( v1_.raw() )->value() == static_cast<HBoolean const*>( v2_.raw() )->value();
			} break;
			default: {
				throw HHuginnRuntimeException( "There is no `==' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
			}
		}
	} else {
		res = v1_->type() == v2_->type();
	}
	return ( res );
}

bool HHuginn::HValue::less( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = static_cast<HInteger const*>( v1_.raw() )->value() < static_cast<HInteger const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::REAL ): {
			res = static_cast<HReal const*>( v1_.raw() )->value() < static_cast<HReal const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::STRING ): {
			res = static_cast<HString const*>( v1_.raw() )->value() < static_cast<HString const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::NUMBER ): {
			res = static_cast<HNumber const*>( v1_.raw() )->value() < static_cast<HNumber const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::CHARACTER ): {
			res = static_cast<int unsigned>( static_cast<HCharacter const*>( v1_.raw() )->value() ) < static_cast<int unsigned>( static_cast<HCharacter const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `<' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

bool HHuginn::HValue::greater( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = static_cast<HInteger const*>( v1_.raw() )->value() > static_cast<HInteger const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::REAL ): {
			res = static_cast<HReal const*>( v1_.raw() )->value() > static_cast<HReal const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::STRING ): {
			res = static_cast<HString const*>( v1_.raw() )->value() > static_cast<HString const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::NUMBER ): {
			res = static_cast<HNumber const*>( v1_.raw() )->value() > static_cast<HNumber const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::CHARACTER ): {
			res = static_cast<int unsigned>( static_cast<HCharacter const*>( v1_.raw() )->value() ) > static_cast<int unsigned>( static_cast<HCharacter const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `>' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

bool HHuginn::HValue::less_or_equal( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = static_cast<HInteger const*>( v1_.raw() )->value() <= static_cast<HInteger const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::REAL ): {
			res = static_cast<HReal const*>( v1_.raw() )->value() <= static_cast<HReal const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::STRING ): {
			res = static_cast<HString const*>( v1_.raw() )->value() <= static_cast<HString const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::NUMBER ): {
			res = static_cast<HNumber const*>( v1_.raw() )->value() <= static_cast<HNumber const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::CHARACTER ): {
			res = static_cast<int unsigned>( static_cast<HCharacter const*>( v1_.raw() )->value() ) <= static_cast<int unsigned>( static_cast<HCharacter const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `<=' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

bool HHuginn::HValue::greater_or_equal( value_t const& v1_, value_t const& v2_, int position_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	bool res( false );
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = static_cast<HInteger const*>( v1_.raw() )->value() >= static_cast<HInteger const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::REAL ): {
			res = static_cast<HReal const*>( v1_.raw() )->value() >= static_cast<HReal const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::STRING ): {
			res = static_cast<HString const*>( v1_.raw() )->value() >= static_cast<HString const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::NUMBER ): {
			res = static_cast<HNumber const*>( v1_.raw() )->value() >= static_cast<HNumber const*>( v2_.raw() )->value();
		} break;
		case ( TYPE::CHARACTER ): {
			res = static_cast<int unsigned>( static_cast<HCharacter const*>( v1_.raw() )->value() ) >= static_cast<int unsigned>( static_cast<HCharacter const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "There is no `>=' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ), position_ );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::boolean_xor( value_t const& v1_, value_t const& v2_, int ) {
	M_ASSERT( ( v1_->type() == TYPE::BOOLEAN ) && ( v2_->type() == TYPE::BOOLEAN ) );
	bool v1( static_cast<HBoolean const*>( v1_.raw() )->value() );
	bool v2( static_cast<HBoolean const*>( v2_.raw() )->value() );
	return ( make_pointer<HBoolean>( ( v1 && ! v2 ) || ( ! v1 && v2 ) ) );
}

HHuginn::value_t HHuginn::HValue::boolean_not( value_t const& v_, int ) {
	M_ASSERT( v_->type() == TYPE::BOOLEAN );
	return ( make_pointer<HBoolean>( ! static_cast<HBoolean const*>( v_.raw() )->value() ) );
}

HHuginn::value_t HHuginn::HValue::string( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::STRING ): {
			res = v_;
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HString>( static_cast<HNumber const*>( v_.raw() )->value().to_string() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HString>( static_cast<HReal const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::CHARACTER ): {
			res = make_pointer<HString>( static_cast<HCharacter const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::INTEGER ): {
			res = make_pointer<HString>( static_cast<HInteger const*>( v_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `string' is not supported." ),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::boolean( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::BOOLEAN ): {
			res = v_;
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `boolean' is not supported." ),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::integer( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::STRING ): {
			res = make_pointer<HInteger>( lexical_cast<int long long>( static_cast<HString const*>( v_.raw() )->value() ) );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HInteger>( static_cast<HNumber const*>( v_.raw() )->value().to_integer() );
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HInteger>( static_cast<int long long>( static_cast<HReal const*>( v_.raw() )->value() ) );
		} break;
		case ( TYPE::CHARACTER ): {
			res = make_pointer<HInteger>( static_cast<int long long>( static_cast<HCharacter const*>( v_.raw() )->value() ) );
		} break;
		case ( TYPE::INTEGER ): {
			res = v_;
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `integer' is not supported." ),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::real( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::STRING ): {
			res = make_pointer<HReal>( lexical_cast<double long>( static_cast<HString const*>( v_.raw() )->value() ) );
		} break;
		case ( TYPE::NUMBER ): {
			res = make_pointer<HReal>( static_cast<HNumber const*>( v_.raw() )->value().to_floating_point() );
		} break;
		case ( TYPE::REAL ): {
			res = v_;
		} break;
		case ( TYPE::INTEGER ): {
			res = make_pointer<HReal>( static_cast<double long>( static_cast<HInteger const*>( v_.raw() )->value() ) );
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `real' is not supported." ),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::character( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::CHARACTER ): {
			res = v_;
		} break;
		case ( TYPE::INTEGER ): {
			res = make_pointer<HCharacter>( static_cast<char>( static_cast<HCharacter const*>( v_.raw() )->value() ) );
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `character' is not supported." ),
				position_
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::number( value_t const& v_, int position_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::STRING ): {
			res = make_pointer<HNumber>( static_cast<HString const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::NUMBER ): {
			res = v_;
		} break;
		case ( TYPE::REAL ): {
			res = make_pointer<HNumber>( static_cast<HReal const*>( v_.raw() )->value() );
		} break;
		case ( TYPE::INTEGER ): {
			res = make_pointer<HNumber>( static_cast<HInteger const*>( v_.raw() )->value() );
		} break;
		default: {
			throw HHuginnRuntimeException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `number' is not supported." ),
				position_
			);
		}
	}
	return ( res );
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
	virtual HHuginn::value_t do_value( void ) {
		return ( make_pointer<HHuginn::HCharacter>( _string->value()[ _index ] ) );
	}
	virtual bool do_is_valid( void ) {
		return ( _index < _string->value().get_size() );
	}
	virtual void do_next( void ) {
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
	virtual HHuginn::value_t do_value( void ) {
		return ( _list->get( _index ) );
	}
	virtual bool do_is_valid( void ) {
		return ( _index < _list->size() );
	}
	virtual void do_next( void ) {
		++ _index;
	}
private:
	HListIterator( HListIterator const& ) = delete;
	HListIterator& operator = ( HListIterator const& ) = delete;
};

class HMapIterator : public HIteratorInterface {
	HHuginn::HMap* _map;
public:
	HMapIterator( HHuginn::HMap* map_ )
		: _map( map_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) {
		return ( HHuginn::value_t() );
	}
	virtual bool do_is_valid( void ) {
		return ( false );
	}
	virtual void do_next( void ) {
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

HHuginn::HIterable::HIterable( TYPE type_ )
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
	return ( make_pointer<HReference>( _data[index_] ) );
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HList::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HListIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::HMap::HMap( void )
	: HIterable( TYPE::MAP ), _data() {
	return;
}

int long HHuginn::HMap::size( void ) const {
	return ( _data.get_size() );
}

HHuginn::HIterable::HIterator HHuginn::HMap::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HMapIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::HBooleanEvaluator::HBooleanEvaluator( expressions_t const& expressions_, OPERATOR operator_ )
	: HValue( TYPE::BOOLEAN ),
	_expressions( expressions_ ),
	_operator( operator_ ) {
	return;
}

bool HHuginn::HBooleanEvaluator::execute( HThread* thread_ ) {
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

HHuginn::HStatement::HStatement( void ) {
	return;
}

void HHuginn::HStatement::execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	do_execute( thread_ );
	return;
	M_EPILOG
}

HHuginn::HExpression::HExpression( HHuginn* huginn_, int position_ )
	: HStatement(),
	_executionSteps(),
	_position( position_ ),
	_huginn( huginn_ ) {
	return;
}

int HHuginn::HExpression::position( void ) const {
	M_PROLOG
	return ( _position );
	M_EPILOG
}

void HHuginn::HExpression::set_position( int position_ ) {
	M_PROLOG
	_position = position_;
	return;
	M_EPILOG
}

void HHuginn::HExpression::add_execution_step( execution_step_t const& executionStep_ ) {
	M_PROLOG
	_executionSteps.push_back( executionStep_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::merge( HExpression& expression_ ) {
	M_PROLOG
	move( expression_._executionSteps.begin(), expression_._executionSteps.end(), back_insert_iterator( _executionSteps ) );
	expression_._executionSteps.clear();
	return;
	M_EPILOG
}

void HHuginn::HExpression::oper( OPERATOR operator_, HFrame* frame_, int position_ ) {
	M_PROLOG
	frame_->operations().emplace( operator_, position_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::close_parenthesis( HFrame* frame_, int position_ ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( ! frame_->values().is_empty() );
	OPERATOR o( frame_->operations().top()._operator );
	M_ASSERT( ( o == OPERATOR::ABSOLUTE ) || ( o == OPERATOR::PARENTHESIS ) );
	frame_->operations().pop();
	if ( o == OPERATOR::ABSOLUTE ) {
		value_t v( frame_->values().top() );
		frame_->values().pop();
		frame_->values().push( HValue::abs( v, position_ ) );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call( yaal::hcore::HString const& name_, HFrame* frame_, int position_ ) {
	M_PROLOG
	frame_->operations().emplace( OPERATOR::FUNCTION_CALL, position_ );
	frame_->values().push( make_pointer<HString>( name_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::get_variable( yaal::hcore::HString const& name_, HFrame* frame_, int position_ ) {
	M_PROLOG
	frame_->values().push( frame_->get_variable( name_, position_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::make_variable( yaal::hcore::HString const& name_, HFrame* frame_, int position_ ) {
	M_PROLOG
	frame_->values().push( frame_->make_variable( name_, position_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::set_variable( HFrame* frame_, int ) {
	M_PROLOG
	while ( ! frame_->operations().is_empty() && ( frame_->operations().top()._operator == OPERATOR::ASSIGN ) ) {
		int p( frame_->operations().top()._position );
		frame_->operations().pop();
		value_t value( frame_->values().top() );
		frame_->values().pop();
		value_t ref( frame_->values().top() );
		frame_->values().pop();
		if ( ref->type() != TYPE::REFERENCE ) {
			M_ASSERT( ref->type() == TYPE::CHARACTER );
			throw HHuginnRuntimeException( "String does not support item assignment.", p );
		}
		static_cast<HReference*>( ref.raw() )->value() = value;
		frame_->values().push( value );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call_exec( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::values_t values;
	while ( frame_->operations().top()._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		frame_->operations().pop();
		M_ASSERT( ! frame_->operations().is_empty() );
		values.push_back( yaal::move( frame_->values().top() ) );
		frame_->values().pop();
		M_ASSERT( ! frame_->values().is_empty() );
	}
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::FUNCTION_CALL );
	M_ASSERT( frame_->values().top()->type() == HHuginn::TYPE::STRING );
	hcore::HString name( yaal::move( static_cast<HString*>( frame_->values().top().raw() )->value() ) );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	frame_->values().pop();
	reverse( values.begin(), values.end() );
	frame_->values().push( _huginn->call( name, values, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::plus( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::PLUS );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( HHuginn::HValue::add( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::minus( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::MINUS );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( HHuginn::HValue::sub( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::mul( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::MULTIPLY );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( HHuginn::HValue::mul( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::div( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::DIVIDE );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( HHuginn::HValue::div( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::mod( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::MODULO );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( HHuginn::HValue::mod( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::negate( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::NEGATE );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	M_ASSERT( ! frame_->values().is_empty() );
	value_t v( frame_->values().top() );
	frame_->values().pop();
	frame_->values().push( HValue::neg( v, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::power( HFrame* frame_, int ) {
	M_PROLOG
	while ( ! frame_->operations().is_empty() && ( frame_->operations().top()._operator == OPERATOR::POWER ) ) {
		int p( frame_->operations().top()._position );
		frame_->operations().pop();
		value_t v2( frame_->values().top() );
		frame_->values().pop();
		value_t v1( frame_->values().top() );
		frame_->values().pop();
		if ( v1->type() != v2->type() ) {
			throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
		}
		frame_->values().push( HHuginn::HValue::pow( v1, v2, p ) );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::subscript( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::SUBSCRIPT );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t index( frame_->values().top() );
	frame_->values().pop();
	value_t base( frame_->values().top() );
	frame_->values().pop();
	if ( index->type() != TYPE::INTEGER ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::IDX_NOT_INT], p );
	}
	frame_->values().push( HHuginn::HValue::subscript( base, index, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::dereference( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->values().is_empty() );
	if ( frame_->values().top()->type() == HHuginn::TYPE::REFERENCE ) {
		value_t v( frame_->values().top() );
		frame_->values().pop();
		frame_->values().push( static_cast<HHuginn::HReference*>( v.raw() )->value() );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::equals( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::EQUALS );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( HHuginn::HValue::equals( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::not_equals( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::NOT_EQUALS );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( ! HHuginn::HValue::equals( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::less( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::LESS );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( HHuginn::HValue::less( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::greater( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::GREATER );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( HHuginn::HValue::greater( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::less_or_equal( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::LESS_OR_EQUAL );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( HHuginn::HValue::less_or_equal( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::greater_or_equal( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::GREATER_OR_EQUAL );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( v1->type() != v2->type() ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_TYPES_NOT_MATCH], p );
	}
	frame_->values().push( make_pointer<HBoolean>( HHuginn::HValue::greater_or_equal( v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_and( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::BOOLEAN_AND );
	frame_->operations().pop();
	value_t v( frame_->values().top() );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( make_pointer<HBoolean>( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_or( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::BOOLEAN_OR );
	frame_->operations().pop();
	value_t v( frame_->values().top() );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( make_pointer<HBoolean>( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_xor( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::BOOLEAN_XOR );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v2( frame_->values().top() );
	frame_->values().pop();
	value_t v1( frame_->values().top() );
	frame_->values().pop();
	if ( ( v1->type() != TYPE::BOOLEAN ) || ( v2->type() != TYPE::BOOLEAN ) ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], p );
	}
	frame_->values().push( HHuginn::HValue::boolean_xor( v1, v2, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_not( HFrame* frame_, int ) {
	M_PROLOG
	M_ASSERT( ! frame_->operations().is_empty() );
	M_ASSERT( frame_->operations().top()._operator == OPERATOR::BOOLEAN_NOT );
	int p( frame_->operations().top()._position );
	frame_->operations().pop();
	value_t v( frame_->values().top() );
	frame_->values().pop();
	if ( v->type() != TYPE::BOOLEAN ) {
		throw HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
	}
	frame_->values().push( HHuginn::HValue::boolean_not( v, p ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_direct( value_t const& value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( value_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_real( double long value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HReal>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_integer( int long long value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HInteger>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_string( yaal::hcore::HString const& value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HString>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_number( yaal::hcore::HString const& value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HNumber>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_character( char value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HCharacter>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_boolean( bool value_, HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( make_pointer<HBoolean>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_none( HFrame* frame_, int ) {
	M_PROLOG
	frame_->values().push( _none_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::do_execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	HFrame* f( thread_->current_frame() );
	for ( execution_step_t const& e : _executionSteps ) {
		e( f );
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
	f->set_result( f->values().top() );
	f->values().pop();
	return;
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

void HHuginn::HScope::do_execute( HHuginn::HThread* thread_ ) const {
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

void HHuginn::HReturn::do_execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	if ( !! _expression ) {
		_expression->execute( thread_ );
	}
	thread_->break_execution( HHuginn::HFrame::STATE::RETURN, thread_->current_frame()->result() );
	return;
	M_EPILOG
}

HHuginn::HBreak::HBreak( void ) {
	return;
}

void HHuginn::HBreak::do_execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->break_execution( HHuginn::HFrame::STATE::BREAK );
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

void HHuginn::HIf::do_execute( HHuginn::HThread* thread_ ) const {
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

void HHuginn::HSwitch::do_execute( HHuginn::HThread* thread_ ) const {
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
				if ( matched || HValue::equals( v, f->result(), it->expression()->position() ) ) {
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

void HHuginn::HWhile::do_execute( HHuginn::HThread* thread_ ) const {
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

void HHuginn::HFor::do_execute( HHuginn::HThread* thread_ ) const {
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
			if ( v->type() == TYPE::REFERENCE ) {
				v = static_cast<HHuginn::HReference*>( it.value().raw() )->value();
			} else {
				M_ASSERT( v->type() == TYPE::CHARACTER );
			}
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
	HHuginn::scope_t const& scope_ )
	: _name( name_ ), _parameterNames( parameterNames_ ), _scope( scope_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunction::execute( HThread* thread_, values_t const& values_, int position_ ) const {
	M_PROLOG
	if ( values_.get_size() != _parameterNames.get_size() ) {
		throw HHuginnRuntimeException(
			"Mismatching number of parameters in call to: `"_ys
			.append( _name ).append( "', expected: " )
			.append( _parameterNames.get_size() ).append( ", got: " )
			.append( values_.get_size() ).append( "." ),
			position_
		);
	}
	parameter_names_t::const_iterator n( _parameterNames.begin() );
	parameter_names_t::const_iterator ne( _parameterNames.end() );
	values_t::const_iterator v( values_.begin() );
	values_t::const_iterator ve( values_.end() );
	thread_->create_function_frame();
	while ( n != ne ) {
		M_ASSERT( v != ve );
		thread_->current_frame()->set_variable( *n, *v, position_ );
		++ n;
		++ v;
	}
	_scope->execute( thread_ );
	HHuginn::value_t res( thread_->current_frame()->result() );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
}

namespace huginn_builtin {

inline HHuginn::value_t convert( HHuginn::TYPE toType_, HHuginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException(
			""_ys
			.append( HHuginn::HValue::type_name( toType_ ) )
			.append( "() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::value_t res;
	switch ( toType_ ) {
		case ( HHuginn::TYPE::INTEGER ): res = HHuginn::HValue::integer( values_.front(), position_ ); break;
		case ( HHuginn::TYPE::REAL ): res = HHuginn::HValue::real( values_.front(), position_ ); break;
		case ( HHuginn::TYPE::STRING ): res = HHuginn::HValue::string( values_.front(), position_ ); break;
		case ( HHuginn::TYPE::NUMBER ): res = HHuginn::HValue::number( values_.front(), position_ ); break;
		case ( HHuginn::TYPE::BOOLEAN ): res = HHuginn::HValue::boolean( values_.front(), position_ ); break;
		case ( HHuginn::TYPE::CHARACTER ): res = HHuginn::HValue::character( values_.front(), position_ ); break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Conversion to `"_ys.append( HHuginn::HValue::type_name( toType_ ) ).append( "' is not supported." ),
				position_
			);
		}
	}
	return ( res );
	M_EPILOG
}

inline HHuginn::value_t size( HHuginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
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
	switch ( v->type() ) {
		case ( HHuginn::TYPE::STRING ): {
			s = static_cast<HHuginn::HString const*>( v )->value().get_length();
		} break;
		case ( HHuginn::TYPE::LIST ): {
			s = static_cast<HHuginn::HList const*>( v )->size();
		} break;
		case ( HHuginn::TYPE::MAP ): {
			s = static_cast<HHuginn::HMap const*>( v )->size();
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Getting size of `"_ys.append( HHuginn::HValue::type_name( v->type() ) ).append( "'s is not supported." ),
				position_
			);
		}
	}
	return ( make_pointer<HHuginn::HInteger>( s ) );
	M_EPILOG
}

inline HHuginn::value_t type( HHuginn::HThread*, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	if ( values_.get_size() != 1 ) {
		throw HHuginn::HHuginnRuntimeException( ""_ys
			.append( "type() expects exactly one parameter, got: " )
			.append( values_.get_size() ),
			position_
		);
	}
	HHuginn::HValue const* v( values_.front().raw() );
	return ( make_pointer<HHuginn::HString>( HHuginn::HValue::type_name( v->type() ) ) );
	M_EPILOG
}

inline HHuginn::value_t list( HHuginn::HThread*, HHuginn::values_t const& values_, int ) {
	M_PROLOG
	HHuginn::value_t v( make_pointer<HHuginn::HList>() );
	HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
	for ( HHuginn::value_t const& e : values_ ) {
		l->push_back( e );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t print( HHuginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) {
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
	switch ( v->type() ) {
		case ( HHuginn::TYPE::INTEGER ): {
			out << static_cast<HHuginn::HInteger const*>( v )->value();
		} break;
		case ( HHuginn::TYPE::REAL ): {
			out << static_cast<HHuginn::HReal const*>( v )->value();
		} break;
		case ( HHuginn::TYPE::STRING ): {
			out << static_cast<HHuginn::HString const*>( v )->value();
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			out << static_cast<HHuginn::HNumber const*>( v )->value();
		} break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			out << static_cast<HHuginn::HBoolean const*>( v )->value();
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			out << static_cast<HHuginn::HCharacter const*>( v )->value();
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Printing `"_ys.append( HHuginn::HValue::type_name( v->type() ) ).append( "'s is not supported." ),
				position_
			);
		}
	}
	out << flush;
	return ( _none_ );
	M_EPILOG
}

inline HHuginn::value_t input( HHuginn::HThread* thread_, HHuginn::values_t const&, int ) {
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
	_functions.insert( make_pair<yaal::hcore::HString const>( "print", hcore::call( &huginn_builtin::print, _1, _2, _3 ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "input", hcore::call( &huginn_builtin::input, _1, _2, _3 ) ) );
	return;
	M_EPILOG
}

}

}


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

#include <cstring>

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

}

executing_parser::HRule HHuginn::make_engine( void ) {
	M_PROLOG
	using namespace executing_parser;
	namespace e_p = executing_parser;
	hcore::HString identifier( "\\<[a-zA-Z_][a-zA-Z0-9_]*\\>" );
	HRule expression( "expression" );
	HRule absoluteValue( "absoluteValue",
		constant( '|', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) )
		>> expression
		>> constant( '|', e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::close_parenthesis ) ) ) );
	HRule parenthesis( "parenthesis",
		constant( '(', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) )
		>> expression
		>> constant( ')', e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::close_parenthesis ) ) ) );
	HRule arg( "argument", expression, HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::add_arg ) ) );
	HRule argList( "argList", arg >> ( * ( ',' >> arg ) ) );
	HRule functionCall(
		"functionCall",
		regex(
			"functionCallIdentifier",
			identifier,
			e_p::HStringLiteral::action_string_t( hcore::call( &HHuginn::OCompiler::defer_function_call, &_compiler, _1 ) )
		) >> '(' >> -argList >> ')',
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::function_call_exec ) )
	);
	HRule variableGetter(
		regex(
			"variableGetter",
			identifier, e_p::HStringLiteral::action_string_t( hcore::call( &HHuginn::OCompiler::defer_get_variable, &_compiler, _1 ) )
		)
	);
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| functionCall
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_t( hcore::call( &HHuginn::OCompiler::defer_store_real, &_compiler, _1 ) )]
		| integer[e_p::HInteger::action_int_long_long_t( hcore::call( &HHuginn::OCompiler::defer_store_integer, &_compiler, _1 ) )]
		| string_literal[e_p::HStringLiteral::action_string_t( hcore::call( &HHuginn::OCompiler::defer_store_string, &_compiler, _1 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_t( hcore::call( &HHuginn::OCompiler::defer_store_character, &_compiler, _1 ) )]
		| variableGetter
	);
	HRule power(
		"power",
		atom >> ( * ( constant( '^', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) ) >> atom ) ),
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::power ) )
	);
	HRule multiplication(
		"multiplication",
		power >> (
			* (
					(
					characters(
						"*/%", e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) )
					) >> power
				)[e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::mul_div_mod ) )]
			)
		)
	);
	HRule sum(
		"sum",
		multiplication >> (
			* (
					(
					characters(
						"+-", e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) )
					) >> multiplication
				)[e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::plus_minus ) )]
			)
		)
	);
	HRule value( "value", sum );
	HRule subscript( "subscript" );
	subscript %= (
		/* subscript base */ ( functionCall | variableGetter ) >>
		/* repeat at least once */ + (
			/* subscript operator it self */ (
				constant( '[', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) ) >> ( value | subscript ) >> ']'
			)[e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::subscript ) )]
		)
	);
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	HRule assignment(
		"assignment",
		* (
			/* make reference */ (
				subscript
				| regex(
					"variableSetter",
					identifier,
					e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_make_variable, &_compiler, _1 ) )
				)
			) >>
			constant( '=' )[e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) )]
		) >> (
			subscript[e_p::HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::dereference ) )]
			| value
		),
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::set_variable ) )
	);
	expression %= assignment;
	HRule booleanExpression( "booleanExpression" );
	HRule booleanValue( "booleanValue", constant( "true" ) | constant( "false" ) | ( constant( '(' ) >> booleanExpression >> ')' ) );
	HRule booleanEquals(
		"booleanEquals",
		expression >> constant( "==", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::equals ) )
	);
	HRule booleanNotEquals(
		"booleanNotEquals",
		expression >> constant( "!=", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::not_equals ) )
	);
	HRule booleanLess(
		"booleanLess",
		expression >> constant( '<', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::less ) )
	);
	HRule booleanGreater(
		"booleanGreater",
		expression >> constant( '>', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::greater ) )
	);
	HRule booleanLessEq(
		"booleanLessEq",
		expression >> constant( "<=", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::less_or_equal ) )
	);
	HRule booleanGreaterEq(
		"booleanGreaterEq",
		expression >> constant( ">=", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> expression,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::greater_or_equal ) )
	);
	HRule booleanAnd(
		"booleanAnd",
		booleanValue >> constant( "&&", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> booleanValue,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::boolean_and ) )
	);
	HRule booleanOr(
		"booleanOr",
		booleanValue >> constant( "||", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> booleanValue,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::boolean_or ) )
	);
	HRule booleanXor(
		"booleanXor",
		booleanValue >> constant( "^^", e_p::HString::action_string_t( hcore::call( &HHuginn::OCompiler::defer_str_oper, &_compiler, _1 ) ) ) >> booleanValue,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::boolean_xor ) )
	);
	HRule booleanNot(
		"booleanNot",
		constant( '!', e_p::HCharacter::action_character_t( hcore::call( &HHuginn::OCompiler::defer_oper, &_compiler, _1 ) ) ) >> booleanValue,
		HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::defer_action, &_compiler, &HHuginn::HExpression::boolean_not ) )
	);
	booleanExpression %= (
		booleanEquals
		| booleanNotEquals
		| booleanLess
		| booleanGreater
		| booleanLessEq
		| booleanGreaterEq
		| booleanAnd
		| booleanOr
		| booleanXor
		| booleanNot
	);
	HRule expressionStatement( "expressionStatement", expression[HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::commit_expression, &_compiler ) )] >> ';' );
	HRule scope( "scope", HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::commit_scope, &_compiler ) ) );
	HRule ifStatement( "ifStatement", executing_parser::constant( "if" ) >> '(' >> booleanExpression >> ')' >> scope >> -( constant( "else" ) >> scope ) );
	HRule continueStatement( "continueStatement", constant( "continue" ) >> ';' );
	/*
	 * TODO:
	 * Allow `break' statement to have literal integer argument
	 * telling how many scopes given `break' should break,
	 * i.e.:
	 * break ( 2 ); // <--- break two levels of nested scopes.
	 */
	HRule breakStatement( "breakStatement", constant( "break" ) >> ';' );
	HRule whileStatement( "whileStatement", constant( "while" ) >> '(' >> booleanExpression >> ')' >> scope );
	HRule variableIdentifier( regex( "variableIdentifier", identifier ) );
	HRule forStatement( "forStatement", constant( "for" ) >> '(' >> variableIdentifier >> ':' >> expression >> ')' >> scope );
	HRule caseStatement( "caseStatement", constant( "case" ) >> '(' >> integer >> ')' >> ':' >> scope >> -breakStatement );
	HRule defaultStatement( "defaultStatement", constant( "default" ) >> ':' >> scope );
	HRule switchStatement( "switchStatement", constant( "switch" ) >> '(' >> expression >> ')' >> '{' >> +caseStatement >> -defaultStatement >> '}' );
	HRule returnStatement( "returnStatement", constant( "return" ) >> -( '(' >> expression >> ')' ) >> ';' );
	HRule statement( "statement",
		ifStatement
		| whileStatement
		| forStatement
		| switchStatement
		| breakStatement
		| continueStatement
		| returnStatement[HRuleBase::action_t( hcore::call( &OCompiler::add_return_statement, &_compiler ) )]
		| expressionStatement
	);
	scope %= ( constant( '{', HRuleBase::action_t( hcore::call( &OCompiler::create_scope, &_compiler ) ) ) >> *statement >> '}' );
	HRule parameter(
		regex(
			"parameter",
			identifier,
			HRegex::action_string_t( hcore::call( &HHuginn::OCompiler::add_paramater, &_compiler, _1 ) )
		)
	);
	HRule nameList( "nameList", parameter >> ( * ( ',' >> parameter ) ) );
	HRule functionDefinition( "functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_t( hcore::call( &HHuginn::OCompiler::set_function_name, &_compiler, _1 ) )
		) >> '(' >> -nameList >> ')' >> scope,
		HRuleBase::action_t( hcore::call( &HHuginn::create_function, this ) )
	);
	HRule huginnGrammar( "huginnGrammar", + functionDefinition );
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
	return ( HIterator( this, _beg ) );
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

HHuginn::OCompiler::OCompiler( HHuginn* huginn_ )
	: _huginn( huginn_ ),
	_functionName(),
	_parameters(),
	_functionScope(),
	_expression( make_pointer<HExpression>( _huginn ) ),
	_scopeStack(),
	_statementList() {
	return;
}

void HHuginn::OCompiler::set_function_name( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginnException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ) );
	}
	_functionName = name_;
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_paramater( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	_parameters.push_back( name_ );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::create_scope( void ) {
	M_PROLOG
	HScope* top( ! _scopeStack.is_empty() ? _scopeStack.top().raw() : nullptr );
	_scopeStack.emplace( make_pointer<HScope>() );
	if ( top ) {
		top->add_statement( _scopeStack.top() );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_scope( void ) {
	M_PROLOG
	scope_t s( _scopeStack.top() );
	_scopeStack.pop();
	if ( _scopeStack.is_empty() ) {
		M_ASSERT( ! _functionScope );
		_functionScope = s;
	} else {
		M_ASSERT( !! _functionScope );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_return_statement( void ) {
	M_PROLOG
	M_ASSERT( ! _scopeStack.is_empty() );
	_scopeStack.top()->add_statement( make_pointer<HReturn>( _expression ) );
	_expression = make_pointer<HExpression>( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_expression( void ) {
	M_PROLOG
	M_ASSERT( ! _scopeStack.is_empty() );
	_scopeStack.top()->add_statement( _expression );
	_expression = make_pointer<HExpression>( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_oper( char operator_ ) {
	M_PROLOG
	HHuginn::HExpression::OPERATOR o( HHuginn::HExpression::OPERATOR::NONE );
	switch ( operator_ ) {
		case ( '+' ): o = HHuginn::HExpression::OPERATOR::PLUS;        break;
		case ( '-' ): o = HHuginn::HExpression::OPERATOR::MINUS;       break;
		case ( '*' ): o = HHuginn::HExpression::OPERATOR::MULTIPLY;    break;
		case ( '/' ): o = HHuginn::HExpression::OPERATOR::DIVIDE;      break;
		case ( '%' ): o = HHuginn::HExpression::OPERATOR::MODULO;      break;
		case ( '^' ): o = HHuginn::HExpression::OPERATOR::POWER;       break;
		case ( '(' ): o = HHuginn::HExpression::OPERATOR::PARENTHESIS; break;
		case ( '|' ): o = HHuginn::HExpression::OPERATOR::ABSOLUTE;    break;
		case ( '=' ): o = HHuginn::HExpression::OPERATOR::ASSIGN;      break;
		case ( '[' ): o = HHuginn::HExpression::OPERATOR::SUBSCRIPT;   break;
		case ( '<' ): o = HHuginn::HExpression::OPERATOR::LESS;        break;
		case ( '>' ): o = HHuginn::HExpression::OPERATOR::GREATER;     break;
		case ( '!' ): o = HHuginn::HExpression::OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	_expression->add_execution_step( hcore::call( &HExpression::oper, _expression.raw(), o ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_str_oper( yaal::hcore::HString const& operator_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, HHuginn::HExpression::OPERATOR> operator_lookup_t;
	static operator_lookup_t const operatorLookup( {
		{ "==", HHuginn::HExpression::OPERATOR::EQUALS },
		{ "!=", HHuginn::HExpression::OPERATOR::NOT_EQUALS },
		{ "<=", HHuginn::HExpression::OPERATOR::LESS_OR_EQUAL },
		{ ">=", HHuginn::HExpression::OPERATOR::GREATER_OR_EQUAL },
		{ "&&", HHuginn::HExpression::OPERATOR::BOOLEAN_AND },
		{ "||", HHuginn::HExpression::OPERATOR::BOOLEAN_OR },
		{ "^^", HHuginn::HExpression::OPERATOR::BOOLEAN_XOR }
	} );
	_expression->add_execution_step( hcore::call( &HExpression::oper, _expression.raw(), operatorLookup.at( operator_ ) ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_action( expression_action_t const& expressionAction_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( expressionAction_, _expression.raw() ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_function_call( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::function_call, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_get_variable( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::get_variable, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_make_variable( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::make_variable, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_real( double long value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::store_real, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_integer( int long long value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::store_integer, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_string( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::store_string, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_store_character( char value_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::store_character, _expression.raw(), value_ ) );
	return;
	M_EPILOG
}

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY ),
	_functions(),
	_engine( make_engine() ),
	_source(),
	_compiler( this ),
	_threads(),
	_argv( new ( memory::yaal ) HList() ),
	_result() {
	M_PROLOG
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
	return ( _engine.parse( _source.begin(), _source.end() ) );
	M_EPILOG
}

void HHuginn::compile( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED );
	_state = STATE::COMPILED;
	_engine.execute();
	return;
	M_EPILOG
}

void HHuginn::execute( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED );
	yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
	threads_t::iterator t( _threads.insert( make_pair( threadId, make_pointer<HThread>( threadId ) ) ).first );
	values_t args;
	if ( _argv->size() > 0 ) {
		args.push_back( _argv );
	}
	_result = call( "main", args );
	_threads.clear();
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::call( yaal::hcore::HString const& name_, values_t const& values_ ) {
	M_PROLOG
	functions_t::const_iterator f( _functions.find( name_ ) );
	value_t res;
	if ( f != _functions.end() ) {
		yaal::hcore::HThread::id_t threadId( hcore::HThread::get_current_thread_id() );
		threads_t::iterator t( _threads.find( threadId ) );
		M_ASSERT( t != _threads.end() );
		res = f->second->execute( t->second.raw(), values_ );
	} else {
		throw HHuginnException( "function `"_ys.append( name_ ).append( "' is not defined" ) );
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
	return ( _source.error_position( _engine.error_position() ) );
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
	HExecutingParser::messages_t const& m( _engine.error_messages() );
	if ( ! m.is_empty() ) {
	message.append( ':' ).append( coord.line() ).append( ':' ).append( coord.column() )
		.append( ": " ).append( m[0] );
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

void HHuginn::create_function( void ) {
	M_PROLOG
	M_ASSERT( ! _compiler._functionName.is_empty() );
	M_ASSERT( !! _compiler._functionScope );
	M_ASSERT( _compiler._scopeStack.is_empty() );
	function_t f( make_pointer<HFunction>( _compiler._functionName, _compiler._parameters, yaal::move( _compiler._functionScope ) ) );
	_functions.insert( make_pair<hcore::HString const, function_t>( yaal::move( _compiler._functionName ), yaal::move( f ) ) );
	_compiler._functionScope.reset();
	_compiler._parameters.clear();
	_compiler._functionName.clear();
	return;
	M_EPILOG
}

HHuginn::HThread::HThread( yaal::hcore::HThread::id_t id_ )
	: _frames(), _id( id_ ) {
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
	_frames.push( make_pointer<HFrame>( parent, true, false ) );
	return;
	M_EPILOG
}

void HHuginn::HThread::create_loop_frame( void ) {
	M_PROLOG
	HHuginn::HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( parent, false, true ) );
	return;
	M_EPILOG
}

void HHuginn::HThread::create_scope_frame( void ) {
	M_PROLOG
	HHuginn::HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( parent, false, false ) );
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
		} else if ( ( state_ == HHuginn::HFrame::STATE::BREAK ) && ( level >= level_ ) ) {
			break;
		}
	}
	if ( target ) {
		target->set_result( value_ );
	}
	return;
	M_EPILOG
}

HHuginn::HFrame::HFrame( HFrame* parent_, bool bump_, bool loop_ )
	: _variables(),
	_result(),
	_number( parent_ ? ( parent_->_number + ( bump_ ? 1 : 0 ) ) : 1 ),
	_parent( parent_ ),
	_loop( loop_ ),
	_state( STATE::NORMAL ) {
	return;
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

HHuginn::value_t& HHuginn::HFrame::get_variable( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	variables_t::iterator it( _variables.find( name_ ) );
	HHuginn::value_t* v( nullptr );
	if ( it != _variables.end() ) {
		v = &it->second;
	} else if ( _parent && ( _parent->_number == _number ) ) {
		v = &_parent->get_variable( name_ );
	} else {
		throw HHuginnException( "variable `"_ys.append( name_ ).append( "' does not exist" ) );
	}
	return ( *v );
	M_EPILOG
}

void HHuginn::HFrame::set_variable( yaal::hcore::HString const& name_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	value_t ref( make_variable( name_ ) );
	static_cast<HReference*>( ref.raw() )->value() = value_;
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HFrame::make_variable( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HFrame* f( this );
	variables_t::iterator it;
	while ( f ) {
		it = _variables.find( name_ );
		if ( it != _variables.end() ) {
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

HHuginn::HValue::HValue( void )
	: _type( TYPE::NONE ), _methods() {
	return;
}

HHuginn::HValue::HValue( TYPE type_ )
	: _type( type_ ), _methods() {
	return;
}

HHuginn::HValue::TYPE HHuginn::HValue::type( void ) const {
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
		"map"
	};
	return ( names[static_cast<int>( type_ )] );
}

HHuginn::value_t HHuginn::HValue::subscript( HHuginn::value_t& base_, HHuginn::value_t const& index_ ) {
	M_ASSERT( index_->type() == TYPE::INTEGER );
	TYPE baseType( base_->type() );
	HInteger const* i( static_cast<HInteger const*>( index_.raw() ) );
	int long long index( i->value() );
	value_t res;
	if ( ( baseType != TYPE::LIST ) && ( baseType != TYPE::STRING ) ) {
		throw HHuginnException( "Subscript is not supported on `"_ys.append( type_name( baseType ) ).append( "'." ) );
	} else if ( baseType == TYPE::LIST ) {
		HList* l( static_cast<HList*>( base_.raw() ) );
		if ( ( index < 0 ) || ( index >= l->size() ) ) {
			throw HHuginnException( "Bad index." );
		}
		res = l->get( index );
	} else {
		M_ASSERT( baseType == TYPE::STRING );
		HString* s( static_cast<HString*>( base_.raw() ) );
		if ( ( index < 0 ) || ( index >= s->value().get_length() ) ) {
			throw HHuginnException( "Bad index." );
		}
		res = make_pointer<HCharacter>( s->value()[static_cast<int>( index )] );
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::add( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
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
			throw HHuginnException( "There is no `+' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::sub( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
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
			throw HHuginnException( "There is no `-' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::mul( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
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
			throw HHuginnException( "There is no `*' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::div( HHuginn::value_t const& v1_, HHuginn::value_t const& v2_ ) {
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
			throw HHuginnException( "There is no `/' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::mod( value_t const& v1_, value_t const& v2_ ) {
	M_ASSERT( v1_->type() == v2_->type() );
	value_t res;
	switch ( v1_->type() ) {
		case ( TYPE::INTEGER ): {
			res = make_pointer<HInteger>( static_cast<HInteger const*>( v1_.raw() )->value() % static_cast<HInteger const*>( v2_.raw() )->value() );
		} break;
		default: {
			throw HHuginnException( "There is no `%' operator for `"_ys.append( type_name( v1_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::pow( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::abs( value_t const& v_ ) {
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
			throw HHuginnException( "There is no |.| operator for `"_ys.append( type_name( v_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::neg( value_t const& v_ ) {
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
			throw HHuginnException( "There is no `negate` operator for `"_ys.append( type_name( v_->type() ) ).append( "'." ) );
		}
	}
	return ( res );
}

bool HHuginn::HValue::equals( value_t const&, value_t const& ) {
	return ( false );
}

bool HHuginn::HValue::less( value_t const&, value_t const& ) {
	return ( false );
}

bool HHuginn::HValue::greater( value_t const&, value_t const& ) {
	return ( false );
}

bool HHuginn::HValue::less_or_equal( value_t const&, value_t const& ) {
	return ( false );
}

bool HHuginn::HValue::greater_or_equal( value_t const&, value_t const& ) {
	return ( false );
}

HHuginn::value_t HHuginn::HValue::boolean_and( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::boolean_or( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::boolean_xor( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::boolean_not( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::string( value_t const& v_ ) {
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
			throw HHuginnException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `string' is not supported." )
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::boolean( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::integer( value_t const& v_ ) {
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
			throw HHuginnException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `integer' is not supported." )
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::real( value_t const& v_ ) {
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
			throw HHuginnException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `real' is not supported." )
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::character( value_t const& v_ ) {
	value_t res;
	switch ( v_->type() ) {
		case ( TYPE::CHARACTER ): {
			res = v_;
		} break;
		case ( TYPE::INTEGER ): {
			res = make_pointer<HCharacter>( static_cast<char>( static_cast<HCharacter const*>( v_.raw() )->value() ) );
		} break;
		default: {
			throw HHuginnException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `character' is not supported." )
			);
		}
	}
	return ( res );
}

HHuginn::value_t HHuginn::HValue::number( value_t const& v_ ) {
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
			throw HHuginnException( "Conversion from `"_ys
				.append( type_name( v_->type() ) )
				.append( "' to `number' is not supported." )
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

HHuginn::HString::HString( yaal::hcore::HString const& value_ )
	: HValue( TYPE::STRING ), _value( value_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HString::value( void ) const {
	return ( _value );
}

yaal::hcore::HString& HHuginn::HString::value( void ) {
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

HHuginn::HIterable::HIterable( TYPE type_ )
	: HValue( type_ ) {
	return;
}

HHuginn::HList::HList( void )
	: HIterable( TYPE::LIST ), _data() {
	return;
}

HHuginn::HMap::HMap( void )
	: HIterable( TYPE::MAP ), _data() {
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

HHuginn::HStatement::HStatement( void ) {
	return;
}

void HHuginn::HStatement::execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	do_execute( thread_ );
	return;
	M_EPILOG
}

HHuginn::HExpression::HExpression( HHuginn* huginn_ )
	: HStatement(),
	_executionSteps(),
	_operations(),
	_values(),
	_huginn( huginn_ ) {
	return;
}

HHuginn::value_t HHuginn::HExpression::result( void ) const {
	M_PROLOG
	return ( ! _values.is_empty() ? _values.top() : _none_ );
	M_EPILOG
}

void HHuginn::HExpression::add_execution_step( HExecutingParser::executor_t const& executionStep_ ) {
	M_PROLOG
	_executionSteps.push_back( executionStep_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::oper( OPERATOR operator_ ) {
	M_PROLOG
	_operations.push( operator_ );
	return;
	M_EPILOG
}

void HHuginn::HExpression::close_parenthesis( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( ( _operations.top() == OPERATOR::ABSOLUTE ) || ( _operations.top() == OPERATOR::PARENTHESIS ) );
	_operations.pop();
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	_operations.push( OPERATOR::FUNCTION_CALL );
	_values.push( make_pointer<HString>( name_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::get_variable( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	_values.push( _huginn->current_frame()->get_variable( name_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::make_variable( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	_values.push( _huginn->current_frame()->make_variable( name_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::set_variable( void ) {
	M_PROLOG
	while ( ! _operations.is_empty() && ( _operations.top() == OPERATOR::ASSIGN ) ) {
		_operations.pop();
		value_t value( _values.top() );
		_values.pop();
		value_t ref( _values.top() );
		_values.pop();
		if ( ref->type() != HValue::TYPE::REFERENCE ) {
			M_ASSERT( ref->type() == HValue::TYPE::CHARACTER );
			throw HHuginnException( "String does not support item assignment." );
		}
		static_cast<HReference*>( ref.raw() )->value() = value;
		_values.push( value );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::add_arg( void ) {
	M_PROLOG
	_operations.push( OPERATOR::FUNCTION_ARGUMENT );
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call_exec( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( !_values.is_empty() );
	HHuginn::values_t values;
	while ( _operations.top() == OPERATOR::FUNCTION_ARGUMENT ) {
		_operations.pop();
		M_ASSERT( !_operations.is_empty() );
		values.push_back( yaal::move( _values.top() ) );
		_values.pop();
		M_ASSERT( !_values.is_empty() );
	}
	M_ASSERT( _operations.top() == OPERATOR::FUNCTION_CALL );
	M_ASSERT( _values.top()->type() == HHuginn::HValue::TYPE::STRING );
	hcore::HString name( yaal::move( static_cast<HString*>( _values.top().raw() )->value() ) );
	_operations.pop();
	_values.pop();
	reverse( values.begin(), values.end() );
	_values.push( _huginn->call( name, values ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::plus_minus( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	OPERATOR op( _operations.top() );
	_operations.pop();
	M_ASSERT( ( op == OPERATOR::PLUS ) || ( op == OPERATOR::MINUS ) );
	value_t v2( _values.top() );
	_values.pop();
	value_t v1( _values.top() );
	_values.pop();
	_values.push( op == OPERATOR::PLUS ? HHuginn::HValue::add( v1, v2 ) : HHuginn::HValue::sub( v1, v2 ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::mul_div_mod( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	OPERATOR op( _operations.top() );
	_operations.pop();
	M_ASSERT( ( op == OPERATOR::MULTIPLY ) || ( op == OPERATOR::DIVIDE ) || ( op == OPERATOR::MODULO ) );
	value_t v2( _values.top() );
	_values.pop();
	value_t v1( _values.top() );
	_values.pop();
	_values.push(
		op == OPERATOR::MULTIPLY
		? HHuginn::HValue::mul( v1, v2 )
		: (
			op == OPERATOR::DIVIDE
			? HHuginn::HValue::div( v1, v2 )
			: HHuginn::HValue::mod( v1, v2 )
		)
	);
	return;
	M_EPILOG
}

void HHuginn::HExpression::power( void ) {
	M_PROLOG
	while ( ! _operations.is_empty() && ( _operations.top() == OPERATOR::POWER ) ) {
		_operations.pop();
		value_t v2( _values.top() );
		_values.pop();
		value_t v1( _values.top() );
		_values.pop();
		_values.push( HHuginn::HValue::pow( v1, v2 ) );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::subscript( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::SUBSCRIPT );
	_operations.pop();
	value_t index( _values.top() );
	_values.pop();
	value_t base( _values.top() );
	_values.pop();
	_values.push( HHuginn::HValue::subscript( base, index ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::dereference( void ) {
	M_PROLOG
	M_ASSERT( ! _values.is_empty() );
	if ( _values.top()->type() == HHuginn::HValue::TYPE::REFERENCE ) {
		value_t v( _values.top() );
		_values.pop();
		_values.push( static_cast<HHuginn::HReference*>( v.raw() )->value() );
	}
	return;
	M_EPILOG
}

void HHuginn::HExpression::equals( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::EQUALS );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( HHuginn::HValue::equals( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::not_equals( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::NOT_EQUALS );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( ! HHuginn::HValue::equals( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::less( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::LESS );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( HHuginn::HValue::less( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::greater( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::GREATER );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( HHuginn::HValue::greater( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::less_or_equal( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::LESS_OR_EQUAL );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( HHuginn::HValue::less_or_equal( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::greater_or_equal( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::GREATER_OR_EQUAL );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( make_pointer<HBoolean>( HHuginn::HValue::greater_or_equal( v1, v2 ) ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_and( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::BOOLEAN_AND );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( HHuginn::HValue::boolean_and( v1, v2 ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_or( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::BOOLEAN_OR );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( HHuginn::HValue::boolean_or( v1, v2 ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_xor( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::BOOLEAN_XOR );
	_operations.pop();
	value_t v1( _values.top() );
	_values.pop();
	value_t v2( _values.top() );
	_values.pop();
	_values.push( HHuginn::HValue::boolean_xor( v1, v2 ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::boolean_not( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( _operations.top() == OPERATOR::BOOLEAN_NOT );
	_operations.pop();
	value_t v( _values.top() );
	_values.pop();
	_values.push( HHuginn::HValue::boolean_not( v ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_real( double long value_ ) {
	M_PROLOG
	_values.push( make_pointer<HReal>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_integer( int long long value_ ) {
	M_PROLOG
	_values.push( make_pointer<HInteger>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_string( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_values.push( make_pointer<HString>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::store_character( char value_ ) {
	M_PROLOG
	_values.push( make_pointer<HCharacter>( value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::do_execute( HHuginn::HThread* thread_ ) const {
	M_PROLOG
	for ( HExecutingParser::executor_t const& e : _executionSteps ) {
		e();
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
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
	for ( HHuginn::statement_t const& s : _statements ) {
		s->execute( thread_ );
		if ( ! thread_->can_continue() ) {
			break;
		}
	}
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
	thread_->break_execution( HHuginn::HFrame::STATE::RETURN, !! _expression ? _expression->result() : HHuginn::value_t() );
	return;
	M_EPILOG
}

HHuginn::HIf::HIf( boolean_expression_t condition_,
		HExecutingParser::executor_t ifClause_,
		HExecutingParser::executor_t elseClause_ )
	: HStatement(), _condition( condition_ ), _ifClause( ifClause_ ), _elseClause( elseClause_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunctionInterface::execute( yaal::tools::HHuginn::HThread * thread_, values_t  const& values_ ) const {
	M_PROLOG
	return ( do_execute( thread_, values_ ) );
	M_EPILOG
}

HHuginn::HFunction::HFunction( yaal::hcore::HString const& name_,
	parameter_names_t const& parameterNames_,
	HHuginn::scope_t const& scope_ )
	: _name( name_ ), _parameterNames( parameterNames_ ), _scope( scope_ ) {
	return;
}

HHuginn::value_t HHuginn::HFunction::do_execute( HThread* thread_, values_t const& values_ ) const {
	M_PROLOG
	if ( values_.get_size() != _parameterNames.get_size() ) {
		throw HHuginnException( "Mismatching number of parameters in call to: `"_ys
			.append( _name ).append( "', expected: " )
			.append( _parameterNames.get_size() ).append( ", got: " )
			.append( values_.get_size() ).append( "." ) );
	}
	parameter_names_t::const_iterator n( _parameterNames.begin() );
	parameter_names_t::const_iterator ne( _parameterNames.end() );
	values_t::const_iterator v( values_.begin() );
	values_t::const_iterator ve( values_.end() );
	thread_->create_function_frame();
	while ( n != ne ) {
		M_ASSERT( v != ve );
		thread_->current_frame()->set_variable( *n, *v );
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

class HConversion : public HHuginn::HFunctionInterface {
public:
	typedef HConversion this_type;
	typedef HHuginn::HFunctionInterface base_type;
private:
	HHuginn::HValue::TYPE _toType;
public:
	HConversion( HHuginn::HValue::TYPE toType_ )
		: _toType( toType_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_execute( HHuginn::HThread*, HHuginn::values_t const& values_ ) const {
		M_PROLOG
		if ( values_.get_size() != 1 ) {
			throw HHuginnException( ""_ys
				.append( HHuginn::HValue::type_name( _toType ) )
				.append( "() expects exactly one parameter, got: " )
				.append( values_.get_size() )
			);
		}
		HHuginn::value_t res;
		switch ( _toType ) {
			case ( HHuginn::HValue::TYPE::INTEGER ): res = HHuginn::HValue::integer( values_.front() ); break;
			case ( HHuginn::HValue::TYPE::REAL ): res = HHuginn::HValue::real( values_.front() ); break;
			case ( HHuginn::HValue::TYPE::STRING ): res = HHuginn::HValue::string( values_.front() ); break;
			case ( HHuginn::HValue::TYPE::NUMBER ): res = HHuginn::HValue::number( values_.front() ); break;
			case ( HHuginn::HValue::TYPE::BOOLEAN ): res = HHuginn::HValue::boolean( values_.front() ); break;
			case ( HHuginn::HValue::TYPE::CHARACTER ): res = HHuginn::HValue::character( values_.front() ); break;
			default: {
				throw HHuginnException( "Conversion to `"_ys.append( HHuginn::HValue::type_name( _toType ) ).append( "' is not supported." ) );
			}
		}
		return ( res );
		M_EPILOG
	}
};

class HPrint : public HHuginn::HFunctionInterface {
public:
	typedef HPrint this_type;
	typedef HHuginn::HFunctionInterface base_type;
protected:
	virtual HHuginn::value_t do_execute( HHuginn::HThread*, HHuginn::values_t const& values_ ) const {
		M_PROLOG
		if ( values_.get_size() != 1 ) {
			throw HHuginnException( ""_ys
				.append( "print() expects exactly one parameter, got: " )
				.append( values_.get_size() )
			);
		}
		HHuginn::HValue const* v( values_.front().raw() );
		switch ( v->type() ) {
			case ( HHuginn::HValue::TYPE::INTEGER ): {
				cout << static_cast<HHuginn::HInteger const*>( v )->value();
			} break;
			case ( HHuginn::HValue::TYPE::REAL ): {
				cout << static_cast<HHuginn::HReal const*>( v )->value();
			} break;
			case ( HHuginn::HValue::TYPE::STRING ): {
				cout << static_cast<HHuginn::HString const*>( v )->value();
			} break;
			case ( HHuginn::HValue::TYPE::NUMBER ): {
				cout << static_cast<HHuginn::HNumber const*>( v )->value();
			} break;
			case ( HHuginn::HValue::TYPE::BOOLEAN ): {
				cout << static_cast<HHuginn::HBoolean const*>( v )->value();
			} break;
			case ( HHuginn::HValue::TYPE::CHARACTER ): {
				cout << static_cast<HHuginn::HCharacter const*>( v )->value();
			} break;
			default: {
				throw HHuginnException( "printing `"_ys.append( HHuginn::HValue::type_name( v->type() ) ).append( "'s is not supported." ) );
			}
		}
		cout << flush;
		return ( _none_ );
		M_EPILOG
	}
};

}

void HHuginn::register_builtins( void ) {
	M_PROLOG
	_functions.insert( make_pair<yaal::hcore::HString const>( "integer", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::INTEGER ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "real", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::REAL ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "string", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::STRING ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "number", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::NUMBER ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "boolean", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::BOOLEAN ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "character", make_pointer<huginn_builtin::HConversion>( HValue::TYPE::CHARACTER ) ) );
	_functions.insert( make_pair<yaal::hcore::HString const>( "print", make_pointer<huginn_builtin::HPrint>() ) );
	return;
	M_EPILOG
}

}

}


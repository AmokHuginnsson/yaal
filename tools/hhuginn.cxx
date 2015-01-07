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
	"file",
	"integer",
	"list",
	"map",
	"number",
	"real",
	"string",
	"print"
}};

bool is_restricted( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) );
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
	HRule variableIdentifier( regex( "variableIdentifier", identifier ) );
	HRule atom( "atom",
		absoluteValue
		| parenthesis
		| functionCall
		| real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_double_long_t( hcore::call( &HHuginn::OCompiler::defer_store_real, &_compiler, _1 ) )]
		| integer[e_p::HInteger::action_int_long_long_t( hcore::call( &HHuginn::OCompiler::defer_store_integer, &_compiler, _1 ) )]
		| string_literal[e_p::HStringLiteral::action_string_t( hcore::call( &HHuginn::OCompiler::defer_store_string, &_compiler, _1 ) )]
		| character_literal[e_p::HCharacterLiteral::action_character_t( hcore::call( &HHuginn::OCompiler::defer_store_character, &_compiler, _1 ) )]
		| variableIdentifier
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
	HRule ref( "ref", value >> *( '[' >> value >> ']' ) );
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	HRule assignment( "assignment", *( variableIdentifier >> '=' ) >> ref );
	expression %= assignment;
	HRule booleanExpression( "booleanExpression" );
	HRule booleanValue( "booleanValue", constant( "true" ) | constant( "false" ) | constant( '(' ) >> booleanExpression >> ')' );
	HRule booleanEquals( "booleanEquals", expression >> "==" >> expression );
	HRule booleanNotEquals( "booleanNotEquals", expression >> "!=" >> expression );
	HRule booleanLess( "booleanLess", expression >> "<" >> expression );
	HRule booleanGreater( "booleanGreater", expression >> ">" >> expression );
	HRule booleanLessEq( "booleanLessEq", expression >> "<=" >> expression );
	HRule booleanGreaterEq( "booleanGreaterEq", expression >> ">=" >> expression );
	HRule booleanAnd( "booleanAnd", booleanValue >> "&&" >> booleanValue );
	HRule booleanOr( "booleanOr", booleanValue >> "||" >> booleanValue );
	HRule booleanXor( "booleanXor", booleanValue >> "^^" >> booleanValue );
	HRule booleanNot( "booleanNot", constant( '!' ) >> booleanValue );
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
	HRule expressionList( "expressionList", + ( expression[HRuleBase::action_t( hcore::call( &HHuginn::OCompiler::commit_expression, &_compiler ) )] >> ';' ) );
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
		| expressionList
	);
	scope %= ( constant( '{', HRuleBase::action_t( hcore::call( &OCompiler::create_scope, &_compiler ) ) ) >> *statement >> '}' );
	HRule nameList( "nameList", variableIdentifier >> ( * ( ',' >> variableIdentifier ) ) );
	HRule functionDefinition( "functionDefinition",
		regex(
			"functionDefinitionIdentifier",
			identifier,
			e_p::HRegex::action_string_t( hcore::call( &HHuginn::OCompiler::set_function_name, &_compiler, _1 ) )
		) >> '(' >> -nameList >> ')' >> scope,
		HRuleBase::action_t( hcore::call( &HHuginn::create_function, this ) ) );
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

void HHuginn::OCompiler::create_scope( void ) {
	M_PROLOG
	HScope* top( ! _scopeStack.is_empty() ? _scopeStack.top().raw() : nullptr );
	if ( top ) {
		M_ASSERT( _functionScope );
		_scopeStack.emplace( top );
		top->add_statement( _scopeStack.top() );
	} else {
		M_ASSERT( ! _functionScope );
		_functionScope = make_pointer<HFunction>( _functionName );
		_scopeStack.emplace( _functionScope );
	}
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_scope( void ) {
	M_PROLOG
	_scopeStack.pop();
	return;
	M_EPILOG
}

void HHuginn::OCompiler::add_return_statement( void ) {
	M_PROLOG
	M_ASSERT( ! _scopeStack.is_empty() && ( !! _functionScope ) );
	_scopeStack.top()->add_statement( make_pointer<HReturn>( _expression, _huginn ) );
	_expression = make_pointer<HExpression>( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::commit_expression( void ) {
	M_PROLOG
	M_ASSERT( ! _scopeStack.is_empty() && ( !! _functionScope ) );
	_scopeStack.top()->add_statement( _expression );
	_expression = make_pointer<HExpression>( _huginn );
	return;
	M_EPILOG
}

void HHuginn::OCompiler::defer_oper( char oper_ ) {
	M_PROLOG
	_expression->add_execution_step( hcore::call( &HExpression::oper, _expression.raw(), oper_ ) );
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
	M_ENSURE( ! _compiler._functionName.is_empty() );
	M_ENSURE( _compiler._scopeStack.is_empty() );
	_functions.insert( make_pair<hcore::HString const, scope_t>( yaal::move( _compiler._functionName ), yaal::move( _compiler._functionScope ) ) );
	_compiler._functionScope.reset();
	_compiler._functionName.clear();
	return;
	M_EPILOG
}

HHuginn::HThread::HThread( yaal::hcore::HThread::id_t id_ )
	: _frames(), _id( id_ ) {
	return;
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
	if ( f ) {
		it->second = value_;
	} else {
		_variables.insert( make_pair( name_, value_ ) );
	}
	return;
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
		"integer",
		"real",
		"string",
		"character",
		"number",
		"list",
		"map"
	};
	return ( names[static_cast<int>( type_ )] );
}

HHuginn::value_t HHuginn::HValue::add( HHuginn::value_t const&, HHuginn::value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::sub( HHuginn::value_t const&, HHuginn::value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::mul( HHuginn::value_t const&, HHuginn::value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::div( HHuginn::value_t const&, HHuginn::value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::mod( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::pow( value_t const&, value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::abs( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::neg( value_t const& ) {
	return ( value_t() );
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

HHuginn::value_t HHuginn::HValue::string( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::integer( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::real( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::character( value_t const& ) {
	return ( value_t() );
}

HHuginn::value_t HHuginn::HValue::number( value_t const& ) {
	return ( value_t() );
}

HHuginn::HReal::HReal( double long value_ )
	: HValue( TYPE::REAL ), _value( value_ ) {
	return;
}

double long HHuginn::HReal::value( void ) const {
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

void HHuginn::HExpression::oper( char operator_ ) {
	M_PROLOG
	OPERATORS o( OPERATORS::NONE );
	switch ( operator_ ) {
		case ( '+' ): o = OPERATORS::PLUS;        break;
		case ( '-' ): o = OPERATORS::MINUS;       break;
		case ( '*' ): o = OPERATORS::MULTIPLY;    break;
		case ( '/' ): o = OPERATORS::DIVIDE;      break;
		case ( '%' ): o = OPERATORS::MODULO;      break;
		case ( '^' ): o = OPERATORS::POWER;       break;
		case ( '(' ): o = OPERATORS::PARENTHESIS; break;
		case ( '|' ): o = OPERATORS::ABSOLUTE;    break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	_operations.push( o );
	return;
	M_EPILOG
}

void HHuginn::HExpression::close_parenthesis( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( ( _operations.top() == OPERATORS::ABSOLUTE ) || ( _operations.top() == OPERATORS::PARENTHESIS ) );
	_operations.pop();
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	_operations.push( OPERATORS::FUNCTION_CALL );
	_values.push( make_pointer<HString>( name_ ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::add_arg( void ) {
	M_PROLOG
	_operations.push( OPERATORS::FUNCTION_ARGUMENT );
	return;
	M_EPILOG
}

void HHuginn::HExpression::function_call_exec( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	M_ASSERT( !_values.is_empty() );
	HHuginn::values_t values;
	while ( _operations.top() == OPERATORS::FUNCTION_ARGUMENT ) {
		_operations.pop();
		M_ASSERT( !_operations.is_empty() );
		values.push_back( yaal::move( _values.top() ) );
		_values.pop();
		M_ASSERT( !_values.is_empty() );
	}
	M_ASSERT( _operations.top() == OPERATORS::FUNCTION_CALL );
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
	OPERATORS op( _operations.top() );
	_operations.pop();
	M_ASSERT( ( op == OPERATORS::PLUS ) || ( op == OPERATORS::MINUS ) );
	value_t v2( _values.top() );
	_values.pop();
	value_t v1( _values.top() );
	_values.pop();
	_values.push( op == OPERATORS::PLUS ? HHuginn::HValue::add( v1, v2 ) : HHuginn::HValue::sub( v1, v2 ) );
	return;
	M_EPILOG
}

void HHuginn::HExpression::mul_div_mod( void ) {
	M_PROLOG
	M_ASSERT( !_operations.is_empty() );
	OPERATORS op( _operations.top() );
	_operations.pop();
	M_ASSERT( ( op == OPERATORS::MULTIPLY ) || ( op == OPERATORS::DIVIDE ) || ( op == OPERATORS::MODULO ) );
	value_t v2( _values.top() );
	_values.pop();
	value_t v1( _values.top() );
	_values.pop();
	_values.push(
		op == OPERATORS::MULTIPLY
		? HHuginn::HValue::mul( v1, v2 )
		: (
			op == OPERATORS::DIVIDE
			? HHuginn::HValue::div( v1, v2 )
			: HHuginn::HValue::mod( v1, v2 )
		)
	);
	return;
	M_EPILOG
}

void HHuginn::HExpression::power( void ) {
	M_PROLOG
	while ( ! _operations.is_empty() && ( _operations.top() == OPERATORS::POWER ) ) {
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

HHuginn::HScope::HScope( HScope* parent_ )
	: _statements(), _parent( parent_ ) {
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

HHuginn::HReturn::HReturn( HHuginn::expression_t const& expression_, HHuginn* huginn_ )
	: _expression( expression_ ), _huginn( huginn_ ) {
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

HHuginn::HFunction::HFunction( yaal::hcore::HString const& name_ )
	: HScope( nullptr ), _name( name_ ), _argumentNames() {
	return;
}

HHuginn::value_t HHuginn::HFunction::execute( HThread* thread_, values_t const& values_ ) const {
	M_PROLOG
	if ( values_.get_size() != _argumentNames.get_size() ) {
		throw HHuginnException( "Mismatching number of parameters in call to: `"_ys
			.append( _name ).append( "', expected: " )
			.append( _argumentNames.get_size() ).append( ", got: " )
			.append( values_.get_size() ).append( "." ) );
	}
	argument_names_t::const_iterator n( _argumentNames.begin() );
	argument_names_t::const_iterator ne( _argumentNames.end() );
	values_t::const_iterator v( values_.begin() );
	values_t::const_iterator ve( values_.end() );
	thread_->create_function_frame();
	while ( n != ne ) {
		M_ASSERT( v != ve );
		thread_->current_frame()->set_variable( *n, *v );
		++ n;
		++ v;
	}
	execute( thread_ );
	HHuginn::value_t res( thread_->current_frame()->result() );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
}

}

}


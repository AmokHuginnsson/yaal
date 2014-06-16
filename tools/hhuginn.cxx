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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"

#include "hcore/system.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace executing_parser {

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

HRule huginn_grammar( void );
HRule huginn_grammar( void ) {
	M_PROLOG
	HRule name( "name", regex( "\\<[a-zA-Z_][a-zA-Z0-9_]*\\>" ) );
	HRule expression( "expression" );
	HRule absoluteValue( "absoluteValue", '|' >> expression >> '|' );
	HRule parenthesis( "parenthesis", '(' >> expression >> ')' );
	HRule argList( "argList", expression >> ( * ( ',' >> expression ) ) );
	HRule functionCall( "functionCall", name >> '(' >> -argList >> ')' );
	HRule atom( "atom", absoluteValue | parenthesis | functionCall | real | integer | string_literal | character_literal | name );
	HRule power( "power", atom >> ( * ( '^' >> atom ) ) );
	HRule multiplication( "multiplication", power >> ( * ( '*' >> power ) ) );
	HRule sum( "sum", multiplication >> ( * ( '+' >> multiplication ) ) );
	HRule value( "value", sum );
	HRule ref( "ref", value >> *( '[' >> value >> ']' ) );
	/*
	 * Assignment shall work only as aliasing.
	 * In other words you cannot modify value of referenced object
	 * with assignment. You can only change where a reference points to.
	 */
	HRule assignment( "assignment", *( name >> '=' ) >> ref );
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
	booleanExpression %= ( booleanEquals | booleanNotEquals | booleanLess | booleanGreater | booleanLessEq | booleanGreaterEq | booleanAnd | booleanOr | booleanXor | booleanNot );
	HRule expressionList( "expressionList", + ( expression >> ';' ) );
	HRule scope( "scope" );
	HRule loopScope( "loopScope" );
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
	HRule whileStatement( "whileStatement", constant( "while" ) >> '(' >> booleanExpression >> ')' >> loopScope );
	HRule foreachStatement( "foreachStatement", constant( "foreach" ) >> '(' >> name >> ':' >> expression >> ')' >> loopScope );
	HRule caseStatement( "caseStatement", constant( "case" ) >> '(' >> integer >> ')' >> ':' >> scope >> -breakStatement );
	HRule defaultStatement( "defaultStatement", constant( "default" ) >> ':' >> scope );
	HRule switchStatement( "switchStatement", constant( "switch" ) >> '(' >> expression >> ')' >> '{' >> +caseStatement >> -defaultStatement >> '}' );
	HRule returnStatement( "returnStatement", constant( "return" ) >> '(' >> expression >> ')' >> ';' );
	HRule statement( "statement",
			ifStatement | whileStatement | foreachStatement | switchStatement | returnStatement | expressionList );
	HRule loopStatement( "loopStatement",
			ifStatement | whileStatement | foreachStatement | switchStatement | breakStatement | continueStatement | returnStatement | expressionList );
	scope %= ( '{' >> *statement >> '}' );
	loopScope %= ( '{' >> *loopStatement >> '}' );
	HRule nameList( "nameList", name >> ( * ( ',' >> name ) ) );
	HRule functionDefinition( "functionDefinition", name >> '(' >> -nameList >> ')' >> scope );
	HRule huginnGrammar( "huginnGrammar", + functionDefinition );
	return ( huginnGrammar );
	M_EPILOG
}

}

HHuginn::HHuginn( void )
	: _state( STATE::EMPTY ), _functions(),
	_engine( executing_parser::huginn_grammar() ),
	_sourceName(), _source(), _sourceSize( 0 ),
	_preprocessedSource(), _preprocessedSourceSize( 0 ),
	_arguments( new HList() ) {
}

void HHuginn::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	M_ENSURE( _state == STATE::EMPTY );
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int nRead( 0 );
	int block( 0 );
	M_ASSERT( _sourceSize == 0 );
	do {
		_source.realloc( ( block + 1 ) * PAGE_SIZE );
		nRead = static_cast<int>( stream_.read( _source.get<char>() + block * PAGE_SIZE, PAGE_SIZE ) );
		M_ENSURE( nRead >= 0 );
		_sourceSize += nRead;
		++ block;
	} while ( nRead == PAGE_SIZE );
	_state = STATE::LOADED;
	return;
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
	static char const NEWLINE = '\n';
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
		: _owner( it_._owner ), _cur( it_._cur ), _state( it_._state ), _escape( false ) {
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_cur = it_._cur;
			_state = it_._state;
			_escape = it_._escape;
		}
		return ( *this );
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

HPrepocessor::HIterator HPrepocessor::begin( void ) const {
	return ( HIterator( this, _beg ) );
}
HPrepocessor::HIterator HPrepocessor::end( void ) const {
	return ( HIterator( this, _end ) );
}

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::skip_comment( yaal::hcore::HString::const_iterator pos_ ) {
	yaal::hcore::HString::const_iterator pos( pos_ );
	while ( true ) {
		if ( *pos == COMMENT_START_CHAR1 ) {
			/* we are possibly in comment, lets check next character */
			++ pos;
			if ( pos == _owner->_end ) {
				/* We are at end of source, we should not update _cur */
				pos = pos_;
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
				continue;
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

/*
 * Strip C-style comments, concatenate literal strings.
 */
void HHuginn::preprocess( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::LOADED );
	_preprocessedSource.realloc( _sourceSize );
	HPrepocessor pp( _source.get<char>(), _source.get<char>() + _sourceSize );
	char* dst( _preprocessedSource.get<char>() );
	for ( HPrepocessor::HIterator it( pp.begin() ), end( pp.end() ); it != end; ++ it, ++ dst )
		*dst = *it;
	_preprocessedSourceSize = static_cast<int>( dst - _preprocessedSource.get<char>() );
	_state = STATE::PREPROCESSED;
	return;
	M_EPILOG
}

void HHuginn::parse( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PREPROCESSED );
	_state = STATE::PARSED;
	return;
	M_EPILOG
}

void HHuginn::compile( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::PARSED );
	_state = STATE::COMPILED;
	return;
	M_EPILOG
}

void HHuginn::execute( void ) {
	M_PROLOG
	M_ENSURE( _state == STATE::COMPILED );
	call( "main" );
	return;
	M_EPILOG
}

void HHuginn::add_argument( yaal::hcore::HString const& arg_ ) {
	M_PROLOG
	_arguments->push_back( value_t( new HString( arg_ ) ) );
	return;
	M_EPILOG
}

void HHuginn::call( yaal::hcore::HString const& ) {
	M_PROLOG
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
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int totalWritten( 0 );
	do {
		int nWritten( static_cast<int>( stream_.write( _preprocessedSource.get<char>() + totalWritten,
						_preprocessedSourceSize - totalWritten >= PAGE_SIZE ? PAGE_SIZE : _preprocessedSourceSize - totalWritten ) ) );
		M_ENSURE( nWritten >= 0 );
		totalWritten += nWritten;
	} while ( totalWritten < _preprocessedSourceSize );
	return;
	M_EPILOG
}

HHuginn::HString::HString( yaal::hcore::HString const& value_ )
	: HValue(), _value( value_ ) {
	return;
}

HHuginn::HList::HList( void )
	: HIterable(), _data() {
	return;
}

void HHuginn::HList::push_back( HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.push_back( value_ );
	return;
	M_EPILOG
}

HHuginn::HScope::HScope( HScope* parent_ )
	: _variables(), _parent( parent_ ) {
}

HHuginn::HIf::HIf( boolean_expression_t condition_,
		HExecutingParser::executor_t ifClause_,
		HExecutingParser::executor_t elseClause_ )
	: HScope( NULL ), _condition( condition_ ), _ifClause( ifClause_ ), _elseClause( elseClause_ ) {
}

}

}


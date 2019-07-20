/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/algorithm.hxx"
#include "preprocessor.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

/*
 * Try to skip leading:
 * - shell comments                (# Some comment)
 * - empty lines                   ([:white:]*)
 * - shell interpreter spawner     (exec huginn -E "${0}" "${@}")
 * - Huginn preprocessor directive (#! huginn opt1=val1 opt2=val2)
 */
HPrepocessor::HIterator HPrepocessor::begin( void ) {
	M_PROLOG
	HString const whitespace( character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	HString const spawner( "exec huginn " );
	HString::const_iterator cur( _beg );
	HString line;
	while ( ! _isKilled_ ) {
		HString::const_iterator eol( find( cur, _end, NEWLINE ) );
		line.assign( cur, eol );
		if (
			( line.find_other_than( whitespace ) != HString::npos )
			&& ( line.front() != '#'_ycp )
			&& (
				( line.get_length() < spawner.get_length() )
				|| ( spawner.compare( line, 0, spawner.get_length() ) != 0 )
			)
		) {
			break;
		}
		cur = eol;
		if ( cur == _end ) {
			break;
		}
		++ cur;
	}
	return ( HIterator( this, cur ) );
	M_EPILOG
}

HPrepocessor::HIterator HPrepocessor::end( void ) {
	return ( HIterator( this, _end ) );
}

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::skip_comment( yaal::hcore::HString::const_iterator pos_ ) {
	yaal::hcore::HString::const_iterator pos( pos_ );
	yaal::hcore::HString::const_iterator goodPos( pos_ );
	while ( ! _isKilled_ ) {
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
		yaal::hcore::HString::const_iterator preSkip( pos );
		pos = skip_comment( pos );
		_owner->_comment.assign( preSkip, pos );
		if ( pos != _owner->_end ) {
			if ( *pos == SINGLE_QUOTE ) {
				_state = STATE::IN_SINGLE_QUOTE;
			} else if ( *pos == DOUBLE_QUOTE ) {
				_state = STATE::IN_DOUBLE_QUOTE;
			}
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

yaal::hcore::HString::const_iterator HPrepocessor::HIterator::try_skip_whitespace( yaal::hcore::HString::const_iterator pos_, code_point_t quote_ ) {
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

}

}

}


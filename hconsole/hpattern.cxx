/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hpattern.hxx"
#include "hcore/harray.hxx"
#include "hcore/hchunk.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

struct OPatternState {
	bool _ignoreCase;
	bool _extended;
	HPattern::pluggable_flags_t _flags;
	OPatternState( void )
		: _ignoreCase( false )
		, _extended( false )
		, _flags() {
	}
};

HPattern::HPattern( void )
	: _initialized( false )
	, _ignoreCaseDefault( false )
	, _ignoreCase( false )
	, _extended( false )
	, _simpleMatchLength( 0 )
	, _regex()
	, _pattern()
	, _errorCause() {
	M_PROLOG
	return;
	M_EPILOG
}

HPattern::~HPattern( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HPattern::save_state( void* sp, pluggable_flags_t* f ) {
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	s._ignoreCase = _ignoreCase;
	s._extended = _extended;
	if ( f ) {
		s._flags = *f;
	}
	return;
	M_EPILOG
}

void HPattern::restore_state( void* sp, pluggable_flags_t* f ) {
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	_ignoreCase = s._ignoreCase;
	_extended = s._extended;
	if ( f ) {
		*f = s._flags;
	}
	return;
	M_EPILOG
}

bool HPattern::parse( HString const& pattern_, pluggable_flags_t* externalFlags ) {
	M_PROLOG
	_errorCause.clear();

/* clear all flags */
	_ignoreCase = _ignoreCaseDefault;
	_extended = false;
	if ( externalFlags ) {
		for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it ) {
			*it->second = false;
		}
	}

	OPatternState savePoint;
	save_state( &savePoint, externalFlags );

	bool ok( true );
	_pattern = pattern_;
	do {
		int stop( static_cast<int>( _pattern.get_length() ) );
		for ( int idx( 0 ); idx < stop; ++ idx ) {
			if ( _pattern[idx] == '\\' ) {
				_pattern.erase( idx, 1 );
				-- stop;
				if ( idx == stop ) {
					_errorCause = _( "dangling escape" );
					ok = false;
					break;
				}
				++ idx;
			} else if ( _pattern[idx] == '/' ) {
				stop = idx;
				break;
			}
		}
		if ( ! ok ) {
			break;
		}

		if ( stop < 1 ) {
			_errorCause = _( "empty pattern" );
			ok = false;
			break;
		}

		for ( int idx( stop + 1 ), LEN( static_cast<int>( _pattern.get_length() ) ); idx < LEN; ++ idx ) {
			if ( ! ( is_ascii( pattern_[ idx ] ) && set_switch( static_cast<char>( pattern_[ idx ].get() ), externalFlags ) ) ) {
				restore_state( &savePoint, externalFlags );
				_errorCause.assign( "bad search option '" ).append( pattern_[ idx ] ).append( "'" );
				ok = false;
				break;
			}
		}
		if ( ! ok ) {
			break;
		}
		_pattern.erase( stop );
		_simpleMatchLength = static_cast<int>( _pattern.get_length() );
		if ( _simpleMatchLength < 1 ) {
			_errorCause = _( "empty pattern" );
			ok = false;
		}
	} while ( false );
	if ( ok && _extended ) {
		ok = parse_re( _pattern );
	}
	return ok;
	M_EPILOG
}

bool HPattern::parse_re( HString const& pattern_ ) {
	M_PROLOG
	_errorCause.clear();
	if ( _regex.compile( pattern_, _ignoreCase ? HRegex::COMPILE::IGNORE_CASE : HRegex::COMPILE::DEFAULT ) ) {
		_extended = true;
		_simpleMatchLength = static_cast<int>( pattern_.get_length() );
	}
	return ( _regex.error_code() == 0 );
	M_EPILOG
}

HString const& HPattern::error( void ) const {
	return ( _regex.error_code() ? _regex.error() : _errorCause );
}

int HPattern::error_code( void ) const {
	return ( _regex.error_code() );
}

bool HPattern::set_switch( char const switch_, pluggable_flags_t* externalFlags ) {
	M_PROLOG
	bool ok( true );
	switch ( switch_ ) {
		case ( 'i' ): { _ignoreCase = ! _ignoreCase; break; }
		case ( 'e' ): { _extended = true;            break; }
		case ( 'c' ): { _ignoreCase = true;          break; }
		case ( 'C' ): { _ignoreCase = false;         break; }
		default : {
			if ( externalFlags ) {
				ok = false;
				for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it ) {
					if ( switch_ == it->first )
						*it->second = ok = true;
					break;
				}
			}
			break;
		}
	}
	return ok;
	M_EPILOG
}

int HPattern::matches( yaal::hcore::HString const& string_, int& matchLength_ ) const {
	M_PROLOG
	M_ASSERT( string_ );
	_errorCause.clear();
	int start( NO_MATCH );
	matchLength_ = 0;
	if ( _simpleMatchLength ) {
		if ( _extended ) {
			HRegex::HMatchIterator it( _regex.find( HString( string_ ) ) );
			if ( it != _regex.end() ) {
				matchLength_ = it->size();
				if ( matchLength_ > 0 ) {
					start = it->start();
				}
			} else {
				_errorCause = _pattern;
			}
		} else {
			if ( _ignoreCase ) {
				int long pos( icasesearch( string_, _pattern ) );
				if ( pos != HString::npos ) {
					start = static_cast<int>( pos );
				}
			} else {
				int long pos( string_.find( _pattern ) );
				if ( pos != HString::npos ) {
					start = static_cast<int>( pos );
				}
			}
			if ( start != NO_MATCH ) {
				matchLength_ = _simpleMatchLength;
			}
		}
	}
	return start;
	M_EPILOG
}

HPattern::HMatchIterator HPattern::find( HString const& str_ ) const {
	M_ASSERT( str_ );
	int len( 0 );
	int start( matches( str_, len ) );
	HMatchIterator it( this, start, len );
	return it;
}

HPattern::HMatchIterator HPattern::end( void ) const {
	return ( HMatchIterator( this, NO_MATCH, 0 ) );
}

bool HPattern::matches( HString const& str_ ) const {
	return ( find( str_ ) != end() );
}

HPattern::HMatch::HMatch( int start_, int size_ )
	: _start( start_ )
	, _size( size_ ) {
}

int HPattern::HMatch::start( void ) const {
	return ( _start );
}

int HPattern::HMatch::size( void ) const {
	return ( _size );
}

HPattern::HMatchIterator::HMatchIterator( HPattern const* owner_, int start_, int len_ )
	: base_type()
	, _owner( owner_ )
	, _match( start_, len_ ) {
}

HPattern::HMatchIterator::HMatchIterator( HMatchIterator const& it_ )
	: base_type()
	, _owner( it_._owner )
	, _match( it_._match ) {
}

HPattern::HMatchIterator& HPattern::HMatchIterator::operator = ( HPattern::HMatchIterator const& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_match = it_._match;
	}
	return ( *this );
}

HPattern::HMatch const* HPattern::HMatchIterator::operator->( void ) const {
	return ( &_match );
}

bool HPattern::HMatchIterator::operator != ( HMatchIterator const& mi_ ) const {
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start != _match._start );
	M_EPILOG
}

bool HPattern::HMatchIterator::operator == ( HMatchIterator const& mi_ ) const {
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start == _match._start );
	M_EPILOG
}

HPattern::HMatchIterator& HPattern::HMatchIterator::operator ++ ( void ) {
	M_PROLOG
	M_ASSERT( _match._start != NO_MATCH );
	_match._start = _owner->matches( _match._start + 1, _match._size );
	return ( *this );
	M_EPILOG
}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.cxx - this file is integral part of `yaal' project.

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
#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hpattern.hxx"
#include "harray.hxx"
#include "hchunk.hxx"

namespace yaal {

namespace hcore {

struct OPatternState {
	bool _ignoreCase;
	bool _extended;
	HPattern::pluggable_flags_t _flags;
	OPatternState( void ) : _ignoreCase( false ), _extended( false ), _flags() {}
};

HPattern::HPattern( HString const& pattern_, bool ignoreCase_ ) : _initialized( false ),
	_ignoreCaseDefault( ignoreCase_ ), _ignoreCase( false ),
	_extended( false ), _simpleMatchLength( 0 ), _compiled( sizeof ( regex_t ) ),
	_patternInput(), _patternReal(), _lastError( 0 ), _errorBuffer(), _errorCause(), _errorMessage() {
	M_PROLOG
	if ( ! pattern_.is_empty() ) {
		M_ENSURE( ! parse_re( pattern_ ) );
	}
	return;
	M_EPILOG
}

HPattern::~HPattern( void ) {
	M_PROLOG
	if ( _initialized )
		::regfree( _compiled.get<regex_t>() );
	return;
	M_DESTRUCTOR_EPILOG
}

void HPattern::save_state( void* sp, pluggable_flags_t* f ) {
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	s._ignoreCase = _ignoreCase;
	s._extended = _extended;
	if ( f )
		s._flags = *f;
	return;
	M_EPILOG
}

void HPattern::restore_state( void* sp, pluggable_flags_t* f ) {
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	_ignoreCase = s._ignoreCase;
	_extended = s._extended;
	if ( f )
		*f = s._flags;
	return;
	M_EPILOG
}

void HPattern::error_clear( void ) const {
	M_PROLOG
	_lastError = 0;
	_errorCause.clear();
	_errorMessage.clear();
	return;
	M_EPILOG
}

int HPattern::parse( HString const& pattern_, pluggable_flags_t* externalFlags ) {
	M_PROLOG
	error_clear();
	char const* const pattern( pattern_.raw() );
	_patternInput = pattern_;
/* making copy of flags */
	OPatternState savePoint;
	save_state( &savePoint, externalFlags );
/* end of copy */
/* clear all flags */
	_ignoreCase = _ignoreCaseDefault;
	_extended = false;
	if ( externalFlags ) {
		for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it )
			*it->second = false;
	}
/* FIXME g++ 4.3 bug *///		flags_[ i ] &= FLAG_MASK;
/* end of clearing */
/* look for switches at the beginnig of pattern */
	int long ctr( 0 );
	int err( 0 );
	while ( pattern[ ctr ] == '\\' ) {
		if ( set_switch( pattern[ ++ ctr ], externalFlags ) ) {
			restore_state( &savePoint, externalFlags );
			err = 1;
			_errorCause.format( "bad search option '%c'", pattern[ ctr ] );
			return ( err );
		}
		++ ctr;
	}
	if ( pattern[ ctr ] == '/' )
		ctr ++;
	int long begin = ctr;
/* end of looking at begin */
/* making copy of flags */
	save_state( &savePoint, externalFlags );
/* end of copy */
/* look for switches at the end of pattern */
	int long endMatch( ctr = _patternInput.get_length() - 1 );
	if ( endMatch < 0 )
		return ( true );
	while ( ( ctr > 0 ) && ( pattern[ ctr ] != '/' ) ) {
		if ( set_switch( pattern[ ctr ], externalFlags ) ) {
			restore_state( &savePoint, externalFlags );
			ctr = 1;
		}
		ctr --;
	}
	if ( ctr )
		endMatch = ctr - 1;
/* end of looking at end */
	_errorCause = _patternReal = _patternInput.mid( begin,
			( endMatch - begin ) + 1 );
	_simpleMatchLength = static_cast<int>( _patternReal.get_length() );
	if ( ! _simpleMatchLength ) {
		err = -1;
		_errorCause = _( "empty pattern" );
	}
	if ( ( ! err ) && _extended )
		err = parse_re( _patternReal.raw() );
	return ( err );
	M_EPILOG
}

int HPattern::parse_re( HString const& pattern_ ) {
	M_PROLOG
	error_clear();
	if ( _initialized )
		::regfree( _compiled.get<regex_t>() );
	if ( ( _lastError = ::regcomp( _compiled.get<regex_t>(), pattern_.raw(),
					_ignoreCase ? REG_ICASE : 0 ) ) ) {
		_errorCause = pattern_;
		_initialized = false;
	} else {
		_initialized = true;
		_extended = true;
		_simpleMatchLength = 1; /* it is not really a simple pattern */
	}
	return ( _lastError );
	M_EPILOG
}

HString const& HPattern::error( void ) const {
	error_message();
	return ( _errorMessage );
}

int HPattern::error_code( void ) const {
	return ( _lastError );
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
	return ( ! ok );
	M_EPILOG
}

char const* HPattern::matches( char const* const string_,
		int long* const matchLength_ ) const {
	M_PROLOG
	M_ASSERT( string_ );
	error_clear();
	char const* ptr = NULL;
	int long matchLength = 0;
	regmatch_t match;
	if ( _simpleMatchLength ) {
		if ( _extended ) {
			if ( ! ( _lastError = ::regexec( _compiled.get<regex_t>(), string_, 1, &match, 0 ) ) ) {
				matchLength = match.rm_eo - match.rm_so;
				if ( matchLength > 0 )
					ptr = string_ + match.rm_so;
			} else
				_errorCause = _patternReal;
		} else {
			if ( _ignoreCase )
				ptr = ::strcasestr( string_, _patternReal.raw() );
			else
				ptr = ::strstr( string_, _patternReal.raw() );
			if ( ptr )
				matchLength = _simpleMatchLength;
		}
	}
	if ( matchLength_ )
		( *matchLength_ ) = matchLength;
	return ( ptr );
	M_EPILOG
}

char const* HPattern::error_message( int ) const {
	M_PROLOG
	if ( _lastError ) {
		int long size( static_cast<int long>( ::regerror( _lastError, _compiled.get<regex_t>(), NULL, 0 ) ) + 1 );
		_errorBuffer.realloc( size + 1 );
		M_ENSURE( static_cast<int>( ::regerror( _lastError, _compiled.get<regex_t>(),
						_errorBuffer.raw(), size ) ) < size );
		_errorMessage = _errorBuffer.raw();
		if ( ! _errorCause.is_empty() ) {
			_errorMessage += ": `";
			_errorMessage += _errorCause;
			_errorMessage += "'";
		}
	} else
		_errorMessage = _errorCause;
	return ( _errorMessage.raw() );
	M_EPILOG
}

HPattern::HMatchIterator HPattern::find( char const* const str_ ) const {
	M_ASSERT( str_ );
	int long len = 0;
	char const* start = matches( str_, &len );
	HMatchIterator it( this, start, len );
	return ( it );
}

HPattern::HMatchIterator HPattern::end( void ) const {
	return ( HMatchIterator( this, NULL, 0 ) );
}

HPattern::HMatch::HMatch( char const* start_, int long size_ )
	: _size( size_ ), _start( start_ ) {
}

int long HPattern::HMatch::size( void ) const {
	return ( _size );
}

char const* HPattern::HMatch::raw( void ) const {
	return ( _start );
}

HPattern::HMatchIterator::HMatchIterator( HPattern const* owner_, char const* const start_, int long len_ )
	: base_type(), _owner( owner_ ), _match( start_, len_ ) {
}

HPattern::HMatchIterator::HMatchIterator( HMatchIterator const& it_ )
	: base_type(), _owner( it_._owner ), _match( it_._match ) {
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
	M_ASSERT( _match._start );
	_match._start = _owner->matches( _match._start + 1, &_match._size );
	return ( *this );
	M_EPILOG
}

}

}


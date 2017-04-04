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
	char const* pattern( pattern_.c_str() );

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
			if ( ! set_switch( pattern[ idx ], externalFlags ) ) {
				restore_state( &savePoint, externalFlags );
				_errorCause.format( "bad search option '%c'", pattern[ idx ] );
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
	return ( ok );
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
	return ( ok );
	M_EPILOG
}

char const* HPattern::matches( char const* string_,
		int* matchLength_ ) const {
	M_PROLOG
	M_ASSERT( string_ );
	_errorCause.clear();
	char const* ptr = nullptr;
	int matchLength = 0;
	if ( _simpleMatchLength ) {
		if ( _extended ) {
			HRegex::HMatchIterator it( _regex.find( string_ ) );
			if ( it != _regex.end() ) {
				matchLength = it->size();
				if ( matchLength > 0 ) {
					ptr = string_ + it->start();
				}
			} else {
				_errorCause = _pattern;
			}
		} else {
			if ( _ignoreCase ) {
				ptr = ::strcasestr( string_, _pattern.c_str() );
			} else {
				ptr = ::strstr( string_, _pattern.c_str() );
			}
			if ( ptr ) {
				matchLength = _simpleMatchLength;
			}
		}
	}
	if ( matchLength_ ) {
		( *matchLength_ ) = matchLength;
	}
	return ( ptr );
	M_EPILOG
}

HPattern::HMatchIterator HPattern::find( HString const& str_ ) const {
	M_ASSERT( str_ );
	int len( 0 );
	char const* start( matches( str_.c_str(), &len ) );
	HMatchIterator it( this, start, len );
	return ( it );
}

HPattern::HMatchIterator HPattern::end( void ) const {
	return ( HMatchIterator( this, nullptr, 0 ) );
}

bool HPattern::matches( HString const& str_ ) const {
	return ( find( str_ ) != end() );
}

HPattern::HMatch::HMatch( char const* start_, int size_ )
	: _size( size_ ), _start( start_ ) {
}

int HPattern::HMatch::size( void ) const {
	return ( _size );
}

char const* HPattern::HMatch::raw( void ) const {
	return ( _start );
}

HPattern::HMatchIterator::HMatchIterator( HPattern const* owner_, char const* start_, int len_ )
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
	M_ASSERT( _match._start );
	_match._start = _owner->matches( _match._start + 1, &_match._size );
	return ( *this );
	M_EPILOG
}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

namespace yaal
{

namespace hcore
{

struct OPatternState
	{
	bool _ignoreCase;
	bool _extended;
	HPattern::pluggable_flags_t _flags;
	OPatternState( void ) : _ignoreCase( false ), _extended( false ), _flags() {}
	};

HPattern::HPattern( bool const ignoreCase_ ) : _initialized( false ),
	_ignoreCaseDefault( ignoreCase_ ), _ignoreCase( false ),
	_extended( false ), _simpleMatchLength( 0 ), _compiled( sizeof ( regex_t ) ),
	_patternInput(), _patternReal(), _lastError( 0 ), _varTmpBuffer()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HPattern::~HPattern( void )
	{
	M_PROLOG
	regfree( _compiled.get<regex_t>() );
	return;
	M_EPILOG
	}

void HPattern::save_state( void* sp, pluggable_flags_t* f )
	{
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	s._ignoreCase = _ignoreCase;
	s._extended = _extended;
	if ( f )
		s._flags = *f;
	return;
	M_EPILOG
	}

void HPattern::restore_state( void* sp, pluggable_flags_t* f )
	{
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	_ignoreCase = s._ignoreCase;
	_extended = s._extended;
	if ( f )
		*f = s._flags;
	return;
	M_EPILOG
	}

int HPattern::parse( HString const& pattern_, pluggable_flags_t* externalFlags ) 
	{
	M_PROLOG
	char const* const pattern = pattern_.raw();
	_patternInput = pattern_;
	_varTmpBuffer = "";
/* making copy of flags */
	OPatternState savePoint;
	save_state( &savePoint, externalFlags );
/* end of copy */
/* clear all flags */
	_ignoreCase = _ignoreCaseDefault;
	_extended = false;
	if ( externalFlags )
		{
		for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it )
			*it->second = false;
		}
/* FIXME g++ 4.3 bug *///		flags_[ i ] &= FLAG_MASK;
/* end of clearing */
/* look for switches at the beginnig of pattern */
	int long ctr( 0 );
	int err( 0 );
	while ( pattern[ ctr ] == '\\' )
		{
		if ( set_switch( pattern[ ++ ctr ], externalFlags ) )
			{
			restore_state( &savePoint, externalFlags );
			err = 1;
			_varTmpBuffer.format( "bad search option '%c'", pattern[ ctr ] );
			return ( err );
			}
		ctr ++;
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
	while ( ( ctr > 0 ) && ( pattern[ ctr ] != '/' ) )
		{
		if ( set_switch( pattern[ ctr ], externalFlags ) )
			{
			restore_state( &savePoint, externalFlags );
			ctr = 1;
			}
		ctr --;
		}
	if ( ctr )
		endMatch = ctr - 1;
/* end of looking at end */
	_varTmpBuffer = _patternReal = _patternInput.mid( begin,
			( endMatch - begin ) + 1 );
	_simpleMatchLength = static_cast<int>( _patternReal.get_length() );
	if ( ! _simpleMatchLength )
		{
		err = -1;
		_varTmpBuffer = _( "empty pattern" );
		}
	_initialized = ! err;
	if ( _initialized && _extended )
		err = parse_re( _patternReal.raw() );
	return ( err );
	M_EPILOG
	}

int HPattern::parse_re( char const* const pattern_ )
	{
	M_PROLOG
	::regfree( _compiled.get<regex_t>() );
	if ( ( _lastError = ::regcomp( _compiled.get<regex_t>(), pattern_,
					_ignoreCase ? REG_ICASE : 0 ) ) )
		{
		prepare_error_message( pattern_ );
		_initialized = false;
		}
	else
		{
		_initialized = true;
		_extended = true;
		_simpleMatchLength = 1; /* it is not really a simple pattern */
		}
	return ( _lastError );
	M_EPILOG
	}

HString const& HPattern::error( void ) const
	{
	return ( _varTmpBuffer );
	}

int HPattern::error_code( void ) const
	{
	return ( _lastError );
	}

bool HPattern::set_switch( char const switch_, pluggable_flags_t* externalFlags ) 
	{
	M_PROLOG
	bool ok( true );
	switch ( switch_ )
		{
		case ( 'i' ):{_ignoreCase = ! _ignoreCase;break;}
		case ( 'e' ):{_extended = true;break;}
		case ( 'c' ):{_ignoreCase = true;break;}
		case ( 'C' ):{_ignoreCase = false;break;}
		default :
			{
			if ( externalFlags )
				{
				ok = false;
				for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it )
					{
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
		int long* const matchLength_ ) const
	{
	M_PROLOG
	M_ASSERT( string_ );
	char const* ptr = NULL;
	int long matchLength = 0;
	regmatch_t match;
	if ( _simpleMatchLength )
		{
		if ( _extended )
			{
			if ( ! ( _lastError = ::regexec( _compiled.get<regex_t>(), string_, 1, &match, 0 ) ) )
				{
				matchLength = match.rm_eo - match.rm_so;
				if ( matchLength > 0 )
					ptr = string_ + match.rm_so;
				}
			else
				prepare_error_message( _patternReal );
			}
		else
			{
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

void HPattern::prepare_error_message( HString const& string_ ) const
	{
	M_PROLOG
	int long size( static_cast<int long>( ::regerror( _lastError, _compiled.get<regex_t>(), NULL, 0 ) ) + 1 );
	HChunk buffer( size + 1 );
	M_ENSURE( static_cast<int>( ::regerror( _lastError, _compiled.get<regex_t>(),
					buffer.raw(), size ) ) < size );
	_varTmpBuffer = buffer.raw();
	if ( ! string_.empty() )
		{
		_varTmpBuffer += ": `";
		_varTmpBuffer += string_;
		_varTmpBuffer += "'";
		}
	return;
	M_EPILOG
	}

HPattern::HMatchIterator HPattern::find( char const* str_ ) const
	{
	M_ASSERT( str_ );
	int long len = 0;
	char const* start = matches( str_, &len );
	HMatchIterator it( this, start, len );
	return ( it );
	}

HPattern::HMatchIterator HPattern::end( void ) const
	{
	return ( HMatchIterator( this, NULL, 0 ) );
	}

HPattern::HMatch::HMatch( char const* const& start_, int long const& size_ )
	: _size( size_ ), _start( start_ )
	{
	}

int long HPattern::HMatch::size( void ) const
	{
	return ( _size );
	}

char const* HPattern::HMatch::raw( void ) const
	{
	return ( _start );
	}

HPattern::HMatchIterator::HMatchIterator( HPattern const* owner_, char const* const start_, int long const& len_ )
	: _owner( owner_ ), _match( start_, len_ )
	{
	}

HPattern::HMatch const* HPattern::HMatchIterator::operator->( void ) const
	{
	return ( &_match );
	}

bool HPattern::HMatchIterator::operator != ( HMatchIterator const& mi_ ) const
	{
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start != _match._start );
	M_EPILOG
	}

bool HPattern::HMatchIterator::operator == ( HMatchIterator const& mi_ ) const
	{
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start == _match._start );
	M_EPILOG
	}

HPattern::HMatchIterator& HPattern::HMatchIterator::operator ++ ( void )
	{
	M_PROLOG
	M_ASSERT( _match._start );
	_match._start = _owner->matches( _match._start + 1, &_match._size );
	return ( *this );
	M_EPILOG
	}

}

}


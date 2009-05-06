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
#include "hpattern.hxx"
#include "harray.hxx"
#include "hpool.hxx"

namespace yaal
{

namespace hcore
{

struct OPatternState
	{
	bool f_bIgnoreCase;
	bool f_bExtended;
	HPattern::pluggable_flags_t f_oFlags;
	OPatternState( void ) : f_bIgnoreCase( false ), f_bExtended( false ), f_oFlags() {}
	};

HPattern::HPattern( bool const a_bIgnoreCase ) : f_bInitialized( false ),
	f_bIgnoreCaseDefault( a_bIgnoreCase ), f_bIgnoreCase( false ),
	f_bExtended( false ), f_iSimpleMatchLength( 0 ), f_oCompiled( xcalloc<regex_t>( 1 ) ),
	f_oPatternInput(), f_oPatternReal(), f_iLastError( 0 ), f_oVarTmpBuffer()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HPattern::~HPattern( void )
	{
	M_PROLOG
	regfree( f_oCompiled.get<regex_t>() );
	return;
	M_EPILOG
	}

void HPattern::save_state( void* sp, pluggable_flags_t* f )
	{
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	s.f_bIgnoreCase = f_bIgnoreCase;
	s.f_bExtended = f_bExtended;
	if ( f )
		s.f_oFlags = *f;
	return;
	M_EPILOG
	}

void HPattern::restore_state( void* sp, pluggable_flags_t* f )
	{
	M_PROLOG
	OPatternState& s = *static_cast<OPatternState*>( sp );
	f_bIgnoreCase = s.f_bIgnoreCase;
	f_bExtended = s.f_bExtended;
	if ( f )
		*f = s.f_oFlags;
	return;
	M_EPILOG
	}

int HPattern::parse( HString const& a_oPattern, pluggable_flags_t* externalFlags ) 
	{
	M_PROLOG
	int l_iError = 0;
	char const* const l_pcPattern = a_oPattern.raw();
	f_oPatternInput = a_oPattern;
	f_oVarTmpBuffer = "";
/* making copy of flags */
	OPatternState savePoint;
	save_state( &savePoint, externalFlags );
/* end of copy */
/* clear all flags */
	f_bIgnoreCase = f_bIgnoreCaseDefault;
	f_bExtended = false;
	if ( externalFlags )
		{
		for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it )
			*it->second = false;
		}
/* FIXME g++ 4.3 bug *///		a_puhFlags[ i ] &= FLAG_MASK;
/* end of clearing */
/* look for switches at the beginnig of pattern */
	int long l_iCtr = 0;
	while ( l_pcPattern[ l_iCtr ] == '\\' )
		{
		if ( set_switch( l_pcPattern[ ++ l_iCtr ], externalFlags ) )
			{
			restore_state( &savePoint, externalFlags );
			l_iError = 1;
			f_oVarTmpBuffer.format( "bad search option '%c'", l_pcPattern[ l_iCtr ] );
			return ( l_iError );
			}
		l_iCtr ++;
		}
	if ( l_pcPattern[ l_iCtr ] == '/' )
		l_iCtr ++;
	int long l_iBegin = l_iCtr;
/* end of looking at begin */
/* making copy of flags */
	save_state( &savePoint, externalFlags );
/* end of copy */
/* look for switches at the end of pattern */
	int long l_iEnd = l_iCtr = f_oPatternInput.get_length() - 1;
	if ( l_iEnd < 0 )
		return ( true );
	while ( ( l_iCtr > 0 ) && ( l_pcPattern[ l_iCtr ] != '/' ) )
		{
		if ( set_switch( l_pcPattern[ l_iCtr ], externalFlags ) )
			{
			restore_state( &savePoint, externalFlags );
			l_iCtr = 1;
			}
		l_iCtr --;
		}
	if ( l_iCtr )
		l_iEnd = l_iCtr - 1;
/* end of looking at end */
	f_oVarTmpBuffer = f_oPatternReal = f_oPatternInput.mid( l_iBegin,
			( l_iEnd - l_iBegin ) + 1 );
	f_iSimpleMatchLength = static_cast<int>( f_oPatternReal.get_length() );
	if ( ! f_iSimpleMatchLength )
		{
		l_iError = - 1;
		f_oVarTmpBuffer = _( "empty pattern" );
		}
	f_bInitialized = ! l_iError;
	if ( f_bInitialized && f_bExtended )
		l_iError = parse_re( f_oPatternReal.raw() );
	return ( l_iError );
	M_EPILOG
	}

int HPattern::parse_re( char const* const a_pcPattern )
	{
	M_PROLOG
	::regfree( f_oCompiled.get<regex_t>() );
	if ( ( f_iLastError = ::regcomp( f_oCompiled.get<regex_t>(), a_pcPattern,
					f_bIgnoreCase ? REG_ICASE : 0 ) ) )
		{
		prepare_error_message( a_pcPattern );
		f_bInitialized = false;
		}
	else
		{
		f_bInitialized = true;
		f_bExtended = true;
		f_iSimpleMatchLength = 1; /* it is not really a simple pattern */
		}
	return ( f_iLastError );
	M_EPILOG
	}

HString const& HPattern::error( void ) const
	{
	return ( f_oVarTmpBuffer );
	}

int HPattern::error_code( void ) const
	{
	return ( f_iLastError );
	}

bool HPattern::set_switch( char const a_cSwitch, pluggable_flags_t* externalFlags ) 
	{
	M_PROLOG
	bool ok( true );
	switch ( a_cSwitch )
		{
		case ( 'i' ):{f_bIgnoreCase = ! f_bIgnoreCase;break;}
		case ( 'e' ):{f_bExtended = true;break;}
		case ( 'c' ):{f_bIgnoreCase = true;break;}
		case ( 'C' ):{f_bIgnoreCase = false;break;}
		default :
			{
			if ( externalFlags )
				{
				ok = false;
				for ( pluggable_flags_t::iterator it = externalFlags->begin(), endIt = externalFlags->end(); it != endIt; ++ it )
					{
					if ( a_cSwitch == it->first )
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

char const* HPattern::matches( char const* const a_pcString,
		int long* const a_piMatchLength ) const
	{
	M_PROLOG
	M_ASSERT( a_pcString );
	char const* l_pcPtr = NULL;
	int l_iMatchLength = 0;
	regmatch_t l_sMatch;
	if ( f_iSimpleMatchLength )
		{
		if ( f_bExtended )
			{
			if ( ! ( f_iLastError = ::regexec( f_oCompiled.get<regex_t>(), a_pcString, 1, &l_sMatch, 0 ) ) )
				{
				l_iMatchLength = l_sMatch.rm_eo - l_sMatch.rm_so;
				if ( l_iMatchLength > 0 )
					l_pcPtr = a_pcString + l_sMatch.rm_so;
				}
			else
				prepare_error_message( f_oPatternReal );
			}
		else
			{
			if ( f_bIgnoreCase )
				l_pcPtr = ::strcasestr( a_pcString, f_oPatternReal.raw() );
			else
				l_pcPtr = ::strstr( a_pcString, f_oPatternReal.raw() );
			if ( l_pcPtr )
				l_iMatchLength = f_iSimpleMatchLength;
			}
		}
	if ( a_piMatchLength )
		( *a_piMatchLength ) = l_iMatchLength;
	return ( l_pcPtr );
	M_EPILOG
	}

void HPattern::prepare_error_message( HString const& a_oString ) const
	{
	M_PROLOG
	int long l_iSize = ::regerror( f_iLastError, f_oCompiled.get<regex_t>(), NULL, 0 ) + 1;
	HPool<char> l_oBuffer( l_iSize + 1 );
	M_ENSURE( static_cast<int>( ::regerror( f_iLastError, f_oCompiled.get<regex_t>(),
					l_oBuffer.raw(), l_iSize ) ) < l_iSize );
	f_oVarTmpBuffer = l_oBuffer.raw();
	if ( ! a_oString.empty() )
		{
		f_oVarTmpBuffer += ": `";
		f_oVarTmpBuffer += a_oString;
		f_oVarTmpBuffer += "'";
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


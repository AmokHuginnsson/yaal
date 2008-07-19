/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */
#include <libintl.h>

#include "base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hpattern.h"
#include "harray.h"
#include "hpool.h"

namespace yaal
{

namespace hcore
{

HPattern::HPattern( bool const a_bIgnoreCase ) : f_bInitialized( false ),
	f_bIgnoreCaseDefault( a_bIgnoreCase ), f_bIgnoreCase( false ),
	f_bExtended( false ), f_iSimpleMatchLength( 0 ), f_oCompiled( xcalloc<regex_t>( 1 ) ),
	f_oPatternInput(), f_oPatternReal(), f_oError()
	{
	M_PROLOG
	memset( f_oCompiled.get<regex_t>(), 0, sizeof ( f_oCompiled ) );
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

int HPattern::parse( HString const& a_oPattern,
		int short unsigned* const a_puhFlags, int const a_iFlagsCount )
	{
	M_PROLOG
	int l_iError = 0;
	bool l_bLocalCopyIgnoreCase = false, l_bLocalCopyExtended = false;
	HArray<int short unsigned> l_oLocalCopyFlags( a_iFlagsCount );
	char const* const l_pcPattern = a_oPattern.raw();
	f_oPatternInput = a_oPattern;
	f_oError = "";
/* making copy of flags */
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( int i = 0; i < a_iFlagsCount; i ++ )
		l_oLocalCopyFlags[ i ] = a_puhFlags[ i ];
/* end of copy */
/* clear all flags */
	f_bIgnoreCase = f_bIgnoreCaseDefault;
	f_bExtended = false;
	static int short unsigned const D_FLAG_MASK = 0x00ff;
	for ( int i = 0; i < a_iFlagsCount; ++ i )
		a_puhFlags[ i ] = static_cast<int short unsigned>( a_puhFlags[ i ] & D_FLAG_MASK );
/* FIXME g++ 4.3 bug *///		a_puhFlags[ i ] &= D_FLAG_MASK;
/* end of clearing */
/* look for switches at the beginnig of pattern */
	int long l_iCtr = 0;
	while ( l_pcPattern[ l_iCtr ] == '\\' )
		{
		if ( set_switch( l_pcPattern[ ++ l_iCtr ], a_puhFlags, a_iFlagsCount ) )
			{
			f_bIgnoreCase = l_bLocalCopyIgnoreCase;
			f_bExtended = l_bLocalCopyExtended;
			for ( int i = 0; i < a_iFlagsCount; ++ i )
				a_puhFlags[ i ] = l_oLocalCopyFlags[ i ];
			l_iError = 1;
			f_oError.format( "bad search option '%c'", l_pcPattern[ l_iCtr ] );
			return ( l_iError );
			}
		l_iCtr ++;
		}
	if ( l_pcPattern[ l_iCtr ] == '/' )
		l_iCtr ++;
	int long l_iBegin = l_iCtr;
/* end of looking at begin */
/* making copy of flags */
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( int i = 0; i < a_iFlagsCount; ++ i )
		l_oLocalCopyFlags[ i ] = a_puhFlags[ i ];
/* end of copy */
/* look for switches at the end of pattern */
	int long l_iEnd = l_iCtr = f_oPatternInput.get_length() - 1;
	if ( l_iEnd < 0 )
		return ( true );
	while ( ( l_iCtr > 0 ) && ( l_pcPattern[ l_iCtr ] != '/' ) )
		{
		if ( set_switch( l_pcPattern[ l_iCtr ], a_puhFlags, a_iFlagsCount ) )
			{
			f_bIgnoreCase = l_bLocalCopyIgnoreCase;
			f_bExtended = l_bLocalCopyExtended;
			for ( int i = 0; i < a_iFlagsCount; ++ i )
				a_puhFlags[ i ] = l_oLocalCopyFlags[ i ];
			l_iCtr = 1;
			}
		l_iCtr --;
		}
	if ( l_iCtr )
		l_iEnd = l_iCtr - 1;
/* end of looking at end */
	f_oError = f_oPatternReal = f_oPatternInput.mid( l_iBegin,
			( l_iEnd - l_iBegin ) + 1 );
	f_iSimpleMatchLength = static_cast<int>( f_oPatternReal.get_length() );
	if ( ! f_iSimpleMatchLength )
		{
		l_iError = - 1;
		f_oError = _ ( "empty pattern" );
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
	int l_iError = 0;
	::regfree( f_oCompiled.get<regex_t>() );
	if ( ( l_iError = ::regcomp( f_oCompiled.get<regex_t>(), a_pcPattern,
					f_bIgnoreCase ? REG_ICASE : 0 ) ) )
		{
		prepare_error_message( l_iError, a_pcPattern );
		f_bInitialized = false;
		}
	else
		{
		f_bInitialized = true;
		f_bExtended = true;
		f_iSimpleMatchLength = 1; /* it is not really a simple pattern */
		}
	return ( l_iError );
	M_EPILOG
	}

HString const& HPattern::error( void ) const
	{
	return ( f_oError );
	}

bool HPattern::set_switch( char const a_cSwitch,
		int short unsigned* const a_puhFlags,
		int const a_iFlagsCount )
	{
	M_PROLOG
	int l_iCtr = 0;
	switch ( a_cSwitch )
		{
		case ( 'i' ):{f_bIgnoreCase = ! f_bIgnoreCase;break;}
		case ( 'e' ):{f_bExtended = true;break;}
		case ( 'c' ):{f_bIgnoreCase = true;break;}
		case ( 'C' ):{f_bIgnoreCase = false;break;}
		default :
			{
			for ( l_iCtr = 0; l_iCtr < a_iFlagsCount; l_iCtr ++ )
				if ( a_cSwitch == ( a_puhFlags[ l_iCtr ] & 0x00ff ) )
					{
					a_puhFlags[ l_iCtr ] |= 0x0100;
					break;
					}
			if ( l_iCtr >= a_iFlagsCount )
				return ( true );
			break;
			}
		}
	return ( false );
	M_EPILOG
	}

char const* HPattern::matches( HString const& a_oString,
		int* const a_piMatchLength, int* const a_piError )
	{
	M_PROLOG
	char const * l_pcPtr = NULL;
	int l_iError = 0;
	int l_iMatchLength = 0;
	regmatch_t l_sMatch;
	if ( f_iSimpleMatchLength )
		{
		if ( f_bExtended )
			{
			if ( ! ( l_iError = ::regexec( f_oCompiled.get<regex_t>(), a_oString.raw(), 1, &l_sMatch, 0 ) ) )
				{
				l_iMatchLength = l_sMatch.rm_eo - l_sMatch.rm_so;
				if ( l_iMatchLength > 0 )
					l_pcPtr = a_oString.raw() + l_sMatch.rm_so;
				}
			else
				prepare_error_message( l_iError, f_oPatternReal );
			}
		else
			{
			if ( f_bIgnoreCase )
				l_pcPtr = ::strcasestr( a_oString.raw(), f_oPatternReal.raw() );
			else
				l_pcPtr = ::strstr( a_oString.raw(), f_oPatternReal.raw() );
			if ( l_pcPtr )
				l_iMatchLength = f_iSimpleMatchLength;
			}
		}
	if ( a_piMatchLength )
		( *a_piMatchLength ) = l_iMatchLength;
	if ( a_piError )
		( *a_piError ) = l_iError;
	return ( l_pcPtr );
	M_EPILOG
	}

int HPattern::count( char const* const a_pcString )
	{
	M_PROLOG
	int l_iCtr = 0;
	char const* l_pcPtr = a_pcString;
	while ( ( l_pcPtr = matches( l_pcPtr ) ) )
		l_iCtr ++, l_pcPtr ++;
	return ( l_iCtr );
	M_EPILOG
	}

void HPattern::prepare_error_message( int const a_iError,
		HString const& a_oString )
	{
	M_PROLOG
	int long l_iSize = ::regerror( a_iError, f_oCompiled.get<regex_t>(), NULL, 0 ) + 1;
	HPool<char> l_oBuffer( l_iSize + 1 );
	M_ENSURE( static_cast<int>( ::regerror( a_iError, f_oCompiled.get<regex_t>(),
					l_oBuffer.raw(), l_iSize ) ) < l_iSize );
	f_oError = l_oBuffer.raw();
	if ( ! a_oString.empty() )
		{
		f_oError += ": `";
		f_oError += a_oString;
		f_oError += "'";
		}
	return;
	M_EPILOG
	}

}

}


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

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
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hpattern.h"

namespace yaal
{

namespace hcore
{

HPattern::HPattern ( bool const a_bIgnoreCase ) : f_bInitialized ( false ),
	f_bIgnoreCaseDefault ( a_bIgnoreCase ), f_bIgnoreCase ( false ),
	f_bExtended ( false ), f_iSimpleMatchLength ( 0 ), f_sCompiled(),
	f_oPatternInput(), f_oPatternReal(), f_oError()
	{
	M_PROLOG
	memset ( & f_sCompiled, 0, sizeof ( f_sCompiled ) );
	return;
	M_EPILOG
	}

HPattern::~HPattern ( void )
	{
	M_PROLOG
	regfree ( & f_sCompiled );
	return;
	M_EPILOG
	}

int HPattern::parse ( char const * const a_pcPattern,
		int short unsigned * const a_puhFlags, int const a_iFlagsCount )
	{
	M_PROLOG
	int l_iError = 0;
	bool l_bLocalCopyIgnoreCase = false, l_bLocalCopyExtended = false;
	int l_iCtr = 0, l_iCtrLoc = 0, l_iBegin = 0, l_iEnd = 0;
	HArray < int short unsigned > l_oLocalCopyFlags ( a_iFlagsCount );
	char const * const l_pcPattern = f_oPatternInput = a_pcPattern;
	f_oError = "";
/* making copy of flags */
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
		l_oLocalCopyFlags [ l_iCtrLoc ] = a_puhFlags [ l_iCtrLoc ];
/* end of copy */
/* clear all flags */
	f_bIgnoreCase = f_bIgnoreCaseDefault;
	f_bExtended = false;
	for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
		a_puhFlags [ l_iCtrLoc ] &= 0x00ff;
/* end of clearing */
/* look for switches at the beginnig of pattern */
	l_iCtr = 0;
	while ( l_pcPattern [ l_iCtr ] == '\\' )
		{
		if ( set_switch ( l_pcPattern [ ++ l_iCtr ], a_puhFlags, a_iFlagsCount ) )
			{
			f_bIgnoreCase = l_bLocalCopyIgnoreCase;
			f_bExtended = l_bLocalCopyExtended;
			for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
				a_puhFlags [ l_iCtrLoc ] = l_oLocalCopyFlags [ l_iCtrLoc ];
			l_iError = 1;
			f_oError.format ( "bad search option '%c'", l_pcPattern [ l_iCtr ] );
			return ( l_iError );
			}
		l_iCtr ++;
		}
	if ( l_pcPattern [ l_iCtr ] == '/' )
		l_iCtr ++;
	l_iBegin = l_iCtr;
/* end of looking at begin */
/* making copy of flags */
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
		l_oLocalCopyFlags [ l_iCtrLoc ] = a_puhFlags [ l_iCtrLoc ];
/* end of copy */
/* look for switches at the end of pattern */
	l_iEnd = l_iCtr = f_oPatternInput.get_length() - 1;
	if ( l_iEnd < 0 )
		return ( true );
	while ( ( l_iCtr > 0 ) && ( l_pcPattern [ l_iCtr ] != '/' ) )
		{
		if ( set_switch ( l_pcPattern [ l_iCtr ], a_puhFlags, a_iFlagsCount ) )
			{
			f_bIgnoreCase = l_bLocalCopyIgnoreCase;
			f_bExtended = l_bLocalCopyExtended;
			for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
				a_puhFlags [ l_iCtrLoc ] = l_oLocalCopyFlags [ l_iCtrLoc ];
			l_iCtr = 1;
			}
		l_iCtr --;
		}
	if ( l_iCtr )
		l_iEnd = l_iCtr - 1;
/* end of looking at end */
	f_oError = f_oPatternReal = f_oPatternInput.mid ( l_iBegin,
			( l_iEnd - l_iBegin ) + 1 );
	f_iSimpleMatchLength = f_oPatternReal.get_length();
	if ( ! f_iSimpleMatchLength )
		{
		l_iError = - 1;
		f_oError = _ ( "empty pattern" );
		}
	f_bInitialized = ! l_iError;
	if ( f_bInitialized && f_bExtended )
		l_iError = parse_re ( f_oPatternReal );
	return ( l_iError );
	M_EPILOG
	}

int HPattern::parse_re ( char const * const a_pcPattern )
	{
	M_PROLOG
	int l_iError = 0;
	regfree ( & f_sCompiled );
	if ( ( l_iError = regcomp ( & f_sCompiled, a_pcPattern,
					f_bIgnoreCase ? REG_ICASE : 0 ) ) )
		{
		prepare_error_message ( l_iError, a_pcPattern );
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

char const * HPattern::error ( void ) const
	{
	return ( f_oError );
	}

bool HPattern::set_switch ( char const a_cSwitch,
		int short unsigned * const a_puhFlags,
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
				if ( a_cSwitch == ( a_puhFlags [ l_iCtr ] & 0x00ff ) )
					{
					a_puhFlags [ l_iCtr ] |= 0x0100;
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

char const * HPattern::matches ( char const * const a_pcString,
		int * const a_piMatchLength, int * const a_piError )
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
			if ( ! ( l_iError = regexec ( & f_sCompiled, a_pcString, 1, & l_sMatch, 0 ) ) )
				{
				l_iMatchLength = l_sMatch.rm_eo - l_sMatch.rm_so;
				if ( l_iMatchLength > 0 )
					l_pcPtr = const_cast < char * > ( a_pcString ) + l_sMatch.rm_so;
				}
			else
				prepare_error_message ( l_iError, f_oPatternReal );
			}
		else
			{
			if ( f_bIgnoreCase )
				l_pcPtr = strcasestr ( a_pcString, f_oPatternReal );
			else
				l_pcPtr = strstr ( a_pcString, f_oPatternReal );
			if ( l_pcPtr )
				l_iMatchLength = f_iSimpleMatchLength;
			}
		}
	if ( a_piMatchLength )
		( * a_piMatchLength ) = l_iMatchLength;
	if ( a_piError )
		( * a_piError ) = l_iError;
	return ( l_pcPtr );
	M_EPILOG
	}

int HPattern::count ( char const * const a_pcString )
	{
	M_PROLOG
	int l_iCtr = 0;
	char const * l_pcPtr = a_pcString;
	while ( ( l_pcPtr = matches ( l_pcPtr ) ) )
		l_iCtr ++, l_pcPtr ++;
	return ( l_iCtr );
	M_EPILOG
	}

void HPattern::prepare_error_message ( int const a_iError,
		char const * const a_pcString )
	{
	M_PROLOG
	int l_iSize = regerror ( a_iError, & f_sCompiled, NULL, 0 ) + 1;
	f_oError.hs_realloc ( l_iSize + 1 );
	M_ENSURE ( static_cast < int > ( regerror ( a_iError, & f_sCompiled,
					f_oError.raw(), l_iSize ) ) < l_iSize );
	if ( a_pcString )
		{
		f_oError += ": `";
		f_oError += a_pcString;
		f_oError += "'";
		}
	return;
	M_EPILOG
	}

}

}


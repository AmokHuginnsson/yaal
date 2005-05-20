/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.c - this file is integral part of `stdhapi' project.

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

#include <string.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hpattern.h"

namespace stdhapi
{

namespace hcore
{

HPattern::HPattern ( bool a_bIgnoreCase ) : f_bInitialized ( false ),
	f_bIgnoreCaseDefault ( a_bIgnoreCase ), f_bIgnoreCase ( false ),
	f_bExtended ( false ), f_iSimpleMatchLength ( 0 ), f_sCompiled ( ),
	f_oPatternInput ( ), f_oPatternReal ( ), f_oError ( )
	{
	M_PROLOG
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

bool HPattern::parse ( char const * a_pcPattern,
		unsigned short int * a_puhFlags, int a_iFlagsCount )
	{
	M_PROLOG
	bool l_bError = false;
	bool l_bLocalCopyIgnoreCase = false, l_bLocalCopyExtended = false;
	int l_iCtr = 0, l_iCtrLoc = 0, l_iBegin = 0, l_iEnd = 0;
	HArray < unsigned short int > l_oLocalCopyFlags ( a_iFlagsCount );
	char * l_pcPattern = f_oPatternInput = a_pcPattern;
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
			l_bError = true;
			M_IRV ( f_oError.format ( "bad search option '%c'", l_pcPattern [ l_iCtr ] ) );
			return ( l_bError );
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
	l_iEnd = l_iCtr = f_oPatternInput.get_length ( ) - 1;
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
	f_iSimpleMatchLength = f_oPatternReal.get_length ( );
	if ( ! f_iSimpleMatchLength )
		l_bError = true;
	f_bInitialized = ! l_bError;
	if ( f_bInitialized && f_bExtended )
		{
		regfree ( & f_sCompiled );
		if ( ( l_iEnd = regcomp ( & f_sCompiled, f_oPatternReal, f_bIgnoreCase ? REG_ICASE : 0 ) ) )
			{
			prepare_error_message ( l_iEnd );
			f_oError += ": `";
			f_oError += f_oPatternReal;
			f_oError += "'";
			f_bInitialized = false;
			l_bError = true;
			}
		}
	return ( l_bError );
	M_EPILOG
	}

char const * HPattern::error ( void ) const
	{
	return ( f_oError );
	}

bool HPattern::set_switch ( char a_cSwitch, unsigned short int * a_puhFlags,
		int a_iFlagsCount )
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

char const * HPattern::matches ( char const * a_pcString, int & a_riMatchLength )
	{
	M_PROLOG
	char const * l_pcPtr = NULL;
	int l_iError = 0;
	if ( f_iSimpleMatchLength )
		{
		if ( f_bExtended )
			{
			regmatch_t l_sMatch;
			if ( ! ( l_iError = regexec ( & f_sCompiled, a_pcString, 1, & l_sMatch, 0 ) ) )
				{
				a_riMatchLength = l_sMatch.rm_eo - l_sMatch.rm_so;
				if ( a_riMatchLength > 0 )
					l_pcPtr = const_cast < char * > ( a_pcString ) + l_sMatch.rm_so;
				}
			else
				{
				prepare_error_message ( l_iError );
				f_oError += ": `";
				f_oError += f_oPatternReal;
				f_oError += "'";
				}
			}
		else
			{
			if ( f_bIgnoreCase )
				l_pcPtr = strcasestr ( a_pcString, f_oPatternReal );
			else
				l_pcPtr = strstr ( a_pcString, f_oPatternReal );
			if ( l_pcPtr )
				a_riMatchLength = f_iSimpleMatchLength;
			}
		}
	return ( l_pcPtr );
	M_EPILOG
	}

int HPattern::count ( char const * a_pcString )
	{
	M_PROLOG
	int l_iCtr = 0, l_iDummy = 0;
	char const * l_pcPtr = a_pcString;
	while ( ( l_pcPtr = matches ( l_pcPtr, l_iDummy ) ) )
		l_iCtr ++, l_pcPtr ++;
	return ( l_iCtr );
	M_EPILOG
	}

void HPattern::prepare_error_message ( int a_iError )
	{
	M_PROLOG
	unsigned int l_iSize = regerror ( a_iError, & f_sCompiled, NULL, 0 );
	f_oError.hs_realloc ( static_cast < int > ( l_iSize + 1 ) );
	M_IRV ( regerror ( a_iError, & f_sCompiled, f_oError, l_iSize ) );
	return;
	M_EPILOG
	}

}

}


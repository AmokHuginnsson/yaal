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

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hstring.h"
#include "hpattern.h"
#include "harray.h"

HPattern::HPattern ( bool a_bIgnoreCase )
	{
	M_PROLOG
	f_bInitialized = false;
	f_bIgnoreCaseDefault = a_bIgnoreCase;
	f_bExtended = false;
	return;
	M_EPILOG
	}

HPattern::~HPattern ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

bool HPattern::parse ( const char * a_pcPattern,
		unsigned short int * a_puhFlags, int a_iFlagsCount )
	{
	M_PROLOG
	bool l_bError = false;
	bool l_bLocalCopyIgnoreCase = false, l_bLocalCopyExtended = false;
	HArray < unsigned short int > l_oLocalCopyFlags ( a_iFlagsCount );
	int l_iCtr = 0, l_iCtrLoc = 0, l_iBegin = 0, l_iEnd = 0;
	char * l_pcPattern = f_oPatternInput = a_pcPattern;
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( l_iCtr = 0; l_iCtr < a_iFlagsCount; l_iCtr ++ )
		l_oLocalCopyFlags [ l_iCtr ] = a_puhFlags [ l_iCtr ];
	f_bIgnoreCase = f_bIgnoreCaseDefault;
	f_bExtended = false;
	for ( l_iCtr = 0; l_iCtr < a_iFlagsCount; l_iCtr ++ )
		a_puhFlags [ l_iCtr ] &= 0x00ff; /* clear all external flags */
	while ( l_pcPattern [ l_iCtr ] == '\\' )
		{
		switch ( l_pcPattern [ ++ l_iCtr ] )
			{
			case ( 'i' ):{f_bIgnoreCase = ! f_bIgnoreCase;break;}
			case ( 'e' ):{f_bExtended = true;break;}
			case ( 'c' ):{f_bIgnoreCase = true;break;}
			case ( 'C' ):{f_bIgnoreCase = false;break;}
			default :
				{
				for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
					if ( l_pcPattern [ l_iCtr ] == ( a_puhFlags [ l_iCtrLoc ] & 0x00ff ) )
						{
						a_puhFlags [ l_iCtrLoc ] |= 0x0100;
						break;
						}
				if ( l_iCtrLoc >= a_iFlagsCount )
					{
					f_bIgnoreCase = l_bLocalCopyIgnoreCase;
					f_bExtended = l_bLocalCopyExtended;
					for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
						a_puhFlags [ l_iCtrLoc ] = l_oLocalCopyFlags [ l_iCtrLoc ];
					l_bError = true;
					f_oError.format ( "bad search option '%c'",
							l_pcPattern [ l_iCtr ] );
					return ( l_bError );
					}
				break;
				}
			}
		l_iCtr ++;
		}
	if ( l_pcPattern [ l_iCtr ] == '/' )l_iCtr ++;
	l_iBegin = l_iCtr;
	l_iEnd = l_iCtr = f_oPatternInput.get_length ( ) - 1;
	l_bLocalCopyIgnoreCase = f_bIgnoreCase;
	l_bLocalCopyExtended = f_bExtended;
	for ( l_iCtr = 0; l_iCtr < a_iFlagsCount; l_iCtr ++ )
		l_oLocalCopyFlags [ l_iCtr ] = a_puhFlags [ l_iCtr ];
	while ( ( l_iCtr > 0 ) && ( l_pcPattern [ l_iCtr ] != '/' ) )
		{
		switch ( l_pcPattern [ l_iCtr ] )
			{
			case ( 'i' ):{f_bIgnoreCase = ! f_bIgnoreCase;break;}
			case ( 'e' ):{f_bExtended = true;break;}
			case ( 'c' ):{f_bIgnoreCase = true;break;}
			case ( 'C' ):{f_bIgnoreCase = false;break;}
			default :
				{
				for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
					if ( l_pcPattern [ l_iCtr ] == ( a_puhFlags [ l_iCtrLoc ] & 0x00ff ) )
						{
						a_puhFlags [ l_iCtrLoc ] |= 0x0100;
						break;
						}
				if ( l_iCtrLoc >= a_iFlagsCount )
					{
					f_bIgnoreCase = l_bLocalCopyIgnoreCase;
					f_bExtended = l_bLocalCopyExtended;
					for ( l_iCtrLoc = 0; l_iCtrLoc < a_iFlagsCount; l_iCtrLoc ++ )
						a_puhFlags [ l_iCtrLoc ] = l_oLocalCopyFlags [ l_iCtrLoc ];
					l_iCtr = 1;
					}
				break;
				}
			}
		l_iCtr --;
		}
	if ( l_iCtr )l_iEnd = l_iCtr - 1;
	f_oPatternReal = f_oPatternInput.mid ( l_iBegin,
			l_iEnd - l_iBegin + 1 );

	f_bInitialized = ! l_bError;
	return ( l_bError );
	M_EPILOG
	}

const char * HPattern::error ( void )
	{
	return ( f_oError );
	}


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	heditcontrol.c - this file is integral part of `stdhapi' project.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../config.h"

#ifdef __PLD_HOST__
#	include <ncurses/ncurses.h>
#else /* __PLD_HOST__ */
#	include <ncurses.h>
#endif /* not __PLD_HOST__ */

#include "heditcontrol.h"

#include "../hcore/xalloc.h"
#include "../hcore/hexception.h"

HEditControl::HEditControl( HWindow * a_poParent, 
		int a_iRow, int a_iColumn, int a_iWidth, int a_iBufferSize,
		const char * a_pcLabel, const char * a_pcValue, const char * a_pcMask,
		bool a_bReplace, bool a_bMultiLine,	bool a_bPassword, 
		int a_iMaxHistoryLevel )
					: HControl ( a_poParent, a_iRow, a_iColumn, 1, a_iWidth, a_pcLabel ),
							f_oString ( ( unsigned long ) a_iBufferSize )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char * l_pcBuffer = 0;
	HString l_oErrorMessage;
	if ( ! console::is_enabled ( ) )
		throw new HException ( __WHERE__, "not in curses mode.", g_iErrNo );
	if ( a_iBufferSize < 1 )
		throw new HException ( __WHERE__, "buffer size is ridiculously low",
				a_iBufferSize );
	if ( a_pcValue )
		{
		l_iLength = strlen ( a_pcValue );
		if ( l_iLength > a_iBufferSize )
			throw new HException ( __WHERE__, "initial value too big",
					l_iLength - a_iBufferSize );
		}
	f_oVarTmpBuffer.hs_realloc ( a_iBufferSize + 1 );
	f_iControlOffset = 0;
	f_iCursorPosition = 0;
	f_iMaxStringSize = a_iBufferSize;
	f_iMaxHistoryLevel = a_iMaxHistoryLevel;
	f_bReplace = a_bReplace;
	f_bMultiLine = a_bMultiLine;
	f_bPassword = a_bPassword;
	f_oString = a_pcValue;
	f_oHistory.add_tail ( HString ( "" ) );
	if ( ( l_iErrorCode = regcomp ( & f_sMask, a_pcMask, REG_EXTENDED | REG_NOSUB ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = ( char * ) xmalloc ( l_iLength );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage = l_pcBuffer;
		xfree ( ( void * ) l_pcBuffer );
		throw new HException ( __WHERE__, l_oErrorMessage, l_iErrorCode );
		}
	if ( ( l_iErrorCode = regexec ( & f_sMask, a_pcValue, 0, NULL, 0 ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = ( char * ) xmalloc ( l_iLength );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage = l_pcBuffer;
		xfree ( ( void * ) l_pcBuffer );
		throw new HException ( __WHERE__, l_oErrorMessage, l_iErrorCode );
		}
	l_iLength = f_oString.get_length ( );
/* this is part of draw_label ( ) method, we can not wait with setting up
 * f_iWidthRaw until draw_label ( ), which is called from refresh ( )
 * because ... see next comment */
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: console::n_iWidth + f_iWidth - f_iColumnRaw;
/* f_iWidthRaw must be set up properly before setting up f_iCursorPosition and
 * f_iControlOffset whose are used in refresh ( ) */
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = l_iLength - f_iWidthRaw + 1;
		}
	else f_iCursorPosition = l_iLength;
#ifdef __DEBUG__
#endif /* __DEBUG__ */
	refresh ( );
	return ;
	M_EPILOG
	}
	
HEditControl::~HEditControl( void )
	{
	M_PROLOG
	regfree ( & f_sMask );
	return;
	M_EPILOG
	}
	
void HEditControl::refresh ( void )
	{
	M_PROLOG
	draw_label ( );
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( f_oVarTmpBuffer, ' ', f_iWidthRaw );
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	if ( ! f_bPassword )
		{
		strcpy ( f_oVarTmpBuffer, ( char * ) f_oString + f_iControlOffset );
		f_oVarTmpBuffer [ f_oVarTmpBuffer.get_length ( ) ] = ' ';
		}
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	cprintf ( f_oVarTmpBuffer );
	move ( f_iRowRaw,	f_iColumnRaw + ( f_bPassword ? 0 : f_iCursorPosition ) );
	curs_set ( 1 );
	return;
	M_EPILOG
	}

HInfo HEditControl::operator = ( const HInfo & a_roInfo )
	{
	M_PROLOG
	HInfo l_oInfo = a_roInfo;
	HString l_oString = l_oInfo;	
	( * this ) = l_oString.left ( f_iMaxStringSize );
	return ( l_oInfo );
	M_EPILOG
	}

void HEditControl::set_flags ( bool a_bReplace, bool a_bPassword )
	{
	M_PROLOG
	f_bReplace = a_bReplace;
	f_bPassword = a_bPassword;
	return;
	M_EPILOG
	}

const char * strrnpbrk ( const char * a_pcBuffer, const char * a_pcStopSet,
		int a_iIndex )
	{
	int l_iCtr = 0;
	int l_iStopSetSize = strlen ( a_pcStopSet );
	while ( a_iIndex -- )
		for ( l_iCtr = 0; l_iCtr < l_iStopSetSize; l_iCtr ++ )
			if ( a_pcBuffer [ a_iIndex ] == a_pcStopSet [ l_iCtr ] )
				return ( a_pcBuffer + a_iIndex );
	return ( NULL );
	}

const char * strrpbrk ( const char * a_pcBuffer, const char * a_pcStopSet )
	{
	return ( strrnpbrk ( a_pcBuffer, a_pcStopSet, strlen ( a_pcBuffer ) ) );
	}

size_t strrnspn ( const char * a_pcBuffer, const char * a_pcSkipSet,
		int a_iLenght )
	{
	int l_iCtr = 0;
	int l_iSkipSetSize = strlen ( a_pcSkipSet );
	int l_iIndex = a_iLenght;
	while ( l_iIndex -- )
		{
		for ( l_iCtr = 0; l_iCtr < l_iSkipSetSize; l_iCtr ++ )
			if ( a_pcBuffer [ l_iIndex ] == a_pcSkipSet [ l_iCtr ] )
				break;
		if ( l_iCtr >= l_iSkipSetSize )return ( a_iLenght - l_iIndex );
		}
	return ( 0 );
	}

size_t strrspn ( const char * a_pcBuffer, const char * a_pcSkipSet )
	{
	return ( strrnspn ( a_pcBuffer, a_pcSkipSet, strlen ( a_pcBuffer ) ) );
	}

const char g_pcWhiteSpace [ ] = " \t\n()[].,!?;{}+-*/";

int HEditControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iLength = 0;
	int l_iErrorCode = 0;
	int l_iOldControlOffset = 0;
	int l_iOldCursorPosition = 0;
	char * l_pcBuffer = 0;
	a_iCode = HControl::process_input ( a_iCode );
	l_pcBuffer = ( char * ) f_oVarTmpBuffer;
	memset ( l_pcBuffer, 0, f_iMaxStringSize );
	f_oVarTmpBuffer = f_oString;
	l_iOldControlOffset = f_iControlOffset;
	l_iOldCursorPosition = f_iCursorPosition;
	l_iLength = f_oVarTmpBuffer.get_length ( );
	switch ( a_iCode )
		{
		case ( KEY_PPAGE ):
			{
			f_oHistory.go ( 0 );	
			f_oHistory.to_tail ( );
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_NPAGE ):
			{
			f_oHistory.go ( 0 );	
			f_oHistory.to_head ( 2 );
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_UP ):
			{
			f_oHistory.to_head ( );
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_DOWN ):
			{
			f_oHistory.to_tail ( );
			l_iErrorCode = -1;
			break;
			}
		case ( '\t' ):
			{
			f_bFocused = false;
			}
		case ( '\r' ):
			{
			l_iErrorCode = f_oHistory.quantity ( );
			l_iErrorCode ++;
			while ( -- l_iErrorCode )
				if ( f_oHistory.to_tail ( ) == f_oString )break;
			if ( f_oString.get_length ( ) &&  ( ! l_iErrorCode ) )
				{
				f_oHistory.add_tail ( f_oString );
				while ( f_oHistory.quantity ( ) > ( int ) f_iMaxHistoryLevel )
					f_oHistory.remove_at ( 1, D_EMPTY_IF_NOT_EMPTIED );
				f_oHistory.go ( 0 );
				f_oHistory.to_head ( );
				}
			l_iErrorCode = a_iCode;
			break;
			}
		case ( KEY_LEFT ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iCursorPosition > 0 )f_iCursorPosition --;
				else if ( f_iControlOffset > 0 )f_iControlOffset --;
				}
			else putchar ( '\a' );
			break;
			}
		case ( D_KEY_CTRL_('a') ):
		case ( KEY_HOME ):
			{
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
			break;
			}
		case ( D_KEY_CTRL_('e') ):
		case ( KEY_END ):
			{
			if ( l_iLength >= f_iWidthRaw )
				{
				f_iCursorPosition = f_iWidthRaw - 1;
				f_iControlOffset = l_iLength - f_iWidthRaw + 1;
				}
			else f_iCursorPosition = l_iLength;
			break;
			}
		case ( KEY_RIGHT ):
			{
			if ( ( f_iCursorPosition + f_iControlOffset ) < l_iLength )
				{
				f_iCursorPosition ++;
				if ( f_iCursorPosition >= f_iWidthRaw )
					{
					f_iCursorPosition = f_iWidthRaw - 1;
					f_iControlOffset ++;
					}
				}
			else putchar ( '\a' );
			break;
			}
		case ( KEY_DELETE ):
			{
			if ( ! f_bReplace )
				{
				if ( ( f_iControlOffset + f_iCursorPosition ) >= l_iLength )
					{
					l_iLength = 0;
					f_iControlOffset = 0;
					f_iCursorPosition = 0;
					break;
					}
				memmove ( l_pcBuffer + f_iControlOffset+ f_iCursorPosition, l_pcBuffer + f_iControlOffset + f_iCursorPosition + 1, l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
				l_iLength --;
				}
			else putchar ( '\a' );
			break;
			}
		case ( KEY_BS ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iControlOffset > 0 )f_iControlOffset --;
				else if ( f_iCursorPosition > 0 )f_iCursorPosition --;
				if ( ! f_bReplace )
					{
					memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
							l_pcBuffer + f_iControlOffset + f_iCursorPosition + 1,
							l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
					l_iLength --;
					}
				}
			else putchar ( '\a' );
			break;
			}
		case ( KEY_IC ):
			{
			f_bReplace = ! f_bReplace;
			break;
			}
		case ( D_KEY_META_('f') ):
			{
			l_pcBuffer += ( f_iControlOffset + f_iCursorPosition );
			l_iErrorCode = strpbrk ( l_pcBuffer, g_pcWhiteSpace ) - l_pcBuffer;
			if ( l_iErrorCode > -1 )
				l_iErrorCode += strspn ( l_pcBuffer + l_iErrorCode, g_pcWhiteSpace );
			else l_iErrorCode = l_iLength - ( f_iControlOffset + f_iCursorPosition );
			f_iCursorPosition += l_iErrorCode;
			if ( f_iCursorPosition >= f_iWidthRaw )
				{
				f_iControlOffset += f_iCursorPosition - f_iWidthRaw + 1;
				f_iCursorPosition = f_iWidthRaw - 1;
				}
			l_pcBuffer = ( char * ) f_oVarTmpBuffer;
			l_iErrorCode = 0;
			if ( ( l_iOldCursorPosition == f_iCursorPosition )
				&& ( l_iOldControlOffset == f_iControlOffset ) )
				putchar ( '\a' );
			break;
			}
		case ( D_KEY_META_('b') ):
			{
			if ( f_iControlOffset + f_iCursorPosition )
				{
				l_iErrorCode = strrnspn ( l_pcBuffer, g_pcWhiteSpace,
						f_iControlOffset + f_iCursorPosition );
				l_pcBuffer = ( char * ) strrnpbrk ( l_pcBuffer, g_pcWhiteSpace,
						f_iControlOffset + f_iCursorPosition - l_iErrorCode );
				if ( l_pcBuffer )
					{
					f_iCursorPosition = 1 - f_iControlOffset + l_pcBuffer
						- ( char * ) f_oVarTmpBuffer;
					if ( f_iCursorPosition < 0 )
						{
						f_iControlOffset += f_iCursorPosition;
						f_iCursorPosition = 0;
						}
					}
				else
					{
					f_iControlOffset = 0;
					f_iCursorPosition = 0;
					}
				l_pcBuffer = ( char * ) f_oVarTmpBuffer;
				l_iErrorCode = 0;
				}
			else putchar ( '\a' );
			break;
			}
		case ( D_KEY_META_('d') ):
			{
			if ( ! f_bReplace )
				{
				if ( ( f_iControlOffset + f_iCursorPosition ) >= l_iLength )
					{
					l_iLength = 0;
					f_iControlOffset = 0;
					f_iCursorPosition = 0;
					break;
					}
				l_pcBuffer += ( f_iControlOffset + f_iCursorPosition );
				l_iErrorCode = strpbrk ( l_pcBuffer, g_pcWhiteSpace ) - l_pcBuffer;
				if ( l_iErrorCode > -1 )
					l_iErrorCode += strspn ( l_pcBuffer + l_iErrorCode, g_pcWhiteSpace );
				else
					l_iErrorCode = l_iLength - ( f_iControlOffset + f_iCursorPosition );
				l_pcBuffer = ( char * ) f_oVarTmpBuffer;
				l_iLength -= l_iErrorCode;
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + f_iControlOffset + f_iCursorPosition + l_iErrorCode,
						l_iLength - f_iControlOffset - f_iCursorPosition + 1);
				l_iErrorCode = 0;
				}
			else putchar ( '\a' );
			break;
			}
		case ( D_KEY_CTRL_('w') ):
			{
			if ( ! f_bReplace )
				{
				if ( f_iControlOffset + f_iCursorPosition )
					{
					l_iErrorCode = strrnspn ( l_pcBuffer, g_pcWhiteSpace,
							f_iControlOffset + f_iCursorPosition );
					l_pcBuffer = ( char * ) strrnpbrk ( l_pcBuffer, g_pcWhiteSpace,
							f_iControlOffset + f_iCursorPosition - l_iErrorCode );
					if ( l_pcBuffer )
						{
						f_iCursorPosition = 1 - f_iControlOffset + l_pcBuffer
							- ( char * ) f_oVarTmpBuffer;
						if ( f_iCursorPosition < 0 )
							{
							f_iControlOffset += f_iCursorPosition;
							f_iCursorPosition = 0;
							}
						}
					else
						{
						f_iControlOffset = 0;
						f_iCursorPosition = 0;
						}
					l_pcBuffer = ( char * ) f_oVarTmpBuffer;
					l_iErrorCode = 0;
					}
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + l_iOldControlOffset + l_iOldCursorPosition,
						l_iLength - l_iOldControlOffset - l_iOldCursorPosition + 1);
				l_iLength = f_oVarTmpBuffer.get_length ( );
				}
			else putchar ( '\a' );
			break;
			}
		default:
			{
			if ( ( a_iCode > 31 ) && ( a_iCode < 256 ) )
				{
				if ( ( ! f_bReplace && ( l_iLength < f_iMaxStringSize ) ) 
|| ( f_bReplace && ( ( f_iControlOffset + f_iCursorPosition ) < l_iLength ) )	)
					{
					if ( ! f_bReplace )
						memmove ( l_pcBuffer + f_iControlOffset+ f_iCursorPosition + 1, l_pcBuffer + f_iControlOffset + f_iCursorPosition, l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
					l_pcBuffer [ f_iCursorPosition + f_iControlOffset ] = a_iCode;
					l_iLength ++;
					f_iCursorPosition ++;
					if ( f_iCursorPosition >= f_iWidthRaw )
						{
						f_iCursorPosition = f_iWidthRaw - 1;
						f_iControlOffset ++;
						}
					}
				else putchar ( '\a' );
				}
			else l_iErrorCode = a_iCode;
			break;
			}
		}
	if ( l_iErrorCode < 0 )
		{
		l_iErrorCode = 0;
		if ( f_oHistory.quantity ( ) )f_oVarTmpBuffer = f_oHistory.present ( );
		l_iLength = f_oVarTmpBuffer.get_length ( );
		if ( l_iLength >= f_iWidthRaw )
			{
			f_iCursorPosition = f_iWidthRaw - 1;
			f_iControlOffset = l_iLength - f_iWidthRaw + 1;
			}
		else 
			{
			f_iControlOffset = 0;
			f_iCursorPosition = l_iLength;
			}
		}
	l_pcBuffer [ l_iLength ] = 0;
	a_iCode = l_iErrorCode;
	l_iErrorCode = regexec ( & f_sMask, f_oVarTmpBuffer, 0, NULL, 0 );
	if ( ! l_iErrorCode )
		{
		f_oString = f_oVarTmpBuffer;
		refresh ( );
		console::c_printf ( console::n_iHeight - 1, -1, D_FG_LIGHTGRAY, "" );
		}
	else
		{
		f_iControlOffset = l_iOldControlOffset;
		f_iCursorPosition = l_iOldCursorPosition;
		putchar ( '\a' );
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = ( char * ) xmalloc ( l_iLength );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		console::c_printf ( console::n_iHeight - 1, -1, D_BG_BROWN, l_pcBuffer );
		xfree ( ( void * ) l_pcBuffer );
		}
	return ( a_iCode );
	M_EPILOG
	}

HString & HEditControl::operator = ( const HString & a_roString )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char * l_pcBuffer = 0;
	HString l_oErrorMessage;
	if ( ( l_iErrorCode = regexec ( & f_sMask, ( HString & ) a_roString,
					0, NULL, 0 ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = ( char * ) xmalloc ( l_iLength );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage = l_pcBuffer;
		xfree ( ( void * ) l_pcBuffer );
		throw new HException ( __WHERE__, l_oErrorMessage, l_iErrorCode );
		}
	f_oString = a_roString;
	l_iLength = f_oString.get_length ( );
	f_iControlOffset = 0;
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = l_iLength - f_iWidthRaw + 1;
		}
	else f_iCursorPosition = l_iLength;
	refresh ( );
	return ( f_oString );
	M_EPILOG
	}

HEditControl::operator HString ( )
	{
	M_PROLOG
	return ( f_oString );
	M_EPILOG
	}

int HEditControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	curs_set ( 1 );
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

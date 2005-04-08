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

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/xalloc.h"
#include "heditcontrol.h"
#include "hwindow.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HEditControl::HEditControl( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel, size_t a_iBufferSize, char const * a_pcValue,
		char const * a_pcMask, bool a_bReplace, bool a_bRightAligned,
		bool a_bMultiLine, bool a_bPassword, int a_iMaxHistoryLevel,
		bool a_bDrawLabel, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
					: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
							a_iWidth, a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
							a_iEnabledAttribute, a_iFocusedAttribute ),
					f_bReplace ( a_bReplace ),
					f_bMultiLine ( a_bMultiLine || ( a_iHeight > 1 ) ? true : false ),
					f_bPassword ( a_bPassword ), f_bRightAligned ( a_bRightAligned ),
					f_iMaxStringSize ( a_iBufferSize ), f_iCursorPosition ( 0 ),
					f_iControlOffset ( 0 ), f_iMaxHistoryLevel ( a_iMaxHistoryLevel ),
					f_sMask ( ), f_oString ( a_iBufferSize ), f_oHistory ( )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	size_t l_iLength = 0;
	char * l_pcBuffer = 0;
	HString l_oErrorMessage;
	if ( a_iBufferSize < 1 )
		M_THROW ( "buffer size is ridiculously low",
				a_iBufferSize );
	if ( a_pcValue )
		{
		l_iLength = strlen ( a_pcValue );
		if ( l_iLength > a_iBufferSize )
			M_THROW ( "initial value too big",
					l_iLength - a_iBufferSize );
		}
	f_oVarTmpBuffer.hs_realloc ( a_iBufferSize + 1 );
	if ( f_bRightAligned && f_bMultiLine )
		M_THROW (
				"edit-control right aligned and multiline at the same time", 0 );
	f_oString = a_pcValue;
	f_oHistory.add_tail ( ) = "";
	if ( ( l_iErrorCode = regcomp ( & f_sMask, a_pcMask,
					REG_EXTENDED | REG_NOSUB ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = xmalloc ( l_iLength, char );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage = l_pcBuffer;
		xfree ( l_pcBuffer );
		M_THROW ( l_oErrorMessage, l_iErrorCode );
		}
	if ( ( l_iErrorCode = regexec ( & f_sMask, a_pcValue, 0, NULL, 0 ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = xmalloc ( l_iLength, char );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage.format ( "%s: %s", l_pcBuffer, a_pcValue );
		xfree ( l_pcBuffer );
		M_THROW ( l_oErrorMessage, l_iErrorCode );
		}
	l_iLength = f_oString.get_length ( );
/* this is part of draw_label ( ) method, we can not wait with setting up
 * f_iWidthRaw until draw_label ( ), which is called from refresh ( )
 * because ... see next comment */
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: n_iWidth + f_iWidth - f_iColumnRaw;
/* f_iWidthRaw must be set up properly before setting up f_iCursorPosition and
 * f_iControlOffset whose are used in refresh ( ) */
	if ( l_iLength >= static_cast < size_t > ( f_iWidthRaw ) )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = l_iLength - f_iWidthRaw + 1;
		}
	else
		f_iCursorPosition = l_iLength;
#ifdef __DEBUG__
#endif /* __DEBUG__ */
	refresh ( );
	return;
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
	if ( ! f_bPassword )
		{
		strcpy ( f_oVarTmpBuffer, static_cast < char * > ( f_oString ) + f_iControlOffset );
		f_oVarTmpBuffer [ f_oVarTmpBuffer.get_length ( ) ] = ' ';
		}
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	::mvprintw ( f_iRowRaw, f_iColumnRaw, f_oVarTmpBuffer );
	if ( f_bFocused )
		{
		::move ( f_iRowRaw,
				f_iColumnRaw + ( f_bPassword ? 0 : f_iCursorPosition ) );
		curs_set ( f_bReplace ? D_CURSOR_VERY_VISIBLE : D_CURSOR_VISIBLE );
		}
	return;
	M_EPILOG
	}

void HEditControl::set ( const HInfo & a_roInfo )
	{
	M_PROLOG
	HInfo l_oInfo = a_roInfo;
	HString l_oString = l_oInfo;	
	set ( static_cast < char const * > ( l_oString.left ( f_iMaxStringSize ) ) );
	return;
	M_EPILOG
	}

HInfo HEditControl::get ( void )
	{
	M_PROLOG
	return ( HInfo ( f_oString ) );
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

/* all str* and mem* functions takes const pointer as argument and returns
	 non const pointer */
char * strrnpbrk ( char const * a_pcBuffer, char const * a_pcStopSet,
		int a_iIndex )
	{
	int l_iCtr = 0;
	int l_iStopSetSize = strlen ( a_pcStopSet );
	while ( a_iIndex -- )
		for ( l_iCtr = 0; l_iCtr < l_iStopSetSize; l_iCtr ++ )
			if ( a_pcBuffer [ a_iIndex ] == a_pcStopSet [ l_iCtr ] )
				return ( const_cast < char * > ( a_pcBuffer + a_iIndex ) );
	return ( NULL );
	}

char const * strrpbrk ( char const * a_pcBuffer, char const * a_pcStopSet )
	{
	return ( strrnpbrk ( a_pcBuffer, a_pcStopSet, strlen ( a_pcBuffer ) ) );
	}

size_t strrnspn ( char const * a_pcBuffer, char const * a_pcSkipSet,
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
		if ( l_iCtr >= l_iSkipSetSize )
			return ( a_iLenght - l_iIndex );
		}
	return ( 0 );
	}

size_t strrspn ( char const * a_pcBuffer, char const * a_pcSkipSet )
	{
	return ( strrnspn ( a_pcBuffer, a_pcSkipSet, strlen ( a_pcBuffer ) ) );
	}

char const g_pcWhiteSpace [ ] = " \t\n()[].,!?;{}+-*/";

int HEditControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iLength = 0;
	int l_iErrorCode = 0;
	int l_iOldControlOffset = 0;
	int l_iOldCursorPosition = 0;
	char * l_pcBuffer = 0;
	a_iCode = HControl::process_input ( a_iCode );
	l_pcBuffer = static_cast < char * > ( f_oVarTmpBuffer );
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
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_NPAGE ):
			{
			f_oHistory.go ( 0 );	
			f_oHistory.to_head ( );
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_UP ):
			{
			f_oHistory.to_tail ( );
			l_iErrorCode = -1;
			break;
			}
		case ( KEY_DOWN ):
			{
			f_oHistory.to_head ( );
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
				if ( ( * f_oHistory.to_tail ( ) ) == f_oString )
					break;
			if ( f_oString.get_length ( ) &&  ( ! l_iErrorCode ) )
				{
				f_oHistory.add_head ( & f_oString );
				l_iErrorCode = f_oHistory.quantity ( );
				while ( l_iErrorCode -- > static_cast < int > ( f_iMaxHistoryLevel ) )
					f_oHistory.remove_at ( l_iErrorCode, D_EMPTY_IF_NOT_EMPTIED );
				f_oHistory.go ( 0 );
				f_oHistory.to_head ( );
				}
			else
				f_oHistory.to_head ( );
			l_iErrorCode = a_iCode;
			break;
			}
		case ( KEY_LEFT ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iCursorPosition > 0 )
					f_iCursorPosition --;
				else if ( f_iControlOffset > 0 )
					f_iControlOffset --;
				}
			else
				putchar ( '\a' );
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
			else
				f_iCursorPosition = l_iLength;
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
			else
				putchar ( '\a' );
			break;
			}
		case ( D_KEY_CTRL_( 'u' ) ):
			{
			l_iLength = 0;
			f_iControlOffset = 0;
			f_iCursorPosition = 0;
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
			else
				putchar ( '\a' );
			break;
			}
		case ( KEY_BS ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iControlOffset > 0 )
					f_iControlOffset --;
				else if ( f_iCursorPosition > 0 )
					f_iCursorPosition --;
				if ( ! f_bReplace )
					{
					memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
							l_pcBuffer + f_iControlOffset + f_iCursorPosition + 1,
							l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
					l_iLength --;
					}
				}
			else
				putchar ( '\a' );
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
			else
				l_iErrorCode = l_iLength - ( f_iControlOffset + f_iCursorPosition );
			f_iCursorPosition += l_iErrorCode;
			if ( f_iCursorPosition >= f_iWidthRaw )
				{
				f_iControlOffset += f_iCursorPosition - f_iWidthRaw + 1;
				f_iCursorPosition = f_iWidthRaw - 1;
				}
			l_pcBuffer = static_cast < char * > ( f_oVarTmpBuffer );
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
				l_pcBuffer = strrnpbrk ( l_pcBuffer, g_pcWhiteSpace,
						f_iControlOffset + f_iCursorPosition - l_iErrorCode );
				if ( l_pcBuffer )
					{
					f_iCursorPosition = 1 - f_iControlOffset + l_pcBuffer
						- static_cast < char * > ( f_oVarTmpBuffer );
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
				l_pcBuffer = static_cast < char * > ( f_oVarTmpBuffer );
				l_iErrorCode = 0;
				}
			else
				putchar ( '\a' );
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
				l_pcBuffer = static_cast < char * > ( f_oVarTmpBuffer );
				l_iLength -= l_iErrorCode;
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + f_iControlOffset + f_iCursorPosition + l_iErrorCode,
						l_iLength - f_iControlOffset - f_iCursorPosition + 1);
				l_iErrorCode = 0;
				}
			else
				putchar ( '\a' );
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
					l_pcBuffer = strrnpbrk ( l_pcBuffer, g_pcWhiteSpace,
							f_iControlOffset + f_iCursorPosition - l_iErrorCode );
					if ( l_pcBuffer )
						{
						f_iCursorPosition = 1 - f_iControlOffset + l_pcBuffer
							- static_cast < char * > ( f_oVarTmpBuffer );
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
					l_pcBuffer = static_cast < char * > ( f_oVarTmpBuffer );
					l_iErrorCode = 0;
					}
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + l_iOldControlOffset + l_iOldCursorPosition,
						l_iLength - l_iOldControlOffset - l_iOldCursorPosition + 1);
				l_iLength = f_oVarTmpBuffer.get_length ( );
				}
			else
				putchar ( '\a' );
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
				else
					putchar ( '\a' );
				}
			else
				l_iErrorCode = a_iCode;
			break;
			}
		}
	if ( l_iErrorCode < 0 )
		{
		l_iErrorCode = 0;
		if ( f_oHistory.quantity ( ) )
			f_oVarTmpBuffer = f_oHistory.present ( );
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
		f_poParent->status_bar ( )->message ( D_FG_LIGHTGRAY, "" );
		refresh ( );
		}
	else
		{
		f_iControlOffset = l_iOldControlOffset;
		f_iCursorPosition = l_iOldCursorPosition;
		putchar ( '\a' );
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = xmalloc ( l_iLength, char );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		f_poParent->status_bar ( )->message ( D_BG_BROWN, l_pcBuffer );
		xfree ( l_pcBuffer );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HEditControl::set ( char const * a_pcString )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char * l_pcBuffer = 0;
	HString l_oErrorMessage;
	if ( ( l_iErrorCode = regexec ( & f_sMask, a_pcString, 0, NULL, 0 ) ) )
		{
		l_iLength = regerror ( l_iErrorCode, & f_sMask, NULL, 0 );
		l_pcBuffer = xmalloc ( l_iLength, char );
		regerror ( l_iErrorCode, & f_sMask, l_pcBuffer, l_iLength );
		l_oErrorMessage.format ( "%s: %s", l_pcBuffer, a_pcString );
		xfree ( l_pcBuffer );
		M_THROW ( l_oErrorMessage, l_iErrorCode );
		}
	f_oString = a_pcString;
	l_iLength = f_oString.get_length ( );
	f_iControlOffset = 0;
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = l_iLength - f_iWidthRaw + 1;
		}
	else
		f_iCursorPosition = l_iLength;
	l_iErrorCode = f_oHistory.quantity ( );
	while ( l_iErrorCode )
		{
		if ( ( * f_oHistory.to_tail ( ) ) == f_oString )
			{
			f_oHistory.to_head ( );
			break;
			}
		l_iErrorCode --;
		}
	refresh ( );
	return;
	M_EPILOG
	}

int HEditControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

int HEditControl::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iPosition = 0;
	if ( ! HControl::click ( a_rsMouse ) )
		return ( 1 );
	l_iPosition = a_rsMouse.f_iColumn - f_iColumnRaw;
	if ( l_iPosition < f_oString.get_length ( ) )
		{
		f_iCursorPosition = l_iPosition;
		refresh ( );
		}
	return ( 0 );
	M_EPILOG
	}

}

}


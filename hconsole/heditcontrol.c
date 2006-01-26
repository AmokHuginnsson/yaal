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

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "heditcontrol.h"
#include "hwindow.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HEditControl::HEditControl( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel, int a_iBufferSize, char const * a_pcValue,
		char const * a_pcMask, bool a_bReplace, bool a_bRightAligned,
		bool a_bMultiLine, bool a_bReadOnly, bool a_bPassword,
		int a_iMaxHistoryLevel, bool a_bDrawLabel, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
					: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
							a_iWidth, a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
							a_iEnabledAttribute, a_iFocusedAttribute ),
					f_bReplace ( a_bReplace ),
					f_bMultiLine ( a_bMultiLine || ( a_iHeight > 1 ) ? true : false ),
					f_bReadOnly ( a_bReadOnly ), f_bRightAligned ( a_bRightAligned ),
					f_bPassword ( a_bPassword ),
					f_iMaxStringSize ( a_iBufferSize ), f_iCursorPosition ( 0 ),
					f_iControlOffset ( 0 ), f_iMaxHistoryLevel ( a_iMaxHistoryLevel ),
					f_oPattern ( ), f_oString ( a_iBufferSize, true ), f_oHistory ( )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
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
	if ( ( l_iErrorCode = f_oPattern.parse_re ( a_pcMask ) ) )
		M_THROW ( f_oPattern.error ( ), l_iErrorCode );
	M_IRV ( f_oPattern.matches ( a_pcValue, NULL, & l_iErrorCode ) );
	if ( l_iErrorCode )
		M_THROW ( f_oPattern.error ( ), l_iErrorCode );
	l_iLength = f_oString.get_length ( );
/* this is part of draw_label ( ) method, we can not wait with setting up
 * f_iWidthRaw until draw_label ( ), which is called from refresh ( )
 * because ... see next comment */
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: n_iWidth + f_iWidth - f_iColumnRaw;
/* f_iWidthRaw must be set up properly before setting up f_iCursorPosition and
 * f_iControlOffset whose are used in refresh ( ) */
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
		}
	else
		f_iCursorPosition = l_iLength;
#ifdef __DEBUG__
#endif /* __DEBUG__ */
	HEditControl::refresh ( );
	return;
	M_EPILOG
	}
	
HEditControl::~HEditControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HEditControl::refresh ( void )
	{
	M_PROLOG
	draw_label ( );
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( f_oVarTmpBuffer.raw ( ), ' ', f_iWidthRaw );
	if ( ! f_bPassword )
		{
		strcpy ( f_oVarTmpBuffer.raw ( ), static_cast < char const * const > ( f_oString ) + f_iControlOffset );
		f_oVarTmpBuffer [ f_oVarTmpBuffer.get_length ( ) ] = ' ';
		}
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	M_ENSURE ( c_mvprintf ( f_iRowRaw, f_iColumnRaw, f_oVarTmpBuffer ) != C_ERR );
	if ( f_bFocused )
		{
		M_ENSURE ( c_move ( f_iRowRaw,
					f_iColumnRaw + ( f_bPassword ? 0 : f_iCursorPosition ) ) != C_ERR );
		M_IRV ( curs_set ( f_bReplace ? D_CURSOR_VERY_VISIBLE : D_CURSOR_VISIBLE ) );
		}
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

char const g_pcWordSeparator [ ] = " \t\n`-=[]\\;',./~!@#$%^&*()+{}|:\"<>?";

int HEditControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iLength = 0;
	int l_iErrorCode = 0;
	int l_iOldControlOffset = 0;
	int l_iOldCursorPosition = 0;
	char * l_pcBuffer = 0;
	a_iCode = HControl::process_input ( a_iCode );
	l_pcBuffer = f_oVarTmpBuffer.raw ( );
	memset ( l_pcBuffer, 0, f_iMaxStringSize );
	f_oVarTmpBuffer = f_oString;
	l_iOldControlOffset = f_iControlOffset;
	l_iOldCursorPosition = f_iCursorPosition;
	l_iLength = f_oVarTmpBuffer.get_length ( );
	switch ( a_iCode )
		{
		case ( KEY_CODES::D_PAGE_UP ):
			{
			M_IRV ( f_oHistory.go ( 0 ) );
			l_iErrorCode = - 1;
			break;
			}
		case ( KEY_CODES::D_PAGE_DOWN ):
			{
			M_IRV ( f_oHistory.go ( - 1 ) );	
			l_iErrorCode = - 1;
			break;
			}
		case ( KEY_CODES::D_UP ):
			{
			M_IRV ( f_oHistory.to_tail ( ) );
			l_iErrorCode = - 1;
			break;
			}
		case ( KEY_CODES::D_DOWN ):
			{
			M_IRV ( f_oHistory.to_head ( ) );
			l_iErrorCode = - 1;
			break;
			}
		case ( '\t' ):
			{
			f_bFocused = false;
			}
		/* enter works like tab without focus movement */
		case ( '\r' ):
			{
			l_iErrorCode = f_oHistory.quantity ( );
			l_iErrorCode ++;
			while ( -- l_iErrorCode )
				if ( ( * f_oHistory.to_tail ( ) ) == f_oString )
					break;
			if ( f_oString.get_length ( ) && ( ! l_iErrorCode ) )
				{
				M_IRV ( f_oHistory.add_head ( & f_oString ) );
				l_iErrorCode = f_oHistory.quantity ( );
				while ( l_iErrorCode -- > f_iMaxHistoryLevel )
					M_IRV ( f_oHistory.remove_at ( l_iErrorCode, history_t::D_EMPTY_IF_NOT_EMPTIED ) );
				M_IRV ( f_oHistory.go ( - 1 ) );
				}
			else
				M_IRV ( f_oHistory.to_head ( ) );
			l_iErrorCode = a_iCode;
			break;
			}
		case ( KEY_CODES::D_LEFT ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iCursorPosition > 0 )
					f_iCursorPosition --;
				else if ( f_iControlOffset > 0 )
					f_iControlOffset --;
				}
			else
				bell ( );
			break;
			}
		case ( D_KEY_CTRL_('a') ):
		case ( KEY_CODES::D_HOME ):
			{
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
			break;
			}
		case ( D_KEY_CTRL_('e') ):
		case ( KEY_CODES::D_END ):
			{
			if ( l_iLength >= f_iWidthRaw )
				{
				f_iCursorPosition = f_iWidthRaw - 1;
				f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
				}
			else
				f_iCursorPosition = l_iLength;
			break;
			}
		case ( KEY_CODES::D_RIGHT ):
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
				bell ( );
			break;
			}
		case ( D_KEY_CTRL_( 'u' ) ):
			{
			if ( ! f_bReadOnly )
				{
				l_iLength = 0;
				f_iControlOffset = 0;
				f_iCursorPosition = 0;
				}
			else
				bell ( );
			break;
			}
		case ( KEY_CODES::D_DELETE ):
			{
			if ( ! ( f_bReadOnly || f_bReplace ) )
				{
				if ( ( f_iControlOffset + f_iCursorPosition ) >= l_iLength )
					{
					l_iLength = 0;
					f_iControlOffset = 0;
					f_iCursorPosition = 0;
					break;
					}
				memmove ( l_pcBuffer + f_iControlOffset+ f_iCursorPosition,
						l_pcBuffer + f_iControlOffset + f_iCursorPosition + 1,
						l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
				l_iLength --;
				}
			else
				bell ( );
			break;
			}
		case ( KEY_CODES::D_BACKSPACE ):
			{
			if ( ! f_bReadOnly && ( ( f_iControlOffset + f_iCursorPosition ) > 0 ) )
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
				bell ( );
			break;
			}
		case ( KEY_CODES::D_INSERT ):
			{
			f_bReplace = ! f_bReplace;
			break;
			}
		case ( D_KEY_META_('f') ):
			{
			l_pcBuffer += ( f_iControlOffset + f_iCursorPosition );
			l_iErrorCode = strpbrk ( l_pcBuffer, g_pcWordSeparator ) - l_pcBuffer;
			if ( l_iErrorCode > -1 )
				l_iErrorCode += strspn ( l_pcBuffer + l_iErrorCode, g_pcWordSeparator );
			else
				l_iErrorCode = l_iLength - ( f_iControlOffset + f_iCursorPosition );
			f_iCursorPosition += l_iErrorCode;
			if ( f_iCursorPosition >= f_iWidthRaw )
				{
				f_iControlOffset += ( ( f_iCursorPosition - f_iWidthRaw  ) + 1 );
				f_iCursorPosition = f_iWidthRaw - 1;
				}
			l_pcBuffer = f_oVarTmpBuffer.raw ( );
			l_iErrorCode = 0;
			if ( ( l_iOldCursorPosition == f_iCursorPosition )
					&& ( l_iOldControlOffset == f_iControlOffset ) )
				bell ( );
			break;
			}
		case ( D_KEY_META_('b') ):
			{
			if ( f_iControlOffset + f_iCursorPosition )
				{
				l_iErrorCode = strrnspn ( l_pcBuffer, g_pcWordSeparator,
						f_iControlOffset + f_iCursorPosition );
				l_pcBuffer = strrnpbrk ( l_pcBuffer, g_pcWordSeparator,
						f_iControlOffset + f_iCursorPosition - l_iErrorCode );
				if ( l_pcBuffer )
					{
					f_iCursorPosition = ( 1 - f_iControlOffset ) + l_pcBuffer
						- static_cast < char const * const > ( f_oVarTmpBuffer );
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
				l_pcBuffer = f_oVarTmpBuffer.raw ( );
				l_iErrorCode = 0;
				}
			else
				bell ( );
			break;
			}
		case ( D_KEY_META_('d') ):
			{
			if ( ! ( f_bReadOnly || f_bReplace ) )
				{
				if ( ( f_iControlOffset + f_iCursorPosition ) >= l_iLength )
					{
					l_iLength = 0;
					f_iControlOffset = 0;
					f_iCursorPosition = 0;
					break;
					}
				l_pcBuffer += ( f_iControlOffset + f_iCursorPosition );
				l_iErrorCode = strpbrk ( l_pcBuffer, g_pcWordSeparator ) - l_pcBuffer;
				if ( l_iErrorCode > -1 )
					l_iErrorCode += strspn ( l_pcBuffer + l_iErrorCode, g_pcWordSeparator );
				else
					l_iErrorCode = l_iLength - ( f_iControlOffset + f_iCursorPosition );
				l_pcBuffer = f_oVarTmpBuffer.raw ( );
				l_iLength -= l_iErrorCode;
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + f_iControlOffset + f_iCursorPosition + l_iErrorCode,
						( ( l_iLength - f_iControlOffset ) - f_iCursorPosition ) + 1 );
				l_iErrorCode = 0;
				}
			else
				bell ( );
			break;
			}
		case ( D_KEY_CTRL_('w') ):
			{
			if ( ! ( f_bReadOnly || f_bReplace ) )
				{
				if ( f_iControlOffset + f_iCursorPosition )
					{
					l_iErrorCode = strrnspn ( l_pcBuffer, g_pcWordSeparator,
							f_iControlOffset + f_iCursorPosition );
					l_pcBuffer = strrnpbrk ( l_pcBuffer, g_pcWordSeparator,
							f_iControlOffset + f_iCursorPosition - l_iErrorCode );
					if ( l_pcBuffer )
						{
						f_iCursorPosition = ( 1 - f_iControlOffset ) + l_pcBuffer
							- static_cast < char const * const > ( f_oVarTmpBuffer );
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
					l_pcBuffer = f_oVarTmpBuffer.raw ( );
					l_iErrorCode = 0;
					}
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + l_iOldControlOffset + l_iOldCursorPosition,
						( ( l_iLength - l_iOldControlOffset ) - l_iOldCursorPosition ) + 1 );
				l_iLength = f_oVarTmpBuffer.get_length ( );
				}
			else
				bell ( );
			break;
			}
		default:
			{
			if ( ! f_bReadOnly && ( a_iCode > 31 ) && ( a_iCode < 256 ) )
				{
				if ( ( ! f_bReplace && ( l_iLength < f_iMaxStringSize ) ) 
|| ( f_bReplace && ( ( f_iControlOffset + f_iCursorPosition ) < l_iLength ) )	)
					{
					if ( ! f_bReplace )
						memmove ( l_pcBuffer + f_iControlOffset+ f_iCursorPosition + 1,
								l_pcBuffer + f_iControlOffset + f_iCursorPosition,
								l_iLength - ( f_iControlOffset + f_iCursorPosition - 1 ) );
					l_pcBuffer [ f_iCursorPosition + f_iControlOffset ] = static_cast < char > ( a_iCode );
					l_iLength ++;
					f_iCursorPosition ++;
					if ( f_iCursorPosition >= f_iWidthRaw )
						{
						f_iCursorPosition = f_iWidthRaw - 1;
						f_iControlOffset ++;
						}
					}
				else
					bell ( );
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
			f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
			}
		else
			{
			f_iControlOffset = 0;
			f_iCursorPosition = l_iLength;
			}
		}
	l_pcBuffer [ l_iLength ] = 0;
	a_iCode = l_iErrorCode;
	M_IRV ( f_oPattern.matches ( f_oVarTmpBuffer, NULL, & l_iErrorCode ) );
	if ( ! l_iErrorCode )
		{
		f_oString = f_oVarTmpBuffer;
		f_poParent->status_bar ( )->message ( COLORS::D_FG_LIGHTGRAY, "" );
		refresh ( );
		}
	else
		{
		f_iControlOffset = l_iOldControlOffset;
		f_iCursorPosition = l_iOldCursorPosition;
		bell ( );
		f_poParent->status_bar ( )->message ( COLORS::D_BG_BROWN, f_oPattern.error ( ) );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HEditControl::set ( HInfo const & a_roInfo )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char const * l_pcString = a_roInfo.get < char const * > ( );
	HString l_oErrorMessage;
	M_IRV ( f_oPattern.matches ( l_pcString, NULL, & l_iErrorCode ) );
	if ( l_iErrorCode )
		M_THROW ( f_oPattern.error ( ), l_iErrorCode );
	f_oString = l_pcString;
	l_iLength = f_oString.get_length ( );
	f_iControlOffset = 0;
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
		}
	else
		f_iCursorPosition = l_iLength;
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


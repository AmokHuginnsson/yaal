/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	heditcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "heditcontrol.h"
#include "hwindow.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

char const * const n_pcMaskLetters  = "^[a-zA-Z�����ʳ����Ӷ�����]*$";
char const * const n_pcMaskDigits   = "^[0-9]*$";
char const * const n_pcMaskAlpha    = "^[a-zA-Z0-9]*$";
char const * const n_pcMaskExtended = "^[0-9a-zA-Z�����ʳ����Ӷ�����\\.\\(\\) -]*$";
char const * const n_pcMaskLoose    = ".*";
char const * const n_pcMaskDefault  = n_pcMaskLetters;

HEditControl::HEditControl( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel, int a_iBufferSize, char const * a_pcValue,
		char const * a_pcMask, bool a_bReplace, bool a_bRightAligned,
		bool a_bMultiLine, bool a_bReadOnly, bool a_bPassword,
		int a_iMaxHistoryLevel )
					: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
							a_iWidth, a_pcLabel ),
					f_bReplace ( a_bReplace ),
					f_bMultiLine ( a_bMultiLine || ( a_iHeight > 1 ) ? true : false ),
					f_bReadOnly ( a_bReadOnly ), f_bRightAligned ( a_bRightAligned ),
					f_bPassword ( a_bPassword ),
					f_iMaxStringSize ( a_iBufferSize ), f_iCursorPosition ( 0 ),
					f_iControlOffset ( 0 ), f_iMaxHistoryLevel ( a_iMaxHistoryLevel ),
					f_oPattern(), f_oString ( a_iBufferSize, true ), f_oHistory()
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	HString l_oErrorMessage;
	if ( a_iBufferSize < 1 )
		M_THROW( _( "buffer size is ridiculously low" ), a_iBufferSize );
	if ( a_pcValue )
		{
		l_iLength = strlen ( a_pcValue );
		if ( l_iLength > a_iBufferSize )
			M_THROW( _( "initial value too big" ), l_iLength - a_iBufferSize );
		}
	f_oVarTmpBuffer.hs_realloc ( a_iBufferSize + 1 );
	if ( f_bRightAligned && f_bMultiLine )
		M_THROW( _( "edit-control right aligned and multiline at the same time" ), 0 );
	f_oString = a_pcValue;
	f_oHistory.push_back ( "" );
	if ( ( l_iErrorCode = f_oPattern.parse_re ( a_pcMask ) ) )
		M_THROW ( f_oPattern.error(), l_iErrorCode );
	f_oPattern.matches ( a_pcValue, NULL, & l_iErrorCode );
	if ( l_iErrorCode )
		M_THROW ( f_oPattern.error(), l_iErrorCode );
	l_iLength = f_oString.get_length();
/* this is part of draw_label() method, we can not wait with setting up
 * f_iWidthRaw until draw_label(), which is called from refresh()
 * because ... see next comment */
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: HCons::get_instance().get_width() + f_iWidth - f_iColumnRaw;
/* f_iWidthRaw must be set up properly before setting up f_iCursorPosition and
 * f_iControlOffset whose are used in refresh() */
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
		}
	else
		f_iCursorPosition = l_iLength;
#ifdef __DEBUG__
#endif /* __DEBUG__ */
	return;
	M_EPILOG
	}
	
HEditControl::~HEditControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HEditControl::do_refresh ( void )
	{
	M_PROLOG
	HConsole& cons = HCons::get_instance();
	draw_label();
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	f_oVarTmpBuffer.fill ( ' ', f_iWidthRaw );
	if ( ! f_bPassword )
		{
		strcpy ( f_oVarTmpBuffer.raw(), static_cast < char const * const > ( f_oString ) + f_iControlOffset );
		f_oVarTmpBuffer [ f_oVarTmpBuffer.get_length() ] = ' ';
		}
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	M_ENSURE ( cons.c_mvprintf ( f_iRowRaw, f_iColumnRaw, f_oVarTmpBuffer ) != C_ERR );
	if ( f_bFocused )
		{
		M_ENSURE ( cons.c_move ( f_iRowRaw,
					f_iColumnRaw + ( f_bPassword ? 0 : f_iCursorPosition ) ) != C_ERR );
		cons.curs_set ( f_bReplace ? CURSOR::D_VERY_VISIBLE : CURSOR::D_VISIBLE );
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

int HEditControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iLength = 0;
	int l_iErrorCode = 0;
	int l_iOldControlOffset = 0;
	int l_iOldCursorPosition = 0;
	char * l_pcBuffer = 0;
	HConsole& cons = HCons::get_instance();
	a_iCode = HControl::do_process_input ( a_iCode );
	l_pcBuffer = f_oVarTmpBuffer.raw();
	f_oVarTmpBuffer = f_oString;
	l_iOldControlOffset = f_iControlOffset;
	l_iOldCursorPosition = f_iCursorPosition;
	l_iLength = f_oVarTmpBuffer.get_length();
	switch ( a_iCode )
		{
		case ( KEY_CODES::D_PAGE_UP ):
			f_oHistory.go ( 0 );
			l_iErrorCode = - 1;
		break;
		case ( KEY_CODES::D_PAGE_DOWN ):
			f_oHistory.go ( - 1 );
			l_iErrorCode = - 1;
		break;
		case ( KEY_CODES::D_UP ):
			f_oHistory.to_tail();
			l_iErrorCode = - 1;
		break;
		case ( KEY_CODES::D_DOWN ):
			f_oHistory.to_head();
			l_iErrorCode = - 1;
		break;
		case ( '\t' ):
			f_bFocused = false;
		/* enter works like tab without focus movement */
		case ( '\r' ):
			{
			l_iErrorCode = f_oHistory.size();
			l_iErrorCode ++;
			while ( -- l_iErrorCode )
				if ( ( * f_oHistory.to_tail() ) == f_oString )
					break;
			if ( f_oString.get_length() && ( ! l_iErrorCode ) )
				{
				f_oHistory.add_head ( & f_oString );
				l_iErrorCode = f_oHistory.size();
				while ( l_iErrorCode -- > f_iMaxHistoryLevel )
					f_oHistory.remove_at ( l_iErrorCode );
				f_oHistory.go ( - 1 );
				}
			else
				f_oHistory.to_head();
			l_iErrorCode = a_iCode;
			}
		break;
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
				cons.bell();
			}
		break;
		case ( KEY < 'a' >::ctrl ):
		case ( KEY_CODES::D_HOME ):
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
		break;
		case ( KEY < 'e' >::ctrl ):
		case ( KEY_CODES::D_END ):
			{
			if ( l_iLength >= f_iWidthRaw )
				{
				f_iCursorPosition = f_iWidthRaw - 1;
				f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
				}
			else
				f_iCursorPosition = l_iLength;
			}
		break;
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
				cons.bell();
			}
		break;
		case ( KEY < 'u' >::ctrl ):
			{
			if ( ! f_bReadOnly )
				{
				l_iLength = 0;
				f_iControlOffset = 0;
				f_iCursorPosition = 0;
				}
			else
				cons.bell();
			}
		break;
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
				cons.bell();
			}
		break;
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
				cons.bell();
			}
		break;
		case ( KEY_CODES::D_INSERT ):
			f_bReplace = ! f_bReplace;
		break;
		case ( KEY < 'f' >::meta ):
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
			l_pcBuffer = f_oVarTmpBuffer.raw();
			l_iErrorCode = 0;
			if ( ( l_iOldCursorPosition == f_iCursorPosition )
					&& ( l_iOldControlOffset == f_iControlOffset ) )
				cons.bell();
			}
		break;
		case ( KEY < 'b' >::meta ):
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
				l_pcBuffer = f_oVarTmpBuffer.raw();
				l_iErrorCode = 0;
				}
			else
				cons.bell();
			}
		break;
		case ( KEY < 'd' >::meta ):
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
				l_pcBuffer = f_oVarTmpBuffer.raw();
				l_iLength -= l_iErrorCode;
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + f_iControlOffset + f_iCursorPosition + l_iErrorCode,
						( ( l_iLength - f_iControlOffset ) - f_iCursorPosition ) + 1 );
				l_iErrorCode = 0;
				}
			else
				cons.bell();
			}
		break;
		case ( KEY < 'w' >::ctrl ):
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
					l_pcBuffer = f_oVarTmpBuffer.raw();
					l_iErrorCode = 0;
					}
				memmove ( l_pcBuffer + f_iControlOffset + f_iCursorPosition,
						l_pcBuffer + l_iOldControlOffset + l_iOldCursorPosition,
						( ( l_iLength - l_iOldControlOffset ) - l_iOldCursorPosition ) + 1 );
				l_iLength = f_oVarTmpBuffer.get_length();
				}
			else
				cons.bell();
			}
		break;
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
					cons.bell();
				}
			else
				l_iErrorCode = a_iCode;
			}
		}
	if ( l_iErrorCode < 0 )
		{
		l_iErrorCode = 0;
		if ( f_oHistory.size() )
			f_oVarTmpBuffer = f_oHistory.present();
		l_iLength = f_oVarTmpBuffer.get_length();
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
	f_oPattern.matches ( f_oVarTmpBuffer, NULL, & l_iErrorCode );
	if ( ! l_iErrorCode )
		{
		f_oString = f_oVarTmpBuffer;
		f_poParent->status_bar()->message ( COLORS::D_FG_LIGHTGRAY, "" );
		schedule_refresh();
		}
	else
		{
		f_iControlOffset = l_iOldControlOffset;
		f_iCursorPosition = l_iOldCursorPosition;
		cons.bell();
		f_poParent->status_bar()->message ( COLORS::D_BG_BROWN, f_oPattern.error() );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HEditControl::set ( HInfo const & a_roInfo )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char const * l_pcString = a_roInfo.get < char const * const >();
	HString l_oErrorMessage;
	f_oPattern.matches ( l_pcString, NULL, & l_iErrorCode );
	if ( l_iErrorCode )
		M_THROW ( f_oPattern.error(), l_iErrorCode );
	f_oString = l_pcString;
	l_iLength = f_oString.get_length();
	f_iControlOffset = 0;
	if ( l_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = ( l_iLength - f_iWidthRaw ) + 1;
		}
	else
		f_iCursorPosition = l_iLength;
	schedule_refresh();
	return;
	M_EPILOG
	}

int HEditControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

int HEditControl::do_click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iPosition = 0;
	if ( ! HControl::do_click ( a_rsMouse ) )
		return ( 1 );
	l_iPosition = a_rsMouse.f_iColumn - f_iColumnRaw;
	if ( l_iPosition < f_oString.get_length() )
		{
		f_iCursorPosition = l_iPosition;
		schedule_refresh();
		}
	return ( 0 );
	M_EPILOG
	}

}

}


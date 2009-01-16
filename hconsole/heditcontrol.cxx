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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "heditcontrol.hxx"
#include "hwindow.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

char const * const n_pcMaskLetters  = "^[a-zA-Z±¡æÆêÊ³£ñÑóÓ¶¦¼¬¿¯]*$";
char const * const n_pcMaskDigits   = "^[0-9]*$";
char const * const n_pcMaskAlpha    = "^[a-zA-Z0-9]*$";
char const * const n_pcMaskExtended = "^[0-9a-zA-Z±¡æÆêÊ³£ñÑóÓ¶¦¼¬¿¯\\.\\(\\) -]*$";
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
					f_oPattern(), f_oString ( a_iBufferSize, true ), f_oHistory(), f_oHistoryIt()
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	HString l_oErrorMessage;
	if ( a_iBufferSize < 1 )
		M_THROW( _( "buffer size is ridiculously low" ), a_iBufferSize );
	if ( a_pcValue )
		{
		l_iLength = static_cast<int>( ::strlen( a_pcValue ) );
		if ( l_iLength > a_iBufferSize )
			M_THROW( _( "initial value too big" ), l_iLength - a_iBufferSize );
		}
	f_oVarTmpBuffer.hs_realloc ( a_iBufferSize + 1 );
	if ( f_bRightAligned && f_bMultiLine )
		M_THROW( _( "edit-control right aligned and multiline at the same time" ), 0 );
	f_oString = a_pcValue;
	f_oHistory.push_back ( "" );
	f_oHistoryIt = f_oHistory.hook();
	if ( ( l_iErrorCode = f_oPattern.parse_re ( a_pcMask ) ) )
		M_THROW ( f_oPattern.error(), l_iErrorCode );
	f_oPattern.matches ( a_pcValue, NULL, & l_iErrorCode );
	if ( l_iErrorCode )
		M_THROW ( f_oPattern.error(), l_iErrorCode );
	l_iLength = static_cast<int>( f_oString.get_length() );
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

void HEditControl::do_refresh( void )
	{
	M_PROLOG
	HConsole& cons = HCons::get_instance();
	draw_label();
	f_oVarTmpBuffer.hs_realloc( f_iWidthRaw + 1 );
	if ( ! f_bPassword )
		f_oVarTmpBuffer = f_oString.mid( f_iControlOffset, f_iWidthRaw );
	else
		f_oVarTmpBuffer.clear();
	int long len = f_oVarTmpBuffer.get_length();
	if ( len < f_iWidthRaw )
		f_oVarTmpBuffer.fill( ' ', f_iWidthRaw - len, len );
	M_ENSURE( cons.c_mvprintf( f_iRowRaw, f_iColumnRaw, f_oVarTmpBuffer.raw() ) != C_ERR );
	if ( f_bFocused )
		{
		M_ENSURE( cons.c_move( f_iRowRaw,
					f_iColumnRaw + ( f_bPassword ? 0 : f_iCursorPosition ) ) != C_ERR );
		cons.curs_set( f_bReplace ? CURSOR::D_VERY_VISIBLE : CURSOR::D_VISIBLE );
		}
	return;
	M_EPILOG
	}

HInfo HEditControl::get( void )
	{
	M_PROLOG
	return ( HInfo( f_oString ) );
	M_EPILOG
	}

void HEditControl::set_flags( bool a_bReplace, bool a_bPassword )
	{
	M_PROLOG
	f_bReplace = a_bReplace;
	f_bPassword = a_bPassword;
	return;
	M_EPILOG
	}

char const n_pcWordSeparator [ ] = " \t\n`-=[]\\;',./~!@#$%^&*()+{}|:\"<>?";

int HEditControl::find_eow( int a_iLength )
	{
	M_PROLOG
	int l_iIndex = static_cast<int>( f_oString.find_other_than( n_pcWordSeparator, f_iControlOffset + f_iCursorPosition ) );
	if ( l_iIndex >= 0 )
		{
		l_iIndex = static_cast<int>( f_oString.find_one_of( n_pcWordSeparator, l_iIndex ) );
		if ( l_iIndex < 0 )
			l_iIndex = a_iLength;
		}
	return ( l_iIndex );
	M_EPILOG
	}

int HEditControl::go_to_eow( int a_iLength )
	{
	M_PROLOG
	int err = ( ( f_iControlOffset + f_iCursorPosition ) < a_iLength ) ? 0 : 1;
	if ( ! err )
		{
		int l_iIndex = find_eow( a_iLength );
		err = ( l_iIndex >= 0 ? 0 : 1 );
		if ( ! err )
			{
			f_iCursorPosition =	( l_iIndex - f_iControlOffset );
			if ( f_iCursorPosition >= f_iWidthRaw )
				{
				f_iControlOffset += ( ( f_iCursorPosition - f_iWidthRaw  ) + 1 );
				f_iCursorPosition = f_iWidthRaw - 1;
				}
			}
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::kill_line( void )
	{
	M_PROLOG
	if ( ! f_bReadOnly )
		{
		f_oVarTmpBuffer.set_at( 0, 0 );
		f_iControlOffset = 0;
		f_iCursorPosition = 0;
		}
	return ( ! f_bReadOnly ? 0 : 1 );
	M_EPILOG
	}

int HEditControl::move_right( int a_iLength )
	{
	M_PROLOG
	int err = ( ( f_iCursorPosition + f_iControlOffset ) < a_iLength ) ? 0 : 1;
	if ( ! err )
		{
		f_iCursorPosition ++;
		if ( f_iCursorPosition >= f_iWidthRaw )
			{
			f_iCursorPosition = f_iWidthRaw - 1;
			f_iControlOffset ++;
			}
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::move_left( void )
	{
	M_PROLOG
	int err = ( ( f_iControlOffset + f_iCursorPosition ) > 0 ) ? 0 : 1;
	if ( ! err )
		{
		if ( f_iCursorPosition > 0 )
			f_iCursorPosition --;
		else if ( f_iControlOffset > 0 )
			f_iControlOffset --;
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::go_to_end( int a_iLength )
	{
	M_PROLOG
	if ( a_iLength >= f_iWidthRaw )
		{
		f_iCursorPosition = f_iWidthRaw - 1;
		f_iControlOffset = ( a_iLength - f_iWidthRaw ) + 1;
		}
	else
		f_iCursorPosition = a_iLength;
	return ( 0 );
	M_EPILOG
	}

int HEditControl::delete_char( int a_iLength )
	{
	M_PROLOG
	int err = ! ( f_bReadOnly || f_bReplace ) ? 0 : 1;
	if ( ! err )
		{
		if ( ( f_iControlOffset + f_iCursorPosition ) >= a_iLength )
			{
			f_oVarTmpBuffer.set_at( 0, 0 );
			f_iControlOffset = 0;
			f_iCursorPosition = 0;
			}
		else
			{
			f_oVarTmpBuffer.erase( f_iControlOffset + f_iCursorPosition, 1 );
			if ( ( f_iControlOffset > 0 ) && ( ( f_iControlOffset + f_iWidthRaw ) >= a_iLength ) )
				{
				f_iControlOffset --;
				++ f_iCursorPosition;
				}
			}
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::kill_char( void )
	{
	M_PROLOG
	int err = ( ! f_bReadOnly && ( ( f_iControlOffset + f_iCursorPosition ) > 0 ) ) ? 0 : 1;
	if ( ! err ) 
		{
		if ( f_iControlOffset > 0 )
			f_iControlOffset --;
		else if ( f_iCursorPosition > 0 )
			f_iCursorPosition --;
		if ( ! f_bReplace )
			f_oVarTmpBuffer.erase( f_iControlOffset+ f_iCursorPosition, 1 );
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::find_bow( int a_iLength )
	{
	M_PROLOG
	int l_iIndex = static_cast<int>( f_oString.reverse_find_other_than( n_pcWordSeparator, a_iLength - ( f_iControlOffset + f_iCursorPosition ) ) );
	if ( l_iIndex >= 0 )
		{
		l_iIndex = static_cast<int>( f_oString.reverse_find_one_of( n_pcWordSeparator, l_iIndex ) );
		if ( l_iIndex < 0 )
			l_iIndex = 0;
		else
			l_iIndex = a_iLength - l_iIndex;
		}
	return ( l_iIndex );
	M_EPILOG
	}

int HEditControl::go_to_bow( int a_iLength )
	{
	M_PROLOG
	int err = ( f_iControlOffset + f_iCursorPosition ) ? 0 : 1;
	if ( ! err )
		{
		int l_iIndex = find_bow( a_iLength );
		if ( l_iIndex >= 0 )
			{
			f_iCursorPosition = ( l_iIndex - f_iControlOffset );
			if ( f_iCursorPosition < 0 )
				{
				f_iControlOffset += f_iCursorPosition;
				f_iCursorPosition = 0;
				}
			}
		else
			{
			err = 1;
			f_iControlOffset = 0;
			f_iCursorPosition = 0;
			}
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::delete_word( int a_iLength )
	{
	M_PROLOG
	int err = ( ! ( f_bReadOnly || f_bReplace ) ) ? 0 : 1;
	if ( ! err )
		{
		int l_iOldIndex = ( f_iControlOffset + f_iCursorPosition );
		if ( l_iOldIndex >= a_iLength )
			{
			f_oVarTmpBuffer.set_at( 0, 0 );
			f_iControlOffset = 0;
			f_iCursorPosition = 0;
			}
		else
			{
			int l_iIndex = find_eow( a_iLength );
			f_oVarTmpBuffer.erase( l_iOldIndex, l_iIndex - l_iOldIndex );
			}
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::kill_word( int a_iLength )
	{
	M_PROLOG
	int err = ( ! ( f_bReadOnly || f_bReplace ) ) ? 0 : 1;
	if ( ! err )
		{
		int l_iOldIndex = ( f_iControlOffset + f_iCursorPosition );
		if ( l_iOldIndex )
			{
			int l_iIndex = find_bow( a_iLength );
			if ( l_iIndex >= 0 )
				{
				f_iCursorPosition = ( l_iIndex - f_iControlOffset );
				if ( f_iCursorPosition < 0 )
					{
					f_iControlOffset += f_iCursorPosition;
					f_iCursorPosition = 0;
					}
				f_oVarTmpBuffer.erase( l_iIndex, l_iOldIndex - l_iIndex );
				}
			else
				{
				err = 1;
				f_iControlOffset = 0;
				f_iCursorPosition = 0;
				}
			}
		else
			err = 1;
		}
	return ( err );
	M_EPILOG
	}

int HEditControl::insert_char( int a_iCode, int a_iLength )
	{
	M_PROLOG
	int err = 0;
	if ( ( ! f_bReadOnly && ( a_iCode > 31 ) && ( a_iCode < 256 ) )
			&& ( ( ! f_bReplace && ( a_iLength < f_iMaxStringSize ) ) 
				|| ( f_bReplace && ( ( f_iControlOffset + f_iCursorPosition ) < a_iLength ) )	) )
		{
		if ( ! f_bReplace )
			f_oVarTmpBuffer.insert( f_iControlOffset+ f_iCursorPosition, 1 );
		f_oVarTmpBuffer.set_at( f_iCursorPosition + f_iControlOffset, static_cast<char>( a_iCode ) );
		f_iCursorPosition ++;
		if ( f_iCursorPosition >= f_iWidthRaw )
			{
			f_iCursorPosition = f_iWidthRaw - 1;
			f_iControlOffset ++;
			}
		}
	else
		err = a_iCode;
	return ( err );
	M_EPILOG
	}

int HEditControl::update_from_history( void )
	{
	M_PROLOG
	if ( ! f_oHistory.is_empty() && ( f_oHistoryIt != f_oHistory.end() ) )
		f_oVarTmpBuffer = *f_oHistoryIt;
	int l_iLength = static_cast<int>( f_oVarTmpBuffer.get_length() );
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
	return ( 0 );
	M_EPILOG
	}

int HEditControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	static int const D_HISTORY_OPERATION = -1;
	static int const D_DATA_ENTER = -2;
	int l_iLength = 0;
	int l_iErrorCode = 0;
	int l_iOldControlOffset = 0;
	int l_iOldCursorPosition = 0;
	HConsole& cons = HCons::get_instance();
	a_iCode = HControl::do_process_input ( a_iCode );
	f_oVarTmpBuffer = f_oString;
	l_iOldControlOffset = f_iControlOffset;
	l_iOldCursorPosition = f_iCursorPosition;
	l_iLength = static_cast<int>( f_oVarTmpBuffer.get_length() );
	switch ( a_iCode )
		{
		case ( KEY_CODES::D_PAGE_UP ):
			f_oHistoryIt = f_oHistory.hook();
			l_iErrorCode = D_HISTORY_OPERATION;
		break;
		case ( KEY_CODES::D_PAGE_DOWN ):
			f_oHistoryIt = f_oHistory.hook();
			-- f_oHistoryIt;
			l_iErrorCode = D_HISTORY_OPERATION;
		break;
		case ( KEY_CODES::D_UP ):
			++ f_oHistoryIt;
			l_iErrorCode = D_HISTORY_OPERATION;
		break;
		case ( KEY_CODES::D_DOWN ):
			-- f_oHistoryIt;
			l_iErrorCode = D_HISTORY_OPERATION;
		break;
		case ( '\t' ):
			f_bFocused = false;
		/* enter works like tab without focus movement */
		case ( '\r' ):
			{
			l_iErrorCode = static_cast<int>( f_oHistory.size() );
			l_iErrorCode ++;
			while ( -- l_iErrorCode )
				if ( ( *( ++ f_oHistoryIt ) ) == f_oString )
					break;
			if ( f_oString.get_length() && ( ! l_iErrorCode ) )
				{
				f_oHistory.add_head( &f_oString );
				l_iErrorCode = static_cast<int>( f_oHistory.size() );
				while ( l_iErrorCode -- > f_iMaxHistoryLevel )
					f_oHistory.pop_back(); /* FIXME investigate if it actually work */
				f_oHistoryIt = f_oHistory.hook();
				-- f_oHistoryIt;
				}
			else
				-- f_oHistoryIt;
			l_iErrorCode = D_DATA_ENTER;
			}
		break;
		case ( KEY_CODES::D_LEFT ):
			l_iErrorCode = move_left();
		break;
		case ( KEY<'a'>::ctrl ):
		case ( KEY_CODES::D_HOME ):
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
		break;
		case ( KEY<'e'>::ctrl ):
		case ( KEY_CODES::D_END ):
			l_iErrorCode = go_to_end( l_iLength );
		break;
		case ( KEY_CODES::D_RIGHT ):
			l_iErrorCode = move_right( l_iLength );
		break;
		case ( KEY<'u'>::ctrl ):
			l_iErrorCode = kill_line();
		break;
		case ( KEY_CODES::D_DELETE ):
			l_iErrorCode = delete_char( l_iLength );
		break;
		case ( KEY_CODES::D_BACKSPACE ):
			l_iErrorCode = kill_char();
		break;
		case ( KEY_CODES::D_INSERT ):
			f_bReplace = ! f_bReplace;
		break;
		case ( KEY<'f'>::meta ):
			l_iErrorCode = go_to_eow( l_iLength );
		break;
		case ( KEY<'b'>::meta ):
			l_iErrorCode = go_to_bow( l_iLength );
		break;
		case ( KEY<'d'>::meta ):
			l_iErrorCode = delete_word( l_iLength );
		break;
		case ( KEY<'w'>::ctrl ):
			l_iErrorCode = kill_word( l_iLength );
		break;
		default:
			l_iErrorCode = insert_char( a_iCode, l_iLength );
		}
	if ( l_iErrorCode == D_HISTORY_OPERATION )
		l_iErrorCode = update_from_history();
	if ( ! l_iErrorCode )
		{
		f_oPattern.matches( f_oVarTmpBuffer, NULL, &l_iErrorCode );
		if ( l_iErrorCode )
			f_poParent->status_bar()->message( COLORS::D_BG_BROWN, f_oPattern.error().raw() );
		else
			{
			a_iCode = l_iErrorCode;
			f_oString = f_oVarTmpBuffer;
			f_poParent->status_bar()->message ( COLORS::D_FG_LIGHTGRAY, "" );
			schedule_refresh();
			}
		}
	if ( l_iErrorCode && ( l_iErrorCode != D_DATA_ENTER ) )
		{
		f_iControlOffset = l_iOldControlOffset;
		f_iCursorPosition = l_iOldCursorPosition;
		cons.bell();
		}
	return ( a_iCode );
	M_EPILOG
	}

void HEditControl::set( HInfo const& a_roInfo )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	int l_iLength = 0;
	char const* l_pcString = a_roInfo.get<char const *>();
	HString l_oErrorMessage;
	f_oPattern.matches( l_pcString, NULL, &l_iErrorCode );
	if ( l_iErrorCode )
		M_THROW( f_oPattern.error(), l_iErrorCode );
	f_oString = l_pcString;
	l_iLength = static_cast<int>( f_oString.get_length() );
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


/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.c - this file is integral part of `stdhapi' project.

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
#include <ctype.h>
#include <libintl.h>

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
#include "hcore/hlog.h"
#include "hlistcontrol.h"
#include "hconsole.h"

HListControl::HColumnInfo::HColumnInfo ( void )
	{
	M_PROLOG
	f_iWidthRaw = 0;
	f_iWidth = 0;
	f_iType = 0;
	f_iAlign = 0;
	f_iShortcutIndex = 0;
	f_cShortcut = 0;
	f_poControl = NULL;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::~HColumnInfo ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HListControl::HColumnInfo::HColumnInfo ( const HColumnInfo & a_roColumnInfo )
	{
	M_PROLOG
	( * this ) = a_roColumnInfo;
	return;
	M_EPILOG
	}

HListControl::HColumnInfo & HListControl::HColumnInfo::operator = ( const HColumnInfo & a_roColumnInfo )
	{
	M_PROLOG
	f_iWidthRaw = a_roColumnInfo.f_iWidthRaw;
	f_iWidth = a_roColumnInfo.f_iWidth;
	f_iType = a_roColumnInfo.f_iType;
	f_iAlign = a_roColumnInfo.f_iAlign;
	f_iShortcutIndex = a_roColumnInfo.f_iShortcutIndex;
	f_cShortcut = a_roColumnInfo.f_cShortcut;
	f_oName = a_roColumnInfo.f_oName;
	f_poControl = a_roColumnInfo.f_poControl;
	return ( * this );
	M_EPILOG
	}

int HListControl::HColumnInfo::operator - ( const HColumnInfo & )
	{
	M_PROLOG
	if ( g_iErrNo || ! g_iErrNo )
		M_THROW ( "this method should not be called", g_iErrNo );
	return ( 0 );
	M_EPILOG
	}

HListControl::HListControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const * a_pcLabel, bool a_bCheckable,
		bool a_bSortable, bool a_bSearchable, bool a_bDrawHeader, bool a_bDrawLabel,
		int a_iDisabledAttribute, int a_iEnabledAttribute,
		int a_iFocudesAttribute )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
								a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
								a_iEnabledAttribute, a_iFocudesAttribute ),
							HSearchableControl ( a_bSearchable ),
							HList < HItem > ( )
	{
	M_PROLOG
	f_bEditable = false;
	f_bCheckable = a_bCheckable;
	f_bSortable = a_bSortable;
	f_bDrawHeader = a_bDrawHeader;
	f_iControlOffset = 0;
	f_iCursorPosition = 0;
	f_iSumForOne = 0;
	f_poFirstVisibleRow = NULL;
	f_sMatch.f_iColumnWithMatch = 0;
	f_sMatch.f_iMatchNumber = - 1;
	f_sMatch.f_poCurrentMatch = NULL;
	f_iSortColumn = 0;
	cmp = ( int ( HList<HItem>::* ) ( HElement *, HElement * ) ) & HListControl::cmpc;
	refresh ( );
	return;
	M_EPILOG
	}

HListControl::~HListControl ( void )
	{
	M_PROLOG
	flush ( );
	return;
	M_EPILOG
	}

void HListControl::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumnOffset = 0;
	int l_iTmp = 0;
	int l_iColumns = f_oHeader.quantity ( );
	int l_iHR = f_bDrawHeader ? 1 : 0; /* HR stands for header row */
	double l_dScaled = 0;
	HElement * l_poElement = f_poSelected;
	HItem l_oItem ( l_iColumns );
	HColumnInfo * l_poColumnInfo = NULL;
	l_iTmp = f_iWidthRaw;
	if ( f_bFocused )curs_set ( D_CURSOR_INVISIBLE );
	draw_label ( );
	if ( ! f_iSumForOne )return;
	if ( f_iWidthRaw != l_iTmp )recalculate_column_widths ( );
/* we need to decrement f_iHeightRaw because we have additional row, 
 * the list control header */
	if ( f_bDrawHeader )f_iHeightRaw --;
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	if ( f_iQuantity > 0 )
		{
		f_poSelected = f_poFirstVisibleRow;
		for ( l_iCtr = 0;
					l_iCtr < ( f_iQuantity > f_iHeightRaw ? f_iHeightRaw : f_iQuantity );
					l_iCtr ++ )
			{
			l_iColumnOffset = 0;
			l_oItem = f_poSelected->get_object ( );
			for ( l_iCtrLoc = 0; l_iCtrLoc < l_iColumns; l_iCtrLoc ++ )
				{
				l_poColumnInfo = & f_oHeader [ l_iCtrLoc ];
				if ( l_poColumnInfo->f_iWidthRaw )
					{
					f_oVarTmpBuffer [ 0 ] = 0;
					switch ( l_poColumnInfo->f_iType )
						{
						case ( D_TYPE_LONG_INT ):
							{
							f_oVarTmpBuffer = ( long int & ) l_oItem [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_DOUBLE ):
							{
							f_oVarTmpBuffer = ( double & ) l_oItem [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_HSTRING ):
							{
							f_oVarTmpBuffer = ( HString & ) l_oItem [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_HTIME ):
							{
							f_oVarTmpBuffer = ( HString & ) ( HTime & ) l_oItem [ l_iCtrLoc ];
							break;
							}
						default :
							{
							M_THROW ( "unknown type",
									l_poColumnInfo->f_iType );
							break;
							}
						}
					l_iTmp = f_oVarTmpBuffer.get_length ( );
					switch ( l_poColumnInfo->f_iAlign )
						{
						case ( D_ALIGN_LEFT ):
							{
							if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								{
								memset ( ( char * ) f_oVarTmpBuffer + l_iTmp, '_',
										l_poColumnInfo->f_iWidthRaw - l_iTmp );
								}
							f_oVarTmpBuffer [ l_poColumnInfo->f_iWidthRaw ] = 0;
							break;
							}
						case ( D_ALIGN_CENTER ):
							{
							if ( l_iTmp > l_poColumnInfo->f_iWidthRaw )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right ( l_poColumnInfo->f_iWidthRaw );
							else if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								{
								memmove ( ( char * ) f_oVarTmpBuffer 
										+ ( l_poColumnInfo->f_iWidthRaw - l_iTmp ) / 2, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								memset ( f_oVarTmpBuffer, '_', ( l_poColumnInfo->f_iWidthRaw - l_iTmp ) / 2 );
								l_iTmp = f_oVarTmpBuffer.get_length ( );
								memset ( ( char * ) f_oVarTmpBuffer + l_iTmp, '_',
										l_poColumnInfo->f_iWidthRaw - l_iTmp );
								f_oVarTmpBuffer [ l_poColumnInfo->f_iWidthRaw ] = 0;
								}
							break;
							}
						case ( D_ALIGN_RIGHT ):
							{
							if ( l_iTmp > l_poColumnInfo->f_iWidthRaw )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right ( l_poColumnInfo->f_iWidthRaw );
							else if ( l_iTmp < l_poColumnInfo->f_iWidthRaw )
								{
								memmove ( ( char * ) f_oVarTmpBuffer + l_poColumnInfo->f_iWidthRaw - l_iTmp, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								memset ( f_oVarTmpBuffer, '_', l_poColumnInfo->f_iWidthRaw - l_iTmp );
								}
							break;
							}
						default :
							{
							M_THROW ( "unknown align",
									l_poColumnInfo->f_iAlign );
							break;
							}
						}
					if ( l_iCtr == f_iCursorPosition )
						{
						if ( l_oItem.m_bChecked )
							console::set_attr ( ! f_bEnabled ? ( ! f_bFocused ? ~ ( f_iFocusedAttribute >> 8 )
										: ~ ( f_iEnabledAttribute >> 8 ) ) : ~ ( f_iDisabledAttribute >> 8 ) );
						else
							console::set_attr ( f_bEnabled ? ( f_bFocused ? ~f_iFocusedAttribute
										: ~ f_iEnabledAttribute ) : ~ f_iDisabledAttribute );
						}
					else
						{
						if ( l_oItem.m_bChecked )
							console::set_attr ( f_bEnabled ? ( f_bFocused ? ~ ( f_iFocusedAttribute >> 8 )
										: ~ ( f_iEnabledAttribute >> 8 ) ) : ~ ( f_iDisabledAttribute >> 8 ) );
						else M_SET_ATTR_DATA ( );
						}
					::mvprintw ( f_iRowRaw + l_iCtr + l_iHR,
							f_iColumnRaw + l_iColumnOffset, f_oVarTmpBuffer	);
					if ( f_bSearchActived )
						highlight ( f_iRowRaw + l_iCtr + l_iHR,
								f_iColumnRaw + l_iColumnOffset, f_sMatch.f_iMatchNumber,
								( f_poSelected == f_sMatch.f_poCurrentMatch ) &&
								( l_iCtrLoc == f_sMatch.f_iColumnWithMatch ) );
					l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
					}
				if ( l_iCtr == f_iCursorPosition )
					{
					if ( l_poColumnInfo->f_poControl )
						( * l_poColumnInfo->f_poControl ) = l_oItem [ l_iCtrLoc ];
					}
				}
			to_tail ( );
			}
		}
	l_iColumnOffset = 0;
	M_SET_ATTR_DATA ( );
	memset ( f_oVarTmpBuffer, '.', f_iWidthRaw );
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	for ( ; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		::mvprintw ( f_iRowRaw + l_iCtr + l_iHR,	f_iColumnRaw, f_oVarTmpBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_poColumnInfo = & f_oHeader [ l_iCtr ];
		if ( l_poColumnInfo->f_iWidthRaw )
			{
			if ( f_bDrawHeader )
				{
				f_oVarTmpBuffer = l_poColumnInfo->f_oName;
				M_SET_ATTR_LABEL ( );
				f_oVarTmpBuffer.format ( "%%-%ds", l_poColumnInfo->f_iWidthRaw );
				::mvprintw ( f_iRowRaw, f_iColumnRaw + l_iColumnOffset, f_oVarTmpBuffer,
	( char * ) ( l_poColumnInfo->f_oName.left ( l_poColumnInfo->f_iWidthRaw ) ) );
				M_SET_ATTR_SHORTCUT ( );
				::mvprintw ( f_iRowRaw,
						f_iColumnRaw + l_iColumnOffset + l_poColumnInfo->f_iShortcutIndex,
						"%c", l_poColumnInfo->f_cShortcut );
				}
			l_iColumnOffset += l_poColumnInfo->f_iWidthRaw;
			if ( l_iCtr < l_iColumns )
				{
				console::set_attr ( f_iDisabledAttribute );
				for ( l_iCtrLoc = 0; l_iCtrLoc < ( f_iHeightRaw + l_iHR ); l_iCtrLoc ++ )
					{
					::move ( f_iRowRaw + l_iCtrLoc,	f_iColumnRaw + l_iColumnOffset - 1 );
					addch ( D_ASCII_VERTICAL_LINE );
					}
				}
			}
		}
	console::set_attr ( ! f_bEnabled ? ( ! f_bFocused ? f_iFocusedAttribute : f_iEnabledAttribute ) : f_iDisabledAttribute );
	if ( f_iQuantity )
		{
		if ( f_iControlOffset )
			{
			::move ( f_iRowRaw + l_iHR, f_iColumnRaw + l_iColumnOffset - 1 );
			addch ( D_ASCII_UP_ARROW );
			}
		if ( ( f_iQuantity - f_iControlOffset ) > f_iHeightRaw )
			{
			::move ( f_iRowRaw + f_iHeightRaw - ( 1 - l_iHR ),
					f_iColumnRaw + l_iColumnOffset - 1 );
			addch ( D_ASCII_DOWN_ARROW );
			}
		l_dScaled = f_iHeightRaw - 3;
		l_dScaled *= ( double ) ( f_iControlOffset + f_iCursorPosition );
		l_dScaled /= ( double )f_iQuantity;
		::mvprintw ( f_iRowRaw + ( int ) ( l_dScaled + 1.5 + l_iHR ),
				f_iColumnRaw + l_iColumnOffset - 1, "#" );
		}
	f_poSelected = l_poElement;
	return;
	M_EPILOG
	}

int HListControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iErrorCode = 0;
	int l_iOldPosition = 0;
	HElement * l_poElement = f_poSelected;
	f_poSelected = f_poFirstVisibleRow;
	l_iOldPosition = f_iCursorPosition;
	a_iCode = HControl::process_input ( a_iCode );
	switch ( a_iCode )
		{
		case ( KEY_PPAGE ):
			{
			if ( ! f_iCursorPosition )
				{
				if ( f_iControlOffset )
					{
					to_head ( f_iHeightRaw - 1, D_TREAT_AS_OPENED );
					l_poElement = f_poSelected;
					f_iControlOffset -= f_iHeightRaw;
					f_iControlOffset ++;
					}
				else putchar ( '\a' );
				if ( f_iControlOffset < 0 )f_iControlOffset = 0;
				}
			else f_iCursorPosition = 0;
			break;
			}
		case ( KEY_NPAGE ):
			{
			if ( f_iQuantity >= f_iHeightRaw )
				{
				if ( f_iCursorPosition == ( f_iHeightRaw - 1 ) )
					{
					if ( f_iControlOffset >= ( f_iQuantity - f_iHeightRaw ) )
						putchar ( '\a' );
					f_iControlOffset += f_iHeightRaw;
					f_iControlOffset --;
					if ( f_iControlOffset > ( f_iQuantity - f_iHeightRaw ) )
						{
						l_poElement = f_poSelected = f_poHook;
						l_iOldPosition = f_iCursorPosition + 1;
						to_head ( f_iHeightRaw );
						f_iControlOffset = f_iQuantity - f_iHeightRaw;
						}
					else
						{
						to_tail ( f_iHeightRaw - 1, D_TREAT_AS_OPENED );
						l_iOldPosition -= f_iCursorPosition;
						}
					}
				else f_iCursorPosition = f_iHeightRaw - 1;
				}
			else
				{
				if ( f_iCursorPosition == ( f_iQuantity - 1 ) )putchar ( '\a' );
				f_iCursorPosition = f_iQuantity - 1;
				}
			break;
			}
		case ( KEY_UP ):
			{
			if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
				{
				if ( f_iCursorPosition > 0 )f_iCursorPosition --;
				else if ( f_iControlOffset > 0 )
					{
					to_head ( );
					l_poElement = f_poSelected;
					f_iControlOffset --;
					}
				}
			else putchar ( '\a' );
			break;
			}
		case ( KEY_HOME ):
			{
			l_iOldPosition = 0;
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
			l_poElement = f_poSelected = f_poHook;
			break;
			}
		case ( KEY_END ):
			{
			if ( f_iQuantity >= f_iHeightRaw )
				{
				l_poElement = f_poSelected = f_poHook;
				to_head ( f_iHeightRaw );
				f_iCursorPosition = f_iHeightRaw - 1;
				l_iOldPosition = f_iCursorPosition + 1;
				f_iControlOffset = f_iQuantity - f_iHeightRaw;
				}
			else f_iCursorPosition = f_iQuantity - 1;
			break;
			}
		case ( KEY_DOWN ):
			{
			if ( ( f_iCursorPosition + f_iControlOffset ) < ( f_iQuantity - 1 ) )
				{
				f_iCursorPosition ++;
				if ( f_iCursorPosition >= f_iHeightRaw )
					{
					f_iCursorPosition = f_iHeightRaw - 1;
					f_iControlOffset ++;
					l_iOldPosition --;
					to_tail ( );
					}
				}
			else putchar ( '\a' );
			break;
			}
		case ( D_KEY_CTRL_( 'n' ) ):
			{
			f_poSelected = l_poElement;
			if ( f_bBackwards )
				go_to_match_previous ( );
			else
				go_to_match ( );
			l_poElement = f_poSelected;
			f_poSelected = f_poFirstVisibleRow;
			l_iOldPosition = f_iCursorPosition;
			break;
			}
		case ( D_KEY_CTRL_( 'p' ) ):
			{
			f_poSelected = l_poElement;
			if ( f_bBackwards )
				go_to_match ( );
			else
				go_to_match_previous ( );
			l_poElement = f_poSelected;
			f_poSelected = f_poFirstVisibleRow;
			l_iOldPosition = f_iCursorPosition;
			break;
			}
		case ( ' ' ):
			{
			if ( f_bCheckable && f_iQuantity )
				l_poElement->get_object ( ).m_bChecked = ! l_poElement->get_object ( ).m_bChecked;
			break;
			}
		case ( '\t' ):
			{
			f_poFirstVisibleRow = f_poSelected;
			f_bFocused = false;	/* very  */
			refresh ( );				/* magic */ /* there is no break in this `case ( ):',
																				 because this list must give up focus
																				 and be refreshed and parent window
																				 must give focus to another control */
			f_poSelected = f_poFirstVisibleRow;
			}
		default :
			{
			l_iErrorCode = f_oHeader.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iErrorCode; l_iCtr ++ )
				if ( tolower ( a_iCode ) == tolower ( f_oHeader [ l_iCtr ].f_cShortcut ) )
					break;
			if ( l_iCtr < l_iErrorCode )
				{
				l_iErrorCode = 0;
				sort_by_contents ( l_iCtr,
						a_iCode == tolower ( a_iCode ) ? D_ASCENDING : D_DESCENDING );
				l_poElement = f_poHook;
				l_iOldPosition = 0;
				}
			else l_iErrorCode = a_iCode;
			break;
			}
		}
	f_poFirstVisibleRow = f_poSelected;
	f_poSelected = l_poElement;
	if ( l_iOldPosition > f_iCursorPosition )
		to_head ( l_iOldPosition - f_iCursorPosition );
	else if ( l_iOldPosition < f_iCursorPosition )
		to_tail ( f_iCursorPosition - l_iOldPosition );
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		refresh ( );
		f_poParent->status_bar ( )->message ( D_FG_LIGHTGRAY, "" );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HListControl::add_column ( const int & a_riColumn, char const * a_pcName,
		const int & a_riWidth, const int & a_riAlign, const int & a_riType, 
		HControl * a_poControl )
	{
	M_PROLOG
	int l_iShortcutIndex = 0;
	HColumnInfo l_oColumnInfo;
	if ( f_iQuantity )
		M_THROW (
				"can not add new column when list not empty", f_iQuantity );
	f_oVarTmpBuffer = a_pcName;
	l_iShortcutIndex = f_oVarTmpBuffer.find ( '&' );
	if ( l_iShortcutIndex > -1 )
		{
		f_oVarTmpBuffer [ l_iShortcutIndex ] = 0;
		f_oVarTmpBuffer += a_pcName + l_iShortcutIndex + 1;
		}
	else l_iShortcutIndex = 0;
	f_iSumForOne += a_riWidth;
	l_oColumnInfo.f_iWidth = a_riWidth;
	l_oColumnInfo.f_iType = a_riType;
	l_oColumnInfo.f_iAlign = a_riAlign;
	l_oColumnInfo.f_iShortcutIndex = l_iShortcutIndex;
	l_oColumnInfo.f_cShortcut = f_oVarTmpBuffer [ l_iShortcutIndex ];
	l_oColumnInfo.f_oName = f_oVarTmpBuffer;
	l_oColumnInfo.f_poControl = a_poControl;
	f_oHeader.add_at ( a_riColumn, & l_oColumnInfo );
	recalculate_column_widths ( );
	refresh ( );
	return;
	M_EPILOG
	}

HItem & HListControl::add_tail ( void )
	{
	M_PROLOG
	HItem l_oDummy ( f_oHeader.quantity ( ) );
	return ( add_tail ( l_oDummy ) );
	M_EPILOG
	}

HItem & HListControl::add_tail ( HItem & a_roItem )
	{
	M_PROLOG
	HItem * l_poDummy = NULL;
	l_poDummy = & HList < HItem > ::add_tail ( & a_roItem );
	if ( f_iQuantity > f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = f_iQuantity - f_iHeightRaw;
		if ( f_poFirstVisibleRow )
			{
			f_poSelected = f_poFirstVisibleRow;
			to_tail ( );
			f_poFirstVisibleRow = f_poSelected;
			}
		}
	else f_iCursorPosition = f_iQuantity - 1;
	if ( ! f_poFirstVisibleRow )f_poFirstVisibleRow = f_poHook;
	f_poSelected = f_poHook;
	to_head ( );
	console::n_bNeedRepaint = true;
	return ( * l_poDummy );
	M_EPILOG
	}

HItem & HListControl::add_orderly ( HItem & a_roItem )
	{
	M_PROLOG
	HItem * l_poDummy = NULL;
	l_poDummy = & HList < HItem > ::add_orderly ( a_roItem );
	f_iCursorPosition = 0;
	f_iControlOffset = 0;
	f_poFirstVisibleRow = f_poHook;
	f_poSelected = f_poHook;
	console::n_bNeedRepaint = true;
	return ( * l_poDummy );
	M_EPILOG
	}

int HListControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

void HListControl::recalculate_column_widths ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCtrLoc = 0;
	int l_iColumns = 0;
	int l_iColumnOffset = 0;
	int l_iNewWidth = 0;
	l_iColumns = f_oHeader.quantity ( );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_iNewWidth = f_oHeader [ l_iCtr ].f_iWidth;
		if ( l_iNewWidth )
			{
			l_iNewWidth *= f_iWidthRaw;
			l_iNewWidth /= f_iSumForOne;
			l_iCtrLoc = l_iCtr; /* last one with non zero width */
			f_oHeader [ l_iCtr ].f_iWidthRaw = l_iNewWidth;
			l_iColumnOffset += l_iNewWidth;
			}
		}
	/* last column with non zero width should fill space */
	l_iColumnOffset -= f_oHeader [ l_iCtrLoc ].f_iWidthRaw;
	f_oHeader [ l_iCtrLoc ].f_iWidthRaw = ( f_iWidthRaw - l_iColumnOffset );
	return;
	M_EPILOG
	}

int HListControl::remove_element ( int a_iFlag, HItem * * a_ppoItem )
	{
	M_PROLOG
	bool l_bFlag = true;
	int l_iError = 0;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == f_iQuantity ) )
		{
		f_iControlOffset --;
		to_head ( f_poFirstVisibleRow );
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( f_iQuantity - 1 ) ) )
		f_iCursorPosition --;
	else l_bFlag = false;
	if ( f_poSelected == f_poFirstVisibleRow )
		to_tail ( f_poFirstVisibleRow );
	console::n_bNeedRepaint = true;
	l_iError = HList < HItem > ::remove_element ( a_iFlag, a_ppoItem );
	if ( l_bFlag )to_head ( );
	refresh ( );
	return ( l_iError );
	M_EPILOG
	}

int HListControl::remove_tail ( int a_iFlag, HItem * * a_ppoItem )
	{
	M_PROLOG
	int l_iError = 0;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == f_iQuantity )  )
		{
		f_iControlOffset --;
		to_head ( f_poFirstVisibleRow );
		if ( f_iCursorPosition < ( f_iHeightRaw - 1 ) )
			{
			f_iCursorPosition ++;
			to_tail ( );
			}
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( f_iQuantity - 1 ) ) )
		f_iCursorPosition --;
	console::n_bNeedRepaint = true;
	l_iError = HList < HItem > ::remove_tail ( a_iFlag, a_ppoItem );
	if ( console::is_enabled ( ) )refresh ( );
	return ( l_iError );
	M_EPILOG
	}

int HListControl::cmpc ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	double l_dDifference = 0;
	HInfo & l_roLeftInfo = a_poLeft->get_object ( ) [ f_iSortColumn ];
	HInfo & l_roRightInfo = a_poRight->get_object ( ) [ f_iSortColumn ];
	f_lComparedItems ++;
	if ( ( f_iQuantity > 1024 ) && ! ( f_lComparedItems % 1024 ) )
		f_poParent->status_bar ( )->update_progress ( f_lComparedItems );
	switch ( f_oHeader [ f_iSortColumn ].f_iType )
		{
		case ( D_TYPE_LONG_INT ):
			{
			return ( ( ( long ) l_roLeftInfo ) - ( ( long ) l_roRightInfo ) );
			break;
			}
		case ( D_TYPE_DOUBLE ):
			{
			l_dDifference = ( ( double ) l_roLeftInfo ) - ( ( double ) l_roRightInfo );
			break;
			}
		case ( D_TYPE_HSTRING ):
			{
			return ( strcasecmp ( ( HString & ) l_roLeftInfo,
					( ( HString & ) l_roRightInfo ) ) );
			break;
			}
		case ( D_TYPE_HTIME ):
			{
			l_dDifference = ( ( time_t ) ( HTime & ) l_roLeftInfo ) - ( ( time_t ) ( HTime & ) l_roRightInfo );
			break;
			}
		default :
			{
			break;
			}
		}
	return ( l_dDifference > 0 ? 1 : ( l_dDifference < 0 ? - 1 : 0 ) );
	M_EPILOG
	}

void HListControl::sort_by_contents ( int a_iColumn, int a_iOrder )
	{
	M_PROLOG
	if ( ! f_bSortable )return;
	f_iSortColumn = a_iColumn;
	f_iOrder = a_iOrder;
	f_lComparedItems = 0;
	cmp = ( int ( HList<HItem>::* ) ( HElement *, HElement * ) ) & HListControl::cmpc;
	if ( f_iQuantity > 128 )
		f_poParent->status_bar ( )->init_progress ( f_iQuantity * f_iQuantity / 2,
				" Sorting ..." );
	sort ( );
	f_iControlOffset = 0;
	f_iCursorPosition = 0;
	f_poFirstVisibleRow = f_poSelected = f_poHook;
	return;
	M_EPILOG
	}

int HListControl::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iRow = 0, l_iMoved = 0;
	if ( ! HControl::click ( a_rsMouse ) )return ( 1 );
	l_iRow = a_rsMouse.f_iRow - f_iRowRaw - ( f_bDrawHeader ? 1 : 0 );
	if ( l_iRow == f_iCursorPosition )return ( 1 );
	if ( l_iRow < f_iQuantity )
		{
		l_iMoved = f_iCursorPosition - l_iRow;
		if ( l_iMoved > 0 )to_head ( l_iMoved );
		else to_tail ( - l_iMoved );
		f_iCursorPosition = l_iRow;
		refresh ( );
		}
	return ( 0 );
	M_EPILOG
	}

bool HListControl::is_searchable ( void )
	{
	return ( f_bSearchable );
	}

void HListControl::go_to_match ( void )
	{
	M_PROLOG
	int l_iDummy = 0, l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iCount = f_iQuantity + 1, l_iColumns = f_oHeader.quantity ( );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char * l_pcHighlightStart = NULL;
	HItem * l_poItem = NULL;
	HElement * l_poSelectedOrig = f_poSelected;
	HElement * l_poFirstVisibleRowOrig = f_poFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_poCurrentMatch != f_poSelected )f_sMatch.f_iMatchNumber = - 1;
	f_sMatch.f_poCurrentMatch = f_poSelected;
	while ( l_iCount -- )
		{
		l_poItem = & f_poSelected->get_object ( );
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr < l_iColumns; l_iCtr ++ )
			{
			l_pcHighlightStart = ( char * ) ( HString & ) ( * l_poItem ) [ l_iCtr ];
			l_iCtrLoc = 0;
			while ( ( l_pcHighlightStart = f_oPattern.matches ( l_pcHighlightStart,
					l_iDummy ) ) )
				{
				if ( l_iCtrLoc > f_sMatch.f_iMatchNumber )break;
				l_pcHighlightStart ++;
				l_iCtrLoc ++;
				}
			if ( l_pcHighlightStart )break;
			f_sMatch.f_iMatchNumber = - 1;
			}
		if ( l_pcHighlightStart )break;
		f_sMatch.f_iColumnWithMatch = 0;
/* this part is from process_input, but slightly modified */
		if ( ( f_iCursorPosition + f_iControlOffset ) < ( f_iQuantity - 1 ) )
			{
			f_iCursorPosition ++;
			if ( f_iCursorPosition >= f_iHeightRaw )
				{
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset ++;
				l_iMoveFirstRow ++;
				}
			to_tail ( );
			}
		else
			{
			f_poSelected = f_poFirstVisibleRow = f_poHook;
			f_iControlOffset = f_iCursorPosition = 0;
			l_iMoveFirstRow = 0;
			f_poParent->status_bar ( )->message ( _ ( "search hit BOTTOM, continuing at TOP" ) );
			}
/* end od it */
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			to_tail ( f_poFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_poCurrentMatch = f_poSelected;
		}
	else
		{
		f_poSelected = l_poSelectedOrig;
		f_poFirstVisibleRow = l_poFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = - 1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_poParent->status_bar ( )->message ( HString ( _ ( "pattern not found: " ) ) + f_oPattern.error ( ) );
		}
	return;
	M_EPILOG
	}

void HListControl::go_to_match_previous ( void )
	{
	M_PROLOG
	int l_iDummy = 0, l_iCtr = 0, l_iCtrLoc = 0, l_iMoveFirstRow = 0;
	int l_iCount = f_iQuantity + 1, l_iColumns = f_oHeader.quantity ( );
	int l_iControlOffsetOrig = f_iControlOffset, l_iCursorPositionOrig = f_iCursorPosition;
	char * l_pcHighlightStart = NULL;
	HItem * l_poItem = NULL;
	HElement * l_poSelectedOrig = f_poSelected;
	HElement * l_poFirstVisibleRowOrig = f_poFirstVisibleRow;
	if ( ! f_bSearchActived )
		return;
	if ( f_sMatch.f_poCurrentMatch != f_poSelected )
		f_sMatch.f_iMatchNumber = - 1;
	f_sMatch.f_poCurrentMatch = f_poSelected;
	while ( l_iCount -- )
		{
		l_poItem = & f_poSelected->get_object ( );
		for ( l_iCtr = f_sMatch.f_iColumnWithMatch; l_iCtr >= 0; l_iCtr -- )
			{
			l_pcHighlightStart = ( char * ) ( HString & ) ( * l_poItem ) [ l_iCtr ];
			l_iCtrLoc = 0;
			if ( f_sMatch.f_iMatchNumber < 0 )
				f_sMatch.f_iMatchNumber = f_oPattern.count ( l_pcHighlightStart );
			while ( ( l_pcHighlightStart = f_oPattern.matches ( l_pcHighlightStart,
					l_iDummy ) ) )
				{
				if ( l_iCtrLoc == ( f_sMatch.f_iMatchNumber - 1 ) )break;
				if ( l_iCtrLoc >= f_sMatch.f_iMatchNumber )
					{
					l_pcHighlightStart = NULL;
					break;
					}
				l_pcHighlightStart ++;
				l_iCtrLoc ++;
				}
			if ( l_pcHighlightStart )break;
			f_sMatch.f_iMatchNumber = - 1;
			}
		if ( l_pcHighlightStart )break;
		f_sMatch.f_iColumnWithMatch = l_iColumns - 1;
/* this part is from process_input, but slightly modified */
		if ( ( f_iControlOffset + f_iCursorPosition ) > 0 )
			{
			if ( f_iCursorPosition > 0 )f_iCursorPosition --;
			else if ( f_iControlOffset > 0 )
				{
				f_iControlOffset --;
				l_iMoveFirstRow ++;
				}
			to_head ( );
			}
		else
			{
			if ( f_iQuantity >= f_iHeightRaw )
				{
				f_poSelected = f_poHook;
				to_head ( );
				f_poFirstVisibleRow = f_poSelected;
				to_head ( f_poFirstVisibleRow, f_iHeightRaw - 1 );
				f_iCursorPosition = f_iHeightRaw - 1;
				f_iControlOffset = f_iQuantity - f_iHeightRaw;
				}
			else f_iCursorPosition = f_iQuantity - 1;
			l_iMoveFirstRow = 0;
			f_poParent->status_bar ( )->message ( _ ( "search hit TOP, continuing at BOTTOM" ) );
			}
/* end od it */
		}
	if ( l_pcHighlightStart )
		{
		if ( l_iMoveFirstRow )
			to_head ( f_poFirstVisibleRow, l_iMoveFirstRow );
		f_sMatch.f_iColumnWithMatch = l_iCtr;
		f_sMatch.f_iMatchNumber = l_iCtrLoc;
		f_sMatch.f_poCurrentMatch = f_poSelected;
		}
	else
		{
		f_poSelected = l_poSelectedOrig;
		f_poFirstVisibleRow = l_poFirstVisibleRowOrig;
		f_iCursorPosition = l_iCursorPositionOrig;
		f_iControlOffset = l_iControlOffsetOrig;
		f_sMatch.f_iMatchNumber = - 1;
		f_sMatch.f_iColumnWithMatch = 0;
		f_poParent->status_bar ( )->message ( _ ( "pattern not found" ) );
		}
	return;
	M_EPILOG
	}


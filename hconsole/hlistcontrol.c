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

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hlistcontrol.h"

#include "../hcore/hexception.h"

HListControl::HListControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
														 int a_iHeight, int a_iWidth,
														 const char * a_pcLabel )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
												 a_pcLabel )
	{
	M_PROLOG
	f_bEditable = false;
	f_bFiltered = false;
	f_iControlOffset = 0;
	f_iCursorPosition = 0;
	f_iSumForOne = 0;
	f_poFirstVisibleRow = NULL;
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
	int l_iColumns = 0;
	int l_iColumnOffset = 0;
	int l_iCurrentColumnWidth = 0;
	int l_iFlags = 0;
	int l_iTmp = 0;
	long int l_lValue = 0;
	double l_dScaled = 0;
	HInfo l_oInfo;
	l_iTmp = f_iWidthRaw;
	draw_label ( );
	if ( ! f_iSumForOne )return;
	l_iColumns = f_oHeader.quantity ( );
	if ( f_iWidthRaw != l_iTmp )recalculate_column_widths ( );
/* we need to decrement f_iHeightRaw because we have additional row, 
 * the list control header */
	f_iHeightRaw --;
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	if ( f_iQuantity > 0 )
		{
		f_poSelected = f_poFirstVisibleRow;
		for ( l_iCtr = 0;
					l_iCtr < ( f_iQuantity > f_iHeightRaw ? f_iHeightRaw : f_iQuantity );
					l_iCtr ++ )
			{
			l_iColumnOffset = 0;
			for ( l_iCtrLoc = 0; l_iCtrLoc < l_iColumns; l_iCtrLoc ++ )
				{
				l_oInfo = f_oHeader [ l_iCtrLoc ];
				l_lValue = l_oInfo;
				l_iFlags = l_oInfo;
				l_iCurrentColumnWidth = l_lValue & 0x000000ff;
				if ( l_iCurrentColumnWidth )
					{
					move ( f_iRowRaw + l_iCtr + 1, f_iColumnRaw + l_iColumnOffset );
					f_oVarTmpBuffer [ 0 ] = 0;
					switch ( l_iFlags & D_TYPE_MASK ) /* 0x0ffff is mask for type */
						{
						case ( D_TYPE_LONG_INT ):
							{
							f_oVarTmpBuffer = ( long int & ) f_poSelected->
																						get_object ( ) [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_DOUBLE ):
							{
							f_oVarTmpBuffer = ( double & ) f_poSelected->
																									get_object ( ) [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_HSTRING ):
							{
							f_oVarTmpBuffer = ( HString & ) f_poSelected->
																									get_object ( ) [ l_iCtrLoc ];
							break;
							}
						case ( D_TYPE_HTIME ):
							{
							f_oVarTmpBuffer = ( HString & ) ( HTime & ) f_poSelected->
																									get_object ( ) [ l_iCtrLoc ];
							break;
							}
						default :
							{
							throw new HException ( __WHERE__, "unknown type",
									l_iFlags & D_TYPE_MASK );
							break;
							}
						}
					l_iTmp = f_oVarTmpBuffer.get_length ( );
					switch ( l_iFlags & D_ALIGN_MASK ) /* 0xff00 is mask for align */
						{
						case ( D_ALIGN_LEFT ):
							{
							if ( l_iTmp < l_iCurrentColumnWidth )
								{
								memset ( ( char * ) f_oVarTmpBuffer + l_iTmp, '_',
										l_iCurrentColumnWidth - l_iTmp );
								}
							f_oVarTmpBuffer [ l_iCurrentColumnWidth ] = 0;
							break;
							}
						case ( D_ALIGN_CENTER ):
							{
							if ( l_iTmp > l_iCurrentColumnWidth )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right ( l_iCurrentColumnWidth );
							else if ( l_iTmp < l_iCurrentColumnWidth )
								{
								memmove ( ( char * ) f_oVarTmpBuffer 
										+ ( l_iCurrentColumnWidth - l_iTmp ) / 2, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								memset ( f_oVarTmpBuffer, '_', ( l_iCurrentColumnWidth - l_iTmp ) / 2 );
								l_iTmp = f_oVarTmpBuffer.get_length ( );
								memset ( ( char * ) f_oVarTmpBuffer + l_iTmp, '_',
										l_iCurrentColumnWidth - l_iTmp );
								f_oVarTmpBuffer [ l_iCurrentColumnWidth ] = 0;
								}
							break;
							}
						case ( D_ALIGN_RIGHT ):
							{
							if ( l_iTmp > l_iCurrentColumnWidth )
								f_oVarTmpBuffer = f_oVarTmpBuffer.right ( l_iCurrentColumnWidth );
							else if ( l_iTmp < l_iCurrentColumnWidth )
								{
								memmove ( ( char * ) f_oVarTmpBuffer + l_iCurrentColumnWidth - l_iTmp, 
										f_oVarTmpBuffer, l_iTmp + 1 );
								memset ( f_oVarTmpBuffer, '_', l_iCurrentColumnWidth - l_iTmp );
								}
							break;
							}
						default :
							{
							throw new HException ( __WHERE__, "unknown align",
									l_iFlags & D_ALIGN_MASK );
							break;
							}
						}
					if ( l_iCtr == f_iCursorPosition )
						console::set_attr ( f_bEnabled ? ( f_bFocused ? ~f_iFocusedAttribute
									: ~ f_iEnabledAttribute ) : ~ f_iAttribute );
					else
						console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute
									: f_iEnabledAttribute ) : f_iAttribute );
					cprintf ( f_oVarTmpBuffer	);
					l_iColumnOffset += l_iCurrentColumnWidth;
					}
				}
			to_tail ( );
			}
		}
	l_iColumnOffset = 0;
	console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute
				: f_iEnabledAttribute ) : f_iAttribute );
	memset ( f_oVarTmpBuffer, '.', f_iWidthRaw );
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	for ( ; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		{
		move ( f_iRowRaw + l_iCtr + 1,	f_iColumnRaw );
		cprintf ( f_oVarTmpBuffer );
		}
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_oInfo = f_oHeader [ l_iCtr ];
		l_lValue = l_oInfo;
		l_iCurrentColumnWidth = l_lValue & 0x000000ff;
		if ( l_iCurrentColumnWidth )
			{
			f_oVarTmpBuffer = ( HString & ) l_oInfo;
			move ( f_iRowRaw, f_iColumnRaw + l_iColumnOffset );
			console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute >> 8 : f_iEnabledAttribute >> 8 ) : f_iAttribute >> 8 );
			f_oVarTmpBuffer.format ( "%%-%ds", l_iCurrentColumnWidth );
			cprintf ( f_oVarTmpBuffer, ( char * ) ( ( HString & ) l_oInfo ).left ( l_iCurrentColumnWidth ) );
			move ( f_iRowRaw, f_iColumnRaw + l_iColumnOffset + ( l_lValue >> 16 ) );
			console::set_attr ( ! f_bEnabled ? ( ! f_bFocused ? f_iFocusedAttribute >> 8 : f_iEnabledAttribute >> 8 ) : f_iAttribute >> 8 );
			cprintf ( "%c", l_oInfo [ l_lValue >> 16 ] );
			l_iColumnOffset += l_iCurrentColumnWidth;
			if ( l_iCtr < l_iColumns )
				{
				console::set_attr ( f_iAttribute );
				for ( l_iCtrLoc = 0; l_iCtrLoc <= f_iHeightRaw; l_iCtrLoc ++ )
					{
					move ( f_iRowRaw + l_iCtrLoc,	f_iColumnRaw + l_iColumnOffset - 1 );
					cprintf ( "|" );
					}
				}
			}
		}
	console::set_attr ( ! f_bEnabled ? ( ! f_bFocused ? f_iFocusedAttribute : f_iEnabledAttribute ) : f_iAttribute );
	if ( f_iQuantity )
		{
		if ( f_iControlOffset )
			{
			move ( f_iRowRaw + 1, f_iColumnRaw + l_iColumnOffset - 1 );
			cprintf ( "^" );
			}
		if ( ( f_iQuantity - f_iControlOffset ) > f_iHeightRaw )
			{
			move ( f_iRowRaw + f_iHeightRaw, f_iColumnRaw + l_iColumnOffset - 1 );
			cprintf ( "v" );
			}
		l_dScaled = f_iHeightRaw - 3;
		l_dScaled *= ( double ) ( f_iControlOffset + f_iCursorPosition );
		l_dScaled /= ( double )f_iQuantity;
		move ( f_iRowRaw + ( int ) ( l_dScaled + 2.5 ),
				f_iColumnRaw + l_iColumnOffset - 1 );
		cprintf ( "#" );
		}
	curs_set ( 0 );
	return;
	M_EPILOG
	}

int HListControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	int l_iErrorCode = 0;
	int l_iOldPosition = 0;
	long l_lShortcutIndex;
	void * l_pvPointer = NULL;
	HElement * l_poElement = f_poSelected;
	f_poSelected = f_poFirstVisibleRow;
	l_iOldPosition = f_iControlOffset + f_iCursorPosition;
	a_iCode = HControl::process_input ( a_iCode );
	switch ( a_iCode )
		{
		case ( KEY_PPAGE ):
			{
			if ( ! f_iCursorPosition )
				{
				if ( f_iControlOffset )
					{
					to_head ( f_iHeightRaw - 1, & l_iFlag );
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
						f_poSelected = f_poHook;
						to_head ( f_iHeightRaw );
						f_iControlOffset = f_iQuantity - f_iHeightRaw;
						}
					else to_tail ( f_iHeightRaw - 1, & l_iFlag );
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
					f_iControlOffset --;
					}
				}
			else putchar ( '\a' );
			break;
			}
		case ( KEY_HOME ):
			{
			f_iCursorPosition = 0;
			f_iControlOffset = 0;
			f_poSelected = f_poHook;
			break;
			}
		case ( KEY_END ):
			{
			if ( f_iQuantity >= f_iHeightRaw )
				{
				f_poSelected = f_poHook;
				to_head ( f_iHeightRaw );
				f_iCursorPosition = f_iHeightRaw - 1;
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
					to_tail ( );
					}
				}
			else putchar ( '\a' );
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
				{
				l_lShortcutIndex = f_oHeader [ l_iCtr ];
				l_lShortcutIndex >>= 16;
				if ( tolower ( a_iCode ) == tolower ( f_oHeader [ l_iCtr ] [ l_lShortcutIndex ] ) )break;
				}
			if ( l_iCtr < l_iErrorCode )
				{
				l_iErrorCode = 0;
				sort_by_contents ( l_iCtr,
						a_iCode == tolower ( a_iCode ) ? D_ASCENDING : D_DESCENDING );
				l_poElement = f_poSelected = f_poHook;
				f_iControlOffset = 0;
				f_iCursorPosition = 0;
				}
			else l_iErrorCode = a_iCode;
			break;
			}
		}
	f_poFirstVisibleRow = f_poSelected;
	f_poSelected = l_poElement;
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		if ( l_iOldPosition != ( f_iControlOffset + f_iCursorPosition ) )
			{
			go ( f_iControlOffset + f_iCursorPosition );
			l_iOldPosition = f_oHeader.quantity ( );
			for ( l_iCtr = 0; l_iCtr < l_iOldPosition; l_iCtr ++ )
				{
				l_pvPointer = f_oHeader [ l_iCtr ];
				if ( l_pvPointer )
					( * ( HControl * ) l_pvPointer ) = f_poSelected
																										->get_object ( ) [ l_iCtr ];
				}
			l_iOldPosition++;
			}
		refresh ( );
		console::c_printf ( console::n_iHeight - 1, -1, 7, "" );
		}
	return ( a_iCode );
	M_EPILOG
	}

void HListControl::add_column ( const int & a_riColumn, const char * a_pcName,
		const int & a_riWidth, const int & a_riAlign, const int & a_riType, 
		HControl * a_poControl )
	{
	M_PROLOG
	long int l_lShortcutIndex = 0;
	long int l_lValue = 0;
	HInfo l_oInfo;
	if ( f_iQuantity )
		throw new HException ( __WHERE__,
				"can not add new column when list not empty", f_iQuantity );
	f_oVarTmpBuffer = a_pcName;
	l_lShortcutIndex = f_oVarTmpBuffer.find ( '&' );
	if ( l_lShortcutIndex > -1 )
		{
		f_oVarTmpBuffer [ l_lShortcutIndex ] = 0;
		f_oVarTmpBuffer += a_pcName + l_lShortcutIndex + 1;
		}
	else l_lShortcutIndex = 0;
	f_iSumForOne += a_riWidth;
	l_lShortcutIndex <<= 16;
	l_lValue = a_riWidth << 8;
	l_lValue |= l_lShortcutIndex;
	l_oInfo = a_riType | a_riAlign;
	l_oInfo = l_lValue;
	l_oInfo = f_oVarTmpBuffer;
	l_oInfo = ( void * ) a_poControl;
	f_oHeader.add_at ( a_riColumn, l_oInfo );
	recalculate_column_widths ( );
	refresh ( );
	return;
	M_EPILOG
	}

HInfoList & HListControl::add_tail ( void )
	{
	M_PROLOG
	HInfoList l_oDummy ( f_oHeader.quantity ( ) );
	return ( add_tail ( l_oDummy ) );
	M_EPILOG
	}

HInfoList & HListControl::add_tail ( HInfoList & a_roInfoList )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	HInfoList * l_oDummy = NULL;
	if ( f_iQuantity >= f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = f_iQuantity - f_iHeightRaw + 1;
		if ( f_poFirstVisibleRow )
			{
			l_poElement = f_poSelected;
			f_poSelected = f_poFirstVisibleRow;
			to_tail ( );
			f_poFirstVisibleRow = f_poSelected;
			f_poSelected = l_poElement;
			}
		}
	else f_iCursorPosition = f_iQuantity;
	l_oDummy = & HFlexibleList::add_tail ( a_roInfoList );
	if ( ! f_poFirstVisibleRow )f_poFirstVisibleRow = f_poHook;
	return ( * l_oDummy );
	M_EPILOG
	}

int HListControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	curs_set ( 0 );
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
	long int l_lValue = 0;
	l_iColumns = f_oHeader.quantity ( );
	for ( l_iCtr = 0; l_iCtr < l_iColumns; l_iCtr ++ )
		{
		l_lValue = f_oHeader [ l_iCtr ];
		l_lValue &= 0xffffff00;
		l_iNewWidth = ( l_lValue & 0x0000ff00 ) >> 8;
		if ( l_iNewWidth )
			{
			l_iNewWidth *= f_iWidthRaw;
			l_iNewWidth /= f_iSumForOne;
			l_iCtrLoc = l_iCtr;
			l_lValue |= l_iNewWidth;
			f_oHeader [ l_iCtr ] = l_lValue;
			l_iColumnOffset += l_iNewWidth;
			}
		}
	l_lValue = f_oHeader [ l_iCtrLoc ];
	l_iColumnOffset -= ( l_lValue & 0x000000ff );
	l_lValue &= 0xffffff00;
	l_lValue |= ( f_iWidthRaw - l_iColumnOffset );
	f_oHeader [ l_iCtrLoc ] = l_lValue;
	return;
	M_EPILOG
	}

HInfoList HListControl::remove_element ( int * a_piFlag )
	{
	M_PROLOG
	if ( f_poFirstVisibleRow == f_poSelected )
		{
		to_tail ( );
		f_poFirstVisibleRow = f_poSelected;
		to_head ( );
		}
	return ( HFlexibleList::remove_element ( a_piFlag ) );
	M_EPILOG
	}

int HListControl::cmpc ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	HInfo & l_roLeftInfo = a_poLeft->get_object ( ) [ f_iSortColumn ];
	HInfo & l_roRightInfo = a_poRight->get_object ( ) [ f_iSortColumn ];
	switch ( ( ( int ) f_oHeader [ f_iSortColumn ] ) & D_TYPE_MASK )
		{
		case ( D_TYPE_LONG_INT ):
			{
			return ( ( ( long ) l_roLeftInfo ) - ( ( long ) l_roRightInfo ) );
			break;
			}
		case ( D_TYPE_DOUBLE ):
			{
			return ( ( int ) ( ( ( ( double ) l_roLeftInfo ) - ( ( double ) l_roRightInfo ) ) * 100 ) );
			break;
			}
		case ( D_TYPE_HSTRING ):
			{
			return ( strcasecmp ( ( HString& ) l_roLeftInfo,
					( ( HString& ) l_roRightInfo ) ) );
			break;
			}
		case ( D_TYPE_HTIME ):
			{
			return ( ( ( time_t ) ( HTime& ) l_roLeftInfo ) - ( ( time_t ) ( HTime& ) l_roRightInfo ) );
			break;
			}
		default :
			{
			break;
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HListControl::sort_by_contents ( int a_iColumn, int a_iOrder )
	{
	M_PROLOG
	f_iSortColumn = a_iColumn;
	f_iOrder = a_iOrder;
	cmp = ( int ( HList<HInfoList>::* ) ( HElement *, HElement * ) ) & HListControl::cmpc;
	sort ( );
	return;
	M_EPILOG
	}


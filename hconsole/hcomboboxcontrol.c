/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxcontrol.c - this file is integral part of `stdhapi' project.

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

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcomboboxcontrol.h"

HComboboxControl::HComboboxControl ( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		const char * a_pcLabel, int a_iDroppedWidth,
		int a_iMaxLength, const char * a_pcMask,
		bool a_bCheckable, bool a_bSortable, bool a_bSearchable,
		bool a_bDrawLabel, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
								: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL, a_iMaxLength, "",
											a_pcMask ),
									HSearchableControl ( a_bSearchable ),
									HListControl ( NULL, 0, 0, 0, 0, NULL,
											a_bCheckable, a_bSortable, a_bSearchable, false )
	{
	M_PROLOG
	f_iMode = D_MODE_EDITCONTROL;
	f_iDroppedWidth = a_iDroppedWidth;
	return;
	M_EPILOG
	}

HComboboxControl::~HComboboxControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HComboboxControl::set_focus ( char a_cCode )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_EDITCONTROL )
		return ( HEditControl::set_focus ( a_cCode ) );
	return ( HListControl::set_focus ( a_cCode ) );
	M_EPILOG
	}

int HComboboxControl::kill_focus ( void )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_LISTCONTROL )
		{
		f_iMode = D_MODE_EDITCONTROL;
		console::clrscr ( );
		f_poParent->refresh ( );
		}
	return ( HControl::kill_focus ( ) );
	M_EPILOG
	}

void HComboboxControl::refresh ( void )
	{
	M_PROLOG
	int l_iWidth = 0;
	int l_iHeight = 0;
	if ( f_iMode == D_MODE_EDITCONTROL )
		{
/* ripped from HControl::draw_label ( ) */
		l_iWidth = ( f_iWidth > 0 ) ? f_iWidth
			: console::n_iWidth + f_iWidth - f_iColumnRaw;
/* end of ripped part */
		HEditControl::refresh ( );
		::move ( f_iRowRaw, f_iColumnRaw + l_iWidth - 1 );
		M_SET_ATTR_LABEL ( );
		addch ( D_ASCII_DOWN_ARROW );
		::move ( f_iRowRaw, f_iColumnRaw + HEditControl::f_iCursorPosition );
		f_iHeightRaw = 0;
		}
	else
		{
		l_iHeight = f_iHeight;
		l_iWidth = f_iWidth;
		f_iWidth = f_iDroppedWidth;
		if ( f_iQuantity < f_iHeight )f_iHeight = f_iQuantity + 1;
		HListControl::refresh ( );
		f_iHeight = l_iHeight;
		f_iWidth = l_iWidth;
		}
	return;
	M_EPILOG
	}

int HComboboxControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_EDITCONTROL )
		{
		switch ( a_iCode )
			{
			case ( KEY_UP ):
				{
				f_iMode = D_MODE_LISTCONTROL;
				break;
				}
			case ( KEY_DOWN ):
				{
				f_iMode = D_MODE_LISTCONTROL;
				break;
				}
			default :
				{
				return ( HEditControl::process_input ( a_iCode ) );
				break;
				}
			}
		refresh ( );
		}
	else
		{
		if ( a_iCode != '\r' )return ( HListControl::process_input ( a_iCode ) );
		close_combo ( );
		}
	return ( 0 );
	M_EPILOG
	}

int HComboboxControl::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_EDITCONTROL )
		{
		HEditControl::click ( a_rsMouse );
		f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
			: console::n_iWidth + f_iWidth - f_iColumnRaw;
		if ( a_rsMouse.f_iColumn == ( f_iColumnRaw + f_iWidthRaw - 1 ) )
			{
			f_iMode = D_MODE_LISTCONTROL;
			refresh ( );
			}
		}
	else if ( HListControl::click ( a_rsMouse ) )
		close_combo ( );
	return ( 0 );
	M_EPILOG
	}
			
void HComboboxControl::close_combo ( void )
	{
	M_PROLOG
	f_iMode = D_MODE_EDITCONTROL;
	if ( f_iQuantity )
		HEditControl::operator = ( ( HString & ) present ( ) [ 0 ] );
	console::clrscr ( );
	f_poParent->refresh ( );
	return;
	M_EPILOG
	}

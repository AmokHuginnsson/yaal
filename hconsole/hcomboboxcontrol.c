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

#include <ncurses.h>

#include "hcomboboxcontrol.h"

#include "hexception.h"

HComboboxControl::HComboboxControl ( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		const char * a_pcLabel, int a_iDroppedWidth,
		bool a_bCheckable, bool a_bSortable, bool a_bSearchable,
		bool a_bDrawLabel, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
								: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL ),
									HListControl ( NULL, 0, 0, 0, 0, NULL,
											a_bCheckable, a_bSortable, a_bSearchable )
	{
	M_PROLOG
	f_iMode = D_MODE_EDIT;
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
	if ( f_iMode == D_MODE_EDIT )
		return ( HEditControl::set_focus ( a_cCode ) );
	return ( HListControl::set_focus ( a_cCode ) );
	M_EPILOG
	}

void HComboboxControl::refresh ( void )
	{
	M_PROLOG
	bool l_bOldDrawLabel = f_bDrawLabel;
	if ( f_iMode == D_MODE_EDIT )
		HEditControl::refresh ( );
	else
		{
		f_bDrawLabel = false;
		HListControl::refresh ( );
		}
	f_bDrawLabel = l_bOldDrawLabel;
	return;
	M_EPILOG
	}

int HComboboxControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_EDIT )
		{
		switch ( a_iCode )
			{
			case ( KEY_UP ):
				{
				f_iMode = D_MODE_LIST;
				break;
				}
			case ( KEY_DOWN ):
				{
				f_iMode = D_MODE_LIST;
				break;
				}
			default :
				{
				return ( HEditControl::process_input ( a_iCode ) );
				break;
				}
			}
		}
	else
		{
		if ( a_iCode == '\r' )f_iMode = D_MODE_EDIT;
		else return ( HListControl::process_input ( a_iCode ) );
		}
	return ( 0 );
	M_EPILOG
	}


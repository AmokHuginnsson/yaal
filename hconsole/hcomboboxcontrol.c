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

#include "hcomboboxcontrol.h"

#include "hexception.h"

HComboboxControl::HComboboxControl ( HWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		const char * a_pcLabel, int a_iDroppedWidth, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute, bool a_bCheckable )
								: HControl ( a_poParent, a_iRow, a_iColumn,
										a_iHeight, a_iWidth, a_pcLabel, a_iDisabledAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL ),
									HListControl ( NULL, 0, 0, 0, 0, NULL, 0, 0, 0, a_bCheckable )
	{
	M_PROLOG
	f_iMode = D_MODE_LIST;
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
	if ( f_iMode == D_MODE_EDIT )
		HEditControl::refresh ( );
	else
		HListControl::refresh ( );
	return;
	M_EPILOG
	}

int HComboboxControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_EDIT )
		return ( HEditControl::process_input ( a_iCode ) );
	return ( HListControl::process_input ( a_iCode ) );
	M_EPILOG
	}


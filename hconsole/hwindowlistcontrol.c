/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindowlist.c - this file is integral part of `stdhapi' project.

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

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hwindowlistcontrol.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HWindowListControl::HWindowListControl ( HWindow * a_poParent, int a_iRow,
		int a_iColumn, int a_iHeight, int a_iWidth, char const * a_pcLabel )
									:	HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
											a_iWidth, a_pcLabel ),
										HSearchableControl ( false ),
										HListControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
												a_iWidth, a_pcLabel, false, false, false, false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HWindowListControl::~HWindowListControl ( void )
	{
	M_PROLOG
	remove_tail ( D_FORCE_REMOVE_ELEMENT );
	flush ( ); /* see htree.h for explanation ! */
	return;
	M_EPILOG
	}

long int HWindowListControl::empty ( HElement* a_poElement )
	{
	M_PROLOG
	HWindow * l_poWindow = NULL;
	HItem * l_poItem = & a_poElement->get_object ( );
	if ( * l_poItem )
		{
		l_poWindow = static_cast < HWindow * > ( ( * l_poItem ) [ 0 ].get < void * > ( ) );
		if ( l_poWindow )
			delete l_poWindow;
		l_poWindow = NULL;
		}
	return ( HListControl::empty ( a_poElement ) );
	M_EPILOG
	}

int HWindowListControl::process_input ( int a_iCode )
	{
	M_PROLOG
	HElement * l_poElement = f_poSelected;
	a_iCode = HListControl::process_input ( a_iCode );
	if ( ( f_iQuantity > 1 ) && ( ( a_iCode == '\r' ) || ( a_iCode == ' ' ) ) )
		{
		go ( f_iControlOffset + f_iCursorPosition - 1 );
		a_iCode = D_KEY_META_( '\t' );
		}
	else
		f_poSelected = l_poElement;
	return ( a_iCode );
	M_EPILOG
	}

void HWindowListControl::refresh ( void )
	{
	M_PROLOG
	HElement * l_poElement = f_poSelected;
	if ( ! f_iQuantity )
		return;
	f_iQuantity --;
	if ( ( f_iControlOffset + f_iCursorPosition ) >= f_iQuantity )
		{
		if ( f_iControlOffset )
			{
			f_iControlOffset --;
			f_poSelected = f_poFirstVisibleRow;
			to_head ( );
			f_poFirstVisibleRow = f_poSelected;
			}
		else
			f_iCursorPosition --;
		}
	HListControl::refresh ( );
	f_iQuantity ++;
	f_poSelected = l_poElement;
	M_EPILOG
	}

HItem & HWindowListControl::add_tail ( HItem & a_roElement )
	{
	M_PROLOG
	HItem * l_poItem = & HListControl::add_tail ( a_roElement );
	if ( f_iQuantity > 1 )
		{
		exchange ( - 2, - 1 );
		if ( f_iQuantity > f_iHeightRaw )
			{
			go ( 0 );
			to_head ( f_iHeightRaw );
			f_poFirstVisibleRow = f_poSelected;
			}
		else
			f_poFirstVisibleRow = f_poHook;
		go ( - 2 );
		}
	return ( * l_poItem );
	M_EPILOG
	}

}

}


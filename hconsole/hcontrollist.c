/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrollist.c - this file is integral part of `stdhapi' project.

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

#include "hcontrollist.h"

#include "../hcore/hexception.h"

HControlList::HControlList ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HControlList::~HControlList ( void )
	{
	M_PROLOG
	flush ( ); /* see htree.h for explanation ! */
	return;
	M_EPILOG
	}

long int HControlList::empty ( HList < HControl * > ::HElement* a_poElement )
	{
	M_PROLOG
	HControl * l_poControl = a_poElement->get_object ( );
	if ( l_poControl )delete l_poControl;
	l_poControl = 0;
	return ( HList < HControl * > ::empty ( a_poElement ) );
	M_EPILOG
	}

HControl * HControlList::next_enabled ( char a_cShorcut )
	{
	M_PROLOG
	bool l_bLoop = true;
	HControl * l_poControlOld = present ( );
	HControl * l_poControlNew = NULL;
	do
		{
		to_tail ( );
		l_poControlNew = f_poSelected->get_object ( );
		l_bLoop = l_poControlNew->set_focus ( a_cShorcut );
		if ( l_poControlOld == l_poControlNew )l_bLoop = false;
		}
	while ( l_bLoop );
	if ( l_poControlNew != l_poControlOld )
		{
		l_poControlOld->kill_focus ( );
		l_poControlNew->set_focus ( );
		}
	return ( l_poControlNew );
	M_EPILOG
	}


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.c - this file is integral part of `stdhapi' project.

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

/* We need ncurses.h here because of KEY_HOME. */

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hdatawindow.h"
#include "hdatalistcontrol.h"

#include "../hcore/hexception.h"

HDataListControl::HDataListControl ( HRecordSet * a_poRecordSet,
		HWindow * a_poWindow, int a_iRow, int a_iColumn, int a_iHeight,
		int a_iWidth, const char * a_pcTitle, int a_iAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
								: HControl ( a_poWindow, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcTitle, a_iAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute ),
								HDataControl ( a_poRecordSet, NULL, 0, 0, 0, 0, NULL, 0, 0, 0 ),
								HListControl ( NULL, 0, 0, 0, 0, NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataListControl::~HDataListControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HDataListControl::populate ( long int /*a_iId*/ )
	{
	M_PROLOG
	int l_iCursorPosition = f_iCursorPosition;
	int l_iControlOffset = f_iControlOffset;
	HElement * l_poSelected = NULL, * l_poFirstVisibleRow = NULL;
	HInfoList l_oInfoList ( f_oHeader.quantity ( ) );
	HDataWindow * l_poParent = ( HDataWindow * ) f_poParent;
	l_poParent->set_sync_store ( & l_oInfoList );
	if ( f_poRecordSet->is_open ( ) )f_poRecordSet->requery ( );
	else f_poRecordSet->open ( );
	flush ( );
	while ( ! f_poRecordSet->is_eof ( ) )
		{
		l_oInfoList [ 0 ] = f_poRecordSet->m_id;
		add_tail ( l_oInfoList );
		if ( l_iControlOffset == ( f_iQuantity - 1 ) )
			l_poFirstVisibleRow = f_poFirstVisibleRow;
		if ( ( l_iCursorPosition + l_iControlOffset ) == ( f_iQuantity - 1 ) )
			l_poSelected = f_poSelected;
		f_poRecordSet->move_next ( );
		}
	l_poParent->set_sync_store ( );
	f_iCursorPosition = l_iCursorPosition;
	f_iControlOffset = l_iControlOffset;
	f_poSelected = l_poSelected;
	f_poFirstVisibleRow = l_poFirstVisibleRow;
	return;
	M_EPILOG
	}

long int HDataListControl::get_current_id ( void )
	{
	M_PROLOG
	return ( ( * ( HInfoList * ) & f_poSelected->get_object ( ) ) [ 0 ] );
	M_EPILOG
	}

void HDataListControl::add_new ( void )
	{
	M_PROLOG
	add_tail ( );
	process_input ( KEY_HOME );
	process_input ( KEY_END );
	return;
	M_EPILOG
	}

void HDataListControl::cancel_new ( void )
	{
	M_PROLOG
	remove_tail ( D_EMPTY_IF_NOT_EMPTIED );
	process_input ( KEY_HOME );
	process_input ( KEY_END );
	return;
	M_EPILOG
	}


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.c - this file is integral part of `stdhapi' project.

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

#include "hdatawindow.h"

#include "../hcore/hexception.h"

HDataWindow::HDataWindow ( const char * a_pcTitle, HDataBase * a_poDataBase,
		ODataControlInfo * a_psDataControlInfo )
	: HWindow ( a_pcTitle ), HRecordSet ( a_poDataBase )
	{
	M_PROLOG
	f_bModified = false;
	f_poMainControl = NULL;
	f_psDataControlInfo = a_psDataControlInfo;
	register_postprocess_handler ( D_KEY_COMMAND_('n'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_add_new );
	register_postprocess_handler ( D_KEY_COMMAND_('e'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_edit );
	register_postprocess_handler ( D_KEY_COMMAND_('d'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_delete );
	register_postprocess_handler ( D_KEY_COMMAND_('w'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_save );
	register_postprocess_handler ( D_KEY_CTRL_('r'),
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_requery );
	register_postprocess_handler ( D_KEY_ESC,
			( int ( HWindow::* ) ( int ) ) & HDataWindow::handler_cancel );
	return;
	M_EPILOG
	}

HDataWindow::~HDataWindow ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataWindow::init ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	HWindow::init ( );
	while ( f_psDataControlInfo [ l_iCtr ].f_pcLabel )
		{
		l_iCtr ++;
		}
	f_poMainControl->populate ( );
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_add_new ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_edit ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_delete ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_save ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_requery ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HDataWindow::handler_cancel ( int )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}


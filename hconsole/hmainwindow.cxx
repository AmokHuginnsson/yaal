/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmainwindow.cxx - this file is integral part of `yaal' project.

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
M_VCSID ( "$Id$" )
#include "hmainwindow.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HMainWindow::HMainWindow ( char const * a_pcTitle, HTUIProcess::model_ptr_t a_oWindows )
						: HWindow ( a_pcTitle ), f_poMenu ( NULL ), f_oWindowList ( a_oWindows )
	{
	M_PROLOG
	register_postprocess_handler ( KEY<'q'>::command, NULL,
				& HMainWindow::handler_close );
	return;
	M_EPILOG
	}

HMainWindow::~HMainWindow ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HMainWindow::init ( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( f_oFocusedChild.is_valid() && ( !! (*f_oFocusedChild) ) )
		return ( 0 );
	l_iError = HWindow::init ( );
	f_poMenu = new HMenuControl ( this, 1, 1, - 2,	- n_iWidth / 2 - 1,
			" &Menu \n" );
	f_poMenu->enable ( true );
	f_poMenu->set_focus ( );
	HWindowListControl * l_poWindowList = new HWindowListControl ( this, 1,
			- n_iWidth / 2 + 1, - 2, - 1, " &Opened window list: \n",
			HListControler<HWindow::ptr_t>::ptr_t( new HListControler<HWindow::ptr_t>( f_oWindowList ) ) );
	l_poWindowList->add_column ( -1, "&Okno", 1 );
	l_poWindowList->enable ( true );
	f_oControls.select( f_poMenu );
	return ( l_iError );
	M_EPILOG
	}

void HMainWindow::init_menu ( HTUIProcess * a_psProcess, OMenuItem * a_psMenu )
	{
	M_PROLOG
	f_poMenu->init ( a_psProcess,	a_psMenu );
	return;
	M_EPILOG
	}

int HMainWindow::handler_close ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = KEY<'x'>::command;
	return ( a_iCode );
	M_EPILOG
	}

}

}


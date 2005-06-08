/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.c - this file is integral part of `stdhapi' project.

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
#include "hcore/hlog.h"
#include "dbwrapper/db_driver_loader.h"
#include "hdataprocess.h"
#include "hdatawindow.h"

using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::dbwrapper;

namespace stdhapi
{

namespace hdata
{

HDataProcess::HDataProcess ( void ) : HProcess ( ), f_oXml ( ), f_oDataBase ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataProcess::init ( char const * a_pcProcessName, char const * a_pcResource )
	{
	M_PROLOG
	int l_iError = HProcess::init ( a_pcProcessName );
	if ( ! dbwrapper::db_connect )
		M_THROW ( "no database driver loaded", g_iErrNo );
	f_oXml.init ( a_pcResource );
	return ( l_iError );
	M_EPILOG
	}

HDataBase * HDataProcess::data_base ( void )
	{
	return ( & f_oDataBase );
	}

int HDataProcess::handler_quit ( int a_iCode, void * )
	{
	M_PROLOG
	HItem * l_poItem = NULL;
	HDataWindow * l_poWindow = NULL;
	if ( f_poWindows->quantity ( ) )
		{
		f_poWindows->go ( 0 );
		while ( ( l_poItem = f_poWindows->to_tail ( 1, D_TREAT_AS_OPENED ) ) )
			{
			l_poWindow = dynamic_cast < HDataWindow * > ( static_cast < HWindow * > ( ( * l_poItem ) [ 0 ].get < void * > ( ) ) );
			if ( l_poWindow && l_poWindow->is_modified ( ) )
				{
				f_poForegroundWindow = l_poWindow;
				handler_refresh ( 0 );
				if ( ! l_poWindow->status_bar ( )->confirm ( "exit program" ) )
					return ( 0 );
				}
			}
		}
	return ( HProcess::handler_quit ( a_iCode ) );
	M_EPILOG
	}

int HDataProcess::handler_close_window ( int a_iCode, void * )
	{
	M_PROLOG
	HDataWindow * l_poWindow = NULL;
	if ( f_poForegroundWindow )
		{
		l_poWindow = dynamic_cast < HDataWindow * > ( f_poForegroundWindow );
		if ( l_poWindow
				&& l_poWindow->is_modified ( )
				&& ! l_poWindow->status_bar ( )->confirm ( "close window" ) )
			return ( 0 );
		}
	return ( HProcess::handler_close_window ( a_iCode ) );
	M_EPILOG
	}

}

}


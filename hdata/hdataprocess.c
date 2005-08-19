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

#include <libintl.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/hlog.h"
#include "hconsole/hmainwindow.h"
#include "dbwrapper/db_driver_loader.h"
#include "hdataprocess.h"
#include "hdatawindow.h"

using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;
using namespace stdhapi::tools;
using namespace stdhapi::dbwrapper;

namespace stdhapi
{

namespace hdata
{

void * HDataXml::parse ( void * )
	{
	M_PROLOG
	return ( NULL );
	M_EPILOG
	}

OMenuItem * HDataXml::build_menu ( menu_handlers_map_t const & a_roHandlers,
		ONode & a_rsNode )
	{
	M_PROLOG
	bool l_bRepeat = false;
	int l_iLevel = 0, l_iLevelOld = - 1;
	int l_iCtr = 0, l_iCount = 0;
	char const l_pcMenuPath [ ] = "/resource/menu";
	char const * const l_pcError = _ ( "malformed resource file (menu section)" );
	char const * const l_pcUnexpected = _ ( ": unexpected node: " );
	OMenuItem l_sMenuItem, * l_psMenu = NULL;
	HList < OMenuItem > l_oSubMenu;
	while ( ( a_rsNode.f_iLevel >= l_iLevelOld )
			&& ( l_bRepeat
				|| ( ( ( l_iLevel = iterate ( a_rsNode, l_pcMenuPath ) ) >= 0 )
					&& ( l_iLevel >= l_iLevelOld ) ) ) )
		{
		l_bRepeat = false;
		if ( a_rsNode.f_oName == "menu" )
			continue;
		if ( a_rsNode.f_oName == "menu_item" )
			{
			l_sMenuItem.reset ( );
			for ( l_iCtr = 0; l_iCtr < 2; l_iCtr ++ )
				{
				if ( ! l_bRepeat )
					if ( iterate ( a_rsNode, l_pcMenuPath ) < 0 )
						M_THROW ( l_pcError, g_iErrNo );
				l_bRepeat = false;
				if ( a_rsNode.f_oName == "label" )
					l_sMenuItem.f_oLabel = a_rsNode.f_oContents;
				else if ( a_rsNode.f_oName == "handler" )
					{
					if ( ! a_roHandlers.get ( a_rsNode.f_oContents,
								l_sMenuItem.HANDLER ) )
						M_THROW ( HString ( _ ( "no such handler: " ) )
								+ a_rsNode.f_oContents, g_iErrNo );
					}
				else if ( a_rsNode.f_oName == "menu" )
					{
					l_sMenuItem.f_psSubMenu = build_menu ( a_roHandlers, a_rsNode );
					l_bRepeat = true;
					}
				else
					M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.f_oName
							+ '=' + a_rsNode.f_oContents,
							g_iErrNo );
				}
			}
		else
			M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.f_oName
					+ '=' + a_rsNode.f_oContents,
					g_iErrNo );
		if ( a_rsNode.f_iLevel >= 0 )
			l_oSubMenu.add_tail ( & l_sMenuItem );
		else
			break;
		l_iLevelOld = l_iLevel;
		}
	l_sMenuItem.reset ( );
	l_oSubMenu.add_tail ( & l_sMenuItem );
	l_psMenu = new OMenuItem [ l_iCount = l_oSubMenu.quantity ( ) ];
	l_oSubMenu.go ( - 1 );
	for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
		l_psMenu [ l_iCtr ] = ( * l_oSubMenu.to_tail ( ) );
	return ( l_psMenu );
	M_EPILOG
	}

void HDataXml::destroy_menu ( OMenuItem * a_psMenu )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_ASSERT ( a_psMenu );
	while ( ! a_psMenu [ l_iCtr ].f_oLabel.is_empty ( ) )
		{
		if ( a_psMenu [ l_iCtr ].f_psSubMenu )
			destroy_menu ( a_psMenu [ l_iCtr ].f_psSubMenu );
		l_iCtr ++;
		}
	delete [ ] a_psMenu;
	return;
	M_EPILOG
	}

HDataProcess::HDataProcess ( void )
	: HTUIProcess ( ), f_oXml ( ), f_oDataBase ( ), f_psRootMenu ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess ( void )
	{
	M_PROLOG
	if ( f_psRootMenu )
		f_oXml.destroy_menu ( f_psRootMenu );
	f_psRootMenu = NULL;
	return;
	M_EPILOG
	}

int HDataProcess::init_xrc ( char const * a_pcProcessName,
		char const * a_pcResource, menu_handlers_map_t const & a_roHandlers )
	{
	M_PROLOG
	int l_iError = HTUIProcess::init_tui ( a_pcProcessName );
	HMainWindow * l_poMainWindow = NULL;
	HXml::ONode l_sNode;
	if ( ! dbwrapper::db_connect )
		M_THROW ( "no database driver loaded", g_iErrNo );
	f_oXml.init ( a_pcResource );
	f_psRootMenu = f_oXml.build_menu ( a_roHandlers, l_sNode );
	l_poMainWindow = dynamic_cast < HMainWindow * > ( f_poForegroundWindow );
	M_ASSERT ( l_poMainWindow );
	l_poMainWindow->init_menu ( this, f_psRootMenu );
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
		f_poWindows->go ( - 1 );
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
	return ( HTUIProcess::handler_quit ( a_iCode ) );
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
	return ( HTUIProcess::handler_close_window ( a_iCode ) );
	M_EPILOG
	}

}

}


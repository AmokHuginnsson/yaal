/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.c - this file is integral part of `yaal' project.

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
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hconsole/hmainwindow.h"
#include "dbwrapper/db_driver_loader.h"
#include "hdataprocess.h"
#include "hdatawindow.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataProcess::HDataProcess ( void )
	: HTUIProcess ( ), f_oDataBase ( ), f_psRootMenu ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess ( void )
	{
	M_PROLOG
	if ( f_psRootMenu )
		destroy_menu ( f_psRootMenu );
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
	HXml l_oXml;
	if ( ! dbwrapper::db_connect )
		M_THROW ( "no database driver loaded", errno );
	l_oXml.init ( a_pcResource );
	HXml::ONode & l_sNode = l_oXml.parse ( "/resource/menu" );
	f_psRootMenu = build_sub_menu ( l_sNode, a_roHandlers );
	l_poMainWindow = dynamic_cast < HMainWindow * > ( f_poForegroundWindow );
	M_ASSERT ( l_poMainWindow );
	l_poMainWindow->init_menu ( this, f_psRootMenu );
	return ( l_iError );
	M_EPILOG
	}

OMenuItem * HDataProcess::build_sub_menu ( HXml::ONode & a_rsNode,
		menu_handlers_map_t const & a_roHandlers )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCount = 0;
	char const * const l_pcError = _ ( "malformed resource file (menu section)" );
	char const * const l_pcUnexpected = _ ( ": unexpected node: " );
	OMenuItem l_sMenuItem, * l_psMenu = NULL;
	HXml::ONode * l_psNode = NULL;
	HList < OMenuItem > l_oSubMenu;
	if ( ! a_rsNode.f_oChilds.quantity ( ) )
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.f_oName == "menu" )
		{
		for ( l_psNode = & a_rsNode.f_oChilds.go ( 0 ); l_psNode;
				l_psNode = a_rsNode.f_oChilds.to_tail ( 1, HList<HXml::ONode>::D_TREAT_AS_OPENED ) )
			{
			l_sMenuItem.reset ( );
			build_menu_item ( * l_psNode, l_sMenuItem, a_roHandlers );
			l_oSubMenu.add_tail ( & l_sMenuItem );
			}
		}
	else
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.f_oName
				+ '=' + a_rsNode.f_oContents.head ( ), errno );
	l_sMenuItem.reset ( );
	l_oSubMenu.add_tail ( & l_sMenuItem );
	l_psMenu = new OMenuItem [ l_iCount = l_oSubMenu.quantity ( ) ];
	l_oSubMenu.go ( - 1 );
	for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )
		l_psMenu [ l_iCtr ] = ( * l_oSubMenu.to_tail ( ) );
	return ( l_psMenu );
	M_EPILOG
	}

void HDataProcess::build_menu_item ( HXml::ONode & a_rsNode,
		OMenuItem & a_rsMenuItem, menu_handlers_map_t const & a_roHandlers )
	{
	M_PROLOG
	char const * const l_pcError = _ ( "malformed resource file (menu section)" );
	char const * const l_pcUnexpected = _ ( ": unexpected node: " );
	HXml::ONode * l_psNode = NULL;
	if ( ! a_rsNode.f_oChilds.quantity ( ) )
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.f_oName == "menu_item" )
		{
		for ( l_psNode = & a_rsNode.f_oChilds.go ( 0 ); l_psNode;
				l_psNode = a_rsNode.f_oChilds.to_tail ( 1, HList<HXml::ONode>::D_TREAT_AS_OPENED ) )
			{
			if ( l_psNode->f_oName == "label" )
				a_rsMenuItem.f_oLabel = l_psNode->f_oContents.head ( );
			else if ( l_psNode->f_oName == "handler" )
				{
				if ( ! a_roHandlers.get ( l_psNode->f_oContents.head ( ),
							a_rsMenuItem.HANDLER ) )
					M_THROW ( HString ( _ ( "no such handler: " ) )
							+ l_psNode->f_oContents.head ( ), errno );
				}
			else if ( l_psNode->f_oName == "menu" )
				a_rsMenuItem.f_psSubMenu = build_sub_menu ( * l_psNode, a_roHandlers );
			else
				M_THROW ( HString ( l_pcError ) + l_pcUnexpected + l_psNode->f_oName
						+ '=' + l_psNode->f_oContents.head ( ), errno );
			}
		}
	else
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.f_oName
				+ '=' + a_rsNode.f_oContents.head ( ), errno );
	return;
	M_EPILOG
	}

void HDataProcess::destroy_menu ( OMenuItem * a_psMenu )
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
		while ( ( l_poItem = f_poWindows->to_tail ( 1, HWindowListControl::D_TREAT_AS_OPENED ) ) )
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


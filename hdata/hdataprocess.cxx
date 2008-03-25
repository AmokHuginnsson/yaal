/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.cxx - this file is integral part of `yaal' project.

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
	: HTUIProcess(), f_oDataBase(), f_psRootMenu ( NULL )
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
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
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
	l_oXml.parse ( "/resource/menu" );
	HXml::HNodeProxy const node = *(l_oXml.get_root().begin());
	f_psRootMenu = build_sub_menu( node, a_roHandlers );
	M_ASSERT ( f_oForegroundWindow.is_valid() );
	l_poMainWindow = dynamic_cast<HMainWindow*>( &*(*f_oForegroundWindow) );
	M_ASSERT ( l_poMainWindow );
	l_poMainWindow->init_menu( this, f_psRootMenu );
	l_poMainWindow->refresh();
	return ( l_iError );
	M_EPILOG
	}

OMenuItem * HDataProcess::build_sub_menu( HXml::HNodeProxy const& a_rsNode,
		menu_handlers_map_t const& a_roHandlers )
	{
	M_PROLOG
	char const * const l_pcError = _ ( "malformed resource file (menu section)" );
	char const * const l_pcUnexpected = _ ( ": unexpected node: " );
	OMenuItem l_sMenuItem, * l_psMenu = NULL;
	typedef HList<OMenuItem> menu_item_list_t;
	menu_item_list_t l_oSubMenu;
	if ( ! a_rsNode.child_count() )
		M_THROW( HString( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.get_name() == "menu" )
		{
		for ( HXml::HIterator it = a_rsNode.begin(); it != a_rsNode.end(); ++ it )
			{
			l_sMenuItem.reset();
			build_menu_item( *it, l_sMenuItem, a_roHandlers );
			l_oSubMenu.add_tail( &l_sMenuItem );
			}
		}
	else
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.get_name()
				+ '=' + a_rsNode.get_value(), errno );
	l_sMenuItem.reset();
	l_oSubMenu.add_tail ( &l_sMenuItem );
	l_psMenu = new OMenuItem[ l_oSubMenu.size() ];
	int l_iCtr = 0;
	for ( menu_item_list_t::iterator it = l_oSubMenu.begin(); it != l_oSubMenu.end(); ++ it, ++ l_iCtr )
		l_psMenu[ l_iCtr ] = *it;
	return ( l_psMenu );
	M_EPILOG
	}

void HDataProcess::build_menu_item ( HXml::HNodeProxy const& a_rsNode,
		OMenuItem& a_rsMenuItem, menu_handlers_map_t const& a_roHandlers )
	{
	M_PROLOG
	char const* const l_pcError = _( "malformed resource file (menu section)" );
	char const* const l_pcUnexpected = _( ": unexpected node: " );
	if ( ! a_rsNode.has_childs() )
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.get_name() == "menu_item" )
		{
		for ( HXml::HIterator it = a_rsNode.begin(); it != a_rsNode.end(); ++ it )
			{
			HString const& name = (*it).get_name();
			HString const contents = (*(*it).begin()).get_type() == HXml::HNode::TYPE::D_CONTENT ? (*(*it).begin()).get_value() : "";
			if ( name == "label" )
				a_rsMenuItem.f_oLabel = contents;
			else if ( name == "handler" )
				{
				if ( ! a_roHandlers.get ( contents, a_rsMenuItem.HANDLER ) )
					M_THROW( HString( _( "no such handler: " ) )
							+ contents, errno );
				}
			else if ( name == "menu" )
				a_rsMenuItem.f_psSubMenu = build_sub_menu( *it, a_roHandlers );
			else
				M_THROW( HString( l_pcError ) + l_pcUnexpected + name
						+ '=' + contents, errno );
			}
		}
	else
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected + a_rsNode.get_name()
				+ '=' + a_rsNode.get_value(), errno );
	return;
	M_EPILOG
	}

void HDataProcess::destroy_menu( OMenuItem* a_psMenu )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_ASSERT ( a_psMenu );
	while ( ! a_psMenu [ l_iCtr ].f_oLabel.is_empty() )
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

int HDataProcess::handler_quit ( int a_iCode, void const* )
	{
	M_PROLOG
	HDataWindow * l_poWindow = NULL;
	if ( f_oWindows->size() )
		{
		model_t::iterator it = f_oWindows->begin();
		if ( it != f_oWindows->end() )
			++ it;
		for ( ; it != f_oWindows->end(); ++ it )
			{
			l_poWindow = dynamic_cast<HDataWindow*>( static_cast<HWindow*>( &*(*it) ) );
			if ( l_poWindow && l_poWindow->is_modified() )
				{
				select( l_poWindow );
				handler_refresh( 0 );
				if ( ! l_poWindow->status_bar()->confirm ( "exit program" ) )
					return ( 0 );
				}
			}
		}
	return ( HTUIProcess::handler_quit ( a_iCode ) );
	M_EPILOG
	}

int HDataProcess::handler_close_window ( int a_iCode, void const* )
	{
	M_PROLOG
	HDataWindow * l_poWindow = NULL;
	if ( !! (*f_oForegroundWindow) )
		{
		l_poWindow = dynamic_cast<HDataWindow*> ( &*f_oForegroundWindow );
		if ( l_poWindow
				&& l_poWindow->is_modified()
				&& ! l_poWindow->status_bar()->confirm ( "close window" ) )
			return ( 0 );
		}
	return ( HTUIProcess::handler_close_window ( a_iCode ) );
	M_EPILOG
	}

}

}


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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hlog.h"
#include "tools/hplugin.h"
#include "hconsole/hmainwindow.h"
#include "dbwrapper/db_driver_loader.h"
#include "hdataprocess.h"
#include "hdatawindow.h"
#include "oresource.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

static int const D_MENU_HANDLERS_MAP_SIZE = 32;

namespace
{

char const* node_val( HXml::HConstNodeProxy const& node )
	{
	M_PROLOG
	HXml::HConstIterator it = node.begin();
	char const* t = "";
	if ( it != node.end() )
		{
		if ( (*it).get_type() == HXml::HNode::TYPE::D_CONTENT )
			t = (*it).get_value().raw();
		}
	return ( t );
	M_EPILOG
	}

char const* node_val( HXml::HConstIterator const& it )
	{
	M_PROLOG
	return ( node_val( *it ) );
	M_EPILOG
	}

char const* attr_val( HXml::HConstNodeProxy const& node, char const* const name )
	{
	M_PROLOG
	HXml::HNode::properties_t const& props = node.properties();
	HXml::HNode::properties_t::const_iterator prop = props.find( name );
	M_ENSURE( prop != props.end() );
	return ( prop->second.raw() );
	M_EPILOG
	}

char const* attr_val( HXml::HConstIterator const& it, char const* const name )
	{
	M_PROLOG
	return ( attr_val( *it, name ) );
	M_EPILOG
	}

}

HDataProcess::HDataProcess( void )
	: HTUIProcess(), f_oDataBase( HDataBase::get_connector() ),
	f_oAutoHandlers( D_MENU_HANDLERS_MAP_SIZE ),
	f_oResource(), f_oResourceCache(), f_oColumnCache(),
	f_oEditCache(), f_oListCache(), f_psRootMenu( NULL )
	{
	M_PROLOG
	menu_handlers_map_t& l_oHandlers = f_oAutoHandlers;
	M_REGISTER_MENU_HANDLER( create_window );
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess( void )
	{
	M_PROLOG
	if ( f_psRootMenu )
		destroy_menu( f_psRootMenu );
	f_psRootMenu = NULL;
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HDataProcess::init_xrc( char const* a_pcProcessName,
		char const* a_pcResource, menu_handlers_map_t const & a_roHandlers )
	{
	M_PROLOG
	int l_iError = HTUIProcess::init_tui( a_pcProcessName );
	HMainWindow* l_poMainWindow = NULL;
	if ( ! dbwrapper::db_connect )
		M_THROW( "no database driver loaded", errno );
	f_oResource.load( a_pcResource );
	HXml::HNodeProxy const node = f_oResource.get_element_by_path( "/resource/menu" );
	f_psRootMenu = build_sub_menu( node, a_roHandlers );
	M_ASSERT( f_oForegroundWindow.is_valid() );
	l_poMainWindow = dynamic_cast<HMainWindow*>( &*(*f_oForegroundWindow) );
	M_ASSERT( l_poMainWindow );
	l_poMainWindow->init_menu( this, f_psRootMenu );
	l_poMainWindow->refresh();
	return ( l_iError );
	M_EPILOG
	}

OMenuItem* HDataProcess::build_sub_menu( HXml::HConstNodeProxy const& a_rsNode,
		menu_handlers_map_t const& a_roHandlers )
	{
	M_PROLOG
	char const* const l_pcError = _( "malformed resource file (menu section)" );
	char const* const l_pcUnexpected = _( ": unexpected node: " );
	OMenuItem l_sMenuItem, * l_psMenu = NULL;
	typedef HList<OMenuItem> menu_item_list_t;
	menu_item_list_t l_oSubMenu;
	if ( ! a_rsNode.child_count() )
		M_THROW( HString( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.get_name() == "menu" )
		{
		for ( HXml::HConstIterator it = a_rsNode.begin(); it != a_rsNode.end(); ++ it )
			{
			l_sMenuItem.reset();
			build_menu_item( *it, l_sMenuItem, a_roHandlers );
			l_oSubMenu.add_tail( &l_sMenuItem );
			}
		}
	else
		M_THROW( HString( l_pcError ) + l_pcUnexpected + a_rsNode.get_name()
				+ '=' + a_rsNode.get_value(), errno );
	l_sMenuItem.reset();
	l_oSubMenu.add_tail( &l_sMenuItem );
	l_psMenu = new OMenuItem[ l_oSubMenu.size() ];
	int l_iCtr = 0;
	for ( menu_item_list_t::iterator it = l_oSubMenu.begin(); it != l_oSubMenu.end(); ++ it, ++ l_iCtr )
		l_psMenu[ l_iCtr ] = *it;
	return ( l_psMenu );
	M_EPILOG
	}

void HDataProcess::build_menu_item( HXml::HConstNodeProxy const& a_rsNode,
		OMenuItem& a_rsMenuItem, menu_handlers_map_t const& a_roHandlers )
	{
	M_PROLOG
	char const* const l_pcError = _( "malformed resource file (menu section)" );
	char const* const l_pcUnexpected = _( ": unexpected node: " );
	if ( ! a_rsNode.has_childs() )
		M_THROW ( HString ( l_pcError ) + l_pcUnexpected, errno );

	if ( a_rsNode.get_name() == "menu_item" )
		{
		for ( HXml::HConstIterator it = a_rsNode.begin(); it != a_rsNode.end(); ++ it )
			{
			HString const& name = (*it).get_name();
			HXml::HConstIterator handlerIt = (*it).begin();
			HString contents = ( ( handlerIt != (*it).end() ) && ( (*handlerIt).get_type() == HXml::HNode::TYPE::D_CONTENT ) ) ? (*(*it).begin()).get_value() : "";
			if ( name == "label" )
				a_rsMenuItem.f_oLabel = contents;
			else if ( name == "handler" )
				{
				HXml::HNode::properties_t const& props = (*it).properties();
				HXml::HNode::properties_t::const_iterator nameIt = props.find( "name" );
				if ( nameIt != props.end() )
					contents = nameIt->second;
				if ( a_roHandlers.get( contents, a_rsMenuItem.HANDLER ) && f_oAutoHandlers.get( contents, a_rsMenuItem.HANDLER ) )
					M_THROW( HString( _( "no such handler: " ) ) + contents, errno );
				HXml::HNode::properties_t::const_iterator paramIt = props.find( "param" );
				if ( paramIt != props.end() )
					a_rsMenuItem.f_pvParam = const_cast<HString*>( &paramIt->second );
				}
			else if ( name == "menu" )
				a_rsMenuItem.f_psSubMenu = build_sub_menu( *it, a_roHandlers );
			else
				M_THROW( HString( l_pcError ) + l_pcUnexpected + name
						+ '=' + contents, errno );
			}
		}
	else
		M_THROW( HString( l_pcError ) + l_pcUnexpected + a_rsNode.get_name()
				+ '=' + a_rsNode.get_value(), errno );
	return;
	M_EPILOG
	}

void HDataProcess::destroy_menu( OMenuItem* a_psMenu )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_ASSERT( a_psMenu );
	while ( ! a_psMenu[ l_iCtr ].f_oLabel.is_empty() )
		{
		if ( a_psMenu[ l_iCtr ].f_psSubMenu )
			destroy_menu( a_psMenu [ l_iCtr ].f_psSubMenu );
		l_iCtr ++;
		}
	delete [ ] a_psMenu;
	return;
	M_EPILOG
	}

database_ptr_t HDataProcess::data_base( void )
	{
	return ( f_oDataBase );
	}

int HDataProcess::handler_quit( int a_iCode, void const* )
	{
	M_PROLOG
	HDataWindow* l_poWindow = NULL;
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
				if ( ! l_poWindow->status_bar()->confirm( "exit program" ) )
					return ( 0 );
				}
			}
		}
	return ( HTUIProcess::handler_quit( a_iCode ) );
	M_EPILOG
	}

int HDataProcess::handler_close_window( int a_iCode, void const* )
	{
	M_PROLOG
	HDataWindow* l_poWindow = NULL;
	if ( !! (*f_oForegroundWindow) )
		{
		l_poWindow = dynamic_cast<HDataWindow*>( &*f_oForegroundWindow );
		if ( l_poWindow
				&& l_poWindow->is_modified()
				&& ! l_poWindow->status_bar()->confirm( "close window" ) )
			return ( 0 );
		}
	return ( HTUIProcess::handler_close_window( a_iCode ) );
	M_EPILOG
	}

int HDataProcess::create_window( void* param )
	{
	M_PROLOG
	HPlugin self;
	self.load( HString() );
	window_factory_t window_factory = NULL;
	self.resolve( *static_cast<HString*>( param ), window_factory );
	if ( ! window_factory )
		M_THROW( _( "cannot locate factory for this window" ), errno );
	HXml::HConstNodeProxy w = f_oResource.get_element_by_id( *static_cast<HString*>( param ) );
	if ( ! w )
		M_THROW( _( "cannot find resources for this window" ), errno );
	HXml::HNode::properties_t const& windowProps = w.properties();
	HXml::HNode::properties_t::const_iterator titleIt = windowProps.find( "title" );
	M_ENSURE( titleIt != windowProps.end() );
	add_window( window_factory( titleIt->second, this, get_resource( *static_cast<HString*>( param ), w ) ) );
	return ( 0 );
	M_EPILOG
	}

OResource* HDataProcess::get_resource( HString const& name, HXml::HConstNodeProxy const& w )
	{
	M_PROLOG
	resource_cache_t::iterator it = f_oResourceCache.find( name );
	OResource* r = NULL;
	if ( it == f_oResourceCache.end() )
		r = build_resource( name, w );
	else
		r = it->second.raw();
	return ( r );
	M_EPILOG
	}

OResource* HDataProcess::build_resource( yaal::hcore::HString const& resourceName, HXml::HConstNodeProxy const& w )
	{
	M_PROLOG
	resource_pool_t r( w.child_count() + 1 );
	int i = 0;
	typedef HMap<int,char const*> int_to_str_t;
	typedef HMap<HString,int> str_to_int_t;
	int_to_str_t i2s;
	str_to_int_t s2i;
	for ( HXml::HConstIterator it = w.begin(); it != w.end(); ++ it, ++ i )
		{
		M_ENSURE( (*it).get_type() == HXml::HNode::TYPE::D_NODE );
		M_ENSURE( (*it).get_name() == "control" );
		HXml::HNode::properties_t const& controlProps = (*it).properties();
		HXml::HNode::properties_t::const_iterator roleIt = controlProps.find( "role" );
		HXml::HNode::properties_t::const_iterator typeIt = controlProps.find( "type" );
		HXml::HNode::properties_t::const_iterator refidIt = controlProps.find( "refid" );
		M_ENSURE( ( roleIt != controlProps.end() ) && ( typeIt != controlProps.end() ) );
		if ( roleIt->second == "main" )
			r[ i ].f_eRole = DATACONTROL_BITS::ROLE::D_MAIN;
		else if ( roleIt->second == "data" )
			r[ i ].f_eRole = DATACONTROL_BITS::ROLE::D_DATA;
		else
			M_THROW( _( "unknown role" ), i );
		if ( typeIt->second == "list" )
			{
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_LIST;
			f_oListCache.push_back( OListControlResource() );
			r[ i ].f_pvTypeSpecific = &f_oListCache.tail();
			}
		else if ( typeIt->second == "edit" )
			{
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_EDIT;
			f_oEditCache.push_back( OEditControlResource() );
			r[ i ].f_pvTypeSpecific = &f_oEditCache.tail();
			}
		else if ( typeIt->second == "tree" )
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_TREE;
		else if ( typeIt->second == "combo" )
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_COMBO;
		else if ( typeIt->second == "check" )
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_CHECK;
		else if ( typeIt->second == "date" )
			r[ i ].f_eType = DATACONTROL_BITS::TYPE::D_DATE;
		else
			M_THROW( _( "unknown type" ), i );
		if ( refidIt != controlProps.end() )
			s2i[ refidIt->second ] = i;
		int columnNo = 0;
		int attrNo = 0;
		for ( HXml::HConstIterator attr = (*it).begin(); attr != (*it).end(); ++ attr, ++ attrNo )
			{
			M_ENSURE( (*attr).get_type() == HXml::HNode::TYPE::D_NODE );
			HString const attrName = (*attr).get_name();
			if ( attrName == "label" )
				r[ i ].f_pcLabel = node_val( attr );
			else if ( attrName == "db" )
				{
				M_ENSURE( r[ i ].f_eRole == DATACONTROL_BITS::ROLE::D_MAIN );
				HXml::HNode::properties_t const& db = (*attr).properties();
				HXml::HNode::properties_t::const_iterator filterIt = db.find( "filter" );
				HXml::HNode::properties_t::const_iterator sortIt = db.find( "sort" );
				r[ i ].f_pcTable = attr_val( attr, "table" ); 
				r[ i ].f_pcColumns = attr_val( attr, "column" );
				r[ i ].f_pcId = attr_val( attr, "id_column" );
				if ( filterIt != db.end() )
					r[ i ].f_pcFilter = filterIt->second.raw();
				if ( sortIt != db.end() )
					r[ i ].f_pcSort = sortIt->second.raw();
				}
			else if ( attrName == "position" )
				{
				r[ i ].f_iRow = lexical_cast<int>( attr_val( attr, "row" ) );
				r[ i ].f_iColumn = lexical_cast<int>( attr_val( attr, "column" ) );
				r[ i ].f_iHeight = lexical_cast<int>( attr_val( attr, "height" ) );
				r[ i ].f_iWidth = lexical_cast<int>( attr_val( attr, "width" ) );
				}
			else if ( attrName == "parent" )
				{
				i2s[ i ] = attr_val( attr, "refid" );
				}
			else if ( r[ i ].f_eType == DATACONTROL_BITS::TYPE::D_LIST )
				{
				OListControlResource* l = static_cast<OListControlResource*>( r[ i ].f_pvTypeSpecific );
				if ( attrName == "column" )
					{
					if ( ! r[ i ].f_psColumnInfo )
						{
						column_pool_t c( ( (*it).child_count() - attrNo ) + 1 );
						f_oColumnCache.push_back( column_pool_t() );
						r[ i ].f_psColumnInfo = c.raw();
						column_pool_t::swap( f_oColumnCache.tail(), c );
						}
					r[ i ].f_psColumnInfo[ columnNo ].f_iPlacement = lexical_cast<int>( attr_val( attr, "placement" ) );
					r[ i ].f_psColumnInfo[ columnNo ].f_pcName = attr_val( attr, "name" );
					r[ i ].f_psColumnInfo[ columnNo ].f_iWidth = lexical_cast<int>( attr_val( attr, "width" ) );
					HXml::HNode::properties_t const& col = (*attr).properties();
					HXml::HNode::properties_t::const_iterator alignIt = col.find( "align" );
					HXml::HNode::properties_t::const_iterator colTypeIt = col.find( "type" );
					M_ENSURE( ( alignIt != col.end() ) && ( colTypeIt != col.end() ) );
					if ( alignIt->second == "left" )
						r[ i ].f_psColumnInfo[ columnNo ].f_eAlign = HControl::BITS::ALIGN::D_LEFT;
					else if ( alignIt->second == "center" )
						r[ i ].f_psColumnInfo[ columnNo ].f_eAlign = HControl::BITS::ALIGN::D_CENTER;
					else if ( alignIt->second == "right" )
						r[ i ].f_psColumnInfo[ columnNo ].f_eAlign = HControl::BITS::ALIGN::D_RIGHT;
					else
						M_THROW( _( "unknown align type" ), i );
					if ( colTypeIt->second == "string" )
						r[ i ].f_psColumnInfo[ columnNo ].f_eType = D_HSTRING;
					else
						M_THROW( _( "unknown column type" ), i );
					++ columnNo;
					}
				else if ( attrName == "checkable" )
					l->f_bCheckable = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "sortable" )
					l->f_bSortable = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "searchable" )
					l->f_bSearchable = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "editable" )
					l->f_bEditable = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "draw_header" )
					l->f_bDrawHeader = lexical_cast<bool>( node_val( attr ) );
				else
					M_THROW( _( "unknown list attribute name" ), i );
				}
			else if ( r[ i ].f_eType == DATACONTROL_BITS::TYPE::D_EDIT )
				{
				OEditControlResource* e = static_cast<OEditControlResource*>( r[ i ].f_pvTypeSpecific );
				if ( attrName == "max_string_size" )
					e->f_iMaxStringSize = lexical_cast<int>( node_val( attr ) );
				else if ( attrName == "value" )
					e->f_pcValue = node_val( attr );
				else if ( attrName == "mask" )
					e->f_pcMask = node_val( attr );
				else if ( attrName == "replace" )
					e->f_bReplace = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "multi_line" )
					e->f_bMultiLine = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "read_only" )
					e->f_bReadOnly = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "right_aligned" )
					e->f_bRightAligned = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "password" )
					e->f_bPassword = lexical_cast<bool>( node_val( attr ) );
				else if ( attrName == "max_history_level" )
					e->f_iMaxHistoryLevel = lexical_cast<int>( node_val( attr ) );
				else
					M_THROW( _( "unknown edit attribute name" ), i );
				}
			else
				M_THROW( _( "auto builing of this type of control is not supported yet" ), i );
			}
		}
	OResource* pr = r.raw();
	for ( int n = 0; n < i; ++ n )
		{
		int_to_str_t::iterator child = i2s.find( n );
		if ( child != i2s.end() )
			{
			str_to_int_t::iterator parentIt = s2i.find( child->second );
			M_ENSURE( parentIt != s2i.end() );
			pr[ n ].f_iParent = parentIt->second;
			pr[ n ].f_psColumnInfo = &pr[ parentIt->second ].f_psColumnInfo[ n - ( parentIt->second + 1 ) ];
			}
		}
	resource_pool_t::swap( f_oResourceCache[ resourceName ], r );
	return ( pr );
	M_EPILOG
	}

}

}


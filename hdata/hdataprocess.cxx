/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "tools/hplugin.hxx"
#include "hconsole/hmainwindow.hxx"
#include "dbwrapper/db_driver_loader.hxx"
#include "hdata/hdataprocess.hxx"
#include "hdatawindow.hxx"
#include "oresource.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

static int const MENU_HANDLERS_MAP_SIZE = 32;

HDataProcess::HDataProcess( void )
	: HTUIProcess(), _dataBase( HDataBase::get_connector() ),
	_autoHandlers( MENU_HANDLERS_MAP_SIZE ),
	_resource(), _resourceCache(), _columnCache(),
	_editCache(), _listCache(), _rootMenu( NULL )
	{
	M_PROLOG
	menu_handlers_map_t& handlers = _autoHandlers;
	M_REGISTER_MENU_HANDLER( create_window );
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess( void )
	{
	M_PROLOG
	if ( _rootMenu )
		destroy_menu( _rootMenu );
	_rootMenu = NULL;
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HDataProcess::init_xrc( char const* processName_,
		char const* resource_, menu_handlers_map_t const & handlers_ )
	{
	M_PROLOG
	int error = HTUIProcess::init_tui( processName_ );
	HMainWindow* mainWindow = NULL;
	if ( dbwrapper::_dBDrivers_.is_empty() )
		M_THROW( "no database driver loaded", errno );
	_resource.load( HStreamInterface::ptr_t( new HFile( resource_, HFile::OPEN::READING ) ) );
	HXml::HNodeProxy const node = _resource.get_element_by_path( "/resource/menu" );
	_rootMenu = build_sub_menu( node, handlers_ );
	M_ASSERT( ( _foregroundWindow	!= HTUIProcess::model_t::cyclic_iterator() ) && ( !! (*_foregroundWindow) ) );
	mainWindow = dynamic_cast<HMainWindow*>( &*(*_foregroundWindow) );
	M_ASSERT( mainWindow );
	mainWindow->init_menu( this, _rootMenu );
	mainWindow->refresh();
	return ( error );
	M_EPILOG
	}

OMenuItem* HDataProcess::build_sub_menu( HXml::HConstNodeProxy const& node_,
		menu_handlers_map_t const& handlers_ )
	{
	M_PROLOG
	char const* const error = _( "malformed resource file (menu section)" );
	char const* const unexpected = _( ": unexpected node: " );
	OMenuItem menuItem, * menu = NULL;
	typedef HList<OMenuItem> menu_item_list_t;
	menu_item_list_t subMenu;
	if ( ! node_.child_count() )
		M_THROW( HString( error ) + unexpected, errno );

	if ( node_.get_name() == "menu" )
		{
		for ( HXml::HConstIterator it = node_.begin(); it != node_.end(); ++ it )
			{
			menuItem.reset();
			build_menu_item( *it, menuItem, handlers_ );
			subMenu.push_back( menuItem );
			}
		}
	else
		M_THROW( HString( error ) + unexpected + node_.get_name()
				+ '=' + node_.get_value(), errno );
	menuItem.reset();
	subMenu.push_back( menuItem );
	menu = new OMenuItem[ subMenu.size() ];
	int ctr = 0;
	for ( menu_item_list_t::iterator it = subMenu.begin(); it != subMenu.end(); ++ it, ++ ctr )
		menu[ ctr ] = *it;
	return ( menu );
	M_EPILOG
	}

void HDataProcess::build_menu_item( HXml::HConstNodeProxy const& node_,
		OMenuItem& menuItem_, menu_handlers_map_t const& handlers_ )
	{
	M_PROLOG
	char const* const error = _( "malformed resource file (menu section)" );
	char const* const unexpected = _( ": unexpected node: " );
	if ( ! node_.has_childs() )
		M_THROW ( HString ( error ) + unexpected, errno );

	if ( node_.get_name() == "menu_item" )
		{
		for ( HXml::HConstIterator it = node_.begin(); it != node_.end(); ++ it )
			{
			HString const& name = (*it).get_name();
			HXml::HConstIterator handlerIt = (*it).begin();
			HString contents = ( ( handlerIt != (*it).end() ) && ( (*handlerIt).get_type() == HXml::HNode::TYPE::CONTENT ) ) ? (*(*it).begin()).get_value() : "";
			if ( name == "label" )
				menuItem_._label = contents;
			else if ( name == "handler" )
				{
				HXml::HNode::properties_t const& props = (*it).properties();
				HXml::HNode::properties_t::const_iterator nameIt = props.find( "name" );
				if ( nameIt != props.end() )
					contents = nameIt->second;
				menu_handlers_map_t::const_iterator handle( handlers_.find( contents ) );
				if ( handle == handlers_.end() )
					{
					handle = _autoHandlers.find( contents );
					if ( handle == _autoHandlers.end() )
						M_THROW( HString( _( "no such handler: " ) ) + contents, errno );
					}
				menuItem_.HANDLER = handle->second;
				HXml::HNode::properties_t::const_iterator paramIt = props.find( "param" );
				if ( paramIt != props.end() )
					menuItem_._param = const_cast<HString*>( &paramIt->second );
				}
			else if ( name == "menu" )
				menuItem_._subMenu = build_sub_menu( *it, handlers_ );
			else
				M_THROW( HString( error ) + unexpected + name
						+ '=' + contents, errno );
			}
		}
	else
		M_THROW( HString( error ) + unexpected + node_.get_name()
				+ '=' + node_.get_value(), errno );
	return;
	M_EPILOG
	}

void HDataProcess::destroy_menu( OMenuItem* menu_ )
	{
	M_PROLOG
	int ctr = 0;
	M_ASSERT( menu_ );
	while ( ! menu_[ ctr ]._label.is_empty() )
		{
		if ( menu_[ ctr ]._subMenu )
			destroy_menu( menu_ [ ctr ]._subMenu );
		ctr ++;
		}
	delete [ ] menu_;
	return;
	M_EPILOG
	}

database_ptr_t HDataProcess::data_base( void )
	{
	return ( _dataBase );
	}

int HDataProcess::handler_quit( int code_, void const* )
	{
	M_PROLOG
	HDataWindow* window = NULL;
	if ( _windows->size() )
		{
		model_t::iterator it = _windows->begin();
		if ( it != _windows->end() )
			++ it;
		for ( ; it != _windows->end(); ++ it )
			{
			window = dynamic_cast<HDataWindow*>( static_cast<HWindow*>( &*(*it) ) );
			if ( window && window->is_modified() )
				{
				select( window );
				handler_refresh( 0 );
				if ( ! window->status_bar()->confirm( "exit program" ) )
					return ( 0 );
				}
			}
		}
	return ( HTUIProcess::handler_quit( code_ ) );
	M_EPILOG
	}

int HDataProcess::handler_close_window( int code_, void const* )
	{
	M_PROLOG
	HDataWindow* window = NULL;
	if ( !! (*_foregroundWindow) )
		{
		window = dynamic_cast<HDataWindow*>( &*_foregroundWindow );
		if ( window
				&& window->is_modified()
				&& ! window->status_bar()->confirm( "close window" ) )
			return ( 0 );
		}
	return ( HTUIProcess::handler_close_window( code_ ) );
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
	HXml::HConstNodeProxy window_ = _resource.get_element_by_id( *static_cast<HString*>( param ) );
	if ( ! window_ )
		M_THROW( _( "cannot find resources for this window" ), errno );
	HXml::HNode::properties_t const& windowProps = window_.properties();
	HXml::HNode::properties_t::const_iterator titleIt = windowProps.find( "title" );
	M_ENSURE( titleIt != windowProps.end() );
	add_window( window_factory( titleIt->second, this, get_resource( *static_cast<HString*>( param ), window_ ) ) );
	return ( 0 );
	M_EPILOG
	}

resources_t* HDataProcess::get_resource( HString const& name, HXml::HConstNodeProxy const& window_ )
	{
	M_PROLOG
	resource_cache_t::iterator it = _resourceCache.find( name );
	resources_t* r( NULL );
	if ( it == _resourceCache.end() )
		r = &build_resource( name, window_ );
	else
		r = &it->second;
	return ( r );
	M_EPILOG
	}

resources_t& HDataProcess::build_resource( yaal::hcore::HString const& resourceName_, HXml::HConstNodeProxy const& window_ )
	{
	M_PROLOG
	resources_t r( window_.child_count() );
	int i = 0;
	typedef HMap<int, char const*> int_to_str_t;
	typedef HMap<HString, int> str_to_int_t;
	int_to_str_t i2s;
	str_to_int_t s2i;
	for ( HXml::HConstIterator it = window_.begin(); it != window_.end(); ++ it, ++ i )
		{
		M_ENSURE( (*it).get_type() == HXml::HNode::TYPE::NODE );
		M_ENSURE( (*it).get_name() == "control" );
		HXml::HNode::properties_t const& controlProps = (*it).properties();
		HXml::HNode::properties_t::const_iterator roleIt = controlProps.find( "role" );
		HXml::HNode::properties_t::const_iterator typeIt = controlProps.find( "type" );
		HXml::HNode::properties_t::const_iterator refidIt = controlProps.find( "refid" );
		M_ENSURE( ( roleIt != controlProps.end() ) && ( typeIt != controlProps.end() ) );
		if ( roleIt->second == "main" )
			r[ i ]._role = DATACONTROL_BITS::ROLE::MAIN;
		else if ( roleIt->second == "data" )
			r[ i ]._role = DATACONTROL_BITS::ROLE::DATA;
		else
			M_THROW( _( "unknown role" ), i );
		if ( typeIt->second == "list" )
			{
			r[ i ]._type = DATACONTROL_BITS::TYPE::LIST;
			_listCache.push_back( OListControlResource() );
			r[ i ]._typeSpecific = &_listCache.tail();
			}
		else if ( typeIt->second == "edit" )
			{
			r[ i ]._type = DATACONTROL_BITS::TYPE::EDIT;
			_editCache.push_back( OEditControlResource() );
			r[ i ]._typeSpecific = &_editCache.tail();
			}
		else if ( typeIt->second == "tree" )
			r[ i ]._type = DATACONTROL_BITS::TYPE::TREE;
		else if ( typeIt->second == "combo" )
			r[ i ]._type = DATACONTROL_BITS::TYPE::COMBO;
		else if ( typeIt->second == "check" )
			r[ i ]._type = DATACONTROL_BITS::TYPE::CHECK;
		else if ( typeIt->second == "date" )
			r[ i ]._type = DATACONTROL_BITS::TYPE::DATE;
		else
			M_THROW( _( "unknown type" ), i );
		if ( refidIt != controlProps.end() )
			s2i[ refidIt->second ] = i;
		int columnNo = 0;
		int attrNo = 0;
		for ( HXml::HConstIterator attr = (*it).begin(); attr != (*it).end(); ++ attr, ++ attrNo )
			{
			M_ENSURE( (*attr).get_type() == HXml::HNode::TYPE::NODE );
			HString const attrName = (*attr).get_name();
			if ( attrName == "label" )
				r[ i ]._label = xml::node_val( attr );
			else if ( attrName == "db" )
				{
				M_ENSURE( r[ i ]._role == DATACONTROL_BITS::ROLE::MAIN );
				HXml::HNode::properties_t const& db = (*attr).properties();
				HXml::HNode::properties_t::const_iterator filterIt = db.find( "filter" );
				HXml::HNode::properties_t::const_iterator sortIt = db.find( "sort" );
				r[ i ]._table = xml::attr_val( attr, "table" ); 
				r[ i ]._columns = xml::attr_val( attr, "column" );
				r[ i ]._id = xml::attr_val( attr, "id_column" );
				if ( filterIt != db.end() )
					r[ i ]._filter = filterIt->second.raw();
				if ( sortIt != db.end() )
					r[ i ]._sort = sortIt->second.raw();
				}
			else if ( attrName == "position" )
				{
				r[ i ]._row = lexical_cast<int>( xml::attr_val( attr, "row" ) );
				r[ i ]._column = lexical_cast<int>( xml::attr_val( attr, "column" ) );
				r[ i ]._height = lexical_cast<int>( xml::attr_val( attr, "height" ) );
				r[ i ]._width = lexical_cast<int>( xml::attr_val( attr, "width" ) );
				}
			else if ( attrName == "parent" )
				{
				i2s[ i ] = xml::attr_val( attr, "refid" );
				}
			else if ( r[ i ]._type == DATACONTROL_BITS::TYPE::LIST )
				{
				OListControlResource* l = static_cast<OListControlResource*>( r[ i ]._typeSpecific );
				if ( attrName == "column" )
					{
					if ( ! r[ i ]._columnInfo )
						{
						_columnCache.add_tail();
						HChunk& c = _columnCache.tail(); 
						c.realloc( chunk_size<OColumnInfo>( ( (*it).child_count() - attrNo ) + 1 ) );
						r[ i ]._columnInfo = c.get<OColumnInfo>();
						}
					r[ i ]._columnInfo[ columnNo ]._placement = lexical_cast<int>( xml::attr_val( attr, "placement" ) );
					r[ i ]._columnInfo[ columnNo ]._name = xml::attr_val( attr, "name" );
					r[ i ]._columnInfo[ columnNo ]._width = lexical_cast<int>( xml::attr_val( attr, "width" ) );
					HXml::HNode::properties_t const& col = (*attr).properties();
					HXml::HNode::properties_t::const_iterator alignIt = col.find( "align" );
					HXml::HNode::properties_t::const_iterator colTypeIt = col.find( "type" );
					M_ENSURE( ( alignIt != col.end() ) && ( colTypeIt != col.end() ) );
					if ( alignIt->second == "left" )
						r[ i ]._columnInfo[ columnNo ]._align = HControl::BITS::ALIGN::LEFT;
					else if ( alignIt->second == "center" )
						r[ i ]._columnInfo[ columnNo ]._align = HControl::BITS::ALIGN::CENTER;
					else if ( alignIt->second == "right" )
						r[ i ]._columnInfo[ columnNo ]._align = HControl::BITS::ALIGN::RIGHT;
					else
						M_THROW( _( "unknown align type" ), i );
					if ( colTypeIt->second == "string" )
						r[ i ]._columnInfo[ columnNo ]._type = TYPE::HSTRING;
					else
						M_THROW( _( "unknown column type" ), i );
					++ columnNo;
					}
				else if ( attrName == "checkable" )
					l->_checkable = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "sortable" )
					l->_sortable = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "searchable" )
					l->_searchable = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "editable" )
					l->_editable = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "draw_header" )
					l->_drawHeader = lexical_cast<bool>( xml::node_val( attr ) );
				else
					M_THROW( _( "unknown list attribute name" ), i );
				}
			else if ( r[ i ]._type == DATACONTROL_BITS::TYPE::EDIT )
				{
				OEditControlResource* e = static_cast<OEditControlResource*>( r[ i ]._typeSpecific );
				if ( attrName == "max_string_size" )
					e->_maxStringSize = lexical_cast<int>( xml::node_val( attr ) );
				else if ( attrName == "value" )
					e->_value = xml::node_val( attr );
				else if ( attrName == "mask" )
					e->_mask = xml::node_val( attr );
				else if ( attrName == "replace" )
					e->_replace = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "multi_line" )
					e->_multiLine = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "read_only" )
					e->_readOnly = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "right_aligned" )
					e->_rightAligned = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "password" )
					e->_password = lexical_cast<bool>( xml::node_val( attr ) );
				else if ( attrName == "max_history_level" )
					e->_maxHistoryLevel = lexical_cast<int>( xml::node_val( attr ) );
				else
					M_THROW( _( "unknown edit attribute name" ), i );
				}
			else
				M_THROW( _( "auto building of this type of control is not supported yet" ), i );
			}
		}
	for ( int n = 0; n < i; ++ n )
		{
		int_to_str_t::iterator child = i2s.find( n );
		if ( child != i2s.end() )
			{
			str_to_int_t::iterator parentIt = s2i.find( child->second );
			M_ENSURE( parentIt != s2i.end() );
			r[ n ]._parent = parentIt->second;
			r[ n ]._columnInfo = &r[ parentIt->second ]._columnInfo[ n - ( parentIt->second + 1 ) ];
			}
		}
	swap( _resourceCache[ resourceName_ ], r );
	return ( _resourceCache[ resourceName_ ] );
	M_EPILOG
	}

}

}


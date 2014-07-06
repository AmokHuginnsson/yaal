/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hdata/hdataprocess.hxx
 * \brief Declaration of HDataProcess class.
 */

#ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED
#define YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED 1

#include "hconsole/htuiprocess.hxx"
#include "hconsole/omenuitem.hxx"
#include "tools/hxml.hxx"
#include "dbwrapper/hdatabase.hxx"
#include "hdata/oresource.hxx"

namespace yaal {

namespace hdata {

#define M_REGISTER_MENU_HANDLER( handler ) do {	handlers[ #handler ] = static_cast<OMenuItem::HANDLER_t>( &this_type::handler ); } while ( 0 )

typedef yaal::hcore::HHashMap<yaal::hcore::HString,
				yaal::hconsole::OMenuItem::HANDLER_t> menu_handlers_map_t;

/*! \brief A backbone of database enabled TUI based application.
 *
 * HDataProcess enables rapid building of database connected TUI
 * applications with simple XML resource files.
 */
class HDataProcess : public hconsole::HTUIProcess {
public:
	typedef HDataProcess this_type;
	typedef HTUIProcess base_type;
protected:
	typedef yaal::hcore::HList<yaal::hcore::HString> param_cache_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, resources_t> resource_cache_t;
	typedef yaal::hcore::HList<OEditControlResource> edit_cache_t;
	typedef yaal::hcore::HList<OListControlResource> list_cache_t;
	typedef yaal::hcore::HList<yaal::hcore::HChunk> column_cache_t;
	typedef yaal::hconsole::HWindow::ptr_t ( *window_factory_t )( yaal::hcore::HString const&, HDataProcess*, resources_t* );
	dbwrapper::database_ptr_t _dataBase;
	menu_handlers_map_t _autoHandlers;
	yaal::tools::HXml _resource;
	resource_cache_t _resourceCache;
	column_cache_t _columnCache;
	edit_cache_t _editCache;
	list_cache_t _listCache;
private:
	yaal::hconsole::OMenuItem* _rootMenu;
public:
	HDataProcess( void );
	virtual ~HDataProcess( void );
	int init_xrc( char const*, char const*, menu_handlers_map_t const& );
/* cppcheck-suppress functionConst */
	dbwrapper::database_ptr_t data_base( void );
protected:
	virtual int do_handler_quit( int );
	virtual int do_handler_close_window( int );
private:
	yaal::hconsole::OMenuItem* build_sub_menu( yaal::tools::HXml::HConstNodeProxy const&,
			menu_handlers_map_t const& );
	void build_menu_item( yaal::tools::HXml::HConstNodeProxy const&,
			yaal::hconsole::OMenuItem&, menu_handlers_map_t const& );
	void destroy_menu( yaal::hconsole::OMenuItem* );
	resources_t* get_resource( yaal::hcore::HString const&, yaal::tools::HXml::HConstNodeProxy const& );
	resources_t& build_resource( yaal::hcore::HString const&, yaal::tools::HXml::HConstNodeProxy const& );
	int create_window( void* );
private:
	HDataProcess( HDataProcess const& );
	HDataProcess& operator = ( HDataProcess const& );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED */


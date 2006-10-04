/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HDATA_HDATAPROCESS_H
#define __YAAL_HDATA_HDATAPROCESS_H

#include "hconsole/htuiprocess.h"
#include "hconsole/omenuitem.h"
#include "tools/hxml.h"
#include "dbwrapper/hdatabase.h"

namespace yaal
{

namespace hdata
{

#define M_REGISTER_MENU_HANDLER( handler ) \
	{ \
	typedef typeof ( *this ) this_t; \
	l_oHandlers [ #handler ] = static_cast < OMenuItem::HANDLER_t > ( \
			& this_t::handler ); \
	}

typedef yaal::hcore::HHashMap < yaal::hcore::HString,
				yaal::hconsole::OMenuItem::HANDLER_t > menu_handlers_map_t;

class HDataProcess : public hconsole::HTUIProcess
	{
protected:
	dbwrapper::HDataBase f_oDataBase;
private:
	yaal::hconsole::OMenuItem * f_psRootMenu;
public:
	HDataProcess ( void );
	virtual ~HDataProcess ( void );
	virtual int init_xrc ( char const *, char const *,
			menu_handlers_map_t const & );
	dbwrapper::HDataBase * data_base ( void );
protected:
	virtual int handler_quit ( int, void * = NULL );
	virtual int handler_close_window ( int, void * = NULL );
	yaal::hconsole::OMenuItem * build_sub_menu ( yaal::tools::HXml::ONode &,
			menu_handlers_map_t const & );
	void build_menu_item ( yaal::tools::HXml::ONode &,
			yaal::hconsole::OMenuItem &, menu_handlers_map_t const & );
	void destroy_menu ( yaal::hconsole::OMenuItem * );
private:
	HDataProcess ( HDataProcess const & );
	HDataProcess & operator = ( HDataProcess const & );
	};

}

}

#endif /* not __YAAL_HDATA_HDATAPROCESS_H */

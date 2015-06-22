/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hconsole/hresource.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hresource.hxx
 * \brief Declaration of HResource class.
 */

#ifndef YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED
#define YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hpointer.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hmenuwidget.hxx"

namespace yaal {

namespace hconsole {

class HTUIProcess;

class HResource {
public:
	typedef HResource this_type;
	typedef yaal::hcore::HPointer<HResource> ptr_t;
private:
	HTUIProcess* _tui;
	yaal::tools::HXml _xml;
public:
	HResource( HTUIProcess*, yaal::hcore::HString const& );
	void load( void );
private:
	void build_sub_menu( yaal::tools::HXml::HConstNodeProxy const&, HMenuWidget::data_t::node_t );
	void build_menu_item( yaal::tools::HXml::HConstNodeProxy const&, HMenuWidget::data_t::node_t );
	void create_window( yaal::hcore::HString const& );
	HResource( HResource const& );
	HResource& operator = ( HResource const& );
};

typedef yaal::hcore::HExceptionT<HResource> HResourceException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED */


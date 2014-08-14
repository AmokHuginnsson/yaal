/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenuwidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED 1

#include "hconsole/htreewidget.hxx"
#include "hconsole/omenuitem.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Menu control class.
 *
 * Menu control displays menu and automatically spawns apropriate actions.
 */
class HMenuWidget : public HTreeWidget {
public:
	typedef HMenuWidget this_type;
	typedef HTreeWidget base_type;
	typedef yaal::hcore::HPointer<HMenuWidget> ptr_t;
protected:
	HTUIProcess* _process;
public:
	HMenuWidget( HWindow* parent,
			int row,
			int col,
			int height,
			int width,
			yaal::hcore::HString const& label );
	virtual ~HMenuWidget( void );
	void init( HTUIProcess*, OMenuItem* );
protected:
	int load_sub_menu( tree_t::node_t, OMenuItem* );
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
private:
	HMenuWidget( HMenuWidget const& );
	HMenuWidget& operator = ( HMenuWidget const& );
};

typedef yaal::hcore::HExceptionT<HMenuWidget, HTreeWidgetException> HMenuWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED */


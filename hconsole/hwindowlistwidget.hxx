/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindowlist.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED 1

#include "hconsole/hlistwidget.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

class HWindow;

/*! \brief Implementation of TUI Window List control class.
 *
 * Window List control displays list of currently opened windows,
 * and allows visual navigation for them.
 */
class HWindowListWidget : public HListWidget {
public:
	typedef HWindowListWidget this_type;
	typedef HListWidget base_type;
private:
	typedef yaal::hcore::HList<HWindow::ptr_t> model_t;
	model_t::cyclic_iterator& _foregroundWindow; /* self explanary */
public:
	HWindowListWidget( HWindow*, int, int, int, int, char const*,
			list_widget_helper::HAbstractControler::ptr_t const&, model_t::cyclic_iterator& );
	virtual int do_process_input( int );
};

typedef yaal::hcore::HExceptionT<HWindowListWidget, HListWidgetException> HWindowListWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED */


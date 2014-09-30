/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmainwindow.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hmainwindow.hxx
 * \brief Declaration of HMainWindow class.
 */

#ifndef YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED
#define YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED 1

#include "hconsole/hwindow.hxx"
#include "hconsole/hmenuwidget.hxx"
#include "hconsole/hwindowlistwidget.hxx"

namespace yaal {

namespace hconsole {

struct OMenuItem;

/*! \brief Main window of TUI application.
 *
 * Main window of TUI application consists of menu and active window list.
 */
class HMainWindow : public HWindow {
public:
	typedef HMainWindow this_type;
	typedef HWindow base_type;
private:
	HMenuWidget* _menu;
	HTUIProcess::model_ptr_t _windowList;
	HTUIProcess::model_t::cyclic_iterator& _foregroundWindow;
public:
	HMainWindow( yaal::hcore::HString const&, HTUIProcess::model_ptr_t, HTUIProcess::model_t::cyclic_iterator& );
	virtual ~HMainWindow( void );
	void set_menu( HMenuWidget::data_ptr_t );
protected:
	virtual void do_init( void );
	bool handler_close( HEvent const& ); /* closes window process */
private:
	HMainWindow( HMainWindow const& );
	HMainWindow& operator = ( HMainWindow const& );
};

typedef yaal::hcore::HExceptionT<HMainWindow, HWindowException> HMainWindowException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED */


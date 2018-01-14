/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
	HTUIProcess::cyclic_iterator& _foregroundWindow;
public:
	HMainWindow( yaal::hcore::HString const&, HTUIProcess::model_ptr_t, HTUIProcess::cyclic_iterator& );
	virtual ~HMainWindow( void );
	void set_menu( HMenuWidget::data_ptr_t );
protected:
	virtual void do_init( void ) override;
	bool handler_close( HEvent const& ); /* closes window process */
private:
	HMainWindow( HMainWindow const& ) = delete;
	HMainWindow& operator = ( HMainWindow const& ) = delete;
};

typedef yaal::hcore::HExceptionT<HMainWindow, HWindowException> HMainWindowException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HMAINWINDOW_HXX_INCLUDED */


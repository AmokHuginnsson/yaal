/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED 1

#include "hconsole/hlistwidget.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

class HWindow;

/*! \brief Implementation of TUI Window List widget class.
 *
 * Window List widget displays list of currently opened windows,
 * and allows visual navigation for them.
 */
class HWindowListWidget : public HListWidget {
public:
	typedef HWindowListWidget this_type;
	typedef HListWidget base_type;
private:
	typedef yaal::hcore::HList<HWindow::ptr_t> model_t;
	typedef hcore::HCyclicIterator<model_t> cyclic_iterator;
	cyclic_iterator& _foregroundWindow; /* self explanatory */
public:
	HWindowListWidget( HWindow*, int, int, int, int, char const*,
			list_widget_helper::HAbstractListModel::ptr_t const&, cyclic_iterator& );
	virtual int do_process_input( int );
};

typedef yaal::hcore::HExceptionT<HWindowListWidget, HListWidgetException> HWindowListWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWINDOWLISTWIDGET_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HWIDGETLIST_HXX_INCLUDED
#define YAAL_HCONSOLE_HWIDGETLIST_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hconsole {

class HWidget;

/*! \brief Keep list of TUI widgets for a given window.
 */
class HWidgetList {
public:
	typedef HWidgetList this_type;
	typedef hcore::HList<HWidget::ptr_t> model_t;
	typedef hcore::HCyclicIterator<model_t> cyclic_iterator;
	typedef yaal::hcore::HMap<int, yaal::hcore::HString> widget_order_t;
private:
	model_t _list;
	cyclic_iterator& _focused;
public:
	HWidgetList( cyclic_iterator& );
	/* find next enabled widget in window, if short cut char is specified */
	void next_enabled( char = 0 ); /* enabled and match shortcut char */
	void pop_front( void );
	void select( HWidget const* );	/* this one should be private :( */
	void select( HWidget::ptr_t const& );	/* this one should be private :( */
	void add_widget( HWidget::ptr_t );
	void refresh_all( bool );
	void update_all( void );
	void hit_test_all( mouse::OMouse& );
	HWidget* get_widget_by_no( int );
	void exchange( int, int );
	void reorder_widgets( widget_order_t const& );
	void sync_focus( void );
};

typedef yaal::hcore::HExceptionT<HWidgetList> HWidgetListException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWIDGETLIST_HXX_INCLUDED */


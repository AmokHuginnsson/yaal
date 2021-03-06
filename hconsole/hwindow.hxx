/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED
#define YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hconsole/hwidgetlist.hxx"
#include "hconsole/hwidget.hxx"
#include "hconsole/hhandler.hxx"
#include "hconsole/hstatusbarwidget.hxx"
#include "hconsole/hwindowfactory.hxx"

namespace yaal {

namespace hconsole {

class HWidgetList;
class HTUIProcess;

/*! \brief TUI Window implementation.
 */
class HWindow : public HHandler {
public:
	typedef HWindow this_type;
	typedef HHandler base_type;
	typedef yaal::hcore::HPointer<HWindow> ptr_t;
protected:
	bool _initialized;                                  /*!< was window properly initialized? */
	bool _needRepaint;                                  /*!< \brief Does this window need to be repainted? */
	hcore::HString _title;                              /*!< title of window */
	HWidgetList::cyclic_iterator _focusedChild;         /*!< points to widget that has focus */
	HWidgetList::cyclic_iterator _previousFocusedChild; /*!< widget that had focus before
	                                                         focus went to status bar */
	HWidgetList _widgets;                               /*!< list of all widget inside _this_ wind */
	HStatusBarWidget::ptr_t _statusBar;
	HTUIProcess* _tuiProcess;
public:
	HWindow( yaal::hcore::HString const& title );
	virtual ~HWindow( void );
	void init( void );
	void set_tui( HTUIProcess* );
	HTUIProcess* get_tui( void ) const;
	void paint( void );
	bool process_input( HKeyPressEvent const& );

	/*! \brief Jump through widgets with tab key.
	 */
	bool handler_jump_tab( HEvent const& );

	/*! \brief Direct jump to specified widget.
	 */
	bool handler_jump_direct( HEvent const& );

	/*! \brief Put window into command awaiting mode.
	 */
	bool handler_command( HEvent const& );

	/*! \brief Put window into search pattern scan mode.
	 */
	bool handler_search( HEvent const& );
	int click( mouse::OMouse& );
	void add_widget( HWidget::ptr_t );
	HStatusBarWidget::ptr_t& status_bar( void );
	hcore::HString get_command( void );
	bool is_initialized( void ) const;
	void update_all( void );
	yaal::hcore::HString const& get_title( void ) const;
	HStatusBarWidget* init_bar( yaal::hcore::HString const& );
	HWidget* get_focused_child( void ) const;

/*! \brief Schedule paint request on next refresh cycle.
 *
 * \param wholeWindow_ - Schedule repaint for all widgets in window.
 */
	void schedule_repaint( bool wholeWindow_ );
	void schedule_call( HTUIProcess::call_t );

	void reorder_widgets( HWidgetList::widget_order_t const& );
protected:
	virtual void do_init( void );
	virtual HStatusBarWidget* do_init_bar( yaal::hcore::HString const& );
private:
	friend bool HWidget::set_focus( int );
	friend void HStatusBarWidget::set_prompt(  yaal::hcore::HString const&,
			HStatusBarWidget::PROMPT );
	friend void HStatusBarWidget::end_prompt( void );
	friend int HStatusBarWidget::process_input_normal( int );
	void acquire_focus( HWidget const* );
	HWindow( HWindow const& );
	HWindow& operator = ( HWindow const& );
};

typedef yaal::hcore::HExceptionT<HWindow, HHandlerException> HWindowException;

class HWindowCreator : public HWindowCreatorInterface {
protected:
	virtual HWindow::ptr_t do_new_instance( HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& );
	void create_widgets( HWindow::ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_apply_resources( HTUIProcess*, window_ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
};

class HWindowEvent : public HEvent {
public:
	typedef HWindowEvent this_type;
	typedef HEvent base_type;
private:
	HWindow* _window;
public:
	HWindowEvent( HWindow* window_ )
		: _window( window_ ) {
	}
	HWindowEvent( HWindowEvent const& ) = default;
	HWindowEvent& operator = ( HWindowEvent const& ) = default;
	HWindow* get_source( void ) const {
		return ( _window );
	}
private:
	virtual TYPE do_get_type( void ) const {
		return ( TYPE::WINDOW );
	}
};

template<typename widget_t, typename window_t, typename... args_t>
widget_t* create_widget( window_t* window_, args_t&&... args_ ) {
	HWidget::ptr_t w( yaal::hcore::make_pointer<widget_t>( window_, yaal::forward<args_t>( args_ )... ) );
	window_->add_widget( w );
	return ( dynamic_cast<widget_t*>( w.raw() ) );
}

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwindow.hxx"
#include "console.hxx"
#include "htuiprocess.hxx"
#include "hcore/hlog.hxx"
#include "hwidgetfactory.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HWindow::HWindow( yaal::hcore::HString const& title_ )
	: _initialized( false )
	, _needRepaint( false )
	, _title( title_ )
	, _focusedChild()
	, _previousFocusedChild()
	, _widgets( _focusedChild )
	, _statusBar()
	, _tuiProcess( nullptr ) {
	M_PROLOG
	int cmds [] = { ':', KEY<':'>::command };
	int search [] = { '/', KEY<'/'>::command, '?', KEY<'?'>::command };
	if ( ! HConsole::get_instance().is_enabled() ) {
		M_THROW( "console not initialized.", errno );
	}
	register_postprocess_handler( '\t', nullptr, call( &HWindow::handler_jump_tab, this, _1 ) );
	register_postprocess_handler( 2, cmds, call( &HWindow::handler_command, this, _1 ) );
	register_postprocess_handler( 4, search, call( &HWindow::handler_search, this, _1 ) );
	return;
	M_EPILOG
}

HWindow::~HWindow( void ) {
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroying window: " << _title << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
}

void HWindow::init( void ) {
	M_PROLOG
	do_init();
	_widgets.sync_focus();
	return;
	M_EPILOG
}

void HWindow::do_init( void ) {
	M_PROLOG
	M_ENSURE( _tuiProcess );
	HString string;
	if ( _tuiProcess ) {
		_tuiProcess->schedule_repaint();
	}
	_statusBar = init_bar( format( " [%s]& \n", _title ) )->get_pointer();
	_statusBar->enable( true );
	_initialized = true;
	return;
	M_EPILOG
}

HStatusBarWidget* HWindow::init_bar( yaal::hcore::HString const& label_ ) {
	M_PROLOG
	return ( do_init_bar( label_ ) );
	M_EPILOG
}

HStatusBarWidget* HWindow::do_init_bar( yaal::hcore::HString const& label_ ) {
	M_PROLOG
	return ( create_widget<HStatusBarWidget>( this, label_ ) );
	M_EPILOG
}

void HWindow::set_tui( yaal::hconsole::HTUIProcess* tui_ ) {
	M_PROLOG
	M_ENSURE( !_tuiProcess );
	_tuiProcess = tui_;
	return;
	M_EPILOG
}

HTUIProcess* HWindow::get_tui( void ) const {
	return ( _tuiProcess );
}

bool HWindow::process_input( HKeyPressEvent const& keyPress_ ) {
	M_PROLOG
	bool consumed( false );
	if ( keyPress_.get_key_code() ) {
		consumed = process_input_with_handlers( keyPress_, _preprocessHandlers );
	}
	if ( ! consumed && !! (*_focusedChild) ) {
		consumed = (*_focusedChild)->process_input( keyPress_.get_key_code() ) ? false : true;
	}
	if ( ! consumed ) {
		consumed = process_input_with_handlers( keyPress_, _postprocessHandlers );
	}
	if ( ! _command.is_empty() ) {
		process_command();
		consumed = _command.is_empty();
	}
	return consumed;
	M_EPILOG
}

void HWindow::add_widget( HWidget::ptr_t widget_ ) {
	M_PROLOG
	int shortCut( KEY<>::meta_r( widget_->get_shortcut() ) );
	if ( _preprocessHandlers.find( shortCut ) != _preprocessHandlers.end() ) {
		M_THROW( _( "shortcut occupied" ), shortCut );
	}
	_widgets.add_widget( widget_ );
	register_postprocess_handler( shortCut, nullptr, call( &HWindow::handler_jump_direct, this, _1 ) );
	return;
	M_EPILOG
}

HStatusBarWidget::ptr_t& HWindow::status_bar( void ) {
	return ( _statusBar );
}

void HWindow::paint( void ) {
	M_PROLOG
	if ( _needRepaint ) {
		HConsole::get_instance().clear_terminal();
	}
	if ( ( !! _statusBar ) && ( _statusBar != *_focusedChild ) ) {
		_statusBar->paint();
	}
	_widgets.refresh_all( _needRepaint );
	_needRepaint = false;
	return;
	M_EPILOG
}

bool HWindow::handler_jump_tab( HEvent const& ) {
	M_PROLOG
	_widgets.next_enabled();
	_tuiProcess->schedule_repaint();
	return ( true );
	M_EPILOG
}

bool HWindow::handler_jump_direct( HEvent const& event_ ) {
	M_PROLOG
	M_ASSERT( event_.get_type() == HEvent::TYPE::KEY_PRESS );
	bool consumed( false );
	if ( *_focusedChild != _statusBar ) {
		HKeyPressEvent const& keyPress( static_cast<HKeyPressEvent const&>( event_ ) );
		/*
		 * call below is _magic_, HWidgetList::next_enabled() takes char as an
		 * argument, so code_ & 0x0ff is passed into the function,
		 * code_ is constructed from ordinary char by KEY_META_ macro,
		 * see console.h for details
		 */
		HWidgetList::cyclic_iterator it = _focusedChild;
		code_point_t::value_type keyCode( static_cast<code_point_t::value_type>( keyPress.get_key_code() ) );
		if ( ( keyCode >= KEY_CODE::META_BASE ) && ( keyCode < KEY_CODE::COMMAND_BASE ) ) {
			_widgets.next_enabled( static_cast<char>( keyCode ) );
			if ( _focusedChild != it ) {
				consumed = true;
			}
			_tuiProcess->schedule_repaint();
		}
	}
	return consumed;
	M_EPILOG
}

void HWindow::acquire_focus( HWidget const* widget_ ) {
	M_PROLOG
	if ( (*_focusedChild) != widget_ ) {
		(*_focusedChild)->kill_focus();
		_widgets.select( widget_ );
		_tuiProcess->schedule_repaint();
	}
	return;
	M_EPILOG
}

HWidget* HWindow::get_focused_child( void ) const {
	return ( _focusedChild->get() );
}

bool HWindow::handler_command( HEvent const& ) {
	M_PROLOG
	_statusBar->set_prompt( ":", HStatusBarWidget::PROMPT::COMMAND );
	return ( true );
	M_EPILOG
}

bool HWindow::handler_search( HEvent const& event_ ) {
	M_PROLOG
	M_ASSERT( event_.get_type() == HEvent::TYPE::KEY_PRESS );
	HKeyPressEvent const& keyPress( static_cast<HKeyPressEvent const&>( event_ ) );
	int code( keyPress.get_key_code() );
	char prompt[] = "/\0";
	bool consumed( false );
	if ( (*_focusedChild)->is_searchable() ) {
		if ( code >= KEY_CODE::COMMAND_BASE )
			code -= KEY_CODE::COMMAND_BASE;
		prompt [ 0 ] = static_cast<char>( code );
		_statusBar->set_prompt ( prompt, HStatusBarWidget::PROMPT::SEARCH );
		consumed = true;
	}
	return consumed;
	M_EPILOG
}

int HWindow::click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	int ret( 0 );
	if ( ! mouse_._buttons ) {
		ret = 1;
	} else if ( (*_focusedChild) == _statusBar ) {
		ret = 1;
	} else {
		_widgets.hit_test_all( mouse_ );
	}
	return ret;
	M_EPILOG
}

HString HWindow::get_command( void ) {
	M_PROLOG
	HString command;
	command = _command;
	_command = "";
	return command;
	M_EPILOG
}

bool HWindow::is_initialized( void ) const {
	return ( _initialized );
}

void HWindow::update_all( void ) {
	M_PROLOG
	_widgets.update_all();
	return;
	M_EPILOG
}

HString const& HWindow::get_title( void ) const {
	M_PROLOG
	return ( _title );
	M_EPILOG
}

void HWindow::schedule_repaint( bool wholeWindow_ ) {
	M_PROLOG
	if ( wholeWindow_ ) {
		_needRepaint = true;
	}
	if ( _tuiProcess ) {
		_tuiProcess->schedule_repaint();
	}
	return;
	M_EPILOG
}

void HWindow::schedule_call( HTUIProcess::call_t call_ ) {
	M_PROLOG
	if ( _tuiProcess ) {
		_tuiProcess->schedule_call( call_ );
	}
	return;
	M_EPILOG
}

void HWindow::reorder_widgets( HWidgetList::widget_order_t const& widgetOrder_ ) {
	M_PROLOG
	_widgets.reorder_widgets( widgetOrder_ );
	return;
	M_EPILOG
}

HWindow::ptr_t HWindowCreator::do_new_instance( HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HString name( xml::attr_val( node_, "title" ) );
	HWindow::ptr_t window( make_pointer<HWindow>( name ) );
	return window;
	M_EPILOG
}

void HWindowCreator::create_widgets( HWindow::ptr_t window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HWidgetFactory& wf( HWidgetFactory::get_instance() );
	int pos( 0 );
	HWidgetList::widget_order_t widgetOrder;
	for ( yaal::tools::HXml::HConstNodeProxy const& n : node_ ) {
		HString type( n.get_name() );
		if ( wf.is_type_valid( type ) ) {
			xml::value_t id( xml::try_attr_val( n, "id" ) );
			if ( ! id ) {
				wf.create_widget( window_.raw(), n );
			} else {
				widgetOrder[pos] = *id;
			}
			++ pos;
		}
	}
	window_->reorder_widgets( widgetOrder );
	return;
	M_EPILOG
}

bool HWindowCreator::do_apply_resources( HTUIProcess*, window_ptr_t window, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	create_widgets( window, node_ );
	return ( true );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWindowFactory::get_instance().register_window_creator( "window", HWindowCreatorInterface::ptr_t( new ( memory::yaal ) HWindowCreator() ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


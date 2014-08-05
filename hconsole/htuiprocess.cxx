/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htuiprocess.hxx"
#include "hcore/memory.hxx"
#include "hconsole.hxx"
#include "hmainwindow.hxx"
#include "mouse.hxx"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HTUIProcess::HTUIProcess( int noFileHandlers_, int keyHandlers_,
		int commandHandlers_ )
	: HHandler( keyHandlers_, commandHandlers_ ),
	_dispatcher( noFileHandlers_, _latency_ * 1000 ), _mainWindow(), _foregroundWindow(),
	_windows( new ( memory::yaal ) model_t() ), _needRepaint( false ), _callQueue() {
	M_PROLOG
	return;
	M_EPILOG
}

HTUIProcess::~HTUIProcess( void ) {
	M_PROLOG
	if ( _windows->size() > 0 )
		_windows->pop_front();
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
}

int HTUIProcess::init_tui( char const* processName_, HWindow::ptr_t mainWindow_ ) {
	M_PROLOG
	static int const CTRLS_COUNT( 2 );
	static int const ALTS_COUNT( 10 );
	int alts[ ALTS_COUNT ];
	int ctrls[] = { KEY<'l'>::ctrl, KEY<'x'>::ctrl };
	HWindow::ptr_t mainWindow;
	_dispatcher.register_file_descriptor_handler( STDIN_FILENO, call( &HTUIProcess::process_stdin, this, _1 ) );
	HConsole& cons = HConsole::get_instance();
	int mouseDes( cons.get_mouse_fd() );
	if ( _useMouse_ && ( mouseDes > 0 ) )
		_dispatcher.register_file_descriptor_handler( mouseDes, call( &HTUIProcess::process_mouse, this, _1 ) );
	_dispatcher.register_file_descriptor_handler( cons.get_event_fd(), call( &HTUIProcess::process_terminal_event, this, _1 ) );
	_dispatcher.add_alert_handle( call( &HTUIProcess::handler_alert, this ) );
	_dispatcher.add_idle_handle( call( &HTUIProcess::handler_idle, this ) );
	register_postprocess_handler( CTRLS_COUNT, ctrls, call( &HTUIProcess::handler_refresh, this, _1 ) );
	register_postprocess_handler( KEY<'x'>::command, NULL, call( &HTUIProcess::handler_quit, this, _1 ) );
	if ( _useMouse_ ) {
		register_postprocess_handler( KEY_CODES::MOUSE, NULL, call( &HTUIProcess::handler_mouse, this, _1 ) );
	}
	if ( !! mainWindow_ )
		mainWindow = mainWindow_;
	else /* Create automatically default main window. */ {
		mainWindow = make_pointer<HMainWindow>( processName_, _windows, ref( _foregroundWindow ) );
		register_postprocess_handler( KEY<'\t'>::meta, NULL, call( &HTUIProcess::handler_jump_meta_tab, this, _1 ) );
		register_postprocess_handler( KEY<'q'>::command, NULL, call( &HTUIProcess::handler_close_window, this, _1 ) );
		for ( int ctr( 0 ); ctr < ALTS_COUNT; ++ ctr )
			alts[ ctr ] = KEY<>::meta_r( '0' + ctr );
		register_postprocess_handler( ALTS_COUNT, alts, call( &HTUIProcess::handler_jump_meta_direct, this, _1 ) );
	}
	_mainWindow = mainWindow;
	add_window( mainWindow );
	if ( ! mainWindow->is_initialised() )
		M_THROW( _( "window has not been initialised" ), errno );
	_foregroundWindow = _windows->begin();
	_commandHandlers[ "quit" ] = call( &HTUIProcess::handler_quit, this, _1 );
	repaint();
	return ( 1 );
	M_EPILOG
}

void HTUIProcess::run( void ) {
	M_PROLOG
	_dispatcher.run();
	return;
	M_EPILOG
}

void HTUIProcess::schedule_repaint( void ) {
	_needRepaint = true;
	return;
}

void HTUIProcess::schedule_call( call_t call_ ) {
	M_PROLOG
	_callQueue.push( call_ );
	return;
	M_EPILOG
}

void HTUIProcess::flush_call_queue( void ) {
	M_PROLOG
	while ( ! _callQueue.is_empty() ) {
		_callQueue.front()();
		_callQueue.pop();
	}
	return;
	M_EPILOG
}

void HTUIProcess::add_window( HWindow::ptr_t window_ ) {
	M_PROLOG
	window_->set_tui( this );
	window_->init();
	_windows->push_back( window_ );
	_foregroundWindow = _windows->rbegin().base();
	M_ASSERT( _foregroundWindow != _windows->end() );
	_mainWindow->update_all();
	if ( ! (*_foregroundWindow)->is_initialised() )
		M_THROW( _( "window has not been initialised" ), errno );
	repaint( true );
	return;
	M_EPILOG
}

void HTUIProcess::process_stdin( int ) {
	M_PROLOG
	HString command;
	HConsole& cons = HConsole::get_instance();
	int code( cons.get_key() );
	bool consumed( false );
	if ( code ) {
		consumed = process_input_with_handlers( code, _preprocessHandlers );
	}
	if ( !consumed && !! (*_foregroundWindow) ) {
		consumed = (*_foregroundWindow)->process_input( code );
	}
	if ( !consumed ) {
		consumed = process_input_with_handlers( code, _postprocessHandlers );
	}
	if ( !consumed ) {
		if ( !! (*_foregroundWindow) )
			_command = (*_foregroundWindow)->get_command();
		if ( ! _command.is_empty() )
			command = process_command();
		if ( ! command.is_empty()
				&& !! (*_foregroundWindow) )
			(*_foregroundWindow)->status_bar()->message( COLORS::FG_RED,
					"unknown command: `%s'", command.raw() );
	}
	flush_call_queue();
	if ( _needRepaint )
		repaint();
#ifdef __DEBUGGER_BABUNI__
	_needRepaint = true;
	if ( code ) {
		if ( code > KEY<KEY<0>::meta>::command )
			cons.cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"COMMAND-META-%c: %5d       ",
					code - KEY<KEY<0>::meta>::command, code );
		else if ( code > KEY<0>::command )
			cons.cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"     COMMAND-%c: %5d       ",
					code - KEY<0>::command, code );
		else if ( code > KEY<0>::meta )
			cons.cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"        META-%c: %5d       ",
					code - KEY<0>::meta, code );
		else if ( code < KEY_CODES::ESC )
			cons.cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"        CTRL-%c: %5d       ",
					code + 96, code);
		else
			cons.cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"             %c: %5d       ",
					code, code );
	} else
		cons.cmvprintf ( 0, 0, COLORS::FG_GREEN, "                           " );
#endif /* __DEBUGGER_BABUNI__ */
	if ( ! consumed && !! (*_foregroundWindow) )
		(*_foregroundWindow)->status_bar()->message( COLORS::FG_RED,
				"unknown function, err code(%d)", code );
	return;
	M_EPILOG
}

void HTUIProcess::handler_alert( void ) {
	M_PROLOG
	if ( _needRepaint ) {
		_needRepaint = false;
		HConsole::get_instance().refresh();
	}
	return;
	M_EPILOG
}

void HTUIProcess::handler_idle( void ) {
	M_PROLOG
#ifdef __DEBUG__
	HConsole& cons = HConsole::get_instance();
	HString clock( HTime( HTime::LOCAL ).string() );
	cons.cmvprintf( 0, static_cast<int>( cons.get_width() - clock.get_length() ),
			COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY, clock.raw() );
	_needRepaint = true;
#endif /* __DEBUG__ */
	if ( !! (*_foregroundWindow) ) {
		HStatusBarWidget::ptr_t& statusBar = (*_foregroundWindow)->status_bar();
		if ( !! statusBar )
			statusBar->paint();
	}
	return;
	M_EPILOG
}

void HTUIProcess::process_mouse( int ) {
	M_PROLOG
	handler_mouse( HMouseEvent( mouse::OMouse() ) );
	return;
	M_EPILOG
}

bool HTUIProcess::handler_mouse( HEvent const& ) {
	M_PROLOG
	mouse::OMouse mouse;
	static_cast<void>( mouse::mouse_get( mouse ) );
	if ( !! (*_foregroundWindow) )
		(*_foregroundWindow)->click( mouse );
	if ( _needRepaint )
		repaint();
#ifdef __DEBUGGER_BABUNI__
	HConsole::get_instance().cmvprintf( 0, 0,	COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY, "mouse: %6d, %3d, %3d",
			mouse._buttons, mouse._row, mouse._column );
	_needRepaint = true;
#endif /* __DEBUGGER_BABUNI__ */
	return ( true );
	M_EPILOG
}

void HTUIProcess::process_terminal_event( int event_ ) {
	M_PROLOG
	char type;
	M_ENSURE( ::read( event_, &type, 1 ) == 1 );
	switch( type ) {
		case 'r':
			handler_refresh( HKeyPressEvent( 'r' ) );
		break;
		case 'm':
			process_mouse( 0 );
		break;
	}
	return;
	M_EPILOG
}

bool HTUIProcess::handler_refresh( HEvent const& ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	cons.endwin();
	cons.kbhit(); /* cleans all trash from stdio buffer */
	cons.refresh();
	repaint( true ); /* there is c_clrscr(); and repaint() call inside */
	return ( true );
	M_EPILOG
}

bool HTUIProcess::handler_quit( HEvent const& ) {
	M_PROLOG
	quit();
	return ( true );
	M_EPILOG
}

void HTUIProcess::quit( void ) {
	M_PROLOG
	do_quit();
	return;
	M_EPILOG
}

void HTUIProcess::do_quit( void ) {
	M_PROLOG
	_dispatcher.stop();
	_needRepaint = false;
	HConsole::get_instance().clrscr();
	return;
	M_EPILOG
}

bool HTUIProcess::handler_jump_meta_tab( HEvent const& ) {
	M_PROLOG
	if ( _dispatcher.idle_cycles() < 5 )
		++ _foregroundWindow;
	else
		_foregroundWindow = _windows->begin();
	repaint( true );
	return ( true );
	M_EPILOG
}

bool HTUIProcess::handler_jump_meta_direct( HEvent const& keyPress_ ) {
	M_PROLOG
	M_ASSERT( keyPress_.get_type() == HEvent::TYPE::KEY_PRESS );
	HKeyPressEvent const& keyPress( static_cast<HKeyPressEvent const&>( keyPress_ ) );
	int code( ( keyPress.get_key_code() & 0xff ) - '0' );
	if ( code >= _windows->size() )
		return ( 0 );
	_foregroundWindow = _windows->begin();
	while ( code -- )
		++ _foregroundWindow;
	repaint( true );
	return ( true );
	M_EPILOG
}

bool HTUIProcess::handler_close_window( HEvent const& ) {
	M_PROLOG
	close_window();
	return ( true );
	M_EPILOG
}

void HTUIProcess::close_window( void ) {
	M_PROLOG
	do_close_window();
	return;
	M_EPILOG
}

void HTUIProcess::do_close_window( void ) {
	M_PROLOG
	model_t::cyclic_iterator it = _foregroundWindow;
	-- _foregroundWindow;
	_windows->erase( it );
	repaint( true );
	return;
	M_EPILOG
}

void HTUIProcess::select( HWindow const* const window_ ) {
	M_PROLOG
	for ( model_t::iterator it( _windows->begin() ), end( _windows->end() ); it != end; ++ it ) {
		if ( (*it) == window_ ) {
			_foregroundWindow = it;
			break;
		}
	}
	return;
	M_EPILOG
}

void HTUIProcess::repaint( bool force_ ) {
	M_PROLOG
	if ( ( _foregroundWindow != HTUIProcess::model_t::cyclic_iterator() ) && ( !! (*_foregroundWindow) ) ) {
		if ( force_ ) {
			(*_foregroundWindow)->schedule_repaint( true );
		}
		(*_foregroundWindow)->paint();
	}
	HConsole::get_instance().refresh();
	_needRepaint = false;
	return;
	M_EPILOG
}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "htuiprocess.hxx"
#include "hconsole.hxx"
#include "hmainwindow.hxx"
#include "mouse.hxx"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HTUIProcess::HTUIProcess( int noFileHandlers_, size_t keyHandlers_,
		size_t commandHandlers_ )
	: HHandler( keyHandlers_, commandHandlers_ ),
	_dispatcher( noFileHandlers_, _latency_ ), _mainWindow(), _foregroundWindow(),
	_windows( new model_t() )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HTUIProcess::~HTUIProcess( void )
	{
	M_PROLOG
	if ( _windows->size() > 0 )
		_windows->pop_front();
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HTUIProcess::init_tui( char const* processName_, HWindow::ptr_t mainWindow_ )
	{
	M_PROLOG
	static int const CTRLS_COUNT = 2;
	static int const ALTS_COUNT = 10;
	int ctr = 0;
	int alts[ ALTS_COUNT ];
	int ctrls[] = { KEY<'l'>::ctrl, KEY<'x'>::ctrl };
	HWindow::ptr_t mainWindow;
	_dispatcher.register_file_descriptor_handler( STDIN_FILENO, call( &HTUIProcess::process_stdin, this, _1 ) );
	HConsole& cons = HConsole::get_instance();
	int mouseDes = cons.get_mouse_fd();
	if ( _useMouse_ && mouseDes )
		_dispatcher.register_file_descriptor_handler( mouseDes, call( &HTUIProcess::process_mouse, this, _1 ) );
	_dispatcher.register_file_descriptor_handler( cons.get_event_fd(), call( &HTUIProcess::process_terminal_event, this, _1 ) );
	_dispatcher.add_alert_handle( call( &HTUIProcess::handler_alert, this ) );
	_dispatcher.add_idle_handle( call( &HTUIProcess::handler_idle, this ) );
	register_postprocess_handler( CTRLS_COUNT, ctrls,
			& HTUIProcess::handler_refresh );
	register_postprocess_handler( KEY<'x'>::command, NULL,
			& HTUIProcess::handler_quit );
	if ( _useMouse_ )
		register_postprocess_handler( KEY_CODES::MOUSE, NULL,
				& HTUIProcess::handler_mouse );
	if ( !! mainWindow_ )
		mainWindow = mainWindow_;
	else /* Create automatically default main window. */
		{
		mainWindow = HWindow::ptr_t( new HMainWindow( processName_, _windows, _foregroundWindow ) );
		register_postprocess_handler( KEY<'\t'>::meta, NULL,
				&HTUIProcess::handler_jump_meta_tab );
		register_postprocess_handler( KEY<'q'>::command, NULL,
				&HTUIProcess::handler_close_window );
		for ( ctr = 0; ctr < ALTS_COUNT; ctr ++ )
			alts[ ctr ] = KEY<>::meta_r( '0' + ctr );
		register_postprocess_handler( ALTS_COUNT, alts,
			&HTUIProcess::handler_jump_meta_direct );
		}
	_mainWindow = mainWindow;
	add_window( mainWindow );
	if ( ! mainWindow->is_initialised() )
		M_THROW( _( "window has not been initialised" ), errno );
	_foregroundWindow = _windows->begin();
	_commandHandlers[ "quit" ] = static_cast<HHandler::HANDLER_t>( &HTUIProcess::handler_quit );
	refresh();
	return ( 1 );
	M_EPILOG
	}

int HTUIProcess::run( void )
	{
	M_PROLOG
	return ( _dispatcher.run() );
	M_EPILOG
	}

int HTUIProcess::add_window( HWindow::ptr_t window_ )
	{
	M_PROLOG
	window_->init();
	_windows->push_back( window_ );
	_foregroundWindow = _windows->rbegin();
	M_ASSERT( _foregroundWindow.is_valid() );
	_mainWindow->update_all();
	if ( ! (*_foregroundWindow)->is_initialised() )
		M_THROW( _( "window has not been initialised" ), errno );
	refresh( true );
	return ( 0 );
	M_EPILOG
	}

void HTUIProcess::process_stdin( int code_ )
	{
	M_PROLOG
	HString command;
	HConsole& cons = HConsole::get_instance();
	if ( ! code_ )
		code_ = cons.get_key();
	if ( code_ )
		code_ = process_input_with_handlers( code_, _preprocessHandlers );
	if ( code_ && !! (*_foregroundWindow) )
			code_ = (*_foregroundWindow)->process_input( code_ );
	if ( code_ )
		code_ = process_input_with_handlers( code_, _postprocessHandlers );
	if ( ! code_ )
		{
		if ( !! (*_foregroundWindow) )
			_command = (*_foregroundWindow)->get_command();
		if ( ! _command.is_empty() )
			command = process_command();
		if ( ! command.is_empty()
				&& !! (*_foregroundWindow) )
			(*_foregroundWindow)->status_bar()->message( COLORS::FG_RED,
					"unknown command: `%s'", command.raw() );
		}
	if ( _needRepaint_ )
		refresh();
#ifdef __DEBUGGER_BABUNI__
	_needRepaint_ = true;
	if ( code_ )
		{
		if ( code_ > KEY<KEY<0>::meta>::command )
			cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"COMMAND-META-%c: %5d       ",
					code_ - KEY<KEY<0>::meta>::command, code_ );
		else if ( code_ > KEY<0>::command )
			cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"     COMMAND-%c: %5d       ",
					code_ - KEY<0>::command, code_ );
		else if ( code_ > KEY<0>::meta )
			cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"        META-%c: %5d       ",
					code_ - KEY<0>::meta, code_ );
		else if ( code_ < KEY_CODES::ESC )
			cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"        CTRL-%c: %5d       ",
					code_ + 96, code_);
		else
			cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN,
					"             %c: %5d       ",
					code_, code_ );
		}
	else
		cons.c_cmvprintf ( 0, 0, COLORS::FG_GREEN, "                           " );
#endif /* __DEBUGGER_BABUNI__ */
	if ( code_ && !! (*_foregroundWindow) )
		(*_foregroundWindow)->status_bar()->message ( COLORS::FG_RED,
				"unknown function, err code(%d)", code_ );
	return;
	M_EPILOG
	}

void HTUIProcess::handler_alert( void )
	{
	M_PROLOG
	if ( _needRepaint_ )
		{
		_needRepaint_ = false;
		HConsole::get_instance().c_refresh();
		}
	return;
	M_EPILOG
	}

void HTUIProcess::handler_idle( void )
	{
	M_PROLOG
#ifdef __DEBUG__
	HConsole& cons = HConsole::get_instance();
	HString clock( static_cast<char const *>( HTime() ) );
	cons.c_cmvprintf( 0, static_cast<int>( cons.get_width() - clock.get_length() ),
			COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY, clock.raw() );
	_needRepaint_ = true;
#endif /* __DEBUG__ */
	if ( !! (*_foregroundWindow) )
		{
		HStatusBarControl::ptr_t& statusBar = (*_foregroundWindow)->status_bar();
		if ( !! statusBar )
			statusBar->refresh();
		}
	return;
	M_EPILOG
	}

void HTUIProcess::process_mouse( int )
	{
	M_PROLOG
	handler_mouse( 0 );
	return;
	M_EPILOG
	}

int HTUIProcess::handler_mouse( int code_, void const* )
	{
	M_PROLOG
	code_ = 0;
	mouse::OMouse mouse;
	static_cast<void>( mouse::mouse_get( mouse ) );
	if ( !! (*_foregroundWindow) )
		(*_foregroundWindow)->click( mouse );
	if ( _needRepaint_ )
		refresh();
#ifdef __DEBUGGER_BABUNI__
	HConsole::get_instance().c_cmvprintf( 0, 0,	COLORS::FG_BLACK | COLORS::BG_LIGHTGRAY, "mouse: %6d, %3d, %3d",
			mouse._buttons, mouse._row, mouse._column );
	_needRepaint_ = true;
#endif /* __DEBUGGER_BABUNI__ */
	return ( code_ );
	M_EPILOG
	}

void HTUIProcess::process_terminal_event( int event_ )
	{
	M_PROLOG
	char type;
	::read( event_, &type, 1 );
	switch( type )
		{
		case 'r':
			handler_refresh( 0 );
		break;
		case 'm':
			process_mouse( 0 );
		break;
		}
	return;
	M_EPILOG
	}

int HTUIProcess::handler_refresh( int, void const* )
	{
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	cons.endwin();
	cons.kbhit(); /* cleans all trash from stdio buffer */
	cons.c_refresh();
	cons.c_getmaxyx();
	refresh( true ); /* there is c_clrscr(); and c_refresh() call inside */
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_quit( int, void const* )
	{
	M_PROLOG
	_dispatcher.stop();
	_needRepaint_ = false;
	HConsole::get_instance().clrscr();
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_jump_meta_tab( int code_, void const* )
	{
	M_PROLOG
	if ( _dispatcher.idle_cycles() < 5 )
		++ _foregroundWindow;
	else
		_foregroundWindow = _windows->begin();
	refresh( true );
	code_ = 0;
	return ( code_ );
	M_EPILOG
	}

int HTUIProcess::handler_jump_meta_direct( int code_, void const* )
	{
	M_PROLOG
	code_ = ( code_ & 0xff ) - '0';
	if ( code_ >= _windows->size() )
		return ( 0 );
	_foregroundWindow = _windows->begin();
	while ( code_ -- )
		++ _foregroundWindow;
	refresh( true );
	code_ = 0;
	return ( code_ );
	M_EPILOG
	}

int HTUIProcess::handler_close_window( int code_, void const* )
	{
	M_PROLOG
	model_t::cyclic_iterator it = _foregroundWindow;
	-- _foregroundWindow;
	_windows->erase( it );
	refresh( true );
	code_ = 0;
	return ( code_ );
	M_EPILOG
	}

void HTUIProcess::select( HWindow const* const window_ )
	{
	M_PROLOG
	for ( model_t::iterator it = _windows->begin(); it != _windows->end(); ++ it )
		{
		if ( (*it) == window_ )
			{
			_foregroundWindow = it;
			break;
			}
		}
	return;
	M_EPILOG
	}

void HTUIProcess::refresh( bool force_ )
	{
	M_PROLOG
	if ( _foregroundWindow.is_valid() && ( !! (*_foregroundWindow) ) )
		{
		if ( force_ )
			(*_foregroundWindow)->schedule_refresh();
		(*_foregroundWindow)->refresh();
		}
	HConsole::get_instance().c_refresh();
	_needRepaint_ = false;
	return;
	M_EPILOG
	}

}

}


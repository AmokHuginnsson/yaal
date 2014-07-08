/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindow.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwindow.hxx"
#include "console.hxx"
#include "htuiprocess.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HWindow::HWindow( char const * title_ ) : _initialised( false ),
	_needRepaint( false ), _title( title_ ), _focusedChild(),
	_previousFocusedChild(), _controls( _focusedChild ),
	_statusBar(), _tuiProcess( NULL ) {
	M_PROLOG
	int cmds [] = { ':', KEY<':'>::command };
	int search [] = { '/', KEY<'/'>::command, '?', KEY<'?'>::command };
	if ( ! HConsole::get_instance().is_enabled() )
		M_THROW( "console not initialised.", errno );
	register_postprocess_handler( '\t', NULL, call( &HWindow::handler_jump_tab, this, _1 ) );
	register_postprocess_handler( 2, cmds, call( &HWindow::handler_command, this, _1 ) );
	register_postprocess_handler( 4, search, call( &HWindow::handler_search, this, _1 ) );
	return;
	M_EPILOG
}

HWindow::~HWindow( void ) {
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing window: " << _title << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
}

int HWindow::init( void ) {
	M_PROLOG
	M_ENSURE( _tuiProcess );
	HString string;
	_tuiProcess->schedule_repaint();
	string.format( " [%s]& \n", _title.raw() );
	new HStatusBarControl( this, string.raw() );
	/*
	 * After line above window has 1 (one) control which is HStatusBarControl.
	 * This only control has a focus.
	 * */
	_statusBar = *_focusedChild;
	_statusBar->enable( true );
	_initialised = true;
	return ( 0 );
	M_EPILOG
}

void HWindow::set_tui( yaal::hconsole::HTUIProcess* tui_ ) {
	M_PROLOG
	M_ENSURE( !_tuiProcess );
	_tuiProcess = tui_;
	return;
	M_EPILOG
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
	if ( ! _command.is_empty() )
		process_command();
	return ( consumed );
	M_EPILOG
}

int HWindow::add_control( HControl::ptr_t control_, int shortCut_ ) {
	M_PROLOG
	if ( _preprocessHandlers.find( shortCut_ ) != _preprocessHandlers.end() )
		M_THROW( _( "shortcut occupied" ), shortCut_ );
	_controls.add_control( control_ );
	register_postprocess_handler( shortCut_, NULL, call( &HWindow::handler_jump_direct, this, _1 ) );
	return ( 0 );
	M_EPILOG
}

HStatusBarControl::ptr_t& HWindow::status_bar( void ) {
	return ( _statusBar );
}

void HWindow::paint( void ) {
	M_PROLOG
	if ( _needRepaint )
		HConsole::get_instance().clrscr();
	if ( ( !! _statusBar ) && ( _statusBar != *_focusedChild ) )
		_statusBar->paint();
	_controls.refresh_all( _needRepaint );
	_needRepaint = false;
	return;
	M_EPILOG
}

bool HWindow::handler_jump_tab( HEvent const& ) {
	M_PROLOG
	_controls.next_enabled();
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
		/* call below is _magic_, HControlList::next_enabled() takes char as an
		 * argument, so code_ & 0x0ff is passed into the function,
		 * code_ is consrtructed from ordinary char by KEY_META_ macro,
		 * see console.h for details */
		HControlList::model_t::cyclic_iterator it = _focusedChild;
		if ( keyPress.get_key_code() & 0x0ff00 ) {
			_controls.next_enabled( static_cast<char>( keyPress.get_key_code() ) );
			if ( _focusedChild != it ) {
				consumed = true;
			}
			_tuiProcess->schedule_repaint();
		}
	}
	return ( consumed );
	M_EPILOG
}

void HWindow::acquire_focus( HControl const* control_ ) {
	M_PROLOG
	if ( (*_focusedChild) != control_ ) {
		(*_focusedChild)->kill_focus();
		_controls.select( control_ );
		_tuiProcess->schedule_repaint();
	}
	return;
	M_EPILOG
}

bool HWindow::handler_command( HEvent const& ) {
	M_PROLOG
	_statusBar->set_prompt( ":", HStatusBarControl::PROMPT::COMMAND );
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
		if ( code >= KEY_CODES::COMMAND_BASE )
			code -= KEY_CODES::COMMAND_BASE;
		prompt [ 0 ] = static_cast<char>( code );
		_statusBar->set_prompt ( prompt, HStatusBarControl::PROMPT::SEARCH );
		consumed = true;
	}
	return ( consumed );
	M_EPILOG
}

int HWindow::click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	if ( ! mouse_._buttons )
		return ( 1 );
	if ( (*_focusedChild) == _statusBar )
		return ( 1 );
	_controls.hit_test_all( mouse_ );
	return ( 0 );
	M_EPILOG
}

HString HWindow::get_command( void ) {
	M_PROLOG
	HString command;
	command = _command;
	_command = "";
	return ( command );
	M_EPILOG
}

bool HWindow::is_initialised( void ) const {
	return ( _initialised );
}

void HWindow::update_all( void ) {
	M_PROLOG
	_controls.update_all();
	return;
	M_EPILOG
}

HString const& HWindow::get_title( void ) const {
	M_PROLOG
	return ( _title );
	M_EPILOG
}

void HWindow::schedule_repaint( bool wholeWindow_ ) {
	if ( wholeWindow_ ) {
		_needRepaint = true;
	}
	_tuiProcess->schedule_repaint();
	return;
}

}

}


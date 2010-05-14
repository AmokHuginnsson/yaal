/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindow.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hwindow.hxx"
#include "console.hxx"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HWindow::HWindow( char const * title_ ) : _initialised( false ),
	_needRepaint( false ), _title( title_ ), _focusedChild(),
	_previousFocusedChild(), _controls( _focusedChild ), _statusBar()
	{
	M_PROLOG
	int cmds [] = { ':', KEY<':'>::command };
	int search [] = { '/', KEY<'/'>::command, '?', KEY<'?'>::command };
	if ( ! HCons::get_instance().is_enabled() )
		M_THROW( "console not initialised.", errno );
	register_postprocess_handler( '\t', NULL, &HWindow::handler_jump_tab );
	register_postprocess_handler( 2, cmds, &HWindow::handler_command );
	register_postprocess_handler( 4, search, &HWindow::handler_search );
	return;
	M_EPILOG
	}

HWindow::~HWindow( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing window: " << _title << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HWindow::init( void )
	{
	M_PROLOG
	HString string;
	_needRepaint_ = true;
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

int HWindow::process_input( int code_ )
	{
	M_PROLOG
	if ( code_ )
		code_ = process_input_with_handlers ( code_, _preprocessHandlers );
	if ( code_ && !! (*_focusedChild) )
		code_ = (*_focusedChild)->process_input ( code_ );
	if ( code_ )
		code_ = process_input_with_handlers ( code_, _postprocessHandlers );
	if ( ! _command.is_empty() )
		process_command();
	return ( code_ );
	M_EPILOG
	}

int HWindow::add_control( HControl::ptr_t control_, int shortCut_ )
	{
	M_PROLOG
	if ( _preprocessHandlers.find( shortCut_ ) != _preprocessHandlers.end() )
		M_THROW( _( "shortcut occupied" ), shortCut_ );
	_controls.add_control( control_ );
	register_postprocess_handler( shortCut_, NULL, &HWindow::handler_jump_direct );
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl::ptr_t& HWindow::status_bar( void )
	{
	return ( _statusBar );
	}

void HWindow::refresh( void )
	{
	M_PROLOG
	if ( _needRepaint )
		HCons::get_instance().clrscr();
	if ( ( !! _statusBar ) && ( _statusBar != *_focusedChild ) )
		_statusBar->refresh();
	_controls.refresh_all( _needRepaint );
	_needRepaint = false;
	_needRepaint_ = true;
	return;
	M_EPILOG
	}

int HWindow::handler_jump_tab( int code_, void const* )
	{
	M_PROLOG
	_controls.next_enabled();
	_needRepaint_ = true;
	code_ = 0;
	return ( code_ );
	M_EPILOG
	}

int HWindow::handler_jump_direct( int code_, void const* )
	{
	M_PROLOG
	/* call below is _magic_, HControlList::next_enabled() takes char as an
	 * argument, so code_ & 0x0ff is passed into the function,
	 * code_ is consrtructed from ordinary char by KEY_META_ macro,
	 * see console.h for details */
	HControlList::model_t::cyclic_iterator it = _focusedChild;
	if ( code_ & 0x0ff00 )
		{
		_controls.next_enabled( static_cast<char>( code_ ) );
		if ( _focusedChild != it )
			code_ = 0;
		_needRepaint_ = true;
		}
	return ( code_ );
	M_EPILOG
	}

void HWindow::acquire_focus( HControl const* const control_ )
	{
	M_PROLOG
	if ( (*_focusedChild) == control_ )
		return;
	(*_focusedChild)->kill_focus();
	_controls.select ( control_ );
	_needRepaint_ = true;
	return;
	M_EPILOG
	}

int HWindow::handler_command( int code_, void const* )
	{
	M_PROLOG
	code_ = 0;
	_statusBar->set_prompt ( ":", HStatusBarControl::PROMPT::COMMAND );
	return ( code_ );
	M_EPILOG
	}

int HWindow::handler_search( int code_, void const* )
	{
	M_PROLOG
	char prompt [ ] = "/\0";
	if ( ! (*_focusedChild)->is_searchable() )
		return ( code_ );
	if ( code_ >= KEY_CODES::COMMAND_BASE )
		code_ -= KEY_CODES::COMMAND_BASE;
	prompt [ 0 ] = static_cast < char > ( code_ );
	_statusBar->set_prompt ( prompt, HStatusBarControl::PROMPT::SEARCH );
	code_ = 0;
	return ( code_ );
	M_EPILOG
	}

int HWindow::click( mouse::OMouse& mouse_ )
	{
	M_PROLOG
	if ( ! mouse_._buttons )
		return ( 1 );
	if ( (*_focusedChild) == _statusBar )
		return ( 1 );
	_controls.hit_test_all( mouse_ );
	return ( 0 );
	M_EPILOG
	}

HString HWindow::get_command( void )
	{
	M_PROLOG
	HString command;
	command = _command;
	_command = "";
	return ( command );
	M_EPILOG
	}

bool HWindow::is_initialised( void ) const
	{
	return ( _initialised );
	}

void HWindow::update_all( void )
	{
	M_PROLOG
	_controls.update_all();
	return;
	M_EPILOG
	}

HString const& HWindow::get_title( void ) const
	{
	M_PROLOG
	return ( _title );
	M_EPILOG
	}

void HWindow::schedule_refresh( void )
	{
	_needRepaint = true;
	_needRepaint_ = true;
	return;
	}

}

}


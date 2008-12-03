/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindow.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
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

HWindow::HWindow( char const * a_pcTitle ) : f_bInitialised( false ),
	f_bNeedRepaint( false ), f_oTitle( a_pcTitle ), f_oFocusedChild(),
	f_oPreviousFocusedChild(), f_oControls( f_oFocusedChild ), f_oStatusBar()
	{
	M_PROLOG
	int l_piCmds [] = { ':', KEY<':'>::command };
	int l_piSearch [] = { '/', KEY<'/'>::command, '?', KEY<'?'>::command };
	if ( ! HCons::get_instance().is_enabled() )
		M_THROW( "console not initialised.", errno );
	register_postprocess_handler( '\t', NULL, &HWindow::handler_jump_tab );
	register_postprocess_handler( 2, l_piCmds, &HWindow::handler_command );
	register_postprocess_handler( 4, l_piSearch, &HWindow::handler_search );
	return;
	M_EPILOG
	}

HWindow::~HWindow( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing window: " << f_oTitle << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HWindow::init( void )
	{
	M_PROLOG
	HString l_oString;
	n_bNeedRepaint = true;
	l_oString.format( " [%s]& \n", f_oTitle.raw() );
	new HStatusBarControl( this, l_oString.raw() );
	/*
	 * After line above window has 1 (one) control which is HStatusBarControl.
	 * This only control has a focus.
	 * */
	f_oStatusBar = *f_oFocusedChild;
	f_oStatusBar->enable( true );
	f_bInitialised = true;
	return ( 0 );
	M_EPILOG
	}

int HWindow::process_input( int a_iCode )
	{
	M_PROLOG
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPreprocessHandlers );
	if ( a_iCode && !! (*f_oFocusedChild) )
		a_iCode = (*f_oFocusedChild)->process_input ( a_iCode );
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPostprocessHandlers );
	if ( ! f_oCommand.is_empty() )
		process_command();
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::add_control( HControl::ptr_t a_oControl, int a_iShortCut )
	{
	M_PROLOG
	if ( f_oPreprocessHandlers.has_key ( a_iShortCut ) )
		M_THROW ( _ ( "shortcut occupied" ), a_iShortCut );
	f_oControls.add_control ( a_oControl );
	register_postprocess_handler ( a_iShortCut, NULL,
			& HWindow::handler_jump_direct );
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl::ptr_t& HWindow::status_bar( void )
	{
	return ( f_oStatusBar );
	}

void HWindow::refresh( void )
	{
	M_PROLOG
	if ( f_bNeedRepaint )
		HCons::get_instance().clrscr();
	if ( ( !! f_oStatusBar ) && ( f_oStatusBar != *f_oFocusedChild ) )
		f_oStatusBar->refresh();
	f_oControls.refresh_all( f_bNeedRepaint );
	f_bNeedRepaint = false;
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_jump_tab( int a_iCode, void const* )
	{
	M_PROLOG
	f_oControls.next_enabled();
	n_bNeedRepaint = true;
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_jump_direct( int a_iCode, void const* )
	{
	M_PROLOG
	/* call below is _magic_, HControlList::next_enabled() takes char as an
	 * argument, so a_iCode & 0x0ff is passed into the function,
	 * a_iCode is consrtructed from ordinary char by D_KEY_META_ macro,
	 * see console.h for details */
	HControlList::model_t::cyclic_iterator it = f_oFocusedChild;
	if ( a_iCode & 0x0ff00 )
		{
		f_oControls.next_enabled( static_cast<char>( a_iCode ) );
		if ( f_oFocusedChild != it )
			a_iCode = 0;
		n_bNeedRepaint = true;
		}
	return ( a_iCode );
	M_EPILOG
	}

void HWindow::acquire_focus( HControl const* const a_poControl )
	{
	M_PROLOG
	if ( (*f_oFocusedChild) == a_poControl )
		return;
	(*f_oFocusedChild)->kill_focus();
	f_oControls.select ( a_poControl );
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_command( int a_iCode, void const* )
	{
	M_PROLOG
	a_iCode = 0;
	f_oStatusBar->set_prompt ( ":", HStatusBarControl::PROMPT::D_COMMAND );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_search( int a_iCode, void const* )
	{
	M_PROLOG
	char l_pcPrompt [ ] = "/\0";
	if ( ! (*f_oFocusedChild)->is_searchable() )
		return ( a_iCode );
	if ( a_iCode >= KEY_CODES::D_COMMAND_BASE )
		a_iCode -= KEY_CODES::D_COMMAND_BASE;
	l_pcPrompt [ 0 ] = static_cast < char > ( a_iCode );
	f_oStatusBar->set_prompt ( l_pcPrompt, HStatusBarControl::PROMPT::D_SEARCH );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::click( mouse::OMouse& a_rsMouse )
	{
	M_PROLOG
	if ( ! a_rsMouse.f_iButtons )
		return ( 1 );
	if ( (*f_oFocusedChild) == f_oStatusBar )
		return ( 1 );
	f_oControls.hit_test_all( a_rsMouse );
	return ( 0 );
	M_EPILOG
	}

HString HWindow::get_command( void )
	{
	M_PROLOG
	HString l_oCommand;
	l_oCommand = f_oCommand;
	f_oCommand = "";
	return ( l_oCommand );
	M_EPILOG
	}

bool HWindow::is_initialised( void ) const
	{
	return ( f_bInitialised );
	}

void HWindow::update_all( void )
	{
	M_PROLOG
	f_oControls.update_all();
	return;
	M_EPILOG
	}

HString const& HWindow::get_title( void ) const
	{
	M_PROLOG
	return ( f_oTitle );
	M_EPILOG
	}

void HWindow::schedule_refresh( void )
	{
	f_bNeedRepaint = true;
	n_bNeedRepaint = true;
	return;
	}

}

}


/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcontrol.h"
#include "hconsole.h"
#include "hwindow.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HControl::HControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
										 int a_iHeight, int a_iWidth, char const * a_pcLabel )
	: f_bEnabled ( false ), f_bFocused ( false ), f_bDrawLabel ( true ),
	f_bSingleLine ( false ), f_uiAttributeDisabled ( n_iAttributeDisabled ),
	f_uiAttributeEnabled ( n_iAttributeEnabled ),
	f_uiAttributeFocused ( n_iAttributeFocused ), f_iRow ( a_iRow ), f_iColumn ( a_iColumn ),
	f_iHeight ( a_iHeight ), f_iWidth ( a_iWidth ), f_iRowRaw ( 0 ),
	f_iColumnRaw ( 0 ), f_iHeightRaw ( 0 ), f_iWidthRaw ( 0 ),
	f_oLabel ( a_pcLabel ), f_oVarTmpBuffer(), f_poParent ( a_poParent ),
	f_iLabelLength ( 0 ), f_iShortcutIndex ( 0 ), f_bValid( false ), f_bNeedRepaint( false )
	{
	M_PROLOG
	if ( ! HCons::get_instance().is_enabled() )
		M_THROW ( "not in curses mode.", errno );
	if ( ! a_poParent )
		M_THROW ( "no parent window.", reinterpret_cast < int > ( a_poParent ) );
	f_iShortcutIndex = f_oLabel.find ( '&' );
	if ( f_iShortcutIndex > -1 )
		{
		f_oLabel.set_at( f_iShortcutIndex, 0 );
		f_oLabel += a_pcLabel + f_iShortcutIndex + 1;
		}
	else
		f_iShortcutIndex = 0;
	f_iLabelLength = a_pcLabel ? f_oLabel.get_length() : 0;
	if ( f_iLabelLength )
		{
		if ( f_oLabel [ f_iLabelLength - 1 ] != '\n' )
			f_bSingleLine = true;
		else
			{
			f_bSingleLine = false;
			f_oLabel.set_at( f_iLabelLength - 1, 0 );
			}
		}
	else
		f_bSingleLine = true;
	f_poParent->add_control ( HControl::ptr_t ( this ),
				KEY<>::meta_r ( f_oLabel [ f_iShortcutIndex ] ) );
	return;
	M_EPILOG
	}

HControl::~HControl ( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing control: " << f_oLabel << endl;
#endif /* __DEBUGGER_BABUNI__ */
	f_poParent = NULL;
	return;
	M_EPILOG
	}

void HControl::enable ( bool a_bEnable )
	{
	M_PROLOG
	f_bEnabled = a_bEnable;
	if ( ! f_bEnabled )
		f_bFocused = false;
	schedule_refresh();
	return;
	M_EPILOG
	}

int HControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ! f_bValid )
		update();
	return ( do_process_input( a_iCode ) );
	M_EPILOG
	}

int HControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ! f_bFocused )
		M_THROW( "input in control without focus", a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

int HControl::set_focus ( char a_cShortCut )
	{
	M_PROLOG
	if ( ! f_bEnabled )
		return ( 1 );
	if ( ( a_cShortCut > 0 ) && ( f_oLabel [ f_iShortcutIndex ] != a_cShortCut ) )
		return ( 1 );
	f_bFocused = true;
	if ( ! a_cShortCut )
		f_poParent->acquire_focus ( this );
	if ( a_cShortCut <= 0 )
		schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

int HControl::kill_focus ( void )
	{
	M_PROLOG
	if ( ! f_bFocused )
		return ( 1 );
	f_bFocused = false;
	schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

void HControl::refresh ( void )
	{
	M_PROLOG
	if ( ! f_bValid )
		update();
	do_refresh();
	return;
	M_EPILOG
	}

void HControl::update ( void )
	{
	M_PROLOG
	do_update();
	f_bValid = true;
	return;
	M_EPILOG
	}

void HControl::set ( HInfo const & )
	{
	M_PROLOG
	if ( errno || ! errno )
		M_THROW ( "Abstract method called!", errno );
	return;
	M_EPILOG
	}

HInfo HControl::get ( void )
	{
	M_PROLOG
	HInfo l_oInfo;
	return ( l_oInfo );
	M_EPILOG
	}

bool HControl::is_searchable ( void )
	{
	M_PROLOG
	return ( false );
	M_EPILOG
	}

void HControl::draw_label( void )
	{
	M_PROLOG
	do_draw_label();
	f_bNeedRepaint = false;
	return;
	M_EPILOG
	}

void HControl::do_draw_label( void )
	{
	M_PROLOG
	HConsole& cons = HCons::get_instance();
	schedule_refresh();
/* reposition control acordingly to current parent window size */
	f_iRowRaw = ( f_iRow >= 0 ) ? f_iRow : cons.get_height() + f_iRow;
	f_iColumnRaw = ( f_iColumn >= 0 ) ? f_iColumn
		: cons.get_width() + f_iColumn;
	f_iHeightRaw = ( f_iHeight > 0 ) ? f_iHeight 
		: cons.get_height() + f_iHeight - f_iRowRaw;
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: cons.get_width() + f_iWidth - f_iColumnRaw;
/* done */
	if ( ! f_bDrawLabel )
		{
		set_attr_data();
		return;
		}
	set_attr_label();
	M_ENSURE ( cons.c_mvprintf ( f_iRowRaw, f_iColumnRaw, f_oLabel ) != C_ERR );
	set_attr_shortcut();
	M_ENSURE ( cons.c_mvprintf ( f_iRowRaw, f_iColumnRaw + f_iShortcutIndex,
				"%c", f_oLabel [ f_iShortcutIndex ] ) != C_ERR );
	set_attr_data();
	if ( f_bSingleLine )
		f_iColumnRaw += f_iLabelLength, f_iWidthRaw -= f_iLabelLength;
	else
		f_iRowRaw ++, f_iHeightRaw --;
	return;
	M_EPILOG
	}

void HControl::set_attributes ( int a_iAttributeDisabled,
		int a_iAttributeEnabled, int a_iAttributeFocused )
	{
	M_PROLOG
	if ( a_iAttributeDisabled == D_DEFAULT_ATTRS )
		f_uiAttributeDisabled = n_iAttributeDisabled;
	else
		f_uiAttributeDisabled = a_iAttributeDisabled;
	if ( a_iAttributeEnabled == D_DEFAULT_ATTRS )
		f_uiAttributeEnabled = n_iAttributeEnabled;
	else
		f_uiAttributeEnabled = a_iAttributeEnabled;
	if ( a_iAttributeFocused == D_DEFAULT_ATTRS )
		f_uiAttributeFocused = n_iAttributeFocused;
	else
		f_uiAttributeFocused = a_iAttributeFocused;
	schedule_refresh();
	return;
	M_EPILOG
	}

void HControl::move ( int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth )
	{
	M_PROLOG
	f_iRow = a_iRow;
	f_iColumn = a_iColumn;
	f_iHeight = a_iHeight;
	f_iWidth = a_iWidth;
	schedule_refresh();
	return;
	M_EPILOG
	}

int HControl::click ( mouse::OMouse & a_roMouse )
	{
	M_PROLOG
	if ( ! f_bValid )
		update();
	return ( do_click( a_roMouse ) );
	M_EPILOG
	}

int HControl::do_click ( mouse::OMouse & )
	{
	M_PROLOG
	if ( f_bFocused )
		return ( 1 );
	set_focus();
	return ( 0 );
	M_EPILOG
	}

void HControl::do_update ( void )
	{
	return;
	}

bool HControl::hit_test ( int a_iRow, int a_iColumn ) const
	{
	M_PROLOG
	if ( ( a_iRow < f_iRowRaw ) || ( a_iRow > ( f_iRowRaw + f_iHeightRaw ) ) )
		return ( false );
	if ( ( a_iColumn < f_iColumnRaw )
			|| ( a_iColumn >= ( f_iColumnRaw + f_iWidthRaw ) ) )
		return ( false );
	return ( true );
	M_EPILOG
	}

int HControl::attr_label ( void ) const
	{
	M_PROLOG
	return ( f_bEnabled ? ( f_bFocused ? f_uiAttributeFocused >> 8 : f_uiAttributeEnabled >> 8 ) : f_uiAttributeDisabled >> 8 );
	M_EPILOG
	}

int HControl::attr_shortcut ( void ) const
	{
	M_PROLOG
	return ( ! f_bEnabled ? ( ! f_bFocused ? f_uiAttributeFocused >> 8 : f_uiAttributeEnabled >> 8 ) : f_uiAttributeDisabled >> 8 );
	M_EPILOG
	}

int HControl::attr_data ( void ) const
	{
	M_PROLOG
	return ( f_bEnabled ? ( f_bFocused ? f_uiAttributeFocused : f_uiAttributeEnabled ) : f_uiAttributeDisabled );
	M_EPILOG
	}

void HControl::set_attr_label ( void ) const
	{
	M_PROLOG
	HCons::get_instance().set_attr ( attr_label() ); /* FIXME */
	return;
	M_EPILOG
	}

void HControl::set_attr_shortcut ( void ) const
	{
	M_PROLOG
	HCons::get_instance().set_attr ( attr_shortcut() ); /* FIXME */
	return;
	M_EPILOG
	}

void HControl::set_attr_data ( void ) const
	{
	M_PROLOG
	HCons::get_instance().set_attr ( attr_data() ); /* FIXME */
	return;
	M_EPILOG
	}

void HControl::set_draw_label( bool a_bDrawLabel )
	{
	f_bDrawLabel = a_bDrawLabel;
	return;
	}

void HControl::schedule_refresh( void )
	{
	f_bNeedRepaint = true;
	n_bNeedRepaint = true;
	return;
	}

void HControl::invalidate( void )
	{
	f_bValid = false;
	return;
	}

bool HControl::need_repaint( void ) const
	{
	return ( f_bNeedRepaint );
	}

}

}


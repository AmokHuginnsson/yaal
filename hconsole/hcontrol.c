/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.c - this file is integral part of `stdhapi' project.

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

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcontrol.h"
#include "hconsole.h"
#include "hwindow.h"

#ifdef __DEBUGGER_BABUNI__
#include "../hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

HControl::HControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
										 int a_iHeight, int a_iWidth, const char * a_pcLabel,
										 bool a_bDrawLabel, int a_iDisabledAttribute,
										 int a_iEnabledAttribute, int a_iFocusedAttribute )
	{
	M_PROLOG
	if ( ! console::is_enabled ( ) )
		throw new HException ( __WHERE__, "not in curses mode.", g_iErrNo );
	if ( ! a_poParent )
		throw new HException ( __WHERE__, "no parent window.", ( int ) a_poParent );
	f_bEnabled = false;
	f_bFocused = false;
	f_bDrawLabel = a_bDrawLabel;
	f_poParent = a_poParent;
	f_iRow = a_iRow;
	f_iColumn = a_iColumn;
	f_iHeight = a_iHeight;
	f_iWidth = a_iWidth;
	f_iRowRaw = 0;
	f_iColumnRaw = 0;
	f_iHeightRaw = 0;
	f_iWidthRaw = 0;
	if ( a_iDisabledAttribute > 0 )f_iDisabledAttribute = a_iDisabledAttribute;
	else f_iDisabledAttribute = console::n_iDisabledAttribute;
	if ( a_iEnabledAttribute > 0 )f_iEnabledAttribute = a_iEnabledAttribute;
	else f_iEnabledAttribute = console::n_iEnabledAttribute;
	if ( a_iFocusedAttribute > 0 )f_iFocusedAttribute = a_iFocusedAttribute;
	else f_iFocusedAttribute = console::n_iFocusedAttribute;
	f_oLabel = a_pcLabel;
	f_iShortcutIndex = f_oLabel.find ( '&' );
	if ( f_iShortcutIndex > -1 )
		{
		f_oLabel [ f_iShortcutIndex ] = 0;
		f_oLabel += a_pcLabel + f_iShortcutIndex + 1;
		}
	else f_iShortcutIndex = 0;
	f_iLabelLength = a_pcLabel ? f_oLabel.get_length ( ) : 0;
	if ( f_iLabelLength )
		{
		if ( f_oLabel [ f_iLabelLength - 1 ] != '\n' )f_bSingleLine = true;
		else
			{
			f_bSingleLine = false;
			f_oLabel [ f_iLabelLength - 1 ] = 0;
			}
		}
	else f_bSingleLine = true;
	f_poParent->add_control ( this, D_KEY_META_(f_oLabel [ f_iShortcutIndex ]) );
	return;
	M_EPILOG
	}

HControl::~HControl ( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing control: " << f_oLabel << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

void HControl::enable ( bool a_bEnable )
	{
	M_PROLOG
	f_bEnabled = a_bEnable;
	if ( ! f_bEnabled )f_bFocused = false;
	refresh ( );
	return;
	M_EPILOG
	}

int HControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ! f_bFocused )throw new HException ( __WHERE__,
			"input in widow without focus", a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

int HControl::set_focus ( char a_cShortCut )
	{
	M_PROLOG
	if ( ! f_bEnabled )return ( 1 );
	if ( ( a_cShortCut > 0 ) && ( f_oLabel [ f_iShortcutIndex ] != a_cShortCut ) )
		return ( 1 );
	f_bFocused = true;
	if ( ! a_cShortCut )
		f_poParent->set_focus ( this );
	if ( a_cShortCut <= 0 )refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HControl::kill_focus ( void )
	{
	M_PROLOG
	if ( ! f_bFocused )return ( 1 );
	f_bFocused = false;
	refresh ( );
	return ( 0 );
	M_EPILOG
	}

void HControl::refresh ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HInfo HControl::operator = ( const HInfo & a_roInfo )
	{
	M_PROLOG
	HInfo l_oInfo = a_roInfo;
	if ( & a_roInfo )
		throw new HException ( __WHERE__, "what the fuck?", g_iErrNo );
	return ( l_oInfo );
	M_EPILOG
	}

HControl::operator HInfo ( void )
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

void HControl::draw_label ( void )
	{
	M_PROLOG
	console::n_bNeedRepaint = true;
/* reposition control acordingly to current parent window size */
	f_iRowRaw = ( f_iRow >= 0 ) ? f_iRow : console::n_iHeight + f_iRow;
	f_iColumnRaw = ( f_iColumn >= 0 ) ? f_iColumn
		: console::n_iWidth + f_iColumn;
	f_iHeightRaw = ( f_iHeight > 0 ) ? f_iHeight 
		: console::n_iHeight + f_iHeight - f_iRowRaw;
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: console::n_iWidth + f_iWidth - f_iColumnRaw;
/* done */
	if ( ! f_bDrawLabel )
		{
		M_SET_ATTR_DATA ( );
		return;
		}
	M_SET_ATTR_LABEL ( );
	::mvprintw ( f_iRowRaw, f_iColumnRaw, f_oLabel );
	M_SET_ATTR_SHORTCUT ( );
	::mvprintw( f_iRowRaw, f_iColumnRaw + f_iShortcutIndex,
			"%c", f_oLabel [ f_iShortcutIndex ] );
	M_SET_ATTR_DATA ( );
	if ( f_bSingleLine )
		f_iColumnRaw += f_iLabelLength, f_iWidthRaw -= f_iLabelLength;
	else f_iRowRaw ++, f_iHeightRaw --;
	return;
	M_EPILOG
	}

void HControl::set_attributes ( int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
	{
	M_PROLOG
	f_iDisabledAttribute = a_iDisabledAttribute;
	f_iEnabledAttribute = a_iEnabledAttribute;
	f_iFocusedAttribute = a_iFocusedAttribute;
	refresh ( );
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
	refresh ( );
	return;
	M_EPILOG
	}

int HControl::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	if ( a_rsMouse.f_iButtons )
		{
		if ( hit_test ( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
			set_focus ( );
		}
	return ( 0 );
	M_EPILOG
	}

bool HControl::hit_test ( int a_iRow, int a_iColumn )
	{
	M_PROLOG
	if ( ( a_iRow < f_iRowRaw ) || ( a_iRow > ( f_iRowRaw + f_iHeight ) ) )
		return ( false );
	if ( ( a_iColumn < f_iColumn )
			|| ( a_iColumn > ( f_iColumnRaw + f_iWidth ) ) )
		return ( false );
	return ( true );
	M_EPILOG
	}


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

#include <string.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcontrol.h"
#include "hconsole.h"
#include "hwindow.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HControl::HControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
										 int a_iHeight, int a_iWidth, char const * a_pcLabel,
										 bool a_bDrawLabel, int a_iDisabledAttribute,
										 int a_iEnabledAttribute, int a_iFocusedAttribute )
	: f_bEnabled ( false ), f_bFocused ( false ), f_bDrawLabel ( a_bDrawLabel ),
	f_bSingleLine ( false ), f_uiDisabledAttribute ( 0 ), f_uiEnabledAttribute ( 0 ),
	f_uiFocusedAttribute ( 0 ), f_iRow ( a_iRow ), f_iColumn ( a_iColumn ),
	f_iHeight ( a_iHeight ), f_iWidth ( a_iWidth ), f_iRowRaw ( 0 ),
	f_iColumnRaw ( 0 ), f_iHeightRaw ( 0 ), f_iWidthRaw ( 0 ),
	f_oLabel ( a_pcLabel ), f_oVarTmpBuffer ( ), f_poParent ( a_poParent ),
	f_iLabelLength ( 0 ), f_iShortcutIndex ( 0 )
	{
	M_PROLOG
	if ( ! is_enabled ( ) )
		M_THROW ( "not in curses mode.", g_iErrNo );
	if ( ! a_poParent )
		M_THROW ( "no parent window.", reinterpret_cast < int > ( a_poParent ) );
	if ( a_iDisabledAttribute > 0 )
		f_uiDisabledAttribute = a_iDisabledAttribute;
	else
		f_uiDisabledAttribute = n_iAttributeDisabled;
	if ( a_iEnabledAttribute > 0 )
		f_uiEnabledAttribute = a_iEnabledAttribute;
	else
		f_uiEnabledAttribute = n_iAttributeEnabled;
	if ( a_iFocusedAttribute > 0 )
		f_uiFocusedAttribute = a_iFocusedAttribute;
	else
		f_uiFocusedAttribute = n_iAttributeFocused;
	f_iShortcutIndex = f_oLabel.find ( '&' );
	if ( f_iShortcutIndex > -1 )
		{
		f_oLabel [ f_iShortcutIndex ] = 0;
		f_oLabel += a_pcLabel + f_iShortcutIndex + 1;
		}
	else
		f_iShortcutIndex = 0;
	f_iLabelLength = a_pcLabel ? f_oLabel.get_length ( ) : 0;
	if ( f_iLabelLength )
		{
		if ( f_oLabel [ f_iLabelLength - 1 ] != '\n' )
			f_bSingleLine = true;
		else
			{
			f_bSingleLine = false;
			f_oLabel [ f_iLabelLength - 1 ] = 0;
			}
		}
	else
		f_bSingleLine = true;
	M_IRV ( f_poParent->add_control ( this,
				D_KEY_META_(f_oLabel [ f_iShortcutIndex ]) ) );
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
	refresh ( );
	return;
	M_EPILOG
	}

int HControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ! f_bFocused )
		M_THROW ( "input in widow without focus", a_iCode );
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
		f_poParent->set_focus ( this );
	if ( a_cShortCut <= 0 )
		refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HControl::kill_focus ( void )
	{
	M_PROLOG
	if ( ! f_bFocused )
		return ( 1 );
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

void HControl::set ( HInfo const & )
	{
	M_PROLOG
	if ( g_iErrNo || ! g_iErrNo )
		M_THROW ( "Abstract method called!", g_iErrNo );
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

void HControl::draw_label ( void )
	{
	M_PROLOG
	n_bNeedRepaint = true;
/* reposition control acordingly to current parent window size */
	f_iRowRaw = ( f_iRow >= 0 ) ? f_iRow : n_iHeight + f_iRow;
	f_iColumnRaw = ( f_iColumn >= 0 ) ? f_iColumn
		: n_iWidth + f_iColumn;
	f_iHeightRaw = ( f_iHeight > 0 ) ? f_iHeight 
		: n_iHeight + f_iHeight - f_iRowRaw;
	f_iWidthRaw = ( f_iWidth > 0 ) ? f_iWidth
		: n_iWidth + f_iWidth - f_iColumnRaw;
/* done */
	if ( ! f_bDrawLabel )
		{
		M_SET_ATTR_DATA ( );
		return;
		}
	M_SET_ATTR_LABEL ( );
	M_ENSURE ( c_mvprintf ( f_iRowRaw, f_iColumnRaw, f_oLabel ) != C_ERR );
	M_SET_ATTR_SHORTCUT ( );
	M_ENSURE ( c_mvprintf ( f_iRowRaw, f_iColumnRaw + f_iShortcutIndex,
				"%c", f_oLabel [ f_iShortcutIndex ] ) != C_ERR );
	M_SET_ATTR_DATA ( );
	if ( f_bSingleLine )
		f_iColumnRaw += f_iLabelLength, f_iWidthRaw -= f_iLabelLength;
	else
		f_iRowRaw ++, f_iHeightRaw --;
	return;
	M_EPILOG
	}

void HControl::set_attributes ( int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
	{
	M_PROLOG
	f_uiDisabledAttribute = a_iDisabledAttribute;
	f_uiEnabledAttribute = a_iEnabledAttribute;
	f_uiFocusedAttribute = a_iFocusedAttribute;
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

int HControl::click ( mouse::OMouse & )
	{
	M_PROLOG
	if ( f_bFocused )
		return ( 1 );
	M_IRV ( set_focus ( ) );
	return ( 0 );
	M_EPILOG
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

}

}


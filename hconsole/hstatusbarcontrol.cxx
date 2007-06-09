/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstatusbarcontrol.cxx - this file is integral part of `yaal' project.

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
#include <libintl.h>
#include <cstdio>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hstatusbarcontrol.h"
#include "hwindow.h"
#include "hconsole.h"
#include "hsearchablecontrol.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HStatusBarControl::HStatusBarControl ( HWindow * a_poParent,
		char const * a_pcLabel, int a_iStatusBarAttribute )
								 : HControl ( a_poParent, - 2, 0, 2, - 1, a_pcLabel ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL, 127, "", n_pcMaskLoose,
											false, false, false, false, false, 255 ),
	f_iStatusBarAttribute ( 0 ), f_iPromptLength ( 0 ),
	f_eMode ( PROMPT::D_NORMAL ), f_eRestrict ( PROMPT::D_RELAXED ),
	f_oPrompt(), f_bDone ( false ), f_bEstimate ( false ), f_dProgressSize ( 1 ),
	f_iLastProgress ( - 1 ), f_iLastPercent ( - 1 ), f_iLastMinute ( 0 ),
	f_iLastSecond ( 0 ), f_iLastStep ( 0 ),
	f_oMessage ( "" ), /* initialization of this field is required by bar() meth */
	f_oStart()
	{
	M_PROLOG
	int l_iAttribte = 0;
	if ( a_iStatusBarAttribute > 0 )
		f_iStatusBarAttribute = a_iStatusBarAttribute;
	else
		f_iStatusBarAttribute = n_iAttributeStatusBar;
	l_iAttribte = f_iStatusBarAttribute;
	l_iAttribte &= 0x00ff;
	f_uiAttributeFocused &= 0xff00;
	f_uiAttributeFocused |= l_iAttribte;
	return;
	M_EPILOG
	}

HStatusBarControl::~HStatusBarControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HStatusBarControl::do_draw_label ( void )
	{
	M_PROLOG
	c_move ( n_iHeight - 2, 0 );
	c_clrtoeol();
	HControl::do_draw_label();
	bar();
	f_iColumnRaw += f_iPromptLength;
	f_iWidthRaw -= f_iPromptLength;
	c_move ( f_iRowRaw, f_iPromptLength );
	return;
	M_EPILOG
	}

void HStatusBarControl::do_refresh ( void )
	{
	M_PROLOG
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	if ( ! f_bFocused )
		c_getyx ( l_iOrigRow, l_iOrigColumn );
	if ( f_iPromptLength )
		{
		set_attr ( f_iStatusBarAttribute >> 8 );
		c_mvprintf ( f_iRowRaw, 0, f_oPrompt );
		}
	HEditControl::do_refresh();
	if ( ! f_bFocused )
		c_move ( l_iOrigRow, l_iOrigColumn );
	return;
	M_EPILOG
	}

int HStatusBarControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ( a_iCode == KEY_CODES::D_BACKSPACE )
			&& ( f_eRestrict == PROMPT::D_RELAXED )
			&& ( f_eMode != PROMPT::D_MENU )
			&& ! f_oString.get_length() )
		{
		end_prompt();
		return ( 0 );
		}
	if ( a_iCode != '\t' )
		a_iCode = HEditControl::do_process_input ( a_iCode );
	switch ( f_eMode )
		{
		case ( PROMPT::D_NORMAL ):
		case ( PROMPT::D_COMMAND ):
		case ( PROMPT::D_SEARCH ):
			a_iCode = process_input_normal ( a_iCode );
		break;
		case ( PROMPT::D_MENU ):
			a_iCode = process_input_menu ( a_iCode );
		break;
		default :
			break;
		}
	return ( a_iCode );
	M_EPILOG
	}

void HStatusBarControl::set_prompt ( char const * a_pcPrompt, PROMPT::mode_t a_eMode,
		PROMPT::restrict_t a_eRestrict )
	{
	M_PROLOG
	f_eMode = a_eMode;
	f_eRestrict = a_eRestrict;
	f_poParent->f_oPreviousFocusedChild = f_poParent->f_oFocusedChild;
	f_poParent->f_oControls.select( f_poParent->f_oStatusBar );
	(*f_poParent->f_oPreviousFocusedChild)->kill_focus();
	set_focus ( -1 );
	if ( a_pcPrompt )
		{
		f_oPrompt = a_pcPrompt;
		f_iPromptLength = f_oPrompt.get_length();
		}
	HEditControl::set ( "" ); /* refresh call inside */
	return;
	M_EPILOG
	}

int HStatusBarControl::verify ( void )
	{
	M_PROLOG
	int l_iLength = 0;
	bool l_bOk = false;
	if ( l_iLength < f_iPromptLength )
		l_bOk = true;
	return ( l_bOk );
	M_EPILOG
	}

void HStatusBarControl::init_progress ( double a_dMax, char const * a_pcTitle,
		bool a_bEstimate )
	{
	M_PROLOG
	f_bDone = false;
	f_bEstimate = a_bEstimate;
	f_dProgressSize = a_dMax;
	f_iLastProgress = - 1;
	f_iLastPercent = - 1;
	f_iLastStep = 0;
	f_oMessage = a_pcTitle;
	f_oStart.set_now();
	return;
	M_EPILOG
	}

void HStatusBarControl::update_progress ( double a_dStep,
		char const * a_pcTitle )
	{
	M_PROLOG
	int l_iMaxBar = 0;
	int l_iNextStep = 0;
	int l_iNextPercent = 0;
	int l_iNextMinute = 0;
	int l_iNextSecond = 0;
	HTime l_oStoper, l_oNow, l_oLeft;
	l_oNow.format ( "(%T)" );
	if ( f_bDone )
		return;
	if ( a_dStep < 0 )
		a_dStep = f_iLastStep ++;
	l_oStoper = l_oNow - f_oStart;
	if ( f_bEstimate )
		{
		if ( a_dStep )
			l_oNow.set ( static_cast < time_t > ( ( f_dProgressSize / a_dStep ) * l_oStoper ) );
		l_oLeft = l_oNow - l_oStoper;
		}
	/* 6 for "[100%]", 10 for elapse, 10 for estimate, 2 for || */
	l_iMaxBar = n_iWidth - ( 6 + 10 + 2 + ( f_bEstimate ? 10 : 0 ) );
	l_iNextPercent = static_cast < int > ( ( 100. * a_dStep / f_dProgressSize ) );
	l_iNextStep = static_cast < int > ( ( l_iMaxBar * a_dStep / f_dProgressSize ) );
	l_iNextMinute = l_oStoper.get_minute();
	l_iNextSecond = l_oStoper.get_second();
	if ( l_iNextStep >= l_iMaxBar )
		l_iNextStep = l_iMaxBar, f_bDone = true;
	if ( l_iNextPercent >= 100 )
		l_iNextPercent = 100, f_bDone = true;
	if ( a_pcTitle || ( f_iLastProgress != l_iNextStep )
			|| ( f_iLastPercent != l_iNextPercent )
			|| ( f_iLastMinute != l_iNextMinute )
			|| ( f_iLastSecond != l_iNextSecond ))
		{
		schedule_refresh();
		bar ( a_pcTitle );
		if ( f_bEstimate )
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					( a_dStep ? ( static_cast < char const * > ( l_oLeft ) ) : "(?\?:?\?:?\?)" ),
					static_cast < char const * > ( l_oStoper ),	l_iNextPercent, "%%" );
			}
		else
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					static_cast < char const * > ( l_oStoper ), l_iNextPercent, "%%" );
			}
		if ( f_bDone )
			strncpy ( f_oString.raw() + n_iWidth - 5, "done", 4 );
		f_oString.fill ( '-', l_iMaxBar, 1 );
		f_oString.fill ( '=', l_iNextStep, 1 );
		c_mvprintf ( n_iHeight - 1, 0, f_oString );
		f_oString = "";
		f_iLastProgress = l_iNextStep;
		f_iLastPercent = l_iNextPercent;
		f_iLastMinute = l_iNextMinute;
		f_iLastSecond = l_iNextSecond;
		c_refresh();
		}
	return;
	M_EPILOG
	}

void HStatusBarControl::message ( int a_iAttribute,
		char const * a_pcFormat, ... )
	{
	M_PROLOG
	va_list ap;
	va_start ( ap, a_pcFormat );
	if ( a_pcFormat && a_pcFormat [ 0 ] )
		bell();
	f_oVarTmpBuffer.vformat( a_pcFormat, &ap );
	c_cmvprintf ( f_iRowRaw, -1, a_iAttribute, f_oVarTmpBuffer );
	va_end ( ap );
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

void HStatusBarControl::message ( char const * a_pcFormat, ... )
	{
	M_PROLOG
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	if ( a_pcFormat && a_pcFormat [ 0 ] )
		bell();
	f_oVarTmpBuffer.vformat( a_pcFormat, &l_xAp );
	c_cmvprintf ( f_iRowRaw, -1, attr_data(), f_oVarTmpBuffer );
	va_end ( l_xAp );
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

void HStatusBarControl::bar ( char const * a_pcBar )
	{
	M_PROLOG
	set_attr_data();
	if ( a_pcBar )
		{
		f_oVarTmpBuffer.format ( " %%-%ds ",
				( n_iWidth - f_iLabelLength ) - ( f_bSingleLine ? 2 : 1 ) );
		f_oMessage.format ( f_oVarTmpBuffer, a_pcBar );
		}
	c_mvprintf ( n_iHeight - 2,
			f_iLabelLength - ( f_bSingleLine ? 0 : 1 ), f_oMessage );
	return;
	M_EPILOG
	}

int HStatusBarControl::ask ( char const * a_pcQuestion,
		char const * a_pcPrompt )
	{
	M_PROLOG
	bar ( a_pcQuestion );
	set_prompt ( a_pcPrompt );
	return ( 0 );
	M_EPILOG
	}

bool HStatusBarControl::confirm ( char const * a_pcQuestion )
	{
	M_PROLOG
	ask ( a_pcQuestion, "[yes/no]: " );
	return ( false );
	M_EPILOG
	}

int HStatusBarControl::process_input_normal ( int a_iCode )
	{
	M_PROLOG
	bool l_bBackwards = false;
	int l_iCode = a_iCode;
	PROMPT::mode_t l_eMode = f_eMode;
	HSearchableControl * l_poSearchableControl = NULL;
	a_iCode = 0;
	switch ( l_iCode )
		{
		case ( '\r' ):
			{
			l_bBackwards = ( f_oPrompt [ 0 ] == '?' );
			end_prompt();
			if ( l_eMode == PROMPT::D_COMMAND )
				f_poParent->f_oCommand = f_oString;
			else if ( l_eMode == PROMPT::D_SEARCH )
				{
				l_poSearchableControl = dynamic_cast < HSearchableControl * > ( &(*(*f_poParent->f_oPreviousFocusedChild)) );
				if ( l_poSearchableControl )
					l_poSearchableControl->search ( f_oString, l_bBackwards );
				}
			}
		break;
		case ( '\t' ):
			bell();
		break;
		default :
			a_iCode = l_iCode;
		}
	return ( a_iCode );
	M_EPILOG
	}

int HStatusBarControl::process_input_menu  ( int a_iCode )
	{
	M_PROLOG
	return ( a_iCode );
	M_EPILOG
	}

void HStatusBarControl::end_prompt ( void )
	{
	M_PROLOG
	f_eMode = PROMPT::D_NORMAL;
	f_oPrompt = "";
	f_iPromptLength = 0;
	f_poParent->f_oFocusedChild = f_poParent->f_oPreviousFocusedChild;
	f_poParent->f_oStatusBar->kill_focus();
	(*f_poParent->f_oFocusedChild)->set_focus ( -1 );
	return;
	M_EPILOG
	}

}

}


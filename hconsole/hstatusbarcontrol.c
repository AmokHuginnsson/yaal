/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstatusbarcontrol.c - this file is integral part of `stdhapi' project.

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

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hstatusbarcontrol.h"
#include "hwindow.h"
#include "hconsole.h"
#include "hsearchablecontrol.h"

HStatusBarControl::HStatusBarControl ( HWindow * a_poParent,
		char const * a_pcLabel, int a_iStatusBarAttribute )
								 : HControl ( a_poParent, - 2, 0, 255, 0, a_pcLabel ),
									HEditControl ( NULL, 0, 0, 0, 0, NULL, 127, "", D_MASK_LOOSE )
	{
	M_PROLOG
	int l_iAttribte = 0;
	if ( a_iStatusBarAttribute > 0 )
		f_iStatusBarAttribute = a_iStatusBarAttribute;
	else f_iStatusBarAttribute = console::n_iAttributeStatusBar;
	l_iAttribte = f_iStatusBarAttribute;
	l_iAttribte &= 0x00ff;
	f_iMode = D_PROMPT_MODE_NORMAL;
	f_iRestrict = D_PROMPT_RESTRICT_RELAXED;
	f_iFocusedAttribute &= 0xff00;
	f_iFocusedAttribute |= l_iAttribte;
	f_iPromptLength = 0;
	f_bDone = false;
	f_iLastProgress = -1;
	f_iLastPercent = -1;
	f_dProgressSize = 1;
	f_oMessage = ""; /* initialization of this field is required by bar() meth */
	return;
	M_EPILOG
	}

HStatusBarControl::~HStatusBarControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HStatusBarControl::draw_label ( void )
	{
	M_PROLOG
	::move ( console::n_iHeight - 2, 0 );
	::clrtoeol ( );
	HControl::draw_label ( );
	bar ( );
	f_iColumnRaw += f_iPromptLength;
	f_iWidthRaw -= f_iPromptLength;
	::move ( f_iRowRaw, f_iPromptLength );
	return;
	M_EPILOG
	}

void HStatusBarControl::refresh ( void )
	{
	M_PROLOG
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	if ( ! f_bFocused )getyx ( stdscr, l_iOrigRow, l_iOrigColumn );
	if ( f_iPromptLength )
		{
		console::set_attr ( f_iStatusBarAttribute >> 8 );
		::mvprintw ( f_iRowRaw, 0, f_oPrompt );
		}
	HEditControl::refresh ( );
	if ( ! f_bFocused )::move ( l_iOrigRow, l_iOrigColumn );
	return;
	M_EPILOG
	}

int HStatusBarControl::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( ( a_iCode == KEY_BACKSPACE )
			&& ( f_iRestrict == D_PROMPT_RESTRICT_RELAXED )
			&& ( f_iMode != D_PROMPT_MODE_MENU )
			&& ! f_oString.get_length ( ) )
		{
		end_prompt ( );
		return ( 0 );
		}
	if ( a_iCode != '\t' )
		a_iCode = HEditControl::process_input ( a_iCode );
	switch ( f_iMode )
		{
		case ( D_PROMPT_MODE_NORMAL ):
		case ( D_PROMPT_MODE_COMMAND ):
		case ( D_PROMPT_MODE_SEARCH ):
			{
			a_iCode = process_input_normal ( a_iCode );
			break;
			}
		case ( D_PROMPT_MODE_MENU ):
			{
			a_iCode = process_input_menu ( a_iCode );
			break;
			}
		default :
			{
			break;
			}
		}
	return ( a_iCode );
	M_EPILOG
	}

void HStatusBarControl::set_prompt ( char const * a_pcPrompt, int a_iMode,
		int a_iRestrict )
	{
	M_PROLOG
	f_iMode = a_iMode;
	f_iRestrict = a_iRestrict;
	f_poParent->f_poPreviousFocusedChild = f_poParent->f_poFocusedChild;
	f_poParent->f_poFocusedChild = f_poParent->f_poStatusBar;
	f_poParent->f_poPreviousFocusedChild->kill_focus ( );
	set_focus ( -1 );
	if ( a_pcPrompt )
		{
		f_oPrompt = a_pcPrompt;
		f_iPromptLength = f_oPrompt.get_length ( );
		}
	HEditControl::operator = ( "" ); /* refresh() call inside */
	return;
	M_EPILOG
	}

int HStatusBarControl::verify ( void )
	{
	M_PROLOG
	int l_iLength = 0;
	bool l_bOk = false;
	if ( l_iLength < f_iPromptLength )l_bOk = true;
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
	f_iLastPercent = -1;
	f_iLastStep = 0;
	f_oMessage = a_pcTitle;
	f_oStart.set_now ( );
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
	HTime l_oStoper, l_oNow ( "(%T)" ), l_oLeft;
	if ( f_bDone )return;
	if ( a_dStep < 0 )a_dStep = f_iLastStep ++;
	l_oStoper = l_oNow - f_oStart;
	if ( f_bEstimate )
		{
		if ( a_dStep )
			l_oNow = ( time_t ) ( f_dProgressSize / a_dStep * l_oStoper );
		l_oLeft = l_oNow - l_oStoper;
		}
	/* 6 for "[100%]", 10 for elapse, 10 for estimate, 2 for || */
	l_iMaxBar = console::n_iWidth - 6 - 10 - 2 - ( f_bEstimate ? 10 : 0 );
	l_iNextPercent = ( int ) ( 100. * a_dStep / f_dProgressSize );
	l_iNextStep = ( int ) ( l_iMaxBar * a_dStep / f_dProgressSize );
	l_iNextMinute = l_oStoper.get_minute ( );
	l_iNextSecond = l_oStoper.get_second ( );
	if ( l_iNextStep >= l_iMaxBar )l_iNextStep = l_iMaxBar, f_bDone = true;
	if ( l_iNextPercent >= 100 )l_iNextPercent = 100, f_bDone = true;
	if ( a_pcTitle || ( f_iLastProgress != l_iNextStep )
			|| ( f_iLastPercent != l_iNextPercent )
			|| ( f_iLastMinute != l_iNextMinute )
			|| ( f_iLastSecond != l_iNextSecond ))
		{
		refresh ( );
		bar ( a_pcTitle );
		if ( f_bEstimate )
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					( a_dStep ? ( ( char const * ) l_oLeft ) : "(?\?:?\?:?\?)" ),
					( char const * ) l_oStoper,	l_iNextPercent, "%%" );
			}
		else
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					( char const * ) l_oStoper, l_iNextPercent, "%%" );
			}
		if ( f_bDone )
			strncpy ( ( ( char * ) f_oString ) + console::n_iWidth - 5, "done", 4 );
		::memset ( ( ( char * ) f_oString ) + 1, '-', l_iMaxBar );
		::memset ( ( ( char * ) f_oString ) + 1, '=', l_iNextStep );
		mvprintw ( console::n_iHeight - 1, 0, f_oString );
		f_oString = "";
		f_iLastProgress = l_iNextStep;
		f_iLastPercent = l_iNextPercent;
		f_iLastMinute = l_iNextMinute;
		f_iLastSecond = l_iNextSecond;
		::refresh ( );
		}
	return;
	M_EPILOG
	}

void HStatusBarControl::message ( int a_iAttribute,
		char const * a_pcFormat, ... )
	{
	M_PROLOG
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	if ( a_pcFormat && a_pcFormat [ 0 ] )putchar ( '\a' );
	console::c_vprintf ( f_iRowRaw, -1, a_iAttribute, a_pcFormat, l_xAp );
	va_end ( l_xAp );
	console::n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

void HStatusBarControl::message ( char const * a_pcFormat, ... )
	{
	M_PROLOG
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	if ( a_pcFormat && a_pcFormat [ 0 ] )putchar ( '\a' );
	console::c_vprintf ( f_iRowRaw, -1, M_ATTR_DATA ( ), a_pcFormat, l_xAp );
	va_end ( l_xAp );
	console::n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

void HStatusBarControl::bar ( char const * a_pcBar )
	{
	M_PROLOG
	M_SET_ATTR_DATA ( );
	if ( a_pcBar )
		{
		f_oVarTmpBuffer.format ( " %%-%ds ",
				console::n_iWidth - f_iLabelLength - ( f_bSingleLine ? 2 : 1 ) );
		f_oMessage.format ( f_oVarTmpBuffer, a_pcBar );
		}
	::mvprintw ( console::n_iHeight - 2,
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

int HStatusBarControl::process_input_normal  ( int a_iCode )
	{
	M_PROLOG
	bool l_bBackwards = false;
	int l_iCode = a_iCode;
	int l_iMode = f_iMode;
	HSearchableControl * l_poSearchableControl = NULL;
	a_iCode = 0;
	switch ( l_iCode )
		{
		case ( '\r' ):
			{
			l_bBackwards = ( f_oPrompt [ 0 ] == '?' );
			end_prompt ( );
			if ( l_iMode == D_PROMPT_MODE_COMMAND )
				f_poParent->f_oCommand = f_oString;
			else if ( l_iMode == D_PROMPT_MODE_SEARCH )
				{
				l_poSearchableControl = dynamic_cast < HSearchableControl * > ( f_poParent->f_poPreviousFocusedChild );
				if ( l_poSearchableControl )
					l_poSearchableControl->search ( f_oString, l_bBackwards );
				}
			break;
			}
		case ( '\t' ):
			{
			putchar ( '\a' );
			break;
			}
		default :
			{
			a_iCode = l_iCode;
			break;
			}
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
	f_iMode = D_PROMPT_MODE_NORMAL;
	f_oPrompt = "";
	f_iPromptLength = 0;
	f_poParent->f_poFocusedChild = f_poParent->f_poPreviousFocusedChild;
	f_poParent->f_poStatusBar->kill_focus ( );
	f_poParent->f_poFocusedChild->set_focus ( -1 );
	return;
	M_EPILOG
	}


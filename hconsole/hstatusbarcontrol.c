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
#include "hstatusbarcontrol.h"

HStatusBarControl::HStatusBarControl ( HWindow * a_poParent,
		const char * a_pcLabel )
								 : HControl ( a_poParent, -2, 0, 0, 255, a_pcLabel ),
									HEditControl ( a_poParent, - 2, 0, 0, 255, a_pcLabel )
	{
	M_PROLOG
	f_iPromptLength = 0;
	f_bDone = false;
	f_iLastProgress = -1;
	f_iLastPercent = -1;
	f_dProgressSize = 1;
	return;
	M_EPILOG
	}

HStatusBarControl::~HStatusBarControl ( void )
	{
	M_PROLOG
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

void HStatusBarControl::init_progress ( double a_dMax, const char * a_pcTitle,
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

void HStatusBarControl::update_progress ( double a_dStep, const char * a_pcTitle )
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
	if ( a_pcTitle )f_oMessage = a_pcTitle;
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
		::move ( console::n_iHeight - 2,
				f_iLabelLength - ( f_bSingleLine ? 0 : 1 ) );
		f_oVarTmpBuffer.format ( " %%-%ds ",
				console::n_iWidth - f_iLabelLength - ( f_bSingleLine ? 2 : 1 ) );
		f_oString.format ( f_oVarTmpBuffer, ( const char * ) f_oMessage );
		cprintf ( f_oString );
		if ( f_bEstimate )
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					( a_dStep ? ( ( const char * ) l_oLeft ) : "(?\?:?\?:?\?)" ),
					( const char * ) l_oStoper,	l_iNextPercent, "%%" );
			}
		else
			{
			f_oVarTmpBuffer.format ( "|%%-%ds|%%s[%%3d%%s]", l_iMaxBar );
			f_oString.format ( f_oVarTmpBuffer, "-",
					( const char * ) l_oStoper, l_iNextPercent, "%%" );
			}
		if ( f_bDone )
			strncpy ( ( ( char * ) f_oString ) + console::n_iWidth - 5, "done", 4 );
		::memset ( ( ( char * ) f_oString ) + 1, '-', l_iMaxBar );
		::memset ( ( ( char * ) f_oString ) + 1, '=', l_iNextStep );
		cprintf ( f_oString );
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

void HStatusBarControl::display ( const char * a_pcMessage, int a_iAttribute )
	{
	M_PROLOG
	int l_iAttribute = ( f_bEnabled ?  (  f_bFocused ? f_iFocusedAttribute
				: f_iEnabledAttribute ) : f_iDisabledAttribute );
	if ( a_iAttribute > -1 )l_iAttribute = a_iAttribute;
	console::c_printf ( f_iRowRaw, 0, l_iAttribute, a_pcMessage );
	return;
	M_EPILOG
	}


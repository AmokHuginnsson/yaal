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

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hstatusbarcontrol.h"

#include "../hcore/hexception.h"

HStatusBarControl::HStatusBarControl ( HWindow * a_poParent,
		const char * a_pcLabel )
								 : HControl ( a_poParent, -2, 0, 0, 255, a_pcLabel ),
												HEditControl ( a_poParent, - 2, 0, 0, 255, a_pcLabel )
	{
	M_PROLOG
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

double HStatusBarControl::init_progress ( const char * a_pcTitle, int a_iMax )
	{
	M_PROLOG
	double l_dDelta = 0;
	f_iProgress = a_iMax;
	f_oString = a_pcTitle;
	return ( l_dDelta );
	M_EPILOG
	}

void HStatusBarControl::update_progress ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HStatusBarControl::display ( const char * a_pcMessage, int a_iAttribute )
	{
	M_PROLOG
	int l_iAttribute = ( f_bEnabled ?  (  f_bFocused ? f_iFocusedAttribute
				: f_iEnabledAttribute ) : f_iDisabledAttribute );
	if ( a_iAttribute > -1 )l_iAttribute = a_iAttribute;
	console::c_printf ( f_iRowRaw, 0, D_FG_BRIGHTRED, a_pcMessage );
	return;
	M_EPILOG
	}


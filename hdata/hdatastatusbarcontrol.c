/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatastatusbarcontrol.c - this file is integral part of `stdhapi' project.

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
#include "hconsole/console.h"
#include "hdatastatusbarcontrol.h"

using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;

namespace stdhapi
{

namespace hdata
{

HDataStatusBarControl::HDataStatusBarControl ( HDataWindow * a_poParent,
		char const * a_pcLabel, int a_iStatusBarAttribute )
								 : HControl ( a_poParent, - 2, 0, 255, 0, a_pcLabel ),
									HStatusBarControl ( NULL, NULL, a_iStatusBarAttribute )
	{
	M_PROLOG
	f_iLabelLength += 3; /* for "[+]" */
	return;
	M_EPILOG
	}

void HDataStatusBarControl::draw_label ( void )
	{
	M_PROLOG
	HStatusBarControl::draw_label ( );
	if ( static_cast < HDataWindow * > ( f_poParent )->is_modified ( ) )
		{
		M_SET_ATTR_SHORTCUT ( );
		mvprintw ( n_iHeight - 2, f_iLabelLength - 4, "[+]" );
		M_SET_ATTR_DATA ( );
		}
	return;
	M_EPILOG
	}

}

}


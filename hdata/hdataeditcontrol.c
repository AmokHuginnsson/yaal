/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditcontrol.c - this file is integral part of `stdhapi' project.

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
#include "../hconsole/console.h"
#include "hdataeditcontrol.h"

HDataEditControl::HDataEditControl( HDataWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iWidth, int a_iHeight,
		const char * a_pcLabel, int a_iBufferSize, const char * a_pcValue,
		const char * a_pcMask, bool a_bReplace, bool a_bRightAligned,
		bool a_bMultiLine, bool a_bPassword, int a_iMaxHistoryLevel,
		bool a_bDrawLabel, int a_iDisabledAttribute,
		int a_iEnabledAttribute, int a_iFocusedAttribute )
								:	HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcLabel, a_bDrawLabel, a_iDisabledAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute ),
									HEditControl( a_poParent,
										a_iRow, a_iColumn, a_iWidth, a_iHeight,
										a_pcLabel, a_iBufferSize, a_pcValue,
										a_pcMask, a_bReplace, a_bRightAligned,
										a_bMultiLine, a_bPassword, a_iMaxHistoryLevel,
										a_bDrawLabel, a_iDisabledAttribute,
										a_iEnabledAttribute, a_iFocusedAttribute )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataEditControl::process_input ( int a_iCode )
	{
	M_PROLOG
	bool l_bNoChange = false;
	switch ( a_iCode )
		{
		case ( '\t' ):
		case ( '\r' ):
		case ( KEY_LEFT ):
		case ( KEY_RIGHT ):
		case ( D_KEY_CTRL_('a') ):
		case ( KEY_HOME ):
		case ( D_KEY_CTRL_('e') ):
		case ( KEY_END ):
		case ( KEY_IC ):
		case ( D_KEY_META_('f') ):
		case ( D_KEY_META_('b') ):
			{
			l_bNoChange = true;
			break;
			}
		case ( KEY_PPAGE ):
		case ( KEY_NPAGE ):
		case ( KEY_UP ):
		case ( KEY_DOWN ):
			{
			if ( f_bMultiLine )
				l_bNoChange = true;
			break;
			}
		case ( KEY_BS ):
			{
			if ( ! ( f_iControlOffset + f_iCursorPosition ) )
				l_bNoChange = true;
			break;
			}
		case ( KEY_DELETE ):
			{
			if ( ! ( ( char * ) f_oString ) [ 0 ] )
				l_bNoChange = true;
			break;
			}
		}
	a_iCode = HEditControl::process_input ( a_iCode );
	if ( ! ( a_iCode || l_bNoChange ) )
		( ( HDataWindow * ) f_poParent )->set_modified ( );
	return ( a_iCode );
	M_EPILOG
	}


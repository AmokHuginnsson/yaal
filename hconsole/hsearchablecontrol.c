/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.c - this file is integral part of `stdhapi' project.

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
#include "hsearchablecontrol.h"
#include "hwindow.h"
#include "hconsole.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HSearchableControl::HSearchableControl ( bool a_bSearchable )
									: HControl ( NULL, 0, 0, 0, 0, NULL )
	{
	M_PROLOG
	f_bFiltered = false;
	f_bSearchActived = false;
	f_bBackwards = false;
	f_bSearchable = a_bSearchable;
	return;
	M_EPILOG
	}

HSearchableControl::~HSearchableControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSearchableControl::search ( const HString & a_oPattern, bool a_bBackwards )
	{
	M_PROLOG
	unsigned short int l_hFlag = 'f';
	f_bSearchActived = ! f_oPattern.parse ( a_oPattern, & l_hFlag,
			sizeof ( l_hFlag ) / sizeof ( unsigned short int ) );
	if ( ! f_bSearchActived )
		f_poParent->status_bar ( )->message ( f_oPattern.error ( ) );
	else
		{
		f_bFiltered = ( l_hFlag & 0xff00 ) ? true : false;
		f_bBackwards = a_bBackwards;
		if ( f_bBackwards )
			go_to_match_previous ( );
		else
			go_to_match ( );
		}
	refresh ( );
	return;
	M_EPILOG
	}

void HSearchableControl::highlight ( int a_iRow, int a_iColumn,
		int a_iCurrent, bool a_bCurrent )
	{
	M_PROLOG
	int l_iHighlightLength = 0, l_iCtr = 0;
	char l_cStopChar, * l_pcHighlightStart = NULL;
	l_pcHighlightStart = f_oVarTmpBuffer;
	while ( ( l_pcHighlightStart = f_oPattern.matches ( l_pcHighlightStart,
			l_iHighlightLength ) ) )
		{
		if ( ( f_bFocused && ( ( a_iCurrent != l_iCtr ) || ! a_bCurrent ) )
				|| ( ! f_bFocused && ( a_iCurrent == l_iCtr ) && a_bCurrent ) )
			set_attr ( n_iAttributeSearchHighlight >> 8 );
		else
			set_attr ( n_iAttributeSearchHighlight );
		l_cStopChar = l_pcHighlightStart [ l_iHighlightLength ];
		l_pcHighlightStart [ l_iHighlightLength ] = 0;
		::mvprintw ( a_iRow, a_iColumn
				+ ( l_pcHighlightStart - ( char * ) f_oVarTmpBuffer ),
				l_pcHighlightStart );
		l_pcHighlightStart [ l_iHighlightLength ] = l_cStopChar;
		l_pcHighlightStart ++;
		l_iCtr ++;
		}
	return;
	M_EPILOG
	}

}

}


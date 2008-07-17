/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hsearchablecontrol.h"
#include "hwindow.h"
#include "hconsole.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HSearchableControl::HSearchableControl ( bool a_bSearchable )
									: HControl ( NULL, 0, 0, 0, 0, NULL ),
	f_bSearchable ( a_bSearchable ), f_bSearchActived ( false ),
	f_bFiltered ( false ), f_bBackwards ( false ), f_oPattern()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HSearchableControl::~HSearchableControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSearchableControl::search( HString const& a_oPattern, bool a_bBackwards )
	{
	M_PROLOG
	int short unsigned l_uhFlag = 'f';
	f_bSearchActived = ! f_oPattern.parse( a_oPattern, &l_uhFlag,
			sizeof ( l_uhFlag ) / sizeof ( int short unsigned ) );
	if ( ! f_bSearchActived )
		f_poParent->status_bar()->message ( f_oPattern.error().raw() );
	else
		{
		f_bFiltered = ( l_uhFlag & 0xff00 ) ? true : false;
		f_bBackwards = a_bBackwards;
		if ( f_bBackwards )
			go_to_match_previous();
		else
			go_to_match();
		}
	schedule_refresh();
	return;
	M_EPILOG
	}

void HSearchableControl::highlight( int a_iRow, int a_iColumn,
		int a_iCurrent, bool a_bCurrent )
	{
	M_PROLOG
	int l_iHighlightLength = 0, l_iCtr = 0;
	char const* l_pcHighlightStart = NULL;
	HConsole& cons = HCons::get_instance();
	l_pcHighlightStart = f_oVarTmpBuffer.raw();
	while ( ( l_pcHighlightStart = const_cast<char*>( f_oPattern.matches( l_pcHighlightStart,
			&l_iHighlightLength ) ) ) )
		{
		if ( ( f_bFocused && ( ( a_iCurrent != l_iCtr ) || ! a_bCurrent ) )
				|| ( ! f_bFocused && ( a_iCurrent == l_iCtr ) && a_bCurrent ) )
			cons.set_attr( n_iAttributeSearchHighlight >> 8 );
		else
			cons.set_attr( n_iAttributeSearchHighlight );
		cons.c_mvprintf( a_iRow,
				static_cast<int>( a_iColumn + ( l_pcHighlightStart - f_oVarTmpBuffer.raw() ) ),
				"%.*s", l_iHighlightLength, l_pcHighlightStart );
		l_pcHighlightStart ++;
		l_iCtr ++;
		}
	return;
	M_EPILOG
	}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditcontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hconsole/console.h"
#include "hdataeditcontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataEditControl::HDataEditControl( HDataWindow * a_poParent,
		int a_iRow, int a_iColumn, int a_iHeight, int a_iWidth,
		char const * a_pcLabel, int a_iBufferSize, char const * a_pcValue,
		char const * a_pcMask, bool a_bReplace, bool a_bMultiLine,
		bool a_bReadOnly, bool a_bRightAligned,
		bool a_bPassword, int a_iMaxHistoryLevel )
								:	HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcLabel ),
									HEditControl( a_poParent,
										a_iRow, a_iColumn, a_iHeight, a_iWidth,
										a_pcLabel, a_iBufferSize, a_pcValue,
										a_pcMask, a_bReplace, a_bMultiLine,
										a_bReadOnly, a_bRightAligned,
										a_bPassword, a_iMaxHistoryLevel ),
									HDataControl()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataEditControl::do_process_input ( int a_iCode )
	{
	M_PROLOG
	bool l_bNoChange = false;
	HDataWindow * l_poWindow = NULL;
	switch ( a_iCode )
		{
		case ( '\t' ):
		case ( '\r' ):
		case ( KEY_CODES::D_LEFT ):
		case ( KEY_CODES::D_RIGHT ):
		case ( KEY < 'a' >::ctrl ):
		case ( KEY_CODES::D_HOME ):
		case ( KEY < 'e' >::ctrl ):
		case ( KEY_CODES::D_END ):
		case ( KEY_CODES::D_INSERT ):
		case ( KEY < 'f' >::meta ):
		case ( KEY < 'b' >::meta ):
			l_bNoChange = true;
		break;
		case ( KEY_CODES::D_PAGE_UP ):
		case ( KEY_CODES::D_PAGE_DOWN ):
		case ( KEY_CODES::D_UP ):
		case ( KEY_CODES::D_DOWN ):
			if ( f_bMultiLine )
				l_bNoChange = true;
		break;
		case ( KEY_CODES::D_BACKSPACE ):
			if ( ! ( f_iControlOffset + f_iCursorPosition ) )
				l_bNoChange = true;
		break;
		case ( KEY_CODES::D_DELETE ):
			if ( f_oString.is_empty() )
				l_bNoChange = true;
		break;
		default:
		break;
		}
	a_iCode = HEditControl::do_process_input ( a_iCode );
	if ( ! ( a_iCode || l_bNoChange ) )
		{
		l_poWindow = dynamic_cast<HDataWindow*>( f_poParent );
		M_ASSERT ( l_poWindow );
		l_poWindow->set_modified();
		}
	return ( a_iCode );
	M_EPILOG
	}

}

}


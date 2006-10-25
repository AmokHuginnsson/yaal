/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrollist.cxx - this file is integral part of `yaal' project.

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
#include "hcontrollist.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HControlList::HControlList ( void ) : f_oList ( ), f_oFocused ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HControl * HControlList::next_enabled ( char a_cShorcut )
	{
	M_PROLOG
	bool l_bLoop = true;
	HControl * l_poControlOld = f_oList.present ( );
	HControl * l_poControlNew = NULL;
	do
		{
		f_oList.to_tail ( );
		l_poControlNew = f_oList.present ( );
		l_bLoop = l_poControlNew->set_focus ( a_cShorcut );
		if ( l_poControlOld == l_poControlNew )
			l_bLoop = false;
		}
	while ( l_bLoop );
	if ( l_poControlNew != l_poControlOld )
		{
		l_poControlOld->kill_focus ( );
		l_poControlNew->set_focus ( - 1 );
		}
	return ( l_poControlNew );
	M_EPILOG
	}

void HControlList::select ( HControl * a_poControl )
	{
	M_PROLOG
	if ( f_oList.present ( ) == a_poControl )
		return;
	control_list_t::HIterator end = f_oList.end ( );
	for ( control_list_t::HIterator it = f_oList.begin ( ); it != end; ++ it )
		{
		if ( *it == a_poControl )
			{
			f_oFocused = it;
			return;
			}
		}
	M_THROW ( "bogus object", reinterpret_cast < int > ( a_poControl ) );
	M_EPILOG
	}

}

}


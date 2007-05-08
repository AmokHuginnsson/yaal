/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatastatusbarcontrol.cxx - this file is integral part of `yaal' project.

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
#include "hconsole/console.h"
#include "hdatastatusbarcontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;

namespace yaal
{

namespace hdata
{

HDataStatusBarControl::HDataStatusBarControl ( HDataWindow * a_poParent,
		char const * a_pcLabel, int a_iStatusBarAttribute )
								 : HControl ( a_poParent, - 2, 0, 2, - 1, a_pcLabel ),
									HStatusBarControl ( NULL, NULL, a_iStatusBarAttribute )
	{
	M_PROLOG
	f_iLabelLength += 3; /* for "[+]" */
	return;
	M_EPILOG
	}

void HDataStatusBarControl::do_draw_label ( void )
	{
	M_PROLOG
	HDataWindow * l_poWindow = NULL;
	HStatusBarControl::draw_label();
	l_poWindow = dynamic_cast < HDataWindow * > ( f_poParent );
	M_ASSERT ( l_poWindow );
	if ( l_poWindow->is_modified() )
		{
		set_attr_shortcut();
		c_mvprintf ( n_iHeight - 2, f_iLabelLength - 4, "[+]" );
		set_attr_data();
		}
	return;
	M_EPILOG
	}

}

}


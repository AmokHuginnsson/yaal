/*
---           hdata.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdata.cxx - this file is integral part of hdata.h project.

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

#include <cstdio>
#include <cstdlib>

#include "hcore/hexception.h"
M_VCSID( "$Id$" )
#include "hcore/hcore.h"
#include "hconsole/hconsole.h"
#include "tools/tools.h"
#include "dbwrapper/dbwrapper.h"
#include "hdata.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

static char const g_pcDynamicLinkerPath[]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_hdata_banner( void )
	{
	fprintf ( stdout, "\thdata\n" );
	return;
	}

extern "C"
int yaal_hdata_main( int, char** ) __attribute__(( __noreturn__ ));
int yaal_hdata_main( int, char** )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		yaal_tools_banner();
		yaal_dbwrapper_banner();
		yaal_hconsole_banner();
		yaal_hdata_banner();
		}
	::exit( 0 );
	}

}

}


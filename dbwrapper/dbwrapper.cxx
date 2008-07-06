/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	dbwrapper.cxx - this file is integral part of `yaal' project.

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

#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "config.h"

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hcore/hstring.h"
#include "hcore/rc_file.h"
#include "hcore/hcore.h"
#include "hconsole/hconsole.h"
#include "tools/tools.h"
#include "dbwrapper.h"
#include "db_driver_loader.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;

namespace yaal
{

namespace dbwrapper
{

struct DB_DRIVER
	{
	enum
		{
		D_NONE = 0,
		D_SQLITE3,
		D_SQLITE,
		D_MYSQL, 
		D_POSTGRESQL,
		D_ORACLE
		};
	};

void dbwrapper_error ( void );
void dbwrapper_exit ( void ) __attribute__  ((noreturn));

int	n_iDataBaseDriver = 0;

OOption n_psVariables[] =
	{
		{ NULL, D_VOID, NULL, 0, OOption::D_NONE, NULL, NULL, NULL }
	};

bool set_dbwrapper_variables( HString& a_roOption, HString& a_roValue )
	{
	M_PROLOG
	if ( ! strcasecmp ( a_roOption, "set_env" ) )
		set_env ( a_roValue );
	else if ( ! strcasecmp ( a_roOption, "log_mask" ) )
		{
		if ( ! strcasecmp ( a_roValue, "LOG_SQL" ) )
			HLog::f_lLogMask |= LOG_TYPE::D_SQL;
		else
			return ( true );
		}
	else if ( ! strcasecmp ( a_roOption, "data_base_driver" ) )
		{
		if ( ! ( strcasecmp ( a_roValue, "none" )
					&& strcasecmp ( a_roValue, "null" ) ) )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_NONE;
		else if ( a_roValue == "SQLite3" )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_SQLITE3;
		else if ( a_roValue == "SQLite" )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_SQLITE;
		else if ( a_roValue == "MySQL" )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_MYSQL;
		else if ( a_roValue == "PostgreSQL" )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_POSTGRESQL;
		else if ( a_roValue == "Oracle" )
			dbwrapper::n_iDataBaseDriver = DB_DRIVER::D_ORACLE;
		else
			{
			log ( LOG_TYPE::D_ERROR ) << "Error: `" << a_roValue;
			log << "' is unknown driver." << endl;
			exit ( 1 );
			}
		}
	else
		return ( true );
	return ( false );
	M_EPILOG
	}

class HDBWrapperInitDeinit
	{
public:
	HDBWrapperInitDeinit( void );
	} initDeinit;

HDBWrapperInitDeinit::HDBWrapperInitDeinit( void )
	{
	M_PROLOG
	rc_file::process_rc_file( "yaal", "dbwrapper", n_psVariables,
			set_dbwrapper_variables );
	return;
	M_EPILOG
	}

extern char const * g_pcDone;

static char const g_pcDynamicLinkerPath[]
	__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;

void yaal_dbwrapper_banner( void )
	{
	fprintf ( stdout, "\tdbwrapper\n" );
	return;
	}

extern "C"
void yaal_dbwrapper_main( void ) __attribute__(( __noreturn__ ));
void yaal_dbwrapper_main( void )
	{
	if ( g_pcDynamicLinkerPath[ 0 ] )
		{
		yaal_hcore_banner();
		yaal_tools_banner();
		yaal_dbwrapper_banner();
		}
	exit( 0 );
	}

}

}


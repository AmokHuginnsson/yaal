/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dlfcn.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"
#include "tools/hplugin.hxx"
#include "db_driver_loader.hxx"
#include "dbwrapper.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal
{

namespace dbwrapper
{

namespace
{

char const* const etag = "Error: Data base request (";
char const* const eend = ") while no driver loaded.";

}

char const* const g_pcDone = "done.\r\n";

static char const* g_ppcDriver[ 7 ] =
	{
	NULL,
	LIB_PREFIX"sqlite3_driver."LIB_EXT,
	LIB_PREFIX"sqlite_driver."LIB_EXT,
	LIB_PREFIX"mysql_driver."LIB_EXT,
	LIB_PREFIX"postgresql_driver."LIB_EXT,
	LIB_PREFIX"oracle_driver."LIB_EXT,
	NULL
	};

HPlugin n_oDBDriver;

/* Null driver */

void* null_db_connect( char const*, char const*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_connect)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

void null_db_disconnect( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_disconnect)" << eend << endl;
	return;
	M_EPILOG
	}

int null_db_errno( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_errno)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char const* null_db_error( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_error)" << eend << endl;
	return ( _( "null database driver loaded" ) );
	M_EPILOG
	}

void* null_db_query( void*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_query)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

void null_db_unquery( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(db_unquery)" << eend << endl;
	return;
	M_EPILOG
	}

char* null_rs_get( void*, int long, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_get)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

int null_rs_fields_count( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_fields_count)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_records_count( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(dbrs_records_count)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_id( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(dbrs_id)" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char* null_rs_column_name( void*, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "(rs_column_name)" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

/* end of null-dummy driver */

void dbwrapper_error( void )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = dlerror();
	log( LOG_TYPE::ERROR ) << l_oMessage << endl;
	::fprintf( stderr, "(%s), ", l_oMessage.raw() );
	return;
	M_EPILOG
	}

void dbwrapper_exit ( void ) __attribute__  ((noreturn));
void dbwrapper_exit ( void )
	{
	::fprintf( stderr, "failed.\r\n" );
	exit ( 1 );
	}

void load_driver( void );
void* autoloader_db_connect( char const* a_pcDataBase,
		char const* a_pcLogin, char const* a_pcPassword )
	{
	M_PROLOG
	load_driver();
	return ( db_connect( a_pcDataBase, a_pcLogin, a_pcPassword ) );
	M_EPILOG
	}

void load_driver( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	errno = 0;
	fprintf ( stderr, "Loading dynamic database driver ... " );
	if ( dbwrapper::n_iDataBaseDriver )
		{
		l_iCtr = dbwrapper::n_iDataBaseDriver;
		try
			{
			n_oDBDriver.load( g_ppcDriver [ l_iCtr ++ ] );
			}
		catch ( HPluginException& )
			{
			}
		while ( ! n_oDBDriver.is_loaded() && g_ppcDriver[ l_iCtr ] )
			{
			if ( ( l_iCtr == dbwrapper::n_iDataBaseDriver ) && l_iCtr ++ )
				continue;
			dbwrapper_error();
			try
				{
				n_oDBDriver.load( g_ppcDriver [ l_iCtr ++ ] );
				}
			catch ( HPluginException& )
				{
				}
			}
		if ( ! n_oDBDriver.is_loaded() )
			dbwrapper_exit();
		else
			{
			log ( LOG_TYPE::NOTICE ) << "Loading [" << g_ppcDriver [ l_iCtr - 1 ];
			log << "] driver." << endl;
			}
		fprintf ( stderr, g_pcDone );
		fprintf ( stderr, "Linking symbols ... " );
		try
			{
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_disconnect", db_disconnect );
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_errno", db_errno );
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_error", db_error );
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_query", db_query );
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_unquery", db_unquery );
			n_oDBDriver.resolve( SYMBOL_PREFIX"rs_get", rs_get );
			n_oDBDriver.resolve( SYMBOL_PREFIX"rs_fields_count", rs_fields_count );
			n_oDBDriver.resolve( SYMBOL_PREFIX"dbrs_records_count", dbrs_records_count );
			n_oDBDriver.resolve( SYMBOL_PREFIX"dbrs_id", dbrs_id );
			n_oDBDriver.resolve( SYMBOL_PREFIX"rs_column_name", rs_column_name );
			n_oDBDriver.resolve( SYMBOL_PREFIX"db_connect", db_connect );
			}
		catch ( HPluginException& )
			{
			M_THROW( _( "cannot load database driver" ), n_iDataBaseDriver );
			}
		if ( db_connect != autoloader_db_connect )
			fprintf( stderr, g_pcDone );
		}
	else
		{
		db_connect = null_db_connect;
		db_disconnect = null_db_disconnect;
		db_errno = null_db_errno;
		db_error = null_db_error;
		db_query = null_db_query;
		db_unquery = null_db_unquery;
		rs_get = null_rs_get;
		rs_fields_count = null_rs_fields_count;
		dbrs_records_count = null_dbrs_records_count;
		dbrs_id = null_dbrs_id;
		rs_column_name = null_rs_column_name;
		}
	return;
	M_EPILOG
	}

/* Driver autoloaders ... */

void autoloader_db_disconnect( void* a_pvDataBase )
	{
	M_PROLOG
	load_driver();
	db_disconnect( a_pvDataBase );
	return;
	M_EPILOG
	}

int autoloader_db_errno( void* a_pvDataBase )
	{
	M_PROLOG
	load_driver();
	return ( db_errno( a_pvDataBase ) );
	M_EPILOG
	}

char const* autoloader_db_error( void* a_pvDataBase )
	{
	M_PROLOG
	load_driver();
	return ( db_error( a_pvDataBase ) );
	M_EPILOG
	}

void* autoloader_db_query( void* a_pvDataBase, char const* a_pcQuery )
	{
	M_PROLOG
	load_driver();
	return ( db_query( a_pvDataBase, a_pcQuery ) );
	M_EPILOG
	}

void autoloader_db_unquery( void* a_pvDataBase )
	{
	M_PROLOG
	load_driver();
	db_unquery( a_pvDataBase );
	return;
	M_EPILOG
	}

char* autoloader_rs_get( void* a_pvResult, int long a_iRow, int a_iCol )
	{
	M_PROLOG
	load_driver();
	return ( rs_get( a_pvResult, a_iRow, a_iCol ) );
	M_EPILOG
	}

int autoloader_rs_fields_count( void* a_pvResult )
	{
	M_PROLOG
	load_driver();
	return ( rs_fields_count( a_pvResult ) );
	M_EPILOG
	}

int long autoloader_dbrs_records_count( void* a_pvDataBase,
		void* a_pvResult )
	{
	M_PROLOG
	load_driver();
	return ( dbrs_records_count( a_pvDataBase, a_pvResult ) );
	M_EPILOG
	}

int long autoloader_dbrs_id( void* a_pvDataBase, void* a_pvResult )
	{
	M_PROLOG
	load_driver();
	return ( dbrs_id( a_pvDataBase, a_pvResult ) );
	M_EPILOG
	}

char* autoloader_rs_column_name( void* a_pvResult, int a_iColumn )
	{
	M_PROLOG
	load_driver();
	return ( rs_column_name( a_pvResult, a_iColumn ) );
	M_EPILOG
	}

db_connect_t db_connect = autoloader_db_connect;
db_disconnect_t db_disconnect = autoloader_db_disconnect;
db_errno_t db_errno = autoloader_db_errno;
db_error_t db_error = autoloader_db_error;
db_query_t db_query = autoloader_db_query;
db_unquery_t db_unquery = autoloader_db_unquery;
rs_get_t rs_get = autoloader_rs_get;
rs_fields_count_t rs_fields_count = autoloader_rs_fields_count;
dbrs_records_count_t dbrs_records_count = autoloader_dbrs_records_count;
dbrs_id_t dbrs_id = autoloader_dbrs_id;
rs_column_name_t rs_column_name = autoloader_rs_column_name;

/* end of driver autoloader section */

} /* namespace dbwrapper */

} /* namespace yaal */


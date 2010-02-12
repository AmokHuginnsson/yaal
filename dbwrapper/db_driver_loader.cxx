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

#include <cstdlib>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "tools/hplugin.hxx"
#include "tools/hstringstream.hxx"
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
	"default",
	"null",
	LIB_PREFIX"sqlite3_driver."LIB_EXT,
	LIB_PREFIX"postgresql_driver."LIB_EXT,
	LIB_PREFIX"mysql_driver."LIB_EXT,
	LIB_PREFIX"oracle_driver."LIB_EXT,
	NULL
	};

drivers_t n_oDBDrivers;

/* Null driver */

void* null_db_connect( char const*, char const*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_connect" << eend << endl;
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

int null_dbrs_errno( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_errno" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char const* null_dbrs_error( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_error" << eend << endl;
	return ( _( "null database driver loaded" ) );
	M_EPILOG
	}

void* null_db_query( void*, char const* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_query" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

void null_rs_unquery( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "db_unquery" << eend << endl;
	return;
	M_EPILOG
	}

char* null_rs_get( void*, int long, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "rs_get" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

int null_rs_fields_count( void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "rs_fields_count" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_records_count( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "dbrs_records_count" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

int long null_dbrs_id( void*, void* )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "dbrs_id" << eend << endl;
	return ( 0 );
	M_EPILOG
	}

char* null_rs_column_name( void*, int )
	{
	M_PROLOG
	log( LOG_TYPE::ERROR ) << etag << "rs_column_name" << eend << endl;
	return ( NULL );
	M_EPILOG
	}

/* end of null-dummy driver */

void dbwrapper_error( void )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = HPlugin().error_message( 0 );
	log( LOG_TYPE::ERROR ) << l_oMessage << endl;
	::fprintf( stderr, "(%s), ", l_oMessage.raw() );
	return;
	M_EPILOG
	}

void dbwrapper_exit( void ) __attribute__ ((noreturn));
void dbwrapper_exit( void )
	{
	cerr << "failed." << endl;
	exit( 1 );
	}

ODBConnector const* try_load_driver( ODBConnector::DRIVER::enum_t const& driverId_ )
	{
	M_PROLOG
	M_ENSURE( ( driverId_ >= ODBConnector::DRIVER::SQLITE3 ) && ( driverId_ < ODBConnector::DRIVER::TERMINATOR ) );
	drivers_t::const_iterator it = n_oDBDrivers.find( driverId_ );
	driver_t driver;
	if ( it == n_oDBDrivers.end() ) /* given driver has not been loaded yet */
		{
		try
			{
			driver = make_pair( HPlugin::ptr_t( new HPlugin() ), ODBConnector() );
			log( LOG_TYPE::NOTICE ) << "Loading [" << g_ppcDriver[ driverId_ + 1 ] << "] driver ... ";
			driver.first->load( g_ppcDriver[ driverId_ + 1 ] );
			cerr << "(linking symbols ...) " << flush;
			driver.first->resolve( SYMBOL_PREFIX"db_disconnect", driver.second.db_disconnect );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_errno", driver.second.dbrs_errno );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_error", driver.second.dbrs_error );
			driver.first->resolve( SYMBOL_PREFIX"db_query", driver.second.db_query );
			driver.first->resolve( SYMBOL_PREFIX"rs_unquery", driver.second.rs_unquery );
			driver.first->resolve( SYMBOL_PREFIX"rs_get", driver.second.rs_get );
			driver.first->resolve( SYMBOL_PREFIX"rs_fields_count", driver.second.rs_fields_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_records_count", driver.second.dbrs_records_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_id", driver.second.dbrs_id );
			driver.first->resolve( SYMBOL_PREFIX"rs_column_name", driver.second.rs_column_name );
			driver.first->resolve( SYMBOL_PREFIX"db_connect", driver.second.db_connect );
			it = n_oDBDrivers.insert( make_pair( driverId_, driver ) ).first;
			}
		catch ( HPluginException& e )
			{
			log( LOG_TYPE::NOTICE ) << "fail." << endl;
			HStringStream reason;
			reason << _( "cannot load database driver: " ) << e.what();
			M_THROW( reason.string(), n_eDataBaseDriver );
			}
		if ( driver.first->is_loaded() )
			log( LOG_TYPE::NOTICE ) << "success." << endl;
		else
			{
			dbwrapper_error();
			dbwrapper_exit();
			}
		}
	return ( &it->second.second );
	M_EPILOG
	}

ODBConnector const* load_driver( ODBConnector::DRIVER::enum_t const& driverId_ )
	{
	M_PROLOG
	errno = 0;
	cerr << "Using dynamic database driver [" << g_ppcDriver[ driverId_ + 1 ] << "] ... " << flush;
	ODBConnector const* pConnector( NULL );
	if ( driverId_ != ODBConnector::DRIVER::NONE )
		{
		if ( driverId_ == ODBConnector::DRIVER::DEFAULT )
			{
			for ( int i = 1; i < ODBConnector::DRIVER::TERMINATOR; ++ i )
				{
				if ( ( pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( i ) ) ) )
					break;
				}
			}
		else
			pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( driverId_ ) );
		}
	if ( pConnector->db_connect != null_db_connect )
		cerr << g_pcDone << flush;
	return ( pConnector );
	M_EPILOG
	}

ODBConnector::ODBConnector( void )
	: db_connect( null_db_connect ),
	db_disconnect( null_db_disconnect ),
	dbrs_errno( null_dbrs_errno ),
	dbrs_error( null_dbrs_error ),
	db_query( null_db_query ),
	rs_unquery( null_rs_unquery ),
	rs_get( null_rs_get ),
	rs_fields_count( null_rs_fields_count ),
	dbrs_records_count( null_dbrs_records_count ),
	dbrs_id( null_dbrs_id ),
	rs_column_name( null_rs_column_name )
	{
	}

/* end of driver null section */

} /* namespace dbwrapper */

} /* namespace yaal */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <cstdio>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "db_driver_loader.hxx"
#include "hcore/memory.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "tools/hplugin.hxx"
#include "tools/hstringstream.hxx"
#include "dbwrapper.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace dbwrapper {

char const _done_[] = "done.\r\n";

static char const* _driver_[ 8 ] = {
	"default",
	LIB_PREFIX "yaal_sqlite3_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_postgresql_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_mysql_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_firebird_driver" LIB_INFIX "." LIB_EXT,
	LIB_PREFIX "yaal_oracle_driver" LIB_INFIX "." LIB_EXT,
	nullptr
};

drivers_t _dBDrivers_;

namespace {

void dbwrapper_error( void ) {
	M_PROLOG
	HString message;
	message = HPlugin().error_message( 0 );
	hcore::log( LOG_LEVEL::ERROR ) << message << endl;
	cerr << "(" << message << "), ";
	return;
	M_EPILOG
}

void dbwrapper_exit( void ) __attribute__ ((noreturn));
void dbwrapper_exit( void ) {
	cerr << "failed." << endl;
	exit( 1 );
}

ODBConnector const* try_load_driver( ODBConnector::DRIVER::enum_t driverId_ ) {
	M_PROLOG
	M_ENSURE( ( driverId_ >= ODBConnector::DRIVER::SQLITE3 ) && ( driverId_ < ODBConnector::DRIVER::TERMINATOR ) );
	drivers_t::const_iterator it = _dBDrivers_.find( driverId_ );
	driver_t driver;
	if ( it == _dBDrivers_.end() ) /* given driver has not been loaded yet */ {
		try {
			driver = make_pair( make_pointer<HPlugin>(), make_pointer<ODBConnector>() );
			hcore::log( LOG_LEVEL::NOTICE ) << "Loading [" << _driver_[ driverId_ + 1 ] << "] driver ... ";
			driver.first->load( _driver_[ driverId_ + 1 ] );
			hcore::log( LOG_LEVEL::NOTICE ) << "(linking symbols ...) " << flush;
			driver.first->try_resolve( SYMBOL_PREFIX"driver_init", driver.second->driver_init );
			driver.first->try_resolve( SYMBOL_PREFIX"driver_cleanup", driver.second->driver_cleanup );
			driver.first->resolve( SYMBOL_PREFIX"db_disconnect", driver.second->db_disconnect );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_errno", driver.second->dbrs_errno );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_error", driver.second->dbrs_error );
			driver.first->resolve( SYMBOL_PREFIX"db_fetch_query_result", driver.second->db_fetch_query_result );
			driver.first->resolve( SYMBOL_PREFIX"rs_free_query_result", driver.second->rs_free_query_result );
			driver.first->resolve( SYMBOL_PREFIX"db_query", driver.second->db_query );
			driver.first->resolve( SYMBOL_PREFIX"db_prepare_query", driver.second->db_prepare_query );
			driver.first->resolve( SYMBOL_PREFIX"query_bind", driver.second->query_bind );
			driver.first->resolve( SYMBOL_PREFIX"query_execute", driver.second->query_execute );
			driver.first->resolve( SYMBOL_PREFIX"query_free", driver.second->query_free );
			driver.first->resolve( SYMBOL_PREFIX"rs_free_cursor", driver.second->rs_free_cursor );
			driver.first->resolve( SYMBOL_PREFIX"rs_get", driver.second->rs_get );
			driver.first->resolve( SYMBOL_PREFIX"rs_next", driver.second->rs_next );
			driver.first->resolve( SYMBOL_PREFIX"rs_get_field", driver.second->rs_get_field );
			driver.first->resolve( SYMBOL_PREFIX"rs_fields_count", driver.second->rs_fields_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_records_count", driver.second->dbrs_records_count );
			driver.first->resolve( SYMBOL_PREFIX"dbrs_id", driver.second->dbrs_id );
			driver.first->resolve( SYMBOL_PREFIX"rs_column_name", driver.second->rs_column_name );
			driver.first->resolve( SYMBOL_PREFIX"db_connect", driver.second->db_connect );
			char const** tlq = nullptr; /* msvcxx bug */
			driver.first->resolve( SYMBOL_PREFIX"TABLE_LIST_QUERY", tlq );
			driver.second->_tableListQuery = *tlq;
			char const** clq = nullptr; /* msvcxx bug */
			driver.first->resolve( SYMBOL_PREFIX"COLUMN_LIST_QUERY", clq );
			driver.second->_columnListQuery = *clq;
			int* cni( nullptr );
			driver.first->resolve( SYMBOL_PREFIX"COLUMN_NAME_INDEX", cni );
			driver.second->_columnNameIndex = *cni;
			it = _dBDrivers_.insert( make_pair( driverId_, driver ) ).first;
			driver.second->init();
		} catch ( HPluginException& e ) {
			hcore::log( LOG_LEVEL::NOTICE ) << "fail." << endl;
			HStringStream reason;
			reason << _( "cannot load database driver: " ) << e.what();
			M_THROW( reason.string(), _dataBaseDriver_ );
		}
		if ( driver.first->is_loaded() ) {
			hcore::log( LOG_LEVEL::NOTICE ) << "success." << endl;
		} else {
			dbwrapper_error();
			dbwrapper_exit();
		}
	}
	return ( it->second.second.raw() );
	M_EPILOG
}

}

ODBConnector const* load_driver( ODBConnector::DRIVER::enum_t driverId_ ) {
	M_PROLOG
	errno = 0;
	hcore::log( LOG_LEVEL::NOTICE ) << "Using dynamic database driver [" << _driver_[ driverId_ + 1 ] << "] ... " << flush;
	ODBConnector const* pConnector( nullptr );
	if ( driverId_ == ODBConnector::DRIVER::DEFAULT ) {
		for ( int i( 0 ); i < ODBConnector::DRIVER::TERMINATOR; ++ i ) {
			if ( ( pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( i ) ) ) ) {
				break;
			}
		}
	} else {
		pConnector = try_load_driver( static_cast<ODBConnector::DRIVER::enum_t>( driverId_ ) );
	}
	if ( ! pConnector || ( pConnector->db_connect != nullptr ) ) {
		hcore::log( LOG_LEVEL::NOTICE ) << _done_ << flush;
	}
	return ( pConnector );
	M_EPILOG
}

/*
 * ODBConnector implementation is in this file instead of db_driver.cxx because
 * build system discards implementation files with file name ending with _driver.cxx.
 */

ODBConnector::ODBConnector( void )
	: driver_init( nullptr )
	, driver_cleanup( nullptr )
	, db_connect( nullptr )
	, db_disconnect( nullptr )
	, dbrs_errno( nullptr )
	, dbrs_error( nullptr )
	, db_fetch_query_result( nullptr )
	, rs_free_query_result( nullptr )
	, db_query( nullptr )
	, db_prepare_query( nullptr )
	, query_bind( nullptr )
	, query_execute( nullptr )
	, query_free( nullptr )
	, rs_free_cursor( nullptr )
	, rs_get( nullptr )
	, rs_next( nullptr )
	, rs_get_field( nullptr )
	, rs_fields_count( nullptr )
	, dbrs_records_count( nullptr )
	, dbrs_id( nullptr )
	, rs_column_name( nullptr )
	, _tableListQuery( nullptr )
	, _columnListQuery( nullptr )
	, _columnNameIndex( 0 ) {
}

ODBConnector::~ODBConnector( void ) {
	M_PROLOG
	cleanup();
	return;
	M_DESTRUCTOR_EPILOG
}

void ODBConnector::cleanup( void ) {
	M_PROLOG
	if ( driver_cleanup ) {
		driver_cleanup();
	}
	return;
	M_EPILOG
}

void ODBConnector::init( void ) {
	M_PROLOG
	if ( driver_init ) {
		driver_init();
	}
	return;
	M_EPILOG
}

yaal::hcore::HString transform_sql( yaal::hcore::HString sql_, place_holder_generator_t gen_ ) {
	M_PROLOG
	int placeholerNo( 1 );
	int len( static_cast<int>( sql_.get_length() ) );
	bool inSingleQuote( false );
	bool inDoubleQuote( false );
	for ( int pos( 0 ), i( 0 ); i < len; ++ i, ++ pos ) {
		code_point_t currChar( sql_[pos] );
		if ( inSingleQuote ) {
			if ( currChar == '\'' ) {
				inSingleQuote = false;
			}
		} else if ( inDoubleQuote ) {
			if ( currChar == '"' ) {
				inDoubleQuote = false;
			}
		} else {
			if ( currChar == '\'' ) {
				inSingleQuote = true;
			} else if ( currChar == '"' ) {
				inDoubleQuote = true;
			} else if ( currChar == '?' ) {
				HString placeholer( gen_( placeholerNo ++ ) );
				M_ENSURE( placeholer.get_length() > 0 );
				if ( placeholer != "?" ) {
					sql_.replace( pos, 1, placeholer );
					pos += ( static_cast<int>( placeholer.get_length() ) - 1 );
				}
			}
		}
	}
	return ( sql_ );
	M_EPILOG
}

} /* namespace dbwrapper */

} /* namespace yaal */


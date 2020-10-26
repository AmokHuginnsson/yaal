/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/hlog.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/hcore.hxx"
#include "tools/tools.hxx"
#include "dbwrapper.hxx"
#include "db_driver_loader.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace dbwrapper {

namespace {

ODBConnector::DRIVER::enum_t db_driver_from_string( HString const& dbType_ ) {
	M_PROLOG
	ODBConnector::DRIVER::enum_t driver( ODBConnector::DRIVER::DEFAULT );
	if ( ! stricasecmp( dbType_, "SQLite3" ) ) {
		driver = ODBConnector::DRIVER::SQLITE3;
	} else if ( ! stricasecmp( dbType_, "PostgreSQL" ) ) {
		driver = ODBConnector::DRIVER::POSTGRESQL;
	} else if ( ! stricasecmp( dbType_, "MySQL" ) ) {
		driver = ODBConnector::DRIVER::MYSQL;
	} else if ( ! stricasecmp( dbType_, "Firebird" ) ) {
		driver = ODBConnector::DRIVER::FIREBIRD;
	} else if ( ! stricasecmp( dbType_, "Oracle" ) ) {
		driver = ODBConnector::DRIVER::ORACLE;
	} else {
		throw HDataBaseException( "unsupported database type: " + dbType_ );
	}
	return ( driver );
	M_EPILOG
}

}

namespace util {

HDataBase::ptr_t connect( yaal::hcore::HString const& dsn_ ) {
	M_PROLOG
	char const SCHEMA_SEPARATOR[] = "://";
	int dbTypeEnd( static_cast<int>( dsn_.find( SCHEMA_SEPARATOR ) ) );
	if ( dbTypeEnd == HString::npos ) {
		throw HDataBaseException( "bad database uri: " + dsn_ );
	}
	HString dbType( dsn_.left( dbTypeEnd ) );
	dbType.lower();
	ODBConnector::DRIVER::enum_t driver( db_driver_from_string( dbType ) );
	HString connectionString = dsn_.substr( dbTypeEnd + static_cast<int>( sizeof ( SCHEMA_SEPARATOR ) ) - 1 );
	int userPassEnd( static_cast<int>( connectionString.find( '@'_ycp ) ) );
	HString userName;
	HString password;
	HString dbHostSpec;
	if ( userPassEnd != HString::npos ) {
		HString userPass( connectionString.left( userPassEnd ) );
		int userEnd( static_cast<int>( userPass.find( ':'_ycp ) ) );
		userName = userPass.left( userEnd );
		if ( userEnd != HString::npos ) {
			password = userPass.substr( userEnd + 1 );
		}
		dbHostSpec = connectionString.substr( userPassEnd + 1 );
	} else {
		dbHostSpec = connectionString;
	}
	int hostSpecEnd( static_cast<int>( dbHostSpec.find( '/'_ycp ) ) );
	HString hostSpec;
	HString database;
	if ( hostSpecEnd != HString::npos ) {
		hostSpec = dbHostSpec.left( hostSpecEnd );
		database = dbHostSpec.substr( hostSpecEnd + 1 );
	} else {
		throw HDataBaseException( "bad database uri: " + dsn_ );
	}
	HDataBase::ptr_t db( HDataBase::get_connector( driver ) );
	db->connect( database, userName, password, hostSpec );
	return ( db );
	M_EPILOG
}

}

bool _logSQL_ = false;
ODBConnector::DRIVER::enum_t _dataBaseDriver_ = ODBConnector::DRIVER::DEFAULT;

namespace {

bool set_dbwrapper_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! stricasecmp( option_, "set_env" ) ) {
		decode_set_env( value_ );
	} else if ( ! stricasecmp( option_, "data_base_driver" ) ) {
		try {
			dbwrapper::_dataBaseDriver_ = db_driver_from_string( value_ );
		} catch ( HDataBaseException const& ) {
			hcore::log( LOG_LEVEL::ERROR ) << "Error: `" << value_;
			hcore::log << "' is unknown driver." << endl;
			exit( 1 );
		}
	} else {
		fail = true;
	}
	return ( ! fail );
	M_EPILOG
}

}

class HDBWrapperInitDeinit {
public:
	HDBWrapperInitDeinit( void );
} initDeinit;

HDBWrapperInitDeinit::HDBWrapperInitDeinit( void ) {
	M_PROLOG
	try {
		yaal_options()(
			HProgramOptionsHandler::HOption()
			.long_form( "log_sql" )
			.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::OPTIONAL )
			.description( "log content of SQL queries" )
			.recipient( _logSQL_ )
		);
		yaal_options().process_rc_file( "dbwrapper", set_dbwrapper_variables );
	} catch ( HException const& e ) {
		fprintf( stderr, "Failed to initialize yaal-dbwrapper: %s\n", e.what() );
		exit( 1 );
	}
	return;
	M_EPILOG
}

void banner( void ) {
	::printf( "\tdbwrapper\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_dbwrapper_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_dbwrapper_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::dbwrapper::banner();
		::printf( "\n" );
		::exit( 0 );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_dbwrapper_main( int, char** );
extern "C"
int yaal_dbwrapper_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <errno.h>

#include "config.hxx"

#ifdef HAVE_POSTGRESQL_LIBPQ_FE_H
#	include <postgresql/libpq-fe.h>
# define HAVE_POSTGRESQL_H 1
#elif defined ( HAVE_LIBPQ_FE_H )
#	include <libpq-fe.h>
# define HAVE_POSTGRESQL_H 1
#else /* HAVE_LIBPQ_FE_H */
#	warning "No libpq-fe.h header available."
#endif /* not HAVE_LIBPQ_FE_H */

#ifdef HAVE_POSTGRESQL_H

#include "hcore/memory.hxx"
#include "hcore/harray.hxx"
#include "hcore/system.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

struct OPostgreSQLResult {
	typedef HArray<char const*> params_t;
	typedef std::atomic<u64_t> statement_id_source_t;
	static statement_id_source_t _statementIdSource;
	ODBLink& _link;
	int _useCount;
	PGresult* _result;
	HUTF8String _id;
	HUTF8String _query;
	int _index;
	int _total;
	params_t _params;
	bool _randomAccess;
	bool _requireSync;
	OPostgreSQLResult( ODBLink& link_ )
		: _link( link_ )
		, _useCount( 1 )
		, _result( nullptr )
		, _id( to_string( system::getpid() ).append( ":" ).append( ++ _statementIdSource ) )
		, _query()
		, _index( 0 )
		, _total( 0 )
		, _params()
		, _randomAccess( false )
		, _requireSync( false ) {
		return;
	}
private:
	OPostgreSQLResult( OPostgreSQLResult const& );
	OPostgreSQLResult& operator = ( OPostgreSQLResult const& );
};

OPostgreSQLResult::statement_id_source_t OPostgreSQLResult::_statementIdSource = { 0 };

namespace {
HString placeholder_generator( int no_ ) {
	static int const PLACEHOLDER_SIZE( 16 );
	char placeholder[PLACEHOLDER_SIZE];
	snprintf( placeholder, PLACEHOLDER_SIZE, "$%d", no_ );
	return ( placeholder );
}
}

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SELECT table_name, table_schema"
		" FROM information_schema.tables"
		" WHERE table_schema NOT IN ('pg_catalog', 'information_schema');" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "SELECT a.attnum, a.attname AS field, t.typname AS type,"
		" a.attlen AS length, a.atttypmod AS length_var,"
		" a.attnotnull AS not_null, a.atthasdef as has_default"
		" FROM pg_class c, pg_attribute a, pg_type t"
		" WHERE c.relname = '%s'"
		" AND a.attnum > 0"
		" AND a.attrelid = c.oid"
		" AND a.atttypid = t.oid"
		" ORDER BY a.attnum;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 1;

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& dataBase_,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& host_ ) {
	PGconn* connection( nullptr );
	HUTF8String dataBase( dataBase_ );
	HUTF8String login( login_ );
	HUTF8String password( password_ );
	HString::size_type portIdx( host_.find_last( ':'_ycp ) );
	HUTF8String host( host_.begin(), portIdx != HString::npos ? host_.begin() + portIdx : host_.end() );
	HUTF8String port( portIdx != HString::npos ? host_.begin() + portIdx + 1 : host_.end(), host_.end() );
	dbLink_._conn = connection = PQsetdbLogin(
		! host.is_empty() ? host.c_str() : nullptr,
		! port.is_empty() ? port.c_str() : nullptr,
		nullptr /* options */,
		nullptr /* debugging tty */,
		dataBase.c_str(),
		login.c_str(),
		password.c_str()
	);
	if ( PQstatus( connection ) == CONNECTION_OK ) {
		dbLink_._valid = true;
	}
	return ( dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		PQfinish( static_cast<PGconn*>( dbLink_._conn ) );
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	int err( 0 );
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( result_ ) );
	if ( pr && pr->_result ) {
		ExecStatusType status = PQresultStatus( pr->_result );
		err = ( ( status == PGRES_COMMAND_OK ) || ( status == PGRES_TUPLES_OK ) ) ? 0 : status;
	} else {
		ConnStatusType connStatus( PQstatus( static_cast<PGconn*>( dbLink_._conn ) ) );
		err = ( connStatus == CONNECTION_OK ) ? 0 : connStatus;
	}
	return ( err );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	M_ASSERT( dbLink_._conn );
	return ( result_ ? ::PQresultErrorMessage( static_cast<OPostgreSQLResult*>( result_ )->_result ) : ::PQerrorMessage( static_cast<PGconn*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OPostgreSQLResult* result( new ( memory::yaal ) OPostgreSQLResult( dbLink_ ) );
	result->_result = PQexec( static_cast<PGconn*>( dbLink_._conn ), query_ );
	result->_randomAccess = true;
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( pr->_randomAccess );
	M_ASSERT( pr->_useCount > 0 );
	-- pr->_useCount;
	if ( ! pr->_useCount ) {
		PQclear( pr->_result );
		M_SAFE( delete pr );
	}
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OPostgreSQLResult* result( new ( memory::yaal ) OPostgreSQLResult( dbLink_ ) );
	result->_randomAccess = false;
	PGconn* conn( static_cast<PGconn*>( dbLink_._conn ) );
	PGresult* r( ::PQprepare( conn, result->_id.c_str(), query_, 0, nullptr ) );
	result->_result = ::PQdescribePrepared( conn, result->_id.c_str() );
	::PQsendQueryPrepared( conn, result->_id.c_str(), 0, nullptr, nullptr, nullptr, 0 );
	::PQclear( r );
/*	::PQsetSingleRowMode( conn ); */
	return ( result );
}

M_EXPORT_SYMBOL void* db_prepare_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OPostgreSQLResult* result( new ( memory::yaal ) OPostgreSQLResult( dbLink_ ) );
	result->_randomAccess = false;
	result->_query = transform_sql( query_, &placeholder_generator );
	return ( result );
}

M_EXPORT_SYMBOL void query_bind( ODBLink&, void*, int, yaal::hcore::HUTF8String const& );
M_EXPORT_SYMBOL void query_bind( ODBLink&, void* data_, int argNo_, yaal::hcore::HUTF8String const& param_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	if ( argNo_ >= static_cast<int>( pr->_params.get_size() ) ) {
		pr->_params.resize( argNo_ );
	}
	pr->_params[argNo_ - 1] = param_.c_str();
	return;
}

M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* );
M_EXPORT_SYMBOL void* query_execute( ODBLink& dbLink_, void* data_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	PGconn* conn( static_cast<PGconn*>( dbLink_._conn ) );
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	bool ok( true );
	if ( ! pr->_result ) {
		PGresult* r( ::PQprepare( conn, pr->_id.c_str(), pr->_query.c_str(), static_cast<int>( pr->_params.get_size() ), nullptr ) );
		ExecStatusType status = PQresultStatus( r );
		::PQclear( r );
		if ( ( ( status == PGRES_COMMAND_OK ) || ( status == PGRES_TUPLES_OK ) ) ? 0 : status ) {
			ok = false;
			pr->_result = r;
		}
	}
	if ( ok ) {
		if ( pr->_result ) {
			::PQclear( pr->_result );
		}
		pr->_result = ::PQdescribePrepared( conn, pr->_id.c_str() );
		::PQsendQueryPrepared(
			conn, pr->_id.c_str(),
			static_cast<int>( pr->_params.get_size() ),
			! pr->_params.is_empty() ? pr->_params.data() : nullptr,
			nullptr, nullptr, 0
		);
		++ pr->_useCount;
		pr->_requireSync = true;
	}
	return ( pr );
}

M_EXPORT_SYMBOL void query_free( ODBLink&, void* );
M_EXPORT_SYMBOL void query_free( ODBLink&, void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( ! pr->_randomAccess );
	M_ASSERT( pr->_useCount > 0 );
	-- pr->_useCount;
	if ( ! pr->_useCount ) {
		::PQclear( pr->_result );
		M_SAFE( delete pr );
	}
	return;
}

M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( ! pr->_randomAccess );
	M_ASSERT( pr->_useCount > 0 );
	-- pr->_useCount;
	if ( ! pr->_useCount ) {
		::PQclear( pr->_result );
		M_SAFE( delete pr );
	}
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	return ( ! ::PQgetisnull( pr->_result, static_cast<int>( row_ ), column_ ) ? ::PQgetvalue( pr->_result, static_cast<int>( row_ ), column_ ) : nullptr );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	M_ASSERT( ! pr->_randomAccess );
	bool gotMore( false );
	if ( ++ pr->_index < pr->_total ) {
		gotMore = true;
	} else {
		::PQclear( pr->_result );
		gotMore = ( pr->_result = ::PQgetResult( static_cast<PGconn*>( pr->_link._conn ) ) ) != nullptr;

		if ( gotMore ) {
			pr->_index = 0;
			pr->_total = ::PQntuples( pr->_result );
			gotMore = pr->_index < pr->_total;
		} else
			pr->_total = 0;
	}
	return ( gotMore );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	return ( ! ::PQgetisnull( pr->_result, pr->_index, field_ ) ? ::PQgetvalue( pr->_result, pr->_index, field_ ) : nullptr );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( data_ ) );
	return ( ::PQnfields( pr->_result ) );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* dataR_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( dataR_ ) );
	if ( pr->_requireSync ) {
		::PQclear( pr->_result );
		pr->_result = ::PQgetResult( static_cast<PGconn*>( pr->_link._conn ) );
		pr->_requireSync = false;
	}
	int long count( -1 );
	if ( pr->_result ) {
		char const* tmp( ::PQcmdTuples( pr->_result ) );
		if ( tmp && tmp [ 0 ] ) {
			count = ::strtol( tmp, nullptr, 10 );
		} else {
			count = ::PQntuples( pr->_result );
		}
	}
	return ( count );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	PGresult* result( PQexec( static_cast<PGconn*>( dbLink_._conn ), "SELECT lastval();" ) );
	int long id( -1 );
	if ( result ) {
		char const* value( PQgetvalue( result, 0, 0 ) );
		if ( value )
			id = ::strtol( value, nullptr, 10 );
		PQclear( result );
	}
	return ( id );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OPostgreSQLResult* pr( static_cast<OPostgreSQLResult*>( dataR_ ) );
	return ( ::PQfname( pr->_result, field_ ) );
}

int yaal_postgresql_driver_main( int, char** );
int yaal_postgresql_driver_main( int, char** ) {
	return ( 0 );
}

}

#endif /* #ifdef HAVE_POSTGRESQL_H */


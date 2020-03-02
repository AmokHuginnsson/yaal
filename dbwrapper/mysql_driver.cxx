/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#ifdef HAVE_MYSQL_MYSQL_H

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "config.hxx"

#ifdef __HOST_OS_TYPE_FREEBSD__
#	define TO_BE_INCLUDED_LATER 1
#endif /* __HOST_OS_TYPE_FREEBSD__ */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <mysql/mysql.h>
#pragma GCC diagnostic pop

#include "hcore/memory.hxx"
#include "dbwrapper/db_driver.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/hregex.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

namespace {

char const AUTODETECT_CHARSET[] = "autodetect";
HString _clientCharacterSet_ = AUTODETECT_CHARSET;

}

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SHOW TABLES;" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "SHOW COLUMNS FROM %s;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 0;

struct OMySQLResult {
	struct OField {
		typedef trait::strip_pointer<decltype ( MYSQL_BIND::is_null )>::type mysql_bool_t;
		int long unsigned _length;
		mysql_bool_t _isNull;
	};
	typedef HArray<MYSQL_BIND> binds_t;
	typedef HArray<OField> field_meta_t;
	ODBLink& _link;
	int _useCount;
	MYSQL_STMT* _statement;
	binds_t _params;
	binds_t _results;
	field_meta_t _fields;
	HChunk _buffer;
	MYSQL_RES* _result;
	MYSQL_ROW _row;
	bool _randomAccess;
	OMySQLResult( ODBLink& link_ )
		: _link( link_ )
		, _useCount( 1 )
		, _statement( nullptr )
		, _params()
		, _results()
		, _fields()
		, _buffer()
		, _result( nullptr )
		, _row()
		, _randomAccess( false ) {
		return;
	}
private:
	OMySQLResult( OMySQLResult const& );
	OMySQLResult& operator = ( OMySQLResult const& );
};

void* mysql_db_prepare_query( ODBLink&, char const* );
void* mysql_query_execute( ODBLink&, void* );
void mysql_query_free( void*, bool );

M_EXPORT_SYMBOL void driver_init( void );
M_EXPORT_SYMBOL void driver_init( void ) {
	M_PROLOG
	log << "Initializing MySQL driver." << endl;
	M_ENSURE( mysql_library_init( 0, nullptr, nullptr ) == 0 );
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL void driver_cleanup( void );
M_EXPORT_SYMBOL void driver_cleanup( void ) {
	M_PROLOG
#ifndef __HOST_OS_TYPE_FEDORA__
	/*
	 * mysql_library_end() causes crash on Fedora via:
	 *
	 * ERR_clear_error()
	 * vio_end()
	 * mysql_library_end()
	 *
	 * stack, it is probably an invalid free in OpenSSL library.
	 *
	 */
	log << "Cleaning up after MySQL driver." << endl;
	mysql_library_end();
#endif /* #ifndef __HOST_OS_TYPE_FEDORA__ */
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& dataBase_,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& host_ ) {
	MYSQL* mySQL( nullptr );
	dbLink_._conn = mySQL = mysql_init( nullptr );
	do {
		if ( ! mySQL ) {
			break;
		}
		HUTF8String dataBase( dataBase_ );
		HUTF8String login( login_ );
		HUTF8String password( password_ );
		HString::size_type portIdx( host_.find_last( ':'_ycp ) );
		HUTF8String host( host_.begin(), portIdx != HString::npos ? host_.begin() + portIdx : host_.end() );
		bool isUnixSocket( host_.find( '/'_ycp ) != HString::npos );
		HString portStr( portIdx != HString::npos ? host_.substr( portIdx + 1 ) : "" );
		int unsigned port( 0 );
		if ( ! ( isUnixSocket || portStr.is_empty() ) ) {
			try {
				port = lexical_cast<int unsigned>( portStr );
			} catch ( HException const& ) {
				break;
			}
		}
		int unsigned protocol( host_.is_empty() || isUnixSocket ? MYSQL_PROTOCOL_SOCKET : MYSQL_PROTOCOL_TCP );
		HUTF8String utf8( _clientCharacterSet_ );
		if ( mysql_options( mySQL, MYSQL_OPT_PROTOCOL, &protocol ) != 0 ) {
			break;
		}
		if ( mysql_options( mySQL, MYSQL_SET_CHARSET_NAME, utf8.c_str() ) != 0 ) {
			break;
		}
		if (
			mysql_real_connect(
				mySQL,
				! ( isUnixSocket || host.is_empty() ) ? host.c_str() : nullptr,
				login.c_str(),
				password.c_str(),
				dataBase.c_str(),
				port,
				isUnixSocket && ! host.is_empty() ? host.c_str() : nullptr,
				CLIENT_IGNORE_SPACE | CLIENT_IGNORE_SIGPIPE
			) == nullptr
		) {
			break;
		}
		dbLink_._valid = true;
	} while ( false );
	return ( dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	if ( dbLink_._conn ) {
		mysql_close( static_cast<MYSQL*>( dbLink_._conn ) );
		mysql_thread_end();
		dbLink_.clear();
	}
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* ) {
	return ( static_cast<int>( ::mysql_errno( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* ) {
	return ( ::mysql_error( static_cast<MYSQL*>( dbLink_._conn ) ) );
}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OMySQLResult* result( new ( memory::yaal ) OMySQLResult( dbLink_ ) );
	::mysql_query( static_cast<MYSQL*>( dbLink_._conn ), query_ );
	result->_randomAccess = true;
	result->_result = ::mysql_store_result( static_cast<MYSQL*>( dbLink_._conn ) );
	return ( result );
}

M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	M_ASSERT( pr->_useCount > 0 );
	-- pr->_useCount;
	if ( ! pr->_useCount ) {
		::mysql_free_result( pr->_result );
		M_SAFE( delete pr );
	}
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	void* result( mysql_db_prepare_query( dbLink_, query_ ) );
	mysql_query_execute( dbLink_, result );
	return ( result );
}

void* mysql_db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	OMySQLResult* query( new ( memory::yaal ) OMySQLResult( dbLink_ ) );
	query->_statement = mysql_stmt_init( static_cast<MYSQL*>( dbLink_._conn ) );
	query->_randomAccess = false;
	HRegex isSelect( "\\bSELECT\\b", HRegex::COMPILE::IGNORE_CASE );
	HRegex isInsert( "\\bINSERT\\b", HRegex::COMPILE::IGNORE_CASE );
	if ( isSelect.matches( query_ ) && ! isInsert.matches( query_ ) ) {
		int long unsigned type( CURSOR_TYPE_READ_ONLY );
		mysql_stmt_attr_set( query->_statement, STMT_ATTR_CURSOR_TYPE, static_cast<void*>( &type ) );
	}
	if ( ! mysql_stmt_prepare( query->_statement, query_, static_cast<int unsigned>( ::strlen( query_ ) ) ) ) {
		query->_result = mysql_stmt_result_metadata( query->_statement );
		int numFields( query->_result ? static_cast<int>( mysql_num_fields( query->_result ) ) : 0 );
		if ( numFields > 0 ) {
			query->_results.resize( numFields );
			query->_fields.resize( numFields );
			int dataLength( 0 );
			for ( int i( 0 ); i < numFields; ++ i ) {
				::memset( &query->_results[i], 0, sizeof ( OMySQLResult::binds_t::value_type ) );
				MYSQL_FIELD* field( mysql_fetch_field_direct( query->_result, static_cast<int unsigned>( i ) ) );
				query->_results[i].buffer_type = MYSQL_TYPE_STRING;
				query->_results[i].buffer_length = field->length + 1;
				query->_results[i].length = &query->_fields[i]._length;
				query->_results[i].is_null = &query->_fields[i]._isNull;
				dataLength += static_cast<int>( field->length + 1 );
			}
			query->_buffer.realloc( dataLength, HChunk::STRATEGY::EXACT );
			int offset( 0 );
			for ( int i( 0 ); i < numFields; ++ i ) {
				query->_results[i].buffer = query->_buffer.get<char>() + offset;
				offset += static_cast<int>( query->_results[i].buffer_length );
			}
		}
	}
	return ( query );
}

M_EXPORT_SYMBOL void* db_prepare_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	return ( mysql_db_prepare_query( dbLink_, query_ ) );
}

M_EXPORT_SYMBOL void query_bind( ODBLink&, void*, int, yaal::hcore::HUTF8String const& );
M_EXPORT_SYMBOL void query_bind( ODBLink&, void* data_, int argNo_, yaal::hcore::HUTF8String const& value_ ) {
	OMySQLResult* pq( static_cast<OMySQLResult*>( data_ ) );
	if ( argNo_ > static_cast<int>( pq->_params.get_size() ) ) {
		pq->_params.resize( argNo_ );
	}
	::memset( &pq->_params[argNo_ - 1], 0, sizeof ( OMySQLResult::binds_t::value_type ) );
	pq->_params[argNo_ - 1].buffer_type = MYSQL_TYPE_STRING;
	pq->_params[argNo_ - 1].buffer = const_cast<char*>( value_.c_str() );
	pq->_params[argNo_ - 1].buffer_length = static_cast<int unsigned>( value_.byte_count() );
	return;
}

void* mysql_query_execute( ODBLink&, void* data_ ) {
	OMySQLResult* pq( static_cast<OMySQLResult*>( data_ ) );
	if ( ! pq->_result ) {
		mysql_stmt_reset( pq->_statement );
		pq->_result = mysql_stmt_result_metadata( pq->_statement );
	}
	if ( ! pq->_params.is_empty() ) {
		mysql_stmt_bind_param( pq->_statement, pq->_params.data() );
	}
	if ( ! pq->_results.is_empty() ) {
		mysql_stmt_bind_result( pq->_statement, pq->_results.data() );
	}
	mysql_stmt_execute( pq->_statement );
	return ( pq );
}
M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* );
M_EXPORT_SYMBOL void* query_execute( ODBLink& dbLink_, void* data_ ) {
	OMySQLResult* pq( static_cast<OMySQLResult*>( data_ ) );
	++ pq->_useCount;
	return ( mysql_query_execute( dbLink_, data_ ) );
}

void mysql_query_free( void* data_, bool reset_ ) {
	OMySQLResult* pq( static_cast<OMySQLResult*>( data_ ) );
	M_ASSERT( pq->_useCount > 0 );
	-- pq->_useCount;
	if ( reset_ && pq->_result ) {
		::mysql_free_result( pq->_result );
		pq->_result = nullptr;
	}
	if ( ! pq->_useCount ) {
		if ( pq->_statement ) {
			::mysql_stmt_close( pq->_statement );
		}
		M_SAFE( delete pq );
	}
	return;
}

M_EXPORT_SYMBOL void query_free( ODBLink&, void* );
M_EXPORT_SYMBOL void query_free( ODBLink&, void* data_ ) {
	mysql_query_free( data_, false );
	return;
}

M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	mysql_query_free( data_, true );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	MYSQL_ROW row;
	::mysql_data_seek( static_cast<OMySQLResult*>( data_ )->_result, static_cast<my_ulonglong>( row_ ) );
	row = mysql_fetch_row( static_cast<OMySQLResult*>( data_ )->_result );
	return ( row[ column_ ] );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	return ( mysql_stmt_fetch( pr->_statement ) == 0 );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	return ( pr->_fields[field_]._isNull ? nullptr : static_cast<char const*>( pr->_results[field_].buffer ) );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OMySQLResult* pr( static_cast<OMySQLResult*>( data_ ) );
	return ( ( pr && pr->_result ) ? static_cast<int>( ::mysql_num_fields( pr->_result ) ) : 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& dbLink_, void* dataR_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OMySQLResult* pr( static_cast<OMySQLResult*>( dataR_ ) );
	if ( pr && pr->_result )
		return ( static_cast<int long>( ::mysql_num_rows( pr->_result ) ) );
	else
		return ( static_cast<int long>( ::mysql_affected_rows( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	return ( static_cast<int long>( mysql_insert_id( static_cast<MYSQL*>( dbLink_._conn ) ) ) );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	MYSQL_FIELD* field = nullptr;
	field = mysql_fetch_field_direct( static_cast<OMySQLResult*>( dataR_ )->_result, static_cast<int unsigned>( field_ ) );
	return ( field->name );
}

}

namespace {

class HMySQLInitDeinit {
public:
	HMySQLInitDeinit( void );
} mySQLInitDeinit;

HMySQLInitDeinit::HMySQLInitDeinit( void ) {
	M_PROLOG
	yaal_options()(
		HProgramOptionsHandler::HOption()
		.long_form( "client_character_set" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "character set used by mysql client" )
		.recipient( _clientCharacterSet_ )
		.argument_name( "name" )
	);
	yaal_options().process_rc_file( "mysql", nullptr );
#if defined( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME ) && ( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME == 1 )
	if ( _clientCharacterSet_ == AUTODETECT_CHARSET ) {
		_clientCharacterSet_ = MYSQL_AUTODETECT_CHARSET_NAME;
	}
#endif /* #ifdef defined( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME ) && ( HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME == 1 ) */
	char const* characterSetOverride( ::getenv( "YAAL_MYSQL_CLIENT_CHARACTER_SET" ) );
	if ( characterSetOverride ) {
		_clientCharacterSet_ = characterSetOverride;
	}
	return;
	M_EPILOG
}

}

extern "C" {

int yaal_mysql_driver_main( int, char** );
int yaal_mysql_driver_main( int, char** ) {
	return ( 0 );
}

}

#endif /* #ifdef HAVE_MYSQL_MYSQL_H */


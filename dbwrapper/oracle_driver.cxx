/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	null_driver.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "config.hxx"

#ifdef HAVE_OCI_H

#include <cstdio> /* snprintf */
#include <cstdlib> /* strtol */

#ifndef __STRICT_ANSI__
#define __STRICT_ANSI__
#endif /* #ifndef __STRICT_ANSI__ */
#include <oci.h>

#include "hcore/memory.hxx"
#include "hcore/base.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hprogramoptionshandler.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

namespace {

static char const* const _logTag_ = "Oracle: ";

HString _instanceName_;

}

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>(
		"SELECT LOWER( table_name ) FROM user_tables;" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>(
		"SELECT LOWER( column_name )"
		" FROM user_tab_columns"
		" WHERE table_name = UPPER( '%s' )"
		" ORDER BY column_id;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 0;

typedef struct {
	int _status;
	OCIEnv* _environment;
	OCIError* _error;
	OCISvcCtx* _serviceContext;
} OOracle;

struct OQuery {
	struct OFieldInfo {
		int _size;
		char* _buffer;
		sb2 _isNull;
		HString _name;
		OFieldInfo( void )
			: _size( 0 ), _buffer( NULL ), _isNull( 0 ), _name()
			{}
		OFieldInfo( OFieldInfo const& fi_ )
			: _size( fi_._size ), _buffer( fi_._buffer ), _isNull( fi_._isNull ), _name( fi_._name )
			{}
		OFieldInfo& operator = ( OFieldInfo const& fi_ ) {
			if ( &fi_ != this ) {
				_size = fi_._size;
				_buffer = fi_._buffer;
				_isNull = fi_._isNull;
				_name = fi_._name;
			}
			return ( *this );
		}
	};
	typedef HArray<OFieldInfo> field_infos_t;
	typedef HList<sb2> in_null_ind_t;
	int _useCount;
	int* _status;
	OCIError* _error;
	OCIStmt* _statement;
	field_infos_t _fieldInfos;
	HChunk _buffer;
	ub4 _mode;
	HString _sql;
	in_null_ind_t _inNullInd;
	OQuery( int* status_ )
		: _useCount( 1 ), _status( status_ ), _error( NULL ), _statement( NULL ),
		_fieldInfos(), _buffer(), _mode(), _sql(), _inNullInd()
		{}
private:
	OQuery( OQuery const& );
	OQuery& operator = ( OQuery const& );
};

void yaal_oracle_db_disconnect( ODBLink& );
void oracle_free_query( void* );
char const* yaal_oracle_rs_get( void*, int long, int );

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& /* In Oracle user name is name of schema. */,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& ) {
	OOracle* oracle( NULL );
	dbLink_._conn = oracle = memory::calloc<OOracle>( 1 );
	do {
		/*
		 * OCIEnvCreate() call should be invoked before any other OCI call and should be used
		 * instead of the OCIInitialize() and OCIEnvInit() calls.
		 * OCIInitialize() and OCIEnvInit() calls will be supported for backward compatibility.
		 */
		if ( ( oracle->_status = OCIEnvCreate( &oracle->_environment,
					OCI_DEFAULT | OCI_THREADED, NULL, NULL, NULL, NULL, 0,
					NULL ) ) != OCI_SUCCESS )
			break;
		/*
		 * Allocate error handle to use it in OCILogon()
		 */
		if ( ( oracle->_status = OCIHandleAlloc( oracle->_environment,
					reinterpret_cast<void**>( &oracle->_error ),
					OCI_HTYPE_ERROR, 0, NULL ) ) != OCI_SUCCESS )
			break;
		if ( ( oracle->_status = OCILogon( oracle->_environment,
					oracle->_error, &oracle->_serviceContext,
					reinterpret_cast<OraText const*>( login_.raw() ),
					static_cast<ub4>( login_.get_length() ),
					reinterpret_cast<OraText const*>( password_.raw() ),
					static_cast<ub4>( password_.get_length() ),
					reinterpret_cast<OraText const*>( _instanceName_.raw() ),
					static_cast<ub4>( _instanceName_.get_length() ) ) ) == OCI_SUCCESS )
			dbLink_._valid = true;
	} while ( false );
	return ( dbLink_._valid );
}

void yaal_oracle_db_disconnect( ODBLink& dbLink_ ) {
	M_ASSERT( dbLink_._conn );
	OOracle* oracle( static_cast<OOracle*>( dbLink_._conn ) );
	if ( oracle->_serviceContext )
		OCILogoff( oracle->_serviceContext, oracle->_error );
	oracle->_serviceContext = NULL;
	if ( oracle->_error )
		OCIHandleFree( oracle->_error, OCI_HTYPE_ERROR );
	oracle->_error = NULL;
	if ( oracle->_environment )
		OCIHandleFree( oracle->_environment, OCI_HTYPE_ENV );
	oracle->_environment = NULL;
	memory::free( oracle );
	return;
}
M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	yaal_oracle_db_disconnect( dbLink_ );
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn );
	int error( 0 );
	OOracle const* oracle( static_cast<OOracle const*>( dbLink_._conn ) );
	if ( ( oracle->_status != OCI_SUCCESS_WITH_INFO )
			&& ( oracle->_status != OCI_ERROR ) )
		return ( oracle->_status );
	OCIErrorGet( oracle->_error, 1, NULL, &error, NULL, 0,
			OCI_HTYPE_ERROR );
	return ( error );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn );
	sb4 code( 0 );
	static char textBuffer[ OCI_ERROR_MAXMSG_SIZE ];
	OOracle const* oracle( static_cast<OOracle const*>( dbLink_._conn ) );
	switch ( oracle->_status ) {
		case ( OCI_SUCCESS_WITH_INFO ):
		case ( OCI_ERROR ):
			OCIErrorGet( oracle->_error, 1, NULL, &code,
					reinterpret_cast<OraText*>( textBuffer ),
					OCI_ERROR_MAXMSG_SIZE - 2, OCI_HTYPE_ERROR );
		break;
		case ( OCI_NEED_DATA ):
			return ( "OCI_NEED_DATA" );
		case ( OCI_NO_DATA ):
			return ( "OCI_NO_DATA" );
		case ( OCI_INVALID_HANDLE ):
			return ( "OCI_INVALID_HANDLE" );
		case ( OCI_STILL_EXECUTING ):
			return ( "OCI_STILL_EXECUTING" );
		case ( OCI_CONTINUE ):
			return ( "OCI_CONTINUE" );
		default:
			snprintf( textBuffer, OCI_ERROR_MAXMSG_SIZE - 2,
					"Error - %d", oracle->_status );
		break;
	}
	return ( textBuffer );
}

namespace {

HString placeholder_generator( int no_ ) {
	HString placeholder;
	placeholder.format( ":%d", no_ );
	return ( placeholder );
}

void* oracle_db_prepare_query( ODBLink& dbLink_, char const* query_, ub4 mode_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OOracle* oracle( static_cast<OOracle*>( dbLink_._conn ) );
	OQuery* queryObj( new OQuery( &oracle->_status ) );
	queryObj->_mode = mode_;
	queryObj->_sql = transform_sql( query_, &placeholder_generator );
	queryObj->_sql.trim();
	queryObj->_sql.trim_right( ";" );
	oracle->_status = OCIStmtPrepare2( oracle->_serviceContext,
			&queryObj->_statement, oracle->_error,
			reinterpret_cast<OraText const*>( queryObj->_sql.raw() ),
			static_cast<ub4>( queryObj->_sql.get_length() ), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT );
	queryObj->_error = oracle->_error;
	if ( ( oracle->_status != OCI_SUCCESS )
			&& ( oracle->_status != OCI_SUCCESS_WITH_INFO ) ) {
		log( LOG_TYPE::ERROR ) << _logTag_ << __FUNCTION__ << ": failed to prepare statement." << endl;
		oracle_free_query( queryObj );
		queryObj = NULL;
	} else if ( oracle->_status == OCI_SUCCESS_WITH_INFO ) {
		log( LOG_TYPE::INFO ) << _logTag_ <<  __FUNCTION__ << ": " << dbrs_error( dbLink_, NULL ) << endl;
	}
	return ( queryObj );
}

void* oracle_query_execute( ODBLink& dbLink_, void* data_ ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OOracle* oracle( static_cast<OOracle*>( dbLink_._conn ) );
	OQuery* query( static_cast<OQuery*>( data_ ) );
	M_ASSERT( query );
	bool fail( false );
	typedef HArray<OCIParam*> params_t;
	params_t params;
	do {
		ub2 stmtType( static_cast<ub2>( -1 ) );
		if ( ( ( *query->_status ) = OCIAttrGet( query->_statement,
						OCI_HTYPE_STMT, &stmtType, 0, OCI_ATTR_STMT_TYPE,
						query->_error ) ) != OCI_SUCCESS ) {
			fail = true;
			break;
		}
		int iters( 0 );
		switch ( stmtType ) {
			case ( OCI_STMT_SELECT ): {
				iters = 0;
			} break;
			case ( OCI_STMT_UPDATE ):
			case ( OCI_STMT_INSERT ):
			case ( OCI_STMT_DELETE ): {
				iters = 1;
			} break;
			case ( OCI_STMT_CREATE ):
			case ( OCI_STMT_DROP ):
			case ( OCI_STMT_ALTER ):
			case ( OCI_STMT_BEGIN ):
			case ( OCI_STMT_DECLARE ): {
				iters = 0;
			} break;
			default: {
				log( LOG_TYPE::ERROR ) << _logTag_ <<  __FUNCTION__ << ": " << "unknown statement type: " << stmtType << endl;
				fail = true;
				break;
			}
		};
		if ( fail ) {
			break;
		}
		oracle->_status = OCIStmtExecute( oracle->_serviceContext,
				query->_statement, oracle->_error, static_cast<ub4>( iters ), 0,
				NULL, NULL,
				OCI_DEFAULT | OCI_COMMIT_ON_SUCCESS | ( iters == 0 ? query->_mode : 0 ) );
		if ( ( oracle->_status != OCI_SUCCESS )
				&& ( oracle->_status != OCI_SUCCESS_WITH_INFO ) ) {
			fail = true;
			break;
		} else if ( oracle->_status == OCI_SUCCESS_WITH_INFO ) {
			log( LOG_TYPE::INFO ) << _logTag_ <<  __FUNCTION__ << ": " << dbrs_error( dbLink_, NULL ) << endl;
		}
		int fc( -1 );
		if ( ( ( *query->_status ) = OCIAttrGet( query->_statement,
						OCI_HTYPE_STMT, &fc, 0, OCI_ATTR_PARAM_COUNT,
						query->_error ) ) != OCI_SUCCESS ) {
			fail = true;
			break;
		}
		if ( fc > 0 ) {
			params.resize( fc, NULL );
			query->_fieldInfos.resize( fc );
			int maxRowSize( 0 );
			for ( int i( 0 ); i < fc; ++ i ) {
				OQuery::OFieldInfo& fi( query->_fieldInfos[i] );
				if ( ( ( *query->_status ) = OCIParamGet( query->_statement,
								OCI_HTYPE_STMT, query->_error,
								reinterpret_cast<void**>( &params[i] ), static_cast<ub4>( i + 1 ) ) ) == OCI_SUCCESS ) {
					if ( ( ( *query->_status ) = OCIAttrGet( params[i],
									OCI_DTYPE_PARAM, &fi._size, 0, OCI_ATTR_DATA_SIZE,
									query->_error ) ) == OCI_SUCCESS ) {
						maxRowSize += fi._size;
					} else {
						fail = true;
						break;
					}
					int nameLength( 0 );
					text* name( NULL );
					if ( ( ( *query->_status ) = OCIAttrGet( params[i],
									OCI_DTYPE_PARAM, &name,
									reinterpret_cast<ub4*>( &nameLength ),
									OCI_ATTR_NAME, query->_error ) ) == OCI_SUCCESS ) {
						if ( nameLength >= 0 ) {
							name[ nameLength ] = 0;
							fi._name = reinterpret_cast<char const*>( name );
						} else {
							fail = true;
							break;
						}
					} else {
						fail = true;
						break;
					}
				} else {
					fail = true;
					break;
				}
			}
			if ( fail )
				break;
			query->_buffer.realloc( maxRowSize + fc, HChunk::STRATEGY::EXACT );
			char* ptr( query->_buffer.get<char>() );
			int offset( 0 );
			for ( int i( 0 ); i < fc; ++ i ) {
				OQuery::OFieldInfo& fi( query->_fieldInfos[i] );
				OCIDefine* result( NULL );
				if ( ( ( *query->_status ) = OCIDefineByPos( query->_statement,
								&result, query->_error, static_cast<ub4>( i + 1 ), fi._buffer = ptr + offset, fi._size + 1,
								SQLT_STR, &fi._isNull, NULL, NULL, OCI_DEFAULT ) ) == OCI_SUCCESS ) {
					offset += ( fi._size + 1 );
				} else {
					fail = true;
					break;
				}
			}
			if ( fail )
				break;
		} else if ( iters == 0 ) {
			fail = true;
			break;
		}
	} while ( false );
	for ( params_t::iterator it( params.begin() ), endIt( params.end() ); it != endIt; ++ it ) {
		if ( *it )
			OCIDescriptorFree( *it, OCI_DTYPE_PARAM );
	}
	if ( fail ) {
		log( LOG_TYPE::ERROR ) << _logTag_ << __FUNCTION__ << ": failed to execute statement." << endl;
	}
	return ( query );
}

}
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	return ( oracle_query_execute( dbLink_, oracle_db_prepare_query( dbLink_, query_, OCI_STMT_SCROLLABLE_READONLY ) ) );
}

void oracle_free_query( void* data_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	M_ASSERT( query->_useCount > 0 );
	-- query->_useCount;
	if ( ! query->_useCount ) {
		if ( ( ( *query->_status ) == OCI_SUCCESS )
				|| ( ( *query->_status ) == OCI_SUCCESS_WITH_INFO ) )
			( *query->_status ) = OCIStmtRelease( query->_statement,
					query->_error, NULL, 0, OCI_DEFAULT );
		else
			OCIStmtRelease( query->_statement,
					NULL, NULL, 0, OCI_DEFAULT );
		M_SAFE( delete query );
	}
	return;
}
M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	oracle_free_query( data_ );
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	return ( oracle_query_execute( dbLink_, oracle_db_prepare_query( dbLink_, query_, 0 ) ) );
}

M_EXPORT_SYMBOL void* db_prepare_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	return ( oracle_db_prepare_query( dbLink_, query_, 0 ) );
}

M_EXPORT_SYMBOL void query_bind( ODBLink&, void*, int, yaal::hcore::HString const& );
M_EXPORT_SYMBOL void query_bind( ODBLink& dbLink_, void* data_, int attrNo_, yaal::hcore::HString const& value_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	OCIBind* dummy( NULL );
	query->_inNullInd.push_back( 0 );
	if ( ( ( *query->_status ) = OCIBindByPos( query->_statement, &dummy, query->_error,
			static_cast<ub4>( attrNo_ ), const_cast<char*>( value_.c_str() ), static_cast<sb4>( value_.get_size() ) + 1,
			SQLT_STR, &query->_inNullInd.back(), NULL, NULL, 0, NULL, OCI_DEFAULT ) ) != OCI_SUCCESS ) {
		log( LOG_TYPE::ERROR ) << _logTag_ <<  __FUNCTION__ << ": " << dbrs_error( dbLink_, query ) << endl;
	}
	return;
}

M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* );
M_EXPORT_SYMBOL void* query_execute( ODBLink& dbLink_, void* data_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	++ query->_useCount;
	return ( oracle_query_execute( dbLink_, data_ ) );
}

M_EXPORT_SYMBOL void query_free( ODBLink&, void* );
M_EXPORT_SYMBOL void query_free( ODBLink&, void* data_ ) {
	oracle_free_query( data_ );
	return;
}

M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	oracle_free_query( data_ );
	return;
}

char const* yaal_oracle_rs_get( void* data_, int long row_, int column_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	OQuery::OFieldInfo& fi( query->_fieldInfos[column_] );
	char const* ptr( NULL );
	if ( ( ( *query->_status ) = OCIStmtFetch2( query->_statement,
					query->_error, 1, OCI_FETCH_ABSOLUTE, static_cast<sb4>( row_ + 1 ),
					OCI_DEFAULT ) ) == OCI_SUCCESS )
		ptr = fi._buffer;
	return ( fi._isNull ? NULL : ptr );
}
M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	return ( yaal_oracle_rs_get( data_, row_, column_ ) );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	return ( ( ( *query->_status ) = OCIStmtFetch2( query->_statement,
					query->_error, 1, OCI_FETCH_NEXT, 0 /* Ignored for non-scrollable statements. */,
					OCI_DEFAULT ) ) == OCI_SUCCESS );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	OQuery::OFieldInfo& fi( query->_fieldInfos[field_] );
	return ( fi._isNull ? NULL : fi._buffer );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OQuery* query( static_cast<OQuery*>( data_ ) );
	return ( static_cast<int>( query->_fieldInfos.get_size() ) );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* dataR_ ) {
	int rows( 0 );
	OQuery* query( static_cast<OQuery*>( dataR_ ) );
	if ( ! query->_fieldInfos.is_empty() ) {
		/* We have SELECT statement. */
		( *query->_status ) = OCIStmtFetch2( query->_statement,
				 query->_error, 1,
				 OCI_FETCH_LAST, 0,
				 OCI_DEFAULT );
		if ( ( *query->_status ) != OCI_NO_DATA ) {
			if ( ( ( *query->_status ) != OCI_SUCCESS )
					&& ( ( *query->_status ) != OCI_SUCCESS_WITH_INFO ) ) {
				log( LOG_TYPE::ERROR ) << _logTag_ << __FUNCTION__ << ": failed to fetch last row." << endl;
				rows = -1;
			}
		}
	}
	if ( ( ( *query->_status ) = OCIAttrGet( query->_statement,
					OCI_HTYPE_STMT, &rows, 0, OCI_ATTR_ROW_COUNT,
					query->_error ) ) != OCI_SUCCESS )
		rows = -1;
	return ( rows );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	M_ASSERT( dbLink_._conn && dbLink_._valid );
	OQuery* autonumber( static_cast<OQuery*>( oracle_query_execute( dbLink_,
					oracle_db_prepare_query( dbLink_, "SELECT SYS_CONTEXT( 'CLIENTCONTEXT', 'LAST_INSERT_ID' ) FROM dual", OCI_STMT_SCROLLABLE_READONLY ) ) ) );
	int long id( strtol( yaal_oracle_rs_get( autonumber, 0, 0 ), NULL, 10 ) );
	oracle_free_query( autonumber );
	return ( id );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OQuery* query = static_cast<OQuery*>( dataR_ );
	return ( query->_fieldInfos[field_]._name.raw() );
}

void oracle_init( void ) __attribute__((__constructor__));
void oracle_init( void ) {
	yaal_options()( "instance_name", program_options_helper::option_value( _instanceName_ ), HProgramOptionsHandler::OOption::ARGUMENT::REQUIRED, "name of the Oracle database instance", "name" );
	yaal_options().process_rc_file( "yaal", "oracle", NULL );
	return;
}

int yaal_oracle_driver_main( int, char** );
int yaal_oracle_driver_main( int, char** ) {
	return ( 0 );
}

}

#endif /* #ifdef HAVE_OCI_H */


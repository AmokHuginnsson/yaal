/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#ifdef HAVE_IBASE_H
#	include <ibase.h>
#elif defined( HAVE_FIREBIRD_IBASE_H )
#	include <firebird/ibase.h>
#else
#	error Firebird database is not supported.
#endif

#include <cstring>

#include "hcore/hchunk.hxx"
#include "hcore/harray.hxx"
#include "hcore/hresource.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

namespace {
static char const LAST_INSERT_ID[] = "SELECT CAST( RDB$GET_CONTEXT('USER_SESSION', 'LAST_INSERT_ID') AS BIGINT ) FROM RDB$DATABASE;";
bool is_err( ISC_STATUS const* status_ ) {
	return ( ( status_[0] == 1 ) && ( status_[1] != 0 ) );
}
}

struct OFirebird {
	static int const MAX_ERROR_COUNT = 20;
	static char const _tpb[];
	isc_db_handle _db;
	HChunk _connectionString;
	ISC_STATUS _status[ MAX_ERROR_COUNT ];
	HChunk _errorMessageBuffer;
	OFirebird( void )
		: _db( 0 )
		, _connectionString()
		, _status()
		, _errorMessageBuffer() {
		return;
	}
private:
	OFirebird( OFirebird const& );
	OFirebird& operator = ( OFirebird const& );
};

char const OFirebird::_tpb[] = {
	isc_tpb_version3,
	isc_tpb_write,
	isc_tpb_read_committed,
	isc_tpb_rec_version,
	isc_tpb_wait
};

struct OFirebirdResult {
	typedef HPointer<HChunk> chunk_t;
	typedef HArray<chunk_t> values_t;
	ODBLink& _dbLink;
	int _useCount;
	isc_stmt_handle _stmt;
	isc_tr_handle _tr;
	HChunk _descIn;
	HChunk _descOut;
	HChunk _cache;
	values_t _values;
	ISC_STATUS _status[ OFirebird::MAX_ERROR_COUNT ];
	HChunk _errorMessageBuffer;
	bool _ok;
	OFirebirdResult( ODBLink& dbLink_ )
		: _dbLink( dbLink_ )
		, _useCount( 1 )
		, _stmt( 0 )
		, _tr( 0 )
		, _descIn()
		, _descOut()
		, _cache()
		, _values()
		, _status()
		, _errorMessageBuffer()
		, _ok( false ) {
		return;
	}
private:
	OFirebirdResult( OFirebirdResult const& );
	OFirebirdResult& operator = ( OFirebirdResult const& );
};

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( "SELECT LOWER( TRIM( rdb$relation_name ) )"
		" FROM rdb$relations"
		" WHERE rdb$view_blr IS NULL"
		" AND (rdb$system_flag IS NULL OR rdb$system_flag = 0);" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "SELECT LOWER( TRIM( f.rdb$field_name ) )"
		" FROM rdb$relation_fields f"
		" JOIN rdb$relations r ON f.rdb$relation_name = r.rdb$relation_name"
		" AND r.rdb$view_blr IS NULL"
		" AND ( r.rdb$system_flag IS NULL OR r.rdb$system_flag = 0 )"
		" WHERE LOWER( TRIM( r.rdb$relation_name ) ) = '%s'"
		" ORDER BY f.rdb$field_position;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 0;

M_EXPORT_SYMBOL bool db_connect( ODBLink&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, yaal::hcore::HString const& dataBase_,
		yaal::hcore::HString const& login_, yaal::hcore::HString const& password_, yaal::hcore::HString const& host_ ) {
	typedef HResource<OFirebird> firebird_resource_guard_t;
	firebird_resource_guard_t db( new OFirebird );
	HUTF8String dataBase( ! host_.is_empty() ? host_ + ":" + dataBase_ : dataBase_ );
	HUTF8String login( login_ );
	HUTF8String password( password_ );
	int short dbLen( static_cast<int short>( dataBase.byte_count() ) );
	int short loginLen( static_cast<int short>( login.byte_count() ) );
	int short passLen( static_cast<int short>( password.byte_count() ) );
	static int short const DPB_VER_LEN( 1 );
	static int short const DPB_CLUSTER_HEADER_LEN( 2 );
	static int short const CLUSTER_COUNT( 2 ); /* login and password as clusters */
	int short dpbLen( static_cast<int short>( DPB_VER_LEN + CLUSTER_COUNT * DPB_CLUSTER_HEADER_LEN + loginLen + passLen ) );
	db->_connectionString.realloc( dpbLen + 1, HChunk::STRATEGY::EXACT ); /* +1 for terminating null */
	char* dpb( db->_connectionString.get<char>() );
	char* pdpb( dpb );
	*pdpb ++ = isc_dpb_version1;
	*pdpb ++ = isc_dpb_user_name;
	*pdpb ++ = static_cast<char>( loginLen );
	::strncpy( pdpb, login.c_str(), static_cast<size_t>( loginLen ) );
	pdpb += loginLen;
	*pdpb ++ = isc_dpb_password;
	*pdpb ++ = static_cast<char>( passLen );
	::strncpy( pdpb, password.c_str(), static_cast<size_t>( passLen ) );
	isc_attach_database( db->_status, dbLen, dataBase.c_str(), &db->_db, dpbLen, dpb );
	if ( ! is_err( db->_status ) ) {
		dbLink_._valid = true;
	}
	dbLink_._conn = db.release();
	return ( dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& );
M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	if ( db->_db ) {
		isc_detach_database( db->_status, &db->_db );
		M_ENSURE( ! is_err( db->_status ) );
	}
	M_SAFE( delete db );
	dbLink_.clear();
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const&, void* );
M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* result_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	OFirebirdResult* res( static_cast<OFirebirdResult*>( result_ ) );
	M_ASSERT( db );
	ISC_STATUS const* status( res && res->_stmt ? res->_status : db->_status );
	return ( isc_sqlcode( status ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const&, void* );
M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	OFirebirdResult* res( static_cast<OFirebirdResult*>( result_ ) );
	M_ASSERT( db || res );
	static int const MIN_ERROR_BUFFER_SIZE( 512 ); /* no documentation is available why 512 is correct here :( */
	ISC_STATUS const* status( res && res->_stmt ? res->_status : db->_status );
	HChunk& errBuf( res && res->_stmt ? res->_errorMessageBuffer : db->_errorMessageBuffer );
	errBuf.realloc( MIN_ERROR_BUFFER_SIZE );
	char* msg( errBuf.get<char>() );
	int msgLen( 0 );
	while ( fb_interpret( msg + msgLen, MIN_ERROR_BUFFER_SIZE, &status ) ) {
		msgLen = static_cast<int>( ::strlen( msg ) );
		errBuf.realloc( MIN_ERROR_BUFFER_SIZE + msgLen );
		msg = errBuf.get<char>();
	}
	return ( msg );
}

namespace {

typedef HResource<OFirebirdResult> firebird_result_resource_guard_t;

void* firebird_db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	firebird_result_resource_guard_t res( new OFirebirdResult( dbLink_ ) );
	res->_ok = false;
	do {
		isc_start_transaction( db->_status, &res->_tr, 1, &db->_db, sizeof ( OFirebird::_tpb ), OFirebird::_tpb );
		if ( is_err( db->_status ) ) {
			break;
		}
		XSQLDA descIn;
		XSQLDA descOut;
		memset( &descIn, 0, sizeof ( descIn ) );
		memset( &descOut, 0, sizeof ( descOut ) );
		descIn.version = SQLDA_VERSION1;
		descOut.version = SQLDA_VERSION1;
		isc_dsql_allocate_statement( db->_status, &db->_db, &res->_stmt );
		M_ENSURE( ! is_err( db->_status ) );
		isc_dsql_prepare( res->_status, &res->_tr, &res->_stmt, 0, query_, 3, &descOut ); /* Dialect version 3. */
		if ( is_err( res->_status ) ) {
			break;
		}
		isc_dsql_describe_bind( res->_status, &res->_stmt, 1, &descIn );
		if ( is_err( res->_status ) ) {
			break;
		}
		if ( descIn.sqld > 0 ) {
			res->_descIn.realloc( static_cast<int>( XSQLDA_LENGTH( static_cast<int unsigned>( descIn.sqld ) ) ), HChunk::STRATEGY::EXACT );
			XSQLDA* in( res->_descIn.get<XSQLDA>() );
			in->version = SQLDA_VERSION1;
			in->sqln = in->sqld = descIn.sqld;
			isc_dsql_describe_bind( res->_status, &res->_stmt, 1, in );
			if ( is_err( res->_status ) ) {
				break;
			}
			int i( 0 );
			XSQLVAR* var( nullptr );
			for ( i = 0, var = in->sqlvar; i < in->sqld; ++ i, ++ var ) {
				var->sqltype = SQL_TEXT;
			}
		}
		isc_dsql_describe( res->_status, &res->_stmt, 1, &descOut );
		if ( is_err( res->_status ) ) {
			break;
		}
		if ( descOut.sqld > 0 ) {
			res->_descOut.realloc( static_cast<int>( XSQLDA_LENGTH( static_cast<int unsigned>( descOut.sqld ) ) ), HChunk::STRATEGY::EXACT );
			XSQLDA* out( res->_descOut.get<XSQLDA>() );
			out->version = SQLDA_VERSION1;
			out->sqld = out->sqln = descOut.sqld;
			isc_dsql_describe( res->_status, &res->_stmt, 1, out );
			if ( is_err( res->_status ) ) {
				break;
			}
			int i( 0 );
			XSQLVAR* var( nullptr );
			int valuesMaxBufferSize( 0 );
			for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var ) {
				valuesMaxBufferSize += var->sqllen;
			}
			valuesMaxBufferSize += out->sqld;
			valuesMaxBufferSize += ( out->sqld * static_cast<int>( sizeof ( short ) ) );
			res->_cache.realloc( valuesMaxBufferSize );
			char* valBuf( res->_cache.get<char>() );
			for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var ) {
				var->sqldata = valBuf;
				valBuf += var->sqllen; /* value buffer */
				++ valBuf; /* \0 terminator */
				var->sqlind = reinterpret_cast<short*>( valBuf ); /* nullptr indicator */
				valBuf += sizeof ( short );
				var->sqltype = SQL_VARYING + 1; /* get everything as null terminated text */
			}
		}
		res->_ok = true;
	} while ( false );
	if ( ! res->_ok ) {
		if ( res->_stmt ) {
			isc_dsql_free_statement( db->_status, &res->_stmt, DSQL_drop );
		}
		res->_stmt = 0;
		isc_rollback_transaction( db->_status, &res->_tr );
		res->_tr = 0;
		if ( ! is_err( db->_status ) && is_err( res->_status ) ) {
			copy( begin( res->_status ), end( res->_status ), begin( db->_status ) );
		}
	}
	return ( res->_ok ? res.release() : nullptr );
}

void* firebird_query_execute( ODBLink& dbLink_, void* data_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	if ( res ) {
		if ( res->_stmt ) {
			isc_dsql_free_statement( db->_status, &res->_stmt, DSQL_close );
		}
		XSQLDA* in( res->_descIn.get<XSQLDA>() );
		if ( in ) {
			isc_dsql_execute2( res->_status, &res->_tr, &res->_stmt, 1, in, nullptr );
		} else {
			isc_dsql_execute( res->_status, &res->_tr, &res->_stmt, 1, nullptr );
		}
		if ( is_err( res->_status ) ) {
			res->_ok = false;
		}
	}
	return ( res );
}

int yaal_firebird_rs_fields_count( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	XSQLDA* out( res->_descOut.get<XSQLDA>() );
	return ( out ? out->sqld : 0 );
}

}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& dbLink_, char const* query_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	firebird_result_resource_guard_t rs( static_cast<OFirebirdResult*>( firebird_query_execute( dbLink_, firebird_db_prepare_query( dbLink_, query_ ) ) ) );
	OFirebirdResult* res( rs.get() );
	do {
		if ( res && ( yaal_firebird_rs_fields_count( res ) > 0 ) ) {
			res->_ok = false;
			XSQLDA* out( res->_descOut.get<XSQLDA>() );
			int retcode( 0 );
			while ( ( retcode = static_cast<int>( isc_dsql_fetch( res->_status, &res->_stmt, 1, out ) ) ) == 0 ) {
				int valuesBufferSize( 0 );
				int i( 0 );
				XSQLVAR* var( nullptr );
				for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var ) {
					valuesBufferSize += ( var->sqllen - static_cast<int>( sizeof ( short ) ) );
				}
				valuesBufferSize += out->sqld * static_cast<int>( sizeof ( int ) ); /* for offsets */
				valuesBufferSize += out->sqld; /* for \0 terminators */
				OFirebirdResult::chunk_t c( make_pointer<HChunk>() );
				c->realloc( valuesBufferSize );
				int offset( 0 );
				int* offsets( c->get<int>() );
				char* buf( reinterpret_cast<char*>( offsets + out->sqld ) );
				for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var ) {
					if ( ( *var->sqlind ) != -1 ) {
						offsets[i] = offset;
						int len( var->sqllen - static_cast<int>( sizeof ( short ) ) );
						::memcpy( buf + offset, var->sqldata + sizeof ( short ), static_cast<size_t>( len ) );
						offset += len;
						buf[offset] = 0;
						++ offset;
					} else {
						offsets[i] = -1;
					}
				}
				res->_values.push_back( c );
			}
			if ( retcode != 100 ) {
				break;
			}
			res->_ok = true;
		}
	} while ( false );
	if ( res ) {
		isc_dsql_free_statement( db->_status, &res->_stmt, res->_ok ? DSQL_close : DSQL_drop );
		if ( res->_ok ) {
			isc_commit_transaction( db->_status, &res->_tr );
			M_ENSURE( ! is_err( db->_status ), dbrs_error( dbLink_, res ) );
		} else {
			res->_stmt = 0;
			isc_rollback_transaction( db->_status, &res->_tr );
			res->_tr = 0;
		}
		if ( ! is_err( db->_status ) && is_err( res->_status ) ) {
			copy( begin( res->_status ), end( res->_status ), begin( db->_status ) );
		}
	}
	return ( res && res->_ok ? rs.release() : nullptr );
}

M_EXPORT_SYMBOL void rs_free_query_result( void* );
M_EXPORT_SYMBOL void rs_free_query_result( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	OFirebird* db( static_cast<OFirebird*>( res->_dbLink._conn ) );
	M_ASSERT( res );
	M_ASSERT( res->_useCount > 0 );
	-- res->_useCount;
	if ( res->_stmt ) {
		isc_dsql_free_statement( db->_status, &res->_stmt, res->_useCount == 0 ? DSQL_drop : DSQL_close );
	}
	if ( ! res->_useCount ) {
		res->_stmt = 0;
		M_ENSURE( ! is_err( db->_status ), dbrs_error( res->_dbLink, res ) );
		M_SAFE( delete res );
	}
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	return ( firebird_query_execute( dbLink_, firebird_db_prepare_query( dbLink_, query_ ) ) );
}

M_EXPORT_SYMBOL void* db_prepare_query( ODBLink&, char const* );
M_EXPORT_SYMBOL void* db_prepare_query( ODBLink& dbLink_, char const* query_ ) {
	return ( firebird_db_prepare_query( dbLink_, query_ ) );
}

M_EXPORT_SYMBOL void query_bind( ODBLink&, void*, int, yaal::hcore::HUTF8String const& );
M_EXPORT_SYMBOL void query_bind( ODBLink&, void* data_, int paramNo_, yaal::hcore::HUTF8String const& value_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	XSQLDA* in( res->_descIn.get<XSQLDA>() );
	int paramIdx( paramNo_ - 1 );
	if ( paramIdx < in->sqld ) {
		XSQLVAR* var( in->sqlvar + paramIdx );
		var->sqldata = const_cast<char*>( value_.c_str() );
		var->sqllen = static_cast<ISC_SHORT>( value_.byte_count() );
	}
	return;
}

M_EXPORT_SYMBOL void* query_execute( ODBLink&, void* );
M_EXPORT_SYMBOL void* query_execute( ODBLink& dbLink_, void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	++ res->_useCount;
	return ( firebird_query_execute( dbLink_, data_ ) );
}

void firebird_rs_free_cursor( void*, bool );

M_EXPORT_SYMBOL void query_free( ODBLink&, void* );
M_EXPORT_SYMBOL void query_free( ODBLink&, void* data_ ) {
	firebird_rs_free_cursor( data_, false );
	return;
}

void firebird_rs_free_cursor( void* data_, bool reset_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	OFirebird* db( static_cast<OFirebird*>( res->_dbLink._conn ) );
	M_ASSERT( res );
	M_ASSERT( res->_useCount > 0 );
	-- res->_useCount;
	if ( reset_ && res->_stmt ) {
		isc_dsql_free_statement( db->_status, &res->_stmt, res->_useCount == 0 ? DSQL_drop : DSQL_close );
	}
	if ( ! res->_useCount ) {
		res->_stmt = 0;
		M_ENSURE( ! is_err( db->_status ), dbrs_error( res->_dbLink, res ) );
		if ( res->_ok ) {
			isc_commit_transaction( db->_status, &res->_tr );
		} else {
			isc_rollback_transaction( db->_status, &res->_tr );
		}
		res->_tr = 0;
		M_ENSURE( ! is_err( db->_status ), dbrs_error( res->_dbLink, res ) );
		M_SAFE( delete res );
	}
	return;
}
M_EXPORT_SYMBOL void rs_free_cursor( void* );
M_EXPORT_SYMBOL void rs_free_cursor( void* data_ ) {
	firebird_rs_free_cursor( data_, true );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void*, int long, int );
M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	OFirebirdResult::chunk_t c( res->_values[row_] );
	XSQLDA* out( res->_descOut.get<XSQLDA>() );
	int* offsets( c->get<int>() );
	char* buf( reinterpret_cast<char*>( offsets + out->sqld ) );
	return ( offsets[column_] >= 0 ? buf + offsets[column_] : nullptr );
}

M_EXPORT_SYMBOL bool rs_next( void* );
M_EXPORT_SYMBOL bool rs_next( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	return ( isc_dsql_fetch( res->_status, &res->_stmt, 1, res->_descOut.get<XSQLDA>() ) == 0 );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int );
M_EXPORT_SYMBOL char const* rs_get_field( void* data_, int field_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	XSQLDA* out( res->_descOut.get<XSQLDA>() );
	XSQLVAR* var( out->sqlvar + field_ );
	return ( *var->sqlind != -1 ? var->sqldata + sizeof ( short ) : nullptr );
}

M_EXPORT_SYMBOL int rs_fields_count( void* );
M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	return ( yaal_firebird_rs_fields_count( data_ ) );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& /*dbLink_*/, void* dataR_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( dataR_ ) );
	M_ASSERT( res );
	char items[] = { isc_info_sql_stmt_type, isc_info_sql_records, isc_info_end };
	static int const COUNT_BUF_SIZE( 48 ); /* Value taken from firebird sources. */
	char countBuffer[COUNT_BUF_SIZE];
	::memset( countBuffer, isc_info_end, COUNT_BUF_SIZE );
	isc_dsql_sql_info( res->_status, &res->_stmt, static_cast<int short>( sizeof ( items ) ), items, static_cast<int short>( sizeof ( countBuffer ) ), countBuffer );
	M_ENSURE( ! is_err( res->_status ), dbrs_error( res->_dbLink, res ) );
	char statementType( 0 );
	char const* p( countBuffer );
	if ( *p == isc_info_sql_stmt_type ) {
		++ p;
		int len( isc_vax_integer( p, static_cast<int short>( sizeof ( short ) ) ) ); /* Stored on two bytes. */
		p += sizeof ( short );
		statementType = static_cast<char>( isc_vax_integer( p, static_cast<int short>( len ) ) );
		p += len;
	}
	int long count( 0 );
	if ( statementType == isc_info_sql_stmt_select ) {
		count = res->_values.size();
	} else {
		int reqCountType( 0 );
		switch ( statementType )  {
			case isc_info_sql_stmt_update: reqCountType = isc_info_req_update_count; break;
			case isc_info_sql_stmt_delete: reqCountType = isc_info_req_delete_count; break;
			case isc_info_sql_stmt_insert: reqCountType = isc_info_req_insert_count; break;
			default: break;
		}
		int totalLen( static_cast<int>( p - countBuffer ) );
		if ( *p == isc_info_sql_records ) {
			++ p;
			++ totalLen;
			totalLen += static_cast<int>( sizeof ( short ) );
			totalLen += isc_vax_integer( p, static_cast<int short>( sizeof ( short ) ) );
			p += sizeof ( short );
			while ( *p != isc_info_end ) {
				char const countType( *p++ );
				int len( isc_vax_integer( p, static_cast<int short>( sizeof ( short ) ) ) ); /* Stored on two bytes. */
				p += sizeof ( short );
				count = isc_vax_integer( p, static_cast<int short>( len ) );
				p += len;
				if ( countType == reqCountType ) {
					break;
				}
			}
			M_ENSURE( countBuffer[totalLen] == isc_info_end );
		}
	}
	return ( count );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink&, void* );
M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	bool ok( false );
	isc_tr_handle tr( 0 );
	isc_stmt_handle stmt( 0 );
	i64_t lastInsertId( -1 );
	short nullInd( 0 );
	do {
		isc_start_transaction( db->_status, &tr, 1, &db->_db, sizeof ( OFirebird::_tpb ), OFirebird::_tpb );
		if ( is_err( db->_status ) ) {
			break;
		}
		XSQLDA desc;
		XSQLVAR& var( desc.sqlvar[0] );
		desc.version = SQLDA_VERSION1;
		desc.sqln = 1;
		desc.sqld = 1;
		isc_dsql_allocate_statement( db->_status, &db->_db, &stmt );
		M_ENSURE( ! is_err( db->_status ) );
		isc_dsql_prepare( db->_status, &tr, &stmt, 0, LAST_INSERT_ID, 3, &desc ); /* Dialect version 3. */
		if ( is_err( db->_status ) ) {
			break;
		}
		isc_dsql_describe( db->_status, &stmt, 1, &desc );
		if ( is_err( db->_status ) ) {
			break;
		}
		var.sqlind = &nullInd;
		var.sqldata = reinterpret_cast<char*>( &lastInsertId );
		isc_dsql_execute( db->_status, &tr, &stmt, 1, nullptr );
		if ( is_err( db->_status ) ) {
			break;
		}
		isc_dsql_fetch( db->_status, &stmt, 1, &desc );
		if ( is_err( db->_status ) ) {
			break;
		}
		ok = true;
	} while ( false );
	isc_dsql_free_statement( db->_status, &stmt, DSQL_drop );
	M_ENSURE( ! is_err( db->_status ), dbrs_error( dbLink_, nullptr ) );
	if ( ! ok ) {
		isc_rollback_transaction( db->_status, &tr );
		M_ENSURE( ! is_err( db->_status ), dbrs_error( dbLink_, nullptr ) );
	} else {
		isc_commit_transaction( db->_status, &tr );
		M_ENSURE( ! is_err( db->_status ), dbrs_error( dbLink_, nullptr ) );
	}
	return ( nullInd != -1 ? static_cast<int long>( lastInsertId ) : -1 );
}

M_EXPORT_SYMBOL char const* rs_column_name( void*, int );
M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( dataR_ ) );
	M_ASSERT( res );
	XSQLDA* out( res->_descOut.get<XSQLDA>() );
	return ( out->sqlvar[field_].aliasname );
}

int yaal_firebird_driver_main( int, char** );
int yaal_firebird_driver_main( int, char** ) {
	return ( 0 );
}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	firebird_driver.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <ibase.h>

#include "hcore/hchunk.hxx"
#include "hcore/harray.hxx"
#include "hcore/hresource.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

namespace {
static char const LAST_INSERT_ID[] = "SELECT CAST( RDB$GET_CONTEXT('USER_SESSION', 'LAST_INSERT_ID') AS BIGINT ) FROM RDB$DATABASE;";
}

struct OFirebird {
	static int const MAX_ERROR_COUNT = 20;
	static char const _tpb[];
	isc_db_handle _db;
	HChunk _connectionString;
	ISC_STATUS _status[ MAX_ERROR_COUNT ];
	HChunk _errorMessageBufer;
	OFirebird( void )
		: _db( 0 ), _connectionString(), _status(), _errorMessageBufer()
		{}
};

char const OFirebird::_tpb[] = {
	isc_tpb_version3,
	isc_tpb_consistency,
	isc_tpb_write
};

struct OFirebirdResult {
	typedef HPointer<HChunk> chunk_t;
	typedef HArray<chunk_t> values_t;
	ODBLink& _dbLink;
	isc_stmt_handle _stmt;
	HChunk _metaData;
	HChunk _cache;
	values_t _values;
	ISC_STATUS _status[ OFirebird::MAX_ERROR_COUNT ];
	HChunk _errorMessageBufer;
	OFirebirdResult( ODBLink& dbLink_ )
		: _dbLink( dbLink_ ), _stmt( 0 ),
		_metaData(), _cache(), _values(),
		_status(), _errorMessageBufer()
		{}
};

extern "C" {

char* TABLE_LIST_QUERY = const_cast<char*>( "SELECT LOWER( TRIM( rdb$relation_name ) )"
		" FROM rdb$relations"
		" WHERE rdb$view_blr IS NULL"
		" AND (rdb$system_flag IS NULL OR rdb$system_flag = 0);" );
char* COLUMN_LIST_QUERY = const_cast<char*>( "SELECT LOWER( TRIM( f.rdb$field_name ) )"
		" FROM rdb$relation_fields f"
		" JOIN rdb$relations r ON f.rdb$relation_name = r.rdb$relation_name"
		" AND r.rdb$view_blr IS NULL"
		" AND ( r.rdb$system_flag IS NULL OR r.rdb$system_flag = 0 )"
		" WHERE LOWER( TRIM( r.rdb$relation_name ) ) = '%s'"
		" ORDER BY f.rdb$field_position;" );
int COLUMN_NAME_INDEX = 0;

M_EXPORT_SYMBOL bool db_connect( ODBLink& dbLink_, char const* dataBase_,
		char const* login_, char const* password_ ) {
	M_ASSERT( dataBase_ && login_ && password_ );
	typedef HResource<OFirebird> firebird_resource_guard_t;
	firebird_resource_guard_t db( new OFirebird );
	int short dbLen( static_cast<int short>( ::strlen( dataBase_ ) ) );
	int short loginLen( static_cast<int short>( ::strlen( login_ ) ) );
	int short passLen( static_cast<int short>( ::strlen( password_ ) ) );
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
	::strncpy( pdpb, login_, loginLen );
	pdpb += loginLen;
	*pdpb ++ = isc_dpb_password;
	*pdpb ++ = static_cast<char>( passLen );
	::strncpy( pdpb, password_, passLen );
	isc_attach_database( db->_status, dbLen, dataBase_, &db->_db, dpbLen, dpb );
	if ( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ) )
		dbLink_._valid = true;
	dbLink_._conn = db.release();
	return ( ! dbLink_._valid );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& dbLink_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	isc_detach_database( db->_status, &db->_db );
	M_ENSURE( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ) );
	M_SAFE( delete db );
	dbLink_.clear();
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& dbLink_, void* ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	return ( isc_sqlcode( db->_status ) );
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& dbLink_, void* result_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	OFirebirdResult* res( static_cast<OFirebirdResult*>( result_ ) );
	M_ASSERT( db || res );
	static int const MIN_ERROR_BUFFER_SIZE( 512 ); /* no documentation is available why 512 is correct here :( */
	ISC_STATUS const* status( res ? res->_status : db->_status );
	HChunk& errBuf( res ? res->_errorMessageBufer : db->_errorMessageBufer );
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

M_EXPORT_SYMBOL void* db_query( ODBLink& dbLink_, char const* query_ ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	typedef HResource<OFirebirdResult> firebird_result_resource_guard_t;
	firebird_result_resource_guard_t res( new OFirebirdResult( dbLink_ ) );
	bool ok( false );
	isc_tr_handle tr( 0 );
	do {
		isc_start_transaction( db->_status, &tr, 1, &db->_db, sizeof ( OFirebird::_tpb ), OFirebird::_tpb );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		XSQLDA desc;
		desc.version = SQLDA_VERSION1;
		desc.sqln = 1;
		desc.sqld = 1;
		isc_dsql_allocate_statement( db->_status, &db->_db, &res->_stmt );
		M_ENSURE( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ) );
		isc_dsql_prepare( db->_status, &tr, &res->_stmt, 0, query_, 3, &desc ); /* Dialect version 3. */
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		isc_dsql_describe( db->_status, &res->_stmt, 1, &desc );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		XSQLDA* out( NULL );
		if ( desc.sqld > 0 ) {
			res->_metaData.realloc( XSQLDA_LENGTH( desc.sqld ) );
			out = res->_metaData.get<XSQLDA>();
			out->version = SQLDA_VERSION1;
			out->sqld = out->sqln = desc.sqld;
			isc_dsql_describe( db->_status, &res->_stmt, 1, out );
			if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
				break;
			int i( 0 );
			XSQLVAR* var( NULL );
			int valuesMaxBufferSize( 0 );
			for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var )
				valuesMaxBufferSize += var->sqllen;
			valuesMaxBufferSize += out->sqld;
			valuesMaxBufferSize += ( out->sqld * static_cast<int>( sizeof ( short ) ) );
			res->_cache.realloc( valuesMaxBufferSize );
			char* valBuf( res->_cache.raw() );
			for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var ) {
				var->sqldata = valBuf;
				valBuf += var->sqllen; /* vlaue buffer */
				++ valBuf; /* \0 terminator */
				var->sqlind = reinterpret_cast<short*>( valBuf ); /* NULL indicator */
				valBuf += sizeof ( short );
				var->sqltype = SQL_VARYING + 1; /* get everything as null terminated text */
			}
		}
		isc_dsql_execute( db->_status, &tr, &res->_stmt, 1, NULL );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		if ( desc.sqld > 0 ) {
			int retcode( 0 );
			while ( ( retcode = static_cast<int>( isc_dsql_fetch( db->_status, &res->_stmt, 1, out ) ) ) == 0 ) {
				int valuesBufferSize( 0 );
				int i( 0 );
				XSQLVAR* var( NULL );
				for ( i = 0, var = out->sqlvar; i < out->sqld; ++ i, ++ var )
					valuesBufferSize += ( var->sqllen - static_cast<int>( sizeof ( short ) ) );
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
						::memcpy( buf + offset, var->sqldata + sizeof ( short ), len );
						offset += len;
						buf[offset] = 0;
						++ offset;
					} else
						offsets[i] = -1;
				}
				res->_values.push_back( c );
			}
			if ( retcode != 100 )
				break;
		}
		ok = true;
	} while ( false );
	if ( ! ok ) {
		isc_dsql_free_statement( res->_status, &res->_stmt, DSQL_drop );
		isc_rollback_transaction( res->_status, &tr );
	} else {
		isc_commit_transaction( res->_status, &tr );
		M_ENSURE_EX( ( res->_status[0] != 1 ) || ( res->_status[1] == 0 ), dbrs_error( res->_dbLink, res.get() ) );
	}
	return ( ok ? res.release() : NULL );
}

M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	isc_dsql_free_statement( res->_status, &res->_stmt, DSQL_drop );
	M_ENSURE_EX( ( res->_status[0] != 1 ) || ( res->_status[1] == 0 ), dbrs_error( res->_dbLink, res ) );
	M_SAFE( delete res );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* data_, int long row_, int column_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	OFirebirdResult::chunk_t c( res->_values[row_] );
	XSQLDA* out( res->_metaData.get<XSQLDA>() );
	int* offsets( c->get<int>() );
	char* buf( reinterpret_cast<char*>( offsets + out->sqld ) );
	return ( offsets[column_] >= 0 ? buf + offsets[column_] : NULL );
}

M_EXPORT_SYMBOL int rs_fields_count( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	XSQLDA* out( res->_metaData.get<XSQLDA>() );
	return ( out ? out->sqld : 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& /*dbLink_*/, void* dataR_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( dataR_ ) );
	M_ASSERT( res );
	char items[] = { isc_info_sql_stmt_type, isc_info_sql_records, isc_info_end };
	static int const COUNT_BUF_SIZE( 48 ); /* Value taken from firebird sources. */
	char countBuffer[COUNT_BUF_SIZE];
	::memset( countBuffer, isc_info_end, COUNT_BUF_SIZE );
	isc_dsql_sql_info( res->_status, &res->_stmt, sizeof ( items ), items, sizeof ( countBuffer ), countBuffer );
	M_ENSURE_EX( ( res->_status[0] != 1 ) || ( res->_status[1] == 0 ), dbrs_error( res->_dbLink, res ) );
	char statementType( 0 );
	char const* p( countBuffer );
	if ( *p == isc_info_sql_stmt_type ) {
		++ p;
		int len( isc_vax_integer( p, sizeof ( short ) ) ); /* Stored on two bytes. */
		p += sizeof ( short );
		statementType = static_cast<char>( isc_vax_integer( p, static_cast<int short>( len ) ) );
		p += len;
	}
	int long count( 0 );
	if ( statementType == isc_info_sql_stmt_select )
		count = res->_values.size();
	else {
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
			totalLen += isc_vax_integer( p, sizeof ( short ) );
			p += sizeof ( short );
			while ( *p != isc_info_end ) {
				char const countType( *p++ );
				int len( isc_vax_integer( p, sizeof ( short ) ) ); /* Stored on two bytes. */
				p += sizeof ( short );
				count = isc_vax_integer( p, static_cast<int short>( len ) );
				p += len;
				if ( countType == reqCountType )
					break;
			}
			M_ENSURE( countBuffer[totalLen] == isc_info_end );
		}
	}
	return ( count );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink& dbLink_, void* ) {
	OFirebird* db( static_cast<OFirebird*>( dbLink_._conn ) );
	M_ASSERT( db );
	bool ok( false );
	isc_tr_handle tr( 0 );
	isc_stmt_handle stmt( 0 );
	int long lastInsertId( -1 );
	short nullInd( 0 );
	do {
		isc_start_transaction( db->_status, &tr, 1, &db->_db, sizeof ( OFirebird::_tpb ), OFirebird::_tpb );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		XSQLDA desc;
		XSQLVAR& var( desc.sqlvar[0] );
		desc.version = SQLDA_VERSION1;
		desc.sqln = 1;
		desc.sqld = 1;
		isc_dsql_allocate_statement( db->_status, &db->_db, &stmt );
		M_ENSURE( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ) );
		isc_dsql_prepare( db->_status, &tr, &stmt, 0, LAST_INSERT_ID, 3, &desc ); /* Dialect version 3. */
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		isc_dsql_describe( db->_status, &stmt, 1, &desc );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		var.sqlind = &nullInd;
		var.sqldata = reinterpret_cast<char*>( &lastInsertId );
		isc_dsql_execute( db->_status, &tr, &stmt, 1, NULL );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		if ( isc_dsql_fetch( db->_status, &stmt, 1, &desc ) != 0 )
			break;
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		ok = true;
	} while ( false );
	isc_dsql_free_statement( db->_status, &stmt, DSQL_drop );
	M_ENSURE_EX( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ), dbrs_error( dbLink_, NULL ) );
	if ( ! ok ) {
		isc_rollback_transaction( db->_status, &tr );
		M_ENSURE_EX( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ), dbrs_error( dbLink_, NULL ) );
	} else {
		isc_commit_transaction( db->_status, &tr );
		M_ENSURE_EX( ( db->_status[0] != 1 ) || ( db->_status[1] == 0 ), dbrs_error( dbLink_, NULL ) );
	}
	return ( nullInd != -1 ? lastInsertId : -1 );
}

M_EXPORT_SYMBOL char const* rs_column_name( void* dataR_, int field_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( dataR_ ) );
	M_ASSERT( res );
	XSQLDA* out( res->_metaData.get<XSQLDA>() );
	return ( out->sqlvar[field_].aliasname );
}

int yaal_firebird_driver_main( int, char** ) {
	return ( 0 );
}

}


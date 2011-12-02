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
#include "hcore/hresource.hxx"
#include "dbwrapper/db_driver.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::dbwrapper;

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
	ODBLink& _dbLink;
	isc_tr_handle _tr;
	isc_stmt_handle _stmt;
	ISC_STATUS _status[ OFirebird::MAX_ERROR_COUNT ];
	HChunk _errorMessageBufer;
	OFirebirdResult( ODBLink& dbLink_ )
		: _dbLink( dbLink_ ), _tr( 0 ), _stmt( 0 ), _status(), _errorMessageBufer()
		{}
};

extern "C" {

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
	do {
		isc_start_transaction( db->_status, &res->_tr, 1, &db->_db, sizeof ( OFirebird::_tpb ), OFirebird::_tpb );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) )
			break;
		XSQLDA desc;
		desc.version = SQLDA_VERSION1;
		desc.sqln = 1;
		desc.sqld = 1;
		isc_dsql_allocate_statement( db->_status, &db->_db, &res->_stmt );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) ) {
			isc_rollback_transaction( res->_status, &res->_tr );
			break;
		}
		isc_dsql_prepare( db->_status, &res->_tr, &res->_stmt, 0, query_, 1, &desc );
		if ( ( db->_status[0] == 1 ) && ( db->_status[1] != 0 ) ) {
			isc_dsql_free_statement( res->_status, &res->_stmt, DSQL_drop );
			isc_rollback_transaction( res->_status, &res->_tr );
			break;
		}
		ok = true;
	} while ( false );
	return ( ok ? res.release() : NULL );
}

M_EXPORT_SYMBOL void rs_unquery( void* data_ ) {
	OFirebirdResult* res( static_cast<OFirebirdResult*>( data_ ) );
	M_ASSERT( res );
	isc_commit_transaction( res->_status, &res->_tr );
	M_ENSURE_EX( ( res->_status[0] != 1 ) || ( res->_status[1] == 0 ), dbrs_error( res->_dbLink, res ) );
	M_SAFE( delete res );
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* /*data_*/, int long /*row_*/, int /*column_*/ ) {
	return ( NULL );
}

M_EXPORT_SYMBOL int rs_fields_count( void* /*data_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL int long dbrs_records_count( ODBLink& /*dbLink_*/, void* /*dataR_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL int long dbrs_id( ODBLink& /*dbLink_*/, void* /*dataR_*/ ) {
	return ( 0 );
}

M_EXPORT_SYMBOL char const* rs_column_name( void* /*dataR_*/, int /*field_*/ ) {
	return ( NULL );
}

int yaal_firebird_driver_main( int, char** ) {
	return ( 0 );
}

}


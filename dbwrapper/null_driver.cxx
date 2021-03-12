/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "dbwrapper/db_driver.hxx"

using namespace yaal::dbwrapper;

extern "C" {

M_EXPORT_SYMBOL char* TABLE_LIST_QUERY = const_cast<char*>( ";" );
M_EXPORT_SYMBOL char* COLUMN_LIST_QUERY = const_cast<char*>( "%s;" );
M_EXPORT_SYMBOL int COLUMN_NAME_INDEX = 0;

M_EXPORT_SYMBOL void driver_init( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL void driver_cleanup( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

M_EXPORT_SYMBOL bool db_connect( ODBLink& /*dbLink_*/,
		yaal::hcore::HString const&, yaal::hcore::HString const&,
		yaal::hcore::HString const&, yaal::hcore::HString const& ) {
	return ( true );
}

M_EXPORT_SYMBOL void db_disconnect( ODBLink& /*dbLink_*/ ) {
	return;
}

M_EXPORT_SYMBOL int dbrs_errno( ODBLink const& /*dbLink_*/, void* /*dataR_*/ ) {
	return errno;
}

M_EXPORT_SYMBOL char const* dbrs_error( ODBLink const& /*dbLink_*/, void* /*dataR_*/ ) {
	return ( "" );
}

M_EXPORT_SYMBOL void* db_fetch_query_result( ODBLink& /*dbLink_*/, char const* /*query_*/ ) {
	return nullptr;
}

M_EXPORT_SYMBOL void rs_free_query_result( void* /*data_*/ ) {
	return;
}

M_EXPORT_SYMBOL void* db_query( ODBLink& /*dbLink_*/, char const* /*query_*/ ) {
	return nullptr;
}

M_EXPORT_SYMBOL void rs_free_cursor( void* /*data_*/ ) {
	return;
}

M_EXPORT_SYMBOL char const* rs_get( void* /*data_*/, int long /*row_*/, int /*column_*/ ) {
	return nullptr;
}

M_EXPORT_SYMBOL bool rs_next( void* ) {
	return ( false );
}

M_EXPORT_SYMBOL char const* rs_get_field( void*, int ) {
	return nullptr;
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
	return nullptr;
}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_DBWRAPPER_DB_DRIVER_HXX_INCLUDED
#define YAAL_DBWRAPPER_DB_DRIVER_HXX_INCLUDED 1

#include "tools/hplugin.hxx"
#include "hcore/hpair.hxx"

namespace yaal {

/*! \brief Wrapper for standardized database access.
 *
 * The dbwrapper allows its user to transparently access
 * any kind of relational database that are available for Unix.
 */
namespace dbwrapper {

struct ODBLink {
	void* _conn;
	bool _valid;
	ODBLink( void )
		: _conn( nullptr )
		, _valid( false ) {
	}
	void clear( void ) {
		_conn = nullptr;
		_valid = false;
	}
private:
	ODBLink( ODBLink const& );
	ODBLink& operator = ( ODBLink const& );
};

struct ODBConnector {
	typedef ODBConnector this_type;
	typedef yaal::hcore::HPointer<ODBConnector> ptr_t;
	struct DRIVER {
		typedef enum {
			DEFAULT = -1,
			SQLITE3,
			POSTGRESQL,
			MYSQL,
			FIREBIRD,
			ORACLE,
			TERMINATOR
		} enum_t;
	};
	typedef void ( * driver_init_t )( void );
	typedef void ( * driver_cleanup_t )( void );
	typedef bool ( * db_connect_t )( ODBLink&, yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
	typedef void ( * db_disconnect_t )( ODBLink& );
	typedef int ( * dbrs_errno_t )( ODBLink const&, void* );
	typedef char const* ( * dbrs_error_t )( ODBLink const&, void* );
	typedef void* ( * db_query_t )( ODBLink&, char const* );
	typedef void* ( * db_prepare_query_t )( ODBLink&, char const* );
	typedef void ( * query_bind_t )( ODBLink&, void*, int, yaal::hcore::HUTF8String const& );
	typedef void* ( * query_execute_t )( ODBLink&, void* );
	typedef void ( * query_free_t )( ODBLink&, void* );
	typedef void ( * rs_unquery_t )( void* );
	typedef char const* ( * rs_get_t )( void*, int long, int );
	typedef char const* ( * rs_get_field_t )( void*, int );
	typedef int ( * rs_fields_count_t )( void* );
	typedef bool ( * rs_next_t )( void* );
	typedef int long ( * dbrs_records_count_t )( ODBLink&, void* );
	typedef int long ( * dbrs_id_t )( ODBLink&, void* );
	typedef char const* ( * rs_column_name_t )( void*, int );
	driver_init_t driver_init;
	driver_cleanup_t driver_cleanup;
	db_connect_t db_connect;
	db_disconnect_t db_disconnect;
	dbrs_errno_t dbrs_errno;
	dbrs_error_t dbrs_error;
	db_query_t db_fetch_query_result;
	rs_unquery_t rs_free_query_result;
	db_query_t db_query;
	db_prepare_query_t db_prepare_query;
	query_bind_t query_bind;
	query_execute_t query_execute;
	query_free_t query_free;
	rs_unquery_t rs_free_cursor;
	rs_get_t rs_get;
	rs_next_t rs_next;
	rs_get_field_t rs_get_field;
	rs_fields_count_t rs_fields_count;
	dbrs_records_count_t dbrs_records_count;
	dbrs_id_t dbrs_id;
	rs_column_name_t rs_column_name;
	char const* _tableListQuery;
	char const* _columnListQuery;
	int _columnNameIndex;
	ODBConnector( void );
	virtual ~ODBConnector( void );
	void init( void );
	void cleanup( void );
private:
	ODBConnector( ODBConnector const& );
	ODBConnector& operator = ( ODBConnector const& );
};

typedef yaal::hcore::HPair<yaal::tools::HPlugin::ptr_t, ODBConnector::ptr_t> driver_t;
class HDataBase;
typedef yaal::hcore::HPointer<HDataBase> database_ptr_t;

typedef yaal::hcore::HString ( *place_holder_generator_t ) ( int );
yaal::hcore::HString transform_sql( yaal::hcore::HString, place_holder_generator_t );

}

}

#endif /* #ifndef YAAL_DBWRAPPER_DB_DRIVER_HXX_INCLUDED */


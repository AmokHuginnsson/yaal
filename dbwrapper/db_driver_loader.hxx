/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED
#define YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED

#include "hcore/hmap.hxx"
#include "tools/hplugin.hxx"

namespace yaal
{

namespace hcore
{
namespace LOG_TYPE
{

static int const SQL = 1073741824;

}
}

/*! \brief Wrapper for standarized database access.
 * 
 * The dbwrapper allows its user to transparently access
 * any kind of relational database that are available for Unix.
 */
namespace dbwrapper
{

struct ODBConnector
	{
	struct DRIVER
		{
		typedef enum
			{
			DEFAULT = -1,
			NONE = 0,
			DUMMY = 0,
			SQLITE3,
			POSTGRESQL,
			MYSQL, 
			ORACLE,
			TERMINATOR
			} enum_t;
		};
	typedef void* ( * db_connect_t )( char const*, char const*, char const* );
	typedef void ( * db_disconnect_t )( void* );
	typedef int ( * dbrs_errno_t )( void*, void* );
	typedef char const* ( * dbrs_error_t )( void*, void* );
	typedef void* ( * db_query_t )( void*, char const* );
	typedef void ( * rs_unquery_t )( void* );
	typedef char* ( * rs_get_t )( void*, int long, int );
	typedef int ( * rs_fields_count_t )( void* );
	typedef int long ( * dbrs_records_count_t )( void*, void* );
	typedef int long ( * dbrs_id_t )( void*, void* );
	typedef char* ( * rs_column_name_t )( void*, int );
	db_connect_t db_connect;
	db_disconnect_t db_disconnect;
	dbrs_errno_t dbrs_errno;
	dbrs_error_t dbrs_error;
	db_query_t db_query;
	rs_unquery_t rs_unquery;
	rs_get_t rs_get;
	rs_fields_count_t rs_fields_count;
	dbrs_records_count_t dbrs_records_count;
	dbrs_id_t dbrs_id;
	rs_column_name_t rs_column_name;
	ODBConnector( void );
	};

ODBConnector const* load_driver( ODBConnector::DRIVER::enum_t const& );
typedef yaal::hcore::HPair<yaal::tools::HPlugin::ptr_t, ODBConnector> driver_t;
typedef yaal::hcore::HMap<ODBConnector::DRIVER::enum_t, driver_t> drivers_t;
extern M_YAAL_DBWRAPPER_PUBLIC_API drivers_t _dBDrivers_;

}

}

#endif /* not YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED */


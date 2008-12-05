/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	db_driver_loader.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED
#define YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED

namespace yaal
{

namespace hcore
{
namespace LOG_TYPE
	{
	static int const D_SQL = 1073741824;
	}
}

/*! \brief Wrapper for standarized database access.
 * 
 * The dbwrapper allows its user to transparently access
 * any kind of relational database that are available for Unix.
 */
namespace dbwrapper
	{

typedef void* ( * db_connect_t )( char const*, char const*, char const* );
typedef void ( * db_disconnect_t )( void* );
typedef int ( * db_errno_t )( void* );
typedef char const* ( * db_error_t )( void* );
typedef void* ( * db_query_t )( void*, char const* );
typedef void ( * db_unquery_t )( void* );
typedef char* ( * rs_get_t )( void*, int long, int );
typedef int ( * rs_fields_count_t )( void* );
typedef int long ( * dbrs_records_count_t )( void*, void* );
typedef int long ( * dbrs_id_t )( void*, void* );
typedef char* ( * rs_column_name_t )( void*, int );

extern db_connect_t db_connect;
extern db_disconnect_t db_disconnect;
extern db_errno_t db_errno;
extern db_error_t db_error;
extern db_query_t db_query;
extern db_unquery_t db_unquery;
extern rs_get_t rs_get;
extern rs_fields_count_t rs_fields_count;
extern dbrs_records_count_t dbrs_records_count;
extern dbrs_id_t dbrs_id;
extern rs_column_name_t rs_column_name;

	}

}

#endif /* not YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED */


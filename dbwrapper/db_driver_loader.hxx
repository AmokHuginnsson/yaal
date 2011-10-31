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
#define YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED 1

#include "dbwrapper/db_driver.hxx"
#include "hcore/hmap.hxx"

namespace yaal {

namespace dbwrapper {

ODBConnector const* load_driver( ODBConnector::DRIVER::enum_t );
typedef yaal::hcore::HMap<ODBConnector::DRIVER::enum_t, driver_t> drivers_t;
extern M_YAAL_DBWRAPPER_PUBLIC_API drivers_t _dBDrivers_;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_DB_DRIVER_LOADER_HXX_INCLUDED */


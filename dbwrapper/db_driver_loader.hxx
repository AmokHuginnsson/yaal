/* Read yaal/LICENSE.md file for copyright and licensing information. */

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


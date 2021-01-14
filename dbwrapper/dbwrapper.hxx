/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_DBWRAPPER_DBWRAPPER_HXX_INCLUDED
#define YAAL_DBWRAPPER_DBWRAPPER_HXX_INCLUDED 1

#include "dbwrapper/db_driver_loader.hxx"
#include "dbwrapper/hdatabase.hxx"

namespace yaal {

namespace dbwrapper {

/*! \brief Helper functions for database access.
 */
namespace util {

/*! \brief Connect to database based on provided DSN URI.
 *
 * \param dsn_ - Data Source Name URI containing all necessary information to obtain database connection.
 * \return Database handle.
 */
HDataBase::ptr_t connect( yaal::hcore::HString const& dsn_ );

}

extern M_YAAL_DBWRAPPER_PUBLIC_API bool _logSQL_;
void banner( void );

}

}

#endif /* #ifndef YAAL_DBWRAPPER_DBWRAPPER_HXX_INCLUDED */


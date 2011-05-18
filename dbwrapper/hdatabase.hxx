/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatabase.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED
#define YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "dbwrapper/db_driver_loader.hxx"
#include "dbwrapper/hrecordset.hxx"

namespace yaal
{

namespace dbwrapper
{

/*! \brief Data base access abstraction layer.
 */
class HDataBase : public yaal::hcore::HPointerFromThisInterface<HDataBase>
	{
	typedef HDataBase this_type;
private:
	ODBConnector const* _connector;
	void* _coreData;	/*!< very internal for this class used only in base class */
public:
	typedef yaal::hcore::HPointer<HDataBase> ptr_t; /*!< Smart pointer type for this class. */
	/*! \brief Connect to database.
	 *
	 * \param database - database to connect to.
	 * \param login - authenticate to database with given login
	 * \param password - password for given login
	 */
	void connect( yaal::hcore::HString const& database,
							yaal::hcore::HString const& login,
							yaal::hcore::HString const& password );
	/*! \brief Close database connection.
	 */
	void disconnect( void );
	/*! \brief Run given query on database connection.
	 *
	 * \param query_ - query to be executed.
	 * \return Result set for given query.
	 */
	HRecordSet::ptr_t query( yaal::hcore::HString const& query_ );
	/*! \brief Get error messege for last error that occurred on this connection.
	 *
	 * \return Error message for last error that occurred.
	 */
	char const* get_error( void ) const;
	/*! \brief Get error code of last error that occurred on this database connection.
	 *
	 * \return Error code for last error on this connection.
	 */
	int get_errno( void ) const;
	static ptr_t get_connector( ODBConnector::DRIVER::enum_t = ODBConnector::DRIVER::DEFAULT );
	ODBConnector const* connector( void ) const;
private:
	HDataBase( void );
	virtual ~HDataBase( void );
	HDataBase( HDataBase const& );
	HDataBase& operator = ( HDataBase const& );
	friend struct yaal::hcore::HPointerScalar<HDataBase>;
	friend class HRecordSet;
	};

typedef yaal::hcore::HExceptionT<HDataBase> HDataBaseException;
typedef HDataBaseException HSQLException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED */


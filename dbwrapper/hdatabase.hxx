/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED
#define YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "dbwrapper/db_driver.hxx"
#include "dbwrapper/hrecordset.hxx"
#include "dbwrapper/hquery.hxx"

namespace yaal {

namespace dbwrapper {

/*! \brief Data base access abstraction layer.
 */
class HDataBase : public yaal::hcore::HPointerFromThisInterface<HDataBase> {
public:
	typedef HDataBase this_type;
	typedef yaal::hcore::HArray<yaal::hcore::HString> table_list_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> column_list_t;
private:
	ODBConnector const* _connector;
	ODBLink _dbLink; /*!< very internal for this class used only in base class */
public:
	typedef yaal::hcore::HPointer<HDataBase> ptr_t; /*!< Smart pointer type for this class. */

	/*! \brief Connect to database.
	 *
	 * \param database - database to connect to.
	 * \param login - authenticate to database with given login
	 * \param password - password for given login
	 * \param server - server connection specification.
	 */
	void connect( yaal::hcore::HString const& database,
							yaal::hcore::HString const& login,
							yaal::hcore::HString const& password,
							yaal::hcore::HString const& server = yaal::hcore::HString() );

	/*! \brief Close database connection.
	 */
	void disconnect( void );

	/*! \brief Run given query on database connection.
	 *
	 * \param query_ - query to be executed.
	 * \param cursor_ - query result cursor type.
	 * \return Result set for given query.
	 */
	HRecordSet::ptr_t execute_query( yaal::hcore::HString const& query_, HRecordSet::CURSOR cursor_ = HRecordSet::CURSOR::RANDOM_ACCESS );

	/*! \brief Prepare query object for argument binding.
	 *
	 * \param query_ - query to be executed.
	 * \return Query object prepared for argument binding.
	 */
	HQuery::ptr_t prepare_query( yaal::hcore::HString const& query_ );

	/*! \brief Get error message for last error that occurred on this connection.
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
	table_list_t get_tables( void ) const;
	column_list_t get_columns( yaal::hcore::HString const& ) const;
private:
	HDataBase( void );
	virtual ~HDataBase( void );
	HDataBase( HDataBase const& );
	HDataBase& operator = ( HDataBase const& );
	friend class yaal::hcore::pointer_helper::HSpaceHolderDeleter<HDataBase>;
	friend class HRecordSet;
	friend class HQuery;
	friend ptr_t yaal::hcore::make_pointer<HDataBase>( void );
};

typedef yaal::hcore::HExceptionT<HDataBase> HDataBaseException;
typedef HDataBaseException HSQLException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED */


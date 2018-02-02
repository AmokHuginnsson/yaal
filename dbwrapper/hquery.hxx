/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file dbwrapper/hquery.hxx
 * \brief Declaration of HQuery class.
 */

#ifndef YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED
#define YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hpointer.hxx"
#include "dbwrapper/db_driver.hxx"
#include "dbwrapper/hrecordset.hxx"

namespace yaal {

namespace dbwrapper {

/*! \brief SQL query with binding capabilities.
 */
class HQuery {
public:
	typedef HQuery this_type;
	typedef yaal::hcore::HPointer<HQuery> ptr_t;
	typedef yaal::hcore::HHashMap<int, yaal::hcore::HUTF8String> bind_buffer_t;
	typedef yaal::hcore::HPointerObserver<HRecordSet> result_observer_t;
private:
	mutable database_ptr_t _dataBase; /*!< data-base that this record-set belongs to */
	mutable ODBConnector const* _connector; /*!< low level database engine connector */
	void* _query; /*!< low level query data */
	yaal::hcore::HString _sql;
	bind_buffer_t _bindBuffer;
	result_observer_t _resultObserver;
public:
	virtual ~HQuery( void );
	void bind( int, yaal::hcore::HString const& );
	HRecordSet::ptr_t execute( void );
private:
	HQuery( database_ptr_t, ODBConnector const*, yaal::hcore::HString const&, void* );
	HQuery( HQuery const& );
	HQuery& operator = ( HQuery const& );
	friend class yaal::hcore::pointer_helper::HSpaceHolderDeleter<HQuery>;
	template<typename tType, typename... arg_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( arg_t&&... );
};

typedef yaal::hcore::HExceptionT<HQuery> HQueryException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED */


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	dbwrapper/hquery.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file dbwrapper/hquery.hxx
 * \brief Declaration of HQuery class.
 */

#ifndef YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED
#define YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
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
	typedef yaal::hcore::HArray<yaal::hcore::HString> bind_buffer_t;
private:
	mutable database_ptr_t _dataBase; /*!< data-base that this record-set belongs to */
	mutable ODBConnector const* _connector; /*!< low level database engine connector */
	void* _query; /*!< low level query data */
	yaal::hcore::HString _sql;
	bind_buffer_t _bindBuffer;
public:
	virtual ~HQuery( void );
	void bind( int, yaal::hcore::HString const& );
	HRecordSet::ptr_t execute( void );
private:
	HQuery( database_ptr_t, ODBConnector const*, yaal::hcore::HString const&, void* );
	HQuery( HQuery const& );
	HQuery& operator = ( HQuery const& );
	friend class yaal::hcore::pointer_helper::HSpaceHolderDeleter<HQuery>;
	friend ptr_t yaal::hcore::make_pointer<
		HQuery,
		database_ptr_t,
		ODBConnector const*,
		yaal::hcore::HString,
		void*
			>( dbwrapper::database_ptr_t const&, dbwrapper::ODBConnector const* const&,
					yaal::hcore::HString const&, void* const& );
};

typedef yaal::hcore::HExceptionT<HQuery> HQueryException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HQUERY_HXX_INCLUDED */


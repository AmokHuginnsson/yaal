/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcruddescriptor.hxx - this file is integral part of `yaal' project.

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
/*! \file dbwrapper/hcruddescriptor.hxx
 * \brief Declaration of HCRUDDescriptor class.
 */

#ifndef YAAL_DBWRAPPER_HCRUDDESCRIPTOR_HXX_INCLUDED
#define YAAL_DBWRAPPER_HCRUDDESCRIPTOR_HXX_INCLUDED 1

#include "hcore/htime.hxx"
#include "dbwrapper/hquery.hxx"
#include "dbwrapper/hrecordset.hxx"

namespace yaal {

namespace dbwrapper {

/*! \brief Build a SQL query based on user supplied meta-data.
 */
class HCRUDDescriptor {
public:
	typedef HCRUDDescriptor this_type;
	typedef yaal::hcore::HPointer<this_type> ptr_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> field_names_t;
	/*! \brief Query types.
	 */
	enum class MODE {
		SELECT, /*!< SELECT query. */
		UPDATE, /*!< Data update query. */
		INSERT, /*!< INSERT new data query. */
		DELETE  /*!< DELETE data query. */
	};
private:
	MODE _mode;
	yaal::hcore::HString _varTmpBuffer;
	yaal::hcore::HString _SQL;
	yaal::hcore::HString _table;    /* table name */
	yaal::hcore::HString _columns;  /* columns that should be returned by next query */
	yaal::hcore::HString _idColumn; /* column that identifies row for UPDATE and DELETE query */
	yaal::hcore::HString _filter;   /* additional constant filter (WHERE clause) */
	yaal::hcore::HString _sort;     /* additional constant sort (ORDER BY clause) */
	typedef HRecordSet::values_t values_t;
	typedef yaal::hcore::HArray<bool> mutated_t;
	field_names_t _fields;
	int _fieldCount;		/* number of columns returned by last query */
	int long _setSize;		/* number of records returned by last query */
	values_t _values;
	database_ptr_t _dataBase;
	dbwrapper::HQuery::ptr_t _query;
	mutated_t _mutated;
public:
	HCRUDDescriptor( void );
	HCRUDDescriptor( yaal::dbwrapper::database_ptr_t );
	virtual ~HCRUDDescriptor( void );
	void set_table( yaal::hcore::HString const& );
	void set_columns( yaal::hcore::HString const& );
	void set_columns( field_names_t&& );
	void set_columns( field_names_t const& );
	void set_id_column( yaal::hcore::HString const& );
	void set_filter( yaal::hcore::HString const& );
	void set_sort( yaal::hcore::HString const& );
	yaal::hcore::HString get_table( void ) const;
	yaal::hcore::HString get_columns( void ) const;
	yaal::hcore::HString get_filter( void ) const;
	yaal::hcore::HString get_sort( void ) const;
	void sync( int, int long& );
	void sync( int, yaal::hcore::HString& );
	void sync( int, yaal::hcore::HTime& );
	MODE get_mode( void ) const;
	int long get_size( void ) const;
	HRecordSet::value_t& operator[]( int );
	void build_query( MODE );
	HRecordSet::ptr_t execute( MODE );
	HRecordSet::ptr_t execute( yaal::hcore::HString const& );
	HRecordSet::ptr_t execute( void );
private:
	void commit_columns( void );
	void verify_dml( void );
};
typedef yaal::hcore::HExceptionT<HCRUDDescriptor> HCRUDDescriptorException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HCRUDDESCRIPTOR_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
		CREATE, /*!< INSERT new data query. */
		READ,   /*!< Read one record with SELECT query. */
		UPDATE, /*!< Data UPDATE query. */
		DELETE  /*!< DELETE data query. */
	};
	struct OFilter {
		enum class CONDITION {
			NONE,
			EQUALS,
			NOT_EQUALS,
			LESS,
			LESS_OR_EQUAL,
			GREATER,
			GREATER_OR_EQUAL,
			IS_NULL,
			IS_NOT_NULL
		};
		yaal::hcore::HString _column;
		CONDITION _condition;
		OFilter( void )
			: _column()
			, _condition( CONDITION::NONE ) {
		}
	};
private:
	MODE _mode;
	yaal::hcore::HString _varTmpBuffer;
	yaal::hcore::HString _SQL;
	yaal::hcore::HString _table;    /* table name */
	yaal::hcore::HString _columns;  /* columns that should be returned by next query */
	yaal::hcore::HString _idColumn; /* column that identifies row for UPDATE and DELETE query */
	OFilter _filter[4];   /* additional constant filter (WHERE clause) */
	yaal::hcore::HString _filterValue;
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
	void set_filter( yaal::hcore::HString const&, OFilter::CONDITION = OFilter::CONDITION::EQUALS );
	void set_filter( yaal::hcore::HString const&, MODE, OFilter::CONDITION = OFilter::CONDITION::EQUALS );
	void set_filter_value( yaal::hcore::HString const& );
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


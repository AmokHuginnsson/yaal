/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED
#define YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/harray.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hpointer.hxx"
#include "tools/hoptional.hxx"
#include "dbwrapper/db_driver.hxx"

namespace yaal {

namespace dbwrapper {

/*! \brief Relational database query result representation.
 */
class HRecordSet {
	typedef HRecordSet this_type;
public:
	typedef yaal::hcore::HPointer<HRecordSet> ptr_t;
	typedef yaal::tools::HOptional<yaal::hcore::HString> value_t;
	typedef yaal::hcore::HArray<value_t> values_t;
	enum class CURSOR {
		FORWARD,
		RANDOM_ACCESS
	};
	class HIterator;
	typedef HIterator iterator;
private:
	mutable database_ptr_t _dataBase; /*!< data-base that this record-set belongs to */
	mutable ODBConnector const* _connector; /*!< low level database engine connector */
	mutable void* _result; /*!< very internal for this class used only in base class */
	CURSOR _cursor; /*!< Query result cursor type. */
	bool _iterating;
public:
	HRecordSet( database_ptr_t, ODBConnector const*, void*, CURSOR );
	virtual ~HRecordSet( void );
	void clear( void );
	iterator begin( void );
	iterator end( void );
	iterator rbegin( void );
	iterator rend( void );
	value_t get( int, int );
	bool is_empty( void ) const;
	int get_field_count( void ) const;
	int long get_size( void ) const;
	int long get_dml_size( void ) const;
	char const* get_column_name( int ) const;
	int long get_insert_id( void ) const;
	char const* get_error( void ) const;
	int get_errno( void ) const;
private:
	HRecordSet( HRecordSet const& );
	HRecordSet& operator = ( HRecordSet const& );
};

/*! \brief Query result iterator.
 */
class HRecordSet::HIterator {
	HRecordSet* _owner;
	int long _cursorPosition; /* cursor position in record-set */
public:
	HIterator( HIterator const& );
	HIterator( HIterator&& );
	HIterator& operator = ( HIterator const& );
	HIterator& operator = ( HIterator&& );
	HIterator& operator ++ ( void );
	HIterator& operator -- ( void );
	HIterator operator ++ ( int );
	HIterator operator -- ( int );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	HRecordSet::values_t operator* ( void ) const;
	HRecordSet::value_t operator[] ( int ) const;
private:
	HIterator( HRecordSet*, int long );
	friend class HRecordSet;
};

typedef yaal::hcore::HExceptionT<HRecordSet> HRecordSetException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED */


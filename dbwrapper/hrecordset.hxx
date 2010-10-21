/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED
#define YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/harray.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hpointer.hxx"
#include "dbwrapper/db_driver_loader.hxx"

namespace yaal
{

namespace dbwrapper
{

class HDataBase;
typedef yaal::hcore::HPointer<HDataBase> database_ptr_t;

/*! \brief Relational database query result representation.
 */
class HRecordSet
	{
	typedef HRecordSet this_type;
public:
	typedef yaal::hcore::HPointer<HRecordSet> ptr_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> values_t;
	class HIterator;
	typedef HIterator iterator;
private:
	mutable database_ptr_t _dataBase; /*!< data-base that this record-set belongs to */
	mutable ODBConnector const* _connector; /*!< low level database engine connector */
	mutable void* _result;	/*!< very internal for this class used only in base cla */
public:
	HRecordSet( database_ptr_t, ODBConnector const*, void* );
	virtual ~HRecordSet( void );
	void clear( void );
	iterator begin( void );
	iterator end( void );
	iterator rbegin( void );
	iterator rend( void );
	bool is_empty( void ) const;
	int get_field_count( void ) const;
	int long get_size( void ) const;
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
class HRecordSet::HIterator
	{
	HRecordSet* _owner;
	int long _cursorPosition; /* cursor position in record-set */
public:
	HIterator( HIterator const& );
	HIterator& operator = ( HIterator const& );
	HIterator& operator ++ ( void );
	HIterator& operator -- ( void );
	HIterator operator ++ ( int );
	HIterator operator -- ( int );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	yaal::hcore::HString operator[] ( int ) const;
private:
	HIterator( HRecordSet*, int long );
	friend class HRecordSet;
	};

/*! \brief Build a SQL query based on user supplied meta-data.
 */
class HSQLDescriptor
	{
public:
	typedef HSQLDescriptor this_type;
	typedef yaal::hcore::HPointer<this_type> ptr_t;
	/*! \brief Query types.
	 */
	struct MODE
		{
		/*! \brief Query types.
		 */
		typedef enum
			{
			SELECT, /*!< SELECT query. */
			UPDATE, /*!< Data update query. */
			INSERT, /*!< INSERT new data query. */
			DELETE  /*!< DELETER data query. */
			} mode_t;
		};
private:
	MODE::mode_t _mode;
	yaal::hcore::HString _varTmpBuffer;
	yaal::hcore::HString _SQL;
	yaal::hcore::HString _table;			/* table name */
	yaal::hcore::HString _columns;		/* columns that should be returned by next query */
	yaal::hcore::HString _filter;		/* additional constant filter (WHERE clause) */
	yaal::hcore::HString _sort;			/* additional constant sort (ORDER BY clause) */
	typedef yaal::hcore::HArray<yaal::hcore::HString> fields_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> values_t;
	typedef yaal::hcore::HArray<bool> mutated_t;
	fields_t _fields;
	int _fieldCount;		/* number of columns returned by last query */
	int long _setSize;		/* number of records returned by last query */
	values_t _values;
	database_ptr_t _dataBase;
	mutated_t _mutated;
public:
	HSQLDescriptor( void );
	HSQLDescriptor( yaal::dbwrapper::database_ptr_t );
	virtual ~HSQLDescriptor( void );
	void set_table( yaal::hcore::HString const& );
	void set_columns( yaal::hcore::HString const& );
	void set_filter( yaal::hcore::HString const& );
	void set_sort( yaal::hcore::HString const& );
	yaal::hcore::HString get_table( void ) const;
	yaal::hcore::HString get_columns( void ) const;
	yaal::hcore::HString get_filter( void ) const;
	yaal::hcore::HString get_sort( void ) const;
	MODE::mode_t get_mode( void ) const;
	int long get_size( void ) const;
	void sync( int, int long& );
	void sync( int, yaal::hcore::HString& );
	void sync( HRecordSet::iterator const& );
	yaal::hcore::HString& operator[]( int );
	yaal::hcore::HString const& build_sql( MODE::mode_t const& );
	HRecordSet::ptr_t execute( MODE::mode_t const& );
	HRecordSet::ptr_t execute( char const* const );
	HRecordSet::ptr_t execute( void );
	};

typedef yaal::hcore::HExceptionT<HRecordSet> HRecordSetException;
typedef yaal::hcore::HExceptionT<HSQLDescriptor> HSQLDescriptorException;

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HRECORDSET_HXX_INCLUDED */


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_DBWRAPPER_HRECORDSET_H
#define __YAAL_DBWRAPPER_HRECORDSET_H

#include "hcore/hlist.h"
#include "hcore/harray.h"
#include "hcore/hstring.h"
#include "hcore/hpointer.h"

namespace yaal
{

namespace dbwrapper
{

class HDataBase;
typedef yaal::hcore::HPointer<HDataBase> database_ptr_t;

class HRecordSet
	{
public:
	typedef yaal::hcore::HArray<yaal::hcore::HString> column_names_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> values_t;
	class HIterator;
	typedef HIterator iterator;
private:
	database_ptr_t f_oDataBase; /* data-base that this record-set belongs to */
	void* f_pvResult;	/* very internal for this class used only in base cla */
	int f_iFieldCount;		/* number of columns returned by last query */
	int f_iSetSize;		/* number of records returned by last query */
	column_names_t f_oColumnNames; /* column names returned by last query */
public:
	HRecordSet( database_ptr_t, void* );
	virtual ~HRecordSet( void );
	HRecordSet( HRecordSet const& );
	HRecordSet& operator = ( HRecordSet const& );
	void clear( void );
	iterator begin( void );
	iterator end( void );
	iterator rbegin( void );
	iterator rend( void );
	bool is_empty( void ) const;
	int field_count( void ) const;
	int long insert_id( void );
	};

class HRecordSet::HIterator
	{
	HRecordSet* f_poOwner;
	int f_iCursorPosition; /* cursor position in record-set */
public:
	HIterator( HIterator const& );
	HIterator& operator = ( HIterator const& );
	HIterator& operator ++ ( void );
	HIterator& operator -- ( void );
	HIterator operator ++ ( int );
	HIterator operator -- ( int );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	yaal::hcore::HString operator[] ( int const& );
	yaal::hcore::HString operator[] ( char const * const );
private:
	HIterator( HRecordSet*, int const& );
	friend class HRecordSet;
	};

class HSQLDescriptor
	{
public:
	typedef HSQLDescriptor self_t;
	typedef yaal::hcore::HPointer<self_t> ptr_t;
	struct MODE
		{
		typedef enum
			{
			D_SELECT,
			D_UPDATE,
			D_INSERT,
			D_DELETE
			} mode_t;
		};
private:
	yaal::hcore::HString f_oVarTmpBuffer;
	yaal::hcore::HString f_oSQL;
	yaal::hcore::HString f_oTable;			/* table name */
	yaal::hcore::HString f_oColumns;		/* columns that should be returned by next query */
	yaal::hcore::HString f_oFilter;		/* additional constant filter (WHERE clause) */
	yaal::hcore::HString f_oSort;			/* additional constant sort (ORDER BY clause) */
	typedef yaal::hcore::HArray<yaal::hcore::HString> fields_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> values_t;
	fields_t f_oFields;
	values_t f_oValues;
public:
	HSQLDescriptor( void );
	yaal::hcore::HString const& build_sql( MODE::mode_t const& );
	};

typedef yaal::hcore::HExceptionT<HRecordSet> HRecordSetException;

}

}

#endif /* not __YAAL_DBWRAPPER_HRECORDSET_H */

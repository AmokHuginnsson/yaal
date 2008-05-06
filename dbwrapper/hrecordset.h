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
	typedef yaal::hcore::HList<yaal::hcore::HString> column_names_t;
	typedef yaal::hcore::HList<yaal::hcore::HString> values_t;
	class HIterator;
	typedef HIterator iterator;
private:
	database_ptr_t f_oDataBase; /* data-base that this record-set belongs to */
	void* f_pvCoreData;	/* very internal for this class used only in base cla */
	yaal::hcore::HString f_oVarTmpBuffer;
	int f_iFieldCount;		/* number of columns returned by last query */
	int f_iSetSize;		/* number of records returned by last query */
	column_names_t f_oColumnNames; /* column names returned by last query */
public:
	HRecordSet( database_ptr_t, void* );
	virtual ~HRecordSet( void );
private:
	HRecordSet( HRecordSet const& );
	HRecordSet& operator = ( HRecordSet const& );
	};

class HRecordSet::HIterator
	{
	
	int f_iCursorPosition; /* cursor position in record-set */
	};

class HSQLDescriptor
	{
private:
	yaal::hcore::HString f_oTable;			/* table name */
	yaal::hcore::HString f_oColumns;		/* columns that should be returned by next query */
	yaal::hcore::HString f_oFilter;		/* additional constant filter (WHERE clause) */
	yaal::hcore::HString f_oSort;			/* additional constant sort (ORDER BY clause) */
public:
	HSQLDescriptor( void );
	yaal::hcore::HString build_sql( void );
	};

}

}

#endif /* not __YAAL_DBWRAPPER_HRECORDSET_H */

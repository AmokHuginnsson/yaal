/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.h - this file is integral part of `stdhapi' project.

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

#ifndef __DBWRAPPER_HRECORDSET_H
#define __DBWRAPPER_HRECORDSET_H

#include "hcore/hlist.h"
#include "hcore/hstring.h"
#include "hcore/htime.h"
#include "hcore/hinfo.h"
#include "hdatabase.h"

namespace stdhapi
{

namespace dbwrapper
{

class HRecordSet
	{
public:
	typedef enum
		{
		D_CLOSED,
		D_OPEN,
		D_NORMAL = D_OPEN,
		D_ADDING,
		D_EDITING
		} mode_t;
private:
	/*{*/
	void * f_pvCoreData;	/* very internal for this class used only in base cla */
	hcore::HString f_oSQL;
	hcore::HString f_oVarTmpBuffer;
	/*}*/
protected:
	/*{*/
	int f_iIdFieldOffset;	/* number (count from 0) of id column in result record-set */
	int f_iFieldCount;		/* number of columns returned by last query */
	mode_t f_eMode;					/* normal(opened), closed, adding, editing */
	int f_iCursorPosition;/* cursor position in record-set */
	int f_iSetQuantity;		/* number of records returned by last query */
	hcore::HString f_oTable;			/* table name */
	hcore::HString f_oColumns;		/* columns that should be returned by next query */
	hcore::HString f_oFilter;		/* additional constant filter (WHERE clause) */
	hcore::HString f_oSort;			/* additional constant sort (ORDER BY clause) */
	hcore::HList< hcore::HString >	f_oColumnNames; /* column names returned by last query */
	hcore::HList< hcore::HString > f_oValues;	/* values returned by last cursor movement */
	HDataBase * f_poDataBase; /* data-base that this record-set belongs to */
	/*}*/
public:
	/*{*/
	hcore::HString m_oFilter;		/* additional variable filter (WHERE clause) */
	hcore::HString m_oSort;			/* additional variable sort (ORDER BY clause) */
	int long m_lId;
	/*}*/
protected:
	/*{*/
	void build_sql ( void );
	virtual void sync ( void );
	void sync ( int, char & );
	void sync ( int, short & );
	void sync ( int, int & );
	void sync ( int, int long & );
	void sync ( int, double & );
	void sync ( int, hcore::HString & );
	void sync ( int, hcore::HTime & );
	void sync ( int, hcore::HInfo & );
	/*}*/
public:
	/*{*/
	HRecordSet ( HDataBase * );
	virtual ~HRecordSet ( void );
	hcore::HString get ( int );
	bool is_eof ( void ) const;
	bool is_bof ( void ) const;
	bool is_open ( void ) const;
	void move_first ( void );
	void move_last ( void );
	void move_next ( void );
	void move_previous ( void );
	int long open ( char const * = NULL );
	void close ( void );
	void cancel ( void );
	void free ( void );
	int long requery ( char const * = NULL );
	void add_new ( void );
	void edit ( void );
	int long update ( void );
	void remove ( void );
	/*}*/
private:
	/*{*/
	HRecordSet ( HRecordSet const & );
	HRecordSet & operator = ( HRecordSet const & );
	/*}*/
	};

}

}

#endif /* not __DBWRAPPER_HRECORDSET_H */

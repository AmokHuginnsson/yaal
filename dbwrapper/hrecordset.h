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

#ifndef __HRECORDSET_H
#define __HRECORDSET_H

/* modes */
#define D_CLOSED	-	1
#define D_NORMAL		0
#define D_ADDING		1
#define D_EDITING		2

#include "../hcore/hobject.h"
#include "../hcore/hstring.h"
#include "../hcore/hstringlist.h"
#include "../hcore/htime.h"
#include "../hcore/hinfo.h"
#include "hdatabase.h"

class HRecordSet : public HObject
	{
private:
	/*{*/
	void * f_pvCoreData;	/* very internal for this class used only in base cla */
	HString f_oSQL;
	HString f_oBuffer;
	/*}*/
protected:
	/*{*/
	int f_iFieldCount;
	int f_iMode;					/* normal(opened), closed, adding, editing */
	int f_iCursorPosition;
	int f_iSetQuantity;
	HString f_oTable;
	HStringList	f_oColumnNames;
	HStringList f_oValues;
	HDataBase * f_poDataBase;
	/*}*/
public:
	/*{*/
	HString m_oFilter;
	HString m_oSort;
	long int m_id;
	/*}*/
protected:
	/*{*/
	virtual void sync ( void );
	void sync ( int, char & );
	void sync ( int, short & );
	void sync ( int, int & );
	void sync ( int, long int & );
	void sync ( int, double & );
	void sync ( int, HString & );
	void sync ( int, HTime & );
	void sync ( int, HInfo & );
	/*}*/
public:
	/*{*/
	HRecordSet ( HDataBase * );
	virtual ~HRecordSet ( void );
	HObject * clone ( void ) const;
	HString get ( int );
	bool is_eof ( void );
	bool is_bof ( void );
	bool is_open ( void );
	void move_first ( void );
	void move_last ( void );
	void move_next ( void );
	void move_previous ( void );
	long int open ( const char * = NULL );
	void close ( void );
	long int requery ( const char * = NULL );
	void add_new ( void );
	void edit ( void );
	long int update ( void );
	void remove ( void );
	/*}*/
	};

#endif /* __HRECORDSET_H */

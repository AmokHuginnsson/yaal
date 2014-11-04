/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.hxx - this file is integral part of `yaal' project.

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
	struct CURSOR {
		typedef enum {
			FORWARD,
			RANDOM_ACCESS
		} cursor_t;
	};
	class HIterator;
	typedef HIterator iterator;
private:
	mutable database_ptr_t _dataBase; /*!< data-base that this record-set belongs to */
	mutable ODBConnector const* _connector; /*!< low level database engine connector */
	mutable void* _result; /*!< very internal for this class used only in base class */
	CURSOR::cursor_t _cursor; /*!< Query result cursor type. */
public:
	HRecordSet( database_ptr_t, ODBConnector const*, void*, CURSOR::cursor_t );
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
	HIterator& operator = ( HIterator const& );
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


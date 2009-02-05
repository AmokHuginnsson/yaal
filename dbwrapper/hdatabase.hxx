/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatabase.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED
#define YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "dbwrapper/hrecordset.hxx"

namespace yaal
{

namespace dbwrapper
{

/*! \brief Data base access abstraction layer.
 */
class HDataBase : public yaal::hcore::HPointerFromThisInterface<HDataBase>
	{
	typedef HDataBase self_t;
private:
	void* f_pvCoreData;	/* very internal for this class used only in base cla */
public:
	typedef yaal::hcore::HPointer<HDataBase> ptr_t;
	int connect( yaal::hcore::HString const&,		/* database */
							yaal::hcore::HString const&,		/* login */
							yaal::hcore::HString const& );	/* password */
	void disconnect( void );
	HRecordSet::ptr_t query( yaal::hcore::HString const& );
	char const* get_error( void ) const;
	int get_errno( void ) const;
	static ptr_t get_connector( void );
private:
	HDataBase( void );
	virtual ~HDataBase( void );
	HDataBase( HDataBase const& );
	HDataBase& operator = ( HDataBase const& );
	friend struct yaal::hcore::HPointerScalar<HDataBase>;
	};

typedef yaal::hcore::HExceptionT<HDataBase> HDataBaseException;
typedef HDataBaseException HSQLException;

}

}

#endif /* not YAAL_DBWRAPPER_HDATABASE_HXX_INCLUDED */

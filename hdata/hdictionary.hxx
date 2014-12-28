/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdictionary.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HDATA_HDICTIONARY_HXX_INCLUDED
#define YAAL_HDATA_HDICTIONARY_HXX_INCLUDED 1

#include "dbwrapper/hcruddescriptor.hxx"

namespace yaal {

namespace hdata {

class HDictionary {
public:
	typedef HDictionary this_type;
	typedef yaal::hcore::HPointer<HDictionary> ptr_t;
	typedef yaal::hcore::HHashMap<int, yaal::hcore::HString> data_t;
private:
	data_t _data;
	dbwrapper::HCRUDDescriptor _crud;
public:
	HDictionary( yaal::dbwrapper::database_ptr_t const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const& );
	void load( void );
	yaal::hcore::HString const& operator[] ( int ) const;
	data_t const& data( void ) const;
private:
	HDictionary( HDictionary const& ) = delete;
	HDictionary& operator = ( HDictionary const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDICTIONARY_HXX_INCLUDED */

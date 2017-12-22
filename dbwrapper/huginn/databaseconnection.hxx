/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  databaseconnection.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED
#define YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hresource.hxx"
#include "dbwrapper/hdatabase.hxx"

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HDatabaseConnection : public tools::HHuginn::HValue {
public:
private:
	yaal::dbwrapper::database_ptr_t _database;
public:
	HDatabaseConnection( tools::HHuginn::HClass const*, yaal::dbwrapper::database_ptr_t );
	static tools::HHuginn::value_t query( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t table_names( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t column_names( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::class_t get_class( tools::huginn::HRuntime*, tools::HHuginn::class_t const& );
private:
	tools::HHuginn::value_t do_query( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
	tools::HHuginn::value_t do_table_names( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
	tools::HHuginn::value_t do_column_names( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
private:
	virtual tools::HHuginn::value_t do_clone( tools::huginn::HThread*, tools::HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED */


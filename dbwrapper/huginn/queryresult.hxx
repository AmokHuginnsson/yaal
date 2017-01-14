/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  queryresult.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED
#define YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "dbwrapper/hrecordset.hxx"

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HQueryResult : public tools::HHuginn::HIterable {
	dbwrapper::HRecordSet::ptr_t _recordSet;
	dbwrapper::HRecordSet::HIterator _it;
	tools::huginn::HRuntime* _runtime;
public:
	HQueryResult( tools::HHuginn::HClass const*, dbwrapper::HRecordSet::ptr_t const& );
	static tools::HHuginn::value_t column_name( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t const&, int );
	static tools::HHuginn::value_t field_count( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t const&, int );
	static tools::HHuginn::value_t insert_id( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t const&, int );
	static tools::HHuginn::value_t has_next( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t const&, int );
	static tools::HHuginn::value_t fetch_row( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t const&, int );
	static tools::HHuginn::class_t get_class( tools::huginn::HRuntime*, tools::HHuginn::class_t const& );
private:
	virtual HIterator do_iterator( tools::huginn::HThread*, int ) override;
	virtual int long do_size( void ) const override;
	virtual tools::HHuginn::value_t do_clone( tools::huginn::HThread*, int ) const override __attribute__((noreturn));
	HQueryResult( HQueryResult const& ) = delete;
	HQueryResult& operator = ( HQueryResult const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED */


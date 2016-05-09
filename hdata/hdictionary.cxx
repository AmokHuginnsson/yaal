/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hdictionary.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdictionary.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDictionary::HDictionary( yaal::dbwrapper::database_ptr_t const& db_,
		yaal::hcore::HString const& table_,
		yaal::hcore::HString const& idColumn_,
		yaal::hcore::HString const& valueColumn_ )
	: _data(), _crud( db_ ) {
	M_PROLOG
	_crud.set_table( table_ );
	_crud.set_columns( idColumn_ + ", " + valueColumn_ );
	return;
	M_EPILOG
}

void HDictionary::load( void ) {
	M_PROLOG
	_data.clear();
	HRecordSet::ptr_t rs( _crud.execute( HCRUDDescriptor::MODE::READ ) );
	_data.resize( rs->get_size() );
	for ( HRecordSet::values_t const& values : *rs ) {
		M_ENSURE( values[0] && values[1] );
		_data.insert( make_pair( lexical_cast<int>( *values[0] ), *values[1] ) );
	}
	return;
	M_EPILOG
}

yaal::hcore::HString const& HDictionary::operator[] ( int id_ ) const {
	M_PROLOG
	return ( _data.at( id_ ) );
	M_EPILOG
}

HDictionary::data_t const& HDictionary::data( void ) const {
	return ( _data );
}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

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


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hquery.hxx"
#include "hdatabase.hxx"
#include "hrecordset.hxx"
#include "dbwrapper.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace dbwrapper {

HQuery::HQuery(
		database_ptr_t database_,
		ODBConnector const* connector_,
		yaal::hcore::HString const& sql_,
		void* query_
) : _dataBase( database_ )
	, _connector( connector_ )
	, _query( query_ )
	, _sql( sql_ )
	, _bindBuffer()
	, _resultObserver() {
	return;
}

HQuery::~HQuery( void ) {
	if ( _query ) {
		(*_connector->query_free)( _dataBase->_dbLink, _query );
		_query = nullptr;
	}
	return;
}

void HQuery::bind( int parameterNo_, yaal::hcore::HString const& parameterValue_ ) {
	M_PROLOG
	(*_connector->query_bind)( _dataBase->_dbLink, _query, parameterNo_, _bindBuffer[parameterNo_] = parameterValue_ );
	return;
	M_EPILOG
}

HRecordSet::ptr_t HQuery::execute( void ) {
	M_PROLOG
	if ( ! _dataBase->_dbLink._valid ) {
		M_THROW( "not connected to database", errno );
	}
	if ( _logSQL_ ) {
		hcore::log << "SQL: " << _sql << endl;
	}
	HRecordSet::ptr_t rs( _resultObserver );
	if ( !! rs ) {
		throw HQueryException( "Previous execution on this query is still pending." );
	}
	void* result( (_connector->query_execute)( _dataBase->_dbLink, _query ) );
	if ( (_connector->dbrs_errno)( _dataBase->_dbLink, result ) ) {
		throw HSQLException( HString( "SQL error: " ) + (_connector->dbrs_error)( _dataBase->_dbLink, result ) );
	}
	rs = make_pointer<HRecordSet>( _dataBase, _connector, result, HRecordSet::CURSOR::FORWARD );
	_resultObserver = rs;
	return ( rs );
	M_EPILOG
}

}

}


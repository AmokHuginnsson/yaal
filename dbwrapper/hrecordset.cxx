/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <stdlib.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hrecordset.hxx"
#include "hdatabase.hxx"
#include "db_driver_loader.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace dbwrapper {

namespace {
static int const INVALID_CURSOR( -1 );
}

HRecordSet::HRecordSet(
	database_ptr_t dataBase_,
	ODBConnector const* connector_, void* result_, CURSOR cursor_
) : _dataBase( dataBase_ )
	, _connector( connector_ )
	, _result( result_ )
	, _cursor( cursor_ )
	, _iterating( false ) {
	M_PROLOG
	if ( ( _cursor == CURSOR::RANDOM_ACCESS ) && ( get_size() < 0 ) ) {
		hcore::log( LOG_LEVEL::ERROR ) << "SQL error (query): " << (_connector->dbrs_error)( _dataBase->_dbLink, _result ) << endl;
	}
	if ( get_field_count() < 0 ) {
		hcore::log( LOG_LEVEL::ERROR ) << "SQL error (fields count): " << (_connector->dbrs_error)( _dataBase->_dbLink, _result ) << endl;
	}
	return;
	M_EPILOG
}

HRecordSet::~HRecordSet( void ) {
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
}

void HRecordSet::clear( void ) {
	M_PROLOG
	if ( _result ) {
		if ( _cursor == CURSOR::RANDOM_ACCESS ) {
			(_connector->rs_free_query_result)( _result );
		} else {
			(_connector->rs_free_cursor)( _result );
		}
	}
	_result = nullptr;
	return;
	M_EPILOG
}

bool HRecordSet::is_empty( void ) const {
	return ( ! get_size() );
}

int HRecordSet::get_field_count( void ) const {
	return ( (_connector->rs_fields_count)( _result ) );
}

int long HRecordSet::get_size( void ) const {
	M_PROLOG
	M_ENSURE( _cursor == CURSOR::RANDOM_ACCESS );
	return ( (_connector->dbrs_records_count)( _dataBase->_dbLink, _result ) );
	M_EPILOG
}

int long HRecordSet::get_dml_size( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_records_count)( _dataBase->_dbLink, _result ) );
	M_EPILOG
}

char const* HRecordSet::get_column_name( int column_ ) const {
	return ( (_connector->rs_column_name)( _result, column_ ) );
}

int long HRecordSet::get_insert_id( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_id)( _dataBase->_dbLink, _result ) );
	M_EPILOG
}

char const* HRecordSet::get_error( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_error)( _dataBase->_dbLink, _result ) );
	M_EPILOG
}

int HRecordSet::get_errno( void ) const {
	M_PROLOG
	return ( (_connector->dbrs_errno)( _dataBase->_dbLink, _result ) );
	M_EPILOG
}

HRecordSet::value_t HRecordSet::get( int cursor_, int field_ ) {
	M_PROLOG
	M_ENSURE( _cursor == CURSOR::RANDOM_ACCESS );
	char const* valRaw( (_connector->rs_get)( _result, cursor_, field_ ) );
	value_t value;
	if ( valRaw ) {
		value = value_t( valRaw );
	}
	return ( value );
	M_EPILOG
}

HRecordSet::iterator HRecordSet::begin( void ) {
	M_PROLOG
	M_ENSURE( ( _cursor == CURSOR::RANDOM_ACCESS ) || ! _iterating );
	_iterating = true;
	return ( iterator( this, ( _cursor == CURSOR::RANDOM_ACCESS ) ? 0 : ( (_connector->rs_next( _result ) ? 0 : INVALID_CURSOR ) ) ) );
	M_EPILOG
}

HRecordSet::iterator HRecordSet::end( void ) {
	return ( iterator( this, ( _cursor == CURSOR::RANDOM_ACCESS ) ? get_size() : INVALID_CURSOR ) );
}

HRecordSet::iterator HRecordSet::rbegin( void ) {
	M_PROLOG
	M_ENSURE( _cursor == CURSOR::RANDOM_ACCESS );
	return ( iterator( this, get_size() - 1 ) );
	M_EPILOG
}

HRecordSet::iterator HRecordSet::rend( void ) {
	M_PROLOG
	M_ENSURE( _cursor == CURSOR::RANDOM_ACCESS );
	return ( iterator( this, -1 ) );
	M_EPILOG
}

HRecordSet::HIterator::HIterator( HRecordSet* owner_, int long position_ )
	: _owner( owner_ )
	, _cursorPosition( position_ ) {
	return;
}

HRecordSet::HIterator::HIterator( HIterator const& it )
	: _owner( it._owner )
	, _cursorPosition( it._cursorPosition ) {
	M_ASSERT( it._owner->_cursor == CURSOR::RANDOM_ACCESS );
	return;
}

HRecordSet::HIterator::HIterator( HIterator&& it_ )
	: _owner( it_._owner )
	, _cursorPosition( it_._cursorPosition ) {
	it_._owner = nullptr;
	it_._cursorPosition = INVALID_CURSOR;
	return;
}

HRecordSet::HIterator& HRecordSet::HIterator::operator = ( HIterator const& it ) {
	M_ASSERT( it._owner->_cursor == CURSOR::RANDOM_ACCESS );
	if ( &it != this ) {
		_owner = it._owner;
		_cursorPosition = it._cursorPosition;
	}
	return ( *this );
}

HRecordSet::HIterator& HRecordSet::HIterator::operator = ( HIterator&& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_cursorPosition = it_._cursorPosition;
		it_._owner = nullptr;
		it_._cursorPosition = INVALID_CURSOR;
	}
	return ( *this );
}

bool HRecordSet::HIterator::operator == ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._cursorPosition == _cursorPosition );
	M_EPILOG
}

bool HRecordSet::HIterator::operator != ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._cursorPosition != _cursorPosition );
	M_EPILOG
}

HRecordSet::HIterator& HRecordSet::HIterator::operator ++ ( void ) {
	M_PROLOG
	M_ASSERT( _owner );
	M_ENSURE( _cursorPosition != INVALID_CURSOR );
	++ _cursorPosition;
	if ( _owner->_cursor == HRecordSet::CURSOR::FORWARD ) {
		if ( ! _owner->_connector->rs_next( _owner->_result ) ) {
			_cursorPosition = INVALID_CURSOR;
		}
	}
	return ( *this );
	M_EPILOG
}

HRecordSet::HIterator& HRecordSet::HIterator::operator -- ( void ) {
	M_PROLOG
	M_ASSERT( _owner );
	M_ENSURE( _cursorPosition != INVALID_CURSOR );
	M_ENSURE( _owner->_cursor == HRecordSet::CURSOR::RANDOM_ACCESS );
	-- _cursorPosition;
	return ( *this );
	M_EPILOG
}

HRecordSet::HIterator HRecordSet::HIterator::operator ++ ( int ) {
	M_PROLOG
	M_ASSERT( _owner );
	HIterator it( *this );
	operator ++ ();
	return ( it );
	M_EPILOG
}

HRecordSet::HIterator HRecordSet::HIterator::operator -- ( int ) {
	M_PROLOG
	M_ASSERT( _owner );
	HIterator it( *this );
	operator -- ();
	return ( it );
	M_EPILOG
}

HRecordSet::value_t HRecordSet::HIterator::operator[] ( int field_ ) const {
	M_PROLOG
	M_ASSERT( _owner );
	M_ENSURE( _cursorPosition != INVALID_CURSOR );
	char const* valRaw( ( _owner->_cursor == HRecordSet::CURSOR::RANDOM_ACCESS )
			? (_owner->_connector->rs_get)( _owner->_result, _cursorPosition, field_ )
			: (_owner->_connector->rs_get_field)( _owner->_result, field_ ) );
	value_t value;
	if ( valRaw ) {
		value = value_t( valRaw );
	}
	return ( value );
	M_EPILOG
}

HRecordSet::values_t HRecordSet::HIterator::operator* ( void ) const {
	M_PROLOG
	HRecordSet::values_t values;
	int fieldCount( _owner->get_field_count() );
	if ( fieldCount > 0 ) {
		values.reserve( _owner->get_field_count() );
		for ( int i( 0 ); i < fieldCount; ++ i ) {
			char const* valRaw( ( _owner->_cursor == HRecordSet::CURSOR::RANDOM_ACCESS )
					? (_owner->_connector->rs_get)( _owner->_result, _cursorPosition, i )
					: (_owner->_connector->rs_get_field)( _owner->_result, i ) );
			if ( valRaw ) {
				values.emplace_back( valRaw );
			} else {
				values.emplace_back();
			}
		}
	}
	return ( values );
	M_EPILOG
}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.cxx - this file is integral part of `yaal' project.

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

#include <stdlib.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hrecordset.hxx"
#include "hdatabase.hxx"
#include "db_driver_loader.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace dbwrapper
{

char _eMode_ [ ] = "record set is not in appropriate mode for operation";

HRecordSet::HRecordSet( database_ptr_t dataBase_,
		ODBConnector const* connector_, void* reuslt_ )
	: _dataBase( dataBase_ ), _connector( connector_ ),
	_result( reuslt_ )
	{
	M_PROLOG
	if ( get_size() < 0 )
		log( LOG_TYPE::ERROR ) << "SQL error (query): " << (_connector->dbrs_error)( _dataBase->_coreData, _result ) << endl;
	if ( get_field_count() < 0 )
		log( LOG_TYPE::ERROR ) << "SQL error (fiels count): " << (_connector->dbrs_error)( _dataBase->_coreData, _result ) << endl;
	return;
	M_EPILOG
	}

HRecordSet::~HRecordSet ( void )
	{
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
	}

void HRecordSet::clear( void )
	{
	M_PROLOG
	if ( _result )
		(_connector->rs_unquery)( _result );
	_result = NULL;
	M_EPILOG
	}

bool HRecordSet::is_empty( void ) const
	{
	return ( ! get_size() );
	}

int HRecordSet::get_field_count( void ) const
	{
	return ( (_connector->rs_fields_count)( _result ) );
	}

int long HRecordSet::get_size( void ) const
	{
	return ( (_connector->dbrs_records_count)( _dataBase->_coreData, _result ) );
	}

char const* HRecordSet::get_column_name( int column_ ) const
	{
	return ( (_connector->rs_column_name)( _result, column_ ) );
	}

int long HRecordSet::get_insert_id( void ) const
	{
	M_PROLOG
	if ( ! _result )
		M_THROW( "no result", errno );
	return ( (_connector->dbrs_id)( _dataBase->_coreData, _result ) );
	M_EPILOG
	}

char const* HRecordSet::get_error( void ) const
	{
	M_PROLOG
	return ( (_connector->dbrs_error)( _dataBase->_coreData, _result ) );
	M_EPILOG
	}

int HRecordSet::get_errno( void ) const
	{
	M_PROLOG
	return ( (_connector->dbrs_errno)( _dataBase->_coreData, _result ) );
	M_EPILOG
	}

HRecordSet::iterator HRecordSet::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

HRecordSet::iterator HRecordSet::end( void )
	{
	return ( iterator( this, get_size() ) );
	}

HRecordSet::iterator HRecordSet::rbegin( void )
	{
	return ( iterator( this, get_size() - 1 ) );
	}

HRecordSet::iterator HRecordSet::rend( void )
	{
	return ( iterator( this, -1 ) );
	}

HSQLDescriptor::HSQLDescriptor( void )
	: _mode( MODE::SELECT ), _varTmpBuffer(), _SQL(), _table(),
	_columns ( "*" ), _filter(), _sort(), _fields(), _fieldCount( 0 ),
	_setSize( 0 ), _values(), _dataBase(), _mutated()
	{
	return;
	}

HSQLDescriptor::HSQLDescriptor( database_ptr_t dataBase_ )
	: _mode( MODE::SELECT ), _varTmpBuffer(), _SQL(), _table(),
	_columns ( "*" ), _filter(), _sort(), _fields(), _fieldCount( 0 ),
	_setSize( 0 ), _values(), _dataBase( dataBase_ ), _mutated()
	{
	return;
	}

HSQLDescriptor::~HSQLDescriptor( void )
	{
	}

HString const& HSQLDescriptor::build_sql( MODE::mode_t const& mode_ )
	{
	M_PROLOG
	_varTmpBuffer = "";
	switch ( mode_ )
		{
		case ( MODE::SELECT ):
			{
			_SQL.format( "SELECT %s FROM %s", _columns.raw(),
					_table.raw() );
			if ( ! _filter.is_empty() )
				_SQL += ( " WHERE " + _filter );
			if ( ! _sort.is_empty() )
				_SQL += ( " ORDER BY " + _sort );
			_SQL += ';';
			}
		break;
		case ( MODE::UPDATE ):
			{
			_SQL = "UPDATE " + _table + " SET ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size = _values.get_size();
			bool hasField = false;
			for ( int i = 0; i < size; ++ i )
				{
				if ( _mutated[ i ] )
					{
					if ( hasField )
						_SQL += ", ";
					hasField = true;
					_SQL += _fields[ i ];
					if ( _values[ i ] )
						{
						_SQL += " = '";
						_SQL += *_values[ i ];
						_SQL += "'";
						}
					else
						_SQL += " = NULL";
					}
				}
			if ( ! _filter.is_empty() )
				{
				_SQL += " WHERE ";
				_SQL += _filter;
				}
			_SQL += ';';
			}
		break;
		case ( MODE::INSERT ):
			{
			_SQL = "INSERT INTO " + _table + " ( ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size = _fields.get_size();
			for ( int i = 0; i < size; ++ i )
				{
				if ( i > 0 )
					_SQL += ", ";
				_SQL += _fields[ i ];
				}
			_SQL += " ) VALUES ( ";
			for ( int i = 0; i < size; ++ i )
				{
				if ( i > 0 )
					_SQL += ", ";
				_SQL += "'";
				_SQL += _fields[ i ];
				_SQL += "'";
				}
			_SQL += " );";
			}
		break;
		case ( MODE::DELETE ):
			{
			_SQL = "DELETE FROM ";
			_SQL += _table;
			if ( ! _filter.is_empty() )
				{
				_SQL += " WHERE ";
				_SQL += _filter;
				}
			_SQL += ";";
			}
		break;
		default :
			M_THROW( _eMode_, mode_ );
		}
	_mode = mode_;
	return ( _SQL );
	M_EPILOG
	}

void HSQLDescriptor::sync( int field_, HString& value )
	{
	M_PROLOG
	if ( _mode == MODE::SELECT )
		{
		if ( _values[ field_ ] )
			value = *_values[ field_ ];
		else
			value.clear();
		}
	else
		_values[ field_ ] = value;
	M_EPILOG
	}

void HSQLDescriptor::sync( int field_, int long& value )
	{
	M_PROLOG
	if ( _mode == MODE::SELECT )
		{
		if ( _values[ field_ ] )
			value = lexical_cast<int long>( *_values[ field_ ] );
		else
			value = 0;
		}
	else
		_values[ field_ ] = HRecordSet::value_t( value );
	M_EPILOG
	}

void HSQLDescriptor::set_table( yaal::hcore::HString const& table_ )
	{
	M_PROLOG
	_table = table_;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_columns( yaal::hcore::HString const& columns_ )
	{
	M_PROLOG
	_columns = columns_;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_filter( yaal::hcore::HString const& filter_ )
	{
	M_PROLOG
	_filter = filter_;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_sort( yaal::hcore::HString const& sort_ )
	{
	M_PROLOG
	_sort = sort_;
	return;
	M_EPILOG
	}

HString HSQLDescriptor::get_table( void ) const
	{
	return ( _table );
	}

HString HSQLDescriptor::get_columns( void ) const
	{
	return ( _columns );
	}

HString HSQLDescriptor::get_filter( void ) const
	{
	return ( _filter );
	}

HString HSQLDescriptor::get_sort( void ) const
	{
	return ( _sort );
	}

HSQLDescriptor::MODE::mode_t HSQLDescriptor::get_mode( void ) const
	{
	return ( _mode );
	}

int long HSQLDescriptor::get_size( void ) const
	{
	return ( _setSize );
	}

HRecordSet::HIterator::HIterator( HRecordSet* owner_, int long position_ )
	: _owner( owner_ ), _cursorPosition( position_ )
	{
	}

HRecordSet::HIterator::HIterator( HIterator const& it )
	: _owner( it._owner ), _cursorPosition( it._cursorPosition )
	{
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator = ( HIterator const& it )
	{
	if ( &it != this )
		{
		_owner = it._owner;
		_cursorPosition = it._cursorPosition;
		}
	return ( *this );
	}

bool HRecordSet::HIterator::operator == ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._cursorPosition == _cursorPosition );
	M_EPILOG
	}

bool HRecordSet::HIterator::operator != ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._cursorPosition != _cursorPosition );
	M_EPILOG
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator ++ ( void )
	{
	M_PROLOG
	M_ASSERT( _owner );
	++ _cursorPosition;
	return ( *this );
	M_EPILOG
	}

HRecordSet::HIterator HRecordSet::HIterator::operator ++ ( int )
	{
	M_PROLOG
	M_ASSERT( _owner );
	HIterator it( *this );
	++ _cursorPosition;
	return ( it );
	M_EPILOG
	}

HRecordSet::value_t HRecordSet::HIterator::operator[] ( int field_ ) const
	{
	M_PROLOG
	M_ASSERT( _owner );
	char const* valRaw( (_owner->_connector->rs_get)( _owner->_result, _cursorPosition, field_ ) );
	value_t value;
	if ( valRaw )
		value = value_t( valRaw );
	return ( value );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( void )
	{
	M_PROLOG
	HRecordSet::ptr_t rs = _dataBase->query( _SQL );
	_fieldCount = rs->get_field_count();
	if ( _fields.get_size() != _fieldCount )
		{
		_fields = fields_t( _fieldCount );
		_values = values_t( _fieldCount );
		_mutated = mutated_t( _fieldCount );
		}
	for ( int ctr = 0; ctr < _fieldCount; ++ ctr )
		{
		_fields[ ctr ] = rs->get_column_name( ctr ); 
		_mutated[ ctr ] = false;
		}
	_setSize = rs->get_size();
	return ( rs );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( MODE::mode_t const& mode_ )
	{
	M_PROLOG
	build_sql( mode_ );
	return ( execute() );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( char const* const query_ )
	{
	M_PROLOG
	_SQL = query_;
	return ( execute() );
	M_EPILOG
	}

void HSQLDescriptor::sync( HRecordSet::iterator const& it )
	{
	M_PROLOG
	for ( int ctr = 0; ctr < _fieldCount; ++ ctr )
		_values[ ctr ] = it[ ctr ];
	M_EPILOG
	}

HRecordSet::value_t& HSQLDescriptor::operator[]( int column_ )
	{
	M_PROLOG
	M_ASSERT( ( column_ >= 0 ) && ( column_ <= _fieldCount ) );
	_mutated[ column_ ] = true;
	return ( _values[ column_ ] );
	M_EPILOG
	}

}

}


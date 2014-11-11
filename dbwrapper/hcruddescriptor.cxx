/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcruddescriptor.cxx - this file is integral part of `yaal' project.

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
#include "hcruddescriptor.hxx"
#include "hdatabase.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace dbwrapper {

namespace {
char const _eMode_[] = "record set is not in appropriate mode for operation";
}

HCRUDDescriptor::HCRUDDescriptor( void )
	: _mode( MODE::SELECT ), _varTmpBuffer(), _SQL(), _table(),
	_columns( "*" ), _filter(), _sort(), _fields(), _fieldCount( 0 ),
	_setSize( 0 ), _values(), _dataBase(), _query(), _mutated() {
	return;
}

HCRUDDescriptor::HCRUDDescriptor( database_ptr_t dataBase_ )
	: _mode( MODE::SELECT ), _varTmpBuffer(), _SQL(), _table(),
	_columns( "*" ), _filter(), _sort(), _fields(), _fieldCount( 0 ),
	_setSize( 0 ), _values(), _dataBase( dataBase_ ), _query(), _mutated() {
	return;
}

HCRUDDescriptor::~HCRUDDescriptor( void ) {
}

void HCRUDDescriptor::build_query( MODE::mode_t const& mode_ ) {
	M_PROLOG
	_varTmpBuffer.clear();
	_query.reset();
	switch ( mode_ ) {
		case ( MODE::SELECT ): {
			_SQL.format( "SELECT %s FROM %s", _columns.raw(),
					_table.raw() );
			if ( ! _filter.is_empty() )
				_SQL += ( " WHERE " + _filter );
			if ( ! _sort.is_empty() )
				_SQL += ( " ORDER BY " + _sort );
			_SQL += ';';
		}
		break;
		case ( MODE::UPDATE ): {
			_SQL = "UPDATE " + _table + " SET ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size = _values.get_size();
			bool hasField = false;
			for ( int i = 0; i < size; ++ i ) {
				if ( _mutated[ i ] ) {
					if ( hasField )
						_SQL += ", ";
					hasField = true;
					_SQL += _fields[ i ];
					if ( _values[ i ] ) {
						_SQL += " = ?";
					} else
						_SQL += " = NULL";
				}
			}
			if ( ! _filter.is_empty() ) {
				_SQL += " WHERE ";
				_SQL += _filter;
			}
			_SQL += ';';
			_query = _dataBase->prepare_query( _SQL );
			for ( int i = 0; i < size; ++ i ) {
				_query->bind( i + 1, *_values[ i ] );
			}
		}
		break;
		case ( MODE::INSERT ): {
			_SQL = "INSERT INTO " + _table + " ( ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size( _fields.get_size() );
			for ( int i( 0 ); i < size; ++ i ) {
				if ( !! _values[i] ) {
					if ( i > 0 )
						_SQL += ", ";
					_SQL += _fields[ i ];
				}
			}
			_SQL += " ) VALUES ( ";
			for ( int i = 0; i < size; ++ i ) {
				if ( !! _values[i] ) {
					if ( i > 0 )
						_SQL += ", ";
					_SQL += "?";
				}
			}
			_SQL += " );";
			_query = _dataBase->prepare_query( _SQL );
			for ( int i( 0 ), pos( 1 ); i < size; ++ i ) {
				if ( !! _values[i] ) {
					_query->bind( pos ++, *_values[ i ] );
				}
			}
		}
		break;
		case ( MODE::DELETE ): {
			_SQL = "DELETE FROM ";
			_SQL += _table;
			if ( ! _filter.is_empty() ) {
				_SQL += " WHERE ";
				_SQL += _filter;
			}
			_SQL += ";";
			_query = _dataBase->prepare_query( _SQL );
		}
		break;
		default :
			M_THROW( _eMode_, mode_ );
		break;
	}
	_mode = mode_;
	return;
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( void ) {
	M_PROLOG
	HRecordSet::ptr_t rs = !! _query ? _query->execute() : _dataBase->execute_query( _SQL );
	_fieldCount = rs->get_field_count();
	if ( _fields.get_size() != _fieldCount ) {
		_fields.resize( _fieldCount );
		_values.resize( _fieldCount );
		_mutated.resize( _fieldCount );
	}
	for ( int ctr = 0; ctr < _fieldCount; ++ ctr ) {
		_fields[ ctr ] = rs->get_column_name( ctr );
		_mutated[ ctr ] = false;
	}
	if ( _mode == MODE::SELECT ) {
		_setSize = rs->get_size();
	} else {
		_setSize = rs->get_dml_size();
	}
	return ( rs );
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( MODE::mode_t const& mode_ ) {
	M_PROLOG
	build_query( mode_ );
	return ( execute() );
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( char const* const query_ ) {
	M_PROLOG
	_SQL = query_;
	_query.reset();
	return ( execute() );
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, HString& value ) {
	M_PROLOG
	if ( _mode == MODE::SELECT ) {
		if ( _values[ field_ ] ) {
			value = *_values[ field_ ];
		} else {
			value.clear();
		}
	} else
		_values[ field_ ] = value;
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, HTime& value ) {
	M_PROLOG
	if ( _mode == MODE::SELECT ) {
		if ( _values[ field_ ] ) {
			value.from_string( *_values[ field_ ] );
		} else {
			value.set_now( HTime::LOCAL );
		}
	} else
		_values[ field_ ] = value.string();
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, int long& value ) {
	M_PROLOG
	if ( _mode == MODE::SELECT ) {
		if ( _values[ field_ ] )
			value = lexical_cast<int long>( *_values[ field_ ] );
		else
			value = 0;
	} else
		_values[ field_ ] = HRecordSet::value_t( value );
	M_EPILOG
}

void HCRUDDescriptor::set_table( yaal::hcore::HString const& table_ ) {
	M_PROLOG
	_table = table_;
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_columns( yaal::hcore::HString const& columns_ ) {
	M_PROLOG
	_columns = columns_;
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_filter( yaal::hcore::HString const& filter_ ) {
	M_PROLOG
	_filter = filter_;
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_sort( yaal::hcore::HString const& sort_ ) {
	M_PROLOG
	_sort = sort_;
	return;
	M_EPILOG
}

HString HCRUDDescriptor::get_table( void ) const {
	return ( _table );
}

HString HCRUDDescriptor::get_columns( void ) const {
	return ( _columns );
}

HString HCRUDDescriptor::get_filter( void ) const {
	return ( _filter );
}

HString HCRUDDescriptor::get_sort( void ) const {
	return ( _sort );
}

HCRUDDescriptor::MODE::mode_t HCRUDDescriptor::get_mode( void ) const {
	return ( _mode );
}

int long HCRUDDescriptor::get_size( void ) const {
	return ( _setSize );
}

void HCRUDDescriptor::sync( HRecordSet::iterator const& it ) {
	M_PROLOG
	for ( int ctr = 0; ctr < _fieldCount; ++ ctr )
		_values[ ctr ] = it[ ctr ];
	M_EPILOG
}

HRecordSet::value_t& HCRUDDescriptor::operator[]( int column_ ) {
	M_PROLOG
	M_ASSERT( ( column_ >= 0 ) && ( column_ <= _fieldCount ) );
	_mutated[ column_ ] = true;
	return ( _values[ column_ ] );
	M_EPILOG
}

}

}


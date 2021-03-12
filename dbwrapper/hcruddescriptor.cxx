/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcruddescriptor.hxx"
#include "hdatabase.hxx"
#include "tools/stringalgo.hxx"
#include "hcore/hformat.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace dbwrapper {

namespace {
char const _eMode_[] = "record set is not in appropriate mode for operation";
}

HCRUDDescriptor::HCRUDDescriptor( void )
	: _mode( MODE::READ )
	, _varTmpBuffer()
	, _SQL()
	, _table()
	, _columns( "*" )
	, _idColumn()
	, _filter()
	, _filterValue()
	, _sort()
	, _fields()
	, _fieldCount( 0 )
	, _setSize( 0 )
	, _values()
	, _dataBase()
	, _query()
	, _mutated() {
	return;
}

HCRUDDescriptor::HCRUDDescriptor( database_ptr_t dataBase_ )
	: _mode( MODE::READ )
	, _varTmpBuffer()
	, _SQL()
	, _table()
	, _columns( "*" )
	, _idColumn()
	, _filter()
	, _filterValue()
	, _sort()
	, _fields()
	, _fieldCount( 0 )
	, _setSize( 0 )
	, _values()
	, _dataBase( dataBase_ )
	, _query()
	, _mutated() {
	return;
}

HCRUDDescriptor::~HCRUDDescriptor( void ) {
	return;
}

namespace {

char const* cond_to_str( HCRUDDescriptor::OFilter::CONDITION condition_ ) {
	char const* str( "" );
	switch ( condition_ ) {
		case ( HCRUDDescriptor::OFilter::CONDITION::EQUALS ):           str = " = ?";         break;
		case ( HCRUDDescriptor::OFilter::CONDITION::NOT_EQUALS ):       str = " <> ?";        break;
		case ( HCRUDDescriptor::OFilter::CONDITION::LESS ):             str = " < ?";         break;
		case ( HCRUDDescriptor::OFilter::CONDITION::LESS_OR_EQUAL ):    str = " <= ?";        break;
		case ( HCRUDDescriptor::OFilter::CONDITION::GREATER ):          str = " > ?";         break;
		case ( HCRUDDescriptor::OFilter::CONDITION::GREATER_OR_EQUAL ): str = " >= ?";        break;
		case ( HCRUDDescriptor::OFilter::CONDITION::IS_NULL ):          str = " IS NULL";     break;
		case ( HCRUDDescriptor::OFilter::CONDITION::IS_NOT_NULL ):      str = " IS NOT NULL"; break;
		default: {}
	}
	return str;
}

bool cond_req_bind( HCRUDDescriptor::OFilter::CONDITION condition_ ) {
	return (
		( condition_ != HCRUDDescriptor::OFilter::CONDITION::NONE )
		&& ( condition_ != HCRUDDescriptor::OFilter::CONDITION::IS_NULL )
		&& ( condition_ != HCRUDDescriptor::OFilter::CONDITION::IS_NOT_NULL )
	);
}

}

void HCRUDDescriptor::build_query( MODE mode_ ) {
	M_PROLOG
	if ( _table.is_empty() ) {
		throw HCRUDDescriptorException( "table is not set" );
	}
	_varTmpBuffer.clear();
	_query.reset();
	switch ( mode_ ) {
		case ( MODE::READ ): {
			_SQL = format( "SELECT %s FROM %s", _columns, _table );
			OFilter const& readFilter( _filter[static_cast<int>( MODE::READ )] );
			if ( readFilter._condition != OFilter::CONDITION::NONE ) {
				_SQL.append( " WHERE " ).append( readFilter._column ).append( cond_to_str( readFilter._condition ) );
			}
			if ( ! _sort.is_empty() ) {
				_SQL.append( " ORDER BY " ).append( _sort );
			}
			_SQL += ';';
			if ( cond_req_bind( readFilter._condition ) ) {
				_query = _dataBase->prepare_query( _SQL );
				_query->bind( 1, _filterValue );
			}
		}
		break;
		case ( MODE::UPDATE ): {
			verify_dml();
			_SQL = "UPDATE " + _table + " SET ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size = _values.get_size();
			bool hasField = false;
			for ( int i = 0; i < size; ++ i ) {
				if ( _mutated[ i ] ) {
					if ( hasField ) {
						_SQL += ", ";
					}
					hasField = true;
					_SQL += _fields[ i ];
					if ( !! _values[ i ] ) {
						_SQL += " = ?";
					} else {
						_SQL += " = NULL";
					}
				}
			}
			OFilter const& updateFilter( _filter[static_cast<int>( MODE::UPDATE )] );
			if ( updateFilter._condition != OFilter::CONDITION::NONE ) {
				_SQL.append( " WHERE " ).append( updateFilter._column ).append( cond_to_str( updateFilter._condition ) );
			}
			_SQL += ';';
			_query = _dataBase->prepare_query( _SQL );
			int pos( 1 );
			for ( int i( 0 ); i < size; ++ i ) {
				if ( _mutated[i] && !! _values[i] ) {
					_query->bind( pos ++, *_values[ i ] );
				}
			}
			if ( cond_req_bind( updateFilter._condition ) ) {
				_query->bind( pos, _filterValue );
			}
		}
		break;
		case ( MODE::CREATE ): {
			verify_dml();
			_SQL = "INSERT INTO " + _table + " ( ";
			M_ENSURE( _fields.get_size() == _values.get_size() );
			int long const size( _fields.get_size() );
			bool hasField( false );
			for ( int i( 0 ); i < size; ++ i ) {
				if ( !! _values[i] ) {
					if ( hasField ) {
						_SQL += ", ";
					}
					hasField = true;
					_SQL += _fields[ i ];
				}
			}
			_SQL += " ) VALUES ( ";
			hasField = false;
			for ( int i = 0; i < size; ++ i ) {
				if ( !! _values[i] ) {
					if ( hasField ) {
						_SQL += ", ";
					}
					hasField = true;
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
			OFilter const& deleteFilter( _filter[static_cast<int>( MODE::DELETE )] );
			if ( deleteFilter._condition != OFilter::CONDITION::NONE ) {
				_SQL.append( " WHERE " ).append( deleteFilter._column ).append( cond_to_str( deleteFilter._condition ) );
			}
			_SQL += ";";
			_query = _dataBase->prepare_query( _SQL );
			if ( cond_req_bind( deleteFilter._condition ) ) {
				_query->bind( 1, _filterValue );
			}
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

void HCRUDDescriptor::verify_dml( void ) {
	M_PROLOG
	if ( _fields.is_empty() ) {
		throw HCRUDDescriptorException( "there are no fields" );
	}
	if ( _values.is_empty() ) {
		throw HCRUDDescriptorException( "there are no values" );
	}
	if ( _fields.get_size() != _values.get_size() ) {
		throw HCRUDDescriptorException( "fields size does not equal values size" );
	}
	return;
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( void ) {
	M_PROLOG
	HRecordSet::ptr_t rs = !! _query ? _query->execute() : _dataBase->execute_query( _SQL );
	if ( _mode == MODE::READ ) {
		_fieldCount = rs->get_field_count();
		if ( ! _query ) {
			_setSize = rs->get_size();
		}
		if ( _fields.get_size() != _fieldCount ) {
			_fields.resize( _fieldCount );
			_values.resize( _fieldCount );
			_mutated.resize( _fieldCount );
		}
		for ( int ctr = 0; ctr < _fieldCount; ++ ctr ) {
			_fields[ ctr ] = rs->get_column_name( ctr );
		}
	} else {
		_setSize = rs->get_dml_size();
	}
	fill( _mutated.begin(), _mutated.end(), false );
	return rs;
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( MODE mode_ ) {
	M_PROLOG
	build_query( mode_ );
	return ( execute() );
	M_EPILOG
}

HRecordSet::ptr_t HCRUDDescriptor::execute( HString const& query_ ) {
	M_PROLOG
	_SQL = query_;
	_query.reset();
	return ( execute() );
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, HString& value ) {
	M_PROLOG
	if ( _mode == MODE::READ ) {
		if ( _values[ field_ ] ) {
			value = *_values[ field_ ];
		} else {
			value.clear();
		}
	} else {
		_values[ field_ ] = value;
		_mutated[ field_ ] = true;
	}
	return;
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, HTime& value ) {
	M_PROLOG
	if ( _mode == MODE::READ ) {
		if ( _values[ field_ ] ) {
			value.from_string( *_values[ field_ ] );
		} else {
			value.set_now();
		}
	} else {
		_values[ field_ ] = value.string();
		_mutated[ field_ ] = true;
	}
	return;
	M_EPILOG
}

void HCRUDDescriptor::sync( int field_, int long& value ) {
	M_PROLOG
	if ( _mode == MODE::READ ) {
		if ( _values[ field_ ] )
			value = lexical_cast<int long>( *_values[ field_ ] );
		else
			value = 0;
	} else {
		_values[ field_ ] = HRecordSet::value_t( value );
		_mutated[ field_ ] = true;
	}
	return;
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

void HCRUDDescriptor::set_columns( field_names_t&& fields_ ) {
	M_PROLOG
	_fields = yaal::move( fields_ );
	_fieldCount = static_cast<int>( _fields.get_size() );
	commit_columns();
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_columns( field_names_t const& fields_ ) {
	M_PROLOG
	_fields = fields_;
	_fieldCount = static_cast<int>( _fields.get_size() );
	commit_columns();
	return;
	M_EPILOG
}

void HCRUDDescriptor::commit_columns( void ) {
	M_PROLOG
	_columns = string::join( _fields, "," );
	if ( ! _idColumn.is_empty() ) {
		_columns.append( "," ).append( _idColumn );
	}
	_values.resize( _fieldCount );
	_mutated.resize( _fieldCount );
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_id_column( yaal::hcore::HString const& idColumn_ ) {
	M_PROLOG
	_idColumn = idColumn_;
	commit_columns();
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_filter( yaal::hcore::HString const& column_, OFilter::CONDITION condition_ ) {
	M_PROLOG
	set_filter( column_, MODE::READ, condition_ );
	set_filter( column_, MODE::UPDATE, condition_ );
	set_filter( column_, MODE::DELETE, condition_ );
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_filter( yaal::hcore::HString const& column_, MODE mode_, OFilter::CONDITION condition_ ) {
	M_PROLOG
	_filter[static_cast<int>( mode_ )]._column = column_;
	_filter[static_cast<int>( mode_ )]._condition = condition_;
	return;
	M_EPILOG
}

void HCRUDDescriptor::set_filter_value( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_filterValue = value_;
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
	OFilter const& filterInfo( _filter[static_cast<int>( _mode )] );
	HString filterStr( filterInfo._column );
	filterStr.append( cond_to_str( filterInfo._condition ) ).replace( "?", _filterValue );
	return filterStr;
}

HString HCRUDDescriptor::get_sort( void ) const {
	return ( _sort );
}

HCRUDDescriptor::MODE HCRUDDescriptor::get_mode( void ) const {
	return ( _mode );
}

int long HCRUDDescriptor::get_size( void ) const {
	return ( _setSize );
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


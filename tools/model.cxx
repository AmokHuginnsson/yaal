/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "model.hxx"

namespace yaal {

namespace tools {

namespace model {

HValue::HValue( void )
	: _data() {
}

HValue::HValue( yaal::hcore::HString const& value_ )
	: _data( value_ ) {
}

HValue::HValue( int long long value_ )
	: _data( value_ ) {
}

HValue::HValue( double long value_ )
	: _data( value_ ) {
}

HValue::HValue( yaal::hcore::HNumber const& value_ )
	: _data( value_ ) {
}

HValue::HValue( LITERAL value_ )
	: _data( value_ ) {
}

HValue::HValue( array_t const& value_ )
	: _data( value_ ) {
}

HValue::HValue( array_t&& value_ )
	: _data( yaal::move( value_ ) ) {
}

HValue::HValue( members_t const& value_ )
	: _data( value_ ) {
}

HValue::HValue( members_t&& value_ )
	: _data( yaal::move( value_ ) ) {
}

bool HValue::operator == ( HValue const& other_ ) const {
	M_PROLOG
	TYPE t( type() );
	if ( other_.type() != t ) {
		return ( false );
	}
	switch ( t ) {
		case ( TYPE::INTEGER ): return ( get_integer() == other_.get_integer() );
		case ( TYPE::REAL ):    return ( get_real()    == other_.get_real() );
		case ( TYPE::NUMBER ):  return ( get_number()  == other_.get_number() );
		case ( TYPE::STRING ):  return ( get_string()  == other_.get_string() );
		case ( TYPE::LITERAL ): return ( get_literal() == other_.get_literal() );
		case ( TYPE::ARRAY ): {
			array_t const& a( get_elements() );
			array_t const& oa( other_.get_elements() );
			return ( a == oa );
		}
		case ( TYPE::MAP ): {
			members_t const& m( get_members() );
			members_t const& om( other_.get_members() );
			return ( m == om );
		}
		default: break;
	}
	return ( true );
	M_EPILOG
}

void HValue::reset( void ) {
	M_PROLOG
	_data.reset();
	return;
	M_EPILOG
}

void HValue::push_back( HValue const& value_ ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::ARRAY ) ) ) {
		throw HModelException( "Pushing elements to non-array value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( array_t() );
	}
	_data.get<array_t>().push_back( value_ );
	return;
	M_EPILOG
}

void HValue::push_back( HValue&& value_ ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::ARRAY ) ) ) {
		throw HModelException( "Pushing elements to non-array value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( array_t() );
	}
	_data.get<array_t>().push_back( yaal::move( value_ ) );
	return;
	M_EPILOG
}

HValue& HValue::operator[]( int index_ ) {
	return ( operator[]( static_cast<int long>( index_ ) ) );
}

HValue& HValue::operator[]( int long index_ ) {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HModelException( "Getting element of non-array value." );
	}
	return ( _data.get<array_t>().at( index_ ) );
	M_EPILOG
}

HValue const& HValue::operator[]( int index_ ) const {
	return ( operator[]( static_cast<int long>( index_ ) ) );
}

HValue const& HValue::operator[]( int long index_ ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HModelException( "Getting element of non-array value." );
	}
	return ( _data.get<array_t>().at( index_ ) );
	M_EPILOG
}

HValue& HValue::operator[]( yaal::hcore::HString const& member_ ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::MAP ) ) ) {
		throw HModelException( "Getting member of non-map value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( members_t() );
	}
	return ( _data.get<members_t>()[ member_ ] );
	M_EPILOG
}

HValue const& HValue::operator[]( yaal::hcore::HString const& member_ ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::MAP ) ) {
		throw HModelException( "Getting member of non-map value." );
	}
	return ( _data.get<members_t>().at( member_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HValue::get_string( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::STRING ) ) {
		throw HModelException( "Getting string from non-string value." );
	}
	return ( _data.get<yaal::hcore::HString>() );
	M_EPILOG
}

int long long HValue::get_integer( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::INTEGER ) ) {
		throw HModelException( "Getting integer from non-integer value." );
	}
	return ( _data.get<int long long>() );
	M_EPILOG
}

double long HValue::get_real( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::REAL ) ) {
		throw HModelException( "Getting real from non-real value." );
	}
	return ( _data.get<double long>() );
	M_EPILOG
}

yaal::hcore::HNumber const& HValue::get_number( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::NUMBER ) ) {
		throw HModelException( "Getting number from non-number value." );
	}
	return ( _data.get<yaal::hcore::HNumber>() );
	M_EPILOG
}

HValue::LITERAL HValue::get_literal( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::LITERAL ) ) {
		throw HModelException( "Getting literal from non-literal value." );
	}
	return ( _data.get<LITERAL>() );
	M_EPILOG
}

HValue::array_t const& HValue::get_elements( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HModelException( "Getting elements of non-array value." );
	}
	return ( _data.get<array_t>() );
	M_EPILOG
}

HValue::array_t& HValue::get_elements( void ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::ARRAY ) ) ) {
		throw HModelException( "Pushing elements to non-array value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( array_t() );
	}
	return ( _data.get<array_t>() );
	M_EPILOG
}

HValue::members_t const& HValue::get_members( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::MAP ) ) {
		throw HModelException( "Getting members of non-map value." );
	}
	return ( _data.get<members_t>() );
	M_EPILOG
}

HValue::members_t& HValue::get_members( void ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::MAP ) ) ) {
		throw HModelException( "Getting member of non-map value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( members_t() );
	}
	return ( _data.get<members_t>() );
	M_EPILOG
}

HValue::TYPE HValue::type( void ) const {
	return ( static_cast<TYPE>( _data.type() ) );
}

}

}

}


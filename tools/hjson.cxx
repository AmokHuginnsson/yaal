/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hjson.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HJSON::HValue::HValue( void )
	: _data() {
}

HJSON::HValue::HValue( yaal::hcore::HString const& value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( int long long value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( double long value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( yaal::hcore::HNumber const& value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( LITERAL value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( array_t const& value_ )
	: _data( value_ ) {
}

HJSON::HValue::HValue( members_t const& value_ )
	: _data( value_ ) {
}

void HJSON::HValue::push_back( HValue const& value_ ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::ARRAY ) ) ) {
		throw HJSONException( "Pushing elements to non-array value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( array_t() );
	}
	_data.get<array_t>().push_back( value_ );
	return;
	M_EPILOG
}

HJSON::HValue& HJSON::HValue::operator[]( int index_ ) {
	return ( operator[]( static_cast<int long>( index_ ) ) );
}

HJSON::HValue& HJSON::HValue::operator[]( int long index_ ) {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HJSONException( "Getting element of non-array value." );
	}
	return ( _data.get<array_t>().at( index_ ) );
	M_EPILOG
}

HJSON::HValue const& HJSON::HValue::operator[]( int index_ ) const {
	return ( operator[]( static_cast<int long>( index_ ) ) );
}

HJSON::HValue const& HJSON::HValue::operator[]( int long index_ ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HJSONException( "Getting element of non-array value." );
	}
	return ( _data.get<array_t>().at( index_ ) );
	M_EPILOG
}

HJSON::HValue& HJSON::HValue::operator[]( yaal::hcore::HString const& member_ ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::MAP ) ) ) {
		throw HJSONException( "Getting member of non-map value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( members_t() );
	}
	return ( _data.get<members_t>()[ member_ ] );
	M_EPILOG
}

HJSON::HValue const& HJSON::HValue::operator[]( yaal::hcore::HString const& member_ ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::MAP ) ) {
		throw HJSONException( "Getting member of non-map value." );
	}
	return ( _data.get<members_t>().at( member_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HJSON::HValue::get_string( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::STRING ) ) {
		throw HJSONException( "Getting string from non-string value." );
	}
	return ( _data.get<yaal::hcore::HString>() );
	M_EPILOG
}

int long long HJSON::HValue::get_integer( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::INTEGER ) ) {
		throw HJSONException( "Getting integer from non-integer value." );
	}
	return ( _data.get<int long long>() );
	M_EPILOG
}

double long HJSON::HValue::get_real( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::REAL ) ) {
		throw HJSONException( "Getting real from non-real value." );
	}
	return ( _data.get<double long>() );
	M_EPILOG
}

yaal::hcore::HNumber const& HJSON::HValue::get_number( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::NUMBER ) ) {
		throw HJSONException( "Getting number from non-number value." );
	}
	return ( _data.get<yaal::hcore::HNumber>() );
	M_EPILOG
}

HJSON::HValue::LITERAL HJSON::HValue::get_literal( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::LITERAL ) ) {
		throw HJSONException( "Getting literal from non-literal value." );
	}
	return ( _data.get<LITERAL>() );
	M_EPILOG
}

HJSON::HValue::array_t const& HJSON::HValue::get_elements( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::ARRAY ) ) {
		throw HJSONException( "Getting elements of non-array value." );
	}
	return ( _data.get<array_t>() );
	M_EPILOG
}

HJSON::HValue::members_t const& HJSON::HValue::get_members( void ) const {
	M_PROLOG
	if ( _data.type() != static_cast<int>( TYPE::MAP ) ) {
		throw HJSONException( "Getting members of non-map value." );
	}
	return ( _data.get<members_t>() );
	M_EPILOG
}

HJSON::HJSON( void )
	: _element() {
}

HJSON::HValue& HJSON::element( void ) {
	return ( _element );
}

HJSON::HValue::TYPE HJSON::HValue::type( void ) const {
	return ( static_cast<TYPE>( _data.type() ) );
}

namespace {
void indent( yaal::hcore::HStreamInterface& out_, int level_, bool enabled_ ) {
	static char const TAB[] =
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
		"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
	;
	if ( enabled_ ) {
		out_.write( TAB, min( size( TAB ), level_ ) );
	}
}
void dump( HJSON::HValue const& value_, yaal::hcore::HStreamInterface& out_, int indentLevel_, bool indent_ ) {
	M_PROLOG
	switch ( value_.type() ) {
		case ( HJSON::HValue::TYPE::STRING ):  out_ << '"' << value_.get_string()  << '"'; break;
		case ( HJSON::HValue::TYPE::INTEGER ): out_ << value_.get_integer(); break;
		case ( HJSON::HValue::TYPE::REAL ):    out_ << value_.get_real(); break;
		case ( HJSON::HValue::TYPE::NUMBER ):  out_ << value_.get_number().to_string(); break;
		case ( HJSON::HValue::TYPE::LITERAL ): {
			switch ( value_.get_literal() ) {
				case ( HJSON::HValue::LITERAL::TRUE ):  out_ << "true";  break;
				case ( HJSON::HValue::LITERAL::FALSE ): out_ << "false"; break;
				case ( HJSON::HValue::LITERAL::NULL ):  out_ << "null";  break;
			}
		} break;
		case ( HJSON::HValue::TYPE::ARRAY ): {
			out_ << "[";
			bool next( false );
			for ( HJSON::HValue const& v : value_.get_elements() ) {
				if ( next ) {
					out_ << ", ";
				}
				dump( v, out_, indentLevel_, indent_ );
				next = true;
			}
			out_ << "]";
		} break;
		case ( HJSON::HValue::TYPE::MAP ): {
			out_ << "{";
			if ( indent_ ) {
				out_ << "\n";
			}
			bool next( false );
			for ( HJSON::HValue::members_t::value_type const& member : value_.get_members() ) {
				if ( next ) {
					out_ << "," << ( indent_ ? "\n" : " " );
				}
				indent( out_, indentLevel_ + 1, indent_ );
				out_ << '"' << member.first << "\": ";
				dump( member.second, out_, indentLevel_ + 1, indent_ );
				next = true;
			}
			if ( indent_ ) {
				out_ << "\n";
			}
			indent( out_, indentLevel_, indent_ );
			out_ << "}";
		} break;
		case ( HJSON::HValue::TYPE::UNINITIALIZED ): {
			throw HJSONException( "Uninitialized value in JSON data structure during serialization." );
		}
	}
	return;
	M_EPILOG
}
}

void HJSON::save( yaal::hcore::HStreamInterface& out_, bool indent_ ) {
	M_PROLOG
	dump( _element, out_, 0, indent_ );
	return;
	M_EPILOG
}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "tools/executingparser.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hjson.hxx"
#include "hcore/system.hxx"

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

bool HJSON::HValue::operator == ( HValue const& other_ ) const {
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

void HJSON::HValue::reset( void ) {
	M_PROLOG
	_data.reset();
	return;
	M_EPILOG
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

HJSON::HValue::array_t& HJSON::HValue::get_elements( void ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::ARRAY ) ) ) {
		throw HJSONException( "Pushing elements to non-array value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( array_t() );
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

HJSON::HValue::members_t& HJSON::HValue::get_members( void ) {
	M_PROLOG
	int dt( _data.type() );
	if ( ( dt != data_t::INVALID ) && ( dt != static_cast<int>( TYPE::MAP ) ) ) {
		throw HJSONException( "Getting member of non-map value." );
	}
	if ( dt == data_t::INVALID ) {
		_data = data_t( members_t() );
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
			if ( indent_ && next ) {
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

bool HJSON::operator == ( HJSON const& other_ ) const {
	return ( _element == other_._element );
}

void HJSON::save( yaal::hcore::HStreamInterface& out_, bool indent_ ) const {
	M_PROLOG
	if ( _element.type() == HValue::TYPE::UNINITIALIZED ) {
		return;
	}
	dump( _element, out_, 0, indent_ );
	if ( indent_ ) {
		out_ << endl;
	}
	return;
	M_EPILOG
}

class HJSONParser {
private:
	typedef yaal::hcore::HStack<HJSON::HValue::array_t> store_t;
	HExecutingParser _engine;
	HJSON& _json;
	store_t _store;
public:
	HJSONParser( HJSON& json_, HJSON::PARSER style_ )
		: _engine( make_engine( style_ ) )
		, _json( json_ )
		, _store() {
		_store.push( HJSON::HValue::array_t() );
	}
	void parse( yaal::hcore::HString const& data_ ) {
		M_PROLOG
		if ( ! _engine.parse( data_.begin(), data_.end() ) ) {
			throw HJSONException(
				"Invalid JSON data at position "_ys
					.append( _engine.error_position() )
					.append( " in input stream, " )
					.append( _engine.error_messages()[0] )
			);
		}
		_engine.execute();
		if ( ! _store.top().is_empty() ) {
			_json.element() = yaal::move( _store.top().front() );
		}
		return;
		M_EPILOG
	}
private:
	void store_string( yaal::hcore::HString const& string_ ) {
		M_PROLOG
		_store.top().push_back( string_ );
		return;
		M_EPILOG
	}
	void store_real( yaal::hcore::HString const& real_ ) {
		M_PROLOG
		static int long const MAX_DECIMAL_DIGIT_DOUBLE_LONG_RESOLUTION( 22 );
		if ( real_.get_length() > MAX_DECIMAL_DIGIT_DOUBLE_LONG_RESOLUTION ) {
			_store.top().push_back( HNumber( real_ ) );
		} else {
			_store.top().push_back( lexical_cast<double long>( real_ ) );
		}
		return;
		M_EPILOG
	}
	void store_integer( int long long integer_ ) {
		M_PROLOG
		_store.top().push_back( integer_ );
		return;
		M_EPILOG
	}
	void store_literal( HJSON::HValue::LITERAL literal_ ) {
		M_PROLOG
		_store.top().push_back( literal_ );
		return;
		M_EPILOG
	}
	void nest( void ) {
		M_PROLOG
		_store.push( HJSON::HValue::array_t() );
		return;
		M_EPILOG
	}
	void store_array( void ) {
		M_PROLOG
		HJSON::HValue::array_t array( yaal::move( _store.top() ) );
		_store.pop();
		_store.top().push_back( yaal::move( array ) );
		return;
		M_EPILOG
	}
	void store_object( void ) {
		M_PROLOG
		HJSON::HValue::array_t membersParts( yaal::move( _store.top() ) );
		_store.pop();
		M_ASSERT( ( membersParts.get_size() % 2 ) == 0 );
		HJSON::HValue::members_t members;
		for ( HJSON::HValue::array_t::iterator it( membersParts.begin() ), end( membersParts.end() ); it != end; ++ it ) {
			HString key( it->get_string() );
			++ it;
			members.insert( make_pair( key, yaal::move( *it ) ) );
		}
		_store.top().push_back( yaal::move( members ) );
		return;
		M_EPILOG
	}
	executing_parser::HRule make_engine( HJSON::PARSER );
};

executing_parser::HRule HJSONParser::make_engine( HJSON::PARSER style_ ) {
	M_PROLOG
	namespace e_p = executing_parser;
	e_p::HRule element( "JSON.element" );
	e_p::HRule literalTrue(
		"JSON.Literal.true",
		e_p::constant( "true" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HJSON::HValue::LITERAL::TRUE ) )]
	);
	e_p::HRule literalFalse(
		"JSON.Literal.false",
		e_p::constant( "false" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HJSON::HValue::LITERAL::FALSE ) )]
	);
	e_p::HRule literalNull(
		"JSON.Literal.null",
		e_p::constant( "null" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HJSON::HValue::LITERAL::NULL ) )]
	);
	e_p::HRule stringLiteral(
		"JSON.string",
		e_p::string_literal[e_p::HStringLiteral::action_string_t( hcore::call( &HJSONParser::store_string, this, _1 ) )]
	);
	e_p::HRule singleQuotedStringLiteral(
		"JSON.string",
		e_p::string_literal( '\''_ycp )[e_p::HStringLiteral::action_string_t( hcore::call( &HJSONParser::store_string, this, _1 ) )]
	);
	e_p::HRule integerLiteral(
		"JSON.integer",
		e_p::integer[e_p::HInteger::action_int_long_long_t( hcore::call( &HJSONParser::store_integer, this, _1 ) )]
	);
	e_p::HRule realLiteral(
		"JSON.real",
		e_p::real( e_p::HReal::PARSE::STRICT )[e_p::HReal::action_string_t( hcore::call( &HJSONParser::store_real, this, _1 ) )]
	);
	e_p::HRule elements(
		"JSON.Array.elements",
		style_ == HJSON::PARSER::STRICT
			? static_cast<e_p::HRule&&>( element >> *( ',' >> element ) )
			: static_cast<e_p::HRule&&>( element >> *( ',' >> element ) >> -e_p::constant( ',' ) )
	);
	e_p::HRule array(
		"JSON.Array",
		e_p::constant( '[' )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::nest, this ) )] >> -elements >> ']',
		e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_array, this ) )
	);
	e_p::HRule member(
		"JSON.Object.member",
		style_ == HJSON::PARSER::STRICT
			? static_cast<e_p::HRule&&>( stringLiteral >> ":" >> element )
			: static_cast<e_p::HRule&&>( ( stringLiteral | singleQuotedStringLiteral ) >> ":" >> element )
	);
	e_p::HRule members(
		"JSON.Object.members",
		style_ == HJSON::PARSER::STRICT
			? static_cast<e_p::HRule&&>( member >> *( ',' >> member ) )
			: static_cast<e_p::HRule&&>( member >> *( ',' >> member ) >> -e_p::constant( ',' ) )
	);
	e_p::HRule object(
		"JSON.Object",
		e_p::constant( '{' )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::nest, this ) )] >> -members >> '}',
		e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_object, this ) )
	);
	if ( style_ == HJSON::PARSER::STRICT ) {
		element %= ( object | array | stringLiteral | realLiteral | integerLiteral | literalTrue | literalFalse | literalNull );
	} else {
		element %= ( object | array | stringLiteral | singleQuotedStringLiteral | realLiteral | integerLiteral | literalTrue | literalFalse | literalNull );
	}
	e_p::HRule json( "JSON", element );
	return json;
	M_EPILOG
}

void HJSON::load( yaal::hcore::HStreamInterface& in_, PARSER parser_ ) {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int nRead( 0 );
	int block( 0 );
	HChunk readBuffer;
	int totalSize( 0 );
	do {
		readBuffer.realloc( ( block + 1 ) * PAGE_SIZE );
		nRead = static_cast<int>( in_.read( readBuffer.get<char>() + block * PAGE_SIZE, PAGE_SIZE ) );
		M_ENSURE( nRead >= 0 );
		totalSize += nRead;
		++ block;
	} while ( nRead == PAGE_SIZE );
	HString data( readBuffer.get<char>(), totalSize );
	HJSONParser p( *this, parser_ );
	clear();
	p.parse( data );
	return;
	M_EPILOG
}

void HJSON::clear( void ) {
	M_PROLOG
	_element.reset();
	return;
	M_EPILOG
}

}

}


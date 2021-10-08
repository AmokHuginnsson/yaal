/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "tools/executingparser.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hjson.hxx"
#include "escape.hxx"
#include "hcore/system.hxx"
#include "tools/hidentifierlexer.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::model;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

HJSON::HJSON( void )
	: _element() {
}

model::HValue& HJSON::element( void ) {
	return ( _element );
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
void dump( HValue const& value_, yaal::hcore::HStreamInterface& out_, int indentLevel_, bool indent_ ) {
	M_PROLOG
	switch ( value_.type() ) {
		case ( HValue::TYPE::STRING ): {
			HString s( value_.get_string() );
			EscapeSet es( "\"\\", 2 );
			escape( s, es );
			escape( s, cxx_escape_table() );
			out_ << '"' << s << '"';
		} break;
		case ( HValue::TYPE::INTEGER ): out_ << value_.get_integer(); break;
		case ( HValue::TYPE::REAL ):    out_ << value_.get_real(); break;
		case ( HValue::TYPE::NUMBER ):  out_ << value_.get_number().to_string(); break;
		case ( HValue::TYPE::TIME ):    out_ << '"' << value_.get_time().to_string() << '"'; break;
		case ( HValue::TYPE::LITERAL ): {
			switch ( value_.get_literal() ) {
				case ( HValue::LITERAL::TRUE ):  out_ << "true";  break;
				case ( HValue::LITERAL::FALSE ): out_ << "false"; break;
				case ( HValue::LITERAL::NULL ):  out_ << "null";  break;
			}
		} break;
		case ( HValue::TYPE::ARRAY ): {
			out_ << "[";
			bool next( false );
			for ( HValue const& v : value_.get_elements() ) {
				if ( next ) {
					out_ << ", ";
				}
				dump( v, out_, indentLevel_, indent_ );
				next = true;
			}
			out_ << "]";
		} break;
		case ( HValue::TYPE::MAP ): {
			out_ << "{";
			if ( indent_ ) {
				out_ << "\n";
			}
			bool next( false );
			for ( HValue::members_t::value_type const& member : value_.get_members() ) {
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
		case ( HValue::TYPE::UNINITIALIZED ): {
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
	typedef yaal::hcore::HStack<HValue::array_t> store_t;
	HExecutingParser _engine;
	HJSON& _json;
	store_t _store;
public:
	HJSONParser( HJSON& json_, HJSON::PARSER style_ )
		: _engine( make_engine( style_ ) )
		, _json( json_ )
		, _store() {
		_store.push( HValue::array_t() );
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
	void store_literal( HValue::LITERAL literal_ ) {
		M_PROLOG
		_store.top().push_back( literal_ );
		return;
		M_EPILOG
	}
	void nest( void ) {
		M_PROLOG
		_store.push( HValue::array_t() );
		return;
		M_EPILOG
	}
	void store_array( void ) {
		M_PROLOG
		HValue::array_t array( yaal::move( _store.top() ) );
		_store.pop();
		_store.top().push_back( yaal::move( array ) );
		return;
		M_EPILOG
	}
	void store_object( void ) {
		M_PROLOG
		HValue::array_t membersParts( yaal::move( _store.top() ) );
		_store.pop();
		M_ASSERT( ( membersParts.get_size() % 2 ) == 0 );
		HValue::members_t members;
		for ( HValue::array_t::iterator it( membersParts.begin() ), end( membersParts.end() ); it != end; ++ it ) {
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
		e_p::constant( "true" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HValue::LITERAL::TRUE ) )]
	);
	e_p::HRule literalFalse(
		"JSON.Literal.false",
		e_p::constant( "false" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HValue::LITERAL::FALSE ) )]
	);
	e_p::HRule literalNull(
		"JSON.Literal.null",
		e_p::constant( "null" )[e_p::HRuleBase::action_t( hcore::call( &HJSONParser::store_literal, this, HValue::LITERAL::NULL ) )]
	);
	e_p::HRule stringLiteral(
		"JSON.string",
		e_p::string_literal[e_p::HStringLiteral::action_string_t( hcore::call( &HJSONParser::store_string, this, _1 ) )]
	);
	e_p::HRule singleQuotedStringLiteral(
		"JSON.string",
		e_p::string_literal( '\''_ycp )[e_p::HStringLiteral::action_string_t( hcore::call( &HJSONParser::store_string, this, _1 ) )]
	);
	e_p::HRule key(
		"JSON.string",
		identifier( "key", e_p::HStringLiteral::action_string_t( hcore::call( &HJSONParser::store_string, this, _1 ) ) )
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
			: static_cast<e_p::HRule&&>( ( stringLiteral | singleQuotedStringLiteral | key ) >> ":" >> element )
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


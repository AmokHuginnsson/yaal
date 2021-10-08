/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <yaml.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hyaml.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hcore {
namespace hidden {
double long std_strtold( HString const&, int* );
}
}

namespace tools {

namespace {

int read_handler( void* ext, char unsigned* buffer, size_t size, size_t* length ) {
	HStreamInterface& stream( *static_cast<HStreamInterface*>( ext ) );
	int long nRead( static_cast<int>( stream.read( buffer, static_cast<int>( size ) ) ) );
	*length = static_cast<size_t>( max( nRead, 0L ) );
	return ( nRead >= 0 ? 1 : 0 );
}

int write_handler( void* ext, char unsigned* buffer, size_t size ) {
	HStreamInterface& stream( *static_cast<HStreamInterface*>( ext ) );
	return ( static_cast<int>( stream.write( buffer, static_cast<int>( size ) ) ) );
}

bool try_integer( yaal::hcore::HString const& str_, int long long& value_ ) {
	try {
		value_ = lexical_cast<int long long>( str_ );
	} catch ( ... ) {
		return ( false );
	}
	return ( true );
}

bool try_number( yaal::hcore::HString const& str_, HNumber& value_ ) {
	try {
		value_ = str_;
	} catch ( ... ) {
		return ( false );
	}
	return ( value_.to_string().get_length() > 22 );
}

bool try_floating_point( yaal::hcore::HString const& str_, double long& value_ ) {
	int end( 0 );
	try {
		value_ = hidden::std_strtold( str_, &end );
	} catch ( ... ) {
		return ( false );
	}
	return ( end == str_.get_length() );
}

bool try_boolean( yaal::hcore::HString const& str_, bool& value_ ) {
	try {
		value_ = lexical_cast<bool>( str_ );
	} catch ( ... ) {
		return ( false );
	}
	return ( true );
}

bool try_time( yaal::hcore::HString const& str_, HTime& value_ ) {
	try {
		value_.set_time();
		value_.from_string( str_ );
	} catch ( ... ) {
		return ( false );
	}
	return ( true );
}

model::HValue load_node( yaml_document_t& document_, yaml_node_t* node_ ) {
	if ( ! node_ ) {
		return model::HValue();
	}
	if ( node_->type == YAML_SCALAR_NODE ) {
		HString scalar( reinterpret_cast<char*>( node_->data.scalar.value ), static_cast<int long>( node_->data.scalar.length ) );
		if ( node_->data.scalar.style == YAML_PLAIN_SCALAR_STYLE ) {
			int long long integer( 0 );
			if ( try_integer( scalar, integer ) ) {
				return integer;
			}
			HNumber number;
			if ( try_number( scalar, number ) ) {
				return number;
			}
			double long floatingPoint( 0.L );
			if ( try_floating_point( scalar, floatingPoint ) ) {
				return floatingPoint;
			}
			bool boolean( false );
			if ( try_boolean( scalar, boolean ) ) {
				return ( boolean ? model::HValue::LITERAL::TRUE : model::HValue::LITERAL::FALSE );
			}
			if ( stricasecmp( scalar, "null" ) == 0 ) {
#undef NULL
				return ( model::HValue::LITERAL::NULL );
			}
			HTime time( HTime::TZ::LOCAL, static_cast<i64_t>( 0 ) );
			if ( try_time( scalar, time ) ) {
				return time;
			}
		}
		return scalar;
	} else if ( node_->type == YAML_SEQUENCE_NODE ) {
		model::HValue sequence;
		sequence.get_elements();
		for ( yaml_node_item_t* n( node_->data.sequence.items.start ); n != node_->data.sequence.items.top; ++ n ) {
			sequence.push_back( load_node( document_, yaml_document_get_node( &document_, *n ) ) );
		}
		return sequence;
	} else if ( node_->type == YAML_MAPPING_NODE ) {
		model::HValue mapping;
		mapping.get_members();
		HString keyStr;
		for ( yaml_node_pair_t* np( node_->data.mapping.pairs.start ); np != node_->data.mapping.pairs.top; ++ np ) {
			yaml_node_t* key( yaml_document_get_node( &document_, np->key ) );
			if ( key->type != YAML_SCALAR_NODE ) {
				throw HYAMLException( "bad key type in loaded YAML tree" );
			}
			keyStr.assign( reinterpret_cast<char*>( key->data.scalar.value ), static_cast<int long>( key->data.scalar.length ) );
			mapping[keyStr] = load_node( document_, yaml_document_get_node( &document_, np->value ) );
		}
		return mapping;
	}
	return model::HValue();
}

int dump_node( yaal::hcore::HUTF8String& utf8cache_, model::HValue const& value_, yaml_document_t& document_ ) {
	int id( 0 );
	switch ( value_.type() ) {
		case ( model::HValue::TYPE::INTEGER ): {
			utf8cache_.assign( to_string( value_.get_integer() ) );
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				YAML_ANY_SCALAR_STYLE
			);
		} break;
		case ( model::HValue::TYPE::REAL ): {
			utf8cache_.assign( to_string( value_.get_real() ) );
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				YAML_ANY_SCALAR_STYLE
			);
		} break;
		case ( model::HValue::TYPE::NUMBER ): {
			utf8cache_.assign( value_.get_number().to_string() );
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				YAML_ANY_SCALAR_STYLE
			);
		} break;
		case ( model::HValue::TYPE::STRING ): {
			HString const& str( value_.get_string() );
			utf8cache_.assign( str );
			yaml_scalar_style_t style( YAML_ANY_SCALAR_STYLE );
			if ( str.find_one_of( "\r\n" ) != HString::npos ) {
				style = YAML_LITERAL_SCALAR_STYLE;
			} else if ( str.find( '{'_ycp ) != HString::npos ) {
				style = YAML_DOUBLE_QUOTED_SCALAR_STYLE;
			}
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				style
			);
		} break;
		case ( model::HValue::TYPE::TIME ): {
			HTime const& t( value_.get_time() );
			if ( ( t.get_year() == 0 ) && ( t.get_month() == 1 ) && ( t.get_day() == 1 ) ) {
				utf8cache_.assign( t.to_string( "%T" ) );
			} else if ( ( t.get_hour() == 0 ) && ( t.get_minute() == 0 ) && ( t.get_second() == 0 ) ) {
				utf8cache_.assign( t.to_string( "%F" ) );
			} else {
				utf8cache_.assign( t.to_string() );
			}
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				YAML_ANY_SCALAR_STYLE
			);
		} break;
		case ( model::HValue::TYPE::LITERAL ): {
			model::HValue::LITERAL literal( value_.get_literal() );
			utf8cache_.assign( literal == model::HValue::LITERAL::NULL ? "null" : ( literal == model::HValue::LITERAL::TRUE ? "true" : "false" ) );
			id = yaml_document_add_scalar(
				&document_,
				nullptr,
				const_cast<yaml_char_t*>( reinterpret_cast<yaml_char_t const*>( utf8cache_.c_str() ) ),
				static_cast<int>( utf8cache_.byte_count() ),
				YAML_ANY_SCALAR_STYLE
			);
		} break;
		case ( model::HValue::TYPE::ARRAY ): {
			id = yaml_document_add_sequence( &document_, nullptr, YAML_ANY_SEQUENCE_STYLE );
			for ( model::HValue const& element : value_.get_elements() ) {
				yaml_document_append_sequence_item( &document_, id, dump_node( utf8cache_, element, document_ ) );
			}
		} break;
		case ( model::HValue::TYPE::MAP ): {
			id = yaml_document_add_mapping( &document_, nullptr, YAML_ANY_MAPPING_STYLE );
			for ( model::HValue::members_t::value_type const& member : value_.get_members() ) {
				int key( dump_node( utf8cache_, member.first, document_ ) );
				int value( dump_node( utf8cache_, member.second, document_ ) );
				yaml_document_append_mapping_pair( &document_, id, key, value );
			}
		} break;
		case ( model::HValue::TYPE::UNINITIALIZED ): {
			throw HYAMLException( "Uninitialized value in YAML data structure during serialization." );
		} break;
	}
	return id;
}

}

HYAML::HYAML( void )
	: _documents() {
	return;
}

HYAML::documents_t& HYAML::documents( void ) {
	return ( _documents );
}

yaal::tools::model::HValue& HYAML::add_document( void ) {
	M_PROLOG
	_documents.push_back( model::HValue() );
	return ( _documents.back() );
	M_EPILOG
}

void HYAML::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	clear();
	yaml_parser_t parser;
	yaml_document_t document;
	yaml_parser_initialize( &parser );
	yaml_parser_set_input( &parser, &read_handler, &stream_ );
	while ( true ) {
		if ( ! yaml_parser_load( &parser, &document ) ) {
			yaml_parser_delete( &parser );
			throw HYAMLException( "Malformed YAML document read from stream." );
		}
		yaml_node_t* root( yaml_document_get_root_node( &document ) );
		if ( root ) {
			_documents.push_back( load_node( document, root ) );
		}
		yaml_document_delete( &document );
		if ( ! root ) {
			break;
		}
	}
	yaml_parser_delete( &parser );
	return;
	M_EPILOG
}

void HYAML::save( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	yaml_emitter_t emitter;
	yaml_emitter_initialize( &emitter );
	yaml_emitter_set_canonical( &emitter, 0 );
	yaml_emitter_set_unicode( &emitter, 1 );
	yaml_emitter_set_encoding( &emitter, YAML_UTF8_ENCODING );
	yaml_emitter_set_indent( &emitter, 2 );
	yaml_emitter_set_break( &emitter, YAML_LN_BREAK );
	yaml_emitter_set_width( &emitter, 80 );
	yaml_emitter_set_output( &emitter, &write_handler, &stream_ );
	HUTF8String utf8cache;
	int docIndicator( _documents.get_size() > 1 ? 0 : 1 );
	for ( model::HValue const& element : _documents ) {
		yaml_document_t document;
		yaml_document_initialize( &document, nullptr, nullptr, nullptr, docIndicator, docIndicator );
		dump_node( utf8cache, element, document );
		yaml_emitter_open( &emitter );
		yaml_emitter_dump( &emitter, &document );
		yaml_emitter_close( &emitter );
		yaml_document_delete( &document );
	}
	yaml_emitter_delete( &emitter );
	return;
	M_EPILOG
}

void HYAML::clear( void ) {
	M_PROLOG
	_documents.clear();
	return;
	M_EPILOG
}

bool HYAML::operator == ( HYAML const& other_ ) const {
	M_PROLOG
	return ( _documents == other_._documents );
	M_EPILOG
}

}

}


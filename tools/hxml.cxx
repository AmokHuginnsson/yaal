/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

  hxml.h - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <libintl.h>
#include <new>

#include <iconv.h>
#include <libexslt/exslt.h>
#include <libxml/xmlwriter.h>
#include <libxml/xinclude.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libexslt/exsltconfig.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hxml.hxx"
#include "hcore/memory.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hlog.hxx"
#include "tools.hxx"
#include "streamtools.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace {

/* char schema_err[] = "bad xml schema"; */
typedef HResource<xmlDoc, void (*)( xmlDocPtr )> doc_resource_t;
typedef HResource<xmlTextWriter, void (*)( xmlTextWriterPtr )> writer_resource_t;
typedef HPointer<xmlCharEncodingHandler> encoder_resource_t;
typedef HPointer<xsltStylesheet> style_resource_t;
typedef HResource<xmlXPathContext, void (*)( xmlXPathContextPtr )> xpath_context_resource_t;
typedef HResource<xmlXPathObject, void (*)( xmlXPathObjectPtr )> xpath_object_resource_t;
typedef HResource<xmlOutputBuffer, int (*)( xmlOutputBufferPtr )> outputbuffer_resource_t;

char const FULL_TREE[] = "//*";

}

namespace yaal {

namespace tools {

HXml::parser_t const HXml::PARSER::DEFAULT = HXml::parser_t::new_flag();
HXml::parser_t const HXml::PARSER::KEEP_EMPTY = HXml::parser_t::new_flag();
HXml::parser_t const HXml::PARSER::STRIP_COMMENT = HXml::parser_t::new_flag();
HXml::parser_t const HXml::PARSER::RESOLVE_ENTITIES = HXml::parser_t::new_flag();
HXml::parser_t const HXml::PARSER::AUTO_XINCLUDE = HXml::parser_t::new_flag();

class HXmlParserG : public HSingleton<HXmlParserG> {
	HXmlParserG( void );
	~HXmlParserG( void );
	friend class HSingleton<HXmlParserG>;
	friend class HDestructor<HXmlParserG>;
};

HXmlParserG::HXmlParserG( void ) {
}

HXmlParserG::~HXmlParserG( void ) {
	M_PROLOG
	xmlCleanupParser();
	xmlCleanupCharEncodingHandlers();
	return;
	M_DESTRUCTOR_EPILOG
}

class HXsltParserG : public HSingleton<HXsltParserG> {
	HXsltParserG( void );
	~HXsltParserG( void );
	friend class HSingleton<HXsltParserG>;
	friend class HDestructor<HXsltParserG>;
};

HXsltParserG::HXsltParserG( void ) {
	exsltRegisterAll();
	xmlSubstituteEntitiesDefault( 0 );
//	xmlLoadExtDtdDefaultValue = 1;
}

HXsltParserG::~HXsltParserG( void ) {
	M_PROLOG
	xsltCleanupGlobals();
	return;
	M_DESTRUCTOR_EPILOG
}

struct HXml::OConvert {
	encoder_resource_t _encoder;
	iconv_t _iconvToExternal;
	iconv_t _iconvToInternal;
	OConvert( void )
		: _encoder()
		, _iconvToExternal( static_cast<iconv_t>( 0 ) )
		, _iconvToInternal( static_cast<iconv_t>( 0 ) ) {
	}
	OConvert( OConvert const& convert_ )
		: _encoder()
		, _iconvToExternal( static_cast<iconv_t>( 0 ) )
		, _iconvToInternal( static_cast<iconv_t>( 0 ) ) {
		operator = ( convert_ );
	}
	OConvert& operator = ( OConvert const& convert_ ) {
		if ( &convert_ != this ) {
			_encoder = convert_._encoder;
			_iconvToExternal = convert_._iconvToExternal;
			_iconvToInternal = convert_._iconvToInternal;
		}
		return ( *this );
	}
	void init( HString const& encoding_ ) {
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = ::xmlFindCharEncodingHandler( encoding_.raw() );
		if ( ! encoder ) {
			M_THROW( _( "cannot enable internal conversion" ), errno );
		}
		_encoder = encoder_resource_t( encoder, xmlCharEncCloseFunc );
		_iconvToExternal = _encoder->iconv_in;
		_iconvToInternal = _encoder->iconv_out;
		M_EPILOG
	}
	void init( yaal::hcore::HString const& encoding_, xmlNodePtr root_, yaal::hcore::HString const& fileName_ ) {
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = nullptr;
		if ( !! encoding_ ) {
			encoder = ::xmlFindCharEncodingHandler( encoding_.raw() );
		} else {
			log( LOG_LEVEL::WARNING ) << _( "HXml::WARNING: no encoding declared in `" )
				<< fileName_ << ":" << root_->line <<"'." << endl;
		}
		if ( ! encoder ) {
			log( LOG_LEVEL::WARNING ) << _( "HXml::WARNING: char encoding handler not found in `" )
				<< fileName_ << ":" << root_->line << "'." << endl;
			xmlCharEncoding encoding = ::xmlDetectCharEncoding( root_->name,
					xmlStrlen( root_->name ) );
			if ( ! encoding ) {
				M_THROW( _( "cannot detect character encoding" ), errno );
			}
			encoder = xmlGetCharEncodingHandler( encoding );
		}
		if ( ! encoder ) {
			M_THROW( _( "cannot enable internal conversion" ), errno );
		}
		_encoder = encoder_resource_t( encoder, xmlCharEncCloseFunc );
		_iconvToExternal = _encoder->iconv_in;
		_iconvToInternal = _encoder->iconv_out;
		M_EPILOG
	}
	void reset( void ) {
		M_PROLOG
		_encoder.reset();
		_iconvToExternal = static_cast<iconv_t>( 0 );
		_iconvToInternal = static_cast<iconv_t>( 0 );
		return;
		M_EPILOG
	}
	bool is_initialized( void ) const {
		return ( ! ( _iconvToExternal && _iconvToInternal ) );
	}
};

class HXmlData {
private:
	doc_resource_t     _doc;
	style_resource_t   _style;
	mutable xpath_context_resource_t _xPathContext;
	mutable xpath_object_resource_t _xPathObject;
	xmlNodeSetPtr      _nodeSet;
public:
	virtual ~HXmlData( void );
protected:
	HXmlData( void );
	HXmlData( HXmlData const& );
	HXmlData( HXmlData&& );
	void clear( void ) const;
private:
	HXmlData& operator = ( HXmlData const& ) = delete;
	friend class HXml;
	template<typename tType, typename... arg_t>
	friend HResource<tType> yaal::hcore::make_resource( arg_t&&... );
};

HXmlData::HXmlData( void )
	: _doc()
	, _style()
	, _xPathContext()
	, _xPathObject()
	, _nodeSet( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

HXmlData::HXmlData( HXmlData const& xmlData_ )
	: _doc( ::xmlCopyDoc( const_cast<xmlDoc*>( xmlData_._doc.get() ), 1 ), &::xmlFreeDoc )
	, _style( xmlData_._style )
	, _xPathContext()
	, _xPathObject()
	, _nodeSet( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

HXmlData::HXmlData( HXmlData&& xmlData_ )
	: _doc( yaal::move( xmlData_._doc ) )
	, _style( yaal::move( xmlData_._style ) )
	, _xPathContext( yaal::move( xmlData_._xPathContext ) )
	, _xPathObject( yaal::move( xmlData_._xPathObject ) )
	, _nodeSet( yaal::move( xmlData_._nodeSet ) ) {
	M_PROLOG
	xmlData_._nodeSet = nullptr;
	return;
	M_EPILOG
}

HXmlData::~HXmlData ( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HXmlData::clear( void ) const {
	M_PROLOG
	_xPathObject.reset();
	_xPathContext.reset();
	return;
	M_EPILOG
}

class HXml::HNameSpace {
	yaal::hcore::HString _prefix;
	yaal::hcore::HString _href;
public:
	HNameSpace( yaal::hcore::HString const& prefix_, yaal::hcore::HString const& href_ )
		: _prefix( prefix_ ),
		_href( href_ ) {
		return;
	}
	bool operator == ( HNameSpace const& ns_ ) const {
		return ( ( _prefix == ns_._prefix ) && ( _href == ns_._href ) );
	}
	yaal::hcore::HString const& prefix( void ) const {
		return ( _prefix );
	}
	yaal::hcore::HString const& href( void ) const {
		return ( _href );
	}
};

HXml::HXml( void )
	: _convert( make_pointer<HXml::OConvert>() )
	, _convertedString()
	, _varTmpBuffer()
	, _encoding( _defaultEncoding_ )
	, _streamId()
	, _xml( make_resource<HXmlData>() )
	, _entities()
	, _namespaces()
	, _domTree() {
	M_PROLOG
	return;
	M_EPILOG
}

HXml::HXml( HXml const& xml_ )
	: _convert( xml_._convert )
	, _convertedString( xml_._convertedString )
	, _varTmpBuffer( xml_._varTmpBuffer )
	, _encoding( xml_._encoding )
	, _streamId( xml_._streamId )
	, _xml()
	, _entities( xml_._entities )
	, _namespaces( xml_._namespaces )
	, _domTree( xml_._domTree ) {
	M_PROLOG
	_xml = xml_low_t( new ( memory::yaal ) HXmlData( *xml_._xml ) );
	HNodeProxy root( get_root() );
	if ( !! root ) {
		root.reset_owner( this );
	}
	return;
	M_EPILOG
}

HXml::HXml( HXml&& xml_ )
	: _convert( yaal::move( xml_._convert ) )
	, _convertedString( yaal::move( xml_._convertedString ) )
	, _varTmpBuffer( yaal::move( xml_._varTmpBuffer ) )
	, _encoding( yaal::move( xml_._encoding ) )
	, _streamId( yaal::move( xml_._streamId ) )
	, _xml( yaal::move( xml_._xml ) )
	, _entities( yaal::move( xml_._entities ) )
	, _namespaces( yaal::move( xml_._namespaces ) )
	, _domTree( yaal::move( xml_._domTree ) ) {
	M_PROLOG
	HNodeProxy root( get_root() );
	if ( !! root ) {
		root.reset_owner( this );
	}
	return;
	M_EPILOG
}

HXml& HXml::operator = ( HXml const& xml_ ) {
	if ( &xml_ != this ) {
		HXml tmp( xml_ );
		swap( tmp );
		HNodeProxy root( get_root() );
		if ( !! root ) {
			root.reset_owner( this );
		}
	}
	return ( *this );
}

HXml& HXml::operator = ( HXml&& xml_ ) {
	if ( &xml_ != this ) {
		swap( xml_ );
		HNodeProxy root( get_root() );
		if ( !! root ) {
			root.reset_owner( this );
		}
		root = xml_.get_root();
		if ( !! root ) {
			root.reset_owner( &xml_ );
		}
		xml_.clear();
	}
	return ( *this );
}

void HXml::swap( HXml& xml_ ) {
	if ( &xml_ != this ) {
		using yaal::swap;
		swap( xml_._convert, _convert );
		swap( xml_._convertedString, _convertedString );
		swap( xml_._varTmpBuffer, _varTmpBuffer );
		swap( xml_._encoding, _encoding );
		swap( xml_._streamId, _streamId );
		swap( xml_._xml, _xml );
		swap( xml_._entities, _entities );
		swap( xml_._namespaces, _namespaces );
		swap( xml_._domTree, _domTree );
	}
	return;
}

HXml::~HXml ( void ) {
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
}

void HXml::clear( void ) {
	M_PROLOG
	/* *FIXME* *TODO*
	 * Actually clear _convert content, not only a reference to shared object.
	 * In other words turn _convert into copy-constructible value.
	 */
	_convert = make_pointer<OConvert>();
	_encoding.clear();
	_domTree.clear();
	_xml->clear();
	_entities.clear();
	_namespaces.clear();
	return;
	M_EPILOG
}

#ifdef HAVE_ICONV_INPUT_CONST
#	define M_YAAL_TOOLS_HXML_ICONV_CONST const
#else /* HAVE_ICONV_INPUT_CONST */
#	define M_YAAL_TOOLS_HXML_ICONV_CONST /**/
#endif /* not HAVE_ICONV_INPUT_CONST */

HString const& HXml::convert( HString const& data_, way_t way_ ) const {
	M_PROLOG
	char M_YAAL_TOOLS_HXML_ICONV_CONST* source = const_cast<char M_YAAL_TOOLS_HXML_ICONV_CONST*>( data_.raw() );
	iconv_t cD = static_cast<iconv_t>( 0 );
	switch ( way_ ) {
		case ( TO_EXTERNAL ): { cD = _convert->_iconvToExternal; break; }
		case ( TO_INTERNAL ): { cD = _convert->_iconvToInternal; break; }
		default:
			M_ASSERT( ! _( "unknown conversion way" ) );
		break;
	}
	size_t sizeOut = 0, sizeIn = ::strlen( source );
	/* The longest single character in any encoding is 6 bytes long. */
	size_t const ICONV_OUTPUT_BUFFER_LENGTH = 8;
	/* Additional character for nil terminator. */
	char output[ ICONV_OUTPUT_BUFFER_LENGTH + 1 ];
	_convertedString = "";
	do {
		::memset( output, 0, ICONV_OUTPUT_BUFFER_LENGTH + 1 );
		sizeOut = ICONV_OUTPUT_BUFFER_LENGTH;
		char* out = output;
		M_ENSURE( ( ::iconv( cD, &source, &sizeIn, &out,
						&sizeOut ) != static_cast<size_t>( -1 ) )
				|| ( errno == E2BIG ) );
		_convertedString += output;
	} while ( sizeIn );
	return ( _convertedString );
	M_EPILOG
}

#undef M_YAAL_TOOLS_HXML_ICONV_CONST

int HXml::get_node_set_by_path( yaal::hcore::HString const& path_ ) {
	M_PROLOG
	_varTmpBuffer = path_;
	int long length = _varTmpBuffer.get_length() - 1;
	xpath_context_resource_t ctx( xmlXPathNewContext( _xml->_doc.get() ), xmlXPathFreeContext );
	int setSize = 0;
	_xml->_nodeSet = nullptr;
	if ( ctx.get() ) {
		xmlXPathObjectPtr objPtr = nullptr;
		while ( ! _varTmpBuffer.is_empty() ) {
			objPtr = xmlXPathEvalExpression(
					reinterpret_cast<xmlChar const*>( _varTmpBuffer.raw() ),
					ctx.get() );
			if ( objPtr ) {
				break;
			}
			_varTmpBuffer.set_at( length --, 0 );
		}
		xpath_object_resource_t obj( objPtr, xmlXPathFreeObject );
		if ( obj.get() ) {
			_varTmpBuffer = path_;
			_xml->_nodeSet = obj.get()->nodesetval;
			using yaal::swap;
			swap( _xml->_xPathObject, obj );
			setSize = _xml->_nodeSet ? _xml->_nodeSet->nodeNr : 0;
		}
	}
	using yaal::swap;
	swap( _xml->_xPathContext, ctx );
	return ( setSize );
	M_EPILOG
}

void HXml::init( yaal::hcore::HStreamInterface& stream, parser_t parser_ ) {
	M_PROLOG
	int savedErrno = errno;
	HString error;
	HXmlParserG::get_instance();
	errno = 0;
	_streamId = get_stream_id( &stream );
	int LOW_LEVEL_PARSING_OPTIONS( XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_NONET | XML_PARSE_NSCLEAN );
	if ( parser_ & PARSER::AUTO_XINCLUDE ) {
		LOW_LEVEL_PARSING_OPTIONS |= XML_PARSE_XINCLUDE;
	}
	if ( parser_ & PARSER::RESOLVE_ENTITIES ) {
		LOW_LEVEL_PARSING_OPTIONS |= XML_PARSE_NOENT;
	}
	doc_resource_t doc( ::xmlReadIO( reader_callback, nullptr, &stream, _streamId.raw(), nullptr, LOW_LEVEL_PARSING_OPTIONS ),
			xmlFreeDoc );
	if ( errno ) {
		log( LOG_LEVEL::WARNING ) << "XML: " << error_message( errno ) << ": " << _streamId;
		log << ", code: " << errno << '.' << endl;
	}
	errno = savedErrno;
	if ( ! doc.get() ) {
		error.format( _( "cannot parse `%s'" ), _streamId.raw() );
		throw HXmlException( error );
	}
	if ( parser_ & PARSER::AUTO_XINCLUDE ) {
		if ( xmlXIncludeProcessFlags( doc.get(), LOW_LEVEL_PARSING_OPTIONS ) < 0 ) {
			throw HXmlException( "a flagged processing XInclude failed" );
		}
	}
	xmlNodePtr root = xmlDocGetRootElement( doc.get() );
	if ( ! root ) {
		M_THROW( _( "empty doc" ), errno );
	}
#ifdef __DEBUGGER_BABUNI__
	cout << root->name << endl;
#endif /* __DEBUGGER_BABUNI__ */
	(*_convert).init( reinterpret_cast<char const *>( doc.get()->encoding ),
			root, _streamId );
	using yaal::swap;
	swap( _xml->_doc, doc );
	parse_dtd( _xml->_doc.get()->intSubset );
	return;
	M_EPILOG
}

void HXml::parse_dtd( void* dtd_ ) {
	M_PROLOG
	if ( dtd_ ) {
		xmlDtdPtr dtd( static_cast<xmlDtdPtr>( dtd_ ) );
		xmlNodePtr node( dtd->children );
		while ( node ) {
			if ( ( node->type == XML_ENTITY_DECL ) && ( node->name && node->content ) ) {
				_entities[ reinterpret_cast<char const*>( node->name ) ] = reinterpret_cast<char const*>( node->content );
			} else {
				log( LOG_LEVEL::ERROR ) << "XML: `" << _streamId << ":" << node->line
					<< "' failed to handle DTD child: " << static_cast<int>( node->type )
					<< " " << ( node->name ? reinterpret_cast<char const*>( node->name ) : "(nil)" )
					<< " " << ( node->content ? reinterpret_cast<char const*>( node->content ) : "(nil)" )<< endl;
			}
			node = node->next;
		}
	}
	return;
	M_EPILOG
}

void HXml::init( yaal::hcore::HStreamInterface::ptr_t stream, parser_t parser_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	init( *stream, parser_ );
	return;
	M_EPILOG
}

void HXml::parse( xml_node_ptr_t data_, tree_t::node_t node_, parser_t parser_ ) {
	M_PROLOG
	xmlNodePtr node = static_cast<xmlNodePtr>( data_ );
	while ( node ) {
		switch ( node->type ) {
			case ( XML_DOCUMENT_NODE ):
				node = node->children;
			continue;
			case ( XML_DTD_NODE ): {
				xmlNodePtr child = node->children;
				while ( child ) {
					parse( child, node_, parser_ );
					child = child->next;
				}
				node = node->next; /* FIXME add DTD handling later */
			}
			continue;
			case ( XML_ELEMENT_NODE ): {
				if ( node_ ) {
					node_ = &*node_->add_node( HNode( this, node->line ) );
				} else {
					node_ = _domTree.create_new_root( HNode( this, node->line ) );
				}
				if ( node->nsDef ) {
					xmlNs* nsDef( node->nsDef );
					while ( nsDef ) {
						HNameSpace ns(
							nsDef->prefix ? reinterpret_cast<char const*>( node->nsDef->prefix ) : "",
							nsDef->href ? reinterpret_cast<char const*>( node->nsDef->href ) : ""
						);
						namespaces_t::iterator it( _namespaces.insert( ns ).first );
						HNode::namespace_definitions_t& n( (**node_)._namespaceDefinitions );
						if ( find( n.begin(), n.end(), &*it ) == n.end() ) {
							n.push_back( &*it );
						}
						nsDef = nsDef->next;
					}
				}
				(**node_)._text = reinterpret_cast<char const*>( node->name );
				if ( node->properties ) {
					xmlAttrPtr attribute = node->properties;
					while ( attribute ) {
						char const* name( reinterpret_cast<char const*>( attribute->name ) );
						if ( name ) {
							if ( attribute->children ) {
								(**node_)._properties[ name ] = attribute->children->content ? convert(
										reinterpret_cast<char*>( attribute->children->content ) ) : "";
							}
						}
						attribute = attribute->next;
					}
				}
				xmlNodePtr child = node->children;
				while ( child ) {
					parse( child, node_, parser_ );
					child = child->next;
				}
			}
			break;
			case ( XML_ENTITY_REF_NODE ): {
				M_ASSERT( node->name );
				entities_t::const_iterator it( _entities.find( reinterpret_cast<char const*>( node->name ) ) );
				M_ENSURE_EX( it != _entities.end(), HString( "entity not found: " ) + reinterpret_cast<char const*>( node->name ) + ", at: " + node->line );
				node_->add_node( HNode( this, HNode::TYPE::ENTITY, reinterpret_cast<char const*>( node->name ), node->line ) );
			}
			break;
			case ( XML_TEXT_NODE ): if ( node->content ) {
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				if ( ( parser_ & PARSER::KEEP_EMPTY ) || ( _varTmpBuffer.find_other_than( _whiteSpace_.data() ) >= 0 ) ) {
					node_->add_node( HNode( this, HNode::TYPE::CONTENT, _varTmpBuffer, node->line ) );
				}
			}
			break;
			case ( XML_COMMENT_NODE ): if ( ! ( parser_ & PARSER::STRIP_COMMENT ) && node->content ) {
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				node_->add_node( HNode( this, HNode::TYPE::COMMENT, _varTmpBuffer, node->line ) );
			}
			break;
			case ( XML_XINCLUDE_START ):
			case ( XML_XINCLUDE_END ):
				/*
				 * We support xinclude but we do anything special about them
				 * except automatical parsing.
				 */
			break;
			default:
				log( LOG_LEVEL::WARNING ) << "XML: `" << _streamId << "' unsupported type: " << static_cast<int>( node->type )
					<< ", at: " << node->line << endl;
			break;
		}
		node = nullptr;
	}
	return;
	M_EPILOG
}

void HXml::apply_style( yaal::hcore::HString const& path_, parameters_t const& parameters_ ) {
	M_PROLOG
	if ( !! get_root() ) {
		generate_intermediate_form( false );
	}
	M_ASSERT( _xml->_doc.get() );
	HXsltParserG::get_instance();
	xsltStylesheet* pstyle( xsltParseStylesheetFile( reinterpret_cast<xmlChar const*>( path_.raw() ) ) );
	M_ENSURE_EX( pstyle, HString( "failure parsing XSLT file: " ) + path_ );
	style_resource_t style( pstyle, xsltFreeStylesheet );
	HResource<char const*[]> parametersHolder( !parameters_.is_empty() ? new char const*[parameters_.get_size() + 1] : nullptr );
	char const** parameters( parametersHolder.get() );
	int parIdx( 0 );
	for ( parameter_t const& p : parameters_ ) {
		parameters[parIdx ++] = p.first.raw();
		parameters[parIdx ++] = p.second.raw();
	}
	if ( parIdx > 0 ) {
		parameters[parIdx] = nullptr;
	}
	doc_resource_t doc(
		xsltApplyStylesheet(
			style.get(),
			_xml->_doc.get(),
			parameters
		),
		xmlFreeDoc
	);
	if ( ! doc.get() ) {
		throw HXmlException( HString( "cannot apply stylesheet: " ) + path_ );
	}
	_xml->clear();
	using yaal::swap;
	swap( _xml->_doc, doc );
	swap( _xml->_style, style );
	M_EPILOG
}

void HXml::parse( void ) {
	M_PROLOG
	parse( FULL_TREE, PARSER::DEFAULT );
	return;
	M_EPILOG
}

void HXml::parse( HString const& xPath_ ) {
	M_PROLOG
	parse( xPath_, PARSER::DEFAULT );
	return;
	M_EPILOG
}

void HXml::parse( parser_t parser_ ) {
	M_PROLOG
	parse( FULL_TREE, parser_ );
	return;
	M_EPILOG
}

void HXml::parse( HString const& xPath_, parser_t parser_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	HString xPath( xPath_.is_empty() ? FULL_TREE : xPath_.raw() );
	get_node_set_by_path( xPath );
	_domTree.clear();
	int ctr = 0;
	while ( xPath[ ctr ] )
		ctr ++;
	ctr --;
	M_ASSERT( ctr >= 0 );
	if ( _xml->_nodeSet ) {
		if ( xPath != FULL_TREE ) {
			tree_t::node_t root = _domTree.create_new_root( HNode( this ) );
			(**root)._text = "xpath_result_set";
			for ( ctr = 0; ctr < _xml->_nodeSet->nodeNr; ++ ctr )
				parse( _xml->_nodeSet->nodeTab[ ctr ],
						root, parser_ );
		} else
			parse( xmlDocGetRootElement( _xml->_doc.get() ), nullptr, parser_ );
	}
	M_EPILOG
}

void HXml::load( yaal::hcore::HStreamInterface& stream, parser_t parser_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	M_ENSURE( stream.is_valid() );
	init( stream, parser_ );
	parse( parser_ );
	return;
	M_EPILOG
}

void HXml::load( yaal::hcore::HStreamInterface::ptr_t stream, parser_t parser_ ) {
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	load( *stream, parser_ );
	return;
	M_EPILOG
}

void HXml::generate_intermediate_form( bool indent_ ) const {
	M_PROLOG
	doc_resource_t doc;
	M_ENSURE( !! get_root() );
	/* scope for dummy */ {
		xmlDocPtr pDoc = nullptr;
		writer_resource_t writer( xmlNewTextWriterDoc( &pDoc, 0 ), xmlFreeTextWriter );
		if ( ! writer.get() ) {
			throw HXmlException( _( "Cannot create the xml DOC writer." ) );
		}
		doc_resource_t dummy( pDoc, xmlFreeDoc );
		using yaal::swap;
		swap( doc, dummy );
		int rc = ::xmlTextWriterStartDocument( writer.get(), nullptr, _encoding.raw(), nullptr );
		if ( rc < 0 ) {
			throw HXmlException( HString( "Unable to start document with encoding: " ) + _encoding );
		}
		if ( indent_ ) {
			rc = xmlTextWriterSetIndent( writer.get(), 1 );
			if ( rc < 0 ) {
				throw HXmlException( "Unable to enable indenting." );
			}
			static char const INDENTION_STRING[] = "\t";
			rc = xmlTextWriterSetIndentString( writer.get(), reinterpret_cast<xmlChar const*>( INDENTION_STRING ) );
			if ( rc < 0 ) {
				throw HXmlException( "Cannot set indent string." );
			}
		}
		if ( ! _convert->is_initialized() ) {
			_convert->init( _encoding );
		}
		if ( ! _entities.is_empty() ) {
			rc = xmlTextWriterStartDTD( writer.get(), reinterpret_cast<xmlChar const*>( "workaround" ), nullptr, nullptr );
			if ( rc < 0 ) {
				throw HXmlException( "Unable to start DTD section." );
			}
			for ( entities_t::const_iterator it( _entities.begin() ), end( _entities.end() ); it != end; ++ it ) {
				rc = xmlTextWriterWriteDTDInternalEntity( writer.get(), 0,
						reinterpret_cast<xmlChar const*>( it->first.raw() ),
						reinterpret_cast<xmlChar const*>( convert( it->second, TO_EXTERNAL ).raw() ) );
				if ( rc < 0 ) {
					throw HXmlException( HString( "Cannot save entity declaration: " ) + it->first );
				}
			}
			rc = xmlTextWriterEndDTD( writer.get() );
			if ( rc < 0 ) {
				throw HXmlException( "Unable to end DTD section." );
			}
		}
		dump_node( &writer, get_root() );
		rc = xmlTextWriterEndDocument( writer.get() );
		if ( rc < 0 ) {
			throw HXmlException( "Unable to end document." );
		}
	}
	_xml->clear();
	if ( _xml->_doc.get() ) {
		xmlFreeNode( xmlDocSetRootElement( const_cast<xmlDoc*>( _xml->_doc.get() ), xmlDocGetRootElement( doc.get() ) ) );
	} else {
		using yaal::swap;
		swap( const_cast<doc_resource_t&>( _xml->_doc ), doc );
	}
	return;
	M_EPILOG
}

void HXml::save( yaal::hcore::HStreamInterface& stream, bool indent_ ) const {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	M_ENSURE( stream.is_valid() );
	/* flush writer to DOM. */
	generate_intermediate_form( indent_ );
	M_ASSERT( _xml->_doc.get() );
	if ( _xml->_style.get() ) {
		outputbuffer_resource_t obuf( ::xmlOutputBufferCreateIO( writer_callback,
							nullptr, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
				xmlOutputBufferClose );
		M_ENSURE( ::xsltSaveResultTo( obuf.get(),
					const_cast<xmlDoc*>( _xml->_doc.get() ), const_cast<xsltStylesheet*>( _xml->_style.get() ) ) != -1 );
	} else {
		M_ENSURE( ::xmlSaveFileTo( ::xmlOutputBufferCreateIO( writer_callback,
						nullptr, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
					const_cast<xmlDoc*>( _xml->_doc.get() ), _encoding.raw() ) != -1 );
	}
	return;
	M_EPILOG
}

void HXml::save( yaal::hcore::HStreamInterface::ptr_t stream, bool indent_ ) const {
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	save( *stream, indent_ );
	return;
	M_EPILOG
}

int HXml::writer_callback( void* stream, char const* buf, int len ) {
	return ( static_cast<int>( static_cast<HStreamInterface*>( stream )->write( buf, len ) ) );
}

int HXml::reader_callback( void* stream, char* buf, int len ) {
	return ( static_cast<int>( static_cast<HStreamInterface*>( stream )->read( buf, len ) ) );
}

void HXml::dump_node( void* writer_p, HConstNodeProxy const& node_ ) const {
	M_PROLOG
	writer_resource_t& writer = *static_cast<writer_resource_t*>( writer_p );
	HString const& str = node_.get_name();
	int rc = xmlTextWriterStartElement( writer.get(), reinterpret_cast<xmlChar const*>( str.raw() ) );
	if ( rc < 0 ) {
		throw HXmlException( HString( "Unable to write start element: " ) + str );
	}
	HNode::properties_t const& prop = node_.properties();
	for ( HNode::properties_t::const_iterator it( prop.begin() ), end( prop.end() ); it != end; ++ it ) {
		HString const& pname = it->first;
		HString const& pvalue = it->second;
		rc = xmlTextWriterWriteAttribute( writer.get(),
				reinterpret_cast<xmlChar const*>( pname.raw() ),
				reinterpret_cast<xmlChar const*>( convert( pvalue.raw(), TO_EXTERNAL ).raw() ) );
		if ( rc < 0 ) {
			throw HXmlException( HString( "Unable to write a property: " ) + str + ", with value: " + pvalue );
		}
	}
	HString prefix;
	for ( HNameSpace const* ns : (*(node_._node))->_namespaceDefinitions ) {
		prefix = "xmlns";
		if ( ! ns->prefix().is_empty() ) {
			prefix.append( ":" ).append( ns->prefix() );
		}
		rc = xmlTextWriterWriteAttribute( writer.get(),
			reinterpret_cast<xmlChar const*>( prefix.raw() ),
			reinterpret_cast<xmlChar const*>( ns->href().raw() )
		);
		if ( rc < 0 ) {
			throw HXmlException(
				"Unable to write a namespace definition: "_ys
					.append( ns->prefix() )
					.append( ":" )
					.append( ns->href() )
			);
		}
	}
	for ( HXml::HConstIterator it( node_.begin() ), end( node_.end() ); it != end; ++ it ) {
		HXml::HNode::TYPE type( (*it).get_type() );
		if ( type == HXml::HNode::TYPE::NODE ) {
			dump_node( writer_p, *it );
		} else if ( type == HXml::HNode::TYPE::CONTENT ) {
			HString const& value = (*it).get_value();
			if ( ! value.is_empty() && ( value[0] == '&' ) && ( value[value.get_length() - 1] == ';' ) ) {
				rc = xmlTextWriterWriteRaw( writer.get(),
						reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			} else {
				rc = xmlTextWriterWriteString( writer.get(),
						reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			}
			if ( rc < 0 ) {
				throw HXmlException( HString( "Unable to write a node value: " ) + value );
			}
		} else if ( type == HXml::HNode::TYPE::ENTITY ) {
			HString const& name( (*it).get_name() );
			rc = xmlTextWriterWriteFormatRaw( writer.get(),
					"&%s;", reinterpret_cast<xmlChar const*>( name.raw() ) );
			if ( rc < 0 ) {
				throw HXmlException( HString( "Unable to write an entity referenc: " ) + name );
			}
		} else {
			M_ASSERT( type == HXml::HNode::TYPE::COMMENT );
			rc = xmlTextWriterStartComment( writer.get() );
			if ( rc < 0 ) {
				throw HXmlException( "Unable to write start comment." );
			}
			HString const& value = (*it).get_value();
			rc = xmlTextWriterWriteString( writer.get(),
					reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			if ( rc < 0 ) {
				throw HXmlException( HString( "Unable to write a comment value: " ) + value );
			}
			rc = xmlTextWriterEndComment( writer.get() );
			if ( rc < 0 ) {
				throw HXmlException( "Unable to write end comment." );
			}
		}
	}
	rc = xmlTextWriterEndElement( writer.get() );
	if ( rc < 0 ) {
		throw HXmlException( HString( "Unable to write end element: " ) + str );
	}
	return;
	M_EPILOG
}

void HXml::create_root( yaal::hcore::HString const& name_, yaal::hcore::HString const& encoding_ ) {
	M_PROLOG
	M_ASSERT( name_ );
	_encoding = ( !! encoding_ ) ? encoding_ : _defaultEncoding_;
	_convert->init( _encoding );
	tree_t::node_t root = _domTree.create_new_root( HNode( this ) );
	(**root)._text = name_;
	return;
	M_EPILOG
}

HXml::HNodeProxy HXml::get_root( void ) {
	M_PROLOG
	return ( HNodeProxy( _domTree.get_root() ) );
	M_EPILOG
}

HXml::HConstNodeProxy const HXml::get_root( void ) const {
	M_PROLOG
	return ( HConstNodeProxy( _domTree.get_root() ) );
	M_EPILOG
}

HXml::entities_t const& HXml::entities( void ) const {
	M_PROLOG
	return ( _entities );
	M_EPILOG
}

HXml::HNodeProxy::HNodeProxy( void )
	: HConstNodeProxy() {
}

HXml::HConstNodeProxy::HConstNodeProxy( void )
	: _node( nullptr ) {
}

HXml::HNodeProxy::HNodeProxy( HXml::tree_t::node_t node_ )
	: HConstNodeProxy( node_ ) {
}

HXml::HConstNodeProxy::HConstNodeProxy( HXml::tree_t::const_node_t node_ )
	: _node( const_cast<HXml::tree_t::node_t>( node_ ) ) {
}

HXml::HNodeProxy::HNodeProxy( HNodeProxy const& other )
	: HConstNodeProxy( other ) {
}

HXml::HConstNodeProxy::HConstNodeProxy( HConstNodeProxy const& other )
	: _node( other._node ) {
}

HXml::HConstNodeProxy::HConstNodeProxy( HNodeProxy const& other )
	: _node( other._node ) {
}

HXml::HNodeProxy& HXml::HNodeProxy::operator = ( HNodeProxy const& other ) {
	if ( &other != this )
		_node = other._node;
	return ( *this );
}

HXml::HConstNodeProxy& HXml::HConstNodeProxy::operator = ( HConstNodeProxy const& other ) {
	if ( &other != this )
		_node = other._node;
	return ( *this );
}

bool HXml::HConstNodeProxy::disjointed( HXml::HConstNodeProxy const& node_ ) const {
	M_PROLOG
	return ( _node->disjointed( node_._node ) );
	M_EPILOG
}

HXml const* HXml::HConstNodeProxy::xml( void ) const {
	return ( (**_node)._owner );
}

int HXml::HConstNodeProxy::get_level( void ) const {
	M_PROLOG
	M_ASSERT( _node );
	return ( _node->get_level() );
	M_EPILOG
}

int HXml::HConstNodeProxy::get_line( void ) const {
	M_PROLOG
	M_ASSERT( _node );
	return ( (**_node)._line );
	M_EPILOG
}

HXml::HNode::TYPE HXml::HConstNodeProxy::get_type( void ) const {
	M_PROLOG
	M_ASSERT( _node );
	return ( (**_node)._type );
	M_EPILOG
}

HString const& HXml::HConstNodeProxy::get_name( void ) const {
	M_PROLOG
	M_DEBUG_CODE( HXml::HNode::TYPE type( (**_node)._type ); );
	M_ASSERT( _node && ( ( type == HXml::HNode::TYPE::NODE ) || ( type == HXml::HNode::TYPE::ENTITY ) ) );
	return ( (**_node)._text );
	M_EPILOG
}

HString const& HXml::HConstNodeProxy::get_value( void ) const {
	M_PROLOG
	HXml::HNode::TYPE type( (**_node)._type );
	M_ASSERT( _node && ( ( type == HXml::HNode::TYPE::CONTENT ) || ( type == HXml::HNode::TYPE::COMMENT ) || ( type == HXml::HNode::TYPE::ENTITY ) ) );
	HString const* val( nullptr );
	if ( type == HXml::HNode::TYPE::ENTITY ) {
		HXml::const_entity_iterator it( (**_node)._owner->_entities.find( (**_node)._text ) );
		M_ASSERT( it != (**_node)._owner->entities().end() );
		val = &it->second;
	}
	return ( type == HXml::HNode::TYPE::ENTITY ? *val : (**_node)._text );
	M_EPILOG
}

HXml* HXml::HNodeProxy::xml( void ) {
	return ( const_cast<HXml*>( (**_node)._owner ) );
}

HXml::HNode::properties_t& HXml::HNodeProxy::properties( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (**_node)._properties );
	M_EPILOG
}

HXml::HConstNodeProxy HXml::HConstNodeProxy::get_parent( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (*_node).get_parent() );
	M_EPILOG
}

bool HXml::HConstNodeProxy::operator == ( HConstNodeProxy const& other_ ) const {
	M_PROLOG
	return ( _node == other_._node );
	M_EPILOG
}

bool HXml::HConstNodeProxy::operator != ( HConstNodeProxy const& other_ ) const {
	M_PROLOG
	return ( _node != other_._node );
	M_EPILOG
}

HXml::HNode::properties_t const& HXml::HConstNodeProxy::properties( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (**_node)._properties );
	M_EPILOG
}

HXml::HNodeProxy HXml::HNodeProxy::get_parent( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (*_node).get_parent() );
	M_EPILOG
}

void HXml::HNodeProxy::set_name( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	(**_node)._text = name_;
	return;
	M_EPILOG
}

void HXml::HNodeProxy::set_value( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::CONTENT ) );
	(**_node)._text = value_;
	return;
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::add_node( HXml::HNode::TYPE type_, yaal::hcore::HString const& name_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator it = _node->add_node( (**_node)._owner );
	(**it)._text = name_;
	(**it)._type = type_;
	return ( HXml::HIterator( _node, it ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::add_node( yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator it = _node->add_node( (**_node)._owner );
	(**it)._text = name_;
	(**it)._type = HXml::HNode::TYPE::NODE;
	if ( !! value_ ) {
		tree_t::HNode::iterator value = it->add_node( (**_node)._owner );
		(**value)._text = value_;
		(**value)._type = HXml::HNode::TYPE::CONTENT;
	}
	return ( HXml::HIterator( _node, it ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, HXml::HNode::TYPE type_, yaal::hcore::HString const& name_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->insert_node( it._iterator, HNode( (**_node)._owner ) );
	(**newIt)._text = name_;
	(**newIt)._type = type_;
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->insert_node( it._iterator, HNode( (**_node)._owner ) );
	(**newIt)._text = name_;
	(**newIt)._type = HXml::HNode::TYPE::NODE;;
	if ( !! value_ ) {
		tree_t::HNode::iterator value = newIt->add_node( (**_node)._owner );
		(**value)._text = value_;
		(**value)._type = HXml::HNode::TYPE::CONTENT;
	}
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

void HXml::HNodeProxy::reset_owner( HXml const* owner_ ) {
	M_PROLOG
	if ( (**_node)._owner != owner_ ) {
		(**_node)._namespaceDefinitions.clear();
	}
	(**_node)._owner = owner_;
	for ( HXml::tree_t::HNode& e : *_node ) {
		HNodeProxy( &e ).reset_owner( owner_ );
	}
	return;
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HIterator it, HXml::HNodeProxy node ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->move_node( it._iterator, node._node );
	reset_owner( (**_node)._owner );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HNodeProxy node ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->move_node( node._node );
	reset_owner( (**_node)._owner );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::replace_node( HXml::HIterator it, HXml::HNodeProxy node ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->replace_node( it._iterator, node._node );
	reset_owner( (**_node)._owner );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HIterator it, HXml::HNodeProxy node ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->copy_node( it._iterator, node._node );
	reset_owner( (**_node)._owner );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HNodeProxy node ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->copy_node( node._node );
	reset_owner( (**_node)._owner );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

bool HXml::HConstNodeProxy::has_childs( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( _node->has_childs() );
	M_EPILOG
}

int long HXml::HConstNodeProxy::child_count( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( _node->child_count() );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::begin( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HIterator( _node, _node->begin() ) );
	M_EPILOG
}

HXml::HConstIterator HXml::HConstNodeProxy::begin( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HConstIterator( _node, const_cast<HXml::tree_t::const_node_t>( _node )->begin() ) );
	M_EPILOG
}

HXml::HIterator HXml::HNodeProxy::end( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HIterator( _node, _node->end() ) );
	M_EPILOG
}

HXml::HConstIterator HXml::HConstNodeProxy::end( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HConstIterator( _node, const_cast<HXml::tree_t::const_node_t>( _node )->end() ) );
	M_EPILOG
}

HXml::HNodeProxy::reverse_iterator HXml::HNodeProxy::rbegin( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( end() );
	M_EPILOG
}

HXml::HConstNodeProxy::const_reverse_iterator HXml::HConstNodeProxy::rbegin( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( end() );
	M_EPILOG
}

HXml::HNodeProxy::reverse_iterator HXml::HNodeProxy::rend( void ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( begin() );
	M_EPILOG
}

HXml::HConstNodeProxy::const_reverse_iterator HXml::HConstNodeProxy::rend( void ) const {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( begin() );
	M_EPILOG
}

bool HXml::HNodeProxy::operator ! ( void ) const {
	return ( ! _node );
}

bool HXml::HConstNodeProxy::operator ! ( void ) const {
	return ( ! _node );
}

HXml::HIterator::HIterator( void )
	: base_type(), _owner( nullptr ), _iterator() {
}

HXml::HConstIterator::HConstIterator( void )
	: base_type(), _owner( nullptr ), _iterator() {
}

HXml::HIterator::HIterator( HXml::tree_t::node_t owner_, HXml::tree_t::HNode::iterator const& it )
	: base_type(), _owner( owner_ ), _iterator( it ) {
}

HXml::HConstIterator::HConstIterator( HXml::tree_t::const_node_t owner_, HXml::tree_t::HNode::const_iterator const& it )
	: base_type(), _owner( owner_ ), _iterator( it ) {
}

HXml::HIterator::HIterator( HXml::HIterator const& other )
	: base_type(), _owner( other._owner ), _iterator( other._iterator ) {
}

HXml::HConstIterator::HConstIterator( HXml::HConstIterator const& other )
	: base_type(), _owner( other._owner ), _iterator( other._iterator ) {
}

HXml::HConstIterator::HConstIterator( HXml::HIterator const& other )
	: base_type(), _owner( other._owner ), _iterator( other._iterator ) {
}

HXml::HIterator& HXml::HIterator::operator = ( HXml::HIterator const& other ) {
	if ( &other != this ) {
		_owner = other._owner;
		_iterator = other._iterator;
	}
	return ( *this );
}

HXml::HConstIterator& HXml::HConstIterator::operator = ( HXml::HConstIterator const& other ) {
	if ( &other != this ) {
		_owner = other._owner;
		_iterator = other._iterator;
	}
	return ( *this );
}

bool HXml::HIterator::operator == ( HXml::HIterator const& other ) const {
	return ( _iterator == other._iterator );
}

bool HXml::HConstIterator::operator == ( HXml::HConstIterator const& other ) const {
	return ( _iterator == other._iterator );
}

bool HXml::HIterator::operator != ( HXml::HIterator const& other ) const {
	return ( _iterator != other._iterator );
}

bool HXml::HConstIterator::operator != ( HXml::HConstIterator const& other ) const {
	return ( _iterator != other._iterator );
}

HXml::HNodeProxy HXml::HIterator::operator* ( void ) const {
	M_PROLOG
	M_ASSERT( _owner );
	M_ASSERT( _iterator != _owner->end() );
	return ( HXml::HNodeProxy( &*_iterator ) );
	M_EPILOG
}

HXml::HConstNodeProxy HXml::HConstIterator::operator* ( void ) const {
	M_PROLOG
	M_ASSERT( _owner );
	M_ASSERT( _iterator != const_cast<HXml::tree_t::const_node_t>( _owner )->end() );
	return ( HXml::HConstNodeProxy( &*_iterator ) );
	M_EPILOG
}

HXml::const_xml_element_t HXml::get_element_by_id( const_xml_element_t const& node, yaal::hcore::HString const& id ) const {
	M_PROLOG
	const_xml_element_t result = nullptr;
	HXml::HNode::properties_t::const_iterator idIt = (*node)->_properties.find( "id" );
	if ( ( idIt != (*node)->_properties.end() ) && ( idIt->second == id ) ) {
		result = node;
	}
	for ( tree_t::HNode::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it ) {
		result = get_element_by_id( &*it, id );
	}
	return ( result );
	M_EPILOG
}

HXml::HNodeProxy HXml::get_element_by_id( yaal::hcore::HString const& id ) {
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_id( _domTree.get_root(), id ) ) ) );
	M_EPILOG
}

HXml::HConstNodeProxy HXml::get_element_by_id( yaal::hcore::HString const& id ) const {
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_id( _domTree.get_root(), id ) ) );
	M_EPILOG
}

HXml::HNodeSet HXml::get_elements_by_name( yaal::hcore::HString const& name_ ) {
	return ( HNodeProxy( _domTree.get_root() ).get_elements_by_name( name_ ) );
}

HXml::HConstNodeSet HXml::get_elements_by_name( yaal::hcore::HString const& name_ ) const {
	return ( HConstNodeProxy( _domTree.get_root() ).get_elements_by_name( name_ ) );
}

HXml::HNodeSet HXml::get_elements_by_path( yaal::hcore::HString const& name_ ) {
	return ( HNodeProxy( _domTree.get_root() ).get_elements_by_path( name_ ) );
}

HXml::HConstNodeSet HXml::get_elements_by_path( yaal::hcore::HString const& name_ ) const {
	return ( HConstNodeProxy( _domTree.get_root() ).get_elements_by_path( name_ ) );
}

HXml::HIterator HXml::HNodeProxy::remove_node( HXml::HIterator it ) {
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::HNode::iterator newIt = _node->remove_node( it._iterator );
	return ( HXml::HIterator( _node, newIt ) );
	M_EPILOG
}

void HXml::HConstNodeProxy::get_elements_by_path( HXml::HConstNodeSet& ns_, const_xml_element_t node,
		HTokenizer const& path, HTokenizer::iterator part ) const {
	M_PROLOG
	const_xml_element_t result = nullptr;
	HString name( *part );
	if ( ! name.is_empty() ) {
		if ( (**node)._text == name ) {
			++ part;
			if ( part != path.end() ) {
				for ( tree_t::HNode::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it ) {
					if ( (**it)._type == HXml::HNode::TYPE::NODE ) {
						get_elements_by_path( ns_, &*it, path, part );
					}
				}
			} else {
				ns_.add( node );
			}
		}
	}
	return;
	M_EPILOG
}

HXml::HNodeSet HXml::HNodeProxy::get_elements_by_path( yaal::hcore::HString const& path_ ) {
	M_PROLOG
	HString path( path_ );
	path.trim_right( "/" );
	HTokenizer t( path, "/" );
	HNodeSet ns;
	HTokenizer::iterator it( t.begin() );
	if ( it != t.end() ) {
		++ it;
		if ( it != t.end() ) {
			get_elements_by_path( ns, _node, t, it );
		}
	}
	return ( ns );
	M_EPILOG
}

HXml::HConstNodeSet HXml::HConstNodeProxy::get_elements_by_path( yaal::hcore::HString const& path_ ) const {
	M_PROLOG
	HString path( path_ );
	path.trim_right( "/" );
	HTokenizer t( path, "/" );
	HConstNodeSet ns;
	HTokenizer::iterator it( t.begin() );
	if ( it != t.end() ) {
		++ it;
		if ( it != t.end() ) {
			get_elements_by_path( ns, _node, t, it );
		}
	}
	return ( ns );
	M_EPILOG
}

HXml::HConstNodeSet HXml::HConstNodeProxy::get_elements_by_name( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	HConstNodeSet ns;
	get_elements_by_name( ns, _node, name_ );
	return ( ns );
	M_EPILOG
}

HXml::HNodeSet HXml::HNodeProxy::get_elements_by_name( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HNodeSet ns;
	get_elements_by_name( ns, const_cast<const_xml_element_t>( _node ), name_ );
	return ( ns );
	M_EPILOG
}

void HXml::HConstNodeProxy::get_elements_by_name( yaal::tools::HXml::HConstNodeSet& ns_, const_xml_element_t node_, yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	if ( (**node_)._text == name_ ) {
		ns_.add( node_ );
	}
	for ( tree_t::HNode::const_iterator it( node_->begin() ), endIt( node_->end() ); it != endIt; ++ it ) {
		if ( (**it)._type == HXml::HNode::TYPE::NODE ) {
			get_elements_by_name( ns_, &*it, name_ );
		}
	}
	return;
	M_EPILOG
}

HXml::HConstNodeSet::HConstNodeSet( void )
	: _nodes() {
	return;
}

void HXml::HConstNodeSet::add( HXml::const_xml_element_t node_ ) {
	M_PROLOG
	_nodes.push_back( node_ );
	return;
	M_EPILOG
}

HXml::HConstNodeSet::HConstIterator HXml::HConstNodeSet::begin( void ) const {
	M_PROLOG
	return ( HXml::HConstNodeSet::HConstIterator( this, 0 ) );
	M_EPILOG
}

HXml::HConstNodeSet::HConstIterator HXml::HConstNodeSet::end( void ) const {
	M_PROLOG
	return ( HXml::HConstNodeSet::HConstIterator( this, static_cast<int>( _nodes.get_size() ) ) );
	M_EPILOG
}

int HXml::HConstNodeSet::get_size( void ) const {
	M_PROLOG
	return ( static_cast<int>( _nodes.get_size() ) );
	M_EPILOG
}

int HXml::HConstNodeSet::size( void ) const {
	return ( get_size() );
}

bool HXml::HConstNodeSet::is_empty( void ) const {
	M_PROLOG
	return ( _nodes.is_empty() );
	M_EPILOG
}

bool HXml::HConstNodeSet::empty( void ) const {
	return ( is_empty() );
}

HXml::HConstNodeProxy HXml::HConstNodeSet::operator[]( int index_ ) const {
	M_PROLOG
	return ( _nodes[index_] );
	M_EPILOG
}

HXml::HConstNodeSet::HConstIterator::HConstIterator( HXml::HConstNodeSet const* owner_, int index_ )
	: _owner( owner_ ), _iterator( index_ ) {
	return;
}

HXml::HConstNodeSet::HConstIterator::HConstIterator( HXml::HConstNodeSet::HConstIterator const& it_ )
	: _owner( it_._owner ), _iterator( it_._iterator ) {
	return;
}

HXml::HConstNodeSet::HConstIterator& HXml::HConstNodeSet::HConstIterator::operator = ( HXml::HConstNodeSet::HConstIterator const& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_iterator = it_._iterator;
	}
	return ( *this );
}

HXml::HConstNodeProxy HXml::HConstNodeSet::HConstIterator::operator* ( void ) const {
	M_PROLOG
	return ( _owner->_nodes[_iterator] );
	M_EPILOG
}

bool HXml::HConstNodeSet::HConstIterator::operator != ( HXml::HConstNodeSet::HConstIterator const& other ) const {
	M_PROLOG
	M_ASSERT( _owner == other._owner );
	return ( _iterator != other._iterator );
	M_EPILOG
}

bool HXml::HConstNodeSet::HConstIterator::operator == ( HXml::HConstNodeSet::HConstIterator const& other ) const {
	M_PROLOG
	M_ASSERT( _owner == other._owner );
	return ( _iterator == other._iterator );
	M_EPILOG
}

HXml::HNodeSet::HNodeSet( void )
	: HConstNodeSet() {
	return;
}

HXml::HNodeSet::HIterator HXml::HNodeSet::begin( void ) {
	M_PROLOG
	return ( HXml::HNodeSet::HIterator( this, 0 ) );
	M_EPILOG
}

HXml::HNodeSet::HIterator HXml::HNodeSet::end( void ) {
	M_PROLOG
	return ( HXml::HNodeSet::HIterator( this, static_cast<int>( _nodes.get_size() ) ) );
	M_EPILOG
}

HXml::HNodeProxy HXml::HNodeSet::operator[]( int index_ ) {
	M_PROLOG
	return ( const_cast<HXml::xml_element_t>( _nodes[index_] ) );
	M_EPILOG
}

HXml::HNodeSet::HIterator::HIterator( HXml::HConstNodeSet const* owner_, int index_ )
	: HConstIterator( owner_, index_ ) {
	return;
}

HXml::HNodeProxy HXml::HNodeSet::HIterator::operator* ( void ) {
	M_PROLOG
	return ( const_cast<HXml::xml_element_t>( _owner->_nodes[_iterator] ) );
	M_EPILOG
}

char const* HXml::error_message( int code ) const {
	M_PROLOG
	char const* msg = "Unknow error code";
	switch ( code ) {
		case ( EILSEQ ):
			msg = "An invalid multibyte sequence has been encountered in the input.";
		break;
		case ( EINVAL ):
			msg = "An incomplete multibyte sequence has been encountered in the input.";
		break;
		case ( 0 ):
			msg = "No error.";
		break;
	}
	return ( msg );
	M_EPILOG
}

namespace xml {

value_t try_node_val( HXml::HConstNodeProxy const& node ) {
	M_PROLOG
	HXml::HConstIterator it = node.begin();
	value_t value;
	if ( it != node.end() ) {
		if ( (*it).get_type() == HXml::HNode::TYPE::CONTENT )
			value = (*it).get_value();
	}
	return ( value );
	M_EPILOG
}

value_t try_node_val( HXml::HConstIterator const& it ) {
	M_PROLOG
	return ( try_node_val( *it ) );
	M_EPILOG
}

value_t try_attr_val( HXml::HConstNodeProxy const& node, yaal::hcore::HString const& name ) {
	M_PROLOG
	HXml::HNode::properties_t const& props = node.properties();
	HXml::HNode::properties_t::const_iterator prop = props.find( name );
	value_t value;
	if ( prop != props.end() )
		value = prop->second;
	return ( value );
	M_EPILOG
}

value_t try_attr_val( HXml::HConstIterator const& it, yaal::hcore::HString const& name ) {
	M_PROLOG
	return ( try_attr_val( *it, name ) );
	M_EPILOG
}

HString const& node_val( HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	value_t nodeValue( try_node_val( node_ ) );
	M_ENSURE( nodeValue );
	return ( *nodeValue );
	M_EPILOG
}

HString const& node_val( HXml::HConstIterator const& it_ ) {
	M_PROLOG
	value_t nodeValue( try_node_val( it_ ) );
	M_ENSURE( nodeValue );
	return ( *nodeValue );
	M_EPILOG
}

HString const& attr_val( HXml::HConstNodeProxy const& node_, yaal::hcore::HString const& name_ ) {
	M_PROLOG
	value_t attrValue( try_attr_val( node_, name_ ) );
	M_ENSURE_EX( attrValue, name_ );
	return ( *attrValue );
	M_EPILOG
}

HString const& attr_val( HXml::HConstIterator const& it_, yaal::hcore::HString const& name_ ) {
	M_PROLOG
	value_t attrValue( try_attr_val( it_, name_ ) );
	M_ENSURE_EX( attrValue, name_ );
	return ( *attrValue );
	M_EPILOG
}

value_t try_node_val_by_path( HXml::HConstNodeProxy const& node_, yaal::hcore::HString const& path_ ) {
	M_PROLOG
	HXml::HConstNodeSet ns( node_.get_elements_by_path( path_ ) );
	xml::value_t v;
	if ( ! ns.is_empty() ) {
		v = xml::try_node_val( ns[0] );
	}
	return ( v );
	M_EPILOG
}

value_t try_node_val_by_path( HXml::HConstIterator const& it_, yaal::hcore::HString const& path_ ) {
	M_PROLOG
	return ( try_node_val_by_path( *it_, path_ ) );
	M_EPILOG
}

value_t try_attr_val_by_path( HXml::HConstNodeProxy const& node_, yaal::hcore::HString const& path_ ) {
	M_PROLOG
	int attrPos( static_cast<int>( path_.reverse_find( '/' ) ) );
	M_ENSURE( attrPos != HString::npos );
	HString path( path_.left( attrPos ) );
	HString attr( path_.mid( attrPos + 1 ) );
	HXml::HConstNodeSet ns( node_.get_elements_by_path( path ) );
	xml::value_t v;
	if ( ! ns.is_empty() ) {
		v = xml::try_attr_val( ns[0], attr );
	}
	return ( v );
	M_EPILOG
}

value_t try_attr_val_by_path( HXml::HConstIterator const& it_, yaal::hcore::HString const& path_ ) {
	M_PROLOG
	return ( try_attr_val_by_path( *it_, path_ ) );
	M_EPILOG
}

}

}

namespace hcore {

template<>
int long hash<HXml::HNameSpace>::operator () ( HXml::HNameSpace const& ns_ ) const {
	return ( hash<HString>()( ns_.prefix() ) + hash<HString>()( ns_.href() ) );
}

}

}


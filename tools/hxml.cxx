/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

	hxml.h - this file is integral part of `yaal' project.

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
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hxml.hxx"

#include "hcore/htokenizer.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hlog.hxx"
#include "tools.hxx"
#include "streamtools.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace
{

/* char schema_err[] = "bad xml schema"; */
typedef HResource<xmlDoc, void (*)( xmlDocPtr )> doc_resource_t;
typedef HResource<xmlTextWriter, void (*)( xmlTextWriterPtr )> writer_resource_t;
typedef HPointer<xmlCharEncodingHandler, HPointerScalar, HPointerStrict, int (*)( xmlCharEncodingHandler* )> encoder_resource_t;
typedef HPointer<xsltStylesheet, HPointerScalar, HPointerStrict, void (*)( xsltStylesheetPtr )> style_resource_t;
typedef HResource<xmlXPathContext, void (*)( xmlXPathContextPtr )> xpath_context_resource_t;
typedef HResource<xmlXPathObject, void (*)( xmlXPathObjectPtr )> xpath_object_resource_t;
typedef HResource<xmlOutputBuffer, int (*)( xmlOutputBufferPtr )> outputbuffer_resource_t;

char const* const FULL_TREE = "//*";

}

namespace yaal
{

namespace tools
{

class HXmlParserG : public HSingleton<HXmlParserG>
	{
	HXmlParserG( void );
	~HXmlParserG( void );
	friend class HSingleton<HXmlParserG>;
	friend class HDestructor<HXmlParserG>;
	};

HXmlParserG::HXmlParserG( void )
	{
	}

HXmlParserG::~HXmlParserG( void )
	{
	M_PROLOG
	xmlCleanupParser();
	xmlCleanupCharEncodingHandlers();
	return;
	M_EPILOG
	}

class HXsltParserG : public HSingleton<HXsltParserG>
	{
	HXsltParserG( void );
	~HXsltParserG( void );
	friend class HSingleton<HXsltParserG>;
	friend class HDestructor<HXsltParserG>;
	};

HXsltParserG::HXsltParserG( void )
	{
	exsltRegisterAll();
	xmlSubstituteEntitiesDefault( 0 );
//	xmlLoadExtDtdDefaultValue = 1;
	}

HXsltParserG::~HXsltParserG( void )
	{
	M_PROLOG
	xsltCleanupGlobals();
	return;
	M_EPILOG
	}

struct HXml::OConvert
	{
	encoder_resource_t _encoder;
	iconv_t _iconvToExternal;
	iconv_t _iconvToInternal;
	OConvert( void ) 
		: _encoder(),
		_iconvToExternal( static_cast<iconv_t>( 0 ) ),
		_iconvToInternal( static_cast<iconv_t>( 0 ) ) { }
	OConvert( OConvert const& convert_ )
		: _encoder(),
		_iconvToExternal( static_cast<iconv_t>( 0 ) ),
		_iconvToInternal( static_cast<iconv_t>( 0 ) )
		{
		operator = ( convert_ );
		}
	OConvert& operator = ( OConvert const& convert_ )
		{
		if ( &convert_ != this )
			{
			_encoder = convert_._encoder;
			_iconvToExternal = convert_._iconvToExternal;
			_iconvToInternal = convert_._iconvToInternal;
			}
		return ( *this );
		}
	void init( HString const& encoding_ )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = ::xmlFindCharEncodingHandler( encoding_.raw() );
		if ( ! encoder )
			M_THROW( _( "cannot enable internal convertion" ), errno );
		_encoder = encoder_resource_t( encoder, xmlCharEncCloseFunc );
		_iconvToExternal = _encoder->iconv_in;
		_iconvToInternal = _encoder->iconv_out;
		M_EPILOG
		}
	void init( yaal::hcore::HString const& encoding_, xmlNodePtr root_, yaal::hcore::HString const& fileName_ )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = NULL;
		if ( !! encoding_ )
			encoder = ::xmlFindCharEncodingHandler( encoding_.raw() );
		else
			log( LOG_TYPE::WARNING ) << _( "HXml::WARNING: no encoding declared in `" )
				<< fileName_ << "'." << endl;
		if ( ! encoder )
			{
			log( LOG_TYPE::WARNING ) << _( "HXml::WARNING: char encoding handler not found" ) << endl;
			xmlCharEncoding encoding = ::xmlDetectCharEncoding( root_->name,
					xmlStrlen( root_->name ) );
			if ( ! encoding )
				M_THROW( _( "cannot detect character encoding" ), errno );
			encoder = xmlGetCharEncodingHandler( encoding );
			}
		if ( ! encoder )
			M_THROW( _( "cannot enable internal convertion" ), errno );
		_encoder = encoder_resource_t( encoder, xmlCharEncCloseFunc );
		_iconvToExternal = _encoder->iconv_in;
		_iconvToInternal = _encoder->iconv_out;
		M_EPILOG
		}
	bool operator ! ( void ) const
		{
		return ( ! ( _iconvToExternal && _iconvToInternal ) );
		}
	};

class HXmlData
	{
private:
	friend class HXml;
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
	HXmlData& operator = ( HXmlData const& ) __attribute__(( __noreturn__ ));
	void clear( void ) const;
	};

HXmlData::HXmlData( void )
	: _doc(), _style(), _xPathContext(), _xPathObject(),
	_nodeSet( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HXmlData::HXmlData( HXmlData const& xmlData_ )
	: _doc(), _style(), _xPathContext(), _xPathObject(),
	_nodeSet( NULL )
	{
	M_PROLOG
	_doc = doc_resource_t( ::xmlCopyDoc( const_cast<xmlDoc*>( xmlData_._doc.get() ), 1 ), &::xmlFreeDoc );
	_style = xmlData_._style;
	return;
	M_EPILOG
	}

HXmlData::~HXmlData ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HXmlData::clear( void ) const
	{
	M_PROLOG
	_xPathObject.reset();
	_xPathContext.reset();
	return;
	M_EPILOG
	}

HXml::HXml( void )
	: _convert( new HXml::OConvert ), _convertedString(),
	_varTmpBuffer(), _encoding( _defaultEncoding_ ), _xml(),
	_entities(), _dOM()
	{
	M_PROLOG
	_xml = xml_low_t( new ( std::nothrow ) HXmlData() );
	M_ENSURE( _xml.get() );
	M_EPILOG
	return;
	}

HXml::HXml( HXml const& xml_ )
	: _convert( xml_._convert ), _convertedString( xml_._convertedString ),
	_varTmpBuffer( xml_._varTmpBuffer ), _encoding( xml_._encoding ), _xml(),
	_entities( xml_._entities ), _dOM( xml_._dOM )
	{
	M_PROLOG
	_xml = xml_low_t( new ( std::nothrow ) HXmlData( *xml_._xml ) );
	M_ENSURE( _xml.get() );
	M_EPILOG
	return;
	}

HXml& HXml::operator = ( HXml const& xml_ )
	{
	if ( &xml_ != this )
		{
		HXml tmp( xml_ );
		swap( tmp );
		}
	return ( *this );
	}

void HXml::swap( HXml& xml_ )
	{
	if ( &xml_ != this )
		{
		using yaal::swap;
		swap( xml_._convert, _convert );
		swap( xml_._convertedString, _convertedString );
		swap( xml_._varTmpBuffer, _varTmpBuffer );
		swap( xml_._encoding, _encoding );
		swap( xml_._xml, _xml );
		swap( xml_._entities, _entities );
		swap( xml_._dOM, _dOM );
		}
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

#ifdef HAVE_ICONV_INPUT_CONST
#	define M_YAAL_TOOLS_HXML_ICONV_CONST const
#else /* HAVE_ICONV_INPUT_CONST */
#	define M_YAAL_TOOLS_HXML_ICONV_CONST /**/
#endif /* not HAVE_ICONV_INPUT_CONST */

HString const& HXml::convert( HString const& data_, way_t way_ ) const
	{
	M_PROLOG
	char M_YAAL_TOOLS_HXML_ICONV_CONST* source = const_cast<char M_YAAL_TOOLS_HXML_ICONV_CONST*>( data_.raw() );
	iconv_t cD = static_cast<iconv_t>( 0 );
	switch ( way_ )
		{
		case ( TO_EXTERNAL ): { cD = ( *_convert )._iconvToExternal; break; }
		case ( TO_INTERNAL ): { cD = ( *_convert )._iconvToInternal; break; }
		default:
			{
			M_ASSERT( ! _( "unknown conversion way" ) );
			}
		}
	size_t sizeOut = 0, sizeIn = ::strlen( source );
	/* The longest single character in any encoding is 6 bytes long. */
	size_t const ICONV_OUTPUT_BUFFER_LENGTH = 8;
	/* Additional character for nil terminator. */
	char output[ ICONV_OUTPUT_BUFFER_LENGTH + 1 ];
	_convertedString = "";
	do
		{
		::memset( output, 0, ICONV_OUTPUT_BUFFER_LENGTH + 1 );
		sizeOut = ICONV_OUTPUT_BUFFER_LENGTH;
		char* out = output;
		M_ENSURE( ( ::iconv( cD, &source, &sizeIn, &out,
						&sizeOut ) != static_cast<size_t>( -1 ) )
				|| ( errno == E2BIG ) );
		_convertedString += output;
		}
	while ( sizeIn );
	return ( _convertedString );
	M_EPILOG
	}

#undef M_YAAL_TOOLS_HXML_ICONV_CONST

int HXml::get_node_set_by_path( yaal::hcore::HString const& path_ )
	{
	M_PROLOG
	_varTmpBuffer = path_;
	int long length = _varTmpBuffer.get_length() - 1;
	xpath_context_resource_t ctx( xmlXPathNewContext( _xml->_doc.get() ), xmlXPathFreeContext );
	int setSize = 0;
	_xml->_nodeSet = NULL;
	if ( ctx.get() )
		{
		xmlXPathObjectPtr objPtr = NULL;
		while ( ! _varTmpBuffer.is_empty() )
			{
			objPtr = xmlXPathEvalExpression(
					reinterpret_cast<xmlChar const*>( _varTmpBuffer.raw() ),
					ctx.get() );
			if ( objPtr )
				break;
			_varTmpBuffer.set_at( length --, 0 );
			}
		xpath_object_resource_t obj( objPtr, xmlXPathFreeObject );
		if ( obj.get() )
			{
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

void HXml::init( yaal::hcore::HStreamInterface& stream, PARSER::parser_t parser_ )
	{
	M_PROLOG
	int savedErrno = errno;
	HString error;
	HXmlParserG::get_instance();
	errno = 0;
	HString streamId = get_stream_id( &stream );
	int LOW_LEVEL_PARSING_OPTIONS( XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_XINCLUDE | XML_PARSE_NONET | XML_PARSE_NSCLEAN );
	if ( parser_ & PARSER::RESOLVE_ENTITIES )
		LOW_LEVEL_PARSING_OPTIONS |= XML_PARSE_NOENT;
	doc_resource_t doc( ::xmlReadIO( reader_callback, NULL, &stream, streamId.raw(), NULL, LOW_LEVEL_PARSING_OPTIONS ),
			xmlFreeDoc );
	if ( errno )
		{
		log( LOG_TYPE::WARNING ) << error_message( errno ) << ": " << streamId;
		log << ", code: " << errno << '.' << endl;
		}
	errno = savedErrno;
	if ( ! doc.get() )
		{
		error.format( _( "cannot parse `%s'" ), streamId.raw() );
		throw HXmlException( error );
		}
	if ( xmlXIncludeProcessFlags( doc.get(), LOW_LEVEL_PARSING_OPTIONS ) < 0 )
		throw HXmlException( "a flagged processing XInclude failed" );
	xmlNodePtr root = xmlDocGetRootElement( doc.get() );
	if ( ! root )
		M_THROW( _( "empty doc" ), errno );
#ifdef __DEBUGGER_BABUNI__
	cout << root->name << endl;
#endif /* __DEBUGGER_BABUNI__ */
	(*_convert).init( reinterpret_cast<char const *>( doc.get()->encoding ),
			root, streamId );
	using yaal::swap;
	swap( _xml->_doc, doc );
	parse_dtd( _xml->_doc.get()->intSubset );
	return;
	M_EPILOG
	}

void HXml::parse_dtd( void* dtd_ )
	{
	M_PROLOG
	if ( dtd_ )
		{
		xmlDtdPtr dtd( static_cast<xmlDtdPtr>( dtd_ ) );
		xmlNodePtr node( dtd->children );
		while ( node )
			{
			if ( ( node->type == XML_ENTITY_DECL ) && ( node->name && node->content ) )
				_entities[ reinterpret_cast<char const*>( node->name ) ] = reinterpret_cast<char const*>( node->content );
			else
				log_trace << "failed to handle DTD child: " << static_cast<int>( node->type )
					<< " " << reinterpret_cast<char const*>( node->name )
					<< " " << reinterpret_cast<char const*>( node->content ) << endl;
			node = node->next;
			}
		}
	return;
	M_EPILOG
	}

void HXml::init( yaal::hcore::HStreamInterface::ptr_t stream, PARSER::parser_t parser_ )
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	init( *stream, parser_ );
	return;
	M_EPILOG
	}

void HXml::parse( xml_node_ptr_t data_, tree_t::node_t node_, PARSER::parser_t parser_ )
	{
	M_PROLOG
	xmlNodePtr node = static_cast<xmlNodePtr>( data_ );
	while ( node )
		{
		switch ( node->type )
			{
			case ( XML_DOCUMENT_NODE ):
				node = node->children;
			continue;
			case ( XML_DTD_NODE ):
				{
				xmlNodePtr child = node->children;
				while ( child )
					{
					parse( child, node_, parser_ );
					child = child->next;
					}
				node = node->next; /* FIXME add DTD handling later */
				}
			continue;
			case ( XML_ELEMENT_NODE ):
				{
				if ( node_ )
					node_ = &*node_->add_node( HNode( this ) );
				else
					node_ = _dOM.create_new_root( HNode( this ) );
				(**node_)._text = reinterpret_cast<char const*>( node->name );
				if ( node->properties )
					{
					xmlAttrPtr attribute = node->properties;
					while ( attribute )
						{
						char const* name( reinterpret_cast<char const*>( attribute->name ) );
						if ( name )
							{
							if ( attribute->children )
								(**node_)._properties[ name ] = attribute->children->content ? convert(
										reinterpret_cast<char*>( attribute->children->content ) ) : "";
							}
						attribute = attribute->next;
						}
					}
				xmlNodePtr child = node->children;
				while ( child )
					{
					parse( child, node_, parser_ );
					child = child->next;
					}
				}
			break;
			case ( XML_ENTITY_REF_NODE ):
				{
				M_ASSERT( node->name );
				entities_t::const_iterator it( _entities.find( reinterpret_cast<char const*>( node->name ) ) );
				M_ENSURE_EX( it != _entities.end(), HString( "entity not found: " ) + reinterpret_cast<char const*>( node->name ) );
				node_->add_node( HNode( this, HNode::TYPE::ENTITY, reinterpret_cast<char const*>( node->name ) ) );
				}
			break;
			case ( XML_TEXT_NODE ): if ( node->content )
				{
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				if ( ( parser_ & PARSER::KEEP_EMPTY ) || ( _varTmpBuffer.find_other_than( _whiteSpace_ ) >= 0 ) )
					node_->add_node( HNode( this, HNode::TYPE::CONTENT, _varTmpBuffer ) );
				}
			break;
			case ( XML_COMMENT_NODE ): if ( ! ( parser_ & PARSER::STRIP_COMMENT ) && node->content )
				{
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				node_->add_node( HNode( this, HNode::TYPE::COMMENT, _varTmpBuffer ) );
				}
			break;
			default:
				log_trace << "unsupported type: " << static_cast<int>( node->type ) << endl;
			break;
			}
		node = NULL;
		}
	return;
	M_EPILOG
	}

void HXml::apply_style( yaal::hcore::HString const& path_ )
	{
	M_PROLOG
	M_ASSERT( _xml->_doc.get() );
	HXsltParserG::get_instance();
	style_resource_t style( xsltParseStylesheetFile( reinterpret_cast<xmlChar const* const>( path_.raw() ) ), xsltFreeStylesheet );
	doc_resource_t doc( xsltApplyStylesheet( style.get(), _xml->_doc.get(), NULL ), xmlFreeDoc );
	if ( ! doc.get() )
		throw HXmlException( HString( "cannot apply stylesheet: " ) + path_ );
	_xml->clear();
	using yaal::swap;
	swap( _xml->_doc, doc );
	swap( _xml->_style, style );
	M_EPILOG
	}

void HXml::parse( void )
	{
	M_PROLOG
	parse( FULL_TREE, PARSER::DEFAULT );
	return;
	M_EPILOG
	}

void HXml::parse( HString const& xPath_ )
	{
	M_PROLOG
	parse( xPath_, PARSER::DEFAULT );
	return;
	M_EPILOG
	}

void HXml::parse( PARSER::parser_t parser_ )
	{
	M_PROLOG
	parse( FULL_TREE, parser_ );
	return;
	M_EPILOG
	}

void HXml::parse( HString const& xPath_, PARSER::parser_t parser_ )
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	HString xPath( xPath_.is_empty() ? FULL_TREE : xPath_.raw() );
	get_node_set_by_path( xPath );
	_dOM.clear();
	int ctr = 0;
	while ( xPath[ ctr ] )
		ctr ++;
	ctr --;
	M_ASSERT( ctr >= 0 );
	if ( _xml->_nodeSet )
		{
		if ( xPath != FULL_TREE )
			{
			tree_t::node_t root = _dOM.create_new_root( HNode( this ) );
			(**root)._text = "xpath_result_set";
			for ( ctr = 0; ctr < _xml->_nodeSet->nodeNr; ++ ctr )
				parse( _xml->_nodeSet->nodeTab[ ctr ],
						root, parser_ );
			}
		else
			parse( xmlDocGetRootElement( _xml->_doc.get() ), NULL, parser_ );
		}
	M_EPILOG
	}

void HXml::load( yaal::hcore::HStreamInterface& stream, PARSER::parser_t parser_ )
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	M_ENSURE( stream.is_valid() );
	init( stream );
	parse( parser_ );
	return;
	M_EPILOG
	}

void HXml::load( yaal::hcore::HStreamInterface::ptr_t stream, PARSER::parser_t parser_ )
	{
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	load( *stream, parser_ );
	return;
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface& stream, bool indent_ ) const
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	M_ENSURE( stream.is_valid() );
	doc_resource_t doc;
	/* flush writer to DOM. */
	M_ENSURE( !! get_root() );
		{
		xmlDocPtr pDoc = NULL;
		writer_resource_t writer( xmlNewTextWriterDoc( &pDoc, 0 ), xmlFreeTextWriter );
		if ( ! writer.get() )
			throw HXmlException( _( "Cannot create the xml DOC writer." ) );
		doc_resource_t dummy( pDoc, xmlFreeDoc );
		using yaal::swap;
		swap( doc, dummy );
		int rc = ::xmlTextWriterStartDocument( writer.get(), NULL, _encoding.raw(), NULL );
		if ( rc < 0 )
			throw HXmlException( HString( "Unable to start document with encoding: " ) + _encoding );
		if ( indent_ )
			{
			rc = xmlTextWriterSetIndent( writer.get(), 1 );
			if ( rc < 0 )
				throw HXmlException( "Unable to enable indenting." );
			static char const* const INDENTION_STRING = "\t";
			rc = xmlTextWriterSetIndentString( writer.get(), reinterpret_cast<xmlChar const* const>( INDENTION_STRING ) );
			if ( rc < 0 )
				throw HXmlException( "Cannot set indent string." );
			}
		if ( ! (*_convert) )
			(*_convert).init( _encoding );
		if ( ! _entities.is_empty() )
			{
			rc = xmlTextWriterStartDTD( writer.get(), reinterpret_cast<xmlChar const*>( "workaround" ), NULL, NULL );
			if ( rc < 0 )
				throw HXmlException( "Unable to start DTD section." );
			for ( entities_t::const_iterator it( _entities.begin() ), end( _entities.end() ); it != end; ++ it )
				{
				rc = xmlTextWriterWriteDTDInternalEntity( writer.get(), 0,
						reinterpret_cast<xmlChar const*>( it->first.raw() ),
						reinterpret_cast<xmlChar const*>( convert( it->second, TO_EXTERNAL ).raw() ) );
				if ( rc < 0 )
					throw HXmlException( HString( "Cannot save entity declaration: " ) + it->first );
				}
			rc = xmlTextWriterEndDTD( writer.get() );
			if ( rc < 0 )
				throw HXmlException( "Unable to end DTD section." );
			}
		dump_node( &writer, get_root() );
		rc = xmlTextWriterEndDocument( writer.get() );
		if ( rc < 0 )
			throw HXmlException( "Unable to end document." );
		}
	_xml->clear();
	if ( _xml->_doc.get() )
		xmlFreeNode( xmlDocSetRootElement( const_cast<xmlDoc*>( _xml->_doc.get() ), xmlDocGetRootElement( doc.get() ) ) );
	else
		{
		using yaal::swap;
		swap( const_cast<doc_resource_t&>( _xml->_doc ), doc );
		}
	M_ASSERT( _xml->_doc.get() );
	if ( _xml->_style.get() )
		{
		outputbuffer_resource_t obuf( ::xmlOutputBufferCreateIO( writer_callback,
							NULL, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
				xmlOutputBufferClose );
		M_ENSURE( ::xsltSaveResultTo( obuf.get(),
					const_cast<xmlDoc*>( _xml->_doc.get() ), const_cast<xsltStylesheet*>( _xml->_style.get() ) ) != -1 );
		}
	else
		M_ENSURE( ::xmlSaveFileTo( ::xmlOutputBufferCreateIO( writer_callback,
						NULL, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
					const_cast<xmlDoc*>( _xml->_doc.get() ), _encoding.raw() ) != -1 );
	return;
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface::ptr_t stream, bool indent_ ) const
	{
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	save( *stream, indent_ );
	return;
	M_EPILOG
	}

int HXml::writer_callback( void* stream, char const* buf, int len )
	{
	return ( static_cast<int>( static_cast<HStreamInterface*>( stream )->write( buf, len ) ) );
	}

int HXml::reader_callback( void* stream, char* buf, int len )
	{
	return ( static_cast<int>( static_cast<HStreamInterface*>( stream )->read( buf, len ) ) );
	}

void HXml::dump_node( void* writer_p, HConstNodeProxy const& node_ ) const
	{
	M_PROLOG
	writer_resource_t& writer = *static_cast<writer_resource_t*>( writer_p );
	HString const& str = node_.get_name();
	int rc = xmlTextWriterStartElement( writer.get(), reinterpret_cast<xmlChar const* const>( str.raw() ) );
	if ( rc < 0 )
		throw HXmlException( HString( "Unable to write start element: " ) + str );
	HNode::properties_t const& prop = node_.properties();
	for ( HNode::properties_t::const_iterator it( prop.begin() ), end( prop.end() ); it != end; ++ it )
		{
		HString const& pname = it->first;
		HString const& pvalue = it->second;
		rc = xmlTextWriterWriteAttribute( writer.get(),
				reinterpret_cast<xmlChar const* const>( pname.raw() ),
				reinterpret_cast<xmlChar const* const>( convert( pvalue.raw(), TO_EXTERNAL ).raw() ) );
		if ( rc < 0 )
			throw HXmlException( HString( "Unable to write a property: " ) + str + ", with value: " + pvalue );
		}
	for ( HXml::HConstIterator it( node_.begin() ), end( node_.end() ); it != end; ++ it )
		{
		HXml::HNode::TYPE::type_t type( (*it).get_type() );
		if ( type == HXml::HNode::TYPE::NODE )
			dump_node( writer_p, *it );
		else if ( type == HXml::HNode::TYPE::CONTENT )
			{
			HString const& value = (*it).get_value();
			if ( ! value.is_empty() && ( value[0] == '&' ) && ( value[value.get_length() - 1] == ';' ) )
				rc = xmlTextWriterWriteRaw( writer.get(),
						reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			else
				rc = xmlTextWriterWriteString( writer.get(),
						reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			if ( rc < 0 )
				throw HXmlException( HString( "Unable to write a node value: " ) + value );
			}
		else if ( type == HXml::HNode::TYPE::ENTITY )
			{
			HString const& name( (*it).get_name() );
			rc = xmlTextWriterWriteFormatRaw( writer.get(),
					"&%s;", reinterpret_cast<xmlChar const*>( name.raw() ) );
			if ( rc < 0 )
				throw HXmlException( HString( "Unable to write an entity referenc: " ) + name );
			}
		else
			{
			M_ASSERT( type == HXml::HNode::TYPE::COMMENT );
			rc = xmlTextWriterStartComment( writer.get() );
			if ( rc < 0 )
				throw HXmlException( "Unable to write start comment." );
			HString const& value = (*it).get_value();
			rc = xmlTextWriterWriteString( writer.get(),
					reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			if ( rc < 0 )
				throw HXmlException( HString( "Unable to write a comment value: " ) + value );
			rc = xmlTextWriterEndComment( writer.get() );
			if ( rc < 0 )
				throw HXmlException( "Unable to write end comment." );
			}
		}
	rc = xmlTextWriterEndElement( writer.get() );
	if ( rc < 0 )
		throw HXmlException( HString( "Unable to write end element: " ) + str );
	return;
	M_EPILOG
	}

void HXml::create_root( yaal::hcore::HString const& name_, yaal::hcore::HString const& encoding_ )
	{
	M_PROLOG
	M_ASSERT( name_ );
	_encoding = ( !! encoding_ ) ? encoding_ : _defaultEncoding_;
	tree_t::node_t root = _dOM.create_new_root( HNode( this ) );
	(**root)._text = name_;
	return;
	M_EPILOG
	}

void HXml::clear( void )
	{
	M_PROLOG
	_encoding.clear();
	_dOM.clear();
	_xml->clear();
	return;
	M_EPILOG
	}

HXml::HNodeProxy HXml::get_root( void )
	{
	M_PROLOG
	return ( HNodeProxy( _dOM.get_root() ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_root( void ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( _dOM.get_root() ) );
	M_EPILOG
	}

HXml::const_entity_iterator HXml::entity_begin( void ) const
	{
	M_PROLOG
	return ( _entities.begin() );
	M_EPILOG
	}

HXml::const_entity_iterator HXml::entity_end( void ) const
	{
	M_PROLOG
	return ( _entities.end() );
	M_EPILOG
	}

HXml::entity_iterator HXml::entity_begin( void )
	{
	M_PROLOG
	return ( _entities.begin() );
	M_EPILOG
	}

HXml::entity_iterator HXml::entity_end( void )
	{
	M_PROLOG
	return ( _entities.end() );
	M_EPILOG
	}

HXml::HNodeProxy::HNodeProxy( void )
	: HConstNodeProxy()
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( void )
	: _node( NULL )
	{
	}

HXml::HNodeProxy::HNodeProxy( HXml::tree_t::node_t node_ )
	: HConstNodeProxy( node_ )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HXml::tree_t::const_node_t node_ )
	: _node( const_cast<HXml::tree_t::node_t>( node_ ) )
	{
	}

HXml::HNodeProxy::HNodeProxy( HNodeProxy const& other )
	: HConstNodeProxy( other )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HConstNodeProxy const& other )
	: _node( other._node )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HNodeProxy const& other )
	: _node( other._node )
	{
	}

HXml::HNodeProxy& HXml::HNodeProxy::operator = ( HNodeProxy const& other )
	{
	if ( &other != this )
		_node = other._node;
	return ( *this );
	}

HXml::HConstNodeProxy& HXml::HConstNodeProxy::operator = ( HConstNodeProxy const& other )
	{
	if ( &other != this )
		_node = other._node;
	return ( *this );
	}

int HXml::HConstNodeProxy::get_level( void ) const
	{
	M_PROLOG
	M_ASSERT( _node );
	return ( _node->get_level() );
	M_EPILOG
	}

HXml::HNode::TYPE::type_t HXml::HConstNodeProxy::get_type( void ) const
	{
	M_PROLOG
	M_ASSERT( _node );
	return ( (**_node)._type );
	M_EPILOG
	}

HString const& HXml::HConstNodeProxy::get_name( void ) const
	{
	M_PROLOG
	HXml::HNode::TYPE::type_t type( (**_node)._type );
	M_ASSERT( _node && ( ( type == HXml::HNode::TYPE::NODE ) || ( type == HXml::HNode::TYPE::ENTITY ) ) );
	return ( (**_node)._text );
	M_EPILOG
	}

HString const& HXml::HConstNodeProxy::get_value( void ) const
	{
	M_PROLOG
	HXml::HNode::TYPE::type_t type( (**_node)._type );
	M_ASSERT( _node && ( ( type == HXml::HNode::TYPE::CONTENT ) || ( type == HXml::HNode::TYPE::COMMENT ) || ( type == HXml::HNode::TYPE::ENTITY ) ) );
	HString const* val( NULL );
	if ( type == HXml::HNode::TYPE::ENTITY )
		{
		HXml::const_entity_iterator it( (**_node)._owner->_entities.find( (**_node)._text ) );
		M_ASSERT( it != (**_node)._owner->entity_end() );
		val = &it->second;
		}
	return ( type == HXml::HNode::TYPE::ENTITY ? *val : (**_node)._text );
	M_EPILOG
	}

HXml::HNode::properties_t& HXml::HNodeProxy::properties( void )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (**_node)._properties );
	M_EPILOG
	}

HXml::HNode::properties_t const& HXml::HConstNodeProxy::properties( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (**_node)._properties );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::add_node( HXml::HNode::TYPE::type_t const& type_, yaal::hcore::HString const& name_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator it = _node->add_node( (**_node)._owner );
	(**it)._text = name_;
	(**it)._type = type_;
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::add_node( yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator it = _node->add_node( (**_node)._owner );
	(**it)._text = name_;
	(**it)._type = HXml::HNode::TYPE::NODE;
	if ( !! value_ )
		{
		tree_t::iterator value = it->add_node( (**_node)._owner );
		(**value)._text = value_;
		(**value)._type = HXml::HNode::TYPE::CONTENT;
		}
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, HXml::HNode::TYPE::type_t const& type_, yaal::hcore::HString const& name_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->insert_node( it._iterator, HNode( (**_node)._owner ) );
	(**newIt)._text = name_;
	(**newIt)._type = type_;
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->insert_node( it._iterator, HNode( (**_node)._owner ) );
	(**newIt)._text = name_;
	(**newIt)._type = HXml::HNode::TYPE::NODE;;
	if ( !! value_ )
		{
		tree_t::iterator value = newIt->add_node( (**_node)._owner );
		(**value)._text = value_;
		(**value)._type = HXml::HNode::TYPE::CONTENT;
		}
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->move_node( it._iterator, node._node );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->move_node( node._node );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->copy_node( it._iterator, node._node );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->copy_node( node._node );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

bool HXml::HConstNodeProxy::has_childs( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( _node->has_childs() );
	M_EPILOG
	}

int long HXml::HConstNodeProxy::child_count( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( _node->child_count() );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::begin( void )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HIterator( this, _node->begin() ) );
	M_EPILOG
	}

HXml::HConstIterator HXml::HConstNodeProxy::begin( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HConstIterator( this, const_cast<HXml::tree_t::const_node_t>( _node )->begin() ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::end( void )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HIterator( this, _node->end() ) );
	M_EPILOG
	}

HXml::HConstIterator HXml::HConstNodeProxy::end( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( HXml::HConstIterator( this, const_cast<HXml::tree_t::const_node_t>( _node )->end() ) );
	M_EPILOG
	}

HXml::HNodeProxy::reverse_iterator HXml::HNodeProxy::rbegin( void )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( end() );
	M_EPILOG
	}

HXml::HConstNodeProxy::const_reverse_iterator HXml::HConstNodeProxy::rbegin( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( end() );
	M_EPILOG
	}

HXml::HNodeProxy::reverse_iterator HXml::HNodeProxy::rend( void )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( begin() );
	M_EPILOG
	}

HXml::HConstNodeProxy::const_reverse_iterator HXml::HConstNodeProxy::rend( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( begin() );
	M_EPILOG
	}

bool HXml::HNodeProxy::operator ! ( void ) const
	{
	return ( ! _node );
	}

bool HXml::HConstNodeProxy::operator ! ( void ) const
	{
	return ( ! _node );
	}

HXml::HIterator::HIterator( void )
	: base_type(), _owner( NULL ), _iterator()
	{
	}

HXml::HConstIterator::HConstIterator( void )
	: base_type(), _owner( NULL ), _iterator()
	{
	}

HXml::HIterator::HIterator( HXml::HNodeProxy const* owner_, HXml::tree_t::iterator const& it )
	: base_type(), _owner( owner_ ), _iterator( it )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstNodeProxy const* owner_, HXml::tree_t::const_iterator const& it )
	: base_type(), _owner( owner_ ), _iterator( it )
	{
	}

HXml::HIterator::HIterator( HXml::HIterator const& other )
	: base_type(), _owner( other._owner ), _iterator( other._iterator )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstIterator const& other )
	: base_type(), _owner( other._owner ), _iterator( other._iterator )
	{
	}

HXml::HIterator& HXml::HIterator::operator = ( HXml::HIterator const& other )
	{
	if ( &other != this )
		{
		_owner = other._owner;
		_iterator = other._iterator;
		}
	return ( *this );
	}

HXml::HConstIterator& HXml::HConstIterator::operator = ( HXml::HConstIterator const& other )
	{
	if ( &other != this )
		{
		_owner = other._owner;
		_iterator = other._iterator;
		}
	return ( *this );
	}

bool HXml::HIterator::operator == ( HXml::HIterator const& other ) const
	{
	return ( _iterator == other._iterator );
	}

bool HXml::HConstIterator::operator == ( HXml::HConstIterator const& other ) const
	{
	return ( _iterator == other._iterator );
	}

bool HXml::HIterator::operator != ( HXml::HIterator const& other ) const
	{
	return ( _iterator != other._iterator );
	}

bool HXml::HConstIterator::operator != ( HXml::HConstIterator const& other ) const
	{
	return ( _iterator != other._iterator );
	}

HXml::HNodeProxy HXml::HIterator::operator* ( void )
	{
	M_PROLOG
	M_ASSERT( _owner );
	M_ASSERT( _iterator != _owner->_node->end() );
	return ( HXml::HNodeProxy( &*_iterator ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::HConstIterator::operator* ( void ) const
	{
	M_PROLOG
	M_ASSERT( _owner );
	M_ASSERT( _iterator != const_cast<HXml::tree_t::const_node_t>( _owner->_node )->end() );
	return ( HXml::HConstNodeProxy( &*_iterator ) );
	M_EPILOG
	}

HXml::const_xml_element_t HXml::get_element_by_id( const_xml_element_t const& node, yaal::hcore::HString const& id ) const
	{
	M_PROLOG
	const_xml_element_t result = NULL;
	HXml::HNode::properties_t::const_iterator idIt = (*node)->_properties.find( "id" );
	if ( ( idIt != (*node)->_properties.end() ) && ( idIt->second == id ) )
		result = node;
	for ( tree_t::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it )
		result = get_element_by_id( &*it, id );
	return ( result );
	M_EPILOG
	}

HXml::HNodeProxy HXml::get_element_by_id( yaal::hcore::HString const& id )
	{
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_id( _dOM.get_root(), id ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_id( yaal::hcore::HString const& id ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_id( _dOM.get_root(), id ) ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::remove_node( HXml::HIterator it )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->remove_node( it._iterator );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::const_xml_element_t HXml::get_element_by_path( const_xml_element_t const& node,
		HString const& path, int part ) const
	{
	M_PROLOG
	const_xml_element_t result = NULL;
	HTokenizer t( path, "/" );
	HString name = t[ part ];
	if ( ! name.is_empty() )
		{
		if ( (**node)._text == name )
			{
			for ( tree_t::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it )
				if ( (**it)._type == HXml::HNode::TYPE::NODE )
					result = get_element_by_path( &*it, path, part + 1 );
			}
		}
	else
		{
		if ( node->get_level() )
			result = node->get_parent();
		else
			result = node;
		}
	return ( result );
	M_EPILOG
	}

HXml::HNodeProxy HXml::get_element_by_path( yaal::hcore::HString const& path )
	{
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_path( _dOM.get_root(), path, 1 ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_path( yaal::hcore::HString const& path ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_path( _dOM.get_root(), path, 1 ) ) );
	M_EPILOG
	}

char const* HXml::error_message( int code ) const
	{
	M_PROLOG
	char const* msg = "Unknow error code";
	switch ( code )
		{
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

namespace xml
{

value_t try_node_val( HXml::HConstNodeProxy const& node )
	{
	M_PROLOG
	HXml::HConstIterator it = node.begin();
	value_t value;
	if ( it != node.end() )
		{
		if ( (*it).get_type() == HXml::HNode::TYPE::CONTENT )
			value = (*it).get_value();
		}
	return ( value );
	M_EPILOG
	}

value_t try_node_val( HXml::HConstIterator const& it )
	{
	M_PROLOG
	return ( try_node_val( *it ) );
	M_EPILOG
	}

value_t try_attr_val( HXml::HConstNodeProxy const& node, char const* const name )
	{
	M_PROLOG
	HXml::HNode::properties_t const& props = node.properties();
	HXml::HNode::properties_t::const_iterator prop = props.find( name );
	value_t value;
	if ( prop != props.end() )
		value = prop->second;
	return ( value );
	M_EPILOG
	}

value_t try_attr_val( HXml::HConstIterator const& it, char const* const name )
	{
	M_PROLOG
	return ( try_attr_val( *it, name ) );
	M_EPILOG
	}

HString const& node_val( HXml::HConstNodeProxy const& node_ )
	{
	M_PROLOG
	value_t nodeValue( try_node_val( node_ ) );
	M_ENSURE( nodeValue );
	return ( *nodeValue );
	M_EPILOG
	}

HString const& node_val( HXml::HConstIterator const& it_ )
	{
	M_PROLOG
	value_t nodeValue( try_node_val( it_ ) );
	M_ENSURE( nodeValue );
	return ( *nodeValue );
	M_EPILOG
	}

HString const& attr_val( HXml::HConstNodeProxy const& node_, char const* const name_ )
	{
	M_PROLOG
	value_t attrValue( try_attr_val( node_, name_ ) );
	M_ENSURE_EX( attrValue, name_ );
	return ( *attrValue );
	M_EPILOG
	}

HString const& attr_val( HXml::HConstIterator const& it_, char const* const name_ )
	{
	M_PROLOG
	value_t attrValue( try_attr_val( it_, name_ ) );
	M_ENSURE_EX( attrValue, name_ );
	return ( *attrValue );
	M_EPILOG
	}

}

}

}


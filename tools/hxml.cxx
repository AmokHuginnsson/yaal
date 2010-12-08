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
#include <libxml/xmlwriter.h>
#include <libxml/xinclude.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hxml.hxx"

#include "hcore/htokenizer.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hsocket.hxx"
#include "tools.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace
{

/* char schema_err[] = "bad xml schema"; */
typedef HResource<xmlDoc, void (*)( xmlDocPtr )> doc_resource_t;
typedef HResource<xmlTextWriter, void (*)( xmlTextWriterPtr )> writer_resource_t;
typedef HResource<xmlCharEncodingHandler, int (*)( xmlCharEncodingHandler* )> encoder_resource_t;
typedef HResource<xsltStylesheet, void (*)( xsltStylesheetPtr )> style_resource_t;
typedef HResource<xmlXPathContext, void (*)( xmlXPathContextPtr )> xpath_context_resource_t;
typedef HResource<xmlXPathObject, void (*)( xmlXPathObjectPtr )> xpath_object_resource_t;
typedef HResource<xmlOutputBuffer, int (*)( xmlOutputBufferPtr )> outputbuffer_resource_t;
typedef HPointer<encoder_resource_t> encoder_resource_ptr_t;

char const* const FULL_TREE = "//*";

HString get_stream_id( HStreamInterface* stream )
	{
	HFile* f = dynamic_cast<HFile*>( stream );
	HSocket* s = dynamic_cast<HSocket*>( stream );
	return ( f ? f->get_path() : ( s ? s->get_host_name() : "anonymous stream" ) );
	}

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
//	xmlSubstituteEntitiesDefault( 1 );
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
	encoder_resource_ptr_t _encoder;
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
		_encoder = encoder_resource_ptr_t(
				new encoder_resource_t( encoder, xmlCharEncCloseFunc ) );
		_iconvToExternal = (*_encoder).get()->iconv_in;
		_iconvToInternal = (*_encoder).get()->iconv_out;
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
		_encoder = encoder_resource_ptr_t(
				new encoder_resource_t( encoder, xmlCharEncCloseFunc ) );
		_iconvToExternal = (*_encoder).get()->iconv_in;
		_iconvToInternal = (*_encoder).get()->iconv_out;
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
	/*{*/
	friend class HXml;
	doc_resource_t     _doc;
	style_resource_t   _style;
	mutable xpath_context_resource_t _xPathContext;
	mutable xpath_object_resource_t _xPathObject;
	xmlNodeSetPtr      _nodeSet;
	/*}*/
protected:
	/*{*/
	HXmlData( void );
	virtual ~HXmlData( void );
	HXmlData( HXmlData const& ) __attribute__(( __noreturn__ ));
	HXmlData& operator = ( HXmlData const& ) __attribute__(( __noreturn__ ));
	void clear( void ) const;
	/*}*/
	};

HXmlData::HXmlData( void )
	: _doc(), _style(), _xPathContext(), _xPathObject(),
	_nodeSet( NULL )
	{
	M_PROLOG
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
	_varTmpBuffer(), _encoding( _defaultEncoding_ ), _xml( NULL ),
	_entities(), _dOM()
	{
	M_PROLOG
	_xml = new ( std::nothrow ) HXmlData();
	M_ENSURE( _xml );
	M_EPILOG
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	if ( _xml )
		delete _xml;
	_xml = NULL;
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
			swap( _xml->_xPathObject, obj );
			setSize = _xml->_nodeSet ? _xml->_nodeSet->nodeNr : 0;
			}
		}
	swap( _xml->_xPathContext, ctx );
	return ( setSize );
	M_EPILOG
	}

void HXml::init( yaal::hcore::HStreamInterface& stream )
	{
	M_PROLOG
	int savedErrno = errno;
	HString error;
	HXmlParserG::get_instance();
	errno = 0;
	HString streamId = get_stream_id( &stream );
	int const LOW_LEVEL_PARSING_OPTIONS( XML_PARSE_NOENT | XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_XINCLUDE | XML_PARSE_NONET | XML_PARSE_NSCLEAN );
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
	swap( _xml->_doc, doc );
	return;
	M_EPILOG
	}

void HXml::init( yaal::hcore::HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	init( *stream );
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
				node = node->next; /* FIXME add DTD handling later (keeping track of entities) */
				}
			continue;
			case ( XML_ELEMENT_NODE ):
				{
				if ( node_ )
					node_ = &*node_->add_node();
				else
					node_ = _dOM.create_new_root();
				(**node_)._text = reinterpret_cast<char const*>( node->name );
				if ( node->properties )
					{
					xmlAttrPtr attribute = node->properties;
					while ( attribute )
						{
						char const* name = reinterpret_cast<char const*>( attribute->name );
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
				entities_t::const_iterator it( _entities.find( node->name ) );
				M_ENSURE_EX( it != _entities.end(), HString( "entity not found:" ) + node->name );
				}
			break;
			case ( XML_TEXT_NODE ): if ( node->content )
				{
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				if ( ! ( parser_ & PARSER::STRIP_EMPTY ) || ( _varTmpBuffer.find_other_than( _whiteSpace_ ) >= 0 ) )
					node_->add_node( HNode( HNode::TYPE::CONTENT, _varTmpBuffer ) );
				}
			break;
			case ( XML_COMMENT_NODE ): if ( ! ( parser_ & PARSER::STRIP_COMMENT ) && node->content )
				{
				_varTmpBuffer = convert( reinterpret_cast<char*>( node->content ) );
				node_->add_node( HNode( HNode::TYPE::COMMENT, _varTmpBuffer ) );
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
	get_node_set_by_path( xPath_ );
	_dOM.clear();
	int ctr = 0;
	while ( xPath_[ ctr ] )
		ctr ++;
	ctr --;
	M_ASSERT( ctr >= 0 );
	if ( _xml->_nodeSet )
		{
		if ( xPath_ != FULL_TREE )
			{
			tree_t::node_t root = _dOM.create_new_root();
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

void HXml::load( yaal::hcore::HStreamInterface& stream )
	{
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	M_ENSURE( stream.is_valid() );
	init( stream );
	parse();
	return;
	M_EPILOG
	}

void HXml::load( yaal::hcore::HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	return ( load( *stream ) );
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface& stream ) const
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
		swap( doc, dummy );
		int rc = ::xmlTextWriterStartDocument( writer.get(), NULL, _encoding.raw(), "yes" );
		if ( rc < 0 )
			throw HXmlException( HString( "Unable to start document with encoding: " ) + _encoding );
		rc = xmlTextWriterSetIndent( writer.get(), 1 );
		if ( rc < 0 )
			throw HXmlException( "Unable to enable indenting." );
		static char const* const INDENTION_STRING = "\t";
		rc = xmlTextWriterSetIndentString( writer.get(), reinterpret_cast<xmlChar const* const>( INDENTION_STRING ) );
		if ( rc < 0 )
			throw HXmlException( "Cannot set indent string." );
		if ( ! (*_convert) )
			(*_convert).init( _encoding );
		dump_node( &writer, get_root() );
		rc = xmlTextWriterEndDocument( writer.get() );
		if ( rc < 0 )
			throw HXmlException( "Unable to end document." );
		}
	_xml->clear();
	if ( _xml->_doc.get() )
		xmlFreeNode( xmlDocSetRootElement( _xml->_doc.get(), xmlDocGetRootElement( doc.get() ) ) );
	else
		swap( _xml->_doc, doc );
	M_ASSERT( _xml->_doc.get() );
	if ( _xml->_style.get() )
		{
		outputbuffer_resource_t obuf( ::xmlOutputBufferCreateIO( writer_callback,
							NULL, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
				xmlOutputBufferClose );
		M_ENSURE( ::xsltSaveResultTo( obuf.get(),
					_xml->_doc.get(), _xml->_style.get() ) != -1 );
		}
	else
		M_ENSURE( ::xmlSaveFileTo( ::xmlOutputBufferCreateIO( writer_callback,
						NULL, &stream, ::xmlFindCharEncodingHandler( _encoding.raw() ) ),
					_xml->_doc.get(), _encoding.raw() ) != -1 );
	return;
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface::ptr_t stream ) const
	{
	M_PROLOG
	M_ENSURE( stream->is_valid() );
	save( *stream );
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
			rc = xmlTextWriterWriteString( writer.get(),
					reinterpret_cast<xmlChar const*>( convert( value, TO_EXTERNAL ).raw() ) );
			if ( rc < 0 )
				throw HXmlException( HString( "Unable to write a node value: " ) + value );
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
	tree_t::node_t root = _dOM.create_new_root();
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
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	return ( (**_node)._text );
	M_EPILOG
	}

HString const& HXml::HConstNodeProxy::get_value( void ) const
	{
	M_PROLOG
	M_ASSERT( _node && ( ( (**_node)._type == HXml::HNode::TYPE::CONTENT ) || ( (**_node)._type == HXml::HNode::TYPE::COMMENT ) ) );
	return ( (**_node)._text );
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
	tree_t::iterator it = _node->add_node();
	(**it)._text = name_;
	(**it)._type = type_;
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::add_node( yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator it = _node->add_node();
	(**it)._text = name_;
	(**it)._type = HXml::HNode::TYPE::NODE;
	if ( !! value_ )
		{
		tree_t::iterator value = it->add_node();
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
	tree_t::iterator newIt = _node->insert_node( it._iterator, HNode() );
	(**newIt)._text = name_;
	(**newIt)._type = type_;
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, yaal::hcore::HString const& name_, yaal::hcore::HString const& value_ )
	{
	M_PROLOG
	M_ASSERT( _node && ( (**_node)._type == HXml::HNode::TYPE::NODE ) );
	tree_t::iterator newIt = _node->insert_node( it._iterator, HNode() );
	(**newIt)._text = name_;
	(**newIt)._type = HXml::HNode::TYPE::NODE;;
	if ( !! value_ )
		{
		tree_t::iterator value = newIt->add_node();
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
	: iterator_interface<HXml::HNodeProxy>(), _owner( NULL ), _iterator()
	{
	}

HXml::HConstIterator::HConstIterator( void )
	: iterator_interface<HXml::HConstNodeProxy>(), _owner( NULL ), _iterator()
	{
	}

HXml::HIterator::HIterator( HXml::HNodeProxy const* owner_, HXml::tree_t::iterator const& it )
	: iterator_interface<HXml::HNodeProxy>(), _owner( owner_ ), _iterator( it )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstNodeProxy const* owner_, HXml::tree_t::const_iterator const& it )
	: iterator_interface<HXml::HConstNodeProxy>(), _owner( owner_ ), _iterator( it )
	{
	}

HXml::HIterator::HIterator( HXml::HIterator const& other )
	: iterator_interface<HXml::HNodeProxy>(), _owner( other._owner ), _iterator( other._iterator )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstIterator const& other )
	: iterator_interface<HXml::HConstNodeProxy>(), _owner( other._owner ), _iterator( other._iterator )
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
	swap( _xml->_doc, doc );
	swap( _xml->_style, style );
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


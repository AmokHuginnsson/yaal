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
#include "hxml.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hsocket.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace
{
/* char schema_err[] = "bad xml schema"; */
char const* const D_DEFAULT_ENCODING = "iso-8859-2";
typedef HResource<xmlDocPtr, __decltype( &xmlFreeDoc )> doc_resource_t;
typedef HResource<xmlTextWriterPtr, __decltype( &xmlFreeTextWriter )> writer_resource_t;
typedef HResource<xmlCharEncodingHandlerPtr, __decltype( &xmlCharEncCloseFunc )> encoder_resource_t;
typedef HResource<xsltStylesheetPtr, __decltype( &xsltFreeStylesheet )> style_resource_t;
typedef HResource<xmlXPathContextPtr, __decltype( &xmlXPathFreeContext )> xpath_context_resource_t;
typedef HResource<xmlXPathObjectPtr, __decltype( &xmlXPathFreeObject )> xpath_object_resource_t;
typedef HResource<xmlOutputBufferPtr, __decltype( &xmlOutputBufferClose )> outputbuffer_resource_t;
typedef HPointer<encoder_resource_t> encoder_resource_ptr_t;

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

class HXmlParserG : public HSingletonInterface
	{
	HXmlParserG( void );
	~HXmlParserG( void );
	friend class HSingleton<HXmlParserG>;
	friend class HDestructor<HXmlParserG>;
	};

typedef HSingleton<HXmlParserG> HXmlParserGlobal;

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

class HXsltParserG : public HSingletonInterface
	{
	HXsltParserG( void );
	~HXsltParserG( void );
	friend class HSingleton<HXsltParserG>;
	friend class HDestructor<HXsltParserG>;
	};

typedef HSingleton<HXsltParserG> HXsltParserGlobal;

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
	encoder_resource_ptr_t f_oEncoder;
	iconv_t f_xIconvToExternal;
	iconv_t f_xIconvToInternal;
	OConvert( void ) 
		: f_oEncoder(),
		f_xIconvToExternal( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal( static_cast<iconv_t>( 0 ) ) { }
	OConvert( OConvert const& a_roConvert )
		: f_oEncoder(),
		f_xIconvToExternal( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal( static_cast<iconv_t>( 0 ) )
		{
		operator = ( a_roConvert );
		}
	OConvert& operator = ( OConvert const& a_roConvert )
		{
		if ( &a_roConvert != this )
			{
			f_oEncoder = a_roConvert.f_oEncoder;
			f_xIconvToExternal = a_roConvert.f_xIconvToExternal;
			f_xIconvToInternal = a_roConvert.f_xIconvToInternal;
			}
		return ( *this );
		}
	void init( HString const& a_oEncoding )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = ::xmlFindCharEncodingHandler( a_oEncoding.raw() );
		if ( ! encoder )
			M_THROW( _( "cannot enable internal convertion" ), errno );
		f_oEncoder = encoder_resource_ptr_t(
				new encoder_resource_t( encoder, xmlCharEncCloseFunc ) );
		f_xIconvToExternal = (*f_oEncoder).get()->iconv_in;
		f_xIconvToInternal = (*f_oEncoder).get()->iconv_out;
		M_EPILOG
		}
	void init( yaal::hcore::HString const& a_oEncoding, xmlNodePtr a_psRoot, yaal::hcore::HString const& a_oFileName )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = NULL;
		if ( !! a_oEncoding )
			encoder = ::xmlFindCharEncodingHandler( a_oEncoding.raw() );
		else
			log( LOG_TYPE::D_WARNING ) << _( "HXml::WARNING: no encoding declared in `" )
				<< a_oFileName << "'." << endl;
		if ( ! encoder )
			{
			log( LOG_TYPE::D_WARNING ) << _( "HXml::WARNING: char encoding handler not found" ) << endl;
			xmlCharEncoding l_xEncoding = ::xmlDetectCharEncoding( a_psRoot->name,
					xmlStrlen( a_psRoot->name ) );
			if ( ! l_xEncoding )
				M_THROW( _( "cannot detect character encoding" ), errno );
			encoder = xmlGetCharEncodingHandler( l_xEncoding );
			}
		if ( ! encoder )
			M_THROW( _( "cannot enable internal convertion" ), errno );
		f_oEncoder = encoder_resource_ptr_t(
				new encoder_resource_t( encoder, xmlCharEncCloseFunc ) );
		f_xIconvToExternal = (*f_oEncoder).get()->iconv_in;
		f_xIconvToInternal = (*f_oEncoder).get()->iconv_out;
		M_EPILOG
		}
	bool operator ! ( void ) const
		{
		return ( ! ( f_xIconvToExternal && f_xIconvToInternal ) );
		}
	};

class HXmlData
	{
private:
	/*{*/
	friend class HXml;
	doc_resource_t     f_oDoc;
	style_resource_t   f_oStyle;
	mutable xpath_context_resource_t f_oXPathContext;
	mutable xpath_object_resource_t f_oXPathObject;
	xmlNodeSetPtr      f_psNodeSet;
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

HXmlData::HXmlData( void ) : f_oDoc( NULL, xmlFreeDoc ),
	f_oStyle( NULL, xsltFreeStylesheet ),
	f_oXPathContext( NULL, xmlXPathFreeContext ),
	f_oXPathObject( NULL, xmlXPathFreeObject ),
	f_psNodeSet( NULL )
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
	xpath_context_resource_t ctx( NULL, xmlXPathFreeContext );
	xpath_object_resource_t obj( NULL, xmlXPathFreeObject );
	swap( f_oXPathObject, obj );
	swap( f_oXPathContext, ctx );
	return;
	M_EPILOG
	}

HXml::HXml( void )
	: f_oConvert( new HXml::OConvert ), f_oConvertedString(),
	f_oVarTmpBuffer(), f_oEncoding( D_DEFAULT_ENCODING ), f_poXml( NULL ), f_oDOM()
	{
	M_PROLOG
	f_poXml = new ( std::nothrow ) HXmlData();
	M_ENSURE( f_poXml );
	M_EPILOG
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	if ( f_poXml )
		delete f_poXml;
	f_poXml = NULL;
	return;
	M_EPILOG
	}

#ifdef HAVE_ICONV_INPUT_CONST
#	define D_YAAL_TOOLS_HXML_ICONV_CONST const
#else /* HAVE_ICONV_INPUT_CONST */
#	define D_YAAL_TOOLS_HXML_ICONV_CONST /**/
#endif /* not HAVE_ICONV_INPUT_CONST */

HString const& HXml::convert( HString const& a_oData, way_t a_eWay ) const
	{
	M_PROLOG
	char D_YAAL_TOOLS_HXML_ICONV_CONST* source = const_cast<char D_YAAL_TOOLS_HXML_ICONV_CONST*>( a_oData.raw() );
	iconv_t l_xCD = static_cast<iconv_t>( 0 );
	switch ( a_eWay )
		{
		case ( D_TO_EXTERNAL ): { l_xCD = ( *f_oConvert ).f_xIconvToExternal; break; }
		case ( D_TO_INTERNAL ): { l_xCD = ( *f_oConvert ).f_xIconvToInternal; break; }
		default:
			{
			M_ASSERT( ! _( "unknown conversion way" ) );
			}
		}
	size_t l_uiSizeOut = 0, l_uiSizeIn = ::strlen( source );
	/* The longest single character in any encoding is 6 bytes long. */
	size_t const D_ICONV_OUTPUT_BUFFER_LENGTH = 8;
	/* Additional character for nil terminator. */
	char l_pcOutput[ D_ICONV_OUTPUT_BUFFER_LENGTH + 1 ];
	f_oConvertedString = "";
	do
		{
		::memset( l_pcOutput, 0, D_ICONV_OUTPUT_BUFFER_LENGTH + 1 );
		l_uiSizeOut = D_ICONV_OUTPUT_BUFFER_LENGTH;
		char* l_pcOut = l_pcOutput;
		M_ENSURE( ( ::iconv( l_xCD, &source, &l_uiSizeIn, &l_pcOut,
						&l_uiSizeOut ) != static_cast<size_t>( -1 ) )
				|| ( errno == E2BIG ) );
		f_oConvertedString += l_pcOutput;
		}
	while ( l_uiSizeIn );
	return ( f_oConvertedString );
	M_EPILOG
	}

#undef D_YAAL_TOOLS_HXML_ICONV_CONST

int HXml::get_node_set_by_path( yaal::hcore::HString const& a_oPath )
	{
	M_PROLOG
	f_oVarTmpBuffer = a_oPath;
	int long l_iLength = f_oVarTmpBuffer.get_length() - 1;
	xpath_context_resource_t ctx( xmlXPathNewContext( f_poXml->f_oDoc.get() ), xmlXPathFreeContext );
	int setSize = 0;
	f_poXml->f_psNodeSet = NULL;
	if ( ctx.get() )
		{
		xmlXPathObjectPtr objPtr = NULL;
		while ( ! f_oVarTmpBuffer.is_empty() )
			{
			objPtr = xmlXPathEvalExpression(
					reinterpret_cast<xmlChar const*>( f_oVarTmpBuffer.raw() ),
					ctx.get() );
			if ( objPtr )
				break;
			f_oVarTmpBuffer.set_at( l_iLength --, 0 );
			}
		xpath_object_resource_t obj( objPtr, xmlXPathFreeObject );
		if ( obj.get() )
			{
			f_oVarTmpBuffer = a_oPath;
			f_poXml->f_psNodeSet = obj.get()->nodesetval;
			swap( f_poXml->f_oXPathObject, obj );
			setSize = f_poXml->f_psNodeSet ? f_poXml->f_psNodeSet->nodeNr : 0;
			}
		}
	swap( f_poXml->f_oXPathContext, ctx );
	return ( setSize );
	M_EPILOG
	}

void HXml::init( yaal::hcore::HStreamInterface& stream )
	{
	M_PROLOG
	int l_iSavedErrno = errno;
	HString l_oError;
	HXmlParserGlobal::get_instance();
	errno = 0;
	HString streamId = get_stream_id( &stream );
	doc_resource_t doc( ::xmlReadIO( reader_callback, NULL, &stream, streamId.raw(), NULL,
				XML_PARSE_NOENT | XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_XINCLUDE | XML_PARSE_NONET | XML_PARSE_NSCLEAN ),
			xmlFreeDoc );
	if ( errno )
		{
		log( LOG_TYPE::D_WARNING ) << error_message( errno ) << ": " << streamId;
		log << ", code: " << errno << '.' << endl;
		}
	errno = l_iSavedErrno;
	if ( ! doc.get() )
		{
		l_oError.format( _( "cannot parse `%s'" ), streamId.raw() );
		throw HXmlException( l_oError );
		}
	if ( xmlXIncludeProcess( doc.get() ) < 0 )
		throw HXmlException( "processing XInclude failed" );
	xmlNodePtr root = xmlDocGetRootElement( doc.get() );
	if ( ! root )
		M_THROW ( _ ( "empty doc" ), errno );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", root->name );
#endif /* __DEBUGGER_BABUNI__ */
	(*f_oConvert).init( reinterpret_cast<char const *>( doc.get()->encoding ),
			root, streamId );
	swap( f_poXml->f_oDoc, doc );
	return;
	M_EPILOG
	}

void HXml::init( yaal::hcore::HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	init( *stream );
	return;
	M_EPILOG
	}

void HXml::parse( xml_node_ptr_t a_pvData, tree_t::node_t a_rsNode, bool a_bStripEmpty )
	{
	M_PROLOG
	xmlNodePtr l_psNode = static_cast<xmlNodePtr>( a_pvData );
	while ( l_psNode )
		{
		switch ( l_psNode->type )
			{
			case ( XML_DOCUMENT_NODE ):
				l_psNode = l_psNode->children;
			continue;
			case ( XML_DTD_NODE ):
				{
				xmlNodePtr child = l_psNode->children;
				while ( child )
					{
					parse( child, a_rsNode, a_bStripEmpty );
					child = child->next;
					}
				l_psNode = l_psNode->next; /* FIXME add DTD handling later (keeping track of entities) */
				}
			continue;
			case ( XML_ELEMENT_NODE ):
				{
				if ( a_rsNode )
					a_rsNode = &*a_rsNode->add_node();
				else
					a_rsNode = f_oDOM.create_new_root();
				(**a_rsNode).f_oText = reinterpret_cast<char const*>( l_psNode->name );
				if ( l_psNode->properties )
					{
					xmlAttrPtr l_psAttribute = l_psNode->properties;
					while ( l_psAttribute )
						{
						char const* l_pcName = reinterpret_cast<char const*>( l_psAttribute->name );
						if ( l_pcName )
							{
							if ( l_psAttribute->children )
								(**a_rsNode).f_oProperties[ l_pcName ] = l_psAttribute->children->content ? convert(
										reinterpret_cast<char*>( l_psAttribute->children->content ) ) : "";
							}
						l_psAttribute = l_psAttribute->next;
						}
					}
				xmlNodePtr child = l_psNode->children;
				while ( child )
					{
					parse( child, a_rsNode, a_bStripEmpty );
					child = child->next;
					}
				}
			break;
			case ( XML_ENTITY_REF_NODE ):
			case ( XML_TEXT_NODE ): if ( l_psNode->content )
				{
				f_oVarTmpBuffer = convert( reinterpret_cast<char*>( l_psNode->content ) );
				if ( ! a_bStripEmpty || ( f_oVarTmpBuffer.find_other_than( n_pcWhiteSpace ) >= 0 ) )
					a_rsNode->add_node( HNode( HNode::TYPE::D_CONTENT, f_oVarTmpBuffer ) );
				}
			break;
			default:
				log_trace << "unsupported type: " << static_cast<int>( l_psNode->type ) << endl;
			break;
			}
		l_psNode = NULL;
		}
	return;
	M_EPILOG
	}

void HXml::parse( HString a_oXPath, bool a_bStripEmpty )
	{
	M_PROLOG
	char const* const D_FULL_TREE = "//*";
	if ( ! a_oXPath || ! a_oXPath[ 0 ] )
		a_oXPath = D_FULL_TREE; /* scan full tree */
	get_node_set_by_path( a_oXPath );
	f_oDOM.clear();
	int l_iCtr = 0;
	while ( a_oXPath[ l_iCtr ] )
		l_iCtr ++;
	l_iCtr --;
	M_ASSERT ( l_iCtr >= 0 );
	if ( f_poXml->f_psNodeSet )
		{
		if ( a_oXPath != D_FULL_TREE )
			{
			tree_t::node_t root = f_oDOM.create_new_root();
			(**root).f_oText = "xpath_result_set";
			for ( l_iCtr = 0; l_iCtr < f_poXml->f_psNodeSet->nodeNr; ++ l_iCtr )
				parse( f_poXml->f_psNodeSet->nodeTab[ l_iCtr ],
						root, a_bStripEmpty );
			}
		else
			parse( xmlDocGetRootElement( f_poXml->f_oDoc.get() ), NULL, a_bStripEmpty );
		}
	M_EPILOG
	}

void HXml::load( yaal::hcore::HStreamInterface& stream )
	{
	M_PROLOG
	init( stream );
	parse( HString(), true );
	return;
	M_EPILOG
	}

void HXml::load( yaal::hcore::HStreamInterface::ptr_t stream )
	{
	M_PROLOG
	return ( load( *stream ) );
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface& stream ) const
	{
	M_PROLOG
	doc_resource_t doc( NULL, xmlFreeDoc );
	/* flush writer to DOM. */
	M_ENSURE( !! get_root() );
		{
		xmlDocPtr pDoc = NULL;
		writer_resource_t writer( xmlNewTextWriterDoc( &pDoc, 0 ), xmlFreeTextWriter );
		if ( ! writer.get() )
			throw HXmlException( _( "Cannot create the xml DOC writer." ) );
		doc_resource_t dummy( pDoc, xmlFreeDoc );
		swap( doc, dummy );
		int rc = ::xmlTextWriterStartDocument( writer.get(), NULL, f_oEncoding.raw(), "yes" );
		if ( rc < 0 )
			throw HXmlException( HString( "Unable to start document with encoding: " ) + f_oEncoding );
		rc = xmlTextWriterSetIndent( writer.get(), 1 );
		if ( rc < 0 )
			throw HXmlException( "Unable to enable indenting." );
		static char const* const D_INDENTION_STRING = "\t";
		rc = xmlTextWriterSetIndentString( writer.get(), reinterpret_cast<xmlChar const* const>( D_INDENTION_STRING ) );
		if ( rc < 0 )
			throw HXmlException( "Cannot set indent string." );
		if ( ! (*f_oConvert) )
			(*f_oConvert).init( f_oEncoding );
		dump_node( &writer, get_root() );
		rc = xmlTextWriterEndDocument( writer.get() );
		if ( rc < 0 )
			throw HXmlException( "Unable to end document." );
		}
	f_poXml->clear();
	if ( f_poXml->f_oDoc.get() )
		xmlFreeNode( xmlDocSetRootElement( f_poXml->f_oDoc.get(), xmlDocGetRootElement( doc.get() ) ) );
	else
		swap( f_poXml->f_oDoc, doc );
	M_ASSERT( f_poXml->f_oDoc.get() );
	if ( f_poXml->f_oStyle.get() )
		{
		outputbuffer_resource_t obuf( ::xmlOutputBufferCreateIO( writer_callback,
							NULL, &stream, ::xmlFindCharEncodingHandler( f_oEncoding.raw() ) ),
				xmlOutputBufferClose );
		M_ENSURE( ::xsltSaveResultTo( obuf.get(),
					f_poXml->f_oDoc.get(), f_poXml->f_oStyle.get() ) != -1 );
		}
	else
		M_ENSURE( ::xmlSaveFileTo( ::xmlOutputBufferCreateIO( writer_callback,
						NULL, &stream, ::xmlFindCharEncodingHandler( f_oEncoding.raw() ) ),
					f_poXml->f_oDoc.get(), f_oEncoding.raw() ) != -1 );
	return;
	M_EPILOG
	}

void HXml::save( yaal::hcore::HStreamInterface::ptr_t stream ) const
	{
	M_PROLOG
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

void HXml::dump_node( void* writer_p, HConstNodeProxy const& node ) const
	{
	M_PROLOG
	writer_resource_t& writer = *static_cast<writer_resource_t*>( writer_p );
	HString const& str = node.get_name();
	int rc = xmlTextWriterStartElement( writer.get(), reinterpret_cast<xmlChar const* const>( str.raw() ) );
	if ( rc < 0 )
		throw HXmlException( HString( "Unable to write start element: " ) + str );
	HNode::properties_t const& prop = node.properties();
	for ( HNode::properties_t::const_iterator it = prop.begin(); it != prop.end(); ++ it )
		{
		HString const& pname = it->first;
		HString const& pvalue = it->second;
		rc = xmlTextWriterWriteAttribute( writer.get(),
				reinterpret_cast<xmlChar const* const>( pname.raw() ),
				reinterpret_cast<xmlChar const* const>( convert( pvalue.raw(), D_TO_EXTERNAL ).raw() ) );
		if ( rc < 0 )
			throw HXmlException( HString( "Unable to write a property: " ) + str + ", with value: " + pvalue );
		}
	for ( HXml::HConstIterator it = node.begin(); it != node.end(); ++ it )
		{
		if ( (*it).get_type() == HXml::HNode::TYPE::D_NODE )
			dump_node( writer_p, *it );
		else
			{
			HString const& value = (*it).get_value();
			rc = xmlTextWriterWriteString( writer.get(),
					reinterpret_cast<xmlChar const*>( convert( value, D_TO_EXTERNAL ).raw() ) );
			if ( rc < 0 )
				throw HXmlException( HString( "Unable to write a node value: " ) + value );
			}
		}
	rc = xmlTextWriterEndElement( writer.get() );
	if ( rc < 0 )
		throw HXmlException( HString( "Unable to write end element: " ) + str );
	return;
	M_EPILOG
	}

void HXml::create_root( yaal::hcore::HString const& a_oName, yaal::hcore::HString const& a_oEncoding )
	{
	M_PROLOG
	M_ASSERT( a_oName );
	f_oEncoding = ( !! a_oEncoding ) ? a_oEncoding : D_DEFAULT_ENCODING;
	tree_t::node_t root = f_oDOM.create_new_root();
	(**root).f_oText = a_oName;
	return;
	M_EPILOG
	}

void HXml::clear( void )
	{
	M_PROLOG
	f_oEncoding.clear();
	f_oDOM.clear();
	f_poXml->clear();
	return;
	M_EPILOG
	}

HXml::HNodeProxy HXml::get_root( void )
	{
	M_PROLOG
	return ( HNodeProxy( f_oDOM.get_root() ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_root( void ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( f_oDOM.get_root() ) );
	M_EPILOG
	}

HXml::HNodeProxy::HNodeProxy( void )
	: f_poNode( NULL )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( void )
	: f_poNode( NULL )
	{
	}

HXml::HNodeProxy::HNodeProxy( HXml::tree_t::node_t a_poNode )
	: f_poNode( a_poNode )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HXml::tree_t::const_node_t a_poNode )
	: f_poNode( a_poNode )
	{
	}

HXml::HNodeProxy::HNodeProxy( HNodeProxy const& other )
	: f_poNode( other.f_poNode )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HConstNodeProxy const& other )
	: f_poNode( other.f_poNode )
	{
	}

HXml::HConstNodeProxy::HConstNodeProxy( HNodeProxy const& other )
	: f_poNode( other.f_poNode )
	{
	}

HXml::HNodeProxy& HXml::HNodeProxy::operator = ( HNodeProxy const& other )
	{
	if ( &other != this )
		f_poNode = other.f_poNode;
	return ( *this );
	}

HXml::HConstNodeProxy& HXml::HConstNodeProxy::operator = ( HConstNodeProxy const& other )
	{
	if ( &other != this )
		f_poNode = other.f_poNode;
	return ( *this );
	}

int HXml::HNodeProxy::get_level( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode );
	return ( f_poNode->get_level() );
	M_EPILOG
	}

int HXml::HConstNodeProxy::get_level( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode );
	return ( f_poNode->get_level() );
	M_EPILOG
	}

HXml::HNode::TYPE::type_t HXml::HNodeProxy::get_type( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode );
	return ( (**f_poNode).f_eType );
	M_EPILOG
	}

HXml::HNode::TYPE::type_t HXml::HConstNodeProxy::get_type( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode );
	return ( (**f_poNode).f_eType );
	M_EPILOG
	}

HString const& HXml::HNodeProxy::get_name( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oText );
	M_EPILOG
	}

HString const& HXml::HConstNodeProxy::get_name( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oText );
	M_EPILOG
	}

HString const& HXml::HNodeProxy::get_value( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_CONTENT ) );
	return ( (**f_poNode).f_oText );
	M_EPILOG
	}

HString const& HXml::HConstNodeProxy::get_value( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_CONTENT ) );
	return ( (**f_poNode).f_oText );
	M_EPILOG
	}

HXml::HNode::properties_t& HXml::HNodeProxy::properties( void )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	M_EPILOG
	}

HXml::HNode::properties_t const& HXml::HNodeProxy::properties( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	M_EPILOG
	}

HXml::HNode::properties_t const& HXml::HConstNodeProxy::properties( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::add_node( HXml::HNode::TYPE::type_t const& a_eType, yaal::hcore::HString const& a_oName )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator it = f_poNode->add_node();
	(**it).f_oText = a_oName;
	(**it).f_eType = a_eType;
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::add_node( yaal::hcore::HString const& a_oName, yaal::hcore::HString const& a_oValue )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator it = f_poNode->add_node();
	(**it).f_oText = a_oName;
	(**it).f_eType = HXml::HNode::TYPE::D_NODE;
	if ( !! a_oValue )
		{
		tree_t::iterator value = it->add_node();
		(**value).f_oText = a_oValue;
		(**value).f_eType = HXml::HNode::TYPE::D_CONTENT;
		}
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, HXml::HNode::TYPE::type_t const& a_eType, yaal::hcore::HString const& a_oName )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->insert_node( it.f_oIterator, HNode() );
	(**newIt).f_oText = a_oName;
	(**newIt).f_eType = a_eType;
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, yaal::hcore::HString const& a_oName, yaal::hcore::HString const& a_oValue )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->insert_node( it.f_oIterator, HNode() );
	(**newIt).f_oText = a_oName;
	(**newIt).f_eType = HXml::HNode::TYPE::D_NODE;;
	if ( !! a_oValue )
		{
		tree_t::iterator value = newIt->add_node();
		(**value).f_oText = a_oValue;
		(**value).f_eType = HXml::HNode::TYPE::D_CONTENT;
		}
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->move_node( it.f_oIterator, node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->move_node( node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->copy_node( it.f_oIterator, node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HNodeProxy node )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->copy_node( node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

bool HXml::HNodeProxy::has_childs( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->has_childs() );
	M_EPILOG
	}

bool HXml::HConstNodeProxy::has_childs( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->has_childs() );
	M_EPILOG
	}

int long HXml::HNodeProxy::child_count( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->child_count() );
	M_EPILOG
	}

int long HXml::HConstNodeProxy::child_count( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->child_count() );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::begin( void )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	M_EPILOG
	}

HXml::HIterator const HXml::HNodeProxy::begin( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	M_EPILOG
	}

HXml::HConstIterator const HXml::HConstNodeProxy::begin( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->begin() ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::end( void )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	M_EPILOG
	}

HXml::HIterator const HXml::HNodeProxy::end( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	M_EPILOG
	}

HXml::HConstIterator const HXml::HConstNodeProxy::end( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->end() ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::rbegin( void )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	M_EPILOG
	}

HXml::HIterator const HXml::HNodeProxy::rbegin( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	M_EPILOG
	}

HXml::HConstIterator const HXml::HConstNodeProxy::rbegin( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->rbegin() ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::rend( void )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	M_EPILOG
	}

HXml::HIterator const HXml::HNodeProxy::rend( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	M_EPILOG
	}

HXml::HConstIterator const HXml::HConstNodeProxy::rend( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->rend() ) );
	M_EPILOG
	}

bool HXml::HNodeProxy::operator ! ( void ) const
	{
	return ( ! f_poNode );
	}

bool HXml::HConstNodeProxy::operator ! ( void ) const
	{
	return ( ! f_poNode );
	}

HXml::HIterator::HIterator( void )
	: f_poOwner( NULL ), f_oIterator()
	{
	}

HXml::HConstIterator::HConstIterator( void )
	: f_poOwner( NULL ), f_oIterator()
	{
	}

HXml::HIterator::HIterator( HXml::HNodeProxy const* a_poOwner, HXml::tree_t::iterator const& it )
	: f_poOwner( a_poOwner ), f_oIterator( it )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstNodeProxy const* a_poOwner, HXml::tree_t::const_iterator const& it )
	: f_poOwner( a_poOwner ), f_oIterator( it )
	{
	}

HXml::HIterator::HIterator( HXml::HIterator const& other )
	: f_poOwner( other.f_poOwner ), f_oIterator( other.f_oIterator )
	{
	}

HXml::HConstIterator::HConstIterator( HXml::HConstIterator const& other )
	: f_poOwner( other.f_poOwner ), f_oIterator( other.f_oIterator )
	{
	}

HXml::HIterator& HXml::HIterator::operator = ( HXml::HIterator const& other )
	{
	if ( &other != this )
		{
		f_poOwner = other.f_poOwner;
		f_oIterator = other.f_oIterator;
		}
	return ( *this );
	}

HXml::HConstIterator& HXml::HConstIterator::operator = ( HXml::HConstIterator const& other )
	{
	if ( &other != this )
		{
		f_poOwner = other.f_poOwner;
		f_oIterator = other.f_oIterator;
		}
	return ( *this );
	}

bool HXml::HIterator::operator == ( HXml::HIterator const& other ) const
	{
	return ( f_oIterator == other.f_oIterator );
	}

bool HXml::HConstIterator::operator == ( HXml::HConstIterator const& other ) const
	{
	return ( f_oIterator == other.f_oIterator );
	}

bool HXml::HIterator::operator != ( HXml::HIterator const& other ) const
	{
	return ( f_oIterator != other.f_oIterator );
	}

bool HXml::HConstIterator::operator != ( HXml::HConstIterator const& other ) const
	{
	return ( f_oIterator != other.f_oIterator );
	}

HXml::HNodeProxy HXml::HIterator::operator* ( void )
	{
	M_PROLOG
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	return ( HXml::HNodeProxy( &*f_oIterator ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::HConstIterator::operator* ( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	return ( HXml::HConstNodeProxy( &*f_oIterator ) );
	M_EPILOG
	}

void HXml::apply_style( yaal::hcore::HString const& a_oPath )
	{
	M_PROLOG
	M_ASSERT( f_poXml->f_oDoc.get() );
	HXsltParserGlobal::get_instance();
	style_resource_t style( xsltParseStylesheetFile( reinterpret_cast<xmlChar const* const>( a_oPath.raw() ) ), xsltFreeStylesheet );
	doc_resource_t doc( xsltApplyStylesheet( style.get(), f_poXml->f_oDoc.get(), NULL ), xmlFreeDoc );
	if ( ! doc.get() )
		throw HXmlException( HString( "cannot apply stylesheet: " ) + a_oPath );
	f_poXml->clear();
	swap( f_poXml->f_oDoc, doc );
	swap( f_poXml->f_oStyle, style );
	M_EPILOG
	}

HXml::const_xml_element_t HXml::get_element_by_id( const_xml_element_t const& node, yaal::hcore::HString const& id ) const
	{
	M_PROLOG
	const_xml_element_t result = NULL;
	HXml::HNode::properties_t::const_iterator idIt = (*node)->f_oProperties.find( "id" );
	if ( ( idIt != (*node)->f_oProperties.end() ) && ( idIt->second == id ) )
		result = node;
	for ( tree_t::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it )
		result = get_element_by_id( &*it, id );
	return ( result );
	M_EPILOG
	}

HXml::HNodeProxy HXml::get_element_by_id( yaal::hcore::HString const& id )
	{
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_id( f_oDOM.get_root(), id ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_id( yaal::hcore::HString const& id ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_id( f_oDOM.get_root(), id ) ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::remove_node( HXml::HIterator it )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->remove_node( it.f_oIterator );
	return ( HXml::HIterator( this, newIt ) );
	M_EPILOG
	}

HXml::const_xml_element_t HXml::get_element_by_path( const_xml_element_t const& node,
		HString const& path, int const& part ) const
	{
	M_PROLOG
	const_xml_element_t result = NULL;
	HString name = path.split( "/", part );
	if ( ! name.is_empty() )
		{
		if ( (**node).f_oText == name )
			{
			for ( tree_t::const_iterator it = node->begin(); ! result && ( it != node->end() ); ++ it )
				if ( (**it).f_eType == HXml::HNode::TYPE::D_NODE )
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
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_path( f_oDOM.get_root(), path, 1 ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_path( yaal::hcore::HString const& path ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_path( f_oDOM.get_root(), path, 1 ) ) );
	M_EPILOG
	}

namespace xml
{

char const* node_val( HXml::HConstNodeProxy const& node )
	{
	M_PROLOG
	HXml::HConstIterator it = node.begin();
	char const* t = "";
	if ( it != node.end() )
		{
		if ( (*it).get_type() == HXml::HNode::TYPE::D_CONTENT )
			t = (*it).get_value().raw();
		}
	return ( t );
	M_EPILOG
	}

char const* node_val( HXml::HConstIterator const& it )
	{
	M_PROLOG
	return ( node_val( *it ) );
	M_EPILOG
	}

char const* attr_val( HXml::HConstNodeProxy const& node, char const* const name )
	{
	M_PROLOG
	HXml::HNode::properties_t const& props = node.properties();
	HXml::HNode::properties_t::const_iterator prop = props.find( name );
	M_ENSURE( prop != props.end() );
	return ( prop->second.raw() );
	M_EPILOG
	}

char const* attr_val( HXml::HConstIterator const& it, char const* const name )
	{
	M_PROLOG
	return ( attr_val( *it, name ) );
	M_EPILOG
	}

}

}

}


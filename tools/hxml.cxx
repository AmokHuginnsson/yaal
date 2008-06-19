/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

	hxml.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
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
#include <libxml/xmlversion.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlmemory.h>
#include <libxml/encoding.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xinclude.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hxml.h"
#include "hcore/hsingleton.h"
#include "hcore/hresource.h"
#include "hcore/hlog.h"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace
{
/* char schema_err[] = "bad xml schema"; */
char const* const D_DEFAULT_ENCODING = "iso-8859-2";
typedef HResource<xmlDocPtr, typeof( &xmlFreeDoc )> doc_resource_t;
typedef HResource<xmlTextWriterPtr, typeof( &xmlFreeTextWriter )> writer_resource_t;
typedef HResource<xmlCharEncodingHandlerPtr, typeof( &xmlCharEncCloseFunc )> encoder_resource_t;
typedef HResource<xsltStylesheetPtr, typeof( &xsltFreeStylesheet )> style_resource_t;
typedef HResource<xmlXPathContextPtr, typeof( &xmlXPathFreeContext )> xpath_context_resource_t;
typedef HResource<xmlXPathObjectPtr, typeof( &xmlXPathFreeObject )> xpath_object_resource_t;
typedef HPointer<encoder_resource_t> encoder_resource_ptr_t;
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
		f_xIconvToExternal ( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal ( static_cast<iconv_t>( 0 ) ) { }
	OConvert( OConvert const& a_roConvert )
		: f_oEncoder(),
		f_xIconvToExternal ( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal ( static_cast<iconv_t>( 0 ) )
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
	void init( char const* const a_pcEncoding )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = xmlFindCharEncodingHandler( a_pcEncoding );
		if ( ! encoder )
			M_THROW( _( "cannot enable internal convertion" ), errno );
		f_oEncoder = encoder_resource_ptr_t(
				new encoder_resource_t( encoder, xmlCharEncCloseFunc ) );
		f_xIconvToExternal = (*f_oEncoder).get()->iconv_in;
		f_xIconvToInternal = (*f_oEncoder).get()->iconv_out;
		M_EPILOG
		}
	void init( char const* const a_pcEncoding, xmlNodePtr a_psRoot, char const* const a_pcFileName )
		{
		M_PROLOG
		xmlCharEncodingHandlerPtr encoder = NULL;
		if ( a_pcEncoding )
			encoder = xmlFindCharEncodingHandler( a_pcEncoding );
		else
			log( LOG_TYPE::D_WARNING ) << _ ( "HXml::WARNING: no encoding declared in `" )
				<< a_pcFileName << "'." << endl;
		if ( ! encoder )
			{
			log( LOG_TYPE::D_WARNING ) << _ ( "HXml::WARNING: char encoding handler not found" ) << endl;
			xmlCharEncoding l_xEncoding = xmlDetectCharEncoding( a_psRoot->name,
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
	xpath_object_resource_t::swap( f_oXPathObject, obj );
	xpath_context_resource_t::swap( f_oXPathContext, ctx );
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

char const* HXml::convert( char const* a_pcData, way_t a_eWay ) const
	{
	M_PROLOG
	M_ASSERT( a_pcData );
	char D_YAAL_TOOLS_HXML_ICONV_CONST* source = const_cast<char D_YAAL_TOOLS_HXML_ICONV_CONST*>( a_pcData );
	iconv_t l_xCD = static_cast<iconv_t>( 0 );
	switch ( a_eWay )
		{
		case ( D_TO_EXTERNAL ): { l_xCD = ( *f_oConvert ).f_xIconvToExternal; break; }
		case ( D_TO_INTERNAL ): { l_xCD = ( *f_oConvert ).f_xIconvToInternal; break; }
		default :
			{
			M_ASSERT( ! _( "unknown convertion way" ) );
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

int HXml::get_node_set_by_path( char const* a_pcPath )
	{
	M_PROLOG
	f_oVarTmpBuffer = a_pcPath;
	int l_iLength = f_oVarTmpBuffer.get_length() - 1;
	xpath_context_resource_t ctx( xmlXPathNewContext( f_poXml->f_oDoc.get() ), xmlXPathFreeContext );
	int setSize = 0;
	f_poXml->f_psNodeSet = NULL;
	if ( ctx.get() )
		{
		xmlXPathObjectPtr objPtr = NULL;
		while ( ! f_oVarTmpBuffer.is_empty() )
			{
			objPtr = xmlXPathEvalExpression(
					reinterpret_cast<xmlChar const*>( static_cast<char const* const>( f_oVarTmpBuffer ) ),
					ctx.get() );
			if ( objPtr )
				break;
			f_oVarTmpBuffer.set_at( l_iLength --, 0 );
			}
		xpath_object_resource_t obj( objPtr, xmlXPathFreeObject );
		if ( obj.get() )
			{
			f_oVarTmpBuffer = a_pcPath;
			f_poXml->f_psNodeSet = obj.get()->nodesetval;
			xpath_object_resource_t::swap( f_poXml->f_oXPathObject, obj );
			setSize = f_poXml->f_psNodeSet ? f_poXml->f_psNodeSet->nodeNr : 0;
			}
		}
	xpath_context_resource_t::swap( f_poXml->f_oXPathContext, ctx );
	return ( setSize );
	M_EPILOG
	}

void HXml::init( char const* a_pcFileName )
	{
	M_PROLOG
	int l_iSavedErrno = errno;
	HString l_oError;
	HXmlParserGlobal::get_instance();
	errno = 0;
	doc_resource_t doc( xmlParseFile( a_pcFileName ), xmlFreeDoc );
	if ( errno )
		{
		log( LOG_TYPE::D_WARNING ) << ::strerror( errno ) << ": " << a_pcFileName;
		log << ", code: " << errno << '.' << endl;
		}
	errno = l_iSavedErrno;
	if ( ! doc.get() )
		{
		l_oError.format( _( "cannot parse `%s'" ), a_pcFileName );
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
			root, a_pcFileName );
	doc_resource_t::swap( f_poXml->f_oDoc, doc );
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

void HXml::parse( char const* a_pcXPath, bool a_bStripEmpty )
	{
	M_PROLOG
	char const* const D_FULL_TREE = "//*";
	if ( ! a_pcXPath || ! a_pcXPath[ 0 ] )
		a_pcXPath = D_FULL_TREE; /* scan full tree */
	get_node_set_by_path( a_pcXPath );
	f_oDOM.clear();
	int l_iCtr = 0;
	while ( a_pcXPath[ l_iCtr ] )
		l_iCtr ++;
	l_iCtr --;
	M_ASSERT ( l_iCtr >= 0 );
	if ( f_poXml->f_psNodeSet )
		{
		if ( strcmp( a_pcXPath, D_FULL_TREE ) )
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

void HXml::load( char const* const a_pcPath )
	{
	M_PROLOG
	init( a_pcPath );
	parse( NULL, true );
	return;
	M_EPILOG
	}

void HXml::save( char const* const a_pcPath ) const
	{
	M_PROLOG
	do_save();
	M_ASSERT( f_poXml->f_oDoc.get() );
	if ( f_poXml->f_oStyle.get() )
		{
		if ( xsltSaveResultToFilename( a_pcPath, f_poXml->f_oDoc.get(), f_poXml->f_oStyle.get(), 0 ) == -1 )
			throw HXmlException( HString( "Cannot open file for writting: " ) + a_pcPath );
		}
	else
		{
		if ( xmlSaveFile( a_pcPath, f_poXml->f_oDoc.get() ) == -1 )
			throw HXmlException( HString( "Cannot open file for writting: " ) + a_pcPath );
		}
	return;
	M_EPILOG
	}

void HXml::save( int const& a_iFileDes ) const
	{
	M_PROLOG
	do_save();
	M_ASSERT( f_poXml->f_oDoc.get() );
	if ( f_poXml->f_oStyle.get() )
		{
		if ( xsltSaveResultToFd( a_iFileDes, f_poXml->f_oDoc.get(), f_poXml->f_oStyle.get() ) == -1 )
			throw HXmlException( HString( "Cannot connect to file descriptor: " ) + a_iFileDes );
		}
	else
		{
		if ( xmlSaveFileTo( xmlOutputBufferCreateFd( a_iFileDes, NULL ), f_poXml->f_oDoc.get(), f_oEncoding ) == -1 )
			throw HXmlException( HString( "Cannot connect to file descriptor: " ) + a_iFileDes );
		}
	return;
	M_EPILOG
	}

void HXml::do_save( void ) const
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
		doc_resource_t::swap( doc, dummy );
		int rc = xmlTextWriterStartDocument( writer.get(), NULL, f_oEncoding, "yes" );
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
		doc_resource_t::swap( f_poXml->f_oDoc, doc );
	return;
	M_EPILOG
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
				reinterpret_cast<xmlChar const* const>( convert( pvalue.raw(), D_TO_EXTERNAL ) ) );
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
					reinterpret_cast<xmlChar const*>( convert( value, D_TO_EXTERNAL ) ) );
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

void HXml::create_root( char const* const a_pcName, char const* const a_pcEncoding )
	{
	M_PROLOG
	M_ASSERT( a_pcName );
	f_oEncoding = a_pcEncoding ? a_pcEncoding : D_DEFAULT_ENCODING;
	tree_t::node_t root = f_oDOM.create_new_root();
	(**root).f_oText = a_pcName;
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

HXml::HIterator HXml::HNodeProxy::add_node( HXml::HNode::TYPE::type_t const& a_eType, char const* const a_pcName )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator it = f_poNode->add_node();
	(**it).f_oText = a_pcName;
	(**it).f_eType = a_eType;
	return ( HXml::HIterator( this, it ) );
	M_EPILOG
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, HXml::HNode::TYPE::type_t const& a_eType, char const* const a_pcName )
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->insert_node( it.f_oIterator, HNode() );
	(**newIt).f_oText = a_pcName;
	(**newIt).f_eType = a_eType;
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

int HXml::HNodeProxy::child_count( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->child_count() );
	M_EPILOG
	}

int HXml::HConstNodeProxy::child_count( void ) const
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

void HXml::apply_style( char const* const a_pcPath )
	{
	M_PROLOG
	M_ASSERT( f_poXml->f_oDoc.get() );
	HXsltParserGlobal::get_instance();
	style_resource_t style( xsltParseStylesheetFile( reinterpret_cast<xmlChar const* const>( a_pcPath ) ), xsltFreeStylesheet );
	doc_resource_t doc( xsltApplyStylesheet( style.get(), f_poXml->f_oDoc.get(), NULL ), xmlFreeDoc );
	if ( ! doc.get() )
		throw HXmlException( HString( "cannot apply stylesheet: " ) + a_pcPath );
	f_poXml->clear();
	doc_resource_t::swap( f_poXml->f_oDoc, doc );
	style_resource_t::swap( f_poXml->f_oStyle, style );
	M_EPILOG
	}

HXml::const_xml_element_t HXml::get_element_by_id( const_xml_element_t const& node, char const* const id ) const
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

HXml::HNodeProxy HXml::get_element_by_id( char const* const id )
	{
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_id( f_oDOM.get_root(), id ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_id( char const* const id ) const
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

HXml::HNodeProxy HXml::get_element_by_path( char const* const path )
	{
	M_PROLOG
	return ( HNodeProxy( const_cast<xml_element_t>( get_element_by_path( f_oDOM.get_root(), path, 1 ) ) ) );
	M_EPILOG
	}

HXml::HConstNodeProxy const HXml::get_element_by_path( char const* const path ) const
	{
	M_PROLOG
	return ( HConstNodeProxy( get_element_by_path( f_oDOM.get_root(), path, 1 ) ) );
	M_EPILOG
	}

}

}


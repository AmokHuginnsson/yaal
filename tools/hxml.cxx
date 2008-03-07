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
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>

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
char free_err[] = "trying to free NULL pointer";
/* char schema_err[] = "bad xml schema"; */
char const* const D_DEFAULT_ENCODING = "iso-8859-2";
typedef HResource<xmlDocPtr, typeof( &xmlFreeDoc )> doc_resource_t;
typedef HResource<xmlTextWriterPtr, typeof( &xmlFreeTextWriter )> writer_resource_t;
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
	xmlCleanupParser();
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
	xsltCleanupGlobals();
	}

class HXmlData
	{
private:
	/*{*/
	friend class HXml;
	doc_resource_t			f_oDoc;
	xmlNodePtr					f_psRoot;
	xmlXPathContextPtr	f_psContext;
	xmlXPathObjectPtr		f_psObject;
	xmlNodeSetPtr				f_psNodeSet;
	xmlCharEncodingHandlerPtr f_psCharEncodingHandler;
	/*}*/
protected:
	/*{*/
	HXmlData( void );
	virtual ~HXmlData( void );
	HXmlData( HXmlData const& ) __attribute__(( __noreturn__ ));
	HXmlData& operator = ( HXmlData const& ) __attribute__(( __noreturn__ ));
	void xml_free( xmlXPathContextPtr& ) const;
	void xml_free( xmlXPathObjectPtr& ) const;
	void clear( void );
	/*}*/
	};

struct HXml::OConvert
	{
	iconv_t f_xIconvToExternal;
	iconv_t f_xIconvToInternal;
	OConvert( void ) 
		: f_xIconvToExternal ( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal ( static_cast<iconv_t>( 0 ) ) { }
	OConvert( OConvert const& a_roConvert )
		: f_xIconvToExternal ( static_cast<iconv_t>( 0 ) ),
		f_xIconvToInternal ( static_cast<iconv_t>( 0 ) )
		{
		operator = ( a_roConvert );
		}
	OConvert& operator = ( OConvert const& a_roConvert )
		{
		if ( &a_roConvert != this )
			{
			f_xIconvToExternal = a_roConvert.f_xIconvToExternal;
			f_xIconvToInternal = a_roConvert.f_xIconvToInternal;
			}
		return ( *this );
		}
	};

HXmlData::HXmlData( void ) : f_oDoc( NULL, xmlFreeDoc ), f_psRoot( NULL ),
	f_psContext( NULL ), f_psObject( NULL ),
	f_psNodeSet( NULL ), f_psCharEncodingHandler( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HXmlData::~HXmlData ( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

void HXmlData::clear( void )
	{
	M_PROLOG
	f_psNodeSet = NULL;
	if ( f_psContext )
		xml_free( f_psContext );
	if ( f_psObject )
		xml_free( f_psObject );
	return;
	M_EPILOG
	}

void HXmlData::xml_free( xmlXPathContextPtr& a_rpsContext ) const
	{
	M_PROLOG
	if ( ! a_rpsContext )
		M_THROW( free_err, errno );
	xmlXPathFreeContext( a_rpsContext );
	a_rpsContext = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free( xmlXPathObjectPtr& a_rpsObject ) const
	{
	M_PROLOG
	if ( ! a_rpsObject )
		M_THROW( free_err, errno );
	xmlXPathFreeObject( a_rpsObject );
	a_rpsObject = NULL;
	return;
	M_EPILOG
	}

HXml::HXml( void )
	: f_oConvert( new OConvert ), f_oConvertedString(),
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
	if ( f_poXml->f_psCharEncodingHandler )
		xmlCharEncCloseFunc( f_poXml->f_psCharEncodingHandler );
	xmlCleanupCharEncodingHandlers();
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
	if ( f_poXml->f_psObject )
		f_poXml->xml_free ( f_poXml->f_psObject );
	if ( f_poXml->f_psContext )
		f_poXml->xml_free ( f_poXml->f_psContext );
	f_poXml->clear();
	f_poXml->f_psContext = xmlXPathNewContext( f_poXml->f_oDoc.get() );
	if ( f_poXml->f_psContext )
		{
		while ( f_oVarTmpBuffer[ 0 ] )
			{
			f_poXml->f_psObject = xmlXPathEvalExpression(
					reinterpret_cast<xmlChar const*>( static_cast<char const* const>( f_oVarTmpBuffer ) ),
					f_poXml->f_psContext );
			if ( f_poXml->f_psObject )
				break;
			f_oVarTmpBuffer.set_at( l_iLength --, 0 );
			}
		if ( f_poXml->f_psObject )
			{
			f_poXml->f_psNodeSet = f_poXml->f_psObject->nodesetval;
			f_oVarTmpBuffer = a_pcPath;
			return ( f_poXml->f_psNodeSet ? f_poXml->f_psNodeSet->nodeNr : 0 );
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HXml::init( char const* a_pcFileName )
	{
	M_PROLOG
	int l_iSavedErrno = errno;
	xmlCharEncoding l_xEncoding = static_cast<xmlCharEncoding>( 0 );
	HString l_oError;
	HXmlParserGlobal::get_instance();
	f_poXml->clear();
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
	f_poXml->f_psRoot = xmlDocGetRootElement( doc.get() );
	if ( ! f_poXml->f_psRoot )
		M_THROW ( _ ( "empty doc" ), errno );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", f_poXml->f_psRoot->name );
#endif /* __DEBUGGER_BABUNI__ */
	if ( doc.get()->encoding )
		f_poXml->f_psCharEncodingHandler = xmlFindCharEncodingHandler (
				reinterpret_cast<char const *>( doc.get()->encoding ) );
	else
		log( LOG_TYPE::D_WARNING ) << _ ( "HXml::WARNING: no encoding declared in `" )
			<< a_pcFileName << "'." << endl;
	if ( ! f_poXml->f_psCharEncodingHandler )
		{
		log( LOG_TYPE::D_WARNING ) << _ ( "HXml::WARNING: char encoding handler not found" ) << endl;
		l_xEncoding = xmlDetectCharEncoding ( f_poXml->f_psRoot->name,
				xmlStrlen ( f_poXml->f_psRoot->name ) );
		if ( ! l_xEncoding )
			M_THROW( _( "cannot detect character encoding" ), errno );
		f_poXml->f_psCharEncodingHandler = xmlGetCharEncodingHandler( l_xEncoding );
		}
	if ( ! f_poXml->f_psCharEncodingHandler )
		M_THROW( _( "cannot enable internal convertion" ), errno );
	(*f_oConvert).f_xIconvToExternal = f_poXml->f_psCharEncodingHandler->iconv_in;
	(*f_oConvert).f_xIconvToInternal = f_poXml->f_psCharEncodingHandler->iconv_out;
	f_poXml->f_oDoc.swap( doc );
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
				l_psNode = l_psNode->next; /* FIXME add DTD handling later (keeping track of entities) */
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
	if ( ! a_pcXPath || ! a_pcXPath[ 0 ] )
		a_pcXPath = "/"; /* scan full tree */
	get_node_set_by_path( a_pcXPath );
	f_oDOM.clear();
	int l_iCtr = 0;
	while ( a_pcXPath[ l_iCtr ] )
		l_iCtr ++;
	l_iCtr --;
	M_ASSERT ( l_iCtr >= 0 );
	if ( f_poXml->f_psNodeSet )
		{
		if ( f_poXml->f_psNodeSet->nodeNr > 1 )
			{
			for ( l_iCtr = 0; l_iCtr < f_poXml->f_psNodeSet->nodeNr; ++ l_iCtr )
				parse( f_poXml->f_psNodeSet->nodeTab[ l_iCtr ],
						NULL, a_bStripEmpty );
			}
		else if ( f_poXml->f_psNodeSet->nodeNr == 1 )
			parse( f_poXml->f_psNodeSet->nodeTab[ 0 ],
					NULL, a_bStripEmpty );
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
	writer_resource_t writer( xmlNewTextWriterFilename( a_pcPath, 0 ), xmlFreeTextWriter );
	if ( ! writer.get() )
		throw HXmlException( HString( "Cannot open file for writting: " ) + a_pcPath );
	int rc = xmlTextWriterStartDocument( writer.get(), NULL, f_oEncoding, NULL );
	if ( rc < 0 )
		throw HXmlException( HString( "Unable to start document with encoding: " ) + f_oEncoding );
	rc = xmlTextWriterSetIndent( writer.get(), 1 );
	if ( rc < 0 )
		throw HXmlException( "Unable to enable indenting." );
	static char const* const D_INDENTION_STRING = "\t";
	rc = xmlTextWriterSetIndentString( writer.get(), reinterpret_cast<xmlChar const* const>( D_INDENTION_STRING ) );
	if ( rc < 0 )
		throw HXmlException( "Cannot set indent string." );
	dump_node( &writer, get_root() );
	rc = xmlTextWriterEndDocument( writer.get() );
	if ( rc < 0 )
		throw HXmlException( "Unable to end document." );
	return;
	M_EPILOG
	}

void HXml::dump_node( void* writer_p, HConstNodeProxy const& node ) const
	{
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
	}

void HXml::create_root( char const* const a_pcName, char const* const a_pcEncoding )
	{
	M_PROLOG
	M_ASSERT( a_pcName );
	if ( a_pcEncoding )
		f_oEncoding = a_pcEncoding;
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
	M_ASSERT( f_poNode );
	return ( f_poNode->get_level() );
	}

int HXml::HConstNodeProxy::get_level( void ) const
	{
	M_ASSERT( f_poNode );
	return ( f_poNode->get_level() );
	}

HXml::HNode::TYPE::type_t HXml::HNodeProxy::get_type( void ) const
	{
	M_ASSERT( f_poNode );
	return ( (**f_poNode).f_eType );
	}

HXml::HNode::TYPE::type_t HXml::HConstNodeProxy::get_type( void ) const
	{
	M_ASSERT( f_poNode );
	return ( (**f_poNode).f_eType );
	}

HString const& HXml::HNodeProxy::get_name( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oText );
	}

HString const& HXml::HConstNodeProxy::get_name( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oText );
	}

HString const& HXml::HNodeProxy::get_value( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_CONTENT ) );
	return ( (**f_poNode).f_oText );
	}

HString const& HXml::HConstNodeProxy::get_value( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_CONTENT ) );
	return ( (**f_poNode).f_oText );
	}

HXml::HNode::properties_t& HXml::HNodeProxy::properties( void )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	}

HXml::HNode::properties_t const& HXml::HNodeProxy::properties( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	}

HXml::HNode::properties_t const& HXml::HConstNodeProxy::properties( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( (**f_poNode).f_oProperties );
	}

HXml::HIterator HXml::HNodeProxy::add_node( HXml::HNode::TYPE::type_t const& a_eType, char const* const a_pcName )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator it = f_poNode->add_node();
	(**it).f_oText = a_pcName;
	(**it).f_eType = a_eType;
	return ( HXml::HIterator( this, it ) );
	}

HXml::HIterator HXml::HNodeProxy::insert_node( HXml::HIterator it, HXml::HNode::TYPE::type_t const& a_eType, char const* const a_pcName )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->insert_node( it.f_oIterator, HNode() );
	(**newIt).f_oText = a_pcName;
	(**newIt).f_eType = a_eType;
	return ( HXml::HIterator( this, newIt ) );
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->move_node( it.f_oIterator, node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	}

HXml::HIterator HXml::HNodeProxy::move_node( HXml::HNodeProxy node )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->move_node( node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HIterator it, HXml::HNodeProxy node )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->copy_node( it.f_oIterator, node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	}

HXml::HIterator HXml::HNodeProxy::copy_node( HXml::HNodeProxy node )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	tree_t::iterator newIt = f_poNode->copy_node( node.f_poNode );
	return ( HXml::HIterator( this, newIt ) );
	}

bool HXml::HNodeProxy::has_childs( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->has_childs() );
	}

bool HXml::HConstNodeProxy::has_childs( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->has_childs() );
	}

int HXml::HNodeProxy::child_count( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->child_count() );
	}

int HXml::HConstNodeProxy::child_count( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( f_poNode->child_count() );
	}

HXml::HIterator HXml::HNodeProxy::begin( void )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	}

HXml::HIterator const HXml::HNodeProxy::begin( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	}

HXml::HConstIterator const HXml::HConstNodeProxy::begin( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->begin() ) );
	}

HXml::HIterator HXml::HNodeProxy::end( void )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	}

HXml::HIterator const HXml::HNodeProxy::end( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	}

HXml::HConstIterator const HXml::HConstNodeProxy::end( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->end() ) );
	}

HXml::HIterator HXml::HNodeProxy::rbegin( void )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	}

HXml::HIterator const HXml::HNodeProxy::rbegin( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	}

HXml::HConstIterator const HXml::HConstNodeProxy::rbegin( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->rbegin() ) );
	}

HXml::HIterator HXml::HNodeProxy::rend( void )
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	}

HXml::HIterator const HXml::HNodeProxy::rend( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	}

HXml::HConstIterator const HXml::HConstNodeProxy::rend( void ) const
	{
	M_ASSERT( f_poNode && ( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE ) );
	return ( HXml::HConstIterator( this, f_poNode->rend() ) );
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
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	return ( HXml::HNodeProxy( &*f_oIterator ) );
	}

HXml::HConstNodeProxy const HXml::HConstIterator::operator* ( void ) const
	{
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	return ( HXml::HConstNodeProxy( &*f_oIterator ) );
	}

void HXml::apply_style( char const* const a_pcPath )
	{
	M_ASSERT( f_poXml->f_oDoc.get() );
	HXsltParserGlobal::get_instance();
	typedef HResource<xsltStylesheetPtr, typeof( &xsltFreeStylesheet )> style_resource_t;
	style_resource_t style( xsltParseStylesheetFile( reinterpret_cast<xmlChar const* const>( a_pcPath ) ), xsltFreeStylesheet );
	doc_resource_t doc( xsltApplyStylesheet( style.get(), f_poXml->f_oDoc.get() ,NULL ), xmlFreeDoc );
	if ( ! doc.get() )
		throw HXmlException( HString( "cannot apply stylesheet: " ) + a_pcPath );
	f_poXml->f_oDoc.swap( doc );
	}

}

}


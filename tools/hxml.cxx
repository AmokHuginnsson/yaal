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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hxml.h"
#include "hcore/hlog.h"

using namespace yaal::hcore;
using namespace yaal::tools;

char free_err [ ] = "trying to free NULL pointer";
char schema_err [ ] = "bad xml schema";

namespace yaal
{

namespace tools
{
	
class HXmlData
	{
private:
	/*{*/
	friend class HXml;
	xmlDocPtr						f_psDoc;
	xmlNodePtr					f_psRoot;
	xmlXPathContextPtr	f_psContext;
	xmlXPathObjectPtr		f_psObject;
	xmlNodeSetPtr				f_psNodeSet;
	xmlCharEncodingHandlerPtr f_psCharEncodingHandler;
	/*}*/
protected:
	/*{*/
	HXmlData ( void );
	virtual ~HXmlData ( void );
	HXmlData ( HXmlData const & ) __attribute__(( __noreturn__ ));
	HXmlData & operator = ( HXmlData const & ) __attribute__(( __noreturn__ ));
	void xml_free ( xmlDocPtr & ) const;
/*	void xml_free ( xmlNodePtr & ); */
/*	void xml_free ( xmlNodeSetPtr & ); */
	void xml_free ( xmlXPathContextPtr & ) const;
	void xml_free ( xmlXPathObjectPtr & ) const;
	void reset ( void );
	/*}*/
	};

struct HXml::OConvert
	{
	iconv_t f_xIconvIn;
	iconv_t f_xIconvOut;
	OConvert ( void ) 
		: f_xIconvIn ( static_cast < iconv_t > ( 0 ) ),
		f_xIconvOut ( static_cast < iconv_t > ( 0 ) ) { }
	OConvert ( OConvert const & a_roConvert )
		: f_xIconvIn ( static_cast < iconv_t > ( 0 ) ),
		f_xIconvOut ( static_cast < iconv_t > ( 0 ) )
		{
		operator = ( a_roConvert );
		}
	OConvert & operator = ( OConvert const & a_roConvert )
		{
		if ( & a_roConvert != this )
			{
			f_xIconvIn = a_roConvert.f_xIconvIn;
			f_xIconvOut = a_roConvert.f_xIconvOut;
			}
		return ( * this );
		}
	};

HXmlData::HXmlData ( void ) : f_psDoc ( NULL ), f_psRoot ( NULL ),
										f_psContext ( NULL ), f_psObject ( NULL ),
										f_psNodeSet ( NULL ), f_psCharEncodingHandler ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HXmlData::~HXmlData ( void )
	{
	M_PROLOG
	if ( f_psContext )
		xml_free ( f_psContext );
	if ( f_psObject )
		xml_free ( f_psObject );
	if ( f_psDoc )
		xml_free ( f_psDoc );
	return;
	M_EPILOG
	}

void HXmlData::reset ( void )
	{
	M_PROLOG
	f_psNodeSet = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlDocPtr & a_rpsDoc ) const
	{
	M_PROLOG
	if ( ! a_rpsDoc )
		M_THROW ( free_err, errno );
	xmlFreeDoc ( a_rpsDoc );
	a_rpsDoc = NULL;
	return;
	M_EPILOG
	}

/*
void HXmlData::xml_free ( xmlNodePtr & a_rpsNode )
	{
	M_PROLOG
	if ( ! a_rpsNode )
		M_THROW ( free_err, errno );
	xmlFreeNode ( a_rpsNode );
	a_rpsNode = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlNodeSetPtr & a_rpsNodeSet )
	{
	M_PROLOG
	if ( ! a_rpsNodeSet )
		M_THROW ( free_err, errno );
	xmlXPathFreeNodeSet ( a_rpsNodeSet );
	a_rpsNodeSet = NULL;
	return;
	M_EPILOG
	}
*/

void HXmlData::xml_free ( xmlXPathContextPtr & a_rpsContext ) const
	{
	M_PROLOG
	if ( ! a_rpsContext )
		M_THROW ( free_err, errno );
	xmlXPathFreeContext ( a_rpsContext );
	a_rpsContext = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlXPathObjectPtr & a_rpsObject ) const
	{
	M_PROLOG
	if ( ! a_rpsObject )
		M_THROW ( free_err, errno );
	xmlXPathFreeObject ( a_rpsObject );
	a_rpsObject = NULL;
	return;
	M_EPILOG
	}

HXml::HXml ( void )
	: f_oConvert ( new OConvert ), f_oConvertedString(),
	f_oVarTmpBuffer(), f_poXml ( NULL ), f_oDOM()
	{
	M_PROLOG
	f_poXml = new ( std::nothrow ) HXmlData();
	M_ENSURE ( f_poXml );
	M_EPILOG
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	if ( f_poXml->f_psCharEncodingHandler )
		xmlCharEncCloseFunc ( f_poXml->f_psCharEncodingHandler );
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

char const* HXml::convert( char D_YAAL_TOOLS_HXML_ICONV_CONST* a_pcData, way_t a_eWay )
	{
	M_PROLOG
	iconv_t l_xCD = static_cast<iconv_t>( 0 );
	switch ( a_eWay )
		{
		case ( D_IN ): { l_xCD = ( *f_oConvert ).f_xIconvIn; break; }
		case ( D_OUT ): { l_xCD = ( *f_oConvert ).f_xIconvOut; break; }
		default :
			{
			M_THROW( _( "unknown convertion way" ), a_eWay );
			break;
			}
		}
	size_t l_uiSizeOut = 0, l_uiSizeIn = ::strlen( a_pcData );
	/* The longers single character in any encoding is 6 bytes long. */
	size_t const D_ICONV_OUTPUT_BUFFER_LENGTH = 8;
	/* Additional character for nil terminator. */
	char l_pcOutput[ D_ICONV_OUTPUT_BUFFER_LENGTH + 1 ];
	f_oConvertedString = "";
	do
		{
		::memset( l_pcOutput, 0, D_ICONV_OUTPUT_BUFFER_LENGTH + 1 );
		l_uiSizeOut = D_ICONV_OUTPUT_BUFFER_LENGTH;
		char* l_pcOut = l_pcOutput;
		M_ENSURE( ( ::iconv( l_xCD, &a_pcData, &l_uiSizeIn, &l_pcOut,
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
	f_poXml->reset();
	f_poXml->f_psContext = xmlXPathNewContext( f_poXml->f_psDoc );
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

void HXml::init ( char const * a_pcFileName )
	{
	M_PROLOG
	int l_iSavedErrno = errno;
	xmlCharEncoding l_xEncoding = static_cast<xmlCharEncoding>( 0 );
	HString l_oError;
	if ( f_poXml->f_psDoc )
		f_poXml->xml_free ( f_poXml->f_psDoc );
	f_poXml->reset();
	errno = 0;
	f_poXml->f_psDoc = xmlParseFile( a_pcFileName );
	if ( errno )
		{
		log( LOG_TYPE::D_WARNING ) << ::strerror( errno ) << ": " << a_pcFileName;
		log << ", code: " << errno << '.' << endl;
		}
	errno = l_iSavedErrno;
	if ( ! f_poXml->f_psDoc )
		{
		l_oError.format ( _ ( "cannot parse `%s'" ), a_pcFileName );
		M_THROW ( l_oError, errno );
		}
	f_poXml->f_psRoot = xmlDocGetRootElement ( f_poXml->f_psDoc );
	if ( ! f_poXml->f_psRoot )
		M_THROW ( _ ( "empty doc" ), errno );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", f_poXml->f_psRoot->name );
#endif /* __DEBUGGER_BABUNI__ */
	if ( f_poXml->f_psDoc->encoding )
		f_poXml->f_psCharEncodingHandler = xmlFindCharEncodingHandler (
				reinterpret_cast < char const * > ( f_poXml->f_psDoc->encoding ) );
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
	(*f_oConvert).f_xIconvIn = f_poXml->f_psCharEncodingHandler->iconv_in;
	(*f_oConvert).f_xIconvOut = f_poXml->f_psCharEncodingHandler->iconv_out;
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

HXml::HNodeProxy::HNodeProxy( HXml::tree_t::node_t a_poNode ) : f_poNode( a_poNode )
	{
	}

HXml::HNodeProxy HXml::get_root ( void )
	{
	M_PROLOG
	return ( HNodeProxy( f_oDOM.get_root() ) );
	M_EPILOG
	}

int HXml::HNodeProxy::get_level( void ) const
	{
	return ( f_poNode->get_level() );
	}

HXml::HNode::TYPE::type_t HXml::HNodeProxy::get_type( void ) const
	{
	return ( (**f_poNode).f_eType );
	}

HString const& HXml::HNodeProxy::get_name( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( (**f_poNode).f_oText );
	}

HString const& HXml::HNodeProxy::get_value( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_CONTENT );
	return ( (**f_poNode).f_oText );
	}

HXml::HNode::properties_t& HXml::HNodeProxy::properties( void )
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( (**f_poNode).f_oProperties );
	}

HXml::HNode::properties_t const& HXml::HNodeProxy::properties( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( (**f_poNode).f_oProperties );
	}

bool HXml::HNodeProxy::has_childs( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( f_poNode->has_childs() );
	}

int HXml::HNodeProxy::child_count( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( f_poNode->child_count() );
	}

HXml::HIterator HXml::HNodeProxy::begin( void )
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	}

HXml::HIterator const HXml::HNodeProxy::begin( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->begin() ) );
	}

HXml::HIterator HXml::HNodeProxy::end( void )
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	}

HXml::HIterator const HXml::HNodeProxy::end( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->end() ) );
	}

HXml::HIterator HXml::HNodeProxy::rbegin( void )
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	}

HXml::HIterator const HXml::HNodeProxy::rbegin( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->rbegin() ) );
	}

HXml::HIterator HXml::HNodeProxy::rend( void )
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	}

HXml::HIterator const HXml::HNodeProxy::rend( void ) const
	{
	M_ASSERT( (**f_poNode).f_eType == HXml::HNode::TYPE::D_NODE );
	return ( HXml::HIterator( this, f_poNode->rend() ) );
	}

HXml::HIterator::HIterator( HXml::HNodeProxy const* a_poOwner, HXml::tree_t::iterator const& it )
	: f_poOwner( a_poOwner ), f_oIterator( it ), f_oProxy( NULL )
	{
	}

bool HXml::HIterator::operator == ( HXml::HIterator const& other ) const
	{
	return ( f_oIterator == other.f_oIterator );
	}

bool HXml::HIterator::operator != ( HXml::HIterator const& other ) const
	{
	return ( f_oIterator != other.f_oIterator );
	}

HXml::HNodeProxy& HXml::HIterator::operator* ( void )
	{
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	f_oProxy.f_poNode = &*f_oIterator;
	return ( f_oProxy );
	}

HXml::HNodeProxy const& HXml::HIterator::operator* ( void ) const
	{
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	f_oProxy.f_poNode = &*f_oIterator;
	return ( f_oProxy );
	}

HXml::HNodeProxy* HXml::HIterator::operator->( void )
	{
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	f_oProxy.f_poNode = &*f_oIterator;
	return ( &f_oProxy );
	}

HXml::HNodeProxy const* HXml::HIterator::operator->( void ) const
	{
	M_ASSERT( f_poOwner );
	M_ASSERT( f_oIterator != f_poOwner->f_poNode->end() );
	f_oProxy.f_poNode = &*f_oIterator;
	return ( &f_oProxy );
	}

}

}


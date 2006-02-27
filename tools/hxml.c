/*
---        `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	hxml.h - this file is integral part of `stdhapi' project.

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

#include <stdio.h>
#include <string.h>
#include <libintl.h>
#include <new>

#include <libxml/xmlversion.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlmemory.h>
#include <libxml/encoding.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "hxml.h"
#include "hcore/hlog.h"

using namespace stdhapi::hcore;
using namespace stdhapi::tools;

char free_err [ ] = "trying to free NULL pointer";
char schema_err [ ] = "bad xml schema";

namespace stdhapi
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
	xmlNodePtr					f_psNode;
	xmlXPathContextPtr	f_psContext;
	xmlXPathObjectPtr		f_psObject;
	xmlNodeSetPtr				f_psNodeSet;
	xmlNodePtr					f_psNodeStart;
	xmlAttrPtr					f_psProperties;
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
	xmlNodePtr next_node ( xmlNodePtr, int & ) const;
	/*}*/
	};

}

}

HXmlData::HXmlData ( void ) : f_psDoc ( NULL ), f_psRoot ( NULL ),
															f_psNode ( NULL ),
										f_psContext ( NULL ), f_psObject ( NULL ),
										f_psNodeSet ( NULL ), f_psNodeStart ( NULL ),
										f_psProperties ( NULL )
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
	f_psNode = NULL;
	f_psNodeSet = NULL;
	f_psNodeStart = NULL;
	return;
	M_EPILOG
	}

HXmlData::HXmlData ( HXmlData const & a_roXml ) : f_psDoc ( NULL ),
										f_psRoot ( NULL ), f_psNode ( NULL ),
										f_psContext ( NULL ), f_psObject ( NULL ),
										f_psNodeSet ( NULL ), f_psNodeStart ( NULL ),
										f_psProperties ( NULL )
	{
	M_PROLOG
	this->operator = ( a_roXml );
	M_EPILOG
	}

HXmlData & HXmlData::operator = ( HXmlData const & )
	{
	M_PROLOG
	M_ASSERT ( ! "This method should not be called." );
	M_EPILOG
	}

void HXmlData::xml_free ( xmlDocPtr & a_rpsDoc ) const
	{
	M_PROLOG
	if ( ! a_rpsDoc )
		M_THROW ( free_err, g_iErrNo );
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
		M_THROW ( free_err, g_iErrNo );
	xmlFreeNode ( a_rpsNode );
	a_rpsNode = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlNodeSetPtr & a_rpsNodeSet )
	{
	M_PROLOG
	if ( ! a_rpsNodeSet )
		M_THROW ( free_err, g_iErrNo );
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
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeContext ( a_rpsContext );
	a_rpsContext = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlXPathObjectPtr & a_rpsObject ) const
	{
	M_PROLOG
	if ( ! a_rpsObject )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeObject ( a_rpsObject );
	a_rpsObject = NULL;
	return;
	M_EPILOG
	}

xmlNodePtr HXmlData::next_node ( xmlNodePtr a_psNode, int & a_riLevel ) const
	{
	M_PROLOG
	xmlNodePtr l_psNode = a_psNode;
	if ( l_psNode->children )
		{
		l_psNode = l_psNode->children;
		a_riLevel ++;
		}
	else if ( l_psNode->next )
		l_psNode = l_psNode->next;
	else if ( l_psNode->parent && l_psNode->parent->next )
		{
		l_psNode = l_psNode->parent->next;
		a_riLevel --;
		}
	else
		l_psNode = NULL;
	return ( l_psNode );
	M_EPILOG
	}

HXml::HXml ( void )
	: f_iIndex ( 0 ), f_iLevel ( 0 ),
	f_xIconvIn ( static_cast < iconv_t > ( 0 ) ),
	f_xIconvOut ( static_cast < iconv_t > ( 0 ) ),
	f_oConvertedString ( ), f_oVarTmpBuffer ( ), f_oPath ( ), f_poXml ( NULL )
	{
	M_PROLOG
	f_poXml = new ( std::nothrow ) HXmlData ( );
	M_ENSURE ( f_poXml );
	M_EPILOG
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	xmlCleanupCharEncodingHandlers ( );
	if ( f_poXml )
		delete f_poXml;
	f_poXml = NULL;
	return;
	M_EPILOG
	}

char const * HXml::convert ( char const * a_pcData, way_t a_eWay )
	{
	M_PROLOG
	size_t l_uiSizeIn = 0, l_uiSizeOut = 0, l_uiOrigSize = 0, l_uiTmp = 0;
	char * l_pcOut = NULL;
#ifdef HAVE_ICONV_INPUT_CONST
	char const * l_pcIn = a_pcData;
#else /* HAVE_ICONV_INPUT_CONST */
	char * l_pcIn = const_cast < char * > ( a_pcData );
#endif /* not HAVE_ICONV_INPUT_CONST */
	iconv_t l_xCD = static_cast < iconv_t > ( 0 );
	l_uiOrigSize = l_uiSizeOut = l_uiSizeIn = strlen ( a_pcData );
	f_oConvertedString.hs_realloc ( l_uiOrigSize + 1 );
	l_pcOut = f_oConvertedString.raw ( );
	switch ( a_eWay )
		{
		case ( D_IN ): { l_xCD = f_xIconvIn; break; }
		case ( D_OUT ): { l_xCD = f_xIconvOut; break; }
		default :
			{
			M_THROW ( _ ( "unknown convertion way" ),
					static_cast < int > ( a_eWay ) );
			break;
			}
		}
	M_ENSURE ( ( iconv ( l_xCD, & l_pcIn, & l_uiSizeIn, & l_pcOut,
					& l_uiSizeOut ) != static_cast < size_t > ( - 1 ) )
				|| ( g_iErrNo == E2BIG ) );
	while ( l_uiSizeIn )
		{
		l_uiTmp = l_uiOrigSize;
		l_uiOrigSize <<= 1;
		f_oConvertedString.hs_realloc ( l_uiOrigSize + 1 );
		l_pcOut = f_oConvertedString.raw ( ) + l_uiTmp - l_uiSizeOut;
		l_uiSizeOut += l_uiTmp;
		M_ENSURE ( ( iconv ( l_xCD, & l_pcIn, & l_uiSizeIn, & l_pcOut,
						& l_uiSizeOut ) != static_cast < size_t > ( - 1 ) )
				|| ( g_iErrNo == E2BIG ) );
		}
	if ( l_pcOut )
		( * l_pcOut ) = 0;
	return ( f_oConvertedString );
	M_EPILOG
	}

char const * HXml::get_leaf_by_name ( int a_iIndex, char const * a_pcName )
	{
	M_PROLOG
	if ( f_poXml->f_psNodeSet )
		return ( get_leaf_by_name (
					static_cast < xml_node_ptr_t > (
						f_poXml->f_psNodeSet->nodeTab [ a_iIndex ] ), a_pcName ) );
	return ( NULL );
	M_EPILOG
	}

char const * HXml::get_leaf_by_name ( xml_node_ptr_t a_psNode, char const * a_pcName )
	{
	M_PROLOG
	char const * l_pcData = NULL;
	xmlNodePtr l_psNode = reinterpret_cast < xmlNodePtr > ( a_psNode );
	while ( l_psNode )
		{
		if ( ! xmlStrcasecmp ( l_psNode->name, reinterpret_cast < const xmlChar * > ( a_pcName ) ) )
			return ( convert ( reinterpret_cast < char const * > ( l_psNode->children->content ) ) );
		l_psNode = l_psNode->next;
		}
	l_psNode = reinterpret_cast < xmlNodePtr > ( a_psNode );
	while ( l_psNode )
		{
		if ( l_psNode->children
				&& ( l_pcData = get_leaf_by_name ( l_psNode->children, a_pcName ) ) )
			return ( l_pcData );
		l_psNode = l_psNode->next;
		}
	return ( NULL );
	M_EPILOG
	}

int HXml::get_node_set_by_path ( char const * a_pcPath )
	{
	M_PROLOG
	int l_iLength = 0;
	char * l_pcPtr = NULL;
	f_oVarTmpBuffer = a_pcPath;
	l_pcPtr = f_oVarTmpBuffer.raw ( );
	l_iLength = f_oVarTmpBuffer.get_length ( ) - 1;
	if ( f_poXml->f_psObject )
		f_poXml->xml_free ( f_poXml->f_psObject );
	if ( f_poXml->f_psContext )
		f_poXml->xml_free ( f_poXml->f_psContext );
	f_poXml->reset ( );
	f_poXml->f_psContext = xmlXPathNewContext ( f_poXml->f_psDoc );
	if ( f_poXml->f_psContext )
		{
		while ( l_pcPtr [ 0 ] )
			{
			f_poXml->f_psObject = xmlXPathEvalExpression (
					reinterpret_cast < xmlChar const * > ( l_pcPtr ),
					f_poXml->f_psContext );
			if ( f_poXml->f_psObject )
				break;
			l_pcPtr [ l_iLength -- ] = 0;
			}
		if ( f_poXml->f_psObject )
			{
			f_poXml->f_psNodeSet = f_poXml->f_psObject->nodesetval;
			f_oVarTmpBuffer = a_pcPath;
			return ( f_poXml->f_psNodeSet->nodeNr );
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HXml::init ( char const * a_pcFileName )
	{
	M_PROLOG
	int l_iSavedErrno = g_iErrNo;
	xmlCharEncoding l_xEncoding = static_cast < xmlCharEncoding > ( 0 );
	xmlCharEncodingHandlerPtr l_pxHnd = NULL;
	HString l_oError;
	if ( f_poXml->f_psDoc )
		f_poXml->xml_free ( f_poXml->f_psDoc );
	f_poXml->reset ( );
	g_iErrNo = 0;
	f_poXml->f_psDoc = xmlParseFile ( a_pcFileName );
	if ( g_iErrNo )
		{
		log ( D_LOG_WARNING ) << strerror ( g_iErrNo ) << ": " << a_pcFileName;
		log << ", code: " << g_iErrNo << '.' << endl;
		}
	g_iErrNo = l_iSavedErrno;
	if ( ! f_poXml->f_psDoc )
		{
		l_oError.format ( _ ( "cannot parse `%s'" ), a_pcFileName );
		M_THROW ( l_oError, g_iErrNo );
		}
	f_poXml->f_psRoot = xmlDocGetRootElement ( f_poXml->f_psDoc );
	if ( ! f_poXml->f_psRoot )
		M_THROW ( _ ( "empty doc" ), g_iErrNo );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", f_poXml->f_psRoot->name );
#endif /* __DEBUGGER_BABUNI__ */
	if ( ! f_poXml->f_psDoc->encoding )
		{
		l_oError.format ( _ ( "WARRNING: no encoding declared in `%s'." ),
				a_pcFileName );
		M_LOG ( l_oError );
		}
	else
		l_pxHnd = xmlFindCharEncodingHandler (
				reinterpret_cast < char const * > ( f_poXml->f_psDoc->encoding ) );
	if ( ! l_pxHnd )
		{
		l_xEncoding = xmlDetectCharEncoding ( f_poXml->f_psRoot->name,
				xmlStrlen ( f_poXml->f_psRoot->name ) );
		if ( ! l_xEncoding )
			M_THROW ( _ ( "cannot detect character encoding" ), g_iErrNo );
		l_pxHnd = xmlGetCharEncodingHandler ( l_xEncoding );
		}
	if ( ! l_pxHnd )
		M_THROW ( _ ( "cannot enable internal convertion" ), g_iErrNo );
	f_xIconvIn = l_pxHnd->iconv_in;
	f_xIconvOut = l_pxHnd->iconv_out;
	return;
	M_EPILOG
	}

int HXml::iterate ( ONode & a_rsNode,
		char const * a_pcPath, bool a_bStripEmpty )
	{
	M_PROLOG
	bool l_bOnlyWhite = false;
	bool l_bEmpty = false;
	int l_iCtr = 0;
	int l_iLevel = - 1;
	char const * l_pcName = NULL;
	HString l_oProperty;
	a_rsNode.reset ( );
	if ( ! a_pcPath || ! a_pcPath [ 0 ] )
		a_pcPath = "/"; /* scan full tree */
	if ( ! f_poXml->f_psNodeSet || ( a_pcPath != f_oVarTmpBuffer ) )
		{
		get_node_set_by_path ( a_pcPath );
		f_iIndex = 0;
		f_iLevel = - 2;
		while ( a_pcPath [ l_iCtr ] )
			{
			if ( a_pcPath [ l_iCtr ] == '/' )
				f_iLevel ++;
			l_iCtr ++;
			}
		l_iCtr --;
		M_ASSERT ( l_iCtr >= 0 );
		if ( a_pcPath [ l_iCtr ] != '/' )
			f_iLevel ++;
		}
	while ( ( l_iLevel < 0 ) && ( f_poXml->f_psNodeSet
				&& ( f_iIndex < f_poXml->f_psNodeSet->nodeNr ) ) )
		{
		f_poXml->f_psNodeStart = f_poXml->f_psNodeSet->nodeTab [ f_iIndex ];
		if ( ! f_poXml->f_psNode )
			f_poXml->f_psNode = f_poXml->f_psNodeStart;
		do
			{
			l_iLevel = iterate ( a_rsNode );
			if ( ( l_iLevel < 0 )
					|| ( f_poXml->f_psNode->prev == f_poXml->f_psNodeStart ) )
				{
				f_poXml->f_psNode = NULL;
				f_iIndex ++;
				break;
				}
			l_bOnlyWhite =
				( a_rsNode.f_oContents.find_other_than ( n_pcWhiteSpace ) < 0 );
			l_bEmpty = l_bOnlyWhite && ( ! strcmp ( a_rsNode.f_oName, "text" ) );
			}
		while ( a_bStripEmpty && l_bEmpty );
		if ( ( l_iLevel >= 0 ) && l_bEmpty )
			l_iLevel = - 1;
		}
	if ( a_bStripEmpty && l_bOnlyWhite )
		a_rsNode.f_oContents = "";
	if ( l_iLevel > 0 )
		{
		while ( ( l_pcName = next_property ( l_oProperty ) ) )
			a_rsNode.f_oProperties [ l_pcName ] = l_oProperty;
		l_iLevel = a_rsNode.f_iLevel;
		}
	else
		a_rsNode.f_iLevel = l_iLevel;
	return ( l_iLevel );
	M_EPILOG
	}

char const * HXml::next_property ( HString & a_rsNode )
	{
	M_PROLOG
	char const * l_pcName = NULL;
	xmlNodePtr l_psNode = NULL;
	l_psNode = f_poXml->f_psNode;
	if ( f_poXml->f_psProperties )
		{
		l_pcName = reinterpret_cast < char const * > (
				f_poXml->f_psProperties->name );
		if ( f_poXml->f_psProperties->children )
			a_rsNode = convert (
					reinterpret_cast < char const * > (
						f_poXml->f_psProperties->children->content ) );
		}
	if ( f_poXml->f_psProperties )
		f_poXml->f_psProperties = f_poXml->f_psProperties->next;
	else if ( l_psNode &&  ( l_psNode->type == XML_ATTRIBUTE_NODE ) && l_psNode->properties )
		f_poXml->f_psProperties = l_psNode->properties;
	return ( l_pcName );
	M_EPILOG
	}

int HXml::iterate ( ONode & a_rsNode )
	{
	M_PROLOG
	char const * l_pcName = NULL;
	xmlNodePtr l_psNode = NULL;
	l_psNode = f_poXml->f_psNode;
	if ( l_psNode )
		{
		while ( ! l_psNode->name || ( l_psNode->type == XML_DTD_NODE ) )
			{
			if ( ( l_psNode = f_poXml->next_node ( f_poXml->f_psNode, f_iLevel ) ) )
				f_poXml->f_psNode = l_psNode;
			else break;
			}
		l_pcName = reinterpret_cast < char const * > ( f_poXml->f_psNode->name );
		a_rsNode.f_iLevel = f_iLevel;
		a_rsNode.f_oName = l_pcName;
		if ( l_psNode )
			{
			f_poXml->f_psNode = l_psNode;
			if ( f_poXml->f_psNode->type == XML_ATTRIBUTE_NODE )
				f_poXml->f_psProperties = f_poXml->f_psNode->properties;
			else
				f_poXml->f_psProperties = NULL;
			}
		if ( l_pcName )
			{
			l_psNode = f_poXml->f_psNode;
			while ( ! l_psNode->content )
				{
				if ( l_psNode->children )
					{
					l_psNode = l_psNode->children;
					f_iLevel ++;
					}
				else break;
				}
			if ( l_psNode->content )
				a_rsNode.f_oContents = convert (
						reinterpret_cast < char const * > ( l_psNode->content ) );
			if ( l_psNode )
				f_poXml->f_psNode = l_psNode;
			}
		if ( ( l_psNode = f_poXml->next_node ( f_poXml->f_psNode, f_iLevel ) ) )
			f_poXml->f_psNode = l_psNode;
		else l_pcName = NULL;
		}
	return ( l_pcName ? f_iLevel : - 1 );
	M_EPILOG
	}	


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
#include <new>

#include <libxml/xmlversion.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlmemory.h>
#include <libxml/encoding.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
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
	xmlNodePtr					f_psStartNode;
	/*}*/
private:
	/*{*/
	HXmlData ( void );
	virtual ~HXmlData ( void );
	HXmlData ( const HXmlData & ) __attribute__(( __noreturn__ ));
	HXmlData & operator = ( const HXmlData & ) __attribute__(( __noreturn__ ));
	void xml_free ( xmlDocPtr & );
/*	void xml_free ( xmlNodePtr & ); */
/*	void xml_free ( xmlNodeSetPtr & ); */
	void xml_free ( xmlXPathContextPtr & );
	void xml_free ( xmlXPathObjectPtr & );
	void reset ( void );
	/*}*/
	};

}

}

HXmlData::HXmlData ( void ) : f_psDoc ( NULL ), f_psRoot ( NULL ), f_psNode ( NULL ),
										f_psContext ( NULL ), f_psObject ( NULL ),
										f_psNodeSet ( NULL ), f_psStartNode ( NULL )
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
	f_psStartNode = NULL;
	return;
	M_EPILOG
	}

HXmlData::HXmlData ( const HXmlData & a_roXml ) : f_psDoc ( NULL ),
										f_psRoot ( NULL ), f_psNode ( NULL ),
										f_psContext ( NULL ), f_psObject ( NULL ),
										f_psNodeSet ( NULL ), f_psStartNode ( NULL )
	{
	M_PROLOG
	this->operator = ( a_roXml );
	M_EPILOG
	}

HXmlData & HXmlData::operator = ( const HXmlData & )
	{
	M_PROLOG
	M_THROW ( "This method should not be called.", g_iErrNo );
	M_EPILOG
	}

void HXmlData::xml_free ( xmlDocPtr & a_rpsDoc )
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
	
void HXmlData::xml_free ( xmlXPathContextPtr & a_rpsContext )
	{
	M_PROLOG
	if ( ! a_rpsContext )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeContext ( a_rpsContext );
	a_rpsContext = NULL;
	return;
	M_EPILOG
	}

void HXmlData::xml_free ( xmlXPathObjectPtr & a_rpsObject )
	{
	M_PROLOG
	if ( ! a_rpsObject )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeObject ( a_rpsObject );
	a_rpsObject = NULL;
	return;
	M_EPILOG
	}
	
HXml::HXml ( void )
	: f_iIndex ( 0 ), f_xIconvIn ( static_cast < iconv_t > ( 0 ) ),
	f_xIconvOut ( static_cast < iconv_t > ( 0 ) ),
	f_oConvertedString ( ), f_oTmpBuffer ( ), f_oPath ( ), f_poXml ( NULL )
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
	if ( f_poXml )
		delete f_poXml;
	f_poXml = NULL;
	return;
	M_EPILOG
	}

char * HXml::convert ( const char * a_pcData, way_t a_eWay )
	{
	M_PROLOG
	size_t l_iSizeIn = 0, l_iSizeOut = 0, l_iOrigSize = 0, l_iTmp = 0;
	char * l_pcOut = NULL, * l_pcIn = const_cast < char * > ( a_pcData );
	iconv_t l_xCD = ( iconv_t ) 0;
	l_iOrigSize = l_iSizeOut = l_iSizeIn = strlen ( a_pcData );
	f_oConvertedString.hs_realloc ( l_iOrigSize );
	l_pcOut = f_oConvertedString;
	switch ( a_eWay )
		{
		case ( D_IN ): { l_xCD = f_xIconvIn; break; }
		case ( D_OUT ): { l_xCD = f_xIconvOut; break; }
		default :
			{
			M_THROW ( "unknown convetion way", a_eWay );
			break;
			}
		}
	iconv ( l_xCD, & l_pcIn, & l_iSizeIn, & l_pcOut, & l_iSizeOut );
	while ( l_iSizeIn )
		{
		l_iTmp = l_iOrigSize;
		l_iOrigSize <<= 1;
		f_oConvertedString.hs_realloc ( l_iOrigSize );
		l_pcOut = static_cast < char * > ( f_oConvertedString ) + l_iTmp - l_iSizeOut;
		l_iSizeOut += l_iTmp;
		iconv ( l_xCD, & l_pcIn, & l_iSizeIn, & l_pcOut, & l_iSizeOut );
		}
	if ( l_pcOut )* l_pcOut = 0;
	return ( f_oConvertedString );
	M_EPILOG
	}

char * HXml::get_leaf_by_name ( int a_iIndex, char const * a_pcName )
	{
	M_PROLOG
	if ( f_poXml->f_psNodeSet )
		return ( get_leaf_by_name (
					static_cast < xml_node_ptr_t > (
						f_poXml->f_psNodeSet->nodeTab [ a_iIndex ] ), a_pcName ) );
	return ( NULL );
	M_EPILOG
	}

char * HXml::get_leaf_by_name ( xml_node_ptr_t a_psNode, const char * a_pcName )
	{
	M_PROLOG
	char * l_pcData = NULL;
	xmlNodePtr l_psNode = reinterpret_cast < xmlNodePtr > ( a_psNode );
	while ( l_psNode )
		{
		if ( ! xmlStrcasecmp ( l_psNode->name, reinterpret_cast < const xmlChar * > ( a_pcName ) ) )
			return ( convert ( reinterpret_cast < const char * > ( l_psNode->children->content ) ) );
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

int HXml::get_node_set_by_path ( const char * a_pcPath )
	{
	M_PROLOG
	int l_iLength = 0;
	char * l_pcPtr = NULL;
	f_oTmpBuffer = a_pcPath;
	l_pcPtr = f_oTmpBuffer;
	l_iLength = f_oTmpBuffer.get_length ( ) - 1;
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
					reinterpret_cast < const xmlChar * > ( l_pcPtr ),
					f_poXml->f_psContext );
			M_LOG ( l_pcPtr );
			if ( f_poXml->f_psObject )
				break;
			l_pcPtr [ l_iLength -- ] = 0;
			}
		if ( f_poXml->f_psObject )
			{
			f_poXml->f_psNodeSet = f_poXml->f_psObject->nodesetval;
			return ( f_poXml->f_psNodeSet->nodeNr );
			}
		}
	return ( 0 );
	M_EPILOG
	}

void HXml::init ( const char * a_pcFileName )
	{
	M_PROLOG
	xmlCharEncoding l_xEncoding = static_cast < xmlCharEncoding > ( 0 );
	xmlCharEncodingHandlerPtr l_pxHnd = NULL;
	HString l_oError;
	if ( f_poXml->f_psDoc )
		f_poXml->xml_free ( f_poXml->f_psDoc );
	f_poXml->reset ( );
	f_poXml->f_psDoc = xmlParseFile ( a_pcFileName );
	if ( ! f_poXml->f_psDoc )
		{
		l_oError.format ( "can not parse `%s'", a_pcFileName );
		M_THROW ( l_oError, g_iErrNo );
		}
	f_poXml->f_psRoot = xmlDocGetRootElement ( f_poXml->f_psDoc );
	if ( ! f_poXml->f_psRoot )
		M_THROW ( "empty doc", g_iErrNo );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", f_poXml->f_psRoot->name );
#endif /* __DEBUGGER_BABUNI__ */
	if ( ! f_poXml->f_psDoc->encoding )
		{
		l_oError.format ( "WARRNING: no encoding declared in `%s'.", a_pcFileName );
		M_LOG ( l_oError );
		}
	else l_pxHnd = xmlFindCharEncodingHandler ( reinterpret_cast < const char * > ( f_poXml->f_psDoc->encoding ) );
	if ( ! l_pxHnd )
		{
		l_xEncoding = xmlDetectCharEncoding ( f_poXml->f_psRoot->name,
				xmlStrlen ( f_poXml->f_psRoot->name ) );
		if ( ! l_xEncoding )
			M_THROW ( "can not detect character encoding", g_iErrNo );
		l_pxHnd = xmlGetCharEncodingHandler ( l_xEncoding );
		}
	if ( ! l_pxHnd )
		M_THROW ( "can not enable internal convertion", g_iErrNo );
	f_xIconvIn = l_pxHnd->iconv_in;
	f_xIconvOut = l_pxHnd->iconv_out;
	return;
	M_EPILOG
	}

char const * HXml::iterate ( HString & a_roValue, char const * a_pcPath )
	{
	M_PROLOG
	char const * l_pcName = NULL;
	if ( ! f_poXml->f_psNodeSet || ( a_pcPath != f_oTmpBuffer ) )
		{
		if ( f_poXml->f_psContext )
			f_poXml->xml_free ( f_poXml->f_psContext );
		get_node_set_by_path ( a_pcPath );
		f_iIndex = 0;
		}
	while ( ! l_pcName && ( f_poXml->f_psNodeSet && ( f_iIndex < f_poXml->f_psNodeSet->nodeNr ) ) )
		{
		f_poXml->f_psStartNode = f_poXml->f_psNode = f_poXml->f_psNodeSet->nodeTab [ f_iIndex ];
		l_pcName = iterate ( a_roValue );
		f_iIndex ++;
		}
	return ( l_pcName );
	M_EPILOG
	}

char const * HXml::iterate ( HString & a_roValue )
	{
	M_PROLOG
	char const * l_pcName = NULL;
	if ( f_poXml->f_psNode )
		{
		l_pcName = reinterpret_cast < char const * > ( f_poXml->f_psNode->name );
		a_roValue = f_poXml->f_psNode->content;
		f_poXml->f_psNode = f_poXml->f_psNode->children;
		}
	else
		{
		if ( f_poXml->f_psNode->parent && ( f_poXml->f_psNode->parent != f_poXml->f_psStartNode ) )
			{
			f_poXml->f_psNode = f_poXml->f_psNode->parent->next;
			l_pcName = iterate ( a_roValue );
			}
		}
	return ( l_pcName );
	M_EPILOG
	}	


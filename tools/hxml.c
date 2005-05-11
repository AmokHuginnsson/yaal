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

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hxml.h"
#include "hcore/hlog.h"

using namespace stdhapi::hcore;
using namespace stdhapi::tools;

char free_err [ ] = "trying to free NULL pointer";
char schema_err [ ] = "bad xml schema";

HXml::HXml ( void )
	: f_xIconvIn ( static_cast < iconv_t > ( 0 ) ),
	f_xIconvOut ( static_cast < iconv_t > ( 0 ) ),
	f_oConvertedString ( ),
	f_psDoc ( NULL ), f_psRoot ( NULL ), f_psNode ( NULL ),
	f_psContext ( NULL ), f_psObject ( NULL ), f_psNodeSet ( NULL )
	{
	M_PROLOG
	M_EPILOG
	return;
	}

HXml::~HXml ( void )
	{
	M_PROLOG
	if ( f_psContext )xmlfree ( f_psContext );
	if ( f_psNodeSet )xmlfree ( f_psNodeSet );
	if ( f_psObject )xmlfree ( f_psObject );
	if ( f_psNode )xmlfree ( f_psNode );
	if ( f_psDoc )xmlfree ( f_psDoc );
	M_EPILOG
	return;
	}

void HXml::xmlfree ( xmlDocPtr & a_rpsDoc )
	{
	M_PROLOG
	if ( ! a_rpsDoc )
		M_THROW ( free_err, g_iErrNo );
	xmlFreeDoc ( a_rpsDoc );
	a_rpsDoc = NULL;
	return;
	M_EPILOG
	}

void HXml::xmlfree ( xmlNodePtr & a_rpsNode )
	{
	M_PROLOG
	if ( ! a_rpsNode )
		M_THROW ( free_err, g_iErrNo );
	xmlFreeNode ( a_rpsNode );
	a_rpsNode = NULL;
	return;
	M_EPILOG
	}

void HXml::xmlfree ( xmlNodeSetPtr & a_rpsNodeSet )
	{
	M_PROLOG
	if ( ! a_rpsNodeSet )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeNodeSet ( a_rpsNodeSet );
	a_rpsNodeSet = NULL;
	return;
	M_EPILOG
	}

void HXml::xmlfree ( xmlXPathContextPtr & a_rpsContext )
	{
	M_PROLOG
	if ( ! a_rpsContext )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeContext ( a_rpsContext );
	a_rpsContext = NULL;
	return;
	M_EPILOG
	}

void HXml::xmlfree ( xmlXPathObjectPtr & a_rpsObject )
	{
	M_PROLOG
	if ( ! a_rpsObject )
		M_THROW ( free_err, g_iErrNo );
	xmlXPathFreeObject ( a_rpsObject );
	a_rpsObject = NULL;
	return;
	M_EPILOG
	}

char * HXml::convert ( const xmlChar * a_pxData, way_t a_eWay )
	{
	M_PROLOG
	return ( convert ( a_pxData, a_eWay ) );
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

char * HXml::get_leaf_by_name ( xmlNodePtr a_psNode, const char * a_pcName )
	{
	M_PROLOG
	char * l_pcData = NULL;
	xmlNodePtr l_psNode = a_psNode;
	while ( l_psNode )
		{
		if ( ! xmlStrcasecmp ( l_psNode->name, reinterpret_cast < const xmlChar * > ( a_pcName ) ) )
			return ( convert ( l_psNode->children->content ) );
		l_psNode = l_psNode->next;
		}
	l_psNode = a_psNode;
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

xmlNodeSetPtr HXml::get_node_set_by_path ( const char * a_pcPath )
	{
	M_PROLOG
	if ( f_psObject )xmlfree ( f_psObject );
	if ( f_psContext )xmlfree ( f_psContext );
	f_psContext = xmlXPathNewContext ( f_psDoc );
	f_psObject = xmlXPathEvalExpression ( reinterpret_cast < const xmlChar * > ( a_pcPath ), f_psContext );
	return ( f_psObject->nodesetval );
	M_EPILOG
	}

void HXml::init ( const char * a_pcFileName )
	{
	M_PROLOG
	xmlCharEncoding l_xEncoding = ( xmlCharEncoding ) 0;
	xmlCharEncodingHandlerPtr l_pxHnd = NULL;
	HString l_oError;
	f_psDoc = xmlParseFile ( a_pcFileName );
	if ( ! f_psDoc )
		{
		l_oError.format ( "can not parse `%s'", a_pcFileName );
		M_THROW ( l_oError, g_iErrNo );
		}
	f_psRoot = xmlDocGetRootElement ( f_psDoc );
	if ( ! f_psRoot )
		M_THROW ( "empty doc", g_iErrNo );
#ifdef __DEBUGGER_BABUNI__
	fprintf ( stdout, "%s\n", f_psRoot->name );
#endif /* __DEBUGGER_BABUNI__ */
	if ( ! f_psDoc->encoding )
		{
		l_oError.format ( "WARRNING: no encoding declared in `%s'.", a_pcFileName );
		M_LOG ( l_oError );
		}
	else l_pxHnd = xmlFindCharEncodingHandler ( reinterpret_cast < const char * > ( f_psDoc->encoding ) );
	if ( ! l_pxHnd )
		{
		l_xEncoding = xmlDetectCharEncoding ( f_psRoot->name,
				xmlStrlen ( f_psRoot->name ) );
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

bool is_alpha ( char ch )
	{
	if ( ( ch >= '0' ) && ( ch <= '9' ) )return ( true );
	if ( ( ch >= 'a' ) && ( ch <= 'z' ) )return ( true );
	if ( ( ch >= 'A' ) && ( ch <= 'Z' ) )return ( true );
	return ( false );
	}


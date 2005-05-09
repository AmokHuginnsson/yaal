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

#ifndef __TOOLS_HXML_H
#define __TOOLS_HXML_H

#include <iconv.h>
#include <libxml/xmlversion.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlmemory.h>
#include <libxml/encoding.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#include "hcore/hstring.h"

namespace stdhapi
{

namespace tools
{

class HXml
	{
protected:
	/*{*/
	typedef enum { D_IN, D_OUT } way_t;
	iconv_t									f_xIconvIn;
	iconv_t									f_xIconvOut;
	stdhapi::hcore::HString	f_oConvertedString;
	xmlDocPtr								f_psDoc;
	xmlNodePtr							f_psRoot;
	xmlNodePtr							f_psNode;
	xmlXPathContextPtr			f_psContext;
	xmlXPathObjectPtr				f_psObject;
	xmlNodeSetPtr						f_psNodeSet;
	/*}*/
public:
	/*{*/
	HXml ( void );
	virtual ~ HXml ( void );
	void init ( const char * );
	virtual void * parse ( void * ) = 0;
	char * convert ( const char *, way_t = D_OUT );
	char * convert ( const xmlChar *, way_t = D_OUT );
	char * get_leaf_by_name ( xmlNodePtr, const char * );
	xmlNodeSetPtr get_node_set_by_path ( const char * );
	/*}*/
protected:
	/*{*/
	void xmlfree ( xmlDocPtr & );
	void xmlfree ( xmlNodePtr & );
	void xmlfree ( xmlNodeSetPtr & );
	void xmlfree ( xmlXPathContextPtr & );
	void xmlfree ( xmlXPathObjectPtr & );
	/*}*/
private:
	/*{*/
	HXml ( const HXml & );
	HXml & operator = ( const HXml & );
	/*}*/
	};

}

}

#endif /* not __TOOLS_HXML_H */


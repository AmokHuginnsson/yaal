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

#include "hcore/hstring.h"

namespace stdhapi
{

namespace tools
{

class HXmlData;

class HXml
	{
protected:
	/*{*/
	typedef void * xml_node_ptr_t;
	typedef void * xml_node_set_ptr_t;
	typedef enum { D_IN, D_OUT } way_t;
	iconv_t									f_xIconvIn;
	iconv_t									f_xIconvOut;
	stdhapi::hcore::HString	f_oConvertedString;
	stdhapi::hcore::HString	f_oTmpBuffer;
	HXmlData *							f_poXml;
	/*}*/
public:
	/*{*/
	HXml ( void );
	virtual ~ HXml ( void );
	void init ( const char * );
	virtual void * parse ( void * ) = 0;
	char * convert ( const char *, way_t = D_OUT );
	char * get_leaf_by_name ( xml_node_ptr_t, const char * );
	xml_node_set_ptr_t get_node_set_by_path ( const char * );
	/*}*/
protected:
	/*{*/
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


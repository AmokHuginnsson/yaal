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

#include "hcore/hstring.h"
#include "hcore/hmap.h"
#include "hcore/hlist.h"
#include "hcore/hpointer.h"

namespace stdhapi
{

namespace tools
{

static int const D_HXML_PROPERTIES_MAP_SIZE = 16;
	
class HXmlData;

class HXml
	{
public:
	struct ONode
		{
		typedef enum
			{
			D_NODE,
			D_CONTENT
			} type_t;
		int f_iLevel;
		stdhapi::hcore::HString	f_oName;
		stdhapi::hcore::HList < stdhapi::hcore::HString > f_oContents;
		stdhapi::hcore::HMap < stdhapi::hcore::HString,
			stdhapi::hcore::HString > f_oProperties;
		stdhapi::hcore::HList < ONode > f_oChilds;
		stdhapi::hcore::HList < type_t > f_oTypes;
		ONode ( void ) : f_iLevel ( - 1 ), f_oName ( ), f_oContents ( ),
								f_oProperties ( D_HXML_PROPERTIES_MAP_SIZE ), f_oChilds ( ), f_oTypes ( ) { }
		void reset ( void )
			{
			M_PROLOG
			f_iLevel = - 1;
			f_oName = "";
			f_oContents.flush ( );
			f_oProperties.flush ( );
			f_oChilds.flush ( );
			f_oTypes.flush ( );
			return;
			M_EPILOG
			}
		};
protected:
	/*{*/
	struct OConvert;
	typedef void * xml_node_ptr_t;
	typedef enum { D_IN, D_OUT } way_t;
	stdhapi::hcore::HPointer<OConvert> f_oConvert;
	stdhapi::hcore::HString	f_oConvertedString;
	stdhapi::hcore::HString	f_oVarTmpBuffer;
	HXmlData *							f_poXml;
	ONode										f_oRoot;
	/*}*/
public:
	/*{*/
	HXml ( void );
	virtual ~ HXml ( void );
	void init ( char const * );
	ONode & parse ( char const * const = NULL, bool = true );
	ONode & get_root ( void );
	/*}*/
protected:
	/*{*/
	void parse ( xml_node_ptr_t, ONode &, int, bool );
	char const * convert ( char const *, way_t = D_OUT );
	int get_node_set_by_path ( char const * );
	/*}*/
private:
	/*{*/
	HXml ( HXml const & );
	HXml & operator = ( HXml const & );
	/*}*/
	};

}

}

#endif /* not __TOOLS_HXML_H */


/*
---        `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

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

#ifndef __YAAL_TOOLS_HXML_H
#define __YAAL_TOOLS_HXML_H

#include "hcore/hstring.h"
#include "hcore/hhashmap.h"
#include "hcore/hlist.h"
#include "hcore/hpointer.h"

namespace yaal
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
		yaal::hcore::HString	f_oName;
		yaal::hcore::HList < yaal::hcore::HString > f_oContents;
		yaal::hcore::HHashMap < yaal::hcore::HString,
			yaal::hcore::HString > f_oProperties;
		yaal::hcore::HList < ONode > f_oChilds;
		yaal::hcore::HList < type_t > f_oTypes;
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
	struct OConvert;
	typedef void * xml_node_ptr_t;
	typedef enum { D_IN, D_OUT } way_t;
	yaal::hcore::HPointer<OConvert> f_oConvert;
	yaal::hcore::HString	f_oConvertedString;
	yaal::hcore::HString	f_oVarTmpBuffer;
	HXmlData *							f_poXml;
	ONode										f_oRoot;
public:
	HXml ( void );
	virtual ~ HXml ( void );
	void init ( char const * );
	ONode & parse ( char const * const = NULL, bool = true );
	ONode & get_root ( void );
protected:
	void parse ( xml_node_ptr_t, ONode &, int, bool );
	char const * convert ( char const *, way_t = D_OUT );
	int get_node_set_by_path ( char const * );
private:
	HXml ( HXml const & );
	HXml & operator = ( HXml const & );
	};

}

}

#endif /* not __YAAL_TOOLS_HXML_H */


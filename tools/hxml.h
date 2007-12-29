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

#ifndef __YAAL_TOOLS_HXML_H
#define __YAAL_TOOLS_HXML_H

#include "hcore/hstring.h"
#include "hcore/hmap.h"
#include "hcore/hlist.h"
#include "hcore/htree.h"
#include "hcore/hpointer.h"

namespace yaal
{

namespace tools
{

class HXmlData;

class HXml
	{
public:
	struct OXMLElement
		{
		typedef enum
			{
			D_NODE,
			D_CONTENT
			} type_t;
		yaal::hcore::HString f_oName;
		yaal::hcore::HString f_oContents;
		typedef yaal::hcore::HMap<yaal::hcore::HString, yaal::hcore::HString> properties_t;
		properties_t f_oProperties;
		OXMLElement( void ) : f_oName(), f_oContents(), f_oProperties() { }
		OXMLElement( OXMLElement const& source ) : f_oName( source.f_oName ),
			f_oContents( source.f_oContents ), f_oProperties()
			{ f_oProperties.copy_from( source.f_oProperties ); }
		OXMLElement& operator = ( OXMLElement const& source )
			{
			M_PROLOG
			if ( &source != this )
				{
				f_oName = source.f_oName;
				f_oContents = source.f_oContents;
				f_oProperties.copy_from( source.f_oProperties );
				}
			return ( *this );
			M_EPILOG
			}
		void reset( void )
			{
			M_PROLOG
			f_oName.clear();
			f_oContents.clear();
			f_oProperties.clear();
			return;
			M_EPILOG
			}
		};
protected:
	struct OConvert;
	typedef void* xml_node_ptr_t;
	typedef enum { D_IN, D_OUT } way_t;
	typedef yaal::hcore::HTree<OXMLElement> tree_t;
	yaal::hcore::HPointer<OConvert> f_oConvert;
	yaal::hcore::HString	f_oConvertedString;
	yaal::hcore::HString	f_oVarTmpBuffer;
	HXmlData*							f_poXml;
	tree_t f_oDOM;
public:
	typedef tree_t::node_t xml_element_t;
	typedef tree_t::const_node_t const_xml_element_t;
	HXml( void );
	virtual ~ HXml( void );
	void init( char const* );
	tree_t::node_t parse( char const* const = NULL, bool = true );
	tree_t::node_t get_root( void );
protected:
	void parse ( xml_node_ptr_t, tree_t::node_t, bool );
#ifdef HAVE_ICONV_INPUT_CONST
#	define D_YAAL_TOOLS_HXML_ICONV_CONST const
#else /* HAVE_ICONV_INPUT_CONST */
#	define D_YAAL_TOOLS_HXML_ICONV_CONST /**/
#endif /* not HAVE_ICONV_INPUT_CONST */
	char const* convert( char D_YAAL_TOOLS_HXML_ICONV_CONST*, way_t = D_OUT );
#undef D_YAAL_TOOLS_HXML_ICONV_CONST
	int get_node_set_by_path( char const* );
private:
	HXml( HXml const& );
	HXml& operator = ( HXml const& );
	};

}

}

#endif /* not __YAAL_TOOLS_HXML_H */


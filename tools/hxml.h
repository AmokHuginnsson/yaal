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
	class HNode;
	class HNodeProxy;
	class HIterator;
	typedef HIterator iterator;
	typedef yaal::hcore::HTree<HNode> tree_t;
protected:
	struct OConvert;
	typedef void* xml_node_ptr_t;
	typedef enum { D_TO_EXTERNAL, D_TO_INTERNAL } way_t;
	yaal::hcore::HPointer<OConvert> f_oConvert;
	yaal::hcore::HString	f_oConvertedString;
	yaal::hcore::HString	f_oVarTmpBuffer;
	yaal::hcore::HString	f_oEncoding;
	HXmlData*							f_poXml;
	tree_t f_oDOM;
public:
	typedef tree_t::node_t xml_element_t;
	typedef tree_t::const_node_t const_xml_element_t;
	HXml( void );
	virtual ~ HXml( void );
	void init( char const* );
	void apply_style( char const* const );
	void parse( char const* const = NULL, bool = true );
	HNodeProxy get_root( void );
	HNodeProxy const get_root( void ) const;
	void load( char const* const );
	void save( char const* const );
	void create_root( char const* const, char const* const = NULL );
	void clear( void );
private:
	void parse ( xml_node_ptr_t, tree_t::node_t, bool );
	void dump_node( void*, HNodeProxy const& );
	char const* convert( char const*, way_t = D_TO_INTERNAL );
	int get_node_set_by_path( char const* );
private:
	HXml( HXml const& );
	HXml& operator = ( HXml const& );
	};

class HXml::HNode
	{
public:
	struct TYPE
		{
		typedef enum
			{
			D_NODE,
			D_CONTENT
			} type_t;
		};
	typedef yaal::hcore::HMap<yaal::hcore::HString, yaal::hcore::HString> properties_t;
private:
	TYPE::type_t f_eType;
	yaal::hcore::HString f_oText;
	properties_t f_oProperties;
public:
	HNode( void ) : f_eType( TYPE::D_NODE ), f_oText(), f_oProperties() { }
	HNode( TYPE::type_t const& type, yaal::hcore::HString const& value ) : f_eType( type ), f_oText( value ), f_oProperties() {}
	HNode( HNode const& source ) : f_eType( source.f_eType ), f_oText( source.f_oText ), f_oProperties()
		{ f_oProperties.copy_from( source.f_oProperties ); }
	HNode& operator = ( HNode const& source )
		{
		M_PROLOG
		if ( &source != this )
			{
			f_eType = source.f_eType;
			f_oText = source.f_oText;
			f_oProperties.copy_from( source.f_oProperties );
			}
		return ( *this );
		M_EPILOG
		}
private:
	void clear( void )
		{
		M_PROLOG
		f_oText.clear();
		f_oProperties.clear();
		return;
		M_EPILOG
		}
	friend class HXml;
	};

class HXml::HNodeProxy
	{
	mutable HXml::tree_t::node_t f_poNode;
public:
	HXml::HIterator begin();
	HXml::HIterator const begin() const;
	HXml::HIterator end();
	HXml::HIterator const end() const;
	HXml::HIterator rbegin();
	HXml::HIterator const rbegin() const;
	HXml::HIterator rend();
	HXml::HIterator const rend() const;
	HXml::HIterator get_element_by_id( char const* const );
	HXml::HIterator const get_element_by_id( char const* const ) const;
	HXml::HIterator query( char const* const );
	HXml::HIterator const query( char const* const ) const;
	HXml::HNode::TYPE::type_t get_type() const;
	bool has_childs( void ) const;
	int child_count( void ) const;
	int get_level( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	yaal::hcore::HString const& get_value( void ) const;
	void set_name( char const* const );
	void set_value( char const* const );
	HXml::HNode::properties_t& properties( void );
	HXml::HNode::properties_t const& properties( void ) const;
	HXml::HIterator remove_node( HXml::HIterator );
	HXml::HIterator add_node( HXml::HNode::TYPE::type_t const&, char const* const );
	HXml::HIterator insert_node( HXml::HIterator );
	HNodeProxy( HNodeProxy const& );
	HNodeProxy& operator = ( HNodeProxy const& );
	bool operator ! ( void ) const;
private:
	friend class HXml;
	friend class HXml::HIterator;
	HNodeProxy( HXml::tree_t::const_node_t );
	};

class HXml::HIterator
	{
	HXml::HNodeProxy const* f_poOwner;
	mutable HXml::tree_t::iterator f_oIterator;
	HXml::HNodeProxy f_oProxy;
public:
	HIterator( void );
	HIterator( HIterator const& );
	HIterator& operator ++ ( void )
		{
		++ f_oIterator;
		return ( *this );
		}
	HIterator operator ++ ( int )
		{
		HIterator it( *this );
		++ f_oIterator;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- f_oIterator;
		return ( *this );
		}
	HIterator operator -- ( int )
		{
		HIterator it( *this );
		-- f_oIterator;
		return ( it );
		}
	HIterator& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	HXml::HNodeProxy& operator* ( void );
	HXml::HNodeProxy const& operator* ( void ) const;
	HXml::HNodeProxy* operator->( void );
	HXml::HNodeProxy const* operator->( void ) const;
private:
	friend class HXml::HNodeProxy;
	HIterator( HXml::HNodeProxy const*, HXml::tree_t::iterator const& );
	};

typedef yaal::hcore::HExceptionT<HXml> HXmlException;

}

}

#endif /* not __YAAL_TOOLS_HXML_H */


/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

	hxml.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file tools/hxml.hxx
 * \brief Declaration of XML related classes.
 *
 * HXml, HXml::HNodeProxy, HXml::HConstNodeProxy, HXml::HIterator, HXml::HConstIterator.
 */

#ifndef YAAL_TOOLS_HXML_HXX_INCLUDED
#define YAAL_TOOLS_HXML_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/htree.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

class HXmlData;

/*! \brief XML document operation wrapper.
 */
class HXml
	{
	typedef HXml self_t;
public:
	class HNode;
	class HNodeProxy;
	class HConstNodeProxy;
	class HIterator;
	class HConstIterator;
	typedef HIterator iterator;
	typedef HConstIterator const_iterator;
	typedef yaal::hcore::HTree<HNode> tree_t;
protected:
	struct OConvert;
	typedef void* xml_node_ptr_t;
	typedef enum { D_TO_EXTERNAL, D_TO_INTERNAL } way_t;
	mutable yaal::hcore::HPointer<OConvert> f_oConvert;
	mutable yaal::hcore::HString	f_oConvertedString;
	yaal::hcore::HString	f_oVarTmpBuffer;
	yaal::hcore::HString	f_oEncoding;
	HXmlData*							f_poXml;
	tree_t f_oDOM;
public:
	typedef tree_t::node_t xml_element_t;
	typedef tree_t::const_node_t const_xml_element_t;
	HXml( void );
	virtual ~HXml( void );
	void init( yaal::hcore::HStreamInterface& );
	void init( yaal::hcore::HStreamInterface::ptr_t );
	void apply_style( yaal::hcore::HString const& );
	void parse( yaal::hcore::HString = yaal::hcore::HString(), bool = true );
	HNodeProxy get_root( void );
	HConstNodeProxy const get_root( void ) const;
	void load( yaal::hcore::HStreamInterface& );
	void load( yaal::hcore::HStreamInterface::ptr_t );
	void save( yaal::hcore::HStreamInterface& ) const;
	void save( yaal::hcore::HStreamInterface::ptr_t ) const;
	void create_root( yaal::hcore::HString const&, yaal::hcore::HString const& = yaal::hcore::HString() );
	void clear( void );
	HNodeProxy get_element_by_id( yaal::hcore::HString const& );
	HConstNodeProxy const get_element_by_id( yaal::hcore::HString const& ) const;
	HNodeProxy get_element_by_path( yaal::hcore::HString const& );
	HConstNodeProxy const get_element_by_path( yaal::hcore::HString const& ) const;
private:
	static int writer_callback( void*, char const*, int );
	static int reader_callback( void*, char*, int );
	void do_save( void ) const;
	void parse( xml_node_ptr_t, tree_t::node_t, bool );
	void dump_node( void*, HConstNodeProxy const& ) const;
	yaal::hcore::HString const& convert( yaal::hcore::HString const&, way_t = D_TO_INTERNAL ) const;
	int get_node_set_by_path( yaal::hcore::HString const& );
	const_xml_element_t get_element_by_id( const_xml_element_t const&, yaal::hcore::HString const& ) const;
	const_xml_element_t get_element_by_path( const_xml_element_t const&, yaal::hcore::HString const&, int const& ) const;
private:
	HXml( HXml const& );
	HXml& operator = ( HXml const& );
	};

/*! \brief Basic building block of XML document.
 */
class HXml::HNode
	{
public:
	/*! \brief XML node types.
	 */
	struct TYPE
		{
		/*! \brief XML node types.
		 */
		typedef enum
			{
			D_NODE,   /*!< XML node. */
			D_CONTENT /*!< XML node content. */
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
	friend class HXml::HNodeProxy;
	friend class HXml::HConstNodeProxy;
	};

/*! \brief Immutable XML document node.
 */
class HXml::HConstNodeProxy
	{
	HXml::tree_t::const_node_t f_poNode;
public:
	HXml::HConstIterator const begin() const;
	HXml::HConstIterator const end() const;
	HXml::HConstIterator const rbegin() const;
	HXml::HConstIterator const rend() const;
	HXml::HConstIterator const get_element_by_id( yaal::hcore::HString const& ) const;
	HXml::HConstIterator const query( yaal::hcore::HString const& ) const;
	HXml::HNode::TYPE::type_t get_type() const;
	bool has_childs( void ) const;
	int long child_count( void ) const;
	int get_level( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	yaal::hcore::HString const& get_value( void ) const;
	HXml::HNode::properties_t const& properties( void ) const;
	HConstNodeProxy( void );
	HConstNodeProxy( HConstNodeProxy const& );
	HConstNodeProxy( HNodeProxy const& );
	HConstNodeProxy& operator = ( HConstNodeProxy const& );
	bool operator ! ( void ) const;
private:
	friend class HXml;
	friend class HXml::HConstIterator;
	HConstNodeProxy( HXml::tree_t::const_node_t );
	};

/*! \brief XML document node.
 */
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
	HXml::HIterator query( yaal::hcore::HString const& );
	HXml::HIterator const query( yaal::hcore::HString const& ) const;
	HXml::HNode::TYPE::type_t get_type() const;
	bool has_childs( void ) const;
	int long child_count( void ) const;
	int get_level( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	yaal::hcore::HString const& get_value( void ) const;
	void set_name( yaal::hcore::HString const& );
	void set_value( yaal::hcore::HString const& );
	HXml::HNode::properties_t& properties( void );
	HXml::HNode::properties_t const& properties( void ) const;
	HXml::HIterator remove_node( HXml::HIterator );
	HXml::HIterator replace_node( HXml::HIterator, HXml::HNodeProxy );
	HXml::HIterator add_node( HXml::HNode::TYPE::type_t const&, yaal::hcore::HString const& );
	HXml::HIterator add_node( yaal::hcore::HString const&, yaal::hcore::HString const& = yaal::hcore::HString() );
	HXml::HIterator insert_node( HXml::HIterator, HXml::HNode::TYPE::type_t const&, yaal::hcore::HString const& );
	HXml::HIterator insert_node( HXml::HIterator, yaal::hcore::HString const&, yaal::hcore::HString const& = yaal::hcore::HString() );
	HXml::HIterator move_node( HXml::HIterator, HXml::HNodeProxy );
	HXml::HIterator move_node( HXml::HNodeProxy );
	HXml::HIterator copy_node( HXml::HIterator, HXml::HNodeProxy );
	HXml::HIterator copy_node( HXml::HNodeProxy );
	HNodeProxy( void );
	HNodeProxy( HNodeProxy const& );
	HNodeProxy& operator = ( HNodeProxy const& );
	bool operator ! ( void ) const;
private:
	friend class HXml;
	friend class HXml::HIterator;
	friend class HXml::HConstNodeProxy;
	HNodeProxy( HXml::tree_t::node_t );
	};

/*! \brief XML node iterator.
 */
class HXml::HIterator
	{
	HXml::HNodeProxy const* f_poOwner;
	mutable HXml::tree_t::iterator f_oIterator;
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
	HXml::HNodeProxy operator* ( void );
	HXml::HConstNodeProxy const operator* ( void ) const;
private:
	friend class HXml::HNodeProxy;
	HIterator( HXml::HNodeProxy const*, HXml::tree_t::iterator const& );
	};

/*! \brief XML immutable node iterator.
 */
class HXml::HConstIterator
	{
	HXml::HConstNodeProxy const* f_poOwner;
	mutable HXml::tree_t::const_iterator f_oIterator;
public:
	HConstIterator( void );
	HConstIterator( HConstIterator const& );
	HConstIterator& operator ++ ( void )
		{
		++ f_oIterator;
		return ( *this );
		}
	HConstIterator operator ++ ( int )
		{
		HConstIterator it( *this );
		++ f_oIterator;
		return ( it );
		}
	HConstIterator& operator -- ( void )
		{
		-- f_oIterator;
		return ( *this );
		}
	HConstIterator operator -- ( int )
		{
		HConstIterator it( *this );
		-- f_oIterator;
		return ( it );
		}
	HConstIterator& operator = ( HConstIterator const& );
	bool operator == ( HConstIterator const& ) const;
	bool operator != ( HConstIterator const& ) const;
	HXml::HConstNodeProxy const operator* ( void ) const;
private:
	friend class HXml::HConstNodeProxy;
	HConstIterator( HXml::HConstNodeProxy const*, HXml::tree_t::const_iterator const& );
	};

typedef yaal::hcore::HExceptionT<HXml> HXmlException;

/*! \brief HXml related convenience and helper functions.
 */
namespace xml
{

char const* node_val( HXml::HConstNodeProxy const& node );
char const* node_val( HXml::HConstIterator const& it );
char const* attr_val( HXml::HConstNodeProxy const& node, char const* const name );
char const* attr_val( HXml::HConstIterator const& it, char const* const name );

}

}

}

#endif /* not YAAL_TOOLS_HXML_HXX_INCLUDED */


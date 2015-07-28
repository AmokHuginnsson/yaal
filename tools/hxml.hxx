/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

  hxml.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
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
#define YAAL_TOOLS_HXML_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/hbitflag.hxx"
#include "hcore/hlist.hxx"
#include "hcore/htree.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/htokenizer.hxx"
#include "tools/hoptional.hxx"

namespace yaal {

namespace tools {

class HXmlData;

/*! \brief XML document operation wrapper.
 */
class HXml {
public:
	typedef HXml this_type;
	class HNode;
	class HNodeProxy;
	class HConstNodeProxy;
	class HIterator;
	class HConstIterator;
	class HNodeSet;
	class HConstNodeSet;
	class HNameSpace;
	typedef HIterator iterator;
	typedef HConstIterator const_iterator;
	typedef yaal::hcore::HTree<HNode> tree_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, yaal::hcore::HString> entities_t;
	typedef yaal::hcore::HPair<yaal::hcore::HString, yaal::hcore::HString> parameter_t;
	typedef yaal::hcore::HArray<parameter_t> parameters_t;
	typedef yaal::hcore::HHashSet<HNameSpace> namespaces_t;
	typedef entities_t::const_iterator const_entity_iterator;
	typedef entities_t::iterator entity_iterator;
	struct PARSER;
	typedef yaal::hcore::HBitFlag<PARSER> parser_t;
	struct PARSER {
		static M_YAAL_TOOLS_PUBLIC_API parser_t const DEFAULT;
		static M_YAAL_TOOLS_PUBLIC_API parser_t const KEEP_EMPTY;
		static M_YAAL_TOOLS_PUBLIC_API parser_t const STRIP_COMMENT;
		static M_YAAL_TOOLS_PUBLIC_API parser_t const RESOLVE_ENTITIES;
		static M_YAAL_TOOLS_PUBLIC_API parser_t const AUTO_XINCLUDE;
	};
private:
	struct OConvert;
	typedef void* xml_node_ptr_t;
	typedef enum { TO_EXTERNAL, TO_INTERNAL } way_t;
	typedef yaal::hcore::HResource<HXmlData> xml_low_t;
	mutable yaal::hcore::HPointer<OConvert> _convert;
	mutable yaal::hcore::HString _convertedString;
	yaal::hcore::HString _varTmpBuffer;
	yaal::hcore::HString _encoding;
	yaal::hcore::HString _streamId;
	xml_low_t _xml;
	entities_t _entities;
	namespaces_t _namespaces;
	tree_t _domTree;
public:
	typedef tree_t::node_t xml_element_t;
	typedef tree_t::const_node_t const_xml_element_t;
	HXml( void );
	virtual ~HXml( void );
	HXml( HXml const& );
	void swap( HXml& );
	HXml& operator = ( HXml const& );
	void init( yaal::hcore::HStreamInterface&, parser_t = PARSER::DEFAULT );
	void init( yaal::hcore::HStreamInterface::ptr_t, parser_t = PARSER::DEFAULT );
	void apply_style( yaal::hcore::HString const&, parameters_t const& = parameters_t() );
	void parse( void );
	void parse( yaal::hcore::HString const& );
	void parse( parser_t );
	void parse( yaal::hcore::HString const&, parser_t );
	HNodeProxy get_root( void );
	HConstNodeProxy const get_root( void ) const;
	void load( yaal::hcore::HStreamInterface&, parser_t = PARSER::DEFAULT );
	void load( yaal::hcore::HStreamInterface::ptr_t, parser_t = PARSER::DEFAULT );
	void save( yaal::hcore::HStreamInterface&, bool = false ) const;
	void save( yaal::hcore::HStreamInterface::ptr_t, bool = false ) const;
	void create_root( yaal::hcore::HString const&, yaal::hcore::HString const& = yaal::hcore::HString() );
	void clear( void );
	HConstNodeProxy get_element_by_id( yaal::hcore::HString const& ) const;
	HNodeProxy get_element_by_id( yaal::hcore::HString const& );
	HConstNodeSet get_elements_by_path( yaal::hcore::HString const& ) const;
	HNodeSet get_elements_by_path( yaal::hcore::HString const& );
	HConstNodeSet get_elements_by_name( yaal::hcore::HString const& ) const;
	HNodeSet get_elements_by_name( yaal::hcore::HString const& );
	const_entity_iterator entity_begin( void ) const;
	const_entity_iterator entity_end( void ) const;
	entity_iterator entity_begin( void );
	entity_iterator entity_end( void );
private:
	static int writer_callback( void*, char const*, int );
	static int reader_callback( void*, char*, int );
	void generate_intermediate_form( bool ) const;
	void parse( xml_node_ptr_t, tree_t::node_t, parser_t );
	void dump_node( void*, HConstNodeProxy const& ) const;
	yaal::hcore::HString const& convert( yaal::hcore::HString const&, way_t = TO_INTERNAL ) const;
	int get_node_set_by_path( yaal::hcore::HString const& );
	const_xml_element_t get_element_by_id( const_xml_element_t const&, yaal::hcore::HString const& ) const;
	void get_elements_by_path( HConstNodeSet&, const_xml_element_t, yaal::hcore::HTokenizer const&, yaal::hcore::HTokenizer::iterator ) const;
	void get_elements_by_name( HConstNodeSet&, const_xml_element_t, yaal::hcore::HString const& ) const;
	void parse_dtd( void* );
	char const* error_message( int ) const;
};

typedef yaal::hcore::HExceptionT<HXml> HXmlException;

/*! \brief Basic building block of XML document.
 */
class HXml::HNode {
public:
	/*! \brief XML node types.
	 */
	struct TYPE {
		/*! \brief XML node types.
		 */
		typedef enum {
			NODE,    /*!< XML node. */
			CONTENT, /*!< XML node content. */
			ENTITY,  /*!< XML entity. */
			COMMENT  /*!< XML comment. */
		} type_t;
	};
	typedef yaal::hcore::HMap<yaal::hcore::HString, yaal::hcore::HString> properties_t;
	typedef yaal::hcore::HArray<HXml::HNameSpace const*> namespace_definitions_t;
private:
	HXml const* _owner;
	TYPE::type_t _type;
	yaal::hcore::HString _text;
	properties_t _properties;
	namespace_definitions_t _namespaceDefinitions;
	int _line;
public:
	HNode( HXml const* owner_, int line_ = 0 )
		: _owner( owner_ ), _type( TYPE::NODE ),
		_text(), _properties(),
		_namespaceDefinitions(), _line( line_ ) {
		return;
	}
	HNode( HXml const* owner_, TYPE::type_t const& type,
			yaal::hcore::HString const& value, int line_ = 0 )
		: _owner( owner_ ), _type( type ),
		_text( value ), _properties(),
		_namespaceDefinitions(), _line( line_ ) {
		return;
	}
	HNode( HNode const& source )
		: _owner( source._owner ), _type( source._type ),
		_text( source._text ), _properties( source._properties ),
		_namespaceDefinitions( source._namespaceDefinitions ), _line( source._line ) {
		return;
	}
	HNode& operator = ( HNode const& source ) {
		M_PROLOG
		if ( &source != this ) {
			HNode tmp( source );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HNode& node_ ) {
		if ( &node_ != this ) {
			using yaal::swap;
			swap( _owner, node_._owner );
			swap( _type, node_._type );
			swap( _text, node_._text );
			swap( _properties, node_._properties );
			swap( _namespaceDefinitions, node_._namespaceDefinitions );
			swap( _line, node_._line );
		}
		return;
	}
private:
	void clear( void ) {
		M_PROLOG
		_text.clear();
		_properties.clear();
		return;
		M_EPILOG
	}
	friend class HXml;
	friend class HXml::HNodeProxy;
	friend class HXml::HConstNodeProxy;
};

/*! \brief Immutable XML document node.
 */
class HXml::HConstNodeProxy {
public:
	typedef HXml::HConstIterator const_iterator;
	typedef HXml::HIterator iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
protected:
	HXml::tree_t::node_t _node;
public:
	const_iterator begin() const;
	const_iterator end() const;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
	HXml const* xml( void ) const;
	HXml::HNode::TYPE::type_t get_type() const;
	bool has_childs( void ) const;
	int long child_count( void ) const;
	int get_level( void ) const;
	int get_line( void ) const;
	yaal::hcore::HString const& get_name( void ) const;
	yaal::hcore::HString const& get_value( void ) const;
	HXml::HNode::properties_t const& properties( void ) const;
	HConstNodeProxy get_parent( void ) const;
	HConstNodeProxy( void );
	virtual ~HConstNodeProxy( void ) {}
	HConstNodeProxy( HConstNodeProxy const& );
	HConstNodeProxy( HNodeProxy const& );
	HConstNodeProxy& operator = ( HConstNodeProxy const& );
	bool operator == ( HConstNodeProxy const& ) const;
	bool operator != ( HConstNodeProxy const& ) const;
	bool operator ! ( void ) const;
private:
	friend class HXml;
	friend class HXml::HConstIterator;
	HConstNodeProxy( HXml::tree_t::const_node_t );
};

/*! \brief XML document node.
 */
class HXml::HNodeProxy : public HXml::HConstNodeProxy {
public:
	typedef HXml::HConstIterator const_iterator;
	typedef HXml::HIterator iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
public:
	iterator begin();
	using HConstNodeProxy::begin;
	iterator end();
	using HConstNodeProxy::end;
	reverse_iterator rbegin();
	using HConstNodeProxy::rbegin;
	reverse_iterator rend();
	using HConstNodeProxy::rend;
	HXml* xml( void );
	using HConstNodeProxy::xml;
	void set_name( yaal::hcore::HString const& );
	void set_value( yaal::hcore::HString const& );
	HXml::HNode::properties_t& properties( void );
	HNodeProxy get_parent( void );
	using HConstNodeProxy::properties;
	using HConstNodeProxy::get_parent;
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
	void reset_owner( HXml const* );
	friend class HXml;
	friend class HXml::HIterator;
	friend class HXml::HConstNodeProxy;
	HNodeProxy( HXml::tree_t::node_t );
};

/*! \brief XML node iterator.
 */
class HXml::HIterator : public yaal::hcore::iterator_interface<HXml::HNodeProxy, yaal::hcore::iterator_category::forward> {
	HXml::tree_t::node_t _owner;
	HXml::tree_t::HNode::iterator _iterator;
public:
	typedef yaal::hcore::iterator_interface<HXml::HNodeProxy, yaal::hcore::iterator_category::forward> base_type;
	HIterator( void );
	HIterator( HIterator const& );
	HIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _iterator;
		return ( it );
	}
	HIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _iterator;
		return ( it );
	}
	HIterator& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	HXml::HNodeProxy operator* ( void );
	HXml::HConstNodeProxy operator* ( void ) const;
private:
	friend class HXml::HNodeProxy;
	HIterator( HXml::tree_t::node_t, HXml::tree_t::HNode::iterator const& );
};

/*! \brief XML immutable node iterator.
 */
class HXml::HConstIterator : public yaal::hcore::iterator_interface<HXml::HConstNodeProxy, yaal::hcore::iterator_category::forward> {
	HXml::tree_t::const_node_t _owner;
	HXml::tree_t::HNode::const_iterator _iterator;
public:
	typedef yaal::hcore::iterator_interface<HXml::HConstNodeProxy, yaal::hcore::iterator_category::forward> base_type;
	HConstIterator( void );
	HConstIterator( HConstIterator const& );
	HConstIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HConstIterator operator ++ ( int ) {
		HConstIterator it( *this );
		++ _iterator;
		return ( it );
	}
	HConstIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HConstIterator operator -- ( int ) {
		HConstIterator it( *this );
		-- _iterator;
		return ( it );
	}
	HConstIterator& operator = ( HConstIterator const& );
	bool operator == ( HConstIterator const& ) const;
	bool operator != ( HConstIterator const& ) const;
	HXml::HConstNodeProxy operator* ( void ) const;
private:
	friend class HXml::HConstNodeProxy;
	HConstIterator( HXml::tree_t::const_node_t, HXml::tree_t::HNode::const_iterator const& );
};

class HXml::HConstNodeSet {
public:
	typedef HConstNodeSet this_type;
	class HConstIterator;
private:
	typedef yaal::hcore::HArray<HXml::const_xml_element_t> nodes_t;
	nodes_t _nodes;
public:
	virtual ~HConstNodeSet( void ) {}
	HConstIterator begin( void ) const;
	HConstIterator end( void ) const;
	int get_size( void ) const;
	int size( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	HConstNodeProxy operator[] ( int ) const;
private:
	HConstNodeSet( void );
	void add( HXml::const_xml_element_t );
	friend class HXml;
};

class HXml::HNodeSet : public HXml::HConstNodeSet {
public:
	typedef HXml::HNodeSet this_type;
	typedef HXml::HConstNodeSet base_type;
	class HIterator;
	virtual ~HNodeSet( void ) {}
	HIterator begin( void );
	HIterator end( void );
	HNodeProxy operator[] ( int );
private:
	HNodeSet( void );
	friend class HXml;
};

class HXml::HConstNodeSet::HConstIterator : public yaal::hcore::iterator_interface<HXml::HConstNodeProxy, yaal::hcore::iterator_category::random_access> {
public:
	typedef HConstIterator this_type;
	typedef yaal::hcore::iterator_interface<HXml::HConstNodeProxy, yaal::hcore::iterator_category::random_access> base_type;
	HXml::HConstNodeSet const* _owner;
	int _iterator;
public:
	HConstIterator( void );
	HConstIterator( HConstIterator const& );
	HConstIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HConstIterator operator ++ ( int ) {
		HConstIterator it( *this );
		++ _iterator;
		return ( it );
	}
	HConstIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HConstIterator operator -- ( int ) {
		HConstIterator it( *this );
		-- _iterator;
		return ( it );
	}
	HConstIterator& operator = ( HConstIterator const& );
	bool operator == ( HConstIterator const& ) const;
	bool operator != ( HConstIterator const& ) const;
	HXml::HConstNodeProxy operator* ( void ) const;
private:
	friend class HXml::HConstNodeSet;
	friend class HXml::HNodeSet::HIterator;
	HConstIterator( HXml::HConstNodeSet const*, int );
};

class HXml::HNodeSet::HIterator : public HXml::HConstNodeSet::HConstIterator {
public:
	typedef HIterator this_type;
	typedef HXml::HConstNodeSet::HConstIterator base_type;
	HXml::HNodeProxy operator* ( void );
private:
	friend class HXml::HNodeSet;
	HIterator( HXml::HConstNodeSet const*, int );
};

/*! \brief HXml related convenience and helper functions.
 */
namespace xml {

typedef HOptional<yaal::hcore::HString const&> value_t;

value_t try_node_val( HXml::HConstNodeProxy const& node );
value_t try_node_val( HXml::HConstIterator const& it );
value_t try_attr_val( HXml::HConstNodeProxy const& node, yaal::hcore::HString const& name );
value_t try_attr_val( HXml::HConstIterator const& it, yaal::hcore::HString const& name );

yaal::hcore::HString const& node_val( HXml::HConstNodeProxy const& node );
yaal::hcore::HString const& node_val( HXml::HConstIterator const& it );
yaal::hcore::HString const& attr_val( HXml::HConstNodeProxy const& node, yaal::hcore::HString const& name );
yaal::hcore::HString const& attr_val( HXml::HConstIterator const& it, yaal::hcore::HString const& name );

}

inline void swap( yaal::tools::HXml& a, yaal::tools::HXml& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_TOOLS_HXML_HXX_INCLUDED */


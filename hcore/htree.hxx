/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htree.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must atttach Copyright information to any part of every copy
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

#ifndef YAAL_HCORE_HTREE_HXX_INCLUDED
#define YAAL_HCORE_HTREE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hlist.hxx"
#include "hcore/functional.hxx"

namespace yaal
{

namespace hcore
{

template<typename value_t>
class HTree;

/*! \brief Tree based data structure and operations.
 */
template<typename type_t>
class HTree
	{
private:
	typedef HTree<type_t> tree_t;
public:
	typedef type_t value_type;
	class HNode;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<HNode> iterator;
	typedef HIterator<HNode const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	HNode* _root;			/* self explanary */
public:
	HTree( void );
	HTree( HTree const& );
	HTree& operator = ( HTree const& );
	virtual ~HTree();
	node_t get_root( void );
	const_node_t get_root( void ) const;
	node_t create_new_root( void );
	node_t set_new_root( HNode* );
	void clear( void );
	void swap( HTree<value_type>& );
private:
	friend class HTree<value_type>::HNode;
	};

/*! \brief Basic building block of HTree<>.
 */
template<typename value_t>
class HTree<value_t>::HNode
	{
public:
	typedef value_t value_type;
	typedef HList<node_t> branch_t;
	typedef typename tree_t::iterator iterator;
	typedef typename tree_t::reverse_iterator reverse_iterator;
	typedef typename tree_t::const_iterator const_iterator;
	typedef typename tree_t::const_reverse_iterator const_reverse_iterator;
private:
	value_t _data;			/* object itself */
	branch_t _branch;	/* list of next level nodes */
	HNode* _trunk;	/* self explanary */
	HTree<value_t>* _tree;
public:
	int long child_count( void ) const;
	bool has_childs( void ) const;
	int get_level( void ) const;
	typename tree_t::iterator move_node( typename tree_t::iterator const&, typename tree_t::HNode* );
	typename tree_t::iterator move_node( typename tree_t::HNode* );
	typename tree_t::iterator copy_node( typename tree_t::iterator const&, typename HTree<value_t>::HNode* );
	typename tree_t::iterator copy_node( typename tree_t::HNode* );
	typename tree_t::iterator insert_node( typename tree_t::iterator const&, value_t const& );
	typename tree_t::iterator add_node( value_t const& );
	typename tree_t::iterator add_node( void );
	typename tree_t::iterator replace_node( typename tree_t::iterator, typename HTree<value_t>::HNode* );
	typename tree_t::iterator remove_node( typename tree_t::iterator );
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	node_t get_parent( void );
	const_node_t get_parent( void ) const;
	value_t* operator->( void );
	value_t const* operator->( void ) const;
	value_t& operator* ( void );
	value_t const& operator* ( void ) const;
private:
	HNode( tree_t* ); /* to create a root node */
	HNode( HNode* ); /* to create a child node */
	HNode( value_t const& ); /* to clone node */
	HNode( value_t const&, HNode* ); /* to clone a child node */
	virtual ~HNode( void );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	void detach( void );
	HNode* clone( HNode* ) const;
	void disjointed( typename HTree<value_t>::iterator const&, typename tree_t::HNode* ) const;
	friend class HTree<value_t>;
	friend class HList<HNode*>;
	};

/*! \brief Iterator for HTree<> data structure.
 */
template<typename value_t>
template<typename const_qual_t>
class HTree<value_t>::HIterator : public iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type>
	{
	typedef iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type> base_type;
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t>::const_node_t owner_t;
	typedef typename HTree<value_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::TREAT_AS_OPENED> list_it_t;
	owner_t _owner;
	list_it_t _iterator;
public:
	HIterator( void );
	HIterator( typename HTree<value_t>::template HIterator<const_qual_t> const& );
	HIterator& operator ++ ( void )
		{
		++ _iterator;
		return ( *this );
		}
	HIterator operator ++ ( int )
		{
		HIterator it( *this );
		++ _iterator;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- _iterator;
		return ( *this );
		}
	HIterator operator -- ( int )
		{
		HIterator it( *this );
		-- _iterator;
		return ( it );
		}
	typename HTree<value_t>::template HIterator<const_qual_t>& operator = ( typename HTree<value_t>::template HIterator<const_qual_t> const& );
	bool operator == ( typename HTree<value_t>::template HIterator<const_qual_t> const& ) const;
	bool operator != ( typename HTree<value_t>::template HIterator<const_qual_t> const& ) const;
	const_qual_node_t& operator* ( void );
	const_qual_node_t& operator* ( void ) const;
	const_qual_node_t* operator->( void );
private:
	friend class HTree<value_t>::HNode;
	HIterator( const_node_t, typename HTree<value_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::TREAT_AS_OPENED> const& );
	};

template<typename value_t>
HTree<value_t>::HNode::HNode( tree_t* tree_ ) : _data(),
	_branch(), _trunk( NULL ), _tree( tree_ )
	{
	M_PROLOG
	M_ASSERT( tree_ );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( HNode* node_ ) : _data(),
	_branch(), _trunk( node_ ), _tree( NULL )
	{
	M_PROLOG
	M_ASSERT( node_ );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( value_t const& data ) : _data( data ),
	_branch(), _trunk( NULL ), _tree( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( value_t const& data, HNode* node_ ) : _data( data ),
	_branch(), _trunk( node_ ), _tree( NULL )
	{
	M_PROLOG
	M_ASSERT( node_ );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::~HNode( void )
	{
	M_PROLOG
	struct tool
		{
		static void deleter( HNode*& node )
			{ delete node; }
		};
	for_each( _branch.begin(), _branch.end(), cref( tool::deleter ) );
	return;
	M_EPILOG
	}

template<typename value_t>
int long HTree<value_t>::HNode::child_count( void ) const
	{
	M_PROLOG
	return ( _branch.size() );
	M_EPILOG
	}

template<typename value_t>
bool HTree<value_t>::HNode::has_childs( void ) const
	{
	M_PROLOG
	return ( ! _branch.is_empty() );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::begin()
	{
	return ( iterator( this, _branch.begin() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::begin() const
	{
	return ( const_iterator( this, _branch.begin() ) );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::end()
	{
	return ( iterator( this, _branch.end() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::end() const
	{
	return ( const_iterator( this, _branch.end() ) );
	}

template<typename value_t>
typename HTree<value_t>::reverse_iterator HTree<value_t>::HNode::rbegin()
	{
	return ( end() );
	}

template<typename value_t>
typename HTree<value_t>::const_reverse_iterator HTree<value_t>::HNode::rbegin() const
	{
	return ( end() );
	}

template<typename value_t>
typename HTree<value_t>::reverse_iterator HTree<value_t>::HNode::rend()
	{
	return ( begin() );
	}

template<typename value_t>
typename HTree<value_t>::const_reverse_iterator HTree<value_t>::HNode::rend() const
	{
	return ( begin() );
	}

template<typename value_t>
value_t& HTree<value_t>::HNode::operator* ( void )
	{
	return ( _data );
	}

template<typename value_t>
value_t const& HTree<value_t>::HNode::operator* ( void ) const
	{
	return ( _data );
	}

template<typename value_t>
value_t* HTree<value_t>::HNode::operator->( void )
	{
	return ( &_data );
	}

template<typename value_t>
value_t const* HTree<value_t>::HNode::operator->( void ) const
	{
	return ( &_data );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::add_node( value_t const& value )
	{
	M_PROLOG
	node_t n = NULL;
	_branch.push_back( n = new HNode( this ) );
	**n = value;
	return ( iterator( this, _branch.rbegin().base() ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::add_node( void )
	{
	M_PROLOG
	node_t n = NULL;
	_branch.push_back( n = new HNode( this ) );
	return ( iterator( this, _branch.rbegin().base() ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::insert_node( typename HTree<value_t>::iterator const& pos, value_t const& value )
	{
	M_PROLOG
	node_t n = NULL;
	M_ASSERT( pos._owner == this );
	iterator it( this, _branch.insert( pos._iterator, n = new HNode( this ) ) );
	**it = value;
	return ( it );
	M_EPILOG
	}

template<typename value_t>
void HTree<value_t>::HNode::disjointed( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node ) const
	{
	M_PROLOG
	HNode* p = _trunk;
	while ( p )
		{
		M_ASSERT( p != node );
		p = p->_trunk;
		}
	M_ASSERT( pos._owner == this );
	return;
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::replace_node( typename HTree<value_t>::iterator pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	if ( *pos._iterator != node )
		{
		node->detach();
		HNode* wasted = *pos._iterator;
		*pos._iterator = node;
		node->_trunk = this;
		delete wasted;
		}
	return ( pos );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::remove_node( typename HTree<value_t>::iterator pos )
	{
	M_PROLOG
	delete *pos._iterator;
	return ( iterator( this, _branch.erase( pos._iterator ) ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::move_node( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	iterator it = pos;
	if ( *pos._iterator != node )
		{
		node->detach();
		it = iterator( this, _branch.insert( pos._iterator, node ) );
		node->_trunk = this;
		}
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::move_node( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( begin(), node );
#endif /* defined( __DEBUG__ ) */
	iterator it = rbegin().base();
	if ( ( it == rend().base() ) || ( *it._iterator != node ) )
		{
		node->detach();
		_branch.push_back( node );
		it = iterator( this, _branch.rbegin().base() );
		node->_trunk = this;
		}
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::copy_node( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	iterator it( this, _branch.insert( pos._iterator, node->clone( this ) ) );
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::copy_node( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( rbegin().base(), node );
#endif /* defined( __DEBUG__ ) */
	_branch.push_back( node->clone( this ) );
	iterator it( this, _branch.rbegin().base() );
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::HNode::clone( HNode* parent ) const
	{
	M_PROLOG
	node_t node = new HNode( _data );
	node->_trunk = parent;
	typename branch_t::const_iterator endIt = _branch.end();
	for ( typename branch_t::const_iterator it = _branch.begin();
			it != endIt; ++ it )
		node->_branch.push_back( (*it)->clone( node ) );
	return ( node );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::HNode::get_parent( void )
	{
	return ( _trunk );
	}

template<typename value_t>
typename HTree<value_t>::const_node_t HTree<value_t>::HNode::get_parent( void ) const
	{
	return ( _trunk );
	}

template<typename value_t>
void HTree<value_t>::HNode::detach( void )
	{
	M_PROLOG
	if ( _tree )
		{
		_tree->_root = NULL;
		_tree = NULL;
		}
	else
		{
		typename branch_t::iterator endIt = _trunk->_branch.end();
		for ( typename branch_t::iterator it = _trunk->_branch.begin();
				it != endIt; ++ it )
			{
			if ( *it == this )
				{
				*it = NULL;
				_trunk->_branch.erase( it );
				break;
				}
			}
		}
	return;
	M_EPILOG
	}

template<typename value_t>
int HTree<value_t>::HNode::get_level( void ) const
	{
	M_PROLOG
	int level = 0;
	HNode const* p = this;
	while ( ( p = p->_trunk ) )
		++ level;
	return ( level );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( void )
	: base_type(), _owner( NULL ), _iterator()
	{
	return;
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( const_node_t owner_,
		typename HTree<value_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t,
		OListBits::TREAT_AS_OPENED> const& iterator_ )
	: base_type(), _owner( owner_ ), _iterator( iterator_ )
	{
	return;
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( typename HTree<value_t>::template HIterator<const_qual_t> const& it )
	: base_type(), _owner( it._owner ), _iterator( it._iterator )
	{
	return;
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>& HTree<value_t>::HIterator<const_qual_t>::operator = ( typename HTree<value_t>::template HIterator<const_qual_t> const& it )
	{
	M_PROLOG
	if ( &it != this )
		{
		_owner = it._owner;
		_iterator = it._iterator;
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t>::HIterator<const_qual_t>::operator* ( void )
	{
	return ( **_iterator );
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t>::HIterator<const_qual_t>::operator* ( void ) const
	{
	return ( **_iterator );
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t* HTree<value_t>::HIterator<const_qual_t>::operator->( void )
	{
	return ( *_iterator );
	}

template<typename value_t>
template<typename const_qual_t>
bool HTree<value_t>::HIterator<const_qual_t>::operator == ( typename HTree<value_t>::template HIterator<const_qual_t> const& it ) const
	{
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	return ( _iterator == it._iterator );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
bool HTree<value_t>::HIterator<const_qual_t>::operator != ( typename HTree<value_t>::template HIterator<const_qual_t> const& it ) const
	{
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	return ( _iterator != it._iterator );
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HTree( void )
	: _root( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::~HTree( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HTree( HTree const& t )
	: _root( t._root ? t._root->clone( NULL ) : NULL )
	{
	M_PROLOG
	_root && ( _root->_tree = this );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>& HTree<value_t>::operator = ( HTree const& t )
	{
	M_PROLOG
	if ( &t != this )
		{
		HTree<value_t> tmp( t );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_t>
void HTree<value_t>::clear( void )
	{
	M_PROLOG
	if ( _root )
		delete _root;
	_root = NULL;
	M_EPILOG
	}

template<typename value_t>
void HTree<value_t>::swap( HTree<value_t>& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( _root, other._root );
		/* after swap */
		_root && ( _root->_tree = this );
		other._root && ( other._root->_tree = &other );
		}
	return;
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::create_new_root( void )
	{
	M_PROLOG
	clear();
	_root = new HNode( this );
	return ( _root );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::set_new_root( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
	if ( node != _root )
		{
		HNode* wasted = _root;
		node->detach();
		_root = node;
		node->_tree = this;
		node->_trunk = NULL;
		delete wasted;
		}
	return ( node );
	M_EPILOG
	}

template<typename type_t>
typename HTree<type_t>::node_t HTree<type_t>::get_root( void )
	{
	return ( _root );
	}

template<typename type_t>
typename HTree<type_t>::const_node_t HTree<type_t>::get_root( void ) const
	{
	return ( _root );
	}

}

template<typename type_t>
inline void swap( yaal::hcore::HTree<type_t>& a, yaal::hcore::HTree<type_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HTREE_HXX_INCLUDED */


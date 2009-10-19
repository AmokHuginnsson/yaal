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
#define YAAL_HCORE_HTREE_HXX_INCLUDED

#line 31

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
template<typename value_type>
class HTree
	{
private:
	typedef HTree<value_type> tree_t;
public:
	typedef value_type value_t;
	class HNode;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_t> iterator;
	typedef HIterator<value_t const> const_iterator;
private:
	HNode* f_poRoot;			/* self explanary */
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
	void swap( HTree<value_t>& );
private:
	friend class HTree<value_t>::HNode;
	};

/*! \brief Basic building block of HTree<>.
 */
template<typename value_t>
class HTree<value_t>::HNode
	{
public:
	typedef HList<node_t> branch_t;
private:
	value_t f_tData;			/* object itself */
	branch_t f_oBranch;	/* list of next level nodes */
	HNode* f_poTrunk;	/* self explanary */
	HTree<value_t>* f_poTree;
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
	typename tree_t::iterator begin();
	typename tree_t::const_iterator begin() const;
	typename tree_t::iterator end();
	typename tree_t::const_iterator end() const;
	typename tree_t::iterator rbegin();
	typename tree_t::const_iterator rbegin() const;
	typename tree_t::iterator rend();
	typename tree_t::const_iterator rend() const;
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
class HTree<value_t>::HIterator
	{
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t>::const_node_t owner_t;
	typedef typename HTree<value_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::TREAT_AS_OPENED> list_it_t;
	owner_t f_poOwner;
	list_it_t f_oIterator;
public:
	HIterator( void );
	HIterator( typename HTree<value_t>::template HIterator<const_qual_t> const& );
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
HTree<value_t>::HNode::HNode( tree_t* a_poTree ) : f_tData(),
	f_oBranch(), f_poTrunk( NULL ), f_poTree( a_poTree )
	{
	M_PROLOG
	M_ASSERT( a_poTree );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( HNode* a_poNode ) : f_tData(),
	f_oBranch(), f_poTrunk( a_poNode ), f_poTree( NULL )
	{
	M_PROLOG
	M_ASSERT( a_poNode );
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( value_t const& data ) : f_tData( data ),
	f_oBranch(), f_poTrunk( NULL ), f_poTree( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HNode::HNode( value_t const& data, HNode* a_poNode ) : f_tData( data ),
	f_oBranch(), f_poTrunk( a_poNode ), f_poTree( NULL )
	{
	M_PROLOG
	M_ASSERT( a_poNode );
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
	for_each( f_oBranch.begin(), f_oBranch.end(), cref( tool::deleter ) );
	return;
	M_EPILOG
	}

template<typename value_t>
int long HTree<value_t>::HNode::child_count( void ) const
	{
	M_PROLOG
	return ( f_oBranch.size() );
	M_EPILOG
	}

template<typename value_t>
bool HTree<value_t>::HNode::has_childs( void ) const
	{
	M_PROLOG
	return ( ! f_oBranch.is_empty() );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::begin()
	{
	return ( iterator( this, f_oBranch.begin() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::begin() const
	{
	return ( const_iterator( this, f_oBranch.begin() ) );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::end()
	{
	return ( iterator( this, f_oBranch.end() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::end() const
	{
	return ( const_iterator( this, f_oBranch.end() ) );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::rbegin()
	{
	return ( iterator( this, f_oBranch.rbegin() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::rbegin() const
	{
	return ( const_iterator( this, f_oBranch.rbegin() ) );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::rend()
	{
	return ( iterator( this, f_oBranch.rend() ) );
	}

template<typename value_t>
typename HTree<value_t>::const_iterator HTree<value_t>::HNode::rend() const
	{
	return ( const_iterator( this, f_oBranch.rend() ) );
	}

template<typename value_t>
value_t& HTree<value_t>::HNode::operator* ( void )
	{
	return ( f_tData );
	}

template<typename value_t>
value_t const& HTree<value_t>::HNode::operator* ( void ) const
	{
	return ( f_tData );
	}

template<typename value_t>
value_t* HTree<value_t>::HNode::operator->( void )
	{
	return ( &f_tData );
	}

template<typename value_t>
value_t const* HTree<value_t>::HNode::operator->( void ) const
	{
	return ( &f_tData );
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::add_node( value_t const& value )
	{
	M_PROLOG
	node_t n = NULL;
	f_oBranch.push_back( n = new HNode( this ) );
	**n = value;
	return ( iterator( this, f_oBranch.rbegin() ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::add_node( void )
	{
	M_PROLOG
	node_t n = NULL;
	f_oBranch.push_back( n = new HNode( this ) );
	return ( iterator( this, f_oBranch.rbegin() ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::insert_node( typename HTree<value_t>::iterator const& pos, value_t const& value )
	{
	M_PROLOG
	node_t n = NULL;
	M_ASSERT( pos.f_poOwner == this );
	iterator it( this, f_oBranch.insert( pos.f_oIterator, n = new HNode( this ) ) );
	**it = value;
	return ( it );
	M_EPILOG
	}

template<typename value_t>
void HTree<value_t>::HNode::disjointed( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node ) const
	{
	M_PROLOG
	HNode* p = f_poTrunk;
	while ( p )
		{
		M_ASSERT( p != node );
		p = p->f_poTrunk;
		}
	M_ASSERT( pos.f_poOwner == this );
	return;
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::replace_node( typename HTree<value_t>::iterator pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if ! defined(NDEBUG)
	disjointed( pos, node );
#endif
	if ( *pos.f_oIterator != node )
		{
		node->detach();
		HNode* wasted = *pos.f_oIterator;
		*pos.f_oIterator = node;
		node->f_poTrunk = this;
		delete wasted;
		}
	return ( pos );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::remove_node( typename HTree<value_t>::iterator pos )
	{
	M_PROLOG
	delete *pos.f_oIterator;
	return ( iterator( this, f_oBranch.erase( pos.f_oIterator ) ) );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::move_node( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if ! defined(NDEBUG)
	disjointed( pos, node );
#endif
	iterator it = pos;
	if ( *pos.f_oIterator != node )
		{
		node->detach();
		it = iterator( this, f_oBranch.insert( pos.f_oIterator, node ) );
		node->f_poTrunk = this;
		}
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::move_node( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if ! defined(NDEBUG)
	disjointed( begin(), node );
#endif
	iterator it = rbegin();
	if ( ( it == rend() ) || ( *it.f_oIterator != node ) )
		{
		node->detach();
		f_oBranch.push_back( node );
		it = iterator( this, f_oBranch.rbegin() );
		node->f_poTrunk = this;
		}
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::copy_node( typename HTree<value_t>::iterator const& pos, typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if ! defined(NDEBUG)
	disjointed( pos, node );
#endif
	iterator it( this, f_oBranch.insert( pos.f_oIterator, node->clone( this ) ) );
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::iterator HTree<value_t>::HNode::copy_node( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
#if ! defined(NDEBUG)
	disjointed( rbegin(), node );
#endif
	f_oBranch.push_back( node->clone( this ) );
	iterator it( this, f_oBranch.rbegin() );
	return ( it );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::HNode::clone( HNode* parent ) const
	{
	M_PROLOG
	node_t node = new HNode( f_tData );
	node->f_poTrunk = parent;
	typename branch_t::const_iterator endIt = f_oBranch.end();
	for ( typename branch_t::const_iterator it = f_oBranch.begin();
			it != endIt; ++ it )
		node->f_oBranch.push_back( (*it)->clone( node ) );
	return ( node );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::HNode::get_parent( void )
	{
	return ( f_poTrunk );
	}

template<typename value_t>
typename HTree<value_t>::const_node_t HTree<value_t>::HNode::get_parent( void ) const
	{
	return ( f_poTrunk );
	}

template<typename value_t>
void HTree<value_t>::HNode::detach( void )
	{
	M_PROLOG
	if ( f_poTree )
		{
		f_poTree->f_poRoot = NULL;
		f_poTree = NULL;
		}
	else
		{
		typename branch_t::iterator endIt = f_poTrunk->f_oBranch.end();
		for ( typename branch_t::iterator it = f_poTrunk->f_oBranch.begin();
				it != endIt; ++ it )
			{
			if ( *it == this )
				{
				*it = NULL;
				f_poTrunk->f_oBranch.erase( it );
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
	while ( ( p = p->f_poTrunk ) )
		++ level;
	return ( level );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( void )
	: f_poOwner( NULL ), f_oIterator()
	{
	return;
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( const_node_t a_poOwner,
		typename HTree<value_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t,
		OListBits::TREAT_AS_OPENED> const& a_oIterator )
	: f_poOwner( a_poOwner ), f_oIterator( a_oIterator )
	{
	return;
	}

template<typename value_t>
template<typename const_qual_t>
HTree<value_t>::HIterator<const_qual_t>::HIterator( typename HTree<value_t>::template HIterator<const_qual_t> const& it )
	: f_poOwner( it.f_poOwner ), f_oIterator( it.f_oIterator )
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
		f_poOwner = it.f_poOwner;
		f_oIterator = it.f_oIterator;
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t>::HIterator<const_qual_t>::operator* ( void )
	{
	return ( **f_oIterator );
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t>::HIterator<const_qual_t>::operator* ( void ) const
	{
	return ( **f_oIterator );
	}

template<typename value_t>
template<typename const_qual_t>
typename HTree<value_t>::template HIterator<const_qual_t>::const_qual_node_t* HTree<value_t>::HIterator<const_qual_t>::operator->( void )
	{
	return ( *f_oIterator );
	}

template<typename value_t>
template<typename const_qual_t>
bool HTree<value_t>::HIterator<const_qual_t>::operator == ( typename HTree<value_t>::template HIterator<const_qual_t> const& it ) const
	{
	M_PROLOG
	M_ASSERT( f_poOwner == it.f_poOwner );
	return ( f_oIterator == it.f_oIterator );
	M_EPILOG
	}

template<typename value_t>
template<typename const_qual_t>
bool HTree<value_t>::HIterator<const_qual_t>::operator != ( typename HTree<value_t>::template HIterator<const_qual_t> const& it ) const
	{
	M_PROLOG
	M_ASSERT( f_poOwner == it.f_poOwner );
	return ( f_oIterator != it.f_oIterator );
	M_EPILOG
	}

template<typename value_t>
HTree<value_t>::HTree( void )
	: f_poRoot( NULL )
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
	: f_poRoot( t.f_poRoot ? t.f_poRoot->clone( NULL ) : NULL )
	{
	M_PROLOG
	f_poRoot && ( f_poRoot->f_poTree = this );
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
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	M_EPILOG
	}

template<typename value_t>
void HTree<value_t>::swap( HTree<value_t>& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_poRoot, other.f_poRoot );
		/* after swap */
		f_poRoot && ( f_poRoot->f_poTree = this );
		other.f_poRoot && ( other.f_poRoot->f_poTree = &other );
		}
	return;
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::create_new_root( void )
	{
	M_PROLOG
	clear();
	f_poRoot = new HNode( this );
	return ( f_poRoot );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::set_new_root( typename HTree<value_t>::HNode* node )
	{
	M_PROLOG
	if ( node != f_poRoot )
		{
		HNode* wasted = f_poRoot;
		node->detach();
		f_poRoot = node;
		node->f_poTree = this;
		node->f_poTrunk = NULL;
		delete wasted;
		}
	return ( node );
	M_EPILOG
	}

template<typename value_t>
typename HTree<value_t>::node_t HTree<value_t>::get_root( void )
	{
	return ( f_poRoot );
	}

template<typename value_t>
typename HTree<value_t>::const_node_t HTree<value_t>::get_root( void ) const
	{
	return ( f_poRoot );
	}

}

template<typename value_t>
inline void swap( yaal::hcore::HTree<value_t>& a, yaal::hcore::HTree<value_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HTREE_HXX_INCLUDED */


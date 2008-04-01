/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htree.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must atttach Copyright information to any part of every copy
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

#ifndef __YAAL_HCORE_HTREE_H
#define __YAAL_HCORE_HTREE_H

#line 31

#define D_VCSID_HTREE_H "$Id$"

#include "hcore/hexception.h"
#include "hcore/hlist.h"

namespace yaal
{

namespace hcore
{

template<typename tType>
class HTree;

template<typename tType>
class HTree
	{
private:
	typedef HTree<tType> tree_t;
public:
	class HNode;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<tType> iterator;
	typedef HIterator<tType const> const_iterator;
private:
	HNode* f_poRoot;			/* self explanary */
public:
	HTree ( void );
	virtual ~HTree();
	node_t get_root( void );
	const_node_t get_root( void ) const;
	node_t create_new_root( void );
	node_t set_new_root( HNode* );
	void clear( void );
	static void swap( HTree<tType>&, HTree<tType>& );
private:
	HTree( HTree const& );
	HTree& operator = ( HTree const& );
	friend class HTree<tType>::HNode;
	};

template<typename tType>
class HTree<tType>::HNode
	{
private:
	typedef HList<node_t> branch_t;
	tType f_tData;			/* object itself */
	branch_t f_oBranch;	/* list of next level nodes */
	HNode* f_poTrunk;	/* self explanary */
	HTree<tType>* f_poTree;
public:
	int long child_count( void ) const;
	bool has_childs( void ) const;
	int get_level( void ) const;
	typename tree_t::iterator move_node( typename tree_t::iterator const&, typename tree_t::HNode* );
	typename tree_t::iterator move_node( typename tree_t::HNode* );
	typename tree_t::iterator copy_node( typename tree_t::iterator const&, typename HTree<tType>::HNode* );
	typename tree_t::iterator copy_node( typename tree_t::HNode* );
	typename tree_t::iterator insert_node( typename tree_t::iterator const&, tType const& );
	typename tree_t::iterator add_node( tType const& );
	typename tree_t::iterator add_node( void );
	typename tree_t::iterator replace_node( typename tree_t::iterator, typename HTree<tType>::HNode* );
	typename tree_t::iterator remove_node( tree_t::iterator );
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
	tType* operator->( void );
	tType const* operator->( void ) const;
	tType& operator* ( void );
	tType const& operator* ( void ) const;
private:
	HNode( tree_t* ); /* to create a root node */
	HNode( HNode* ); /* to create a child node */
	HNode( tType const& ); /* to clone node */
	HNode( tType const&, HNode* ); /* to clone a child node */
	virtual ~HNode( void );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	void detach( void );
	HNode* clone( HNode* ) const;
	void disjointed( HTree<tType>::iterator const&, typename tree_t::HNode* ) const;
	friend class HTree<tType>;
	friend class HList<HNode*>;
	};

template<typename tType>
template<typename const_qual_t>
class HTree<tType>::HIterator
	{
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<tType>::const_node_t owner_t;
	typedef typename HTree<tType>::HNode::branch_t::template HIterator<const_qual_node_ptr_t,
					OListBits::D_TREAT_AS_OPENED> list_it_t;
	owner_t f_poOwner;
	list_it_t f_oIterator;
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
	const_qual_node_t& operator* ( void );
	const_qual_node_t& operator* ( void ) const;
	const_qual_node_t* operator->( void );
private:
	friend class HTree<tType>::HNode;
	HIterator( const_node_t, typename HTree<tType>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::D_TREAT_AS_OPENED> const& );
	};

template<typename tType>
HTree<tType>::HNode::HNode( tree_t* a_poTree ) : f_tData(),
	f_oBranch(), f_poTrunk( NULL ), f_poTree( a_poTree )
	{
	M_PROLOG
	M_ASSERT( a_poTree );
	return;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::HNode( HNode* a_poNode ) : f_tData(),
	f_oBranch(), f_poTrunk( a_poNode ), f_poTree( NULL )
	{
	M_PROLOG
	M_ASSERT( a_poNode );
	return;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::HNode( tType const& data ) : f_tData( data ),
	f_oBranch(), f_poTrunk( NULL ), f_poTree( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::HNode( tType const& data, HNode* a_poNode ) : f_tData( data ),
	f_oBranch(), f_poTrunk( a_poNode ), f_poTree( NULL )
	{
	M_PROLOG
	M_ASSERT( a_poNode );
	return;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::~HNode( void )
	{
	M_PROLOG
	struct tool
		{
		static void deleter( HNode*& node )
			{ delete node; }
		};
	for_each( f_oBranch.begin(), f_oBranch.end(), tool::deleter );
	return;
	M_EPILOG
	}

template<typename tType>
int long HTree<tType>::HNode::child_count( void ) const
	{
	M_PROLOG
	return ( f_oBranch.size() );
	M_EPILOG
	}

template<typename tType>
bool HTree<tType>::HNode::has_childs( void ) const
	{
	M_PROLOG
	return ( ! f_oBranch.is_empty() );
	M_EPILOG
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::begin()
	{
	return ( iterator( this, f_oBranch.begin() ) );
	}

template<typename tType>
typename HTree<tType>::const_iterator HTree<tType>::HNode::begin() const
	{
	return ( const_iterator( this, f_oBranch.begin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::end()
	{
	return ( iterator( this, f_oBranch.end() ) );
	}

template<typename tType>
typename HTree<tType>::const_iterator HTree<tType>::HNode::end() const
	{
	return ( const_iterator( this, f_oBranch.end() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::rbegin()
	{
	return ( iterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::const_iterator HTree<tType>::HNode::rbegin() const
	{
	return ( const_iterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::rend()
	{
	return ( iterator( this, f_oBranch.rend() ) );
	}

template<typename tType>
typename HTree<tType>::const_iterator HTree<tType>::HNode::rend() const
	{
	return ( const_iterator( this, f_oBranch.rend() ) );
	}

template<typename tType>
tType& HTree<tType>::HNode::operator* ( void )
	{
	return ( f_tData );
	}

template<typename tType>
tType const& HTree<tType>::HNode::operator* ( void ) const
	{
	return ( f_tData );
	}

template<typename tType>
tType* HTree<tType>::HNode::operator->( void )
	{
	return ( &f_tData );
	}

template<typename tType>
tType const* HTree<tType>::HNode::operator->( void ) const
	{
	return ( &f_tData );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::add_node( tType const& value )
	{
	node_t n = NULL;
	f_oBranch.push_back( n = new HNode( this ) );
	**n = value;
	return ( iterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::add_node( void )
	{
	node_t n = NULL;
	f_oBranch.push_back( n = new HNode( this ) );
	return ( iterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::insert_node( HTree<tType>::iterator const& pos, tType const& value )
	{
	node_t n = NULL;
	M_ASSERT( pos.f_poOwner == this );
	iterator it( this, f_oBranch.insert( pos.f_oIterator, n = new HNode( this ) ) );
	**it = value;
	return ( it );
	}

template<typename tType>
void HTree<tType>::HNode::disjointed( HTree<tType>::iterator const& pos, HTree<tType>::HNode* node ) const
	{
	HNode* p = f_poTrunk;
	while ( p )
		{
		M_ASSERT( p != node );
		p = p->f_poTrunk;
		}
	M_ASSERT( pos.f_poOwner == this );
	return;
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::replace_node( HTree<tType>::iterator pos, HTree<tType>::HNode* node )
	{
#if not defined(NDEBUG)
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
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::remove_node( HTree<tType>::iterator pos )
	{
	delete *pos.f_oIterator;
	return ( iterator( this, f_oBranch.erase( pos.f_oIterator ) ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::move_node( HTree<tType>::iterator const& pos, HTree<tType>::HNode* node )
	{
#if not defined(NDEBUG)
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
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::move_node( HTree<tType>::HNode* node )
	{
#if not defined(NDEBUG)
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
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::copy_node( HTree<tType>::iterator const& pos, HTree<tType>::HNode* node )
	{
#if not defined(NDEBUG)
	disjointed( pos, node );
#endif
	iterator it( this, f_oBranch.insert( pos.f_oIterator, node->clone( this ) ) );
	return ( it );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::copy_node( HTree<tType>::HNode* node )
	{
#if not defined(NDEBUG)
	disjointed( rbegin(), node );
#endif
	f_oBranch.push_back( node->clone( this ) );
	iterator it( this, f_oBranch.rbegin() );
	return ( it );
	}

template<typename tType>
typename HTree<tType>::node_t HTree<tType>::HNode::clone( HNode* parent ) const
	{
	node_t node = new HNode( f_tData );
	node->f_poTrunk = parent;
	typename branch_t::const_iterator endIt = f_oBranch.end();
	for ( typename branch_t::const_iterator it = f_oBranch.begin();
			it != endIt; ++ it )
		node->f_oBranch.push_back( (*it)->clone( node ) );
	return ( node );
	}

template<typename tType>
typename HTree<tType>::node_t HTree<tType>::HNode::get_parent( void )
	{
	return ( f_poTrunk );
	}

template<typename tType>
typename HTree<tType>::const_node_t HTree<tType>::HNode::get_parent( void ) const
	{
	return ( f_poTrunk );
	}

template<typename tType>
void HTree<tType>::HNode::detach( void )
	{
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
	}

template<typename tType>
int HTree<tType>::HNode::get_level( void ) const
	{
	int level = 0;
	HNode const* p = this;
	while ( ( p = p->f_poTrunk ) )
		++ level;
	return ( level );
	}

template<typename tType>
template<typename const_qual_t>
HTree<tType>::HIterator<const_qual_t>::HIterator( void )
	: f_poOwner( NULL ), f_oIterator()
	{
	return;
	}

template<typename tType>
template<typename const_qual_t>
HTree<tType>::HIterator<const_qual_t>::HIterator( const_node_t a_poOwner,
		typename HTree<tType>::HNode::branch_t::template HIterator<const_qual_node_ptr_t,
		OListBits::D_TREAT_AS_OPENED> const& a_oIterator )
	: f_poOwner( a_poOwner ), f_oIterator( a_oIterator )
	{
	return;
	}

template<typename tType>
template<typename const_qual_t>
HTree<tType>::HIterator<const_qual_t>::HIterator( HTree<tType>::HIterator<const_qual_t> const& it )
	: f_poOwner( it.f_poOwner ), f_oIterator( it.f_oIterator )
	{
	return;
	}

template<typename tType>
template<typename const_qual_t>
typename HTree<tType>::template HIterator<const_qual_t>& HTree<tType>::HIterator<const_qual_t>::operator = ( HTree<tType>::HIterator<const_qual_t> const& it )
	{
	if ( &it != this )
		{
		f_poOwner = it.f_poOwner;
		f_oIterator = it.f_oIterator;
		}
	return ( *this );
	}

template<typename tType>
template<typename const_qual_t>
typename HTree<tType>::template HIterator<const_qual_t>::const_qual_node_t& HTree<tType>::HIterator<const_qual_t>::operator* ( void )
	{
	return ( **f_oIterator );
	}

template<typename tType>
template<typename const_qual_t>
typename HTree<tType>::template HIterator<const_qual_t>::const_qual_node_t& HTree<tType>::HIterator<const_qual_t>::operator* ( void ) const
	{
	return ( **f_oIterator );
	}

template<typename tType>
template<typename const_qual_t>
typename HTree<tType>::template HIterator<const_qual_t>::const_qual_node_t* HTree<tType>::HIterator<const_qual_t>::operator->( void )
	{
	return ( *f_oIterator );
	}

template<typename tType>
template<typename const_qual_t>
bool HTree<tType>::HIterator<const_qual_t>::operator == ( HTree<tType>::HIterator<const_qual_t> const& it ) const
	{
	M_ASSERT( f_poOwner == it.f_poOwner );
	return ( f_oIterator == it.f_oIterator );
	}

template<typename tType>
template<typename const_qual_t>
bool HTree<tType>::HIterator<const_qual_t>::operator != ( HTree<tType>::HIterator<const_qual_t> const& it ) const
	{
	M_ASSERT( f_poOwner == it.f_poOwner );
	return ( f_oIterator != it.f_oIterator );
	}

template<typename tType>
HTree<tType>::HTree( void )
	: f_poRoot( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::~HTree( void )
	{
	M_PROLOG
	clear();
	return ;
	M_EPILOG
	}

template<typename tType>
void HTree<tType>::clear( void )
	{
	M_PROLOG
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	M_EPILOG
	}

template<typename tType>
void HTree<tType>::swap( HTree<tType>& left, HTree<tType>& right )
	{
	if ( &left != &right )
		{
		yaal::swap( left.f_poRoot, right.f_poRoot );
		/* after swap */
		left.f_poRoot->f_poTree = &left;
		right.f_poRoot->f_poTree = &right;
		}
	return;
	}

template<typename tType>
typename HTree<tType>::node_t HTree<tType>::create_new_root( void )
	{
	M_PROLOG
	clear();
	f_poRoot = new HNode( this );
	return ( f_poRoot );
	M_EPILOG
	}

template<typename tType>
typename HTree<tType>::node_t HTree<tType>::set_new_root( HTree<tType>::HNode* node )
	{
	M_PROLOG
	if ( node != f_poRoot )
		{
		node->detach();
		HNode* wasted = f_poRoot;
		f_poRoot = node;
		delete wasted;
		node->f_poTree = this;
		node->f_poTrunk = NULL;
		}
	return ( node );
	M_EPILOG
	}

template<typename tType>
typename HTree<tType>::node_t HTree<tType>::get_root( void )
	{
	return ( f_poRoot );
	}

template<typename tType>
typename HTree<tType>::const_node_t HTree<tType>::get_root( void ) const
	{
	return ( f_poRoot );
	}

}

}

#endif /* not __YAAL_HCORE_HTREE_H */


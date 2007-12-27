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
	typedef HTree<tType> tree_t;
public:
	class HNode;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	class HIterator;
	typedef HIterator iterator;
private:
	HNode* f_poRoot;			/* self explanary */
public:
	HTree ( void );
	virtual ~HTree();
	node_t get_root( void );
	const_node_t get_root( void ) const;
	node_t set_new_root( void );
	void clear( void );
private:
	HTree( HTree const& );
	HTree& operator = ( HTree const& );
	friend class HTree<tType>::HNode;
	};

template<typename tType>
class HTree<tType>::HNode
	{
	typedef HList<node_t> branch_t;
	tType f_tData;			/* object itself */
	int f_iLevel;				/* self explanary */
	branch_t f_oBranch;	/* list of next level nodes */
	HNode* f_poTrunk;	/* self explanary */
	HTree<tType>* f_poTree;
public:
	int long child_count( void ) const;
	bool has_childs( void ) const;
	typename tree_t::iterator insert_node( typename tree_t::iterator const&, typename tree_t::HNode* );
	typename tree_t::iterator insert_node( typename tree_t::iterator const&, tType const& );
	typename tree_t::iterator replace_node( typename tree_t::iterator, typename HTree<tType>::HNode* );
	typename tree_t::iterator add_node( typename tree_t::HNode* );
	typename tree_t::iterator add_node( tType const& );
	void remove_node( tree_t::iterator const& );
	typename tree_t::iterator begin();
	typename tree_t::iterator const begin() const;
	typename tree_t::iterator end();
	typename tree_t::iterator const end() const;
	typename tree_t::iterator rbegin();
	typename tree_t::iterator const rbegin() const;
	typename tree_t::iterator rend();
	typename tree_t::iterator const rend() const;
	node_t get_parent( void );
	const_node_t get_parent( void ) const;
	tType* operator->( void );
	tType const* operator->( void ) const;
	tType& operator* ( void );
	tType const& operator* ( void ) const;
private:
	HNode ( tree_t*, HNode* = NULL ); /* ( parent ) */
	virtual ~HNode( void );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	void detach( HNode* );
	friend class HTree<tType>;
	friend class HList<HNode*>;
	};

template<typename tType>
class HTree<tType>::HIterator
	{
	typename HTree<tType>::const_node_t f_poOwner;
	typename HTree<tType>::HNode::branch_t::iterator f_oIterator;
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
	typename HTree<tType>::HNode& operator* ( void );
	typename HTree<tType>::HNode const& operator* ( void ) const;
	typename HTree<tType>::HNode* operator->( void );
	typename HTree<tType>::HNode const* operator->( void ) const;
private:
	friend class HTree<tType>::HNode;
	HIterator( const_node_t, typename HTree<tType>::HNode::branch_t::iterator const& );
	};

template<typename tType>
HTree<tType>::HNode::HNode( tree_t* a_poTree, HNode* a_poNode ) : f_tData(),
	f_iLevel( 0 ), f_oBranch(),
	f_poTrunk( a_poNode ), f_poTree( a_poTree )
	{
	M_PROLOG
	M_ASSERT( a_poTree );
	if ( a_poNode )
		f_iLevel = a_poNode->f_iLevel + 1;
	return ;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::~HNode( void )
	{
	M_PROLOG
	for ( typename branch_t::iterator it = f_oBranch.begin(); it != f_oBranch.end(); ++ it )
		delete *it;
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
	return ( HIterator( this, f_oBranch.begin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator const HTree<tType>::HNode::begin() const
	{
	return ( HIterator( this, f_oBranch.begin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::end()
	{
	return ( HIterator( this, f_oBranch.end() ) );
	}

template<typename tType>
typename HTree<tType>::iterator const HTree<tType>::HNode::end() const
	{
	return ( HIterator( this, f_oBranch.end() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::rbegin()
	{
	return ( HIterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator const HTree<tType>::HNode::rbegin() const
	{
	return ( HIterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::rend()
	{
	return ( HIterator( this, f_oBranch.rend() ) );
	}

template<typename tType>
typename HTree<tType>::iterator const HTree<tType>::HNode::rend() const
	{
	return ( HIterator( this, f_oBranch.rend() ) );
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
typename HTree<tType>::iterator HTree<tType>::HNode::add_node( tType const& value )
	{
	node_t n = NULL;
	f_oBranch.push_back( n = new HNode( f_poTree, this ) );
	**n = value;
	return ( HIterator( this, f_oBranch.rbegin() ) );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::insert_node( HTree<tType>::HIterator const& pos, tType const& value )
	{
	node_t n = NULL;
	HIterator it( this, f_oBranch.insert( pos.f_oIterator, n = new HNode( f_poTree, this ) ) );
	**it = value;
	return ( it );
	}

template<typename tType>
typename HTree<tType>::iterator HTree<tType>::HNode::replace_node( HTree<tType>::HIterator pos, HTree<tType>::HNode* node )
	{
	if ( node->f_poTrunk )
		node->f_poTrunk->detach( node );
	else
		node->f_poTree->f_poRoot = NULL;
	delete *pos.f_oIterator;
	*pos.f_oIterator = node;
	node->f_poTrunk = this;
	return ( pos );
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
void HTree<tType>::HNode::detach( HTree<tType>::HNode* node )
	{
	for ( typename branch_t::iterator it = f_oBranch.begin(); it != f_oBranch.end(); ++ it )
		{
		if ( *it == node )
			{
			*it = NULL;
			break;
			}
		}
	return;
	}

template<typename tType>
HTree<tType>::HIterator::HIterator( typename HTree<tType>::const_node_t a_poOwner,
		typename HTree<tType>::HNode::branch_t::iterator const& a_oIterator )
	: f_poOwner( a_poOwner ), f_oIterator( a_oIterator )
	{
	return;
	}

template<typename tType>
HTree<tType>::HIterator::HIterator( HTree<tType>::HIterator const& it )
	: f_poOwner( it.f_poOwner ), f_oIterator( it.f_oIterator )
	{
	return;
	}

template<typename tType>
typename HTree<tType>::HIterator& HTree<tType>::HIterator::operator = ( HTree<tType>::HIterator const& it )
	{
	if ( &it != this )
		{
		f_poOwner = it.f_poOwner;
		f_oIterator = it.f_oIterator;
		}
	return ( *this );
	}

template<typename tType>
typename HTree<tType>::HNode& HTree<tType>::HIterator::operator* ( void )
	{
	return ( **f_oIterator );
	}

template<typename tType>
typename HTree<tType>::HNode const& HTree<tType>::HIterator::operator* ( void ) const
	{
	return ( **f_oIterator );
	}

template<typename tType>
typename HTree<tType>::HNode* HTree<tType>::HIterator::operator->( void )
	{
	return ( *f_oIterator );
	}

template<typename tType>
typename HTree<tType>::HNode const* HTree<tType>::HIterator::operator->( void ) const
	{
	return ( *f_oIterator );
	}

template<typename tType>
bool HTree<tType>::HIterator::operator == ( HTree<tType>::HIterator const& it ) const
	{
	M_ASSERT( f_poOwner == it.f_poOwner );
	return ( f_oIterator == it.f_oIterator );
	}

template<typename tType>
bool HTree<tType>::HIterator::operator != ( HTree<tType>::HIterator const& it ) const
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
typename HTree<tType>::node_t HTree<tType>::set_new_root( void )
	{
	M_PROLOG
	clear();
	f_poRoot = new HNode( this, NULL );
	return ( f_poRoot );
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


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
	class HNode;
	typedef HList<HNode*> branch_t;
public:
	class HIterator;
	typedef HIterator iterator;
private:
	class HNode
		{
		tType f_tData;			/* object itself */
		int f_iLevel;				/* self explanary */
		branch_t f_oBranch;	/* list of next level nodes */
		HNode* f_poTrunk;	/* self explanary */
		HTree<tType>* f_poTree;
	public:
		int long child_count( void ) const;
		bool has_childs( void ) const;
		typename tree_t::iterator insert_node( typename tree_t::iterator const&, typename tree_t::HNode* = NULL );
		typename tree_t::iterator insert_node( typename tree_t::iterator const&, tType const& );
		typename tree_t::iterator replace_node( typename tree_t::iterator const&, typename tree_t::HNode* );
		typename tree_t::iterator add_node( typename tree_t::HNode* = NULL );
		typename tree_t::iterator add_node( tType const& );
		void remove_node( tree_t::iterator );
		typename tree_t::iterator begin();
		typename tree_t::iterator const begin() const;
		typename tree_t::iterator end();
		typename tree_t::iterator const end() const;
		typename tree_t::iterator rbegin();
		typename tree_t::iterator rend();
		tType* operator->( void );
		tType const* operator->( void ) const;
	private:
		HNode ( HNode* = NULL ); /* ( parent ) */
		virtual ~HNode( void );
		HNode( HNode const& );
		HNode& operator = ( HNode const& );
		friend class HTree<tType>;
		friend class HList<HNode*>;
		};
	HNode* f_poRoot;			/* self explanary */
public:
	HTree ( void );
	virtual ~HTree();
	HNode& get_root( void );
	virtual void clear( void );
	void erase( iterator const& );
private:
	HTree( HTree const& );
	HTree& operator = ( HTree const& );
	friend class HTree<tType>::HNode;
	};

template<typename tType>
class HTree<tType>::HIterator
	{
public:
	HIterator( void );
	HIterator( HIterator const& );
	HIterator& operator ++ ( void );
	HIterator& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	typename HTree<tType>::HNode& operator* ( void );
	typename HTree<tType>::HNode const& operator* ( void ) const;
	typename HTree<tType>::HNode* operator->( void );
	typename HTree<tType>::HNode const* operator->( void ) const;
private:
	};

template<typename tType>
HTree<tType>::HNode::HNode( HNode* a_poNode ) : f_tData(),
	f_iLevel( 0 ), f_oBranch(),
	f_poTrunk( a_poNode )
	{
	M_PROLOG
	if ( a_poNode )
		f_iLevel = a_poNode->f_iLevel + 1;
	return ;
	M_EPILOG
	}

template<typename tType>
HTree<tType>::HNode::~HNode ( void )
	{
	M_PROLOG
	for ( typename branch_t::iterator it = f_oBranch.begin(); it != f_oBranch.end(); ++ it )
		delete *it;
	return;
	M_EPILOG
	}

template<typename tType>
HTree < tType >::HTree ( void )
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
void HTree<tType>::clear ( void )
	{
	M_PROLOG
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	M_EPILOG
	}

}

}

#endif /* not __YAAL_HCORE_HTREE_H */


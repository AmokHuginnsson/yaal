/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsbbstree.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HSBBSTREE_H
#define __YAAL_HCORE_HSBBSTREE_H

#line 31

#define D_VCSID_HSBBSTREE_H "$Id$"

#include "hcore/hexception.h"

namespace yaal
{

namespace hcore
{

extern char const * const n_ppcErrMsgHSBBSTree [ ];

/* HSBBSTree stands for: Self-balancing binary search tree. */

class HSBBSTree
	{
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_NON_EXISTING_KEY,
			E_NIL_ITERATOR
			} error_t;
		};
	class HIterator;
private:
	class HAbstractNode;
	struct ONodePtr
		{
		HAbstractNode * f_poNode;
		bool f_bExists;
		ONodePtr ( void ) : f_poNode ( NULL ), f_bExists ( false ) { }
		};
	class HAbstractNode
		{
		typedef enum
			{
			D_RED,
			D_BLACK
			} color_t;
		color_t f_eColor;
		HAbstractNode * f_poParent;
		HAbstractNode * f_poLeft;
		HAbstractNode * f_poRight;
		HAbstractNode ( void );
		virtual ~HAbstractNode ( void );
		HAbstractNode ( HAbstractNode const & );
		HAbstractNode & operator = ( HAbstractNode const & );
		void set_child ( HAbstractNode *, HAbstractNode * );
		friend class HSBBSTree;
		};
	template < typename tType >
	class HNode : public HAbstractNode
		{
		tType f_tKey;
		HNode ( tType const & );
		HNode ( HNode const & );
		HNode & operator = ( HNode const & );
		friend class HSBBSTree;
		};
	HAbstractNode * f_poRoot;
	int long f_lQuantity;
public:
	HSBBSTree ( void );
	virtual ~HSBBSTree ( void );
	template < typename tType >
	void insert ( tType const & );
	template < typename tType >
	void remove ( tType const & );
	void remove ( HIterator const & );
	template < typename tType >
	HIterator find ( tType const & ) const;
	int long size ( void ) const;
	bool empty ( void ) const;
	HIterator begin ( void ) const;
	HIterator end ( void ) const;
	HIterator rbegin ( void ) const;
	HIterator rend ( void ) const;
private:
	template < typename tType >
	ONodePtr find_node ( tType const & ) const;
	void remove_node ( HAbstractNode * );
	void swap ( HAbstractNode *, HAbstractNode * );
	HAbstractNode * sibling ( HAbstractNode * ) const;
	void insert_rebalance ( HAbstractNode * );
	void remove_rebalance ( HAbstractNode * );
	void rotate_left ( HAbstractNode * );
	void rotate_right ( HAbstractNode * );
	HSBBSTree ( HSBBSTree const & );
	HSBBSTree & operator = ( HSBBSTree const & );
	};

class HSBBSTree::HIterator
	{
	HAbstractNode * f_poCurrent;
public:
	HIterator ( void );
	HIterator ( HIterator const & );
	HIterator & operator ++ ( void );
	HIterator const operator ++ ( int );
	HIterator & operator -- ( void );
	HIterator const operator -- ( int );
	HIterator & operator = ( HIterator const & );
	bool operator == ( HIterator const & ) const;
	bool operator != ( HIterator const & ) const;
	template < typename tType >
	tType const & operator * ( void );
private:
	friend class HSBBSTree;
	explicit HIterator ( HAbstractNode * const );
	};

template < typename tType >
tType const & HSBBSTree::HIterator::operator * ( void )
	{
	M_ASSERT ( f_poCurrent );
	return ( static_cast < HNode < tType > * > ( f_poCurrent )->f_tKey );
	}

template < typename tType >
HSBBSTree::HNode < tType >::HNode ( tType const & a_tKey )
	: HAbstractNode(), f_tKey ( a_tKey )
	{
	return;
	}

template < typename tType >
void HSBBSTree::insert ( tType const & a_tKey )
	{
	ONodePtr l_oNode;
	if ( f_poRoot )
		l_oNode = find_node ( a_tKey );
	if ( l_oNode.f_bExists )
		static_cast< HNode < tType > * >( l_oNode.f_poNode )->f_tKey = a_tKey;
	else
		{
		HNode < tType > * l_poNode = new HNode < tType > ( a_tKey );
		f_lQuantity ++;
		if ( f_poRoot )
			{
			l_poNode->f_poParent = l_oNode.f_poNode;
			if ( a_tKey < static_cast< HNode < tType > * > ( l_oNode.f_poNode )->f_tKey )
				{
				M_ASSERT ( ! l_oNode.f_poNode->f_poLeft );
				l_oNode.f_poNode->f_poLeft = l_poNode;
				}
			else
				{
				M_ASSERT ( ! l_oNode.f_poNode->f_poRight );
				l_oNode.f_poNode->f_poRight = l_poNode;
				}
			insert_rebalance ( l_poNode );
			}
		else
			{
			f_poRoot = l_poNode;
			f_poRoot->f_eColor = HAbstractNode::D_BLACK;
			}
		}
	M_ASSERT ( ( ! f_poRoot ) || ( f_poRoot->f_poParent == NULL ) );
	M_ASSERT ( ( ! f_poRoot ) || ( f_poRoot->f_eColor == HAbstractNode::D_BLACK ) );
	return;
	}

template < typename tType >
void HSBBSTree::remove ( tType const & a_tKey )
	{
	if ( f_poRoot )
		{
		ONodePtr l_oNode = find_node ( a_tKey );
		if ( l_oNode.f_bExists )
			{
			remove_node ( l_oNode.f_poNode );
			return;
			}
		}
	M_THROW ( n_ppcErrMsgHSBBSTree [ ERROR::E_NON_EXISTING_KEY ],
			static_cast < int > ( ERROR::E_NON_EXISTING_KEY ) );
	}

template < typename tType >
HSBBSTree::HIterator HSBBSTree::find ( tType const & a_tKey ) const
	{
	ONodePtr l_oNodePtr = find_node ( a_tKey );
	return ( HIterator ( l_oNodePtr.f_bExists ? l_oNodePtr.f_poNode : NULL ) );
	}

template < typename tType >
typename HSBBSTree::ONodePtr HSBBSTree::find_node ( tType const & a_tKey ) const
	{
	ONodePtr l_oNodePtr;
	if ( f_poRoot )
		{
		l_oNodePtr.f_poNode = f_poRoot;
		while ( ! l_oNodePtr.f_bExists )
			{
			if ( a_tKey < static_cast < HNode < tType > * > ( l_oNodePtr.f_poNode )->f_tKey )
				{
				if ( l_oNodePtr.f_poNode->f_poLeft )
					l_oNodePtr.f_poNode = l_oNodePtr.f_poNode->f_poLeft;
				else
					break;
				}
			else if ( static_cast < HNode < tType > * > ( l_oNodePtr.f_poNode )->f_tKey < a_tKey )
				{
				if ( l_oNodePtr.f_poNode->f_poRight )
					l_oNodePtr.f_poNode = l_oNodePtr.f_poNode->f_poRight;
				else
					break;
				}
			else
				l_oNodePtr.f_bExists = true;
			}
		}
	return ( l_oNodePtr );
	}

}

}

#endif /* not __YAAL_HCORE_HSBBSTREE_H */


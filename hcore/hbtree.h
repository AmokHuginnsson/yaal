/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hbtree.h - this file is integral part of `stdhapi' project.

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

#ifndef __STDHAPI_HCORE_HBTREE_H
#define __STDHAPI_HCORE_HBTREE_H

#line 31

#include "hexception.h"

namespace stdhapi
{

namespace hcore
{

#define D_CVSID_HBTREE_H "$CVSHeader$"

template < typename tType >
class HBTree
	{
protected:
	/*{*/
	class HNode;
	struct ONodePtr
		{
		HNode * f_poNode;
		bool f_bExists;
		ONodePtr ( void ) : f_poNode ( NULL ), f_bExists ( false ) { }
		};
	class HNode
		{
		typedef enum
			{
			D_RED,
			D_BLACK
			} color_t;
	protected:
		/*{*/
		color_t f_eColor;
		HNode * f_poParent;
		HNode * f_poLeft;
		HNode * f_poRight;
		tType f_tKey;
		/*}*/
	public:
	protected:
		/*{*/
		HNode ( HNode * const, tType const & );
		virtual ~HNode ( void );
		void bud ( tType const &, HNode * const & );
		ONodePtr find ( tType const & );
		void insert_rebalance_red_uncle ( void );
		void insert_rebalance_black_uncle ( HNode * );
		void insert_rebalance ( void );
		/*}*/
	private:
		/*{*/
		HNode ( HNode const & );
		HNode & operator = ( HNode const & );
		/*}*/
		friend class HBTree < tType >;
		};
	HNode * f_poRoot;
	/*}*/
public:
	/*{*/
	HBTree ( void );
	virtual ~HBTree ( void );
	void insert ( tType const & );
	void remove ( tType const & );
	void find ( tType const & );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

template < typename tType >
HBTree < tType >::HNode::HNode ( HNode * const a_poParent, tType const & a_tKey )
	: f_eColor ( D_RED ), f_poParent ( a_poParent ),
	f_poLeft ( NULL ), f_poRight ( NULL ), f_tKey ( a_tKey )
	{
	return;
	}

template < typename tType >
HBTree < tType >::HNode::~HNode ( void )
	{
	if ( f_poLeft )
		delete f_poLeft;
	f_poLeft = NULL;
	if ( f_poRight )
		delete f_poRight;
	f_poRight = NULL;
	return;
	}

template < typename tType >
typename HBTree < tType >::ONodePtr HBTree < tType >::HNode::find ( tType const & a_tKey )
	{
	ONodePtr l_oNodePtr;
	l_oNodePtr.f_poNode = this;
	if ( a_tKey < f_tKey )
		{
		if ( f_poLeft )
			return ( f_poLeft->find ( a_tKey ) );
		}
	else if ( a_tKey > f_tKey )
		{
		if ( f_poRight )
			return ( f_poRight->find ( a_tKey ) );
		}
	else
		l_oNodePtr.f_bExists = true;
	return ( l_oNodePtr );
	}

template < typename tType >
void HBTree < tType >::HNode::bud ( tType const & a_tKey, HNode * const & a_rpoRoot )
	{
	HNode * l_poNode = new HNode ( this, a_tKey );
	if ( a_tKey < f_tKey )
		f_poLeft = l_poNode;
	else
		f_poRight = l_poNode;
	if ( f_eColor == D_RED )
		l_poNode->insert_rebalance ( );
	return;
	}

template < typename tType >
void HBTree < tType >::HNode::insert_rebalance ( void )
	{
	if ( f_poParent )
		{
		HNode * l_poGrandpa = f_poParent->f_poParent;
		if ( l_poGrandpa )
			{
			if ( l_poGrandpa->f_poLeft && l_poGrandpa->f_poRight
					&& ( l_poGrandpa->f_poLeft->f_eColor == D_RED )
					&& ( l_poGrandpa->f_poRight->f_eColor == D_RED ) )
				l_poGrandpa->insert_rebalance_red_uncle ( );
			else
				f_poParent->insert_rebalance_black_uncle ( this );
			}
		}
	else
		f_eColor = D_BLACK;
	return;
	}

template < typename tType >
void HBTree < tType >::HNode::insert_rebalance_red_uncle ( void )
	{
	f_poLeft->f_eColor = D_BLACK;
	f_poRight->f_eColor = D_BLACK;
	f_eColor = D_RED;
	insert_rebalance ( );
	return;
	}

template < typename tType >
void HBTree < tType >::HNode::insert_rebalance_black_uncle ( HNode * a_poNewOne )
	{
	bool l_bRotation = false;
	if ( ( f_poParent->f_poLeft == this ) && ( a_poNewOne == f_poRight ) )
		a_poNewOne->f_poLeft = this, l_bRotation = true;
	else if ( ( f_poParent->f_poRight == this ) && ( a_poNewOne == f_poLeft ) )
		a_poNewOne->f_poRight = this, l_bRotation = true;
	if ( l_bRotation )
		{
		a_poNewOne->f_poParent = f_poParent;
		f_poParent = a_poNewOne;
		f_poLeft = NULL;
		f_poRight = NULL;
		a_poNewOne = this;
		}

	return;
	}

template < typename tType >
HBTree < tType >::HBTree ( void ) : f_poRoot ( NULL )
	{
	return;
	}

template < typename tType >
HBTree < tType >::~HBTree ( void )
	{
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	return;
	}

template < typename tType >
void HBTree < tType >::insert ( tType const & a_tKey )
	{
	if ( f_poRoot )
		{
		ONodePtr l_oNode = f_poRoot->find ( a_tKey );
		if ( l_oNode.f_bExists )
			l_oNode.f_poNode->f_tKey = a_tKey;
		else
			l_oNode.f_poNode->bud ( a_tKey, f_poRoot );
		}
	else
		{
		f_poRoot = new HNode ( NULL, a_tKey );
		f_poRoot->f_eColor = HNode::D_BLACK;
		}
	return;
	}

template < typename tType >
void HBTree < tType >::remove ( tType const & )
	{
	return;
	}

}

}

#endif /* not __STDHAPI_HCORE_HBTREE_H */


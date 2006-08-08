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

#define D_CVSID_HBTREE_H "$CVSHeader$"

#include "hexception.h"

namespace stdhapi
{

namespace hcore
{

extern char const * const n_ppcErrMsgHBTree [ ];

template < typename tType >
class HBTree
	{
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_NON_EXISTING_KEY
			} error_t;
		};
	class HIterator;
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
		void insert_rebalance_red_uncle ( void );
		void insert_rebalance_black_uncle ( HNode * );
		void rotate_left ( HNode * );
		void rotate_right ( HNode * );
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
	HIterator find ( tType const & );
	/*}*/
protected:
	/*{*/
	ONodePtr find_node ( tType const & );
	/*}*/
	};

template < typename tType >
class HBTree < tType >::HIterator
	{
protected:
	/*{*/
	HNode * f_poHook;
	HNode * f_poCurrent;
	/*}*/
public:
	/*{*/
	HIterator ( void );
	HIterator ( HIterator const & );
	HIterator & operator ++ ( void )
		{
		M_PROLOG
		return ( * this );
		M_EPILOG
		}
	HIterator const operator ++ ( int )
		{
		M_PROLOG
		HIterator l_oIterator ( * this );
		++ ( * this );
		return ( l_oIterator );
		M_EPILOG
		}
	HIterator & operator -- ( void )
		{
		M_PROLOG
		return ( * this );
		M_EPILOG
		}
	HIterator const operator -- ( int )
		{
		M_PROLOG
		HIterator l_oIterator ( * this );
		-- ( * this );
		return ( l_oIterator );
		M_EPILOG
		}
	HIterator & operator = ( HIterator const & );
	bool operator == ( HIterator const & ) const;
	bool operator != ( HIterator const & ) const;
	tType & operator * ( void );
	tType * operator -> ( void );
	/*}*/
protected:
	/*{*/
	friend class HBTree < tType >;
	explicit HIterator ( HNode * const, HNode * const );
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
	if ( ( a_poNewOne == f_poRight ) && ( f_poParent->f_poLeft == this ) )
		rotate_left ( a_poNewOne ), a_poNewOne = this;
	else if ( ( a_poNewOne == f_poLeft ) && ( f_poParent->f_poRight == this ) )
		rotate_right ( a_poNewOne ), a_poNewOne = this;
	a_poNewOne->f_poParent->f_eColor = D_BLACK;
	a_poNewOne->f_poParent->f_poParent->f_eColor = D_RED;
	if ( ( a_poNewOne == a_poNewOne->f_poParent->f_poLeft )
			&& ( a_poNewOne->f_poParent == a_poNewOne->f_poParent->f_poParent->f_poLeft ) )
		a_poNewOne->f_poParent->f_poParent->rotate_right ( a_poNewOne->f_poParent );
	else
		a_poNewOne->f_poParent->f_poParent->rotate_left ( a_poNewOne->f_poParent );
	return;
	}

template < typename tType >
void HBTree < tType >::HNode::rotate_left ( HNode * a_poOne )
	{
	HNode * l_poNode1 = NULL;
	HNode * l_poNode2 = NULL;
	HNode * l_poNode3 = NULL;
	if ( f_poParent )
		f_poParent->f_poLeft = a_poOne;
	l_poNode1 = f_poLeft;
	l_poNode2 = a_poOne->f_poLeft;
	l_poNode3 = a_poOne->f_poRight;
	f_poLeft = l_poNode1;
	f_poRight = l_poNode2;
	a_poOne->f_poLeft = this;
	a_poOne->f_poRight = l_poNode3;
	return;
	}

template < typename tType >
void HBTree < tType >::HNode::rotate_right ( HNode * a_poOne )
	{
	HNode * l_poNode1 = NULL;
	HNode * l_poNode2 = NULL;
	HNode * l_poNode3 = NULL;
	if ( f_poParent )
		f_poParent->f_poRight = a_poOne;
	l_poNode1 = f_poRight;
	l_poNode2 = a_poOne->f_poLeft;
	l_poNode3 = a_poOne->f_poLeft;
	f_poRight = l_poNode1;
	f_poLeft = l_poNode2;
	a_poOne->f_poRight = this;
	a_poOne->f_poLeft = l_poNode3;
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
		ONodePtr l_oNode = find_node ( a_tKey );
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
void HBTree < tType >::remove ( tType const & a_tKey )
	{
	if ( f_poRoot )
		{
		ONodePtr l_oNode = find_node ( a_tKey );
		if ( l_oNode.f_bExists )
			{
			l_oNode.f_poNode->f_tKey = a_tKey;
			return;
			}
		}
	M_THROW ( n_ppcErrMsgHBTree [ HBTree::E_NON_EXISTING_KEY ],
			static_cast < int > ( HBTree::E_NON_EXISTING_KEY ) );
	}

template < typename tType >
typename HBTree < tType >::ONodePtr HBTree < tType >::find_node ( tType const & a_tKey )
	{
	ONodePtr l_oNodePtr;
	if ( f_poRoot )
		{
		l_oNodePtr.f_poNode = f_poRoot;
		while ( ! l_oNodePtr.f_bExists )
			{
			if ( a_tKey < l_oNodePtr.f_poNode->f_tKey )
				{
				if ( l_oNodePtr.f_poNode->f_poLeft )
					l_oNodePtr.f_poNode = l_oNodePtr.f_poNode->f_poLeft;
				else
					break;
				}
			else if ( a_tKey > l_oNodePtr.f_poNode->f_tKey )
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

#endif /* not __STDHAPI_HCORE_HBTREE_H */


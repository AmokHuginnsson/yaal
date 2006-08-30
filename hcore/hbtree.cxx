/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hbtree.c - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hbtree.h"
M_VCSTID ( D_VCSID_HBTREE_H )

namespace yaal
{

namespace hcore
{

char const * const g_ppcErrMsgHBTree [ 4 ] =
	{
	_ ( "ok" ),
/* HBTree::E_NON_EXISTING_KEY */			_ ( "key does not exists" ),
	};

HBTree::HIterator::HIterator ( HBTree::HAbstractNode * const a_poNode )
	: f_poCurrent ( a_poNode )
	{
	return;
	}

bool HBTree::HIterator::operator == ( HBTree::HIterator const & a_roIterator ) const
	{
	return ( f_poCurrent == a_roIterator.f_poCurrent );
	}

bool HBTree::HIterator::operator != ( HBTree::HIterator const & a_roIterator ) const
	{
	return ( f_poCurrent != a_roIterator.f_poCurrent );
	}

HBTree::HIterator & HBTree::HIterator::operator ++ ( void )
	{
	M_PROLOG
	HAbstractNode * l_poLastNode = f_poCurrent;
	while ( f_poCurrent )
		{
		if ( f_poCurrent->f_poRight && ( f_poCurrent->f_poRight != l_poLastNode ) )
			{
			f_poCurrent = f_poCurrent->f_poRight;
			break;
			}
		else
			{
			l_poLastNode = f_poCurrent;
			f_poCurrent = f_poCurrent->f_poParent;
			}
		}
	return ( * this );
	M_EPILOG
	}

HBTree::HIterator const HBTree::HIterator::operator ++ ( int )
	{
	M_PROLOG
	HIterator l_oIterator ( * this );
	++ ( * this );
	return ( l_oIterator );
	M_EPILOG
	}

HBTree::HIterator & HBTree::HIterator::operator -- ( void )
	{
	M_PROLOG
	HAbstractNode * l_poLastNode = f_poCurrent;
	while ( f_poCurrent )
		{
		if ( f_poCurrent->f_poLeft && ( f_poCurrent->f_poLeft != l_poLastNode ) )
			{
			f_poCurrent = f_poCurrent->f_poLeft;
			break;
			}
		else
			{
			l_poLastNode = f_poCurrent;
			f_poCurrent = f_poCurrent->f_poParent;
			}
		}
	return ( * this );
	M_EPILOG
	}

HBTree::HIterator const HBTree::HIterator::operator -- ( int )
	{
	M_PROLOG
	HIterator l_oIterator ( * this );
	-- ( * this );
	return ( l_oIterator );
	M_EPILOG
	}

HBTree::HAbstractNode::HAbstractNode ( void )
	: f_eColor ( D_RED ), f_poParent ( NULL ),
	f_poLeft ( NULL ), f_poRight ( NULL )
	{
	return;
	}

HBTree::HAbstractNode::~HAbstractNode ( void )
	{
	if ( f_poLeft )
		delete f_poLeft;
	f_poLeft = NULL;
	if ( f_poRight )
		delete f_poRight;
	f_poRight = NULL;
	return;
	}

void HBTree::HAbstractNode::set_child ( HAbstractNode * a_poWhich, HAbstractNode * a_poNew )
	{
	M_ASSERT ( ( a_poWhich == f_poLeft ) || ( a_poWhich == f_poRight ) );
	if ( a_poWhich == f_poLeft )
		f_poLeft = a_poNew;
	else
		f_poRight = a_poNew;
	return;
	}

HBTree::HBTree ( void ) : f_poRoot ( NULL ), f_lQuantity ( 0 )
	{
	return;
	}

HBTree::~HBTree ( void )
	{
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	return;
	}

void HBTree::insert_rebalance ( HAbstractNode * a_poNode )
	{
	for ( ; ; )
		{
		if ( a_poNode->f_poParent )
			{
			if ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_BLACK )
				break;
			HAbstractNode * l_poGrandpa = a_poNode->f_poParent->f_poParent;
			if ( l_poGrandpa )
				{
				if ( l_poGrandpa->f_poLeft && l_poGrandpa->f_poRight
						&& ( l_poGrandpa->f_poLeft->f_eColor == HAbstractNode::D_RED )
						&& ( l_poGrandpa->f_poRight->f_eColor == HAbstractNode::D_RED ) )
					{
					l_poGrandpa->f_poLeft->f_eColor = HAbstractNode::D_BLACK;
					l_poGrandpa->f_poRight->f_eColor = HAbstractNode::D_BLACK;
					l_poGrandpa->f_eColor = HAbstractNode::D_RED;
					a_poNode = l_poGrandpa;
					continue;
					}
				else
					{
					if ( ( a_poNode == a_poNode->f_poParent->f_poRight ) && ( l_poGrandpa->f_poLeft == a_poNode->f_poParent ) )
						rotate_left ( a_poNode = a_poNode->f_poParent );
					else if ( ( a_poNode == a_poNode->f_poParent->f_poLeft ) && ( l_poGrandpa->f_poRight == a_poNode->f_poParent ) )
						rotate_right ( a_poNode = a_poNode->f_poParent );
					a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
					a_poNode->f_poParent->f_poParent->f_eColor = HAbstractNode::D_RED;
					if ( ( a_poNode == a_poNode->f_poParent->f_poLeft )
							&& ( a_poNode->f_poParent == a_poNode->f_poParent->f_poParent->f_poLeft ) )
						rotate_right ( a_poNode->f_poParent->f_poParent );
					else
						{
						M_ASSERT ( ( a_poNode == a_poNode->f_poParent->f_poRight )
								&& ( a_poNode->f_poParent == a_poNode->f_poParent->f_poParent->f_poRight ) );
						rotate_left ( a_poNode->f_poParent->f_poParent );
						}
					}
				}
			}
		else
			a_poNode->f_eColor = HAbstractNode::D_BLACK;
		break;
		}
	return;
	}

void HBTree::rotate_left ( HAbstractNode * a_poNode )
	{
	if ( a_poNode->f_poParent )
		a_poNode->f_poParent->f_poLeft = a_poNode->f_poRight;
	else
		f_poRoot = a_poNode->f_poRight;
	HAbstractNode * l_poNode1 = a_poNode->f_poLeft;
	HAbstractNode * l_poNode2 = a_poNode->f_poRight->f_poLeft;
	HAbstractNode * l_poNode3 = a_poNode->f_poRight->f_poRight;
	HAbstractNode * l_poParent = a_poNode->f_poParent;
	a_poNode->f_poParent = a_poNode->f_poRight;
	a_poNode->f_poLeft = l_poNode1;
	a_poNode->f_poRight = l_poNode2;
	a_poNode->f_poParent->f_poLeft = a_poNode;
	a_poNode->f_poParent->f_poRight = l_poNode3;
	a_poNode->f_poParent->f_poParent = l_poParent;
	return;
	}

void HBTree::rotate_right ( HAbstractNode * a_poNode )
	{
	if ( a_poNode->f_poParent )
		a_poNode->f_poParent->f_poRight = a_poNode->f_poLeft;
	else
		f_poRoot = a_poNode->f_poLeft;
	HAbstractNode * l_poNode1 = a_poNode->f_poRight;
	HAbstractNode * l_poNode2 = a_poNode->f_poLeft->f_poRight;
	HAbstractNode * l_poNode3 = a_poNode->f_poLeft->f_poLeft;
	HAbstractNode * l_poParent = a_poNode->f_poParent;
	a_poNode->f_poParent = a_poNode->f_poLeft;
	a_poNode->f_poRight = l_poNode1;
	a_poNode->f_poLeft = l_poNode2;
	a_poNode->f_poParent->f_poRight = a_poNode;
	a_poNode->f_poParent->f_poLeft = l_poNode3;
	a_poNode->f_poParent->f_poParent = l_poParent;
	return;
	}

void HBTree::remove_node ( HAbstractNode * a_poNode )
	{
	if ( a_poNode->f_poLeft && a_poNode->f_poRight ) /* both children exists */
		{
		HAbstractNode * l_poNode = a_poNode->f_poLeft;
		while ( l_poNode->f_poRight )
			l_poNode = l_poNode->f_poRight;
		swap ( a_poNode, l_poNode );
		a_poNode = l_poNode;
		}
	if ( a_poNode->f_poLeft )
		{
		a_poNode->f_poLeft->f_poParent = a_poNode->f_poParent;
		if ( a_poNode->f_poParent )
			a_poNode->f_poParent->set_child ( a_poNode, a_poNode->f_poLeft );
		else
			{
			M_ASSERT ( f_poRoot == a_poNode );
			f_poRoot = a_poNode->f_poLeft;
			}
		}
	else if ( a_poNode->f_poRight )
		{
		a_poNode->f_poRight->f_poParent = a_poNode->f_poParent;
		if ( a_poNode->f_poParent )
			a_poNode->f_poParent->set_child ( a_poNode, a_poNode->f_poRight );
		else
			{
			M_ASSERT ( f_poRoot == a_poNode );
			f_poRoot = a_poNode->f_poRight;
			}
		}
	remove_rebalance ( a_poNode );
	if ( a_poNode->f_poParent &&
			! ( a_poNode->f_poLeft || a_poNode->f_poRight ) )
		a_poNode->f_poParent->set_child ( a_poNode, NULL );
	a_poNode->f_poLeft = a_poNode->f_poRight = NULL;
	delete a_poNode;
	f_lQuantity --;
	if ( ! f_lQuantity )
		f_poRoot = NULL;
	M_ASSERT ( ! ( f_poRoot && f_poRoot->f_poParent ) ); /* very tricky :^) */
	}

void HBTree::remove_rebalance ( HAbstractNode * a_poNode )
	{
	M_ASSERT ( a_poNode );
	HAbstractNode * l_poChild = a_poNode->f_poLeft ? a_poNode->f_poLeft : a_poNode->f_poRight;
	M_ASSERT ( a_poNode->f_poParent == l_poChild->f_poParent );
	if ( a_poNode->f_eColor == HAbstractNode::D_BLACK )
		{
		if ( l_poChild && l_poChild->f_eColor == HAbstractNode::D_RED )
			l_poChild->f_eColor = HAbstractNode::D_BLACK;
		else
			{
			for ( ; ; ) /* tail recursion may be easily changed to iteration */
				{
				if ( a_poNode->f_poParent == NULL )
					f_poRoot->f_eColor = HAbstractNode::D_BLACK;
				else /* hard part starts here */
					{
					if ( l_poChild )
						a_poNode = l_poChild;
					/* after this line a_poNode is a pivot of rebalancing
					 * no matter if it's child of removed node, or the node it self */
					HAbstractNode * l_poSibling = sibling ( a_poNode );
					if ( l_poSibling->f_eColor == HAbstractNode::D_RED )
						{
						/* sibling must exists because all black nodes have siblings,
						 * moreover if the sibling is red, we know for sure that parent
						 * is lack, due to rule that prevents two subsequent red nodes */
						l_poSibling->f_eColor = HAbstractNode::D_BLACK;
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_RED;
						if ( a_poNode == a_poNode->f_poParent->f_poLeft )
							rotate_left ( a_poNode->f_poParent );
						else
							rotate_right ( a_poNode->f_poParent );
						}
					if ( ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_BLACK )
							&& ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
							&& ( l_poSibling->f_poLeft || ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) )
							&& ( l_poSibling->f_poRight || ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) ) )
						{
						l_poSibling->f_eColor = HAbstractNode::D_RED;
						a_poNode = a_poNode->f_poParent;
						continue;
						}
					else if ( ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_RED )
							&& ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
							&& ( l_poSibling->f_poLeft || ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) )
							&& ( l_poSibling->f_poRight || ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) ) )
						{
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
						l_poSibling->f_eColor = HAbstractNode::D_RED;
						}
					else
						{
						if ( ( a_poNode == a_poNode->f_poParent->f_poLeft ) && ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
								&& ( l_poSibling->f_poLeft && l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_RED )
								&& ( l_poSibling->f_poRight || l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) )
							{
							l_poSibling->f_eColor = HAbstractNode::D_RED;
							l_poSibling->f_poLeft->f_eColor = HAbstractNode::D_BLACK;
							rotate_right ( l_poSibling );
							}
						else if ( ( a_poNode == a_poNode->f_poParent->f_poRight ) && ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
								&& ( l_poSibling->f_poRight && l_poSibling->f_poRight->f_eColor == HAbstractNode::D_RED )
								&& ( l_poSibling->f_poLeft || l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) )
							{
							l_poSibling->f_eColor = HAbstractNode::D_RED;
							l_poSibling->f_poRight->f_eColor = HAbstractNode::D_BLACK;
							rotate_left ( l_poSibling );
							}
						l_poSibling->f_eColor = a_poNode->f_poParent->f_eColor;
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
						if ( a_poNode == a_poNode->f_poParent->f_poLeft )
							{
							M_ASSERT ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_RED );
							l_poSibling->f_poRight->f_eColor = HAbstractNode::D_BLACK;
							rotate_left ( a_poNode->f_poParent );
							}
						else
							{
							M_ASSERT ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_RED );
							l_poSibling->f_poLeft->f_eColor = HAbstractNode::D_BLACK;
							rotate_right ( a_poNode->f_poParent );
							}
						}
					}
				break;
				}
			}
		}
	return;
	}

HBTree::HIterator HBTree::begin ( void )
	{
	HAbstractNode * l_poNode = f_poRoot;
	while ( l_poNode && l_poNode->f_poLeft )
		l_poNode = l_poNode->f_poLeft;
	return ( HIterator ( l_poNode ) );
	}

HBTree::HIterator HBTree::end ( void )
	{
	return ( HIterator ( NULL ) );
	}

HBTree::HIterator HBTree::rbegin ( void )
	{
	HAbstractNode * l_poNode = f_poRoot;
	while ( l_poNode && l_poNode->f_poRight )
		l_poNode = l_poNode->f_poRight;
	return ( HIterator ( l_poNode ) );
	}

HBTree::HIterator HBTree::rend ( void )
	{
	return ( HIterator ( NULL ) );
	}

void HBTree::swap ( HAbstractNode * a_poFirst, HAbstractNode * a_poSecond )
	{
	M_ASSERT ( a_poFirst && a_poSecond );
	M_ASSERT ( a_poFirst != a_poSecond );
	if ( a_poFirst == f_poRoot )
		f_poRoot = a_poSecond;
	else if ( a_poSecond == f_poRoot )
		f_poRoot = a_poFirst;
	HAbstractNode * l_poFirstParent = a_poFirst->f_poParent;
	HAbstractNode * l_poFirstLeft = a_poFirst->f_poLeft;
	HAbstractNode * l_poFirstRight = a_poFirst->f_poRight;
	HAbstractNode * l_poSecondParent = a_poSecond->f_poParent;
	HAbstractNode * l_poSecondLeft = a_poSecond->f_poLeft;
	HAbstractNode * l_poSecondRight = a_poSecond->f_poRight;
	if ( l_poFirstParent == l_poSecondParent ) /* siblings */
		{
		if ( l_poFirstParent )
			{
			if ( l_poFirstParent->f_poLeft == a_poFirst )
				{
				l_poFirstParent->f_poLeft = a_poSecond;
				l_poFirstParent->f_poRight = a_poFirst;
				}
			else
				{
				l_poFirstParent->f_poLeft = a_poFirst;
				l_poFirstParent->f_poRight = a_poSecond;
				}
			}
		}
	else /* not siblings */
		{
		if ( l_poFirstParent )
			{
			if ( l_poFirstParent->f_poLeft == a_poFirst )
				l_poFirstParent->f_poLeft = a_poSecond;
			else
				l_poFirstParent->f_poRight = a_poSecond;
			}
		if ( l_poSecondParent )
			{
			if ( l_poSecondParent->f_poLeft == a_poSecond )
				l_poSecondParent->f_poLeft = a_poFirst;
			else
				l_poSecondParent->f_poRight = a_poFirst;
			}
		}
	if ( l_poFirstLeft )
		l_poFirstLeft->f_poParent = a_poSecond;
	if ( l_poFirstRight )
		l_poFirstRight->f_poParent = a_poSecond;
	if ( l_poSecondLeft )
		l_poSecondLeft->f_poParent = a_poFirst;
	if ( l_poSecondRight )
		l_poSecondRight->f_poParent = a_poFirst;
	a_poFirst->f_poParent = l_poSecondParent;
	a_poFirst->f_poLeft = l_poSecondLeft;
	a_poFirst->f_poRight = l_poSecondRight;
	a_poSecond->f_poParent = l_poFirstParent;
	a_poSecond->f_poLeft = l_poFirstLeft;
	a_poSecond->f_poRight = l_poFirstRight;
	HAbstractNode::color_t l_eColor = a_poFirst->f_eColor;
	a_poFirst->f_eColor = a_poSecond->f_eColor;
	a_poSecond->f_eColor = l_eColor;
	return;
	}

HBTree::HAbstractNode * HBTree::sibling ( HAbstractNode * a_poNode )
	{
	if ( a_poNode->f_poParent->f_poLeft == a_poNode )
		return ( a_poNode->f_poParent->f_poRight );
	M_ASSERT ( a_poNode->f_poParent->f_poRight == a_poNode );
	return ( a_poNode->f_poParent->f_poLeft );
	}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsbbstree.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
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

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hsbbstree.hxx"

namespace yaal
{

namespace hcore
{

char const* const n_ppcErrMsgHSBBSTree[ 4 ] =
	{
	_( "ok" ),
/* HSBBSTree::E_NON_EXISTING_KEY */			_( "key does not exists" ),
/* HSBBSTree::E_NIL_ITERATOR */					_( "dereferencing nil iterator" )
	};

HSBBSTree::HIterator::HIterator( void )
	: f_poCurrent( NULL )
	{
	return;
	}

HSBBSTree::HIterator::HIterator( HSBBSTree::HAbstractNode* const a_poNode )
	: f_poCurrent( a_poNode )
	{
	return;
	}

bool HSBBSTree::HIterator::operator == ( HSBBSTree::HIterator const& a_roIterator ) const
	{
	return ( f_poCurrent == a_roIterator.f_poCurrent );
	}

bool HSBBSTree::HIterator::operator != ( HSBBSTree::HIterator const& a_roIterator ) const
	{
	return ( f_poCurrent != a_roIterator.f_poCurrent );
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator ++ ( void )
	{
	M_PROLOG
	HAbstractNode * l_poLastNode = f_poCurrent;
	while ( f_poCurrent )
		{
		if ( f_poCurrent->f_poRight && ( f_poCurrent->f_poRight != l_poLastNode ) )
			{
			f_poCurrent = f_poCurrent->f_poRight;
			while ( f_poCurrent->f_poLeft )
				f_poCurrent = f_poCurrent->f_poLeft;
			break;
			}
		else
			{
			l_poLastNode = f_poCurrent;
			f_poCurrent = f_poCurrent->f_poParent;
			if ( f_poCurrent && ( l_poLastNode == f_poCurrent->f_poLeft ) )
				break;
			}
		}
	return ( *this );
	M_EPILOG
	}

HSBBSTree::HIterator const HSBBSTree::HIterator::operator ++ ( int )
	{
	M_PROLOG
	HIterator l_oIterator( *this );
	++ ( *this );
	return ( l_oIterator );
	M_EPILOG
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator -- ( void )
	{
	M_PROLOG
	HAbstractNode * l_poLastNode = f_poCurrent;
	while ( f_poCurrent )
		{
		if ( f_poCurrent->f_poLeft && ( f_poCurrent->f_poLeft != l_poLastNode ) )
			{
			f_poCurrent = f_poCurrent->f_poLeft;
			while ( f_poCurrent->f_poRight )
				f_poCurrent = f_poCurrent->f_poRight;
			break;
			}
		else
			{
			l_poLastNode = f_poCurrent;
			f_poCurrent = f_poCurrent->f_poParent;
			if ( f_poCurrent && ( l_poLastNode == f_poCurrent->f_poRight ) )
				break;
			}
		}
	return ( *this );
	M_EPILOG
	}

HSBBSTree::HIterator const HSBBSTree::HIterator::operator -- ( int )
	{
	M_PROLOG
	HIterator l_oIterator( *this );
	-- ( *this );
	return ( l_oIterator );
	M_EPILOG
	}

HSBBSTree::HIterator::HIterator ( HSBBSTree::HIterator const & a_roIterator )
	: f_poCurrent ( a_roIterator.f_poCurrent )
	{
	return;
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator = ( HSBBSTree::HIterator const& a_roIterator )
	{
	if ( &a_roIterator != this )
		f_poCurrent = a_roIterator.f_poCurrent;
	return ( *this );
	}

HSBBSTree::HAbstractNode::HAbstractNode( void )
	: f_eColor( D_RED ), f_poParent( NULL ),
	f_poLeft( NULL ), f_poRight( NULL )
	{
	return;
	}

HSBBSTree::HAbstractNode::~HAbstractNode( void )
	{
	if ( f_poLeft )
		delete f_poLeft;
	f_poLeft = NULL;
	if ( f_poRight )
		delete f_poRight;
	f_poRight = NULL;
	return;
	}

void HSBBSTree::HAbstractNode::set_child( HAbstractNode* a_poWhich, HAbstractNode* a_poNew )
	{
	M_ASSERT( ( a_poWhich == f_poLeft ) || ( a_poWhich == f_poRight ) );
	if ( a_poWhich == f_poLeft )
		f_poLeft = a_poNew;
	else
		f_poRight = a_poNew;
	return;
	}

HSBBSTree::HSBBSTree( void ) : f_poRoot( NULL ), f_lSize( 0 )
	{
	return;
	}

HSBBSTree::~HSBBSTree( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

void HSBBSTree::clear( void )
	{
	M_PROLOG
	if ( f_poRoot )
		delete f_poRoot;
	f_poRoot = NULL;
	f_lSize = 0;
	M_EPILOG
	}

void HSBBSTree::insert_rebalance( HAbstractNode* a_poNode )
	{
	M_PROLOG
	for ( ; ; )
		{
		if ( a_poNode->f_poParent )
			{
			if ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_BLACK )
				break;
			HAbstractNode* l_poGrandpa = a_poNode->f_poParent->f_poParent;
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
						rotate_left( a_poNode = a_poNode->f_poParent );
					else if ( ( a_poNode == a_poNode->f_poParent->f_poLeft ) && ( l_poGrandpa->f_poRight == a_poNode->f_poParent ) )
						rotate_right( a_poNode = a_poNode->f_poParent );
					a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
					a_poNode->f_poParent->f_poParent->f_eColor = HAbstractNode::D_RED;
					if ( ( a_poNode == a_poNode->f_poParent->f_poLeft )
							&& ( a_poNode->f_poParent == a_poNode->f_poParent->f_poParent->f_poLeft ) )
						rotate_right( a_poNode->f_poParent->f_poParent );
					else
						{
						M_ASSERT( ( a_poNode == a_poNode->f_poParent->f_poRight )
								&& ( a_poNode->f_poParent == a_poNode->f_poParent->f_poParent->f_poRight ) );
						rotate_left( a_poNode->f_poParent->f_poParent );
						}
					}
				}
			}
		else
			a_poNode->f_eColor = HAbstractNode::D_BLACK;
		break;
		}
	return;
	M_EPILOG
	}

void HSBBSTree::rotate_left( HAbstractNode* a_poNode )
	{
	/* 
	 * At the beggining of left rotation:
	 * a_poNode is parent node (center of the rotation)
	 * a_poNode->f_poRight is child node (lets call it satelite)
	 *
	 * At the end of left rotation:
	 * center of the rotation is left child
	 * of its satelite
	 */
	HAbstractNode* l_poParent = a_poNode->f_poParent;
	HAbstractNode* l_poNode = a_poNode->f_poRight;
	if ( l_poParent )
		{
		if ( l_poParent->f_poLeft == a_poNode )
			l_poParent->f_poLeft = l_poNode;
		else
			{
			M_ASSERT( l_poParent->f_poRight == a_poNode );
			l_poParent->f_poRight = l_poNode;
			}
		}
	else
		f_poRoot = l_poNode;
	a_poNode->f_poParent = l_poNode;
	a_poNode->f_poRight = l_poNode->f_poLeft;
	if ( a_poNode->f_poRight )
		a_poNode->f_poRight->f_poParent = a_poNode;
	l_poNode->f_poLeft = a_poNode;
	l_poNode->f_poParent = l_poParent;
	return;
	}

void HSBBSTree::rotate_right( HAbstractNode* a_poNode )
	{
	HAbstractNode* l_poParent = a_poNode->f_poParent;
	HAbstractNode* l_poNode = a_poNode->f_poLeft;
	if ( l_poParent )
		{
		if ( l_poParent->f_poRight == a_poNode )
			l_poParent->f_poRight = l_poNode;
		else
			{
			M_ASSERT( l_poParent->f_poLeft == a_poNode );
			l_poParent->f_poLeft = l_poNode;
			}
		}
	else
		f_poRoot = l_poNode;
	a_poNode->f_poParent = l_poNode;
	a_poNode->f_poLeft = l_poNode->f_poRight;
	if ( a_poNode->f_poLeft )
		a_poNode->f_poLeft->f_poParent = a_poNode;
	l_poNode->f_poRight = a_poNode;
	l_poNode->f_poParent = l_poParent;
	return;
	}

void HSBBSTree::remove( HIterator const& a_oIt )
	{
	if ( ! a_oIt.f_poCurrent )
		M_THROW( n_ppcErrMsgHSBBSTree[ ERROR::E_NIL_ITERATOR ],
				static_cast<int>( ERROR::E_NIL_ITERATOR ) );
	remove_node( a_oIt.f_poCurrent );
	return;
	}

void HSBBSTree::remove_node( HAbstractNode* a_poNode )
	{
	if ( a_poNode->f_poLeft && a_poNode->f_poRight ) /* both children exists */
		{
		HAbstractNode* l_poNode = a_poNode->f_poLeft;
		while ( l_poNode->f_poRight )
			l_poNode = l_poNode->f_poRight;
		swap( a_poNode, l_poNode );
		}
	if ( a_poNode->f_poLeft )
		{
		M_ASSERT( ! a_poNode->f_poRight );
		a_poNode->f_poLeft->f_poParent = a_poNode->f_poParent;
		if ( a_poNode->f_poParent )
			a_poNode->f_poParent->set_child( a_poNode, a_poNode->f_poLeft );
		else
			{
			M_ASSERT( f_poRoot == a_poNode );
			f_poRoot = a_poNode->f_poLeft;
			}
		}
	else if ( a_poNode->f_poRight )
		{
		a_poNode->f_poRight->f_poParent = a_poNode->f_poParent;
		if ( a_poNode->f_poParent )
			a_poNode->f_poParent->set_child( a_poNode, a_poNode->f_poRight );
		else
			{
			M_ASSERT( f_poRoot == a_poNode );
			f_poRoot = a_poNode->f_poRight;
			}
		}
	remove_rebalance( a_poNode );
	if ( ( a_poNode->f_poParent )
			&& ! ( a_poNode->f_poLeft || a_poNode->f_poRight ) )
		a_poNode->f_poParent->set_child( a_poNode, NULL );
	a_poNode->f_poLeft = a_poNode->f_poRight = NULL;
	delete a_poNode;
	f_lSize --;
	if ( ! f_lSize )
		f_poRoot = NULL;
	M_ASSERT( ! ( f_poRoot && f_poRoot->f_poParent ) ); /* very tricky :^) */
	}

void HSBBSTree::remove_rebalance( HAbstractNode* a_poNode )
	{
	M_ASSERT( a_poNode );
	HAbstractNode* l_poChild = a_poNode->f_poLeft ? a_poNode->f_poLeft : a_poNode->f_poRight;
	M_ASSERT( ! l_poChild || ( a_poNode->f_poParent == l_poChild->f_poParent ) );
	if ( a_poNode->f_eColor == HAbstractNode::D_BLACK )
		{
		if ( l_poChild && l_poChild->f_eColor == HAbstractNode::D_RED )
			l_poChild->f_eColor = HAbstractNode::D_BLACK;
		else
			{
			if ( l_poChild )
				a_poNode = l_poChild;
			/* after this line a_poNode is a pivot of rebalancing
			 * no matter if it's child of removed node, or the node it self */
			for ( int i = 0; ; ++i ) /* tail recursion may be easily changed to iteration */
				{
				if ( a_poNode->f_poParent == NULL )
					f_poRoot->f_eColor = HAbstractNode::D_BLACK;
				else /* hard part starts here */
					{
					HAbstractNode* l_poSibling = sibling ( a_poNode );
					if ( l_poSibling->f_eColor == HAbstractNode::D_RED )
						{
						/* sibling must exists because all black nodes have siblings,
						 * moreover if the sibling is red, we know for sure that parent
						 * is black, due to rule that prevents two subsequent red nodes */
						l_poSibling->f_eColor = HAbstractNode::D_BLACK;
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_RED;
						if ( a_poNode == a_poNode->f_poParent->f_poLeft )
							rotate_left( a_poNode->f_poParent );
						else
							rotate_right( a_poNode->f_poParent );
						l_poSibling = sibling( a_poNode );
						}
					if ( ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_BLACK )
							&& ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
							&& ( ! l_poSibling->f_poLeft || ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) )
							&& ( ! l_poSibling->f_poRight || ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) ) )
						{
						l_poSibling->f_eColor = HAbstractNode::D_RED;
						a_poNode = a_poNode->f_poParent;
						continue;
						}
					else if ( ( a_poNode->f_poParent->f_eColor == HAbstractNode::D_RED )
							&& ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
							&& ( ! l_poSibling->f_poLeft || ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) )
							&& ( ! l_poSibling->f_poRight || ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) ) )
						{
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
						l_poSibling->f_eColor = HAbstractNode::D_RED;
						}
					else
						{
						if ( ( a_poNode == a_poNode->f_poParent->f_poLeft ) && ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
								&& ( l_poSibling->f_poLeft && l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_RED )
								&& ( ! l_poSibling->f_poRight || ( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_BLACK ) ) )
							{
							l_poSibling->f_eColor = HAbstractNode::D_RED;
							l_poSibling->f_poLeft->f_eColor = HAbstractNode::D_BLACK;
							rotate_right( l_poSibling );
							l_poSibling = sibling( a_poNode );
							}
						else if ( ( a_poNode == a_poNode->f_poParent->f_poRight ) && ( l_poSibling->f_eColor == HAbstractNode::D_BLACK )
								&& ( l_poSibling->f_poRight && l_poSibling->f_poRight->f_eColor == HAbstractNode::D_RED )
								&& ( ! l_poSibling->f_poLeft || ( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_BLACK ) ) )
							{
							l_poSibling->f_eColor = HAbstractNode::D_RED;
							l_poSibling->f_poRight->f_eColor = HAbstractNode::D_BLACK;
							rotate_left( l_poSibling );
							l_poSibling = sibling( a_poNode );
							}
						l_poSibling->f_eColor = a_poNode->f_poParent->f_eColor;
						a_poNode->f_poParent->f_eColor = HAbstractNode::D_BLACK;
						if ( a_poNode == a_poNode->f_poParent->f_poLeft )
							{
							M_ASSERT( l_poSibling->f_poRight->f_eColor == HAbstractNode::D_RED );
							l_poSibling->f_poRight->f_eColor = HAbstractNode::D_BLACK;
							rotate_left( a_poNode->f_poParent );
							}
						else
							{
							M_ASSERT( l_poSibling->f_poLeft->f_eColor == HAbstractNode::D_RED );
							l_poSibling->f_poLeft->f_eColor = HAbstractNode::D_BLACK;
							rotate_right( a_poNode->f_poParent );
							}
						}
					}
				break;
				}
			}
		}
	return;
	}

int long HSBBSTree::size( void ) const
	{
	return ( f_lSize );
	}

bool HSBBSTree::empty( void ) const
	{
	return ( ! f_poRoot );
	}

bool HSBBSTree::is_empty( void ) const
	{
	return ( ! f_poRoot );
	}

void HSBBSTree::swap( HSBBSTree& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_lSize, other.f_lSize );
		swap( f_poRoot, other.f_poRoot );
		}
	return;
	}

HSBBSTree::HIterator HSBBSTree::begin( void ) const
	{
	HAbstractNode * l_poNode = f_poRoot;
	while ( l_poNode && l_poNode->f_poLeft )
		l_poNode = l_poNode->f_poLeft;
	return ( HIterator ( l_poNode ) );
	}

HSBBSTree::HIterator HSBBSTree::end ( void ) const
	{
	return ( HIterator ( NULL ) );
	}

HSBBSTree::HIterator HSBBSTree::rbegin( void ) const
	{
	HAbstractNode * l_poNode = f_poRoot;
	while ( l_poNode && l_poNode->f_poRight )
		l_poNode = l_poNode->f_poRight;
	return ( HIterator ( l_poNode ) );
	}

HSBBSTree::HIterator HSBBSTree::rend( void ) const
	{
	return ( HIterator ( NULL ) );
	}

void HSBBSTree::swap( HAbstractNode* a_poFirst, HAbstractNode* a_poSecond )
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
		M_ASSERT ( l_poFirstParent );
		if ( a_poFirst == l_poFirstParent->f_poLeft )
			{
			l_poFirstParent->f_poLeft = a_poSecond;
			l_poFirstParent->f_poRight = a_poFirst;
			}
		else
			{
			M_ASSERT ( a_poFirst == l_poFirstParent->f_poRight );
			l_poFirstParent->f_poLeft = a_poFirst;
			l_poFirstParent->f_poRight = a_poSecond;
			}
		}
	else /* not siblings */
		{
		if ( l_poFirstParent )
			{
			if ( a_poFirst == l_poFirstParent->f_poLeft )
				l_poFirstParent->f_poLeft = a_poSecond;
			else
				{
				M_ASSERT ( a_poFirst == l_poFirstParent->f_poRight );
				l_poFirstParent->f_poRight = a_poSecond;
				}
			}
		if ( l_poSecondParent )
			{
			if ( a_poSecond == l_poSecondParent->f_poLeft )
				l_poSecondParent->f_poLeft = a_poFirst;
			else
				{
				M_ASSERT ( a_poSecond == l_poSecondParent->f_poRight );
				l_poSecondParent->f_poRight = a_poFirst;
				}
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
	if ( l_poSecondParent != a_poFirst )
		a_poFirst->f_poParent = l_poSecondParent;
	else
		a_poFirst->f_poParent = a_poSecond;
	if ( l_poSecondLeft != a_poFirst )
		a_poFirst->f_poLeft = l_poSecondLeft;
	else
		a_poFirst->f_poLeft = a_poSecond;
	if ( l_poSecondRight != a_poFirst )
		a_poFirst->f_poRight = l_poSecondRight;
	else
		a_poFirst->f_poRight = a_poSecond;
	if ( l_poFirstParent != a_poSecond )
		a_poSecond->f_poParent = l_poFirstParent;
	else
		a_poSecond->f_poParent = a_poFirst;
	if ( l_poFirstLeft != a_poSecond )
		a_poSecond->f_poLeft = l_poFirstLeft;
	else
		a_poSecond->f_poLeft = a_poFirst;
	if ( l_poFirstRight != a_poSecond )
		a_poSecond->f_poRight = l_poFirstRight;
	else
		a_poSecond->f_poRight = a_poFirst;
	HAbstractNode::color_t l_eColor = a_poFirst->f_eColor;
	a_poFirst->f_eColor = a_poSecond->f_eColor;
	a_poSecond->f_eColor = l_eColor;
	return;
	}

HSBBSTree::HAbstractNode * HSBBSTree::sibling ( HAbstractNode * a_poNode ) const
	{
	if ( a_poNode->f_poParent->f_poLeft == a_poNode )
		return ( a_poNode->f_poParent->f_poRight );
	M_ASSERT ( a_poNode->f_poParent->f_poRight == a_poNode );
	return ( a_poNode->f_poParent->f_poLeft );
	}

}

}


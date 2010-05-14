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
#include "algorithm.hxx"
#include "hsbbstree.hxx"

namespace yaal
{

namespace hcore
{

char const* const _errMsgHSBBSTree_[ 4 ] =
	{
	_( "ok" ),
/* HSBBSTree::NON_EXISTING_KEY */			_( "key does not exists" ),
/* HSBBSTree::NIL_ITERATOR */					_( "dereferencing nil iterator" )
	};

HSBBSTree::HIterator::HIterator( void )
	: _current( NULL )
	{
	return;
	}

HSBBSTree::HIterator::HIterator( HSBBSTree::HAbstractNode* const node_ )
	: _current( node_ )
	{
	return;
	}

bool HSBBSTree::HIterator::operator == ( HSBBSTree::HIterator const& iterator_ ) const
	{
	return ( _current == iterator_._current );
	}

bool HSBBSTree::HIterator::operator != ( HSBBSTree::HIterator const& iterator_ ) const
	{
	return ( _current != iterator_._current );
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator ++ ( void )
	{
	M_PROLOG
	HAbstractNode * lastNode = _current;
	while ( _current )
		{
		if ( _current->_right && ( _current->_right != lastNode ) )
			{
			_current = _current->_right;
			while ( _current->_left )
				_current = _current->_left;
			break;
			}
		else
			{
			lastNode = _current;
			_current = _current->_parent;
			if ( _current && ( lastNode == _current->_left ) )
				break;
			}
		}
	return ( *this );
	M_EPILOG
	}

HSBBSTree::HIterator const HSBBSTree::HIterator::operator ++ ( int )
	{
	M_PROLOG
	HIterator iterator( *this );
	++ ( *this );
	return ( iterator );
	M_EPILOG
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator -- ( void )
	{
	M_PROLOG
	HAbstractNode * lastNode = _current;
	while ( _current )
		{
		if ( _current->_left && ( _current->_left != lastNode ) )
			{
			_current = _current->_left;
			while ( _current->_right )
				_current = _current->_right;
			break;
			}
		else
			{
			lastNode = _current;
			_current = _current->_parent;
			if ( _current && ( lastNode == _current->_right ) )
				break;
			}
		}
	return ( *this );
	M_EPILOG
	}

HSBBSTree::HIterator const HSBBSTree::HIterator::operator -- ( int )
	{
	M_PROLOG
	HIterator iterator( *this );
	-- ( *this );
	return ( iterator );
	M_EPILOG
	}

HSBBSTree::HIterator::HIterator ( HSBBSTree::HIterator const & iterator_ )
	: _current ( iterator_._current )
	{
	return;
	}

HSBBSTree::HIterator& HSBBSTree::HIterator::operator = ( HSBBSTree::HIterator const& iterator_ )
	{
	if ( &iterator_ != this )
		_current = iterator_._current;
	return ( *this );
	}

HSBBSTree::HAbstractNode::HAbstractNode( void )
	: _color( RED ), _parent( NULL ),
	_left( NULL ), _right( NULL )
	{
	return;
	}

HSBBSTree::HAbstractNode::~HAbstractNode( void )
	{
	if ( _left )
		delete _left;
	_left = NULL;
	if ( _right )
		delete _right;
	_right = NULL;
	return;
	}

void HSBBSTree::HAbstractNode::set_child( HAbstractNode* which_, HAbstractNode* new_ )
	{
	M_ASSERT( ( which_ == _left ) || ( which_ == _right ) );
	if ( which_ == _left )
		_left = new_;
	else
		_right = new_;
	return;
	}

HSBBSTree::HSBBSTree( void ) : _root( NULL ), _size( 0 )
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
	if ( _root )
		delete _root;
	_root = NULL;
	_size = 0;
	M_EPILOG
	}

void HSBBSTree::insert_rebalance( HAbstractNode* node_ )
	{
	M_PROLOG
	for ( ; ; )
		{
		if ( node_->_parent )
			{
			if ( node_->_parent->_color == HAbstractNode::BLACK )
				break;
			HAbstractNode* grandpa = node_->_parent->_parent;
			if ( grandpa )
				{
				if ( grandpa->_left && grandpa->_right
						&& ( grandpa->_left->_color == HAbstractNode::RED )
						&& ( grandpa->_right->_color == HAbstractNode::RED ) )
					{
					grandpa->_left->_color = HAbstractNode::BLACK;
					grandpa->_right->_color = HAbstractNode::BLACK;
					grandpa->_color = HAbstractNode::RED;
					node_ = grandpa;
					continue;
					}
				else
					{
					if ( ( node_ == node_->_parent->_right ) && ( grandpa->_left == node_->_parent ) )
						rotate_left( node_ = node_->_parent );
					else if ( ( node_ == node_->_parent->_left ) && ( grandpa->_right == node_->_parent ) )
						rotate_right( node_ = node_->_parent );
					node_->_parent->_color = HAbstractNode::BLACK;
					node_->_parent->_parent->_color = HAbstractNode::RED;
					if ( ( node_ == node_->_parent->_left )
							&& ( node_->_parent == node_->_parent->_parent->_left ) )
						rotate_right( node_->_parent->_parent );
					else
						{
						M_ASSERT( ( node_ == node_->_parent->_right )
								&& ( node_->_parent == node_->_parent->_parent->_right ) );
						rotate_left( node_->_parent->_parent );
						}
					}
				}
			}
		else
			node_->_color = HAbstractNode::BLACK;
		break;
		}
	return;
	M_EPILOG
	}

void HSBBSTree::rotate_left( HAbstractNode* node_ )
	{
	/* 
	 * At the beggining of left rotation:
	 * node_ is parent node (center of the rotation)
	 * node_->_right is child node (lets call it satelite)
	 *
	 * At the end of left rotation:
	 * center of the rotation is left child
	 * of its satelite
	 */
	HAbstractNode* parent = node_->_parent;
	HAbstractNode* node = node_->_right;
	if ( parent )
		{
		if ( parent->_left == node_ )
			parent->_left = node;
		else
			{
			M_ASSERT( parent->_right == node_ );
			parent->_right = node;
			}
		}
	else
		_root = node;
	node_->_parent = node;
	node_->_right = node->_left;
	if ( node_->_right )
		node_->_right->_parent = node_;
	node->_left = node_;
	node->_parent = parent;
	return;
	}

void HSBBSTree::rotate_right( HAbstractNode* node_ )
	{
	HAbstractNode* parent = node_->_parent;
	HAbstractNode* node = node_->_left;
	if ( parent )
		{
		if ( parent->_right == node_ )
			parent->_right = node;
		else
			{
			M_ASSERT( parent->_left == node_ );
			parent->_left = node;
			}
		}
	else
		_root = node;
	node_->_parent = node;
	node_->_left = node->_right;
	if ( node_->_left )
		node_->_left->_parent = node_;
	node->_right = node_;
	node->_parent = parent;
	return;
	}

void HSBBSTree::remove( HIterator const& it_ )
	{
	if ( ! it_._current )
		M_THROW( _errMsgHSBBSTree_[ ERROR::NIL_ITERATOR ],
				static_cast<int>( ERROR::NIL_ITERATOR ) );
	remove_node( it_._current );
	return;
	}

void HSBBSTree::remove_node( HAbstractNode* node_ )
	{
	if ( node_->_left && node_->_right ) /* both children exists */
		{
		HAbstractNode* node = node_->_left;
		while ( node->_right )
			node = node->_right;
		swap( node_, node );
		}
	if ( node_->_left )
		{
		M_ASSERT( ! node_->_right );
		node_->_left->_parent = node_->_parent;
		if ( node_->_parent )
			node_->_parent->set_child( node_, node_->_left );
		else
			{
			M_ASSERT( _root == node_ );
			_root = node_->_left;
			}
		}
	else if ( node_->_right )
		{
		node_->_right->_parent = node_->_parent;
		if ( node_->_parent )
			node_->_parent->set_child( node_, node_->_right );
		else
			{
			M_ASSERT( _root == node_ );
			_root = node_->_right;
			}
		}
	remove_rebalance( node_ );
	if ( ( node_->_parent )
			&& ! ( node_->_left || node_->_right ) )
		node_->_parent->set_child( node_, NULL );
	node_->_left = node_->_right = NULL;
	delete node_;
	_size --;
	if ( ! _size )
		_root = NULL;
	M_ASSERT( ! ( _root && _root->_parent ) ); /* very tricky :^) */
	}

void HSBBSTree::remove_rebalance( HAbstractNode* node_ )
	{
	M_ASSERT( node_ );
	HAbstractNode* child = node_->_left ? node_->_left : node_->_right;
	M_ASSERT( ! child || ( node_->_parent == child->_parent ) );
	if ( node_->_color == HAbstractNode::BLACK )
		{
		if ( child && child->_color == HAbstractNode::RED )
			child->_color = HAbstractNode::BLACK;
		else
			{
			if ( child )
				node_ = child;
			/* after this line node_ is a pivot of rebalancing
			 * no matter if it's child of removed node, or the node it self */
			for ( int i = 0; ; ++i ) /* tail recursion may be easily changed to iteration */
				{
				if ( node_->_parent == NULL )
					_root->_color = HAbstractNode::BLACK;
				else /* hard part starts here */
					{
					HAbstractNode* sibling = get_sibling( node_ );
					if ( sibling->_color == HAbstractNode::RED )
						{
						/* sibling must exists because all black nodes have siblings,
						 * moreover if the sibling is red, we know for sure that parent
						 * is black, due to rule that prevents two subsequent red nodes */
						sibling->_color = HAbstractNode::BLACK;
						node_->_parent->_color = HAbstractNode::RED;
						if ( node_ == node_->_parent->_left )
							rotate_left( node_->_parent );
						else
							rotate_right( node_->_parent );
						sibling = get_sibling( node_ );
						}
					if ( ( node_->_parent->_color == HAbstractNode::BLACK )
							&& ( sibling->_color == HAbstractNode::BLACK )
							&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) )
							&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) )
						{
						sibling->_color = HAbstractNode::RED;
						node_ = node_->_parent;
						continue;
						}
					else if ( ( node_->_parent->_color == HAbstractNode::RED )
							&& ( sibling->_color == HAbstractNode::BLACK )
							&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) )
							&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) )
						{
						node_->_parent->_color = HAbstractNode::BLACK;
						sibling->_color = HAbstractNode::RED;
						}
					else
						{
						if ( ( node_ == node_->_parent->_left ) && ( sibling->_color == HAbstractNode::BLACK )
								&& ( sibling->_left && sibling->_left->_color == HAbstractNode::RED )
								&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) )
							{
							sibling->_color = HAbstractNode::RED;
							sibling->_left->_color = HAbstractNode::BLACK;
							rotate_right( sibling );
							sibling = get_sibling( node_ );
							}
						else if ( ( node_ == node_->_parent->_right ) && ( sibling->_color == HAbstractNode::BLACK )
								&& ( sibling->_right && sibling->_right->_color == HAbstractNode::RED )
								&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) ) )
							{
							sibling->_color = HAbstractNode::RED;
							sibling->_right->_color = HAbstractNode::BLACK;
							rotate_left( sibling );
							sibling = get_sibling( node_ );
							}
						sibling->_color = node_->_parent->_color;
						node_->_parent->_color = HAbstractNode::BLACK;
						if ( node_ == node_->_parent->_left )
							{
							M_ASSERT( sibling->_right->_color == HAbstractNode::RED );
							sibling->_right->_color = HAbstractNode::BLACK;
							rotate_left( node_->_parent );
							}
						else
							{
							M_ASSERT( sibling->_left->_color == HAbstractNode::RED );
							sibling->_left->_color = HAbstractNode::BLACK;
							rotate_right( node_->_parent );
							}
						}
					}
				break;
				}
			}
		}
	return;
	}

int long HSBBSTree::get_size( void ) const
	{
	return ( _size );
	}

bool HSBBSTree::is_empty( void ) const
	{
	return ( ! _root );
	}

void HSBBSTree::swap( HSBBSTree& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( _size, other._size );
		swap( _root, other._root );
		}
	return;
	}

HSBBSTree::HIterator HSBBSTree::begin( void ) const
	{
	HAbstractNode* node = _root;
	while ( node && node->_left )
		node = node->_left;
	return ( HIterator ( node ) );
	}

HSBBSTree::HIterator HSBBSTree::end( void ) const
	{
	return ( HIterator( NULL ) );
	}

HSBBSTree::HIterator HSBBSTree::rbegin( void ) const
	{
	HAbstractNode* node = _root;
	while ( node && node->_right )
		node = node->_right;
	return ( HIterator ( node ) );
	}

HSBBSTree::HIterator HSBBSTree::rend( void ) const
	{
	return ( HIterator ( NULL ) );
	}

void HSBBSTree::swap( HAbstractNode* first_, HAbstractNode* second_ )
	{
	M_ASSERT( first_ && second_ );
	M_ASSERT( first_ != second_ );
	if ( first_ == _root )
		_root = second_;
	else if ( second_ == _root )
		_root = first_;
	HAbstractNode* firstParent = first_->_parent;
	HAbstractNode* firstLeft = first_->_left;
	HAbstractNode* firstRight = first_->_right;
	HAbstractNode* secondParent = second_->_parent;
	HAbstractNode* secondLeft = second_->_left;
	HAbstractNode* secondRight = second_->_right;
	if ( firstParent == secondParent ) /* siblings */
		{
		M_ASSERT( firstParent );
		if ( first_ == firstParent->_left )
			{
			firstParent->_left = second_;
			firstParent->_right = first_;
			}
		else
			{
			M_ASSERT( first_ == firstParent->_right );
			firstParent->_left = first_;
			firstParent->_right = second_;
			}
		}
	else /* not siblings */
		{
		if ( firstParent )
			{
			if ( first_ == firstParent->_left )
				firstParent->_left = second_;
			else
				{
				M_ASSERT( first_ == firstParent->_right );
				firstParent->_right = second_;
				}
			}
		if ( secondParent )
			{
			if ( second_ == secondParent->_left )
				secondParent->_left = first_;
			else
				{
				M_ASSERT( second_ == secondParent->_right );
				secondParent->_right = first_;
				}
			}
		}
	if ( firstLeft )
		firstLeft->_parent = second_;
	if ( firstRight )
		firstRight->_parent = second_;
	if ( secondLeft )
		secondLeft->_parent = first_;
	if ( secondRight )
		secondRight->_parent = first_;
	if ( secondParent != first_ )
		first_->_parent = secondParent;
	else
		first_->_parent = second_;
	if ( secondLeft != first_ )
		first_->_left = secondLeft;
	else
		first_->_left = second_;
	if ( secondRight != first_ )
		first_->_right = secondRight;
	else
		first_->_right = second_;
	if ( firstParent != second_ )
		second_->_parent = firstParent;
	else
		second_->_parent = first_;
	if ( firstLeft != second_ )
		second_->_left = firstLeft;
	else
		second_->_left = first_;
	if ( firstRight != second_ )
		second_->_right = firstRight;
	else
		second_->_right = first_;
	HAbstractNode::color_t color = first_->_color;
	first_->_color = second_->_color;
	second_->_color = color;
	return;
	}

HSBBSTree::HAbstractNode* HSBBSTree::get_sibling( HAbstractNode* node_ ) const
	{
	if ( node_->_parent->_left == node_ )
		return ( node_->_parent->_right );
	M_ASSERT( node_->_parent->_right == node_ );
	return ( node_->_parent->_left );
	}

}

}


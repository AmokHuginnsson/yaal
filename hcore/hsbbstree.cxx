/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "algorithm.hxx"
#include "hsbbstree.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHSBBSTree_[ 4 ] = {
	_( "ok" ),
/* HSBBSTreeBase::NON_EXISTING_KEY */ _( "key does not exists" ),
/* HSBBSTreeBase::NIL_ITERATOR */     _( "dereferencing nil iterator" ),
/* HSBBSTreeBase::BAD_HINT */         _( "bad hint" )
};

HSBBSTreeBase::HAbstractNode* HSBBSTreeBase::next( HAbstractNode* node_ ) const {
	M_PROLOG
	if ( node_ ) {
		HAbstractNode* lastNode( node_ );
		while ( node_ ) {
			if ( node_->_right && ( node_->_right != lastNode ) ) {
				node_ = node_->_right;
				while ( node_->_left ) {
					node_ = node_->_left;
				}
				break;
			} else {
				lastNode = node_;
				node_ = node_->_parent;
				if ( node_ && ( lastNode == node_->_left ) ) {
					break;
				}
			}
		}
	} else {
		node_ = _root;
		while ( node_ && node_->_left ) {
			node_ = node_->_left;
		}
	}
	return node_;
	M_EPILOG
}

HSBBSTreeBase::HAbstractNode* HSBBSTreeBase::previous( HAbstractNode* node_ ) const {
	M_PROLOG
	if ( node_ ) {
		HAbstractNode* lastNode( node_ );
		while ( node_ ) {
			if ( node_->_left && ( node_->_left != lastNode ) ) {
				node_ = node_->_left;
				while ( node_->_right ) {
					node_ = node_->_right;
				}
				break;
			} else {
				lastNode = node_;
				node_ = node_->_parent;
				if ( node_ && ( lastNode == node_->_right ) ) {
					break;
				}
			}
		}
	} else {
		node_ = _root;
		while ( node_ && node_->_right ) {
			node_ = node_->_right;
		}
	}
	return node_;
	M_EPILOG
}

HSBBSTreeBase::HAbstractNode::HAbstractNode( void )
	: _color( RED )
	, _parent( nullptr )
	, _left( nullptr )
	, _right( nullptr ) {
	return;
}

void HSBBSTreeBase::HAbstractNode::set_child( HAbstractNode* which_, HAbstractNode* new_ ) {
	M_ASSERT( ( which_ == _left ) || ( which_ == _right ) );
	if ( which_ == _left ) {
		_left = new_;
	} else {
		_right = new_;
	}
	return;
}

HSBBSTreeBase::HSBBSTreeBase( void )
	: _root( nullptr )
	, _size( 0 ) {
	return;
}

void HSBBSTreeBase::insert_rebalance( HAbstractNode* node_ ) {
	M_PROLOG
	for ( ; ; ) {
		if ( node_->_parent ) {
			if ( node_->_parent->_color == HAbstractNode::BLACK ) {
				break;
			}
			HAbstractNode* grandpa = node_->_parent->_parent;
			if ( grandpa ) {
				if ( grandpa->_left && grandpa->_right
						&& ( grandpa->_left->_color == HAbstractNode::RED )
						&& ( grandpa->_right->_color == HAbstractNode::RED ) ) {
					grandpa->_left->_color = HAbstractNode::BLACK;
					grandpa->_right->_color = HAbstractNode::BLACK;
					grandpa->_color = HAbstractNode::RED;
					node_ = grandpa;
					continue;
				} else {
					if ( ( node_ == node_->_parent->_right ) && ( grandpa->_left == node_->_parent ) ) {
						rotate_left( node_ = node_->_parent );
					} else if ( ( node_ == node_->_parent->_left ) && ( grandpa->_right == node_->_parent ) ) {
						rotate_right( node_ = node_->_parent );
					}
					node_->_parent->_color = HAbstractNode::BLACK;
					node_->_parent->_parent->_color = HAbstractNode::RED;
					if ( ( node_ == node_->_parent->_left )
							&& ( node_->_parent == node_->_parent->_parent->_left ) ) {
						rotate_right( node_->_parent->_parent );
					} else {
						M_ASSERT( ( node_ == node_->_parent->_right )
								&& ( node_->_parent == node_->_parent->_parent->_right ) );
						rotate_left( node_->_parent->_parent );
					}
				}
			}
		} else {
			node_->_color = HAbstractNode::BLACK;
		}
		break;
	}
	return;
	M_EPILOG
}

void HSBBSTreeBase::rotate_left( HAbstractNode* node_ ) {
	/*
	 * At the beginning of left rotation:
	 * node_ is parent node (center of the rotation)
	 * node_->_right is child node (lets call it satellite)
	 *
	 * At the end of left rotation:
	 * center of the rotation is left child
	 * of its satellite
	 */
	HAbstractNode* parent = node_->_parent;
	HAbstractNode* node = node_->_right;
	if ( parent ) {
		if ( parent->_left == node_ ) {
			parent->_left = node;
		} else {
			M_ASSERT( parent->_right == node_ );
			parent->_right = node;
		}
	} else {
		_root = node;
	}
	node_->_parent = node;
	node_->_right = node->_left;
	if ( node_->_right ) {
		node_->_right->_parent = node_;
	}
	node->_left = node_;
	node->_parent = parent;
	return;
}

void HSBBSTreeBase::rotate_right( HAbstractNode* node_ ) {
	HAbstractNode* parent = node_->_parent;
	HAbstractNode* node = node_->_left;
	if ( parent ) {
		if ( parent->_right == node_ ) {
			parent->_right = node;
		} else {
			M_ASSERT( parent->_left == node_ );
			parent->_left = node;
		}
	} else {
		_root = node;
	}
	node_->_parent = node;
	node_->_left = node->_right;
	if ( node_->_left ) {
		node_->_left->_parent = node_;
	}
	node->_right = node_;
	node->_parent = parent;
	return;
}

void HSBBSTreeBase::remove_node( HAbstractNode* node_ ) {
	if ( node_->_left && node_->_right ) { /* both children exists */
		HAbstractNode* node = node_->_left;
		while ( node->_right ) {
			node = node->_right;
		}
		swap( node_, node );
	}
	if ( node_->_left ) {
		M_ASSERT( ! node_->_right );
		node_->_left->_parent = node_->_parent;
		if ( node_->_parent ) {
			node_->_parent->set_child( node_, node_->_left );
		} else {
			M_ASSERT( _root == node_ );
			_root = node_->_left;
		}
	} else if ( node_->_right ) {
		node_->_right->_parent = node_->_parent;
		if ( node_->_parent ) {
			node_->_parent->set_child( node_, node_->_right );
		} else {
			M_ASSERT( _root == node_ );
			_root = node_->_right;
		}
	}
	remove_rebalance( node_ );
	if ( ( node_->_parent )
			&& ! ( node_->_left || node_->_right ) ) {
		node_->_parent->set_child( node_, nullptr );
	}
	node_->_left = node_->_right = nullptr;
	_size --;
	if ( ! _size ) {
		_root = nullptr;
	}
	M_ASSERT( ! ( _root && _root->_parent ) ); /* very tricky :^) */
	return;
}

void HSBBSTreeBase::remove_rebalance( HAbstractNode* node_ ) {
	M_ASSERT( node_ );
	HAbstractNode* child = node_->_left ? node_->_left : node_->_right;
	M_ASSERT( ! child || ( node_->_parent == child->_parent ) );
	if ( node_->_color == HAbstractNode::BLACK ) {
		if ( child && child->_color == HAbstractNode::RED ) {
			child->_color = HAbstractNode::BLACK;
		} else {
			if ( child ) {
				node_ = child;
			}
			/* after this line node_ is a pivot of rebalancing
			 * no matter if it's child of removed node, or the node it self */
			for ( int i = 0; ; ++i ) { /* tail recursion may be easily changed to iteration */
				if ( node_->_parent == nullptr ) {
					_root->_color = HAbstractNode::BLACK;
				} else { /* hard part starts here */
					HAbstractNode* sibling = get_sibling( node_ );
					if ( sibling->_color == HAbstractNode::RED ) {
						/* sibling must exists because all black nodes have siblings,
						 * moreover if the sibling is red, we know for sure that parent
						 * is black, due to rule that prevents two subsequent red nodes */
						sibling->_color = HAbstractNode::BLACK;
						node_->_parent->_color = HAbstractNode::RED;
						if ( node_ == node_->_parent->_left ) {
							rotate_left( node_->_parent );
						} else {
							rotate_right( node_->_parent );
						}
						sibling = get_sibling( node_ );
					}
					if ( ( node_->_parent->_color == HAbstractNode::BLACK )
							&& ( sibling->_color == HAbstractNode::BLACK )
							&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) )
							&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) ) {
						sibling->_color = HAbstractNode::RED;
						node_ = node_->_parent;
						continue;
					} else if ( ( node_->_parent->_color == HAbstractNode::RED )
							&& ( sibling->_color == HAbstractNode::BLACK )
							&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) )
							&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) ) {
						node_->_parent->_color = HAbstractNode::BLACK;
						sibling->_color = HAbstractNode::RED;
					} else {
						if ( ( node_ == node_->_parent->_left ) && ( sibling->_color == HAbstractNode::BLACK )
								&& ( sibling->_left && sibling->_left->_color == HAbstractNode::RED )
								&& ( ! sibling->_right || ( sibling->_right->_color == HAbstractNode::BLACK ) ) ) {
							sibling->_color = HAbstractNode::RED;
							sibling->_left->_color = HAbstractNode::BLACK;
							rotate_right( sibling );
							sibling = get_sibling( node_ );
						} else if ( ( node_ == node_->_parent->_right ) && ( sibling->_color == HAbstractNode::BLACK )
								&& ( sibling->_right && sibling->_right->_color == HAbstractNode::RED )
								&& ( ! sibling->_left || ( sibling->_left->_color == HAbstractNode::BLACK ) ) ) {
							sibling->_color = HAbstractNode::RED;
							sibling->_right->_color = HAbstractNode::BLACK;
							rotate_left( sibling );
							sibling = get_sibling( node_ );
						}
						sibling->_color = node_->_parent->_color;
						node_->_parent->_color = HAbstractNode::BLACK;
						if ( node_ == node_->_parent->_left ) {
							M_ASSERT( sibling->_right->_color == HAbstractNode::RED );
							sibling->_right->_color = HAbstractNode::BLACK;
							rotate_left( node_->_parent );
						} else {
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

int long HSBBSTreeBase::get_size( void ) const {
	return ( _size );
}

bool HSBBSTreeBase::is_empty( void ) const {
	return ( ! _root );
}

HSBBSTreeBase::HAbstractNode* HSBBSTreeBase::leftmost( void ) const {
	HAbstractNode* node( _root );
	while ( node && node->_left ) {
		node = node->_left;
	}
	return node;
}

HSBBSTreeBase::HAbstractNode* HSBBSTreeBase::rightmost( void ) const {
	HAbstractNode* node( _root );
	while ( node && node->_right ) {
		node = node->_right;
	}
	return node;
}

void HSBBSTreeBase::swap( HAbstractNode* first_, HAbstractNode* second_ ) {
	M_ASSERT( first_ && second_ );
	M_ASSERT( first_ != second_ );
	if ( first_ == _root ) {
		_root = second_;
	} else if ( second_ == _root ) {
		_root = first_;
	}
	HAbstractNode* firstParent = first_->_parent;
	HAbstractNode* firstLeft = first_->_left;
	HAbstractNode* firstRight = first_->_right;
	HAbstractNode* secondParent = second_->_parent;
	HAbstractNode* secondLeft = second_->_left;
	HAbstractNode* secondRight = second_->_right;
	if ( firstParent == secondParent ) { /* siblings */
		M_ASSERT( firstParent );
		if ( first_ == firstParent->_left ) {
			firstParent->_left = second_;
			firstParent->_right = first_;
		} else {
			M_ASSERT( first_ == firstParent->_right );
			firstParent->_left = first_;
			firstParent->_right = second_;
		}
	} else { /* not siblings */
		if ( firstParent ) {
			if ( first_ == firstParent->_left ) {
				firstParent->_left = second_;
			} else {
				M_ASSERT( first_ == firstParent->_right );
				firstParent->_right = second_;
			}
		}
		if ( secondParent ) {
			if ( second_ == secondParent->_left ) {
				secondParent->_left = first_;
			} else {
				M_ASSERT( second_ == secondParent->_right );
				secondParent->_right = first_;
			}
		}
	}
	if ( firstLeft ) {
		firstLeft->_parent = second_;
	}
	if ( firstRight ) {
		firstRight->_parent = second_;
	}
	if ( secondLeft ) {
		secondLeft->_parent = first_;
	}
	if ( secondRight ) {
		secondRight->_parent = first_;
	}
	if ( secondParent != first_ ) {
		first_->_parent = secondParent;
	} else {
		first_->_parent = second_;
	}
	if ( secondLeft != first_ ) {
		first_->_left = secondLeft;
	} else {
		first_->_left = second_;
	}
	if ( secondRight != first_ ) {
		first_->_right = secondRight;
	} else {
		first_->_right = second_;
	}
	if ( firstParent != second_ ) {
		second_->_parent = firstParent;
	} else {
		second_->_parent = first_;
	}
	if ( firstLeft != second_ ) {
		second_->_left = firstLeft;
	} else {
		second_->_left = first_;
	}
	if ( firstRight != second_ ) {
		second_->_right = firstRight;
	} else {
		second_->_right = first_;
	}
	HAbstractNode::color_t color = first_->_color;
	first_->_color = second_->_color;
	second_->_color = color;
	return;
}

HSBBSTreeBase::HAbstractNode* HSBBSTreeBase::get_sibling( HAbstractNode* node_ ) const {
	if ( node_->_parent->_left == node_ ) {
		return ( node_->_parent->_right );
	}
	M_ASSERT( node_->_parent->_right == node_ );
	return ( node_->_parent->_left );
}

}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsbbstree.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hsbbstree.hxx
 * \brief Declaration of HSBBSTree related classes.
 */

#ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED
#define YAAL_HCORE_HSBBSTREE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/memory.hxx"
#include "hcore/hpair.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHSBBSTree_[];

/*! \brief HSBBSTree stands for: Self-balancing binary search tree.
 *
 * HSBBSTree is implementation of self balancing binary search tree data structure.
 * Implementation of core engine for HMap<> and HSet<> classes.
 * HSBBSTree is not ment to be used directly.
 */
class HSBBSTreeBase {
	typedef HSBBSTreeBase this_type;
public:
	/*! \brief HSBBSTreeBase operation error types.
	 */
	struct ERROR {
		typedef enum {
			OK = 0,
			NON_EXISTING_KEY,
			NIL_ITERATOR
		} error_t;
	};
private:
	class HAbstractNode;
	struct ONodePtr {
		HAbstractNode* _node;
		bool _exists;
		ONodePtr( void )
			: _node( NULL ), _exists( false )
			{ }
		ONodePtr( ONodePtr const& np )
			: _node( np._node ), _exists( np._exists )
			{}
		ONodePtr& operator = ( ONodePtr const& np ) {
			if ( &np != this ) {
				_node = np._node;
				_exists = np._exists;
			}
			return ( *this );
		}
	};
	class HAbstractNode {
	protected:
		typedef enum {
			RED,
			BLACK
		} color_t;
		color_t _color;
		HAbstractNode* _parent;
		HAbstractNode* _left;
		HAbstractNode* _right;
		HAbstractNode( void );
		HAbstractNode( HAbstractNode const& );
		HAbstractNode& operator = ( HAbstractNode const & );
		void set_child( HAbstractNode*, HAbstractNode* );
		friend class HSBBSTreeBase;
		virtual ~HAbstractNode( void ) {}
	};
protected:
	HAbstractNode* _root;
	int long _size;
public:
	HSBBSTreeBase( void );
	virtual ~HSBBSTreeBase( void ) {}
	int long get_size( void ) const;
	bool is_empty( void ) const;
	HAbstractNode* leftmost( void ) const;
	HAbstractNode* next( HAbstractNode* ) const;
	HAbstractNode* previous( HAbstractNode* ) const;
protected:
	void remove_node( HAbstractNode* );
	void swap( HAbstractNode*, HAbstractNode* );
	HAbstractNode* get_sibling( HAbstractNode* ) const;
	void insert_rebalance( HAbstractNode* );
	void remove_rebalance( HAbstractNode* );
	void rotate_left( HAbstractNode* );
	void rotate_right( HAbstractNode* );
	HSBBSTreeBase( HSBBSTreeBase const& );
	HSBBSTreeBase& operator = ( HSBBSTreeBase const& );
};

template<typename key_value_t, typename compare_t, typename key_get_t>
class HSBBSTree : public HSBBSTreeBase {
public:
	typedef key_value_t key_value_type;
	typedef compare_t compare_type;
	typedef key_get_t key_get_type;
	typedef typename key_get_type::key_type key_type;
	class HIterator;
private:
	/*! \brief True HSBBSTree contents.
	 */
	class	HNode : public HSBBSTreeBase::HAbstractNode {
		using HAbstractNode::color_t;
		using HAbstractNode::_color;
		using HAbstractNode::_parent;
		using HAbstractNode::_left;
		using HAbstractNode::_right;
		key_value_type _key;
		HNode( key_value_type const& key_ )
			: HAbstractNode(), _key( key_ ) {
			return;
		}
		~HNode( void ) {
			if ( _left )
				M_SAFE( delete static_cast<HNode*>( _left ) );
			_left = NULL;
			if ( _right )
				M_SAFE( delete static_cast<HNode*>( _right ) );
			_right = NULL;
			return;
		}
		HNode( HNode const& );
		HNode& operator = ( HNode const& );
		friend class HSBBSTree;
		friend class HSBBSTree::HIterator;
	};
	using HSBBSTreeBase::_root;
	using HSBBSTreeBase::_size;
	compare_type _compare;
public:
	HSBBSTree( compare_type const& compare_ )
		: HSBBSTreeBase(), _compare( compare_ )
		{}
	~HSBBSTree( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HPair<HIterator, bool> insert( key_value_type const& );
	void remove( HIterator const& it_ ) {
		M_PROLOG
		if ( ! it_._current )
			M_THROW( _errMsgHSBBSTree_[ ERROR::NIL_ITERATOR ],
					static_cast<int>( ERROR::NIL_ITERATOR ) );
		remove_node( it_._current );
		M_SAFE( delete static_cast<HNode*>( it_._current ) );
		return;
		M_EPILOG
	}
	HIterator find( key_type const& key_ ) const {
		M_PROLOG
		ONodePtr nodePtr = find_node( key_ );
		return ( HIterator( this, nodePtr._exists ? nodePtr._node : NULL ) );
		M_EPILOG
	}
	HIterator lower_bound( key_type const& ) const;
	HIterator upper_bound( key_type const& ) const;
	HIterator begin( void ) const {
		return ( HIterator( this, leftmost() ) );
	}
	HIterator end( void ) const {
		return ( HIterator( this, NULL ) );
	}
	void clear( void ) {
		M_PROLOG
		if ( _root )
			M_SAFE( delete static_cast<HNode*>( _root ) );
		_root = NULL;
		_size = 0;
		M_EPILOG
	}
	void swap( HSBBSTree& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _size, other._size );
			swap( _root, other._root );
		}
		return;
	}
	void copy_from( HSBBSTree const& source ) {
		M_PROLOG
		if ( &source != this ) {
			clear();
			_compare = source._compare;
			if ( source._root )
				_root = copy_node( static_cast<HNode*>( source._root ) );
			_size = source._size;
		}
		return;
		M_EPILOG
	}
	compare_type const& compare( void ) const {
		return ( _compare );
	}
private:
	ONodePtr find_node( key_type const& ) const;
	HNode* copy_node( HNode const* source ) {
		M_PROLOG
		HNode* node( new HNode( source->_key ) );
		node->_color = source->_color;
		if ( source->_left ) {
			node->_left = copy_node( static_cast<HNode*>( source->_left ) );
			static_cast<HNode*>( node->_left )->_parent = node;
		}
		if ( source->_right ) {
			node->_right = copy_node( static_cast<HNode*>( source->_right ) );
			static_cast<HNode*>( node->_right )->_parent = node;
		}
		return ( node );
		M_EPILOG
	}
};

/*! \brief Iterator for HSBBSTree data structure.
 */
template<typename key_value_t, typename compare_t, typename key_get_t>
class HSBBSTree<key_value_t, compare_t, key_get_t>::HIterator {
	typedef HSBBSTree<key_value_t, compare_t, key_get_t> owner_t;
	owner_t const* _owner;
	HAbstractNode* _current;
public:
	HIterator( void )
		: _owner( NULL ), _current( NULL ) {
		return;
	}
	HIterator( HIterator const & iterator_ )
		: _owner( iterator_._owner ), _current ( iterator_._current ) {
		return;
	}
	HIterator& operator = ( HIterator const& iterator_ ) {
		if ( &iterator_ != this ) {
			_owner = iterator_._owner;
			_current = iterator_._current;
		}
		return ( *this );
	}
	HIterator& operator ++ ( void ) {
		M_ASSERT( _owner );
		_current = _owner->next( _current );
		return ( *this );
	}
	HIterator& operator -- ( void ) {
		M_ASSERT( _owner );
		_current = _owner->previous( _current );
		return ( *this );
	}
	bool operator == ( HIterator const& iterator_ ) const {
		M_ASSERT( _owner == iterator_._owner );
		return ( _current == iterator_._current );
	}
	bool operator != ( HIterator const& iterator_ ) const {
		M_ASSERT( _owner == iterator_._owner );
		return ( _current != iterator_._current );
	}
	key_value_t& get( void ) const {
		M_PROLOG
		M_ASSERT( _current );
		return ( static_cast<typename owner_t::HNode*>( _current )->_key );
		M_EPILOG
	}
private:
	friend class HSBBSTree<key_value_t, compare_t, key_get_t>;
	explicit HIterator( owner_t const* owner_, HSBBSTreeBase::HAbstractNode* const node_ )
		: _owner( owner_ ), _current( node_ ) {
		return;
	}
};

template<typename key_value_t, typename compare_t, typename key_get_t>
HPair<typename HSBBSTree<key_value_t, compare_t, key_get_t>::HIterator, bool> HSBBSTree<key_value_t, compare_t, key_get_t>::insert( key_value_type const& key_ ) {
	M_PROLOG
	ONodePtr nodeHolder;
	HNode* node( NULL );
	key_type const& key( key_get_type::key( key_ ) );
	if ( _root )
		nodeHolder = find_node( key );
	if ( nodeHolder._exists )
		node = static_cast<HNode*>( nodeHolder._node );
	else {
		node = new ( memory::yaal ) HNode( key_ );
		++ _size;
		if ( _root ) {
			node->_parent = nodeHolder._node;
			if ( _compare( key, key_get_type::key( static_cast<HNode*>( nodeHolder._node )->_key ) ) ) {
				M_ASSERT( ! static_cast<HNode*>( nodeHolder._node )->_left );
				static_cast<HNode*>( nodeHolder._node )->_left = node;
			} else {
				M_ASSERT( ! static_cast<HNode*>( nodeHolder._node )->_right );
				static_cast<HNode*>( nodeHolder._node )->_right = node;
			}
			insert_rebalance( node );
		} else {
			_root = node;
			static_cast<HNode*>( _root )->_color = HAbstractNode::BLACK;
		}
	}
	M_ASSERT( ( ! _root ) || ( static_cast<HNode*>( _root )->_parent == NULL ) );
	M_ASSERT( ( ! _root ) || ( static_cast<HNode*>( _root )->_color == HAbstractNode::BLACK ) );
	return ( make_pair( HIterator( this, node ), ! nodeHolder._exists ) );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t>
typename HSBBSTree<key_value_t, compare_t, key_get_t>::HIterator HSBBSTree<key_value_t, compare_t, key_get_t>::lower_bound( key_type const& key_ ) const {
	M_PROLOG
	HIterator it( this, NULL );
	if ( _root ) {
		ONodePtr nodeHolder( find_node( key_ ) );
		it._current = nodeHolder._node;
		if ( nodeHolder._node && ! nodeHolder._exists && _compare( key_get_type::key( static_cast<HNode*>( nodeHolder._node )->_key ), key_ ) )
			++ it;
	}
	return ( it );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t>
typename HSBBSTree<key_value_t, compare_t, key_get_t>::HIterator HSBBSTree<key_value_t, compare_t, key_get_t>::upper_bound( key_type const& key_ ) const {
	M_PROLOG
	HIterator it( this, NULL );
	if ( _root ) {
		ONodePtr nodeHolder( find_node( key_ ) );
		it._current = nodeHolder._node;
		if ( nodeHolder._node && ( nodeHolder._exists || _compare( key_get_type::key( static_cast<HNode*>( nodeHolder._node )->_key ), key_ ) ) )
			++ it;
	}
	return ( it );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t>
typename HSBBSTree<key_value_t, compare_t, key_get_t>::ONodePtr HSBBSTree<key_value_t, compare_t, key_get_t>::find_node( key_type const& key_ ) const {
	M_PROLOG
	ONodePtr nodePtr;
	if ( _root ) {
		nodePtr._node = _root;
		while ( ! nodePtr._exists ) {
			if ( _compare( key_, key_get_type::key( static_cast<HNode*>( nodePtr._node )->_key ) ) ) {
				if ( static_cast<HNode*>( nodePtr._node )->_left )
					nodePtr._node = static_cast<HNode*>( nodePtr._node )->_left;
				else
					break;
			} else if ( _compare( key_get_type::key( static_cast<HNode*>( nodePtr._node )->_key ), key_ ) ) {
				if ( static_cast<HNode*>( nodePtr._node )->_right )
					nodePtr._node = static_cast<HNode*>( nodePtr._node )->_right;
				else
					break;
			} else
				nodePtr._exists = true;
		}
	}
	return ( nodePtr );
	M_EPILOG
}

}

template<typename key_value_t, typename compare_t, typename key_get_t>
inline void swap( yaal::hcore::HSBBSTree<key_value_t, compare_t, key_get_t>& a, yaal::hcore::HSBBSTree<key_value_t, compare_t, key_get_t>& b )
	{ a.swap( b );	}

}

#endif /* #ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED */


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

extern M_YAAL_HCORE_PUBLIC_API char const * const _errMsgHSBBSTree_ [ ];

/*! \brief HSBBSTree stands for: Self-balancing binary search tree.
 *
 * HSBBSTree is implementation of self balancing binary search tree data structure.
 * Implementation of core engine for HMap<> and HSet<> classes.
 * HSBBSTree is not ment to be used directly.
 */
class HSBBSTree {
	typedef HSBBSTree this_type;
public:
	/*! \brief HSBBSTree operation error types.
	 */
	struct ERROR {
		typedef enum {
			OK = 0,
			NON_EXISTING_KEY,
			NIL_ITERATOR
		} error_t;
	};
	class HIterator;
private:
	class HAbstractNode;
	struct ONodePtr {
		HAbstractNode* _node;
		bool _exists;
		ONodePtr( void ) : _node( NULL ), _exists( false ) { }
		ONodePtr( ONodePtr const& np ) : _node( np._node ), _exists( np._exists ) {}
		ONodePtr& operator = ( ONodePtr const& np ) {
			if ( &np != this ) {
				_node = np._node;
				_exists = np._exists;
			}
			return ( *this );
		}
	};
	template<typename tType>
	class HNode;
	class HAbstractNode {
		typedef enum {
			RED,
			BLACK
		} color_t;
		color_t _color;
		HAbstractNode* _parent;
		HAbstractNode* _left;
		HAbstractNode* _right;
		HAbstractNode( void );
		virtual ~HAbstractNode( void );
		HAbstractNode( HAbstractNode const& );
		HAbstractNode& operator = ( HAbstractNode const & );
		virtual HAbstractNode* clone( void ) const = 0;
		void set_child( HAbstractNode*, HAbstractNode* );
		friend class HSBBSTree;
		friend class HSBBSTree::HIterator;
		template<typename any_t>
		friend class HNode;
	};
	HAbstractNode* _root;
	int long _size;
public:
	HSBBSTree( void );
	virtual ~HSBBSTree( void );
	template<typename tType, typename ttType>
	HPair<HIterator, bool> insert( tType const& );
	void remove( HIterator const& );
	template<typename tType, typename ttType, typename tttType>
	HIterator find( ttType const& ) const;
	template<typename tType, typename ttType, typename tttType>
	HIterator lower_bound( ttType const& ) const;
	template<typename tType, typename ttType, typename tttType>
	HIterator upper_bound( ttType const& ) const;
	int long get_size( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	void swap( HSBBSTree& );
	void copy_from( HSBBSTree const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
private:
	template<typename tType, typename ttType, typename tttType>
	ONodePtr find_node( ttType const& ) const;
	HAbstractNode* copy_node( HAbstractNode const* );
	void remove_node( HAbstractNode* );
	void swap( HAbstractNode*, HAbstractNode* );
	HAbstractNode* get_sibling( HAbstractNode* ) const;
	void insert_rebalance( HAbstractNode* );
	void remove_rebalance( HAbstractNode* );
	void rotate_left( HAbstractNode* );
	void rotate_right( HAbstractNode* );
	HSBBSTree( HSBBSTree const& );
	HSBBSTree& operator = ( HSBBSTree const& );
};

/*! \brief Iterator for HSBBSTree data structure.
 */
class HSBBSTree::HIterator {
	HSBBSTree const* _owner;
	HAbstractNode* _current;
public:
	HIterator( void );
	HIterator( HIterator const& );
	HIterator& operator ++ ( void );
	HIterator& operator -- ( void );
	HIterator& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	template<typename tType>
	tType& get( void );
	template<typename tType>
	tType const& get( void ) const;
private:
	friend class HSBBSTree;
	explicit HIterator( HSBBSTree const*, HAbstractNode* const );
};

/*! \brief True HSBBSTree contents.
 */
template<typename tType>
class	HSBBSTree::HNode : public HSBBSTree::HAbstractNode {
	tType _key;
	HNode( tType const& );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	virtual HAbstractNode* clone( void ) const;
	friend class HSBBSTree;
	friend tType& HSBBSTree::HIterator::get<tType>( void );
	friend tType const& HSBBSTree::HIterator::get<tType>( void ) const;
};

template<typename tType>
tType& HSBBSTree::HIterator::get( void ) {
	M_PROLOG
	M_ASSERT( _current );
	return ( static_cast<HNode<tType>*>( _current )->_key );
	M_EPILOG
}

template<typename tType>
tType const& HSBBSTree::HIterator::get( void ) const {
	M_PROLOG
	M_ASSERT( _current );
	return ( static_cast<HNode<tType>*>( _current )->_key );
	M_EPILOG
}

template<typename tType>
HSBBSTree::HNode<tType>::HNode( tType const& key_ )
	: HAbstractNode(), _key( key_ ) {
	return;
}

template<typename tType>
HSBBSTree::HAbstractNode* HSBBSTree::HNode<tType>::clone( void ) const {
	M_PROLOG
	return ( new ( memory::yaal ) HNode( _key ) );
	M_EPILOG
}

template<typename tType, typename ttType>
HPair<HSBBSTree::HIterator, bool> HSBBSTree::insert( tType const& key_ ) {
	M_PROLOG
	ONodePtr nodeHolder;
	HNode<tType>* node( NULL );
	if ( _root )
		nodeHolder = find_node<tType, tType, ttType>( key_ );
	if ( nodeHolder._exists )
		node = static_cast<HNode<tType>*>( nodeHolder._node );
	else {
		node = new ( memory::yaal ) HNode<tType>( key_ );
		++ _size;
		if ( _root ) {
			node->_parent = nodeHolder._node;
			if ( ttType::less( key_, static_cast<HNode<tType>*>( nodeHolder._node )->_key ) ) {
				M_ASSERT( ! nodeHolder._node->_left );
				nodeHolder._node->_left = node;
			} else {
				M_ASSERT( ! nodeHolder._node->_right );
				nodeHolder._node->_right = node;
			}
			insert_rebalance( node );
		} else {
			_root = node;
			_root->_color = HAbstractNode::BLACK;
		}
	}
	M_ASSERT( ( ! _root ) || ( _root->_parent == NULL ) );
	M_ASSERT( ( ! _root ) || ( _root->_color == HAbstractNode::BLACK ) );
	return ( make_pair( HIterator( this, node ), ! nodeHolder._exists ) );
	M_EPILOG
}

template<typename tType, typename ttType, typename tttType>
HSBBSTree::HIterator HSBBSTree::find( ttType const& key_ ) const {
	M_PROLOG
	ONodePtr nodePtr = find_node<tType, ttType, tttType>( key_ );
	return ( HIterator( this, nodePtr._exists ? nodePtr._node : NULL ) );
	M_EPILOG
}

template<typename tType, typename ttType, typename tttType>
HSBBSTree::HIterator HSBBSTree::lower_bound( ttType const& key_ ) const {
	M_PROLOG
	HIterator it( this, NULL );
	if ( _root ) {
		ONodePtr nodeHolder( find_node<tType, ttType, tttType>( key_ ) );
		it._current = nodeHolder._node;
		if ( nodeHolder._node && ! nodeHolder._exists && tttType::less( static_cast<HNode<tType>*>( nodeHolder._node )->_key, key_ ) )
			++ it;
	}
	return ( it );
	M_EPILOG
}

template<typename tType, typename ttType, typename tttType>
HSBBSTree::HIterator HSBBSTree::upper_bound( ttType const& key_ ) const {
	M_PROLOG
	HIterator it( this, NULL );
	if ( _root ) {
		ONodePtr nodeHolder( find_node<tType, ttType, tttType>( key_ ) );
		it._current = nodeHolder._node;
		if ( nodeHolder._node && ( nodeHolder._exists || tttType::less( static_cast<HNode<tType>*>( nodeHolder._node )->_key, key_ ) ) )
			++ it;
	}
	return ( it );
	M_EPILOG
}

template<typename tType, typename ttType, typename tttType>
typename HSBBSTree::ONodePtr HSBBSTree::find_node( ttType const& key_ ) const {
	M_PROLOG
	ONodePtr nodePtr;
	if ( _root ) {
		nodePtr._node = _root;
		while ( ! nodePtr._exists ) {
			if ( tttType::less( key_, static_cast<HNode<tType>*>( nodePtr._node )->_key ) ) {
				if ( nodePtr._node->_left )
					nodePtr._node = nodePtr._node->_left;
				else
					break;
			} else if ( tttType::less( static_cast<HNode<tType>*>( nodePtr._node )->_key, key_ ) ) {
				if ( nodePtr._node->_right )
					nodePtr._node = nodePtr._node->_right;
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

inline void swap( yaal::hcore::HSBBSTree& a, yaal::hcore::HSBBSTree& b )
	{ a.swap( b );	}

}

#endif /* #ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED */


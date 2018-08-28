/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hsbbstree.hxx
 * \brief Declaration of HSBBSTree related classes.
 */

#ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED
#define YAAL_HCORE_HSBBSTREE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/memory.hxx"
#include "hcore/hpair.hxx"
#include "hcore/allocator.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHSBBSTree_[];

/*! \brief HSBBSTree stands for: Self-balancing binary search tree.
 *
 * HSBBSTree is implementation of self balancing binary search tree data structure.
 * Implementation of core engine for HMap<> and HSet<> classes.
 * HSBBSTree is not meant to be used directly.
 */
class HSBBSTreeBase {
public:
	typedef HSBBSTreeBase this_type;
	typedef int long size_type;
	/*! \brief HSBBSTreeBase operation error types.
	 */
	struct ERROR {
		typedef enum {
			OK = 0,
			NON_EXISTING_KEY,
			NIL_ITERATOR,
			BAD_HINT
		} error_t;
	};
protected:
	class HAbstractNode {
	public:
		typedef enum {
			RED,
			BLACK
		} color_t;
	protected:
		color_t _color;
		HAbstractNode* _parent;
		HAbstractNode* _left;
		HAbstractNode* _right;
		HAbstractNode( void );
		HAbstractNode( HAbstractNode const& );
		HAbstractNode& operator = ( HAbstractNode const & );
		void set_child( HAbstractNode*, HAbstractNode* );
		friend class HSBBSTreeBase;
	protected:
		~HAbstractNode( void ) {}
	};
protected:
	HAbstractNode* _root;
	size_type _size;
public:
	HSBBSTreeBase( void );
	virtual ~HSBBSTreeBase( void ) {}
	void swap( HSBBSTreeBase& other_ ) {
		using yaal::swap;
		swap( _size, other_._size );
		swap( _root, other_._root );
	}
	size_type get_size( void ) const;
	bool is_empty( void ) const;
	HAbstractNode* leftmost( void ) const;
	HAbstractNode* rightmost( void ) const;
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

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
class HSBBSTree : public HSBBSTreeBase {
public:
	typedef HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t> this_type;
	typedef HSBBSTreeBase base_type;
	typedef key_value_t key_value_type;
	typedef compare_t compare_type;
	typedef key_get_t key_get_type;
	typedef typename key_get_type::key_type key_type;
	class HIterator;
	typedef HIterator iterator_type;
private:
#ifndef __sun__
#pragma pack( push, 1 )
#else /* #ifndef __sun__ */
#pragma pack( 1 )
#endif /* #else #ifndef __sun__ */
	/*! \brief True HSBBSTree contents.
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
	class HNode : public HSBBSTreeBase::HAbstractNode {
#pragma GCC diagnostic pop
		key_value_type _key;
		HNode( key_value_type const& key_ )
			: HAbstractNode()
			, _key( key_ ) {
		}
		HNode( key_value_type&& key_ )
			: HAbstractNode()
			, _key( yaal::move( key_ ) ) {
		}
		HNode( HNode const& );
		HNode& operator = ( HNode const& );
		friend class HSBBSTree;
		friend class HSBBSTree::HIterator;
	};
#ifndef __sun__
#pragma pack( pop )
#else /* #ifndef __sun__ */
#pragma pack()
#endif /* #else #ifndef __sun__ */
	typedef yaal::hcore::HPair<HNode**, HNode*> hint_t;
	struct constructor_copy {
		key_value_type const& _key;
		constructor_copy( key_value_type const& key_ )
			: _key( key_ ) {
		}
		void construct( HNode* node ) {
			new ( node ) HNode( _key );
		}
	};
	struct constructor_move {
		key_value_type& _key;
		constructor_move( key_value_type& key_ )
			: _key( key_ ) {
		}
		void construct( HNode* node ) {
			new ( node ) HNode( yaal::move( _key ) );
		}
	};
public:
	typedef typename allocator_t::template rebind<HNode>::other allocator_type;
	typedef trait::delayed_sizeof<HNode> node_size;
	typedef HNode node_type;
private:
	compare_type _compare;
	allocator_type _allocator;
public:
	HSBBSTree( compare_type const& compare_, allocator_type const& allocator_ )
		: HSBBSTreeBase()
		, _compare( compare_ )
		, _allocator( allocator_ ) {
	}
	~HSBBSTree( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HPair<HIterator, bool> insert( key_value_type const& key_ ) {
		return ( insert_impl( constructor_copy( key_ ), find_node( key_get_type::key( key_ ), &_root ) ) );
	}
	HPair<HIterator, bool> insert( key_value_type&& key_ ) {
		return ( insert_impl( constructor_move( key_ ), find_node( key_get_type::key( key_ ), &_root ) ) );
	}
	void insert( HIterator const& pos_, key_value_type const& key_, bool forceHint_ ) {
		insert_impl( constructor_copy( key_ ), verify_hint( key_get_type::key( key_ ), pos_, forceHint_ ) );
	}
	void insert( HIterator const& pos_, key_value_type&& key_, bool forceHint_ ) {
		insert_impl( constructor_move( key_ ), verify_hint( key_get_type::key( key_ ), pos_, forceHint_ ) );
	}
	HIterator remove( HIterator const& it_ ) {
		M_PROLOG
		if ( ! it_._current ) {
			M_THROW( _errMsgHSBBSTree_[ ERROR::NIL_ITERATOR ],
					static_cast<int>( ERROR::NIL_ITERATOR ) );
		}
		HIterator it( it_ );
		++ it;
		remove_node( it_._current );
		HNode* node( static_cast<HNode*>( it_._current ) );
		M_SAFE( node->~HNode() );
		_allocator.deallocate( node, 1 );
		return ( it );
		M_EPILOG
	}
	HIterator find( key_type const& key_ ) const {
		M_PROLOG
		return ( HIterator( this, *(find_node( key_, &_root ).first) ) );
		M_EPILOG
	}
	HIterator lower_bound( key_type const& ) const;
	HIterator upper_bound( key_type const& ) const;
	HIterator begin( void ) const {
		return ( HIterator( this, leftmost() ) );
	}
	HIterator end( void ) const {
		return ( HIterator( this, nullptr ) );
	}
	void clear( void ) {
		M_PROLOG
		if ( _root ) {
			delete_node( static_cast<HNode*>( _root ) );
		}
		_root = nullptr;
		_size = 0;
		M_EPILOG
	}
	void swap( HSBBSTree& other_ ) {
		if ( &other_ != this ) {
			using yaal::swap;
			HSBBSTreeBase::swap( other_ );
			swap( _compare, other_._compare );
			swap( _allocator, other_._allocator );
		}
		return;
	}
	void copy_from( HSBBSTree const& source ) {
		M_PROLOG
		if ( &source != this ) {
			clear();
			_compare = source._compare;
			if ( source._root ) {
				_root = copy_node( static_cast<HNode*>( source._root ) );
			}
			_size = source._size;
		}
		return;
		M_EPILOG
	}
	compare_type const& compare( void ) const {
		return ( _compare );
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
private:
	hint_t find_node( key_type const& key_, HAbstractNode const* const* node_ ) const {
		M_PROLOG
		HNode* parent( nullptr );
		HNode** node( reinterpret_cast<HNode**>( const_cast<HAbstractNode**>( node_ ) ) );
		while ( *node ) {
			if ( _compare( key_, key_get_type::key( (*node)->_key ) ) ) {
				parent = *node;
				node = reinterpret_cast<HNode**>( &((*node)->_left) );
			} else if ( _compare( key_get_type::key( (*node)->_key ), key_ ) ) {
				parent = *node;
				node = reinterpret_cast<HNode**>( &((*node)->_right) );
			} else {
				break;
			}
		}
		return ( make_pair( node, parent ) );
		M_EPILOG
	}
	template<typename constructor_t>
	HPair<HIterator, bool> insert_impl( constructor_t&&, hint_t );
	hint_t verify_hint( key_type const& key_, HIterator const& pos_, bool forceHint_ ) {
		M_ASSERT( pos_._owner == this );
		/*
		 * Test from most frequent use case.
		 * (1) _e.insert( _e.end(), val );
		 * (2) _e.insert( _e.begin(), val );
		 * (3) _e.insert( it, val ); // where `it' is in the middle of the collection.
		 */
		hint_t hint;
		bool goodHint( false );
		if ( _size > 0 ) {
			if ( pos_ == end() ) { /* pos_ == _col.end() */
				HNode* last( static_cast<HNode*>( rightmost() ) );
				if ( _compare( key_get_type::key( last->_key ), key_ ) ) {
					hint.first = reinterpret_cast<HNode**>( &last->_right );
					hint.second = last;
					goodHint = true;
				} else if ( ! _compare( key_, key_get_type::key( last->_key ) ) ) {
					HNode* parent( static_cast<HNode*>( last->_parent ) );
					hint.first = reinterpret_cast<HNode**>( parent ? &parent->_right : &_root );
					hint.second = parent;
					goodHint = true;
				}
			} else if ( pos_ == begin() ) {
				HNode* first( const_cast<HNode*>( static_cast<HNode const*>( pos_._current ) ) );
				if ( _compare( key_, key_get_type::key( first->_key ) ) ) {
					hint.first = reinterpret_cast<HNode**>( &first->_left );
					hint.second = first;
					goodHint = true;
				} else if ( ! _compare( key_get_type::key( first->_key ), key_ ) ) {
					HNode* parent( static_cast<HNode*>( first->_parent ) );
					hint.first = reinterpret_cast<HNode**>( parent ? &parent->_left : &_root );
					hint.second = parent;
					goodHint = true;
				}
			} else {
				HNode* proposedHint( const_cast<HNode*>( static_cast<HNode const*>( pos_._current ) ) );
				HIterator prevIt( pos_ );
				-- prevIt;
				HNode* prevToHint( static_cast<HNode*>( prevIt._current ) );
				if ( _compare( key_, key_get_type::key( proposedHint->_key ) ) ) {
					if ( _compare( key_get_type::key( prevToHint->_key ), key_ ) ) {
						HNode* parent( static_cast<HNode*>( proposedHint->_parent ) );
						hint = find_node( key_, parent ? ( parent->_left == proposedHint ? &parent->_left : &parent->_right ) : &_root );
						goodHint = true;
					} else if ( ! _compare( key_, key_get_type::key( prevToHint->_key ) ) ) {
						HNode* parent( static_cast<HNode*>( prevToHint->_parent ) );
						hint.first = reinterpret_cast<HNode**>( parent ? ( parent->_left == prevToHint ? &parent->_left : &parent->_right ) : &_root );
						hint.second = parent;
						goodHint = true;
					}
				} else if ( ! _compare( key_get_type::key( proposedHint->_key ), key_ ) ) {
					HNode* parent( static_cast<HNode*>( proposedHint->_parent ) );
					hint.first = reinterpret_cast<HNode**>( parent ? ( parent->_left == proposedHint ? &parent->_left : &parent->_right ) : &_root );
					hint.second = parent;
					goodHint = true;
				}
			}
		} else {
			hint.first = reinterpret_cast<HNode**>( &_root );
			goodHint = true;
		}
		if ( ! goodHint ) {
			if ( forceHint_ ) {
				M_THROW( _errMsgHSBBSTree_[ ERROR::BAD_HINT ], static_cast<int>( ERROR::BAD_HINT ) );
			} else {
				hint = find_node( key_, &_root );
			}
		}
		return ( hint );
	}
	HNode* copy_node( HNode const* source ) {
		M_PROLOG
		HNode* node( _allocator.allocate( 1 ) );
		new ( node ) HNode( source->_key );
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
	void delete_node( HNode* node_ ) {
		M_PROLOG
		if ( node_->_left ) {
			delete_node( static_cast<HNode*>( node_->_left ) );
		}
		node_->_left = nullptr;
		if ( node_->_right ) {
			delete_node( static_cast<HNode*>( node_->_right ) );
		}
		node_->_right = nullptr;
		M_SAFE( node_->~HNode() );
		_allocator.deallocate( node_, 1 );
		M_EPILOG
	}
};

/*! \brief Iterator for HSBBSTree data structure.
 */
template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
class HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::HIterator {
	typedef HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t> owner_t;
	owner_t const* _owner;
	HAbstractNode* _current;
public:
	HIterator( void )
		: _owner( nullptr )
		, _current( nullptr ) {
		return;
	}
	HIterator( HIterator const & iterator_ )
		: _owner( iterator_._owner )
		, _current ( iterator_._current ) {
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
	key_value_t& operator*( void ) const {
		M_PROLOG
		M_ASSERT( _current );
		return ( static_cast<typename owner_t::HNode*>( _current )->_key );
		M_EPILOG
	}
	key_value_t* operator->( void ) const {
		M_PROLOG
		M_ASSERT( _current );
		return ( &static_cast<typename owner_t::HNode*>( _current )->_key );
		M_EPILOG
	}
	void const* node_id( void ) const {
		return ( _current );
	}
private:
	friend class HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>;
	explicit HIterator( owner_t const* owner_, HSBBSTreeBase::HAbstractNode* node_ )
		: _owner( owner_ )
		, _current( node_ ) {
		return;
	}
};

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
template<typename constructor_t>
HPair<typename HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::HIterator, bool>
HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::insert_impl( constructor_t&& constructor_, hint_t hint_ ) {
	M_PROLOG
	HNode* node( *hint_.first );
	bool existed( !! node );
	if ( ! existed ) {
		node = _allocator.allocate( 1 );
		try {
			constructor_.construct( node );
		} catch ( ... ) {
			_allocator.deallocate( node, 1 );
			throw;
		}
		*(hint_.first) = node;
		++ _size;
		/*
		 * _size > 1 means that _root existed before insertion of this node
		 */
		if ( _size > 1 ) {
			node->_parent = hint_.second;
			insert_rebalance( node );
		} else {
			static_cast<HNode*>( _root )->_color = HAbstractNode::BLACK;
		}
	}
	M_ASSERT( ( ! _root ) || ( static_cast<HNode*>( _root )->_parent == nullptr ) );
	M_ASSERT( ( ! _root ) || ( static_cast<HNode*>( _root )->_color == HAbstractNode::BLACK ) );
	return ( make_pair( HIterator( this, node ), ! existed ) );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
typename HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::HIterator
HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::lower_bound( key_type const& key_ ) const {
	M_PROLOG
	HIterator it( this, nullptr );
	if ( _root ) {
		hint_t hint( find_node( key_, &_root ) );
		if ( *(hint.first) ) {
			it._current = *(hint.first);
		} else {
			it._current = hint.second;
			if ( hint.first == reinterpret_cast<HNode* const*>( &( hint.second->_right ) ) ) {
				++ it;
			}
		}
	}
	return ( it );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
typename HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::HIterator
HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>::upper_bound( key_type const& key_ ) const {
	M_PROLOG
	HIterator it( this, nullptr );
	if ( _root ) {
		hint_t hint( find_node( key_, &_root ) );
		if ( *(hint.first) ) {
			it._current = *(hint.first);
			++ it;
		} else {
			it._current = hint.second;
			if ( hint.first == reinterpret_cast<HNode* const*>( &( hint.second->_right ) ) ) {
				++ it;
			}
		}
	}
	return ( it );
	M_EPILOG
}

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
inline void swap( yaal::hcore::HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>& a,
		yaal::hcore::HSBBSTree<key_value_t, compare_t, key_get_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED */


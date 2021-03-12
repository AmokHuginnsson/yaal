/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/htree.hxx
 * \brief Declaration and implementation of HTree<> class template.
 */

#ifndef YAAL_HCORE_HTREE_HXX_INCLUDED
#define YAAL_HCORE_HTREE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlist.hxx"
#include "hcore/harray.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

template<typename value_t, typename, template <typename, typename> class>
class HTree;

/*! \brief Tree based data structure and operations.
 */
template<typename value_t, typename allocator_t = allocator::system<value_t>, template <typename, typename> class sequence_t = HList>
class HTree final {
public:
	typedef HTree<value_t, allocator_t, sequence_t> this_type;
	typedef value_t value_type;
	typedef int long size_type;
	class HNode;
	typedef typename sequence_t<HNode*, allocator::ref<HNode*, allocator_t> >::allocator_type ref_branch_allocator_type;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	typedef yaal::hcore::HExceptionT<this_type> exception_type;
	/*! \brief Basic building block of HTree<>.
	 */
	class HNode final {
	public:
		template<typename const_qual_t>
		class HIterator;
		typedef value_t value_type;
		typedef sequence_t<HNode*, allocator::ref<HNode*, allocator_t> > branch_t;
		typedef HIterator<HNode> iterator;
		typedef HIterator<HNode const> const_iterator;
		typedef HReverseIterator<iterator> reverse_iterator;
		typedef HReverseIterator<const_iterator> const_reverse_iterator;
		typedef HTree<value_t, allocator_t, sequence_t> tree_t;
	private:
		value_t _data;    /*!< object itself */
		branch_t _branch; /*!< list of next level nodes */
		HNode* _trunk;    /*!< parent node */
		tree_t* _tree;    /*!< owning tree */
		typedef allocator::ref<HNode, typename allocator_t::template rebind<HNode>::other> allocator_type;
		allocator_type _allocator;
	public:
		size_type child_count( void ) const {
			M_PROLOG
			return ( _branch.size() );
			M_EPILOG
		}
		bool has_children( void ) const {
			M_PROLOG
			return ( ! _branch.is_empty() );
			M_EPILOG
		}
		const_node_t get_child_at( int index_ ) const {
			M_PROLOG
			M_ENSURE( ( index_ >= 0 ) && ( index_ < _branch.get_size() ) );
			typename branch_t::const_iterator it( _branch.begin() );
			advance( it, index_ );
			return ( *it );
			M_EPILOG
		}
		node_t get_child_at( int index_ ) {
			M_PROLOG
			M_ENSURE( ( index_ >= 0 ) && ( index_ < _branch.get_size() ) );
			typename branch_t::iterator it( _branch.begin() );
			advance( it, index_ );
			return ( *it );
			M_EPILOG
		}
		int get_level( void ) const {
			M_PROLOG
			int level = 0;
			HNode const* p = this;
			while ( ( p = p->_trunk ) )
				++ level;
			return level;
			M_EPILOG
		}
		iterator replace_node( iterator pos, typename tree_t::node_t node ) {
			M_PROLOG
			M_ASSERT( disjointed( node ) );
			M_ENSURE( pos._owner == this );
			HNode* wasted( *pos._iterator );
			if ( wasted != node ) {
				node->detach();
				if ( ( _allocator == node->_allocator ) && ( _branch.get_allocator() == node->_branch.get_allocator() ) ) {
					*pos._iterator = node;
					node->_trunk = this;
				} else {
					*pos._iterator = node->clone_self_to( _allocator, _branch.get_allocator(), this );
					allocator_type allocator( node->_allocator );
					M_SAFE( node->~HNode() );
					allocator.deallocate( node, 1 );
				}
				/* Node removal must be last in case we down-graft. */
				M_SAFE( wasted->~HNode() );
				_allocator.deallocate( wasted, 1 );
			}
			return pos;
			M_EPILOG
		}
		iterator remove_node( iterator pos ) {
			M_PROLOG
			HNode* node( *pos._iterator );
			M_SAFE( node->~HNode() );
			_allocator.deallocate( node, 1 );
			return ( iterator( this, _branch.erase( pos._iterator ) ) );
			M_EPILOG
		}
		iterator move_node( iterator const& pos, typename tree_t::node_t node ) {
			M_PROLOG
			M_ASSERT( disjointed( node ) );
			M_ENSURE( pos._owner == this );
			iterator it = pos;
			if ( *pos._iterator != node ) {
				if ( ( _allocator == node->_allocator ) && ( _branch.get_allocator() == node->_branch.get_allocator() ) ) {
					node->detach();
					it = iterator( this, _branch.insert( pos._iterator, node ) );
					node->_trunk = this;
				} else {
					it = iterator( this, _branch.insert( pos._iterator, node->clone_self_to( _allocator, _branch.get_allocator(), this ) ) );
					node->detach();
					allocator_type allocator( node->_allocator );
					M_SAFE( node->~HNode() );
					allocator.deallocate( node, 1 );
				}
			}
			return it;
			M_EPILOG
		}
		iterator move_node( typename tree_t::node_t node ) {
			M_PROLOG
			M_ASSERT( disjointed( node ) );
			iterator it = prev( end() );
			if ( ( it == prev( begin() ) ) || ( *it._iterator != node ) ) {
				if ( ( _allocator == node->_allocator ) && ( _branch.get_allocator() == node->_branch.get_allocator() ) ) {
					node->detach();
					_branch.push_back( node );
					node->_trunk = this;
				} else {
					_branch.push_back( node->clone_self_to( _allocator, _branch.get_allocator(), this ) );
					node->detach();
					allocator_type allocator( node->_allocator );
					M_SAFE( node->~HNode() );
					allocator.deallocate( node, 1 );
				}
				it = iterator( this, prev( _branch.end() ) );
			}
			return it;
			M_EPILOG
		}
		iterator copy_node( iterator const& pos, typename tree_t::node_t node ) {
			M_PROLOG
			M_ASSERT( disjointed( node ) );
			M_ENSURE( pos._owner == this );
			iterator it( this, _branch.insert( pos._iterator, node->clone_self_to( _allocator, _branch.get_allocator(), this ) ) );
			return it;
			M_EPILOG
		}
		iterator copy_node( typename tree_t::node_t node ) {
			M_PROLOG
			M_ASSERT( disjointed( node ) );
			_branch.push_back( node->clone_self_to( _allocator, _branch.get_allocator(), this ) );
			iterator it( this, prev( _branch.end() ) );
			return it;
			M_EPILOG
		}
		iterator add_node( value_t const& value ) {
			M_PROLOG
			HNode* node( _allocator.allocate( 1 ) );
			try {
				new ( node ) HNode( this, value );
			} catch ( ... ) {
				_allocator.deallocate( node, 1 );
				throw;
			}
			try {
				_branch.push_back( node );
			} catch ( ... ) {
				node->~HNode();
				_allocator.deallocate( node, 1 );
				throw;
			}
			return ( iterator( this, prev( _branch.end() ) ) );
			M_EPILOG
		}
		iterator add_node( void ) {
			M_PROLOG
			HNode* node( _allocator.allocate( 1 ) );
			try {
				new ( node ) HNode( this );
			} catch ( ... ) {
				_allocator.deallocate( node, 1 );
				throw;
			}
			try {
				_branch.push_back( node );
			} catch ( ... ) {
				node->~HNode();
				_allocator.deallocate( node, 1 );
				throw;
			}
			return ( iterator( this, prev( _branch.end() ) ) );
			M_EPILOG
		}
		iterator insert_node( iterator const& pos, value_t const& value ) {
			M_PROLOG
			M_ASSERT( pos._owner == this );
			HNode* node( _allocator.allocate( 1 ) );
			try {
				new ( node ) HNode( this, value );
			} catch ( ... ) {
				_allocator.deallocate( node, 1 );
				throw;
			}
			typename branch_t::iterator bIt;
			try {
				bIt = _branch.insert( pos._iterator, node );
			} catch ( ... ) {
				node->~HNode();
				_allocator.deallocate( node, 1 );
				throw;
			}
			return ( iterator( this, bIt ) );
			M_EPILOG
		}
		iterator begin( void ) {
			return ( iterator( this, _branch.begin() ) );
		}
		const_iterator begin( void ) const {
			return ( const_iterator( this, _branch.begin() ) );
		}
		iterator end( void ) {
			return ( iterator( this, _branch.end() ) );
		}
		const_iterator end( void ) const {
			return ( const_iterator( this, _branch.end() ) );
		}
		reverse_iterator rbegin( void ) {
			return ( end() );
		}
		const_reverse_iterator rbegin( void ) const {
			return ( end() );
		}
		reverse_iterator rend( void ) {
			return ( begin() );
		}
		const_reverse_iterator rend( void ) const {
			return ( begin() );
		}
		node_t get_parent( void ) {
			return ( _trunk );
		}
		const_node_t get_parent( void ) const {
			return ( _trunk );
		}
		tree_t& get_tree( void ) {
			HNode* node( this );
			while ( node->_trunk )
				node = node->_trunk;
			M_ASSERT( node->_tree );
			return ( *node->_tree );
		}
		tree_t const& get_tree( void ) const {
			HNode const* node( this );
			while ( node->_trunk )
				node = node->_trunk;
			M_ASSERT( node->_tree );
			return ( *node->_tree );
		}
		value_t& operator* ( void ) {
			return ( _data );
		}
		value_t const& operator* ( void ) const {
			return ( _data );
		}
		value_t* operator->( void ) {
			return ( &_data );
		}
		value_t const* operator->( void ) const {
			return ( &_data );
		}
		bool disjointed( typename tree_t::const_node_t node ) const {
			M_PROLOG
			HNode* p( _trunk );
			while ( p ) {
				if ( p == node ) {
					break;
				}
				p = p->_trunk;
			}
			return ( p == nullptr );
			M_EPILOG
		}
	private:
		/*! \brief Create a root node.
		 */
		HNode( tree_t* tree_ )
			: _data()
			, _branch( ref_branch_allocator_type( &tree_->_branchAllocator ) )
			, _trunk( nullptr )
			, _tree( tree_ )
			, _allocator( allocator_type( &tree_->_allocator ) ) {
			M_PROLOG
			M_ASSERT( tree_ );
			return;
			M_EPILOG
		}
		/*! \brief Create a root node.
		 */
		HNode( tree_t* tree_, value_type const& value_ )
			: _data( value_ )
			, _branch( ref_branch_allocator_type( &tree_->_branchAllocator ) )
			, _trunk( nullptr )
			, _tree( tree_ )
			, _allocator( allocator_type( &tree_->_allocator ) ) {
			M_PROLOG
			M_ASSERT( tree_ );
			return;
			M_EPILOG
		}
		/*! \brief  Create a child node.
		 */
		HNode( HNode* node_ )
			: _data()
			, _branch( node_->_branch.get_allocator() )
			, _trunk( node_ )
			, _tree( nullptr )
			, _allocator( node_->_allocator ) {
			M_PROLOG
			M_ASSERT( node_ );
			return;
			M_EPILOG
		}
		/*! \brief Clone node.
		 */
		HNode( value_t const& data, ref_branch_allocator_type const& branchAllocator_, allocator_type const& allocator_ )
			: _data( data )
			, _branch( branchAllocator_ )
			, _trunk( nullptr )
			, _tree( nullptr )
			, _allocator( allocator_ ) {
			M_PROLOG
			return;
			M_EPILOG
		}
		/*! \brief  Clone a child node.
		 */
		HNode( node_t node_, value_t const& data )
			: _data( data )
			, _branch( node_->_branch.get_allocator() )
			, _trunk( node_ )
			, _tree( nullptr )
			, _allocator( node_->_allocator ) {
			M_PROLOG
			M_ASSERT( node_ );
			return;
			M_EPILOG
		}
		~HNode( void ) {
			M_PROLOG
			for ( typename branch_t::iterator it( _branch.begin() ), endIt( _branch.end() ); it != endIt; ++ it ) {
				HNode* node( *it );
				M_SAFE( node->~HNode() );
				_allocator.deallocate( node, 1 );
			}
			return;
			M_DESTRUCTOR_EPILOG
		}
		HNode( HNode const& ) = delete;
		HNode& operator = ( HNode const& ) = delete;
		void detach( void ) {
			M_PROLOG
			if ( _tree ) {
				_tree->_root = nullptr;
				_tree = nullptr;
			} else {
				typename branch_t::iterator endIt = _trunk->_branch.end();
				for ( typename branch_t::iterator it = _trunk->_branch.begin();
						it != endIt; ++ it ) {
					if ( *it == this ) {
						*it = nullptr;
						_trunk->_branch.erase( it );
						break;
					}
				}
			}
			return;
			M_EPILOG
		}
		node_t clone_self_to( allocator_type& allocator_, ref_branch_allocator_type const& branchAllocator_, HNode* parent ) const {
			M_PROLOG
			node_t node( allocator_.allocate( 1 ) );
			try {
				new ( node ) HNode( _data, branchAllocator_, allocator_ );
			} catch ( ... ) {
				allocator_.deallocate( node, 1 );
				throw;
			}
			node->_trunk = parent;
			typename branch_t::const_iterator endIt = _branch.end();
			for ( typename branch_t::const_iterator it = _branch.begin();
					it != endIt; ++ it ) {
				node->_branch.push_back( (*it)->clone_self_to( allocator_, branchAllocator_, node ) );
			}
			return node;
			M_EPILOG
		}
		friend class HTree<value_t, allocator_t, sequence_t>;
		friend class HList<HNode*, allocator_t>;
	};

	typedef typename allocator_t::template rebind<HNode>::other allocator_type;
	typedef typename ref_branch_allocator_type::allocator_type branch_allocator_type;
	typedef trait::delayed_sizeof<HNode> node_size;
	typedef HNode node_type;

	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_t> iterator;
	typedef HIterator<value_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	allocator_type _allocator;
	branch_allocator_type _branchAllocator;
	HNode* _root; /* self explanatory */
public:
	explicit HTree( allocator_type const& allocator_ = allocator_type() )
		: _allocator( allocator_ )
		, _branchAllocator()
		, _root( nullptr ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	explicit HTree( branch_allocator_type const& allocator_ )
		: _allocator()
		, _branchAllocator( allocator_ )
		, _root( nullptr ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	HTree( allocator_type const& allocator_, branch_allocator_type const& branchAllocator_ )
		: _allocator( allocator_ )
		, _branchAllocator( branchAllocator_ )
		, _root( nullptr ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	~HTree( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HTree( HTree const& t )
		: _allocator( t._allocator )
		, _branchAllocator( t._branchAllocator )
		, _root( nullptr ) {
		M_PROLOG
		if ( t._root ) {
			typename HNode::allocator_type allocator( &_allocator );
			_root = t._root->clone_self_to( allocator, ref_branch_allocator_type( &_branchAllocator ), nullptr );
		}
		_root && ( _root->_tree = this );
		return;
		M_EPILOG
	}
	HTree( HTree&& tree_ )
		: _allocator()
		, _branchAllocator()
		, _root( nullptr ) {
		M_PROLOG
		swap( tree_ );
		return;
		M_EPILOG
	}
	HTree( HTree const& t, allocator_type const& allocator_ )
		: _allocator( allocator_ )
		, _branchAllocator( t._branchAllocator )
		, _root( nullptr ) {
		M_PROLOG
		if ( t._root ) {
			typename HNode::allocator_type allocator( &_allocator );
			_root = t._root->clone_self_to( allocator, ref_branch_allocator_type( &_branchAllocator ), nullptr );
		}
		_root && ( _root->_tree = this );
		return;
		M_EPILOG
	}
	HTree& operator = ( HTree const& t ) {
		M_PROLOG
		if ( &t != this ) {
			HTree tmp( t );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	HTree& operator = ( HTree&& tree_ ) {
		M_PROLOG
		if ( &tree_ != this ) {
			swap( tree_ );
			tree_.clear();
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HTree& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _root, other._root );
			/* after swap */
			_root && ( _root->_tree = this );
			other._root && ( other._root->_tree = &other );
		}
		return;
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	branch_allocator_type const& get_branch_allocator( void ) const {
		return ( _branchAllocator );
	}
	node_t get_root( void ) {
		return ( _root );
	}
	const_node_t get_root( void ) const {
		return ( _root );
	}
	node_t create_new_root( value_type const& value_ = value_type() ) {
		M_PROLOG
		clear();
		HNode* node( _allocator.allocate( 1 ) );
		try {
			new ( node ) HNode( this, value_ );
		} catch ( ... ) {
			_allocator.deallocate( node, 1 );
			throw;
		}
		_root = node;
		return ( _root );
		M_EPILOG
	}
	node_t set_new_root( node_t node ) {
		M_PROLOG
		if ( node != _root ) {
			HNode* wasted = _root;
			node->detach();
			_root = node;
			node->_tree = this;
			node->_trunk = nullptr;
			M_SAFE( wasted->~HNode() );
			_allocator.deallocate( wasted, 1 );
		}
		return node;
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		if ( _root ) {
			HNode* node( _root );
			M_SAFE( node->~HNode() );
			_allocator.deallocate( node, 1 );
		}
		_root = nullptr;
		M_EPILOG
	}
	bool is_empty( void ) const {
		return ( _root ? false : true );
	}
	bool empty( void ) const {
		M_PROLOG
		return ( is_empty() );
		M_EPILOG
	}
	iterator begin( void ) {
		M_PROLOG
		iterator it( this );
		++ it;
		return it;
		M_EPILOG
	}
	const_iterator begin( void ) const {
		M_PROLOG
		const_iterator it( this );
		++ it;
		return it;
		M_EPILOG
	}
	iterator end( void ) {
		M_PROLOG
		return ( iterator( this ) );
		M_EPILOG
	}
	const_iterator end( void ) const {
		M_PROLOG
		return ( const_iterator( this ) );
		M_EPILOG
	}
	reverse_iterator rbegin( void ) {
		M_PROLOG
		return ( reverse_iterator( end() ) );
		M_EPILOG
	}
	const_reverse_iterator rbegin( void ) const {
		M_PROLOG
		return ( const_reverse_iterator( end() ) );
		M_EPILOG
	}
	reverse_iterator rend( void ) {
		M_PROLOG
		return ( reverse_iterator( begin() ) );
		M_EPILOG
	}
	const_reverse_iterator rend( void ) const {
		M_PROLOG
		return ( const_reverse_iterator( begin() ) );
		M_EPILOG
	}
private:
	friend class HTree<value_type, allocator_t, sequence_t>::HNode;
};

/*! \brief Iterator for HTree<>::HNode data structure.
 */
template<typename value_t, typename allocator_t, template <typename, typename> class sequence_t>
template<typename const_qual_t>
class HTree<value_t, allocator_t, sequence_t>::HNode::HIterator
	: public iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type, typename sequence_t<HNode*, allocator_t>::iterator::category_type> {
	typedef iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type, typename sequence_t<HNode*, allocator_t>::iterator::category_type> base_type;
	typedef HTree<value_t, allocator_t, sequence_t> tree_t;
	typedef typename tree_t::const_node_t owner_t;
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t, allocator_t, sequence_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t> list_it_t;
	owner_t _owner;
	list_it_t _iterator;
public:
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _iterator() {
		return;
	}
	HIterator( HIterator const& it )
		: base_type()
		, _owner( it._owner )
		, _iterator( it._iterator ) {
		return;
	}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it )
		: base_type()
		, _owner( it._owner )
		, _iterator( it._iterator ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
		return;
	}
	HIterator& operator = ( HIterator const& it ) {
		M_PROLOG
		if ( &it != this ) {
			_owner = it._owner;
			_iterator = it._iterator;
		}
		return ( *this );
		M_EPILOG
	}
	HIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _iterator;
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _iterator;
		return it;
	}
	HIterator operator + ( size_type off_ ) const {
		HIterator it( _owner, _iterator + off_ );
		return it;
	}
	HIterator& operator += ( size_type off_ ) {
		_iterator += off_;
		return ( *this );
	}
	HIterator operator - ( size_type off_ ) const {
		HIterator it( _owner, _iterator - off_ );
		return it;
	}
	HIterator& operator -= ( size_type off_ ) {
		_iterator -= off_;
		return ( *this );
	}
	size_type operator - ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _iterator - it._iterator );
	}
	const_qual_node_t& operator* ( void ) const {
		return ( **_iterator );
	}
	const_qual_node_t& operator* ( void ) {
		return ( **_iterator );
	}
	const_qual_node_t* operator->( void ) const {
		return ( *_iterator );
	}
	const_qual_node_t* operator->( void ) {
		return ( *_iterator );
	}
	bool operator == ( HIterator const& it ) const {
		M_PROLOG
		M_ASSERT( _owner == it._owner );
		return ( _iterator == it._iterator );
		M_EPILOG
	}
	bool operator != ( HIterator const& it ) const {
		M_PROLOG
		M_ASSERT( _owner == it._owner );
		return ( _iterator != it._iterator );
		M_EPILOG
	}
	void const* node_id( void ) const {
		return ( _iterator.node_id() );
	}
private:
	friend class HTree<value_t, allocator_t, sequence_t>::HNode;
	template<typename other_const_qual_t>
	friend class HIterator;
	HIterator( const_node_t owner_, list_it_t const& iterator_ )
		: base_type()
		, _owner( owner_ )
		, _iterator( iterator_ ) {
		return;
	}
};

/*! \brief Iterator for HTree<> data structure.
 */
template<typename value_t, typename allocator_t, template <typename, typename> class sequence_t>
template<typename const_qual_t>
class HTree<value_t, allocator_t, sequence_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::forward> {
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	typedef HTree<value_t, allocator_t, sequence_t> tree_t;
	typedef tree_t const* owner_t;
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t, allocator_t, sequence_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t> list_it_t;
	typedef HArray<list_it_t> it_stack_t;
	owner_t _owner;
	it_stack_t _track;
public:
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _track() {
		return;
	}
	HIterator( HIterator const& it )
		: base_type()
		, _owner( it._owner )
		, _track( it._track ) {
		return;
	}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it )
		: base_type()
		, _owner( it._owner )
		, _track( it._track.begin(), it._track.end() ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
		return;
	}
	HIterator& operator = ( HIterator const& it ) {
		M_PROLOG
		if ( &it != this ) {
			_owner = it._owner;
			_track = it._track;
		}
		return ( *this );
		M_EPILOG
	}
	HIterator& operator ++ ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( ! _track.is_empty() ) {
			list_it_t thisIt( _track.back() );
			_track.pop_back();
			if ( ! _track.is_empty() ) {
				const_qual_node_ptr_t parent( ( _track.get_size() > 1 ) ? *_track.back() : _owner->_root );
				++ thisIt;
				if ( thisIt != parent->_branch.end() )
					descent( *thisIt, thisIt );
			}
		} else if ( _owner->_root )
			descent( _owner->_root, typename HNode::branch_t::iterator() );
		return ( *this );
		M_EPILOG
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		operator ++ ();
		return it;
	}
	HIterator& operator -- ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( ! _track.is_empty() ) {
			const_qual_node_ptr_t node( _track.get_size() > 1 ? *_track.back() : _owner->_root );
			if ( node->has_children() ) {
				list_it_t last( node->_branch.end() );
				_track.push_back( -- last );
			} else {
				list_it_t thisIt;
				do {
					thisIt = _track.back();
					_track.pop_back();
				} while ( ( _track.get_size() > 1 ) && ( thisIt == (*_track.back())->_branch.begin() ) );
				if ( _track.get_size() > 1 ) {
					_track.push_back( -- thisIt );
				} else {
					if ( thisIt != _owner->_root->_branch.begin() )
						_track.push_back( -- thisIt );
					else
						_track.pop_back();
				}
			}
		} else
			_track.push_back( typename HNode::branch_t::iterator() );
		return ( *this );
		M_EPILOG
	}

	HIterator operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return it;
	}
	bool operator == ( HIterator const& it ) const {
		M_PROLOG
		M_ASSERT( _owner == it._owner );
		bool lEmpty( _track.is_empty() );
		bool rEmpty( it._track.is_empty() );
		return ( ( lEmpty && rEmpty ) || ( ! ( lEmpty || rEmpty ) && ( _track.back() == it._track.back() ) ) );
		M_EPILOG
	}
	bool operator != ( HIterator const& it ) const {
		M_PROLOG
		M_ASSERT( _owner == it._owner );
		bool lEmpty( _track.is_empty() );
		bool rEmpty( it._track.is_empty() );
		return ( ( lEmpty && ! rEmpty ) || ( ! lEmpty && rEmpty ) || ( ! ( lEmpty || rEmpty ) && ( _track.back() != it._track.back() ) ) );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) const {
		return ( _track.get_size() > 1 ? ***_track.back() : **_owner->_root );
	}
	const_qual_t& operator* ( void ) {
		return ( _track.get_size() > 1 ? ***_track.back() : **_owner->_root );
	}
	const_qual_t* operator->( void ) const {
		return ( _track.get_size() > 1 ? &***_track.back() : &**_owner->_root );
	}
	const_qual_t* operator->( void ) {
		return ( _track.get_size() > 1 ? &***_track.back() : &**_owner->_root );
	}
private:
	friend class HTree<value_t, allocator_t, sequence_t>::HNode;
	template<typename other_const_qual_t>
	friend class HIterator;
	HIterator( owner_t owner_ )
		: base_type()
		, _owner( owner_ )
		, _track() {
		return;
	}
	void descent( const_qual_node_t* n_, list_it_t const& start_ ) {
		M_PROLOG
		_track.push_back( start_ );
		while ( n_ && n_->has_children() ) {
			_track.push_back( n_->_branch.begin() );
			n_ = n_->_branch.head();
		}
		return;
		M_EPILOG
	}
};

template<typename value_t, typename allocator_t, template <typename, typename> class sequence_t>
inline void swap( yaal::hcore::HTree<value_t, allocator_t, sequence_t>& a, yaal::hcore::HTree<value_t, allocator_t, sequence_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HTREE_HXX_INCLUDED */


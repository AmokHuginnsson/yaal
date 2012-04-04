/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htree.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must atttach Copyright information to any part of every copy
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

#ifndef YAAL_HCORE_HTREE_HXX_INCLUDED
#define YAAL_HCORE_HTREE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlist.hxx"
#include "hcore/harray.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

template<typename value_t, template <typename> class sequence_t>
class HTree;

/*! \brief Tree based data structure and operations.
 */
template<typename type_t, template <typename> class sequence_t = HList>
class HTree {
private:
	typedef HTree<type_t, sequence_t> tree_t;
public:
	typedef type_t value_type;
	class HNode;
	typedef HNode* node_t;
	typedef HNode const* const_node_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<type_t> iterator;
	typedef HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	HNode* _root;			/* self explanary */
public:
	HTree( void );
	HTree( HTree const& );
	HTree& operator = ( HTree const& );
	virtual ~HTree( void );
	node_t get_root( void );
	const_node_t get_root( void ) const;
	node_t create_new_root( value_type const& = value_type() );
	node_t set_new_root( node_t );
	void clear( void );
	void swap( HTree<value_type, sequence_t>& );
	bool is_empty( void ) const;
	bool empty( void ) const;
	iterator begin( void );
	const_iterator begin( void ) const;
	iterator end( void );
	const_iterator end( void ) const;
	reverse_iterator rbegin( void );
	const_reverse_iterator rbegin( void ) const;
	reverse_iterator rend( void );
	const_reverse_iterator rend( void ) const;
private:
	friend class HTree<value_type>::HNode;
};

/*! \brief Basic building block of HTree<>.
 */
template<typename value_t, template <typename> class sequence_t>
class HTree<value_t, sequence_t>::HNode {
public:
	template<typename const_qual_t>
	class HIterator;
	typedef value_t value_type;
	typedef sequence_t<node_t> branch_t;
	typedef HIterator<HNode> iterator;
	typedef HIterator<HNode const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	value_t _data;    /* object itself */
	branch_t _branch; /* list of next level nodes */
	node_t _trunk;    /* self explanary */
	HTree<value_t, sequence_t>* _tree;
public:
	int long child_count( void ) const;
	bool has_childs( void ) const;
	int get_level( void ) const;
	iterator move_node( iterator const&, typename tree_t::node_t );
	iterator move_node( typename tree_t::node_t );
	iterator copy_node( iterator const&, typename tree_t::node_t );
	iterator copy_node( typename tree_t::node_t );
	iterator insert_node( iterator const&, value_t const& );
	iterator add_node( value_t const& );
	iterator add_node( void );
	iterator replace_node( iterator, typename tree_t::node_t );
	iterator remove_node( iterator );
	iterator begin( void );
	const_iterator begin( void ) const;
	iterator end( void );
	const_iterator end( void ) const;
	reverse_iterator rbegin( void );
	const_reverse_iterator rbegin( void ) const;
	reverse_iterator rend( void );
	const_reverse_iterator rend( void ) const;
	node_t get_parent( void );
	const_node_t get_parent( void ) const;
	tree_t& get_tree( void );
	tree_t const& get_tree( void ) const;
	value_t* operator->( void );
	value_t const* operator->( void ) const;
	value_t& operator* ( void );
	value_t const& operator* ( void ) const;
private:
	HNode( tree_t* ); /* to create a root node */
	HNode( tree_t*, value_type const& ); /* to create a root node */
	HNode( HNode* ); /* to create a child node */
	HNode( value_t const& ); /* to clone node */
	HNode( HNode*, value_t const& ); /* to clone a child node */
	virtual ~HNode( void );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	void detach( void );
	HNode* clone( HNode* ) const;
	void disjointed( iterator const&, typename tree_t::node_t ) const;
	friend class HTree<value_t, sequence_t>;
	friend class HList<HNode*>;
};

/*! \brief Iterator for HTree<>::HNode data structure.
 */
template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
class HTree<value_t, sequence_t>::HNode::HIterator : public iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type, iterator_category::forward> {
	typedef iterator_interface<typename trait::copy_const<const_qual_t, HNode>::type, iterator_category::forward> base_type;
	typedef HTree<value_t, sequence_t> tree_t;
	typedef typename tree_t::const_node_t owner_t;
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t, sequence_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::TREAT_AS_OPENED> list_it_t;
	owner_t _owner;
	list_it_t _iterator;
public:
	HIterator( void );
	HIterator( HIterator const& );
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& );
	HIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _iterator;
		return ( it );
	}
	HIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _iterator;
		return ( it );
	}
	typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	const_qual_node_t& operator* ( void );
	const_qual_node_t& operator* ( void ) const;
	const_qual_node_t* operator->( void );
	const_qual_node_t* operator->( void ) const;
private:
	friend class HTree<value_t, sequence_t>::HNode;
	template<typename other_const_qual_t>
	friend class HIterator;
	HIterator( const_node_t, list_it_t const& );
};

/*! \brief Iterator for HTree<> data structure.
 */
template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
class HTree<value_t, sequence_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::forward> {
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	typedef HTree<value_t, sequence_t> tree_t;
	typedef tree_t const* owner_t;
	typedef typename trait::copy_const<const_qual_t, HNode>::type const_qual_node_t;
	typedef typename trait::copy_const<const_qual_t, HNode*>::type const_qual_node_ptr_t;
	typedef typename HTree<value_t, sequence_t>::HNode::branch_t::template HIterator<const_qual_node_ptr_t, OListBits::TREAT_AS_OPENED> list_it_t;
	typedef HArray<list_it_t> it_stack_t;
	owner_t _owner;
	it_stack_t _track;
public:
	HIterator( void );
	HIterator( HIterator const& );
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& );
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
		return ( it );
	}
	HIterator& operator -- ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( ! _track.is_empty() ) {
			const_qual_node_ptr_t node( _track.get_size() > 1 ? *_track.back() : _owner->_root );
			if ( node->has_childs() ) {
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
		return ( it );
	}
	typename HTree<value_t, sequence_t>::template HIterator<const_qual_t>& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	const_qual_t& operator* ( void );
	const_qual_t& operator* ( void ) const;
	const_qual_t* operator->( void );
	const_qual_t* operator->( void ) const;
private:
	friend class HTree<value_t, sequence_t>::HNode;
	template<typename other_const_qual_t>
	friend class HIterator;
	HIterator( owner_t );
	void descent( const_qual_node_t*, list_it_t const& );
};

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::HNode( tree_t* tree_ ) : _data(),
	_branch(), _trunk( NULL ), _tree( tree_ ) {
	M_PROLOG
	M_ASSERT( tree_ );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::HNode( tree_t* tree_, value_type const& value_ ) : _data( value_ ),
	_branch(), _trunk( NULL ), _tree( tree_ ) {
	M_PROLOG
	M_ASSERT( tree_ );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::HNode( HNode* node_ ) : _data(),
	_branch(), _trunk( node_ ), _tree( NULL ) {
	M_PROLOG
	M_ASSERT( node_ );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::HNode( value_t const& data ) : _data( data ),
	_branch(), _trunk( NULL ), _tree( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::HNode( node_t node_, value_t const& data ) : _data( data ),
	_branch(), _trunk( node_ ), _tree( NULL ) {
	M_PROLOG
	M_ASSERT( node_ );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HNode::~HNode( void ) {
	M_PROLOG
	struct tool {
		static void deleter( node_t& node )
			{ M_SAFE( delete node ); }
	};
	for_each( _branch.begin(), _branch.end(), cref( tool::deleter ) );
	return;
	M_DESTRUCTOR_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
int long HTree<value_t, sequence_t>::HNode::child_count( void ) const {
	M_PROLOG
	return ( _branch.size() );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
bool HTree<value_t, sequence_t>::HNode::has_childs( void ) const {
	M_PROLOG
	return ( ! _branch.is_empty() );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::begin( void ) {
	return ( iterator( this, _branch.begin() ) );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::const_iterator HTree<value_t, sequence_t>::HNode::begin( void ) const {
	return ( const_iterator( this, _branch.begin() ) );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::end( void ) {
	return ( iterator( this, _branch.end() ) );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::const_iterator HTree<value_t, sequence_t>::HNode::end( void ) const {
	return ( const_iterator( this, _branch.end() ) );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::reverse_iterator HTree<value_t, sequence_t>::HNode::rbegin( void ) {
	return ( end() );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::const_reverse_iterator HTree<value_t, sequence_t>::HNode::rbegin( void ) const {
	return ( end() );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::reverse_iterator HTree<value_t, sequence_t>::HNode::rend( void ) {
	return ( begin() );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::const_reverse_iterator HTree<value_t, sequence_t>::HNode::rend( void ) const {
	return ( begin() );
}

template<typename value_t, template <typename> class sequence_t>
value_t& HTree<value_t, sequence_t>::HNode::operator* ( void ) {
	return ( _data );
}

template<typename value_t, template <typename> class sequence_t>
value_t const& HTree<value_t, sequence_t>::HNode::operator* ( void ) const {
	return ( _data );
}

template<typename value_t, template <typename> class sequence_t>
value_t* HTree<value_t, sequence_t>::HNode::operator->( void ) {
	return ( &_data );
}

template<typename value_t, template <typename> class sequence_t>
value_t const* HTree<value_t, sequence_t>::HNode::operator->( void ) const {
	return ( &_data );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::add_node( value_t const& value ) {
	M_PROLOG
	_branch.push_back( new ( memory::yaal ) HNode( this, value ) );
	return ( iterator( this, _branch.rbegin().base() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::add_node( void ) {
	M_PROLOG
	_branch.push_back( new ( memory::yaal ) HNode( this ) );
	return ( iterator( this, _branch.rbegin().base() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::insert_node( iterator const& pos, value_t const& value ) {
	M_PROLOG
	M_ASSERT( pos._owner == this );
	iterator it( this, _branch.insert( pos._iterator, new ( memory::yaal ) HNode( this, value ) ) );
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
void HTree<value_t, sequence_t>::HNode::disjointed( iterator const& pos, typename tree_t::node_t node ) const {
	M_PROLOG
	HNode* p = _trunk;
	while ( p ) {
		M_ASSERT( p != node );
		p = p->_trunk;
	}
	M_ASSERT( pos._owner == this );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::replace_node( iterator pos, typename tree_t::node_t node ) {
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	if ( *pos._iterator != node ) {
		node->detach();
		HNode* wasted = *pos._iterator;
		*pos._iterator = node;
		node->_trunk = this;
		M_SAFE( delete wasted );
	}
	return ( pos );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::remove_node( iterator pos ) {
	M_PROLOG
	M_SAFE( delete *pos._iterator );
	return ( iterator( this, _branch.erase( pos._iterator ) ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::move_node( iterator const& pos, typename tree_t::node_t node ) {
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	iterator it = pos;
	if ( *pos._iterator != node ) {
		node->detach();
		it = iterator( this, _branch.insert( pos._iterator, node ) );
		node->_trunk = this;
	}
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::move_node( typename tree_t::node_t node ) {
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( begin(), node );
#endif /* defined( __DEBUG__ ) */
	iterator it = rbegin().base();
	if ( ( it == rend().base() ) || ( *it._iterator != node ) ) {
		node->detach();
		_branch.push_back( node );
		it = iterator( this, _branch.rbegin().base() );
		node->_trunk = this;
	}
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::copy_node( iterator const& pos, typename tree_t::node_t node ) {
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( pos, node );
#endif /* defined( __DEBUG__ ) */
	iterator it( this, _branch.insert( pos._iterator, node->clone( this ) ) );
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::HNode::iterator HTree<value_t, sequence_t>::HNode::copy_node( typename tree_t::node_t node ) {
	M_PROLOG
#if defined( __DEBUG__ )
	disjointed( rbegin().base(), node );
#endif /* defined( __DEBUG__ ) */
	_branch.push_back( node->clone( this ) );
	iterator it( this, _branch.rbegin().base() );
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::node_t HTree<value_t, sequence_t>::HNode::clone( HNode* parent ) const {
	M_PROLOG
	node_t node = new ( memory::yaal ) HNode( _data );
	node->_trunk = parent;
	typename branch_t::const_iterator endIt = _branch.end();
	for ( typename branch_t::const_iterator it = _branch.begin();
			it != endIt; ++ it )
		node->_branch.push_back( (*it)->clone( node ) );
	return ( node );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::node_t HTree<value_t, sequence_t>::HNode::get_parent( void ) {
	return ( _trunk );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_node_t HTree<value_t, sequence_t>::HNode::get_parent( void ) const {
	return ( _trunk );
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>& HTree<value_t, sequence_t>::HNode::get_tree( void ) {
	HNode* node( this );
	while ( node->_trunk )
		node = node->_trunk;
	M_ASSERT( node->_tree );
	return ( *node->_tree );
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t> const& HTree<value_t, sequence_t>::HNode::get_tree( void ) const {
	HNode* node( this );
	while ( node->_trunk )
		node = node->_trunk;
	M_ASSERT( node->_tree );
	return ( *node->_tree );
}

template<typename value_t, template <typename> class sequence_t>
void HTree<value_t, sequence_t>::HNode::detach( void ) {
	M_PROLOG
	if ( _tree ) {
		_tree->_root = NULL;
		_tree = NULL;
	} else {
		typename branch_t::iterator endIt = _trunk->_branch.end();
		for ( typename branch_t::iterator it = _trunk->_branch.begin();
				it != endIt; ++ it ) {
			if ( *it == this ) {
				*it = NULL;
				_trunk->_branch.erase( it );
				break;
			}
		}
	}
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
int HTree<value_t, sequence_t>::HNode::get_level( void ) const {
	M_PROLOG
	int level = 0;
	HNode const* p = this;
	while ( ( p = p->_trunk ) )
		++ level;
	return ( level );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::HIterator( void )
	: base_type(), _owner( NULL ), _iterator() {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::HIterator( const_node_t owner_, list_it_t const& iterator_ )
	: base_type(), _owner( owner_ ), _iterator( iterator_ ) {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::HIterator( HIterator const& it )
	: base_type(), _owner( it._owner ), _iterator( it._iterator ) {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
template<typename other_const_qual_t>
HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::HIterator( HIterator<other_const_qual_t> const& it )
	: base_type(), _owner( it._owner ), _iterator( it._iterator ) {
	STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>& HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator = ( HIterator const& it ) {
	M_PROLOG
	if ( &it != this ) {
		_owner = it._owner;
		_iterator = it._iterator;
	}
	return ( *this );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator* ( void ) const {
	return ( **_iterator );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>::const_qual_node_t& HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator* ( void ) {
	return ( **_iterator );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>::const_qual_node_t* HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator->( void ) const {
	return ( *_iterator );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::HNode::template HIterator<const_qual_t>::const_qual_node_t* HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator->( void ) {
	return ( *_iterator );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
bool HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator == ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	return ( _iterator == it._iterator );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
bool HTree<value_t, sequence_t>::HNode::HIterator<const_qual_t>::operator != ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	return ( _iterator != it._iterator );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HTree( void )
	: _root( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::~HTree( void ) {
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>::HTree( HTree const& t )
	: _root( t._root ? t._root->clone( NULL ) : NULL ) {
	M_PROLOG
	_root && ( _root->_tree = this );
	return;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
HTree<value_t, sequence_t>& HTree<value_t, sequence_t>::operator = ( HTree const& t ) {
	M_PROLOG
	if ( &t != this ) {
		HTree<value_t, sequence_t> tmp( t );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
void HTree<value_t, sequence_t>::clear( void ) {
	M_PROLOG
	if ( _root )
		M_SAFE( delete _root );
	_root = NULL;
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
bool HTree<value_t, sequence_t>::is_empty( void ) const {
	return ( _root ? false : true );
}

template<typename value_t, template <typename> class sequence_t>
bool HTree<value_t, sequence_t>::empty( void ) const {
	M_PROLOG
	return ( is_empty() );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
void HTree<value_t, sequence_t>::swap( HTree<value_t, sequence_t>& other ) {
	if ( &other != this ) {
		using yaal::swap;
		swap( _root, other._root );
		/* after swap */
		_root && ( _root->_tree = this );
		other._root && ( other._root->_tree = &other );
	}
	return;
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::node_t HTree<value_t, sequence_t>::create_new_root( value_type const& value_ ) {
	M_PROLOG
	clear();
	_root = new ( memory::yaal ) HNode( this, value_ );
	return ( _root );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::node_t HTree<value_t, sequence_t>::set_new_root( typename tree_t::node_t node ) {
	M_PROLOG
	if ( node != _root ) {
		HNode* wasted = _root;
		node->detach();
		_root = node;
		node->_tree = this;
		node->_trunk = NULL;
		M_SAFE( delete wasted );
	}
	return ( node );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::iterator HTree<value_t, sequence_t>::begin( void ) {
	M_PROLOG
	iterator it( this );
	++ it;
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_iterator HTree<value_t, sequence_t>::begin( void ) const {
	M_PROLOG
	const_iterator it( this );
	++ it;
	return ( it );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::iterator HTree<value_t, sequence_t>::end( void ) {
	M_PROLOG
	return ( iterator( this ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_iterator HTree<value_t, sequence_t>::end( void ) const {
	M_PROLOG
	return ( const_iterator( this ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::reverse_iterator HTree<value_t, sequence_t>::rbegin( void ) {
	M_PROLOG
	return ( reverse_iterator( end() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_reverse_iterator HTree<value_t, sequence_t>::rbegin( void ) const {
	M_PROLOG
	return ( const_reverse_iterator( end() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::reverse_iterator HTree<value_t, sequence_t>::rend( void ) {
	M_PROLOG
	return ( reverse_iterator( begin() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_reverse_iterator HTree<value_t, sequence_t>::rend( void ) const {
	M_PROLOG
	return ( const_reverse_iterator( begin() ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::node_t HTree<value_t, sequence_t>::get_root( void ) {
	return ( _root );
}

template<typename value_t, template <typename> class sequence_t>
typename HTree<value_t, sequence_t>::const_node_t HTree<value_t, sequence_t>::get_root( void ) const {
	return ( _root );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HIterator<const_qual_t>::HIterator( void )
	: base_type(), _owner( NULL ), _track() {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HIterator<const_qual_t>::HIterator( owner_t owner_ )
	: base_type(), _owner( owner_ ), _track() {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
HTree<value_t, sequence_t>::HIterator<const_qual_t>::HIterator( HIterator const& it )
	: base_type(), _owner( it._owner ), _track( it._track ) {
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
template<typename other_const_qual_t>
HTree<value_t, sequence_t>::HIterator<const_qual_t>::HIterator( HIterator<other_const_qual_t> const& it )
	: base_type(), _owner( it._owner ), _track( it._track.begin(), it._track.end() ) {
	STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
	return;
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
typename HTree<value_t, sequence_t>::template HIterator<const_qual_t>& HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator = ( HIterator const& it ) {
	M_PROLOG
	if ( &it != this ) {
		_owner = it._owner;
		_track = it._track;
	}
	return ( *this );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
const_qual_t& HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator* ( void ) const {
	return ( _track.get_size() > 1 ? ***_track.back() : **_owner->_root );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
const_qual_t& HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator* ( void ) {
	return ( _track.get_size() > 1 ? ***_track.back() : **_owner->_root );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
const_qual_t* HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator->( void ) const {
	return ( _track.get_size() > 1 ? &***_track.back() : &**_owner->_root );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
const_qual_t* HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator->( void ) {
	return ( _track.get_size() > 1 ? &***_track.back() : &**_owner->_root );
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
bool HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator == ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	bool lEmpty( _track.is_empty() );
	bool rEmpty( it._track.is_empty() );
	return ( ( lEmpty && rEmpty ) || ( ! ( lEmpty || rEmpty ) && ( _track.back() == it._track.back() ) ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
bool HTree<value_t, sequence_t>::HIterator<const_qual_t>::operator != ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( _owner == it._owner );
	bool lEmpty( _track.is_empty() );
	bool rEmpty( it._track.is_empty() );
	return ( ( lEmpty && ! rEmpty ) || ( ! lEmpty && rEmpty ) || ( ! ( lEmpty || rEmpty ) && ( _track.back() != it._track.back() ) ) );
	M_EPILOG
}

template<typename value_t, template <typename> class sequence_t>
template<typename const_qual_t>
void HTree<value_t, sequence_t>::HIterator<const_qual_t>::descent( const_qual_node_t* n_, list_it_t const& start_ ) {
	M_PROLOG
	_track.push_back( start_ );
	while ( n_ && n_->has_childs() ) {
		_track.push_back( n_->_branch.begin() );
		n_ = n_->_branch.head();
	}
	return;
	M_EPILOG
}

}

template<typename type_t, template <typename> class sequence_t>
inline void swap( yaal::hcore::HTree<type_t, sequence_t>& a, yaal::hcore::HTree<type_t, sequence_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HTREE_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hhashmultimap.hxx
 * \brief Declaration and implementation of HHashMultiMap<> class template.
 */

#ifndef YAAL_HCORE_HHASHMULTIMAP_HXX_INCLUDED
#define YAAL_HCORE_HHASHMULTIMAP_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hmulticontainer.hxx"

namespace yaal {

namespace hcore {

/*! \brief HHashContainer util, a helper for HHashMultiMap<> instantiations.
 */
template<typename key_t, typename value_t>
struct hashmultimap_helper {
	typedef key_t key_type;
	inline static key_type const& key( HPair<key_t, value_t> const& key_ ) {
		return ( key_.first );
	}
};

/*! \brief Hash map with relation one-to-many on keys to values.
 *
 * HHashMultiMap<> is a template representing hash map
 * data structure that holds pairs of keys and lists of values.
 *
 * \tparam key_type_t - type of key held in map.
 * \tparam value_type_t - type of value held in map.
 * \tparam hasher_t - hash calculator.
 */
template<typename key_type_t, typename value_type_t,
	typename hasher_t = hash<key_type_t>,
	typename equal_key_t = yaal::equal_to<key_type_t>,
	typename allocator_t = allocator::system<HPair<key_type_t, value_type_t>>,
	template<typename, typename>class storage_policy_t = HMultiContainerStorage::HTransparent
>
class HHashMultiMap final {
public:
	typedef HHashMultiMap<key_type_t, value_type_t, hasher_t, equal_key_t, allocator_t, storage_policy_t> this_type;
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HPair<key_type const, data_type> value_type;
	typedef storage_policy_t<key_type const, data_type> storage_t;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	typedef HList<typename storage_t::stored_type> value_list_t;
	typedef HPointer<value_list_t> value_list_ptr_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	typedef HHashMap<key_type, value_list_ptr_t, hasher_type, equal_key_type, allocator_t> hashmultimap_engine_t;
	hashmultimap_engine_t _engine;
public:
	typedef typename hashmultimap_engine_t::size_type size_type;
	typedef typename hashmultimap_engine_t::allocator_type allocator_type;
	typedef typename hashmultimap_engine_t::node_size node_size;
	typedef typename hashmultimap_engine_t::node_type node_type;
	HHashMultiMap( void )
		: _engine() {
		return;
	}
	HHashMultiMap( hasher_type const& hasher_, equal_key_type const& equals_ = equal_key_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, equals_, allocator_ ) {
		return;
	}
	explicit HHashMultiMap( allocator_type const& allocator_ )
		: _engine( allocator_ ) {
		return;
	}
	explicit HHashMultiMap( size_type size_ )
		: _engine( size_ ) {
		return;
	}
	template<typename iterator_t>
	HHashMultiMap(
		iterator_t first,
		iterator_t last,
		hasher_type const& hasher_ = hasher_type(),
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}
	HHashMultiMap( HHashMultiMap const& multimap_ )
		: _engine( multimap_._engine.hasher(), multimap_._engine.equal_key(), multimap_.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( multimap_._engine );
		return;
		M_EPILOG
	}
	HHashMultiMap& operator = ( HHashMultiMap const& multimap_ ) {
		M_PROLOG
		if ( &multimap_ != this ) {
			HHashMultiMap tmp( multimap_ );
			swap( multimap_ );
		}
		return ( *this );
		M_EPILOG
	}
	size_type size( void ) const {
		return ( get_size() );
	}
	size_type get_size( void ) const {
		M_PROLOG
		size_type sizeAcc( 0 );
		for ( typename hashmultimap_engine_t::const_iterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it->second->get_size();
		return sizeAcc;
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	bool empty( void ) const {
		return ( is_empty() );
	}
	bool is_empty( void ) const {
		return ( _engine.is_empty() );
	}
	iterator push_back( value_type const& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( storage_t::value( value_ ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator push_back( value_type&& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( yaal::move( storage_t::value( value_ ) ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator insert( value_type const& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( storage_t::value( value_ ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator insert( value_type&& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( yaal::move( storage_t::value( value_ ) ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	template<typename iterator_t>
	void insert( iterator_t first, iterator_t last ) {
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	template<typename iter_t>
	void push_back( iter_t i, iter_t endIt ) {
		M_PROLOG
		for ( ; i != endIt; ++ i )
			push_back( *i );
		return;
		M_EPILOG
	}
	iterator push_front( value_type const& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_front( storage_t::value( value_ ) );
		typename value_list_t::iterator minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator push_front( value_type&& value_ ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_front( yaal::move( storage_t::value( value_ ) ) );
		typename value_list_t::iterator minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	template<typename iter_t>
	void push_front( iter_t i, iter_t endIt ) {
		M_PROLOG
		for ( ; i != endIt; ++ i )
			push_front( *i );
		return;
		M_EPILOG
	}
	size_type count( key_type const& key ) {
		M_PROLOG
		iterator it = find( key );
		size_type num( 0 );
		if ( it != end() )
			num = it._major->second->get_size();
		return num;
		M_EPILOG
	}
	size_type erase( key_type const& key ) {
		M_PROLOG
		iterator it = find( key );
		size_type erased( 0 );
		if ( it != end() ) {
			erased = it._major->second->get_size();
			_engine.erase( it._major );
		}
		return erased;
		M_EPILOG
	}
	iterator erase( iterator& it ) {
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		value_list_ptr_t list = it._major->second;
		list->erase( it._minor );
		if ( ! list->size() )
			_engine.erase( it._major );
		return newIt;
		M_EPILOG
	}
	iterator erase( iterator first_, iterator const& last_ ) {
		M_PROLOG
		while ( first_ != last_ ) {
			first_ = erase( first_ );
		}
		return first_;
		M_EPILOG
	}
	const_iterator find( key_type const& key ) const {
		M_PROLOG
		typename hashmultimap_engine_t::const_iterator major = _engine.find( key );
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator find( key_type const& key ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = _engine.find( key );
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator lower_bound( key_type const& key_ ) const {
		M_PROLOG
		return ( find( key_ ) );
		M_EPILOG
	}
	iterator lower_bound( key_type const& key_ ) {
		M_PROLOG
		return ( find( key_ ) );
		M_EPILOG
	}
	const_iterator upper_bound( key_type const& key ) const {
		M_PROLOG
		typename hashmultimap_engine_t::const_iterator major = _engine.find( key );
		if ( major != _engine.end() )
			++ major;
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator upper_bound( key_type const& key ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = _engine.find( key );
		if ( major != _engine.end() )
			++ major;
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator begin( void ) const {
		M_PROLOG
		typename hashmultimap_engine_t::const_iterator major = _engine.begin();
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator cbegin( void ) const {
		return ( begin() );
	}
	iterator begin( void ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = _engine.begin();
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, _engine.end(), typename value_list_t::const_iterator() ) );
	}
	const_iterator cend( void ) const {
		return ( end() );
	}
	iterator end( void ) {
		return ( iterator( this, _engine.end(), typename value_list_t::iterator() ) );
	}
	const_reverse_iterator rbegin( void ) const {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	const_reverse_iterator crbegin( void ) const {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	reverse_iterator rbegin( void ) {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	const_reverse_iterator rend( void ) const {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	const_reverse_iterator crend( void ) const {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	reverse_iterator rend( void ) {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	void clear( void ) {
		_engine.clear();
	}
	size_type count( key_type const& key ) const {
		M_PROLOG
		typename hashmultimap_engine_t::const_iterator major = _engine.find( key );
		size_type cnt = 0;
		if ( major != _engine.end() )
			cnt = major->second->size();
		return cnt;
		M_EPILOG
	}
	void swap( HHashMultiMap& multimap_ ) {
		if ( &multimap_ != this ) {
			using yaal::swap;
			swap( _engine, multimap_._engine );
		}
		return;
	}
	bool operator == ( HHashMultiMap const& map_ ) const {
		M_PROLOG
		if ( &map_ == this ) {
			return ( true );
		}
		if ( _engine.get_size() != map_._engine.get_size() ) {
			return ( false );
		}
		for ( typename hashmultimap_engine_t::value_type const& e : _engine ) {
			typename hashmultimap_engine_t::const_iterator it( map_._engine.find( e.first ) );
			if ( it == map_._engine.end() ) {
				return ( false );
			}
			if ( ! ( *(e.second) == *(it->second) ) ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}
	bool operator != ( HHashMultiMap const& map_ ) const {
		M_PROLOG
		return ( ! operator == ( map_ ) );
		M_EPILOG
	}
private:
	typename hashmultimap_engine_t::iterator ensure_key( key_type const& key ) {
		M_PROLOG
		typename hashmultimap_engine_t::iterator major = _engine.find( key );
		if ( major == _engine.end() ) {
			value_list_ptr_t list = value_list_ptr_t( make_pointer<value_list_t>() );
			major = _engine.insert( make_pair( key, list ) ).first;
		}
		return major;
		M_EPILOG
	}
};

/*! \brief Forward iterator for HHashMultiMap<>.
 */
template<
	typename key_type_t,
	typename value_type_t,
	typename hasher_t,
	typename equal_key_t,
	typename allocator_t,
	template<typename, typename> class storage_policy_t
>
template<typename const_qual_t>
class HHashMultiMap<key_type_t, value_type_t, hasher_t, equal_key_t, allocator_t, storage_policy_t>::HIterator
	: public iterator_interface<
		typename HHashMultiMap<
			key_type_t,
			value_type_t,
			hasher_t,
			equal_key_t,
			allocator_t,
			storage_policy_t
		>::storage_t::template const_aware_type<const_qual_t>::accessor_t,
		iterator_category::forward
	> {
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HHashMultiMap<key_type, data_type, hasher_t, equal_key_t, allocator_t, storage_policy_t> hash_multi_map_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename hash_multi_map_t::hashmultimap_engine_t::const_iterator,
					typename hash_multi_map_t::hashmultimap_engine_t::iterator>::type key_iterator_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename value_list_t::const_iterator,
					typename value_list_t::iterator>::type value_iterator_t;
	hash_multi_map_t const* _owner;
	key_iterator_t _major;
	value_iterator_t _minor;
public:
	typedef iterator_interface<typename hash_multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t, iterator_category::forward> base_type;
	HIterator( void ) : base_type(), _owner( nullptr ), _major(), _minor() {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : base_type(), _owner( it_._owner ), _major( it_._major ), _minor( it_._minor ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_major = it_._major;
			_minor = it_._minor;
		}
		return ( *this );
	}
	HIterator& operator ++ ( void ) {
		M_PROLOG
		++ _minor;
		if ( _minor == _major->second->end() ) {
			++ _major;
			if ( _major != _owner->_engine.end() ) {
				_minor = _major->second->begin();
			}
		}
		return ( *this );
		M_EPILOG
	}
	HIterator operator ++ ( int ) {
		HIterator it( _owner, _major, _minor );
		operator++();
		return it;
	}
	HIterator& operator -- ( void ) {
		M_PROLOG
		if ( _minor != value_iterator_t() ) {
			-- _minor;
		} else if ( _major != prev( _owner->_engine.begin() ) ) {
			_minor = prev( _major->second->end() );
		}
		if ( _major == _owner->_engine.end() ) {
			-- _major;
			_minor = prev( _major->second->end() );
		}
		if ( _minor == prev( _major->second->begin() ) ) {
			-- _major;
			if ( _major != prev( _owner->_engine.begin() ) ) {
				_minor = prev( _major->second->end() );
			} else {
				_minor = value_iterator_t();
			}
		}
		return ( *this );
		M_EPILOG
	}
	HIterator operator -- ( int ) {
		HIterator it( _owner, _major, _minor );
		operator--();
		return it;
	}
	typename hash_multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void ) const {
		return ( hash_multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );
	}
	typename hash_multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void ) const {
		return ( &hash_multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );
	}
	bool operator == ( HIterator const& it ) const {
		return ( ( _major == it._major ) && ( _minor == it._minor ) );
	}
	bool operator != ( HIterator const& it ) const {
		return ( ! ( ( _major == it._major ) && ( _minor == it._minor ) ) );
	}
private:
	friend class HHashMultiMap<key_type, data_type, hasher_type, equal_key_type, allocator_t, storage_policy_t>;
	explicit HIterator( hash_multi_map_t const* owner_, key_iterator_t const& major, value_iterator_t const& minor )
		: base_type()
		, _owner( owner_ )
		, _major( major )
		, _minor( minor ) {
		return;
	}
};

}

template<
	typename key_type,
	typename data_type,
	typename hasher_t,
	typename equal_key_t,
	typename allocator_t,
	template<typename, typename>class storage_policy_t
>
inline void swap(
		yaal::hcore::HHashMultiMap<key_type, data_type, hasher_t, equal_key_t, allocator_t, storage_policy_t>& a,
		yaal::hcore::HHashMultiMap<key_type, data_type, hasher_t, equal_key_t, allocator_t, storage_policy_t>& b ) {
	a.swap( b );
}

}

#endif /* #ifndef YAAL_HCORE_HHASHMULTIMAP_HXX_INCLUDED */


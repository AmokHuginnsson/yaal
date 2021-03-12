/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hmultimap.hxx
 * \brief Declaration and implementation of HMultiMap class template.
 */

#ifndef YAAL_HCORE_HMULTIMAP_HXX_INCLUDED
#define YAAL_HCORE_HMULTIMAP_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hmulticontainer.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

/*! \brief Binary tree based map with relation one-to-many on keys to values.
 *
 * HMultiMap<> is a template representing self balancing binary search tree
 * data structure that holds pairs of keys and lists of values.
 *
 * \tparam key_type_t - type of key held in map.
 * \tparam value_type_t - type of value held in map.
 * \tparam compare_t - key ordering definition.
 */
template<typename key_type_t, typename value_type_t,
	typename compare_t = less<key_type_t>,
	typename allocator_t = allocator::system<HPair<key_type_t, value_type_t> >,
	template<typename, typename>class storage_policy_t = HMultiContainerStorage::HTransparent>
class HMultiMap final {
public:
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef compare_t compare_type;
	typedef storage_policy_t<key_type const, data_type> storage_t;
	typedef HList<typename storage_t::stored_type> value_list_t;
	typedef HPointer<value_list_t> value_list_ptr_t;
	typedef HPair<key_type const, data_type> value_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	typedef HMap<key_type, value_list_ptr_t, compare_type, allocator_t> multimap_engine_t;
	multimap_engine_t _engine;
public:
	typedef typename multimap_engine_t::size_type size_type;
	typedef typename multimap_engine_t::allocator_type allocator_type;
	typedef typename multimap_engine_t::node_size node_size;
	typedef typename multimap_engine_t::node_type node_type;
	typedef HMultiMap<key_type_t, value_type_t, compare_t, allocator_t, storage_policy_t> this_type;
	HMultiMap( void )
		: _engine() {
		return;
	}
	explicit HMultiMap( compare_type const& compare_, allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		return;
	}
	explicit HMultiMap( allocator_type const& allocator_ )
		: _engine( allocator_ ) {
		return;
	}
	template<typename iterator_t>
	HMultiMap( iterator_t first, iterator_t last, compare_type const& compare_ = compare_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}
	HMultiMap( HMultiMap const& multimap_ )
		: _engine( multimap_._engine.compare(), multimap_._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( multimap_._engine );
		return;
		M_EPILOG
	}
	HMultiMap& operator = ( HMultiMap const& multimap_ ) {
		M_PROLOG
		if ( &multimap_ != this ) {
			HMultiMap tmp( multimap_ );
			swap( multimap_ );
		}
		return ( *this );
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	size_type size( void ) const {
		return ( get_size() );
	}
	size_type get_size( void ) const {
		M_PROLOG
		size_type sizeAcc( 0 );
		for ( typename multimap_engine_t::const_iterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it->second->get_size();
		return sizeAcc;
		M_EPILOG
	}
	bool empty( void ) const {
		return ( is_empty() );
	}
	bool is_empty( void ) const {
		return ( _engine.is_empty() );
	}
	iterator push_back( value_type const& value_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( storage_t::value( value_ ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator push_back( value_type&& value_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( yaal::move( storage_t::value( value_ ) ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator insert( value_type const& value_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_back( storage_t::value( value_ ) );
		return ( iterator( this, major, prev( major->second->end() ) ) );
		M_EPILOG
	}
	iterator insert( value_type&& value_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
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
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
		major->second->push_front( storage_t::value( value_ ) );
		typename value_list_t::iterator minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator push_front( value_type&& value_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( value_.first );
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
	/*! \brief Erase all elements of given \e key.
	 *
	 * \param key_ - erase elements with this key.
	 * \return number of elements actually erased.
	 */
	size_type erase( key_type const& key_ ) {
		M_PROLOG
		iterator it = find( key_ );
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
		typename multimap_engine_t::const_iterator major = _engine.find( key );
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator find( key_type const& key ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.find( key );
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator lower_bound( key_type const& key_ ) const {
		M_PROLOG
		typename multimap_engine_t::const_iterator major =  _engine.lower_bound( key_ );
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG }
	iterator lower_bound( key_type const& key_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.lower_bound( key_ );
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator upper_bound( key_type const& key_ ) const {
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.upper_bound( key_ );
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
	}
	iterator upper_bound( key_type const& key_ ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.upper_bound( key_ );
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
	}
	const_iterator begin( void ) const {
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.begin();
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
		typename multimap_engine_t::iterator major = _engine.begin();
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
		return ( begin() );
	}
	const_reverse_iterator crend( void ) const {
		return ( begin() );
	}
	reverse_iterator rend( void ) {
		return ( begin() );
	}
	void clear( void ) {
		_engine.clear();
	}
	size_type count( key_type const& key ) const {
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.find( key );
		size_type cnt = 0;
		if ( major != _engine.end() )
			cnt = major->second->size();
		return cnt;
		M_EPILOG
	}
	void swap( HMultiMap& multimap_ ) {
		if ( &multimap_ != this ) {
			using yaal::swap;
			swap( _engine, multimap_._engine );
		}
		return;
	}
	bool operator == ( HMultiMap const& map_ ) const {
		M_PROLOG
		return ( ( &map_ == this ) || safe_equal( begin(), end(), map_.begin(), map_.end() ) );
		M_EPILOG
	}
	bool operator < ( HMultiMap const& map_ ) const {
		M_PROLOG
		return ( ( &map_ != this ) && lexicographical_compare( begin(), end(), map_.begin(), map_.end() ) );
		M_EPILOG
	}
private:
	typename multimap_engine_t::iterator ensure_key( key_type const& key ) {
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.find( key );
		if ( major == _engine.end() ) {
			value_list_ptr_t list = value_list_ptr_t( make_pointer<value_list_t>() );
			major = _engine.insert( make_pair( key, list ) ).first;
		}
		return major;
		M_EPILOG
	}
};

/*! \brief Forward iterator for HMultiMap<>.
 */
template<typename key_type_t, typename value_type_t, typename compare_t, typename allocator_t, template<typename, typename>class storage_policy_t>
template<typename const_qual_t>
class HMultiMap<key_type_t, value_type_t, compare_t, allocator_t, storage_policy_t>::HIterator : public iterator_interface<typename HMultiMap<key_type, data_type, compare_type, allocator_t, storage_policy_t>::storage_t::template const_aware_type<const_qual_t>::accessor_t, iterator_category::forward> {
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HMultiMap<key_type, data_type, compare_type, allocator_t, storage_policy_t> multi_map_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename multi_map_t::multimap_engine_t::const_iterator,
					typename multi_map_t::multimap_engine_t::iterator>::type key_iterator_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename value_list_t::const_iterator,
					typename value_list_t::iterator>::type value_iterator_t;
	multi_map_t const* _owner;
	key_iterator_t _major;
	value_iterator_t _minor;
public:
	typedef iterator_interface<typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t, iterator_category::forward> base_type;
	HIterator( void ) : base_type(), _owner( nullptr ), _major(), _minor() {}
	HIterator( HIterator const& it_ ) : base_type(), _owner( it_._owner ), _major( it_._major ), _minor( it_._minor ) {}
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
	HIterator const operator ++ ( int ) {
		HIterator it( *this );
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
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		operator--();
		return it;
	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void ) const {
		return ( multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );
	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void ) const {
		return ( &multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );
	}
	bool operator == ( HIterator const& it ) const {
		return ( ( _major == it._major ) && ( _minor == it._minor ) );
	}
	bool operator != ( HIterator const& it ) const {
		return ( ! ( ( _major == it._major ) && ( _minor == it._minor ) ) );
	}
private:
	friend class HMultiMap<key_type, data_type, compare_type, allocator_t, storage_policy_t>;
	explicit HIterator( multi_map_t const* owner_, key_iterator_t const& major, value_iterator_t const& minor )
		: base_type()
		, _owner( owner_ )
		, _major( major )
		, _minor( minor ) {
	}
};

}

template<typename key_type_t, typename value_type_t, typename compare_t, typename allocator_t, template<typename, typename>class storage_policy_t>
inline void swap( yaal::hcore::HMultiMap<key_type_t, value_type_t, compare_t, allocator_t, storage_policy_t>& a,
		yaal::hcore::HMultiMap<key_type_t, value_type_t, compare_t, allocator_t, storage_policy_t>& b ) {
	a.swap( b );
}

}

#endif /* #ifndef YAAL_HCORE_HMULTIMAP_HXX_INCLUDED */


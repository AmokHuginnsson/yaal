/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hhashmap.hxx
 * \brief Declaration and implementation of HHashMap<> class template.
 */

#ifndef YAAL_HCORE_HHASHMAP_HXX_INCLUDED
#define YAAL_HCORE_HHASHMAP_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHHashMap_[];

/*! \brief HHashContainer util, a helper for HHashMap<> instantiations.
 */
template<typename key_t, typename value_t>
struct hashmap_helper {
	typedef key_t key_type;
	inline static key_type const& key( HPair<key_t const, value_t> const& key_ ) {
		return ( key_.first );
	}
};

/*! \brief Hash map container implementation.
 */
template<
	typename key_t,
	typename data_t,
	typename hasher_t = hash<key_t>,
	typename equal_key_t = yaal::equal_to<key_t>,
	typename allocator_t = allocator::system<HPair<key_t, data_t>>
>
class HHashMap final {
public:
	typedef key_t key_type;
	typedef data_t data_type;
/* cppcheck-suppress variableHidingTypedef */
	typedef HPair<key_t const, data_t> value_type;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	typedef HPair<iterator, bool> insert_result;
	/*! \brief Error codes for HHashMap<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HHashMap<> operations.
		 */
		typedef enum {
			OK = 0,     /*!< No error. */
			INVALID_KEY /*!< Dereferencing non-existing key. */
		} error_t;
	};
private:
	typedef HHashContainer<value_type, hasher_type, equal_key_type, hashmap_helper<key_type, data_type>, allocator_t> engine_t;
	engine_t _engine;
public:
	typedef typename engine_t::size_type size_type;
	typedef typename engine_t::allocator_type allocator_type;
	typedef typename engine_t::node_size node_size;
	typedef typename engine_t::node_type node_type;
	typedef HHashMap<key_type, data_type, hasher_type, equal_key_type, allocator_t> this_type;
	HHashMap( void )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		return;
	}
	explicit HHashMap( hasher_type const& hasher_, equal_key_type const& equals_ = equal_key_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, equals_, allocator_ ) {
		return;
	}
	explicit HHashMap( allocator_type const& allocator_ )
		: _engine( hasher_type(), equal_key_type(), allocator_ ) {
		return;
	}
	/*! \brief Lower bound of size of map's table */
	explicit HHashMap( size_type size_ )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	HHashMap(
		size_type size_,
		hasher_type const& hasher_,
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashMap(
		iterator_t first,
		iterator_t last,
		hasher_type const& hasher_ = hasher_type(),
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashMap(
		iterator_t first,
		iterator_t last,
		size_type size_,
		hasher_type const& hasher_ = hasher_type(),
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		resize( size_ );
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	HHashMap( std::initializer_list<value_type> constants_ )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( static_cast<int>( constants_.size() ) );
		for ( value_type const& v : constants_ ) {
			insert( v );
		}
		return;
		M_EPILOG
	}
	HHashMap( HHashMap const& map_ )
		: _engine( map_._engine.hasher(), map_._engine.equal_key(), map_._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( map_._engine );
		return;
		M_EPILOG
	}
	~HHashMap( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HHashMap& operator = ( HHashMap const& map_ ) {
		M_PROLOG
		if ( &map_ != this ) {
			HHashMap tmp( map_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	data_type& operator [] ( key_type const& key_ ) {
		M_PROLOG
		return ( insert( make_pair( key_, data_type() ) ).first->second );
		M_EPILOG
	}
	data_type const& operator[] ( key_type const& key ) const {
		M_PROLOG
		return ( at( key ) );
		M_EPILOG
	}
	data_type const& at( key_type const& key ) const {
		M_PROLOG
		typename engine_t::HIterator it( _engine.find( key ) );
		if ( ! ( it != _engine.end() ) )
			throw HInvalidKeyException( _errMsgHHashMap_[ERROR::INVALID_KEY] );
		return ( it.get().second );
		M_EPILOG
	}
	data_type& at( key_type const& key ) {
		M_PROLOG
		typename engine_t::HIterator it( _engine.find( key ) );
		if ( ! ( it != _engine.end() ) )
			throw HInvalidKeyException( _errMsgHHashMap_[ERROR::INVALID_KEY] );
		return ( it.get().second );
		M_EPILOG
	}
	const_iterator begin( void ) const {
		M_PROLOG
		return ( const_iterator( _engine.begin() ) );
		M_EPILOG
	}
	const_iterator cbegin( void ) const {
		M_PROLOG
		return ( const_iterator( _engine.begin() ) );
		M_EPILOG
	}
	iterator begin( void ) {
		M_PROLOG
		return ( iterator( _engine.begin() ) );
		M_EPILOG
	}
	const_iterator end( void ) const {
		M_PROLOG
		return ( const_iterator( _engine.end() ) );
		M_EPILOG
	}
	const_iterator cend( void ) const {
		M_PROLOG
		return ( const_iterator( _engine.end() ) );
		M_EPILOG
	}
	iterator end( void ) {
		M_PROLOG
		return ( iterator( _engine.end() ) );
		M_EPILOG
	}
	const_reverse_iterator rbegin( void ) const {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	const_reverse_iterator rend( void ) const {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	const_reverse_iterator crbegin( void ) const {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	const_reverse_iterator crend( void ) const {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	reverse_iterator rbegin( void ) {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	reverse_iterator rend( void ) {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	const_iterator find( key_t const& key_ ) const {
		M_PROLOG
		return ( const_iterator( _engine.find( key_ ) ) );
		M_EPILOG
	}
	iterator find( key_t const& key_ ) {
		M_PROLOG
		return ( iterator( _engine.find( key_ ) ) );
		M_EPILOG
	}
	insert_result insert( value_type const& val_ ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> it( _engine.insert( val_ ) );
		return ( make_pair( iterator( it.first ), it.second ) );
		M_EPILOG
	}
	insert_result insert( value_type&& val_ ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> it( _engine.insert( yaal::move( val_ ) ) );
		return ( make_pair( iterator( it.first ), it.second ) );
		M_EPILOG
	}
	void resize( size_type size_ ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
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
	iterator erase( iterator it ) {
		M_PROLOG
		iterator next( it );
		++ next;
		_engine.erase( it._engine );
		return next;
		M_EPILOG
	}
	/*! \brief Remove given key from map.
	 *
	 * \param key_ - key to be removed.
	 * \return Number of erased elements.
	 */
	size_type erase( key_t const& key_ ) {
		M_PROLOG
		iterator it( find( key_ ) );
		bool erased( it != end() );
		if ( erased )
			erase( it );
		return ( erased ? 1 : 0 );
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
	size_type count( key_t const& key_ ) const {
		M_PROLOG
		return ( find( key_ ) != end() ? 1 : 0 );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		_engine.clear();
		return;
		M_EPILOG
	}
	size_type get_size( void ) const {
		M_PROLOG
		return ( _engine.get_size() );
		M_EPILOG
	}
	size_type size( void ) const {
		M_PROLOG
		return ( _engine.get_size() );
		M_EPILOG
	}
	bool is_empty( void ) const {
		M_PROLOG
		return ( _engine.is_empty() );
		M_EPILOG
	}
	bool empty( void ) const {
		M_PROLOG
		return ( _engine.is_empty() );
		M_EPILOG
	}
	void swap( HHashMap& map_ ) {
		if ( &map_ != this ) {
			using yaal::swap;
			swap( _engine, map_._engine );
		}
		return;
	}
	bool operator == ( HHashMap const& map_ ) const {
		M_PROLOG
		if ( &map_ == this ) {
			return ( true );
		}
		if ( _engine.get_size() != map_._engine.get_size() ) {
			return ( false );
		}
		for ( value_type const& e : *this ) {
			const_iterator it( map_.find( e.first ) );
			if ( it == map_.end() ) {
				return ( false );
			}
			if ( ! ( it->second == e.second ) ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}
	bool operator != ( HHashMap const& map_ ) const {
		M_PROLOG
		return ( ! operator == ( map_ ) );
		M_EPILOG
	}
private:
};


template<typename key_type_t, typename data_type_t, typename hasher_t, typename equal_key_t, typename allocator_t>
template<typename const_qual_t>
class HHashMap<key_type_t, data_type_t, hasher_t, equal_key_t, allocator_t>::HIterator
	: public iterator_interface<const_qual_t, iterator_category::forward> {
	typedef key_type_t key_type;
	typedef data_type_t data_type;
	typedef HHashMap<key_type, data_type, hasher_t, equal_key_t, allocator_t> hashmap_t;
	typename hashmap_t::engine_t::HIterator _engine;
public:
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	HIterator( void )
		: base_type(), _engine() {
		return;
	}
	HIterator( HIterator const& ) = default;
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ )
		: base_type(), _engine( it_._engine ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this )
			_engine = it_._engine;
		return ( *this );
	}
	HIterator& operator ++ ( void ) {
		++ _engine;
		return ( *this );
	}
	HIterator const operator ++ ( int ) {
		HIterator it( _engine );
		++ _engine;
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _engine;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( _engine );
		-- _engine;
		return it;
	}
	const_qual_t& operator* ( void ) const {
		return ( _engine.get() );
	}
	const_qual_t* operator-> ( void ) const {
		return ( &_engine.get() );
	}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const {
		return ( _engine == it._engine );
	}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const {
		return ( _engine != it._engine );
	}
	void const* node_id( void ) const {
		return ( _engine.node_id() );
	}
private:
	friend class HHashMap<key_type, data_type, hasher_t, equal_key_t, allocator_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( typename hashmap_t::engine_t::HIterator const& it )
		: base_type()
		, _engine( it ) {
		return;
	}
};

template<typename key_type, typename data_type, typename hasher_t, typename equal_key_t, typename allocator_t>
inline void swap(
	yaal::hcore::HHashMap<key_type, data_type, hasher_t, equal_key_t, allocator_t>& a,
	yaal::hcore::HHashMap<key_type, data_type, hasher_t, equal_key_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HHASHMAP_HXX_INCLUDED */


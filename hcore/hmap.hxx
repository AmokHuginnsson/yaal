/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hmap.hxx
 * \brief Declaration and implementation of HMap<> class template.
 */

#ifndef YAAL_HCORE_HMAP_HXX_INCLUDED
#define YAAL_HCORE_HMAP_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/hsbbstree.hxx"
#include "hcore/hlookup.hxx"
#include "hcore/hpair.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHMap_[];

/*! \brief HSBBSTree util, a helper for HMap<> instantiations.
 */
template<typename key_t, typename value_t>
struct map_helper {
	typedef key_t key_type;
	inline static key_type const& key( HPair<key_t const, value_t> const& key_ ) {
		return ( key_.first );
	}
};

/*! \brief Binary tree based map.
 *
 * HMap<> is a template representing self balancing binary search tree
 * data structure that holds pairs of keys and values.
 *
 * \tparam key_type_t - type of key held in map.
 * \tparam value_type_t - type of value held in map.
 * \tparam compare_t - key ordering definition.
 */
template<
	typename key_type_t, typename value_type_t,
	typename compare_t = less<key_type_t>,
	typename allocator_t = allocator::system<HPair<key_type_t, value_type_t>>,
	typename engine_t = HSBBSTree<HPair<key_type_t const, value_type_t>, compare_t, map_helper<key_type_t, value_type_t>, allocator_t>
>
class HMap {
public:
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef compare_t compare_type;
	typedef HPair<key_type const, data_type> value_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	typedef HPair<iterator, bool> insert_result;
	typedef engine_t engine_type;
	typedef typename engine_type::size_type size_type;
	typedef typename engine_type::allocator_type allocator_type;
	typedef typename engine_type::node_size node_size;
	typedef typename engine_type::node_type node_type;
	typedef HMap<key_type_t, value_type_t, compare_t, allocator_t, engine_t> this_type;
	/*! \brief Error codes for HMap<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HMap<> operations.
		 */
		typedef enum {
			OK = 0,     /*!< No error. */
			INVALID_KEY /*!< Dereferencing non-existing key. */
		} error_t;
	};
private:
	engine_type _engine;
public:
	HMap( void )
		: _engine( compare_type(), allocator_type() )
		{}
	explicit HMap( compare_type const& compare_, allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ )
		{}
	explicit HMap( allocator_type const& allocator_ )
		: _engine( compare_type(), allocator_ )
		{}
	template<typename iterator_t>
	HMap( iterator_t first, iterator_t last, compare_type const& compare_ = compare_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}
	HMap( HMap const& source )
		: _engine( source._engine.compare(), source._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
	}
	HMap( std::initializer_list<value_type> constants_ )
		: _engine( compare_type(), allocator_type() ) {
		M_PROLOG
		for ( value_type const& v : constants_ ) {
			insert( v );
		}
		return;
		M_EPILOG
	}
	HMap& operator = ( HMap const& map_ ) {
		M_PROLOG
		if ( &map_ != this ) {
			HMap tmp( map_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	size_type size( void ) const
		{ return ( get_size() ); }
	size_type get_size( void ) const
		{ return ( _engine.get_size() );	}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	insert_result insert( value_type const& e ) {
		HPair<typename engine_type::iterator_type, bool> p = _engine.insert( e );
		return ( make_pair( iterator( p.first ), p.second ) );
	}
	insert_result insert( value_type&& e ) {
		HPair<typename engine_type::iterator_type, bool> p = _engine.insert( yaal::move( e ) );
		return ( make_pair( iterator( p.first ), p.second ) );
	}
	void insert( iterator it, value_type const& elem, bool forceHint = false ) {
		M_PROLOG
		_engine.insert( it._engine, elem, forceHint );
		return;
		M_EPILOG
	}
	void insert( iterator it, value_type&& elem, bool forceHint = false ) {
		M_PROLOG
		_engine.insert( it._engine, yaal::move( elem ), forceHint );
		return;
		M_EPILOG
	}
	template<typename iter_t>
	void insert( iter_t i, iter_t endIt ) {
		M_PROLOG
		for ( ; i != endIt; ++ i )
			insert( *i );
		return;
		M_EPILOG
	}
	size_type erase( key_type const& key ) {
		M_PROLOG
		iterator it = find( key );
		size_type erased( 0 );
		if ( it != end() ) {
			erased = 1;
			_engine.remove( it._engine );
		}
		return ( erased );
		M_EPILOG
	}
	iterator erase( iterator const& it ) {
		M_PROLOG
		return ( iterator( _engine.remove( it._engine ) ) );
		M_EPILOG
	}
	iterator erase( iterator first_, iterator const& last_ ) {
		M_PROLOG
		iterator it( first_ );
		while ( it != last_ ) {
			++ it;
			first_ = erase( first_ );
		}
		return ( first_ );
		M_EPILOG
	}
	iterator find( key_type const& e )
		{ return ( iterator( _engine.find( e ) ) ); }
	const_iterator find( key_type const& e ) const
		{ return ( const_iterator( _engine.find( e ) ) ); }
	iterator lower_bound( key_type const& e )
		{ return ( iterator( _engine.lower_bound( e ) ) ); }
	const_iterator lower_bound( key_type const& e ) const
		{ return ( const_iterator( _engine.lower_bound( e ) ) ); }
	iterator upper_bound( key_type const& e )
		{ return ( iterator( _engine.upper_bound( e ) ) ); }
	const_iterator upper_bound( key_type const& e ) const
		{ return ( const_iterator( _engine.upper_bound( e ) ) ); }
	iterator begin( void )
		{ return ( iterator( _engine.begin() ) ); }
	const_iterator begin( void ) const
		{ return ( const_iterator( _engine.begin() ) ); }
	const_iterator cbegin( void ) const
		{ return ( const_iterator( _engine.begin() ) ); }
	iterator end( void )
		{ return ( iterator( _engine.end() ) ); }
	const_iterator end( void ) const
		{ return ( const_iterator( _engine.end() ) ); }
	const_iterator cend( void ) const
		{ return ( const_iterator( _engine.end() ) ); }
	reverse_iterator rbegin( void )
		{ return ( end() ); }
	const_reverse_iterator rbegin( void ) const
		{ return ( end() ); }
	const_reverse_iterator crbegin( void ) const
		{ return ( end() ); }
	reverse_iterator rend( void )
		{ return ( begin() ); }
	const_reverse_iterator rend( void ) const
		{ return ( begin() ); }
	const_reverse_iterator crend( void ) const
		{ return ( begin() ); }
	data_type& operator[] ( key_type const& key ) {
		M_PROLOG
		return ( insert( value_type( key, data_type() ) ).first->second );
		M_EPILOG
	}
	data_type const& operator[] ( key_type const& key ) const {
		M_PROLOG
		return ( at( key ) );
		M_EPILOG
	}
	data_type const& at( key_type const& key ) const {
		M_PROLOG
		typename engine_type::iterator_type it( _engine.find( key ) );
		if ( ! ( it != _engine.end() ) )
			throw HInvalidKeyException( _errMsgHMap_[ERROR::INVALID_KEY] );
		return ( it->second );
		M_EPILOG
	}
	data_type& at( key_type const& key ) {
		M_PROLOG
		typename engine_type::iterator_type it( _engine.find( key ) );
		if ( ! ( it != _engine.end() ) )
			throw HInvalidKeyException( _errMsgHMap_[ERROR::INVALID_KEY] );
		return ( it->second );
		M_EPILOG
	}
	size_type count( key_type const& key_ ) const
		{ M_PROLOG return ( find( key_ ) != end() ? 1 : 0 ); M_EPILOG }
	void clear( void )
		{ _engine.clear(); }
	void swap( HMap& other ) {
		if ( &other != this ) {
			using yaal::swap;
			_engine.swap( other._engine );
		}
	}
	bool operator == ( HMap const& map_ ) const
		{ M_PROLOG return ( ( &map_ == this ) || safe_equal( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
	bool operator < ( HMap const& map_ ) const
		{ M_PROLOG return ( ( &map_ != this ) && lexicographical_compare( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
};

/*! \brief Iterator for HMap<> data structure.
 */
template<typename key_type_t, typename value_type_t, typename compare_t, typename allocator_t, typename engine_t>
template<typename const_qual_t>
class HMap<key_type_t, value_type_t, compare_t, allocator_t, engine_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::forward> {
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HMap<key_type, data_type, compare_t, allocator_t, engine_t> map_t;
	typedef typename map_t::engine_type engine_type;
	typename engine_type::iterator_type _engine;
public:
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	HIterator( void )
		: base_type(), _engine() {}
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
		HIterator it( *this );
		++ _engine;
		return ( it );
	}
	HIterator& operator -- ( void ) {
		-- _engine;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		-- _engine;
		return ( it );
	}
	const_qual_t& operator* ( void ) const {
		return ( *_engine );
	}
	const_qual_t* operator-> ( void ) const {
		return ( &*_engine );
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
	friend class HMap<key_type, data_type, compare_t, allocator_t, engine_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( typename engine_type::iterator_type const& it )
		: base_type()
		, _engine( it ) {
	}
};

template<typename key_type, typename data_type, typename compare_t, typename allocator_t, typename engine_t>
inline void swap(
	yaal::hcore::HMap<key_type, data_type, compare_t, allocator_t, engine_t>& a,
	yaal::hcore::HMap<key_type, data_type, compare_t, allocator_t, engine_t>& b
) {
	a.swap( b );
}

template<
	typename key_type_t, typename value_type_t,
	typename compare_t = less<key_type_t>
>
using HLookupMap = HMap<key_type_t, value_type_t, compare_t, trait::no_type, HLookup<HPair<key_type_t const, value_type_t>, compare_t, map_helper<key_type_t, value_type_t>, trait::no_type>>;

}

}

#endif /* #ifndef YAAL_HCORE_HMAP_HXX_INCLUDED */


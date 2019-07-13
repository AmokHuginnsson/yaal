/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hhashset.hxx
 * \brief Declaration and implementation of HHashSet<> class template.
 */

#ifndef YAAL_HCORE_HHASHSET_HXX_INCLUDED
#define YAAL_HCORE_HHASHSET_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

/*! \brief HHashContainer util, a helper for HHashSet<> instantiations.
 */
template<typename type_t>
struct hashset_helper {
	typedef type_t key_type;
	inline static key_type const& key( type_t const& key_ )
		{	return ( key_ );	}
};

/*! \brief Hash set container implementation.
 */
template<
	typename type_t,
	typename hasher_t = hash<type_t>,
	typename equal_key_t = yaal::equal_to<type_t>,
	typename allocator_t = allocator::system<type_t>
>
class HHashSet final {
public:
	typedef type_t key_type;
/* cppcheck-suppress variableHidingTypedef */
	typedef type_t value_type;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	typedef HHashContainer<value_type, hasher_type, equal_key_type, hashset_helper<value_type>, allocator_t> engine_t;
	typedef typename engine_t::size_type size_type;
	typedef typename engine_t::allocator_type allocator_type;
	typedef typename engine_t::node_size node_size;
	typedef typename engine_t::node_type node_type;
	typedef HHashSet<value_type, hasher_type, equal_key_type, allocator_t> this_type;
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		typename engine_t::HIterator _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void )
			: base_type(), _engine() {
			return;
		}
		HIterator( HIterator const& ) = default;
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
			return ( it );
		}
		HIterator& operator -- ( void ) {
			-- _engine;
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( _engine );
			-- _engine;
			return ( it );
		}
		key_type const& operator* ( void ) const {
			return ( _engine.get() );
		}
		key_type const* operator-> ( void ) const {
			return ( &_engine.get() );
		}
		bool operator == ( HIterator const& it ) const {
			return ( _engine == it._engine );
		}
		bool operator != ( HIterator const& it ) const {
			return ( _engine != it._engine );
		}
		void const* node_id( void ) const {
			return ( _engine.node_id() );
		}
	private:
		friend class HHashSet<key_type, hasher_type, equal_key_type, allocator_t>;
		explicit HIterator( typename engine_t::HIterator const& it ) : base_type(), _engine( it ) {};
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	typedef HPair<iterator, bool> insert_result;
private:
	engine_t _engine;
public:
	HHashSet( void )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		return;
	}
	explicit HHashSet( hasher_type const& hasher_, equal_key_type const& equals_ = equal_key_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, equals_, allocator_ ) {
		return;
	}
	explicit HHashSet( allocator_type const& allocator_ )
		: _engine( hasher_type(), equal_key_type(), allocator_ ) {
		return;
	}
	/*! \brief Lower bound of size of map's table */
	HHashSet( size_type size_ )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	HHashSet(
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
	HHashSet(
		iterator_t first,
		iterator_t last,
		hasher_type const& hasher_ = hasher_type(),
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		for ( ; first != last; ++ first ) {
			insert( *first );
		}
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashSet(
		iterator_t first,
		iterator_t last,
		size_type size_,
		hasher_type const& hasher_ = hasher_type(),
		equal_key_type const& equals_ = equal_key_type(),
		allocator_type const& allocator_ = allocator_type()
	) : _engine( hasher_, equals_, allocator_ ) {
		M_PROLOG
		resize( size_ );
		for ( ; first != last; ++ first ) {
			insert( *first );
		}
		return;
		M_EPILOG
	}
	HHashSet( std::initializer_list<value_type> constants_ )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( static_cast<int>( constants_.size() ) );
		for ( value_type const& v : constants_ ) {
			insert( v );
		}
		return;
		M_EPILOG
	}
	HHashSet( HHashSet const& set_ )
		: _engine( set_._engine.hasher(), set_._engine.equal_key(), set_._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( set_._engine );
		return;
		M_EPILOG
	}
	~HHashSet( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HHashSet& operator = ( HHashSet const& set_ ) {
		M_PROLOG
		if ( &set_ != this ) {
			HHashSet tmp( set_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	iterator begin( void ) const {
		M_PROLOG
		return ( iterator( _engine.begin() ) );
		M_EPILOG
	}
	iterator end( void ) const {
		M_PROLOG
		return ( iterator( _engine.end() ) );
		M_EPILOG
	}
	reverse_iterator rbegin( void ) const {
		M_PROLOG
		return ( end() );
		M_EPILOG
	}
	reverse_iterator rend( void ) const {
		M_PROLOG
		return ( begin() );
		M_EPILOG
	}
	iterator find( type_t const& key_ ) const {
		M_PROLOG
		return ( iterator( _engine.find( key_ ) ) ); M_EPILOG
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
		return ( next );
		M_EPILOG
	}
	/*! \brief Remove given key from map.
	 *
	 * \param key_ - key to be removed.
	 * \return Number of erased elements.
	 */
	size_type erase( type_t const& key_ ) {
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
		return ( first_ );
		M_EPILOG
	}
	size_type count( type_t const& key_ ) const {
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
	void swap( HHashSet& set_ ) {
		if ( &set_ != this ) {
			using yaal::swap;
			swap( _engine, set_._engine );
		}
		return;
	}
	bool operator == ( HHashSet const& set_ ) const {
		M_PROLOG
		if ( &set_ == this ) {
			return ( true );
		}
		if ( _engine.get_size() != set_._engine.get_size() ) {
			return ( false );
		}
		for ( value_type const& e : *this ) {
			if ( set_._engine.find( e ) == set_._engine.end() ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}
	bool operator != ( HHashSet const& set_ ) const {
		M_PROLOG
		return ( ! operator == ( set_ ) );
		M_EPILOG
	}
private:
};

template<typename key_type, typename hasher_t, typename equal_key_t, typename allocator_t>
inline void swap(
	yaal::hcore::HHashSet<key_type, hasher_t, equal_key_t, allocator_t>& a,
	yaal::hcore::HHashSet<key_type, hasher_t, equal_key_t, allocator_t>& b
) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HHASHSET_HXX_INCLUDED */


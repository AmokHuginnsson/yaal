/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hhashmultiset.hxx
 * \brief Declaration and implementation of HHashMultiSet<> class template.
 */

#ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED 1

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

/*! \brief HHashContainer util, a helper for HHashSet<> instantiations.
 */
template<typename type_t>
struct hashmultiset_helper {
	typedef typename type_t::first_type key_type;
	inline static key_type const& key( type_t const& key_ ) {
		return ( key_.first );
	}
};

/*! \brief Hash multi-set container implementation.
 */
template<
	typename type_t,
	typename hasher_t = hash<type_t>,
	typename equal_key_t = yaal::equal_to<type_t>,
	typename allocator_t = allocator::system<HPair<type_t, int long>>
>
class HHashMultiSet final {
public:
	typedef type_t key_type;
/* cppcheck-suppress variableHidingTypedef */
	typedef type_t value_type;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	typedef HPair<type_t, int long> elem_t;
	typedef HHashContainer<elem_t, hasher_type, equal_key_type, hashmultiset_helper<elem_t>, allocator_t> engine_t;
	typedef typename engine_t::size_type size_type;
	typedef typename engine_t::allocator_type allocator_type;
	typedef typename engine_t::node_size node_size;
	typedef typename engine_t::node_type node_type;
	typedef HHashMultiSet<value_type, hasher_type, equal_key_type, allocator_t> this_type;
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		size_type _index;
		engine_t const* _owner;
		typename engine_t::HIterator _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void )
			: base_type(), _index( 0 ), _owner( nullptr ), _engine() {
			return;
		}
		HIterator( HIterator const& it_ )
			: base_type(), _index( it_._index ), _owner( it_._owner ), _engine( it_._engine ) {
			return;
		}
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_index = it_._index;
				_engine = it_._engine;
			}
			return ( *this );
		}
		HIterator& operator ++ ( void ) {
			if ( _index < ( _engine.get().second - 1 ) ) {
				++ _index;
			} else {
				_index = 0;
				++ _engine;
			}
			return ( *this );
		}
		HIterator const operator ++ ( int ) {
			HIterator it( *this );
			operator ++ ();
			return ( it );
		}
		HIterator& operator -- ( void ) {
			if ( _index > 0 ) {
				-- _index;
			} else {
				-- _engine;
				if ( _engine != _owner->end() ) {
					_index = _engine.get().second - 1;
				}
			}
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( *this );
			operator -- ();
			return ( it );
		}
		key_type const& operator* ( void ) const {
			return ( _engine.get().first );
		}
		key_type const* operator-> ( void ) const {
			return ( &_engine.get().first );
		}
		bool operator == ( HIterator const& it ) const {
			return ( ( _engine == it._engine ) && ( _index == it._index )  );
		}
		bool operator != ( HIterator const& it ) const {
			return ( ( _engine != it._engine ) || ( _index != it._index ) );
		}
	private:
		friend class HHashMultiSet<key_type, hasher_type, equal_key_type, allocator_t>;
		explicit HIterator( engine_t const* owner_, typename engine_t::HIterator const& it, size_type index_ )
			: base_type(), _index( index_ ), _owner( owner_ ), _engine( it ) {
			return;
		}
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	engine_t _engine;
public:
	HHashMultiSet( void )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		return;
	}
	HHashMultiSet( hasher_type const& hasher_, equal_key_type const& equals_ = equal_key_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, equals_, allocator_ ) {
		return;
	}
	explicit HHashMultiSet( allocator_type const& allocator_ )
		: _engine( hasher_type(), equal_key_type(), allocator_ ) {
		return;
	}
	/*! \brief Lower bound of size of map's table */
	explicit HHashMultiSet( size_type size_ )
		: _engine( hasher_type(), equal_key_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	HHashMultiSet(
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
	HHashMultiSet(
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
	HHashMultiSet(
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
	HHashMultiSet( HHashMultiSet const& set_ )
		: _engine( set_._engine.hasher(), set_._engine.equal_key(), set_._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( set_._engine );
		return;
		M_EPILOG
	}
	~HHashMultiSet( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HHashMultiSet& operator = ( HHashMultiSet const& set_ ) {
		M_PROLOG
		if ( &set_ != this ) {
			HHashMultiSet tmp( set_ );
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
		return ( iterator( &_engine, _engine.begin(), 0 ) );
		M_EPILOG
	}
	iterator end( void ) const {
		M_PROLOG
		return ( iterator( &_engine, _engine.end(), 0 ) );
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
		return ( iterator( &_engine, _engine.find( key_ ), 0 ) );
		M_EPILOG
	}
	iterator insert( value_type const& val_ ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( val_, 1L ) ) );
		if ( ! p.second )
			++ p.first.get().second;
		return ( iterator( &_engine, p.first, p.first.get().second - 1 ) );
		M_EPILOG
	}
	iterator insert( value_type&& val_ ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( yaal::move( val_ ), 1L ) ) );
		if ( ! p.second ) {
			++ p.first.get().second;
		}
		return ( iterator( &_engine, p.first, p.first.get().second - 1 ) );
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
	size_type count( value_type const& elem ) const {
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine.get().second : 0 );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		_engine.clear(); return;
		M_EPILOG
	}
	size_type get_size( void ) const {
		M_PROLOG
		size_type sizeAcc( 0 );
		for ( typename engine_t::HIterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it.get().second;
		return ( sizeAcc );
		M_EPILOG
	}
	size_type size( void ) const {
		M_PROLOG
		return ( get_size() );
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
	void swap( HHashMultiSet& set_ ) {
		if ( &set_ != this ) {
			using yaal::swap;
			swap( _engine, set_._engine );
		}
		return;
	}
	bool operator == ( HHashMultiSet const& set_ ) const {
		M_PROLOG
		if ( &set_ == this ) {
			return ( true );
		}
		if ( _engine.get_size() != set_._engine.get_size() ) {
			return ( false );
		}
		for ( typename engine_t::HIterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it ) {
			typename engine_t::HIterator otherIt( set_._engine.find( it.get().first ) );
			if ( otherIt == set_._engine.end() ) {
				return ( false );
			}
			if ( ! ( it.get().second == otherIt.get().second ) ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}
	bool operator != ( HHashMultiSet const& set_ ) const {
		M_PROLOG
		return ( ! operator == ( set_ ) );
		M_EPILOG
	}
private:
};

template<typename key_type, typename hasher_t, typename equal_key_t, typename allocator_t>
inline void swap(
	yaal::hcore::HHashMultiSet<key_type, hasher_t, equal_key_t, allocator_t>& a,
	yaal::hcore::HHashMultiSet<key_type, hasher_t, equal_key_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED */


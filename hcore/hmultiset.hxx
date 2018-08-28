/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hmultiset.hxx
 * \brief Declaration of HMultiSet<> class.
 */

#ifndef YAAL_HCORE_HMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HMULTISET_HXX_INCLUDED 1

#include "hcore/hsbbstree.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

/*! \brief HSBBSTree util, a helper for HMultiSet<> instantiations.
 */
template<typename type_t>
struct multiset_helper {
	typedef typename type_t::first_type key_type;
	inline static key_type const& key( type_t const& key_ ) {
		return ( key_.first );
	}
};

/*! \brief Binary tree based multi set.
 *
 * HMultiSet<> is a template representing self balancing binary search tree
 * data structure that holds set of (non-unique) keys.
 *
 * \tparam type_t - type of values held in set.
 * \tparam helper_t - HSBBSTree pluggable code.
 */
template<typename type_t, typename compare_t = less<type_t>, typename allocator_t = allocator::system<type_t> >
class HMultiSet final {
public:
	typedef type_t value_type;
	typedef type_t key_type;
	typedef compare_t compare_type;
private:
	typedef HPair<type_t, int long> elem_t;
	typedef HSBBSTree<elem_t, compare_type, multiset_helper<elem_t>, allocator_t> engine_t;
public:
	typedef typename engine_t::size_type size_type;
	typedef typename engine_t::allocator_type allocator_type;
	typedef typename engine_t::node_size node_size;
	typedef typename engine_t::node_type node_type;
	typedef HMultiSet<type_t, compare_t, allocator_t> this_type;
	/*! \brief Iterator for HMultiSet<> data structure.
	 */
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		size_type _index;
		engine_t const* _owner;
		typename engine_t::HIterator _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void )
			: base_type()
			, _index( 0 )
			, _owner( nullptr )
			, _engine() {
			return;
		}
		HIterator( HIterator const& it_ )
			: base_type()
			, _index( it_._index )
			, _owner( it_._owner )
			, _engine( it_._engine ) {
			return;
		}
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_index = it_._index;
				_owner = it_._owner;
				_engine = it_._engine;
			}
			return ( *this );
		}
		HIterator& operator ++ ( void ) {
			if ( _index < ( _engine->second - 1 ) ) {
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
					_index = _engine->second - 1;
				}
			}
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( *this );
			operator -- ();
			return ( it );
		}
		value_type const& operator * ( void ) const {
			M_ASSERT( _owner );
			return ( _engine->first );
		}
		value_type const* operator -> ( void ) const {
			M_ASSERT( _owner );
			return ( &_engine->first );
		}
		bool operator == ( HIterator const& it ) const {
			M_ASSERT( it._owner == _owner );
			return ( ( _engine == it._engine ) && ( _index == it._index )  );
		}
		bool operator != ( HIterator const& it ) const {
			M_ASSERT( it._owner == _owner );
			return ( ( _engine != it._engine ) || ( _index != it._index ) );
		}
	private:
		friend class HMultiSet<type_t, compare_type, allocator_t>;
		explicit HIterator( engine_t const* owner_, typename engine_t::HIterator const& it, size_type index_ )
			: base_type(), _index( index_ ), _owner( owner_ ), _engine( it ) {};
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	engine_t _engine;
public:
	HMultiSet( void )
		: _engine( compare_type(), allocator_type() ) {
		return;
	}
	explicit HMultiSet( compare_type const& compare_, allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		return;
	}
	explicit HMultiSet( allocator_type const& allocator_ )
		: _engine( compare_type(), allocator_ ) {
		return;
	}
	template<typename iterator_t>
	HMultiSet( iterator_t first, iterator_t last, compare_type const& compare_ = compare_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}
	HMultiSet( HMultiSet const& source )
		: _engine( source._engine.compare(), source._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
	}
	HMultiSet& operator = ( HMultiSet const& multiset_ ) {
		M_PROLOG
		if ( &multiset_ != this ) {
			HMultiSet tmp( multiset_ );
			swap( tmp );
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
		for ( typename engine_t::HIterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it->second;
		return ( sizeAcc );
		M_EPILOG
	}
	bool empty( void ) const {
		return ( is_empty() );
	}
	bool is_empty( void ) const {
		return ( _engine.is_empty() );
	}
	HIterator insert( value_type const& elem ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( elem, 1L ) ) );
		if ( ! p.second ) {
			++ p.first->second;
		}
		return ( HIterator( &_engine, p.first, p.first->second - 1 ) );
		M_EPILOG
	}
	HIterator insert( value_type&& elem ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( yaal::move( elem ), yaal::move( 1L ) ) ) );
		if ( ! p.second ) {
			++ p.first->second;
		}
		return ( HIterator( &_engine, p.first, p.first->second - 1 ) );
		M_EPILOG
	}
	template<typename iter_t>
	void insert( iter_t i, iter_t endIt ) {
		M_PROLOG
		for ( ; i != endIt; ++ i ) {
			insert( *i );
		}
		return;
		M_EPILOG
	}
	size_type count( value_type const& elem ) const {
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine->second : 0 );
		M_EPILOG
	}
	size_type erase( value_type const& elem ) {
		M_PROLOG
		HIterator it( find( elem ) );
		size_type erased( 0 );
		if ( it != end() ) {
			erased = it._engine->second;
			_engine.remove( it._engine );
		}
		return ( erased );
		M_EPILOG
	}
	HIterator erase( HIterator const& it ) {
		M_PROLOG
		return ( _engine.remove( it._engine ) );
		M_EPILOG
	}
	HIterator erase( HIterator first_, HIterator const& last_ ) {
		M_PROLOG
		while ( first_ != last_ ) {
			first_ = erase( first_ );
		}
		return ( first_ );
		M_EPILOG
	}
	HIterator find( value_type const& e ) const {
		return ( HIterator( &_engine, _engine.find( e ), 0 ) );
	}
	HIterator lower_bound( value_type const& e ) const {
		return ( HIterator( &_engine, _engine.lower_bound( e ), 0 ) );
	}
	HIterator upper_bound( value_type const& e ) const {
		return ( HIterator( &_engine, _engine.upper_bound( e ), 0 ) );
	}
	HIterator begin( void ) const {
		return ( HIterator( &_engine, _engine.begin(), 0 ) );
	}
	HIterator end( void ) const {
		return ( HIterator( &_engine, _engine.end(), 0 ) );
	}
	reverse_iterator rbegin( void ) const {
		return ( end() );
	}
	reverse_iterator rend( void ) const {
		return ( begin() );
	}
	void clear( void ) {
		_engine.clear();
	}
	void swap( HMultiSet& other ) {
		if ( &other != this ) {
			using yaal::swap;
			_engine.swap( other._engine );
		}
	}
	bool operator == ( HMultiSet const& set_ ) const {
		M_PROLOG
		return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) );
		M_EPILOG
	}
	bool operator < ( HMultiSet const& set_ ) const {
		M_PROLOG
		return ( ( &set_ != this ) && lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) );
		M_EPILOG
	}
};

template<typename value_type, typename compare_t, typename allocator_t>
inline void swap( yaal::hcore::HMultiSet<value_type, compare_t, allocator_t>& a, yaal::hcore::HMultiSet<value_type, compare_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HMULTISET_HXX_INCLUDED */


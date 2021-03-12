/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hset.hxx
 * \brief Declaration of HSet<> class.
 */

#ifndef YAAL_HCORE_HSET_HXX_INCLUDED
#define YAAL_HCORE_HSET_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/hsbbstree.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

/*! \brief HSBBSTree utility, a helper for HSet<> instantiations.
 */
template<typename type_t>
struct set_helper {
	typedef type_t key_type;
	inline static key_type const& key( type_t const& key_ ) {
		return key_;
	}
};

/*! \brief Binary tree based set.
 *
 * HSet<> is a template representing self balancing binary search tree
 * data structure that holds set of keys.
 *
 * \tparam type_t - type of values held in set.
 * \tparam helper_t - HSBBSTree pluggable code.
 */
template<typename type_t, typename compare_t = less<type_t>, typename allocator_t = allocator::system<type_t> >
class HSet final {
public:
	typedef type_t value_type;
	typedef type_t key_type;
	typedef compare_t compare_type;
	typedef HSBBSTree<value_type, compare_type, set_helper<value_type>, allocator_t> engine_type;
	typedef typename engine_type::size_type size_type;
	typedef typename engine_type::allocator_type allocator_type;
	typedef typename engine_type::node_size node_size;
	typedef typename engine_type::node_type node_type;
	typedef HSet<type_t, compare_t, allocator_t> this_type;
	/*! \brief Iterator for HSet<> data structure.
	 */
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		typename engine_type::iterator_type _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void ) : base_type(), _engine() {}
		HIterator( HIterator const& it_ )
			: base_type()
			, _engine( it_._engine ) {
			return;
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
			return it;
		}
		HIterator& operator -- ( void ) {
			-- _engine;
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( *this );
			-- _engine;
			return it;
		}
		value_type const& operator * ( void ) const {
			return ( *_engine );
		}
		value_type const* operator -> ( void ) const {
			return ( &*_engine );
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
		friend class HSet<type_t, compare_type, allocator_t>;
		explicit HIterator( typename engine_type::iterator_type const& it )
			: base_type(), _engine( it ) {}
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	typedef HPair<iterator, bool> insert_result;
private:
	engine_type _engine;
public:
	HSet( void )
		: _engine( compare_type(), allocator_type() ) {
		return;
	}
	explicit HSet( compare_type const& compare_, allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		return;
	}
	explicit HSet( allocator_type const& allocator_ )
		: _engine( compare_type(), allocator_ ) {
		return;
	}
	template<typename iterator_t>
	HSet( iterator_t first, iterator_t last, compare_type const& compare_ = compare_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( compare_, allocator_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}

	/*! \brief Construct set based on compile time constant data set.
	 *
	 * \param constants_ - set of compile time constants to into into this set.
	 */
	HSet( std::initializer_list<value_type> constants_ )
		: _engine( compare_type(), allocator_type() ) {
		M_PROLOG
		insert( constants_.begin(), constants_.end() );
		return;
		M_EPILOG
	}
	HSet( HSet const& source )
		: _engine( source._engine.compare(), source._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
	}
	HSet& operator = ( HSet const& set_ ) {
		M_PROLOG
		if ( &set_ != this ) {
			HSet tmp( set_ );
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
	insert_result insert( value_type const& elem ) {
		M_PROLOG
		HPair<typename engine_type::iterator_type, bool> p = _engine.insert( elem );
		return ( make_pair( HIterator( p.first ), p.second ) );
		M_EPILOG
	}
	insert_result insert( value_type&& elem ) {
		M_PROLOG
		HPair<typename engine_type::iterator_type, bool> p = _engine.insert( yaal::move( elem ) );
		return ( make_pair( HIterator( p.first ), p.second ) );
		M_EPILOG
	}
	void insert( HIterator it, value_type const& elem, bool forceHint = false ) {
		M_PROLOG
		_engine.insert( it._engine, elem, forceHint );
		return;
		M_EPILOG
	}
	void insert( HIterator it, value_type&& elem, bool forceHint = false ) {
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
	size_type count( value_type const& elem ) const {
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? 1 : 0 );
		M_EPILOG
	}
	size_type erase( value_type const& elem ) {
		M_PROLOG
		HIterator it( find( elem ) );
		size_type erased( 0 );
		if ( it != end() ) {
			_engine.remove( it._engine );
			erased = 1;
		}
		return erased;
		M_EPILOG
	}
	HIterator erase( HIterator const& it ) {
		M_PROLOG
		return ( HIterator( _engine.remove( it._engine ) ) );
		M_EPILOG
	}
	HIterator erase( HIterator first_, HIterator const& last_ ) {
		M_PROLOG
		HIterator it( first_ );
		while ( it != last_ ) {
			++ it;
			first_ = erase( first_ );
		}
		return first_;
		M_EPILOG
	}
	HIterator find( value_type const& e ) const
		{ return ( HIterator( _engine.find( e ) ) ); }
	HIterator lower_bound( value_type const& e ) const
		{ return ( HIterator( _engine.lower_bound( e ) ) ); }
	HIterator upper_bound( value_type const& e ) const
		{ return ( HIterator( _engine.upper_bound( e ) ) ); }
	HIterator begin( void ) const
		{ return ( HIterator( _engine.begin() ) ); }
	HIterator end( void ) const
		{ return ( HIterator( _engine.end() ) ); }
	reverse_iterator rbegin( void ) const
		{ return ( end() ); }
	reverse_iterator rend( void ) const
		{ return ( begin() ); }
	void clear( void ) {
		_engine.clear();
	}
	void swap( HSet& other ) {
		if ( &other != this ) {
			using yaal::swap;
			_engine.swap( other._engine );
		}
	}
	bool operator == ( HSet const& set_ ) const {
		M_PROLOG
		return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) );
		M_EPILOG
	}
	bool operator < ( HSet const& set_ ) const {
		M_PROLOG
		return ( ( &set_ != this ) && lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) );
		M_EPILOG
	}
};

template<typename value_type, typename compare_t, typename allocator_t>
inline void swap( yaal::hcore::HSet<value_type, compare_t, allocator_t>& a, yaal::hcore::HSet<value_type, compare_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HSET_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/htwowaymap.hxx
 * \brief Declaration and implementation of HTwoWayMap<> class template.
 */

#ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED
#define YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/hmap.hxx"

namespace yaal {

namespace tools {


/*! \brief less for HTwoWayMap.
 */
template<typename key_type>
struct twowaymap_less {
inline bool operator()( key_type const& left, key_type const& right ) const
	{  return ( *left < *right );  }
};

/*! \brief Bijective relation of elements.
 *
 * HTwoWayMap<> is a template representing pair of sets with unambiguous
 * one-to-one correspondence between their elements.
 *
 * \tparam left_type_t - type of elements in "left" set in map.
 * \tparam right_type_t - type of elements in "right" set in map.
 */
template<typename left_type_t, typename right_type_t>
class HTwoWayMap final {
public:
	typedef HTwoWayMap<left_type_t, right_type_t> this_type;
	typedef left_type_t left_type;
	typedef right_type_t right_type;
	typedef yaal::hcore::HPair<left_type const, right_type const> value_type;
	typedef yaal::hcore::HList<value_type> storage_type;
	typedef typename storage_type::const_iterator const_iterator;
	typedef typename storage_type::const_iterator iterator;
	typedef typename storage_type::const_reverse_iterator reverse_iterator;
	typedef typename storage_type::const_reverse_iterator const_reverse_iterator;
	typedef yaal::hcore::HPair<iterator, bool> insert_result;
	template<typename view_type_t, typename selector_t>
	class HView;
	typedef HView<left_type, select2nd<value_type>> left_view_type;
	typedef HView<right_type, select1st<value_type>> right_view_type;
private:
	storage_type _data;
	left_view_type _leftView;
	right_view_type _rightView;
public:
	HTwoWayMap( void )
		: _data(), _leftView(), _rightView() {
		return;
	}
	template<typename iter_t>
	HTwoWayMap( iter_t first, iter_t last )
		: _data(), _leftView(), _rightView() {
		M_PROLOG
		assign( first, last );
		return;
		M_EPILOG
	}
	HTwoWayMap( HTwoWayMap const& twm_ )
		: _data(), _leftView(), _rightView() {
		M_PROLOG
		assign( twm_._data.begin(), twm_._data.end() );
		return;
		M_EPILOG
	}
	HTwoWayMap& operator = ( HTwoWayMap const& twm_ ) {
		M_PROLOG
		if ( &twm_ != this ) {
			HTwoWayMap tmp( twm_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HTwoWayMap& twm_ ) {
		M_PROLOG
		if ( &twm_ != this ) {
			using yaal::swap;
			swap( _data, twm_._data );
			swap( _leftView._data, twm_._leftView._data );
			swap( _rightView._data, twm_._rightView._data );
		}
		return;
		M_EPILOG
	}
	template<typename iter_t>
	void assign( iter_t first, iter_t last ) {
		M_PROLOG
		clear();
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	bool is_empty( void ) const {
		M_PROLOG
		bool isEmpty( _data.is_empty() );
		return isEmpty;
		M_EPILOG
	}
	bool empty( void ) const {
		M_PROLOG
		return ( is_empty() );
		M_EPILOG
	}
	int long get_size( void ) const {
		M_PROLOG
		return ( _data.get_size() );
		M_EPILOG
	}
	int long size( void ) const {
		M_PROLOG
		return ( get_size() );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		_rightView._data.clear();
		_leftView._data.clear();
		_data.clear();
		return;
		M_EPILOG
	}
	insert_result insert( value_type const& );
	const_iterator begin( void ) const {
		M_PROLOG
		return ( _data.begin() );
		M_EPILOG
	}
	const_iterator end( void ) const {
		M_PROLOG
		return ( _data.end() );
		M_EPILOG
	}
	const_reverse_iterator rbegin( void ) const {
		M_PROLOG
		return ( _data.rbegin() );
		M_EPILOG
	}
	const_reverse_iterator rend( void ) const {
		M_PROLOG
		return ( _data.rend() );
		M_EPILOG
	}
	int long count( value_type const& value_ ) const {
		M_PROLOG
		typename left_view_type::view_storage_type::const_iterator it( _leftView._data.find( &(value_.first) ) );
		return ( ( ( it != _leftView._data.end() ) && ( it->second->second == value_.second ) ) ? 1 : 0 );
		M_EPILOG
	}
	int long erase( value_type const& );
	iterator erase( const_iterator const& );
	iterator erase( iterator first, iterator last ) {
		M_PROLOG
		for ( ; first != last; ) {
			first = erase( first );
		}
		return first;
		M_EPILOG
	}
	bool operator == ( HTwoWayMap const& twm_ ) const {
		M_PROLOG
		return ( ( &twm_ == this )
				|| safe_equal( _leftView._data.begin(),
					_leftView._data.end(),
					twm_._leftView._data.begin(),
					twm_._leftView._data.end(),
					equal_helper() ) );
		M_EPILOG
	}
	bool operator < ( HTwoWayMap const& twm_ ) const {
		M_PROLOG
		return ( ( &twm_ != this )
				&& lexicographical_compare(
					_leftView._data.begin(),
					_leftView._data.end(),
					twm_._leftView._data.begin(),
					twm_._leftView._data.end(),
					less_helper() ) );
		M_EPILOG
	}
	left_view_type const& left( void ) const {
		return ( _leftView );
	}
	right_view_type const& right( void ) const {
		return ( _rightView );
	}
	private:
		struct equal_helper {
			bool operator()( typename left_view_type::view_storage_type::value_type const& left_, typename left_view_type::view_storage_type::value_type const& right_ ) const {
				return ( *(left_.second) == *(right_.second) );
			}
		};
		struct less_helper {
			bool operator()( typename left_view_type::view_storage_type::value_type const& left_, typename left_view_type::view_storage_type::value_type const& right_ ) const {
				return ( *(left_.second) < *(right_.second) );
			}
		};
};

template<typename left_type_t, typename right_type_t>
template<typename view_type_t, typename selector_t>
class HTwoWayMap<left_type_t, right_type_t>::HView {
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t, selector_t> this_type;
	typedef HTwoWayMap<left_type_t, right_type_t> twowaymap_type;
	typedef typename twowaymap_type::storage_type storage_type;
	typedef yaal::hcore::HMap<view_type_t const*, typename storage_type::iterator, twowaymap_less<view_type_t const*> > view_storage_type;
public:
	class HIterator;
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
	typedef typename selector_t::result_type result_type;
private:
	view_storage_type _data;
public:
	const_iterator begin( void ) const {
		return ( const_iterator( this, _data.begin() ) );
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, _data.end() ) );
	}
	const_iterator find( view_type_t const& key_ ) const {
		return ( const_iterator( this, _data.find( &key_ ) ) );
	}
	const_iterator lower_bound( view_type_t const& key_ ) const {
		return ( const_iterator( this, _data.lower_bound( &key_ ) ) );
	}
	const_iterator upper_bound( view_type_t const& key_ ) const {
		return ( const_iterator( this, _data.upper_bound( &key_ ) ) );
	}
	int long count( view_type_t const& key_ ) const {
		return ( _data.count( &key_ ) );
	}
	result_type const& at( view_type_t const& key_ ) const {
		M_PROLOG
		typename view_storage_type::const_iterator it( _data.find( &key_ ) );
		if ( ! ( it != _data.end() ) ) {
			throw hcore::HInvalidKeyException( "bad key" );
		}
		return ( selector_t()( *(it->second) ) );
		M_EPILOG
	}
	const_reverse_iterator rbegin( void ) const;
	const_reverse_iterator rend( void ) const;
private:
	HView( void )
		: _data() {
		return;
	}
	friend class HTwoWayMap<left_type_t, right_type_t>;
};

template<typename left_type_t, typename right_type_t>
template<typename view_type_t, typename selector_t>
class HTwoWayMap<left_type_t, right_type_t>::HView<view_type_t, selector_t>::HIterator
	: public yaal::hcore::iterator_interface<typename HTwoWayMap<left_type_t, right_type_t>::value_type, yaal::hcore::iterator_category::forward> {
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t, selector_t>::HIterator this_type;
	typedef yaal::hcore::iterator_interface<typename HTwoWayMap<left_type_t, right_type_t>::value_type, yaal::hcore::iterator_category::forward> base_type;
	typedef typename base_type::value_type value_type;
	typedef HTwoWayMap<left_type_t, right_type_t> twowaymap_type;
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t, selector_t> view_type;
	typedef typename view_type::view_storage_type view_storage_type;
	typedef typename view_storage_type::const_iterator iterator_t;
	view_type const* _owner;
	iterator_t _engine;
public:
	HIterator( void ) : base_type(), _owner( nullptr ), _engine() {}
	HIterator( HIterator const& it_ ) : base_type(), _owner( it_._owner ), _engine( it_._engine ) {}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_engine = it_._engine;
		}
		return ( *this );
	}
	HIterator& operator ++ ( void ) {
		++ _engine;
		return ( *this );
	}
	HIterator& operator -- ( void ) {
		-- _engine;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _engine;
		return it;
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		++ _engine;
		return it;
	}
	bool operator != ( HIterator const& it_ ) {
		M_ASSERT( _owner == it_._owner );
		return ( _engine != it_._engine );
	}
	bool operator == ( HIterator const& it_ ) {
		M_ASSERT( _owner == it_._owner );
		return ( _engine == it_._engine );
	}
	value_type const& operator*( void ) const {
		return ( *(_engine->second) );
	}
	value_type const* operator->( void ) const {
		return ( &*(_engine->second) );
	}
private:
	HIterator( view_type const* owner_, iterator_t engine_ ) : base_type(), _owner( owner_ ), _engine( engine_ ) {}
	friend class HTwoWayMap<left_type_t, right_type_t>;
};

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::insert_result
HTwoWayMap<left_type_t, right_type_t>::insert( value_type const& value_ ) {
	M_PROLOG
	typename left_view_type::view_storage_type::const_iterator lvit( _leftView._data.find( &(value_.first) ) );
	bool inserted( false );
	typename storage_type::iterator it;
	if ( ! ( lvit != _leftView._data.end() ) ) {
		typename right_view_type::view_storage_type::const_iterator rvit( _rightView._data.find( &(value_.second) ) );
		if ( ! ( rvit != _rightView._data.end() ) ) {
			_data.push_back( value_ );
			it = prev( _data.end() );
			_leftView._data.insert( make_pair( &it->first, it ) );
			_rightView._data.insert( make_pair( &it->second, it ) );
			inserted = true;
		} else
			it = rvit->second;
	} else
		it = lvit->second;
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return ( make_pair( it, inserted ) );
	M_EPILOG
}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::erase( value_type const& value_ ) {
	M_PROLOG
	typename left_view_type::view_storage_type::iterator lvit( _leftView._data.find( &(value_.first) ) );
	bool erased = false;
	if ( ( lvit != _leftView._data.end() ) && ( lvit->second->second == value_.second ) ) {
		typename storage_type::iterator it( lvit->second );
		int long const erasedInRight( _rightView._data.erase( &(value_.second) ) );
		M_ASSERT( erasedInRight == 1 );
		static_cast<void>( erasedInRight );
		_leftView._data.erase( lvit );
		_data.erase( it );
		erased = true;
	}
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return ( erased ? 1 : 0 );
	M_EPILOG
}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::iterator HTwoWayMap<left_type_t, right_type_t>::erase( const_iterator const& it ) {
	M_PROLOG
	iterator next( it );
	++ next;
	typename left_view_type::view_storage_type::iterator lvit( _leftView._data.find( &(it->first) ) );
	M_ASSERT( lvit != _leftView._data.end() );
	typename storage_type::iterator toErase( lvit->second );
	_leftView._data.erase( lvit );
	int long const erasedInRight( _rightView._data.erase( &(it->second) ) );
	M_ASSERT( erasedInRight == 1 );
	static_cast<void>( erasedInRight );
	_data.erase( toErase );
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return next;
	M_EPILOG
}

template<typename left_type, typename right_type>
inline void swap( yaal::tools::HTwoWayMap<left_type, right_type>& a, yaal::tools::HTwoWayMap<left_type, right_type>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED */


/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/htwowaymap.hxx - this file is integral part of yaal project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file tools/htwowaymap.hxx
 * \brief Declaration and implmentation of HTwoWayMap<> class template.
 */

#ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED
#define YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/hmap.hxx"

namespace yaal
{

namespace tools
{

/*! \brief HSBBSTree util, a helper for HSet<> instatiations for left view.
 */
template<typename key_type, typename data_type>
struct twowaymap_helper
{

inline static bool less( yaal::hcore::HPair<key_type, data_type> const& left, yaal::hcore::HPair<key_type, data_type> const& right )
	{	return ( *(left.first) < *(right.first) );	}

inline static bool less( key_type const& left, yaal::hcore::HPair<key_type, data_type> const& right )
	{	return ( *left < *(right.first) );	}

inline static bool less( yaal::hcore::HPair<key_type, data_type> const& left, key_type const& right )
	{	return ( *(left.first) < *right );	}

};

/*! \brief Bijective relation of elemnets.
 *
 * HTwoWayMap<> is a template representing pair of sets with unambiguous
 * ono-to-one correspondence between their elements.
 *
 * \tparam left_type_t - type of elements in "left" set in map.
 * \tparam right_type_t - type of elements in "right" set in map.
 */
template<typename left_type_t, typename right_type_t>
class HTwoWayMap
	{
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
	template<typename view_type_t>
	class HView;
	typedef HView<left_type> left_view_type;
	typedef HView<right_type> right_view_type;
private:
	storage_type _data;
	left_view_type _leftView;
	right_view_type _rightView;
public:
	HTwoWayMap( void );
	template<typename iter_t>
	HTwoWayMap( iter_t, iter_t );
	HTwoWayMap( HTwoWayMap const& );
	HTwoWayMap& operator = ( HTwoWayMap const& );
	void swap( HTwoWayMap& );
	template<typename iter_t>
	void assign( iter_t, iter_t );
	bool is_empty( void ) const;
	bool empty( void ) const;
	int long get_size( void ) const;
	int long size( void ) const;
	void clear( void );
	insert_result insert( value_type const& );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	const_reverse_iterator rbegin( void ) const;
	const_reverse_iterator rend( void ) const;
	int long count( value_type const& ) const;
	int long erase( value_type const& );
	void erase( const_iterator const& );
	void erase( iterator, iterator );
	bool operator == ( HTwoWayMap const& ) const;
	bool operator < ( HTwoWayMap const& ) const;
private:
	struct equal_helper
		{
		bool operator()( typename left_view_type::view_storage_type::value_type const& left_, typename left_view_type::view_storage_type::value_type const& right_ ) const
			{
			return ( *(left_.second) == *(right_.second) );
			}
		};
	struct less_helper
		{
		bool operator()( typename left_view_type::view_storage_type::value_type const& left_, typename left_view_type::view_storage_type::value_type const& right_ ) const
			{
			return ( *(left_.second) < *(right_.second) );
			}
		};
	};

template<typename left_type_t, typename right_type_t>
template<typename view_type_t>
class HTwoWayMap<left_type_t, right_type_t>::HView
	{
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t> this_type;
	typedef HTwoWayMap<left_type_t, right_type_t> twowaymap_type;
	typedef typename twowaymap_type::storage_type storage_type;
	typedef twowaymap_helper<view_type_t const*, typename storage_type::iterator> twowaymap_view_helper;
	typedef yaal::hcore::HMap<view_type_t const*, typename storage_type::iterator, twowaymap_view_helper> view_storage_type;
public:
	class HIterator;
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
private:
	view_storage_type _data;
public:
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	const_reverse_iterator rbegin( void ) const;
	const_reverse_iterator rend( void ) const;
	const_iterator find( view_type_t const& ) const;
private:
	friend class HTwoWayMap<left_type_t, right_type_t>;
	};

template<typename left_type_t, typename right_type_t>
template<typename view_type_t>
class HTwoWayMap<left_type_t, right_type_t>::HView<view_type_t>::HIterator
	: public yaal::hcore::iterator_interface<typename HTwoWayMap<left_type_t, right_type_t>::value_type, yaal::hcore::iterator_category::forward>
	{
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t>::HIterator this_type;
	typedef yaal::hcore::iterator_interface<typename HTwoWayMap<left_type_t, right_type_t>::value_type, yaal::hcore::iterator_category::forward> base_type;
	typedef typename base_type::value_type value_type;
	typedef HTwoWayMap<left_type_t, right_type_t> twowaymap_type;
	typedef typename HTwoWayMap<left_type_t, right_type_t>::template HView<view_type_t> view_type;
	typedef typename view_type::view_storage_type view_storage_type;
	typedef typename view_storage_type::const_iterator iterator_t;
	view_type const* _owner;
	iterator_t _engine;
public:
	HIterator( void ) : _owner( NULL ), _engine() {}
	HIterator( HIterator const& it_ ) : _owner( it_._owner ), _engine( it_._engine ) {}
	HIterator& operator = ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_owner = it_._owner;
			_engine = it_._engine;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ _engine;
		return ( *this );
		}
	HIterator& operator -- ( void )
		{
		-- _engine;
		return ( *this );
		}
	HIterator operator ++ ( int )
		{
		HIterator it( *this );
		++ _engine;
		return ( it );
		}
	HIterator operator -- ( int )
		{
		HIterator it( *this );
		++ _engine;
		return ( it );
		}
	bool operator != ( HIterator const& it_ )
		{
		M_ASSERT( _owner == it_._owner );
		return ( _engine != it_._engine );
		}
	bool operator == ( HIterator const& it_ )
		{
		M_ASSERT( _owner == it_._owner );
		return ( _engine == it_._engine );
		}
	value_type const& operator*( void ) const
		{
		return ( *(_engine->second) );
		}
	value_type const* operator->( void ) const
		{
		return ( &*(_engine->second) );
		}
private:
	HIterator( view_type const* owner_, iterator_t engine_ ) : _owner( owner_ ), _engine( engine_ ) {}
	friend class HTwoWayMap<left_type_t, right_type_t>;
	template<typename>
	friend class HTwoWayMap<left_type_t, right_type_t>::HView;
	};

template<typename left_type_t, typename right_type_t>
HTwoWayMap<left_type_t, right_type_t>::HTwoWayMap( void )
	: _data(), _leftView(), _rightView()
	{
	}

template<typename left_type_t, typename right_type_t>
template<typename iter_t>
HTwoWayMap<left_type_t, right_type_t>::HTwoWayMap( iter_t first, iter_t last )
	: _data(), _leftView(), _rightView()
	{
	M_PROLOG
	assign( first, last );
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
HTwoWayMap<left_type_t, right_type_t>::HTwoWayMap( HTwoWayMap const& twm_ )
	: _data(), _leftView(), _rightView()
	{
	M_PROLOG
	assign( twm_._data.begin(), twm_._data.end() );
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
HTwoWayMap<left_type_t, right_type_t>& HTwoWayMap<left_type_t, right_type_t>::operator = ( HTwoWayMap const& twm_ )
	{
	M_PROLOG
	if ( &twm_ != this )
		{
		HTwoWayMap tmp( twm_ );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
void HTwoWayMap<left_type_t, right_type_t>::swap( HTwoWayMap& twm_ )
	{
	M_PROLOG
	if ( &twm_ != this )
		{
		using yaal::swap;
		swap( _data, twm_._data );
		swap( _leftView._data, twm_._leftView._data );
		swap( _rightView._data, twm_._rightView._data );
		}
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
template<typename iter_t>
void HTwoWayMap<left_type_t, right_type_t>::assign( iter_t first, iter_t last )
	{
	M_PROLOG
	clear();
	for ( ; first != last; ++ first )
		insert( *first );
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
bool HTwoWayMap<left_type_t, right_type_t>::is_empty( void ) const
	{
	M_PROLOG
	bool isEmpty( _data.is_empty() );
	return ( isEmpty );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
bool HTwoWayMap<left_type_t, right_type_t>::empty( void ) const
	{
	M_PROLOG
	return ( is_empty() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::get_size( void ) const
	{
	M_PROLOG
	return ( _data.get_size() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::size( void ) const
	{
	M_PROLOG
	return ( get_size() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
void HTwoWayMap<left_type_t, right_type_t>::clear( void )
	{
	M_PROLOG
	_rightView._data.clear();
	_leftView._data.clear();
	_data.clear();
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::const_iterator
HTwoWayMap<left_type_t, right_type_t>::begin( void ) const
	{
	M_PROLOG
	return ( _data.begin() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::const_iterator
HTwoWayMap<left_type_t, right_type_t>::end( void ) const
	{
	M_PROLOG
	return ( _data.end() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::const_reverse_iterator
HTwoWayMap<left_type_t, right_type_t>::rbegin( void ) const
	{
	M_PROLOG
	return ( _data.rbegin() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::const_reverse_iterator
HTwoWayMap<left_type_t, right_type_t>::rend( void ) const
	{
	M_PROLOG
	return ( _data.rend() );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
typename HTwoWayMap<left_type_t, right_type_t>::insert_result
HTwoWayMap<left_type_t, right_type_t>::insert( value_type const& value_ )
	{
	M_PROLOG
	typename left_view_type::view_storage_type::const_iterator lvit( _leftView._data.find( &(value_.first) ) );
	bool inserted( false );
	typename storage_type::iterator it;
	if ( ! ( lvit != _leftView._data.end() ) )
		{
		typename right_view_type::view_storage_type::const_iterator rvit( _rightView._data.find( &(value_.second) ) );
		if ( ! ( rvit != _rightView._data.end() ) )
			{
			_data.push_back( value_ );
			it = _data.rbegin().base();
			_leftView._data.insert( make_pair( &it->first, it ) );
			_rightView._data.insert( make_pair( &it->second, it ) );
			inserted = true;
			}
		else
			it = rvit->second;
		}
	else
		it = lvit->second;
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return ( make_pair( it, inserted ) );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::count( value_type const& value_ ) const
	{
	M_PROLOG
	typename left_view_type::view_storage_type::const_iterator it( _leftView._data.find( &(value_.first) ) );
	return ( ( ( it != _leftView._data.end() ) && ( it->second->second == value_.second ) ) ? 1 : 0 );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::erase( value_type const& value_ )
	{
	M_PROLOG
	typename left_view_type::view_storage_type::iterator lvit( _leftView._data.find( &(value_.first) ) );
	bool erased = false;
	if ( ( lvit != _leftView._data.end() ) && ( lvit->second->second == value_.second ) )
		{
		typename storage_type::iterator it( lvit->second );
		int long const erasedInRight( _rightView._data.erase( &(value_.second) ) );
		M_ASSERT( erasedInRight == 1 );
		_leftView._data.erase( lvit );
		_data.erase( it );
		erased = true;
		}
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return ( erased ? 1 : 0 );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
void HTwoWayMap<left_type_t, right_type_t>::erase( const_iterator const& it )
	{
	M_PROLOG
	typename left_view_type::view_storage_type::iterator lvit( _leftView._data.find( &(it->first) ) );
	M_ASSERT( lvit != _leftView._data.end() );
	typename storage_type::iterator toErase( lvit->second );
	_leftView._data.erase( lvit );
	int long const erasedInRight( _rightView._data.erase( &(it->second) ) );
	M_ASSERT( erasedInRight == 1 );
	_data.erase( toErase );
	M_ASSERT( ( _data.get_size() == _leftView._data.get_size() ) && ( _data.get_size() == _rightView._data.get_size() ) );
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
void HTwoWayMap<left_type_t, right_type_t>::erase( iterator first, iterator last )
	{
	M_PROLOG
	for ( ; first != last; )
		{
		iterator del( first );
		++ first;
		erase( del );
		}
	return;
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
bool HTwoWayMap<left_type_t, right_type_t>::operator == ( HTwoWayMap const& twm_ ) const
	{
	M_PROLOG
	return ( ( &twm_ == this ) || safe_equal( _leftView._data.begin(), _leftView._data.end(), twm_._leftView._data.begin(), twm_._leftView._data.end(), equal_helper() ) );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
bool HTwoWayMap<left_type_t, right_type_t>::operator < ( HTwoWayMap const& twm_ ) const
	{
	M_PROLOG
	return ( ( &twm_ != this ) && lexicographical_compare( _leftView._data.begin(), _leftView._data.end(), twm_._leftView._data.begin(), twm_._leftView._data.end(), less_helper() ) );
	M_EPILOG
	}

}

template<typename left_type, typename right_type>
inline void swap( yaal::tools::HTwoWayMap<left_type, right_type>& a, yaal::tools::HTwoWayMap<left_type, right_type>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED */


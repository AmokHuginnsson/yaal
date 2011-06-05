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
	typedef left_type_t left_type;
	typedef right_type_t right_type;
	typedef yaal::hcore::HPair<left_type const, right_type const> value_type;
	typedef yaal::hcore::HList<value_type> storage_type;
	typedef typename storage_type::const_iterator const_iterator;
	typedef typename storage_type::const_iterator iterator;
	typedef typename storage_type::const_reverse_iterator reverse_iterator;
	typedef typename storage_type::const_reverse_iterator const_reverse_iterator;
	typedef yaal::hcore::HPair<iterator, bool> insert_result;
	typedef twowaymap_helper<left_type const*, typename storage_type::iterator> twowaymap_helper_left;
	typedef twowaymap_helper<right_type const*, typename storage_type::iterator> twowaymap_helper_right;
	typedef yaal::hcore::HMap<left_type const*, typename storage_type::iterator, twowaymap_helper_left> left_view_type;
	typedef yaal::hcore::HMap<right_type const*, typename storage_type::iterator, twowaymap_helper_right> right_view_type;
private:
	storage_type _data;
	left_view_type _leftView;
	right_view_type _rightView;
public:
	HTwoWayMap( void );
	bool is_empty( void ) const;
	bool empty( void ) const;
	int long get_size( void ) const;
	int long size( void ) const;
	void clear( void );
	insert_result insert( value_type const& );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	int long count( value_type const& ) const;
	int long erase( value_type const& );
	void erase( const_iterator const& );
	};

template<typename left_type_t, typename right_type_t>
HTwoWayMap<left_type_t, right_type_t>::HTwoWayMap( void )
	: _data(), _leftView(), _rightView()
	{
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
	_rightView.clear();
	_leftView.clear();
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
typename HTwoWayMap<left_type_t, right_type_t>::insert_result
HTwoWayMap<left_type_t, right_type_t>::insert( value_type const& value_ )
	{
	M_PROLOG
	typename left_view_type::const_iterator lvit( _leftView.find( &(value_.first) ) );
	bool inserted( false );
	typename storage_type::iterator it;
	if ( ! ( lvit != _leftView.end() ) )
		{
		typename right_view_type::const_iterator rvit( _rightView.find( &(value_.second) ) );
		if ( ! ( rvit != _rightView.end() ) )
			{
			_data.push_back( value_ );
			it = _data.rbegin().base();
			_leftView.insert( make_pair( &it->first, it ) );
			_rightView.insert( make_pair( &it->second, it ) );
			inserted = true;
			}
		else
			it = rvit->second;
		}
	else
		it = lvit->second;
	M_ASSERT( ( _data.get_size() == _leftView.get_size() ) && ( _data.get_size() == _rightView.get_size() ) );
	return ( make_pair( it, inserted ) );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::count( value_type const& value_ ) const
	{
	M_PROLOG
	typename left_view_type::const_iterator it( _leftView.find( &(value_.first) ) );
	return ( ( ( it != _leftView.end() ) && ( it->second->second == value_.second ) ) ? 1 : 0 );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
int long HTwoWayMap<left_type_t, right_type_t>::erase( value_type const& value_ )
	{
	M_PROLOG
	typename left_view_type::iterator lvit( _leftView.find( &(value_.first) ) );
	bool erased = false;
	if ( ( lvit != _leftView.end() ) && ( lvit->second->second == value_.second ) )
		{
		typename storage_type::iterator it( lvit->second );
		int long const erasedInRight( _rightView.erase( &(value_.second) ) );
		M_ASSERT( erasedInRight == 1 );
		_leftView.erase( lvit );
		_data.erase( it );
		erased = true;
		}
	M_ASSERT( ( _data.get_size() == _leftView.get_size() ) && ( _data.get_size() == _rightView.get_size() ) );
	return ( erased ? 1 : 0 );
	M_EPILOG
	}

template<typename left_type_t, typename right_type_t>
void HTwoWayMap<left_type_t, right_type_t>::erase( const_iterator const& it )
	{
	M_PROLOG
	typename left_view_type::iterator lvit( _leftView.find( &(it->first) ) );
	M_ASSERT( lvit != _leftView.end() );
	typename storage_type::iterator toErase( lvit->second );
	_leftView.erase( lvit );
	int long const erasedInRight( _rightView.erase( &(it->second) ) );
	M_ASSERT( erasedInRight == 1 );
	_data.erase( toErase );
	M_ASSERT( ( _data.get_size() == _leftView.get_size() ) && ( _data.get_size() == _rightView.get_size() ) );
	return;
	M_EPILOG
	}

}

}

#endif /* #ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED */


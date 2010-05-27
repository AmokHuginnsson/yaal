/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmultiset.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hmultiset.hxx
 * \brief Declaration of HMultiSet<> class.
 */

#ifndef YAAL_HCORE_HMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HMULTISET_HXX_INCLUDED

#include "hcore/hsbbstree.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief HSBBSTree util, a helper for HMultiSet<> instatiations.
 */
template<typename type_t>
struct multiset_helper
{

inline static bool less( HPair<type_t, int long> const& left, HPair<type_t, int long> const& right )
	{	return ( left.first < right.first );	}

inline static bool less( type_t const& left, HPair<type_t, int long> const& right )
	{	return ( left < right.first );	}

inline static bool less( HPair<type_t, int long> const& left, type_t const& right )
	{	return ( left.first < right );	}

};

/*! \brief Binary tree based set.
 *
 * HMultiSet<> is a template representing self balancing binary search tree
 * data structure that holds set of keys.
 *
 * \tparam type_t - type of values held in set.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename type_t, typename helper_t = multiset_helper<type_t> >
class HMultiSet
	{
public:
	class HIterator;
	typedef type_t value_type;
	typedef type_t key_type;
	typedef HPair<type_t, int long> elem_t;
	typedef HIterator iterator;
private:
	HSBBSTree _engine;
public:
	HMultiSet( void ) : _engine() {}
	template<typename iterator_t>
	HMultiSet( iterator_t first, iterator_t last ) : _engine()
		{
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
		}
	HMultiSet( HMultiSet const& source ) : _engine()
		{
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
		}
	HMultiSet& operator = ( HMultiSet const& multiset_ )
		{
		M_PROLOG
		if ( &multiset_ != this )
			{
			HMultiSet tmp( multiset_ );
			swap( tmp );
			}
		return ( *this );
		M_EPILOG
		}
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const
		{
		M_PROLOG
		int long sizeAcc( 0 );
		for ( HSBBSTree::HIterator it = _engine.begin(); it != _engine.end(); ++ it )
			sizeAcc += it.operator*<elem_t>().second;
		return ( sizeAcc );
		M_EPILOG
		}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	HIterator insert( value_type const& elem )
		{
		M_PROLOG
		HPair<HSBBSTree::HIterator, bool> p( _engine.insert<elem_t, helper_t>( make_pair( elem, 1 ) ) );
		if ( ! p.second )
			++ p.first.operator*<elem_t>().second;
		return ( HIterator( p.first ) );
		M_EPILOG
		}
	template<typename iter_t>
	void insert( iter_t i, iter_t endIt )
		{
		M_PROLOG
		for ( ; i != endIt; ++ i )
			insert( *i );
		return;
		M_EPILOG
		}
	int long count( value_type const& elem ) const
		{
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine.template operator*<elem_t>().second : 0 );
		M_EPILOG
		}
	int long erase( value_type const& elem )
		{
		M_PROLOG
		HIterator it( find( elem ) );
		int long erased( 0 );
		if ( it != end() )
			{
			erased = it._engine.template operator*<elem_t>().second;
			_engine.remove( it._engine );
			}
		return ( erased );
		M_EPILOG
		}
	HIterator erase( HIterator const& it )
		{
		M_PROLOG
		HIterator newIt( it );
		++ newIt;
		_engine.remove( it._engine );
		return ( newIt );
		M_EPILOG
		}
	HIterator find( value_type const& e ) const
		{ return ( HIterator( _engine.find<elem_t, value_type, helper_t>( e ) ) ); }
	HIterator begin( void ) const
		{ return ( HIterator( _engine.begin() ) ); }
	HIterator end( void ) const
		{ return ( HIterator( _engine.end() ) ); }
	HIterator rbegin( void ) const
		{ return ( HIterator( _engine.rbegin() ) ); }
	HIterator rend( void ) const
		{ return ( HIterator( _engine.rend() ) ); }
	void clear( void )
		{ _engine.clear(); }
	void swap( HMultiSet<value_type, helper_t>& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			_engine.swap( other._engine );
			}
		}
	bool operator == ( HMultiSet const& set_ ) const
		{ M_PROLOG return ( equals( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HMultiSet const& set_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	};

/*! \brief Iterator for HMultiSet<> data structure.
 */
template<typename value_type, typename helper_t = multiset_helper<value_type> >
class HMultiSet<value_type, helper_t>::HIterator
	{
	typedef HPair<value_type, int long> elem_t;
	int long _index;
	HSBBSTree::HIterator _engine;
public:
	HIterator( void ) : _index( 0 ), _engine() {}
	HIterator( HIterator const& it_ ) : _index( it_._index ), _engine( it_._engine ) {}
	HIterator& operator= ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_index = it_._index;
			_engine = it_._engine;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		if ( _index < ( _engine.operator*<elem_t>().second - 1 ) )
			++ _index;
		else
			{
			_index = 0;
			++ _engine;
			}
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( _engine );
		++ _engine;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- _engine;
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( _engine );
		-- _engine;
		return ( it );
		}
	value_type const& operator * ( void )
		{	return ( _engine.operator*<elem_t>().first );	}
	value_type const* operator -> ( void )
		{ return ( &_engine.operator*<elem_t>().first );	}
	bool operator == ( HIterator const& it ) const
		{ return ( ( _engine == it._engine ) && ( _index == it._index )  ); }
	bool operator != ( HIterator const& it ) const
		{ return ( ( _engine != it._engine ) || ( _index != it._index ) ); }
private:
	friend class HMultiSet<value_type, helper_t>;
	explicit HIterator( HSBBSTree::HIterator const& it ) : _index( 0 ), _engine( it ) {};
	};

}

template<typename value_type, typename helper_t>
inline void swap( yaal::hcore::HMultiSet<value_type, helper_t>& a, yaal::hcore::HMultiSet<value_type, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HMULTISET_HXX_INCLUDED */


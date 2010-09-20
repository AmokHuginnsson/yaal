/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hset.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hset.hxx
 * \brief Declaration of HSet<> class.
 */

#ifndef YAAL_HCORE_HSET_HXX_INCLUDED
#define YAAL_HCORE_HSET_HXX_INCLUDED

#include "hcore/hsbbstree.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief HSBBSTree util, a helper for HSet<> instatiations.
 */
template<typename type_t>
struct set_helper
{

template<typename key_t>
inline static bool less( key_t const& left, key_t const& right )
	{	return ( left < right );	}

};

/*! \brief Binary tree based set.
 *
 * HSet<> is a template representing self balancing binary search tree
 * data structure that holds set of keys.
 *
 * \tparam type_t - type of values held in set.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename type_t, typename helper_t = set_helper<type_t> >
class HSet
	{
public:
	class HIterator;
	typedef type_t value_type;
	typedef type_t key_type;
	typedef HIterator iterator;
private:
	HSBBSTree _engine;
public:
	HSet( void ) : _engine() {}
	template<typename iterator_t>
	HSet( iterator_t first, iterator_t last ) : _engine()
		{
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
		}
	HSet( HSet const& source )
		{
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
		}
	HSet& operator = ( HSet const& set_ )
		{
		M_PROLOG
		if ( &set_ != this )
			{
			HSet tmp( set_ );
			swap( tmp );
			}
		return ( *this );
		M_EPILOG
		}
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const
		{ return ( _engine.get_size() );	}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	HPair<HIterator, bool> insert( value_type const& elem )
		{
		M_PROLOG
		HPair<HSBBSTree::HIterator, bool> p = _engine.insert<value_type, helper_t>( elem );
		return ( make_pair( HIterator( p.first ), p.second ) );
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
		return ( it != end() ? 1 : 0 );
		M_EPILOG
		}
	int long erase( value_type const& elem )
		{
		M_PROLOG
		HIterator it( find( elem ) );
		int long erased( 0 );
		if ( it != end() )
			{
			_engine.remove( it._engine );
			erased = 1;
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
	void erase( HIterator first_, HIterator const& last_ )
		{
		M_PROLOG
		while ( first_ != last_ )
			first_ = erase( first_ );
		return;
		M_EPILOG
		}
	HIterator find( value_type const& e ) const
		{ return ( HIterator( _engine.find<value_type, value_type, helper_t>( e ) ) ); }
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
	void swap( HSet<value_type, helper_t>& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			_engine.swap( other._engine );
			}
		}
	bool operator == ( HSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HSet const& set_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	};

/*! \brief Iterator for HSet<> data structure.
 */
template<typename value_type, typename helper_t = set_helper<value_type> >
class HSet<value_type, helper_t>::HIterator
	{
	HSBBSTree::HIterator _engine;
public:
	HIterator( void ) : _engine() {}
	HIterator( HIterator const& it_ ) : _engine( it_._engine ) {}
	HIterator& operator= ( HIterator const& it_ )
		{
		if ( &it_ != this )
			_engine = it_._engine;
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ _engine;
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
		{	return ( _engine.get<value_type>() );	}
	value_type const* operator -> ( void )
		{ return ( &_engine.get<value_type>() );	}
	bool operator == ( HIterator const& it ) const
		{ return ( _engine == it._engine ); }
	bool operator != ( HIterator const& it ) const
		{ return ( _engine != it._engine ); }
	bool operator == ( HSet const& set_ ) const
		{ M_PROLOG return ( equals( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HSet const& set_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
private:
	friend class HSet<value_type, helper_t>;
	explicit HIterator( HSBBSTree::HIterator const& it ) : _engine( it ) {};
	};

}

template<typename value_type, typename helper_t>
inline void swap( yaal::hcore::HSet<value_type, helper_t>& a, yaal::hcore::HSet<value_type, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HSET_HXX_INCLUDED */


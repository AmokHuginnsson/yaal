/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmultimap.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HMULTIMAP_HXX_INCLUDED
#define YAAL_HCORE_HMULTIMAP_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hmulticontainer.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Binary tree based map with relation one-to-many on keys to values.
 *
 * HMultiMap<> is a template representing self balancing binary search tree
 * data structure that holds pairs of keys and lists of values.
 *
 * \tparam key_type_t - type of key held in map.
 * \tparam value_type_t - type of value held in map.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename key_type_t, typename value_type_t,
	template<typename, typename>class storage_policy_t = HMultiContainerStorage::HTransparent,
	typename helper_t = map_helper<key_type_t const, HPointer<HList<typename storage_policy_t<key_type_t const, value_type_t>::stored_type > > > >
class HMultiMap
	{
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef storage_policy_t<key_type const, data_type> storage_t;
public:
	typedef HList<typename storage_t::stored_type> value_list_t;
	typedef HPointer<value_list_t> value_list_ptr_t;
	typedef HPair<key_type const, data_type> value_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
private:
	typedef HMap<key_type, value_list_ptr_t, helper_t> multimap_engine_t;
	multimap_engine_t _engine;
public:
	HMultiMap( void ) : _engine() {}
	template<typename iterator_t>
	HMultiMap( iterator_t first, iterator_t last ) : _engine()
		{
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
		}
	HMultiMap( HMultiMap const& multimap_ ) : _engine()
		{
		M_PROLOG
		_engine.copy_from( multimap_._engine );
		return;
		M_EPILOG
		}
	HMultiMap& operator = ( HMultiMap const& multimap_ )
		{
		M_PROLOG
		if ( &multimap_ != this )
			{
			HMultiMap tmp( multimap_ );
			swap( multimap_ );
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
		for ( typename multimap_engine_t::const_iterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it->second->get_size();
		return ( sizeAcc );
		M_EPILOG
		}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	iterator push_back( key_type const& key, data_type const& value )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( key );
		major->second->push_back( storage_t::value( key, value ) );
		typename value_list_t::reverse_iterator minor = major->second->rbegin();
		return ( iterator( this, major, minor.base() ) );
		M_EPILOG
		}
	iterator insert( value_type const& e )
		{
		typename multimap_engine_t::iterator major = ensure_key( e.first );
		major->second->push_back( storage_t::value( e ) );
		typename value_list_t::iterator minor = major->second->rbegin().base();
		return ( iterator( this, major, minor ) );
		}
	template<typename iterator_t>
	void insert( iterator_t first, iterator_t last )
		{
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	template<typename iter_t>
	void push_back( iter_t i, iter_t endIt )
		{
		M_PROLOG
		for ( ; i != endIt; ++ i )
			push_back( *i );
		return;
		M_EPILOG
		}
	iterator push_front( key_type const& key, data_type const& value )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( key );
		major->second->push_front( storage_t::value( key, value ) );
		typename value_list_t::iterator minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	template<typename iter_t>
	void push_front( iter_t i, iter_t endIt )
		{
		M_PROLOG
		for ( ; i != endIt; ++ i )
			push_front( *i );
		return;
		M_EPILOG
		}
	/*! \brief Erase all elements of given \e key.
	 *
	 * \param key_ - erase elements with this key.
	 * \return number of elements actualy erased.
	 */
	int long erase( key_type const& key_ )
		{
		M_PROLOG
		iterator it = find( key_ );
		int long erased( 0 );
		if ( it != end() )
			{
			erased = it._major->second->get_size();
			_engine.erase( it._major );
			}
		return ( erased );
		M_EPILOG
		}
	iterator erase( iterator& it )
		{
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		value_list_ptr_t list = it._major->second;
		list->erase( it._minor );
		if ( ! list->size() )
			_engine.erase( it._major );
		return ( newIt );
		M_EPILOG
		}
	void erase( iterator first_, iterator const& last_ )
		{
		M_PROLOG
		while ( first_ != last_ )
			first_ = erase( first_ );
		return;
		M_EPILOG
		}
	const_iterator find( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.find( key );
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator lower_bound( key_type const& key_ ) const
		{ M_PROLOG return ( find( key_ ) ); M_EPILOG }
	iterator lower_bound( key_type const& key_ )
		{ M_PROLOG return ( find( key_ ) ); M_EPILOG }
	iterator find( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.find( key );
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator upper_bound( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.find( key );
		if ( major != _engine.end() )
			++ major;
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator upper_bound( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.find( key );
		if ( major != _engine.end() )
			++ major;
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator begin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.begin();
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator begin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.begin();
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator end( void ) const
		{ return ( const_iterator( this, _engine.end(), typename value_list_t::const_iterator() ) ); }
	iterator end( void )
		{ return ( iterator( this, _engine.end(), typename value_list_t::iterator() ) ); }
	const_iterator rbegin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.rbegin();
		typename value_list_t::const_iterator minor;
		if ( major != _engine.end() )
			minor = major->second->rbegin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator rbegin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.rbegin();
		typename value_list_t::iterator minor;
		if ( major != _engine.end() )
			minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator rend( void ) const
		{ return ( const_iterator( this, _engine.rend(), typename value_list_t::const_iterator() ) ); }
	iterator rend( void )
		{ return ( iterator( this, _engine.rend(), typename value_list_t::iterator() ) ); }
	void clear( void )
		{ _engine.clear(); }
	int long count( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = _engine.find( key );
		int long cnt = 0;
		if ( major != _engine.end() )
			cnt = major->second->size();
		return ( cnt );
		M_EPILOG
		}
	void swap( HMultiMap& multimap_ )
		{
		if ( &multimap_ != this )
			{
			using yaal::swap;
			swap( _engine, multimap_._engine );
			}
		return;
		}
	bool operator == ( HMultiMap const& map_ ) const
		{ M_PROLOG return ( ( &map_ == this ) || safe_equal( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
	bool operator < ( HMultiMap const& map_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
private:
	typename multimap_engine_t::iterator ensure_key( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = _engine.find( key );
		if ( major == _engine.end() )
			{
			value_list_ptr_t list = value_list_ptr_t( new value_list_t() );
			major = _engine.insert( make_pair( key, list ) ).first;
			}
		return ( major );
		M_EPILOG
		}
	};

/*! \brief Forward iterator for HMultiMap<>.
 */
template<typename key_type_t, typename value_type_t, template<typename, typename> class storage_policy_t, typename helper_t>
template<typename const_qual_t>
class HMultiMap<key_type_t, value_type_t, storage_policy_t, helper_t>::HIterator
	{
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HMultiMap<key_type, data_type, storage_policy_t, helper_t> multi_map_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename multi_map_t::multimap_engine_t::const_iterator,
					typename multi_map_t::multimap_engine_t::iterator>::type key_iterator_t;
	typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
					typename value_list_t::const_iterator,
					typename value_list_t::iterator>::type value_iterator_t;
	multi_map_t const* _owner;
	key_iterator_t _major;
	value_iterator_t _minor;
public:
	HIterator( void ) : _owner( NULL ), _major(), _minor() {}
	HIterator( HIterator const& it_ ) : _owner( it_._owner ), _major( it_._major ), _minor( it_._minor ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : _owner( it_._owner ), _major( it_._major ), _minor( it_._minor )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator = ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_owner = it_._owner;
			_major = it_._major;
			_minor = it_._minor;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		M_PROLOG
		++ _minor;
		if ( _minor == _major->second->end() )
			{
			++ _major;
			if ( _major != _owner->_engine.end() )
				_minor = _major->second->begin();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( _owner, _major, _minor );
		operator++();
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		M_PROLOG
		-- _minor;
		if ( _minor == _major->second.rend().base() )
			{
			-- _major;
			if ( _major != _owner->_engine.rend().base() )
				_minor = _major->second->rbegin().base();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator -- ( int )
		{
		HIterator it( _owner, _major, _minor );
		operator--();
		return ( it );
		}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void )
		{	return ( multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void ) const
		{	return ( multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void )
		{	return ( &multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void ) const
		{	return ( &multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( _major->first, *_minor ) );	}
	bool operator == ( HIterator const& it ) const
		{ return ( ( _major == it._major ) && ( _minor == it._minor ) ); }
	bool operator != ( HIterator const& it ) const
		{ return ( ! ( ( _major == it._major ) && ( _minor == it._minor ) ) ); }
private:
	friend class HMultiMap<key_type, data_type, storage_policy_t, helper_t>;
	explicit HIterator( multi_map_t const* const owner_,
			key_iterator_t const& major,
			value_iterator_t const& minor ) : _owner( owner_ ), _major( major ), _minor( minor ) {};
	};

}

template<typename value_type, typename helper_t>
inline void swap( yaal::hcore::HMultiMap<value_type, helper_t>& a, yaal::hcore::HMultiMap<value_type, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HMULTIMAP_HXX_INCLUDED */


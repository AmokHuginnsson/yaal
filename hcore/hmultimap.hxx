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
#define YAAL_HCORE_HMULTIMAP_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hlist.hxx"

namespace yaal
{

namespace hcore
{

struct HMultiMapStorage
	{
	template<typename key_type_t, typename value_type_t>
	struct HPacked
		{
		typedef value_type_t data_type;
		typedef data_type stored_type;
		typedef yaal::hcore::HPair<key_type_t, value_type_t> value_type;
		typedef yaal::hcore::HPair<key_type_t, trait::reference<value_type_t> > accessor_elem_t;
		template<typename const_qual_t>
		struct const_aware_type
			{
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const,
							accessor_elem_t>::type accessor_t;
			typedef accessor_t accessor_ptr_t;
			template<typename key_provider_t, typename value_provider_t>
			static accessor_t accessor( key_provider_t const& key_, value_provider_t& val_ )
				{ return ( accessor_t( key_, ref( val_ ) ) ); }
			};
		static data_type value( key_type_t const&, value_type_t const& val_ )
			{ return ( val_ ); }
		static data_type value( value_type const& elem_ )
			{ return ( elem_.second ); }
		};
	template<typename key_type_t, typename value_type_t>
	struct HTransparent
		{
		typedef yaal::hcore::HPair<key_type_t, value_type_t> value_type;
		typedef value_type stored_type;
		template<typename const_qual_t>
		struct const_aware_type
			{
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const&,
							value_type&>::type accessor_t;
			typedef typename trait::ternary<trait::same_type<const_qual_t, const_qual_t const>::value,
							value_type const*,
							value_type*>::type accessor_ptr_t;
			template<typename key_provider_t, typename value_provider_t>
			static accessor_t accessor( key_provider_t const&, value_provider_t& val_ )
				{ return ( val_ ); }
			};
		static value_type value( key_type_t const& key_, value_type_t const& val_ )
			{ return ( value_type( key_, val_ ) ); }
		static value_type value( value_type const& elem_ )
			{ return ( elem_ ); }
		};
	};

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
	template<typename, typename>class storage_policy_t = HMultiMapStorage::HTransparent,
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
	multimap_engine_t f_oEngine;
public:
	HMultiMap( void ) : f_oEngine() {};
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const
		{
		M_PROLOG
		int long l_iSize = 0;
		for ( typename multimap_engine_t::const_iterator it = f_oEngine.begin(); it != f_oEngine.end(); ++ it )
			l_iSize += it->second->get_size();
		return ( l_iSize );
		M_EPILOG
		}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( f_oEngine.is_empty() );	}
	iterator push_back( key_type const& key, data_type const& value )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( key );
		major->second->push_back( storage_t::value( key, value ) );
		typename value_list_t::iterator minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator insert( value_type const& e )
		{
		typename multimap_engine_t::iterator major = ensure_key( e.first );
		major->second->push_back( storage_t::value( e ) );
		typename value_list_t::iterator minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
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
	int long count( key_type const& key )
		{
		M_PROLOG
		iterator it = find( key );
		int long num( 0 );
		if ( it != end() )
			num = it.f_oMajor->second->get_size();
		return ( num );
		M_EPILOG
		}
	int long erase( key_type const& key )
		{
		M_PROLOG
		iterator it = find( key );
		int long erased( 0 );
		if ( it != end() )
			{
			erased = it.f_oMajor->second->get_size();
			f_oEngine.erase( it.f_oMajor );
			}
		return ( erased );
		M_EPILOG
		}
	iterator erase( iterator& it )
		{
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		value_list_ptr_t list = it.f_oMajor->second;
		list->erase( it.f_oMinor );
		if ( ! list->size() )
			f_oEngine.erase( it.f_oMajor );
		return ( newIt );
		M_EPILOG
		}
	const_iterator find( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.find( key );
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator find( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator upper_bound( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.find( key );
		if ( major != f_oEngine.end() )
			++ major;
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator upper_bound( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		if ( major != f_oEngine.end() )
			++ major;
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator begin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.begin();
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator begin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.begin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator end( void ) const
		{ return ( const_iterator( this, f_oEngine.end(), typename value_list_t::const_iterator() ) ); }
	iterator end( void )
		{ return ( iterator( this, f_oEngine.end(), typename value_list_t::iterator() ) ); }
	const_iterator rbegin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.rbegin();
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator rbegin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.rbegin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator rend( void ) const
		{ return ( const_iterator( this, f_oEngine.rend(), typename value_list_t::const_iterator() ) ); }
	iterator rend( void )
		{ return ( iterator( this, f_oEngine.rend(), typename value_list_t::iterator() ) ); }
	void clear( void )
		{ f_oEngine.clear(); }
	int long count( key_type const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.find( key );
		int long cnt = 0;
		if ( major != f_oEngine.end() )
			cnt = major->second->size();
		return ( cnt );
		M_EPILOG
		}

private:
	typename multimap_engine_t::iterator ensure_key( key_type const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		if ( major == f_oEngine.end() )
			{
			value_list_ptr_t list = value_list_ptr_t( new value_list_t() );
			major = f_oEngine.insert( make_pair( key, list ) ).first;
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
	multi_map_t const* f_poOwner;
	key_iterator_t f_oMajor;
	value_iterator_t f_oMinor;
public:
	HIterator( void ) : f_poOwner( NULL ), f_oMajor(), f_oMinor() {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& a_oIt ) : f_poOwner( a_oIt.f_poOwner ), f_oMajor( a_oIt.f_oMajor ), f_oMinor( a_oIt.f_oMinor )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator = ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			{
			f_poOwner = a_oIt.f_poOwner;
			f_oMajor = a_oIt.f_oMajor;
			f_oMinor = a_oIt.f_oMinor;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		M_PROLOG
		++ f_oMinor;
		if ( ! f_oMinor.is_valid() )
			{
			++ f_oMajor;
			if ( f_oMajor != f_poOwner->f_oEngine.end() )
				f_oMinor = f_oMajor->second->begin();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( f_poOwner, f_oMajor, f_oMinor );
		operator++();
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		M_PROLOG
		-- f_oMinor;
		if ( ! f_oMinor.is_valid() )
			{
			-- f_oMajor;
			if ( f_oMajor != f_poOwner->f_oEngine.rend() )
				f_oMinor = f_oMajor->second->rbegin();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator -- ( int )
		{
		HIterator it( f_poOwner, f_oMajor, f_oMinor );
		-- f_oEngine;
		return ( it );
		}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void )
		{	return ( multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( f_oMajor->first, *f_oMinor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_t operator* ( void ) const
		{	return ( multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( f_oMajor->first, *f_oMinor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void )
		{	return ( &multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( f_oMajor->first, *f_oMinor ) );	}
	typename multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor_ptr_t operator->( void ) const
		{	return ( &multi_map_t::storage_t::template const_aware_type<const_qual_t>::accessor( f_oMajor->first, *f_oMinor ) );	}
	bool operator == ( HIterator const& it ) const
		{ return ( ( f_oMajor == it.f_oMajor ) && ( f_oMinor == it.f_oMinor ) ); }
	bool operator != ( HIterator const& it ) const
		{ return ( ! ( ( f_oMajor == it.f_oMajor ) && ( f_oMinor == it.f_oMinor ) ) ); }
private:
	friend class HMultiMap<key_type, data_type, storage_policy_t, helper_t>;
	explicit HIterator( multi_map_t const* const a_poOwner,
			key_iterator_t const& major,
			value_iterator_t const& minor ) : f_poOwner( a_poOwner ), f_oMajor( major ), f_oMinor( minor ) {};
	};

}

}

#endif /* not YAAL_HCORE_HMULTIMAP_HXX_INCLUDED */


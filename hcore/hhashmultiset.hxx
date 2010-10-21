/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashmultiset.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED 1

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Hash set container implementation.
 */
template<typename type_t, typename hash_function_t = int long(*)( type_t const& )>
class HHashMultiSet
	{
public:
	class HIterator;
	typedef type_t key_type;
	typedef type_t value_type;
	typedef HPair<type_t, int long> elem_t;
	typedef HIterator iterator;
private:
	struct hasher
		{
		typedef typename HHashMultiSet<type_t, hash_function_t>::elem_t value_type;
		hash_function_t _hasher;
		hasher( hash_function_t hashFunction_ ) : _hasher( hashFunction_ ) {}
		int long operator()( value_type const& val_ ) const
			{ return ( _hasher( val_.first ) ); }
		int long operator()( type_t const& key_ ) const
			{ return ( _hasher( key_ ) ); }
		bool operator()( value_type const& a_, value_type const& b_ ) const
			{ return ( a_.first == b_.first ); }
		bool operator()( value_type const& a_, type_t const& b_ ) const
			{ return ( a_.first == b_ ); }
		};
	typedef HHashMultiSet<type_t, hash_function_t> this_type;
	hasher _hasher;
	HHashContainer _engine;
public:
	HHashMultiSet( void )
		: _hasher( &hash ), _engine()
		{}
	/*! \brief Lower bound of size of map's table */
	HHashMultiSet( int long size_ )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
		}
	HHashMultiSet( int long size_, hash_function_t hasher_ )
		: _hasher(  hasher_ ), _engine()
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
		}
	template<typename iterator_t>
	HHashMultiSet( iterator_t first, iterator_t last )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	template<typename iterator_t>
	HHashMultiSet( iterator_t first, iterator_t last, int long size_ )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		resize( size_, _hasher );
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	HHashMultiSet( HHashMultiSet const& set_ )
		: _hasher( set_._hasher ), _engine()
		{
		M_PROLOG
		_engine.copy_from( set_._engine );
		return;
		M_EPILOG
		}
	virtual ~HHashMultiSet( void )
		{
		M_PROLOG
		clear();
		return;
		M_EPILOG
		}
	HHashMultiSet& operator = ( HHashMultiSet const& set_ )
		{
		M_PROLOG
		int i( 0 );
		if ( &set_ != this )
			{
			HHashMultiSet tmp( set_ );
			swap( tmp );
			}
		return ( *this );
		M_EPILOG
		}
	iterator begin( void ) const
		{ M_PROLOG return ( iterator( _engine.begin() ) ); M_EPILOG }
	iterator end( void ) const
		{ M_PROLOG return ( iterator( _engine.end() ) ); M_EPILOG }
	iterator find( type_t const& key_ ) const
		{ M_PROLOG return ( iterator( _engine.find( key_, _hasher ) ) ); M_EPILOG }
	iterator insert( value_type const& val_ )
		{
		M_PROLOG
		HPair<HHashContainer::HIterator, bool> p( _engine.insert( make_pair( val_, 1L ), _hasher ) );
		if ( ! p.second )
			++ p.first.get<elem_t>().second;
		return ( iterator( p.first ) );
		M_EPILOG
		}
	void resize( int long size_ )
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
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
	void erase( iterator it )
		{
		M_PROLOG
		_engine.erase( it._engine );
		return;
		M_EPILOG
		}
	/*! \brief Remove given key from map.
	 *
	 * \param key_ - key to be removed.
	 * \return Number of erased elements.
	 */
	int long erase( type_t const& key_ )
		{
		M_PROLOG
		iterator it( find( key_ ) );
		bool erased( it != end() );
		if ( erased )
			erase( it );
		return ( erased ? 1 : 0 );
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
	int long count( value_type const& elem ) const
		{
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine.template get<elem_t>().second : 0 );
		M_EPILOG
		}
	void clear( void )
		{ M_PROLOG _engine.clear(); return; M_EPILOG }
	int long get_size( void ) const
		{
		M_PROLOG
		int long sizeAcc( 0 );
		for ( HHashContainer::HIterator it = _engine.begin(); it != _engine.end(); ++ it )
			sizeAcc += it.get<elem_t>().second;
		return ( sizeAcc );
		M_EPILOG
		}
	int long size( void ) const
		{ M_PROLOG return ( get_size() ); M_EPILOG }
	bool is_empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	bool empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	void swap( HHashMultiSet& set_ )
		{
		if ( &set_ != this )
			{
			using yaal::swap;
			swap( _engine, set_._engine );
			swap( _hasher, set_._hasher );
			}
		return;
		}
	bool operator == ( HHashMultiSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HHashMultiSet const& set_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
private:
	};


template<typename key_type_t, typename hash_function_t>
class HHashMultiSet<key_type_t, hash_function_t>::HIterator
	{
	typedef key_type_t key_type;
	typedef HHashMultiSet<key_type, hash_function_t> set_t;
	int long _index;
	HHashContainer::HIterator _engine;
public:
	HIterator( void ) : _index( 0 ), _engine() {}
	HIterator( HIterator const& it_ ) : _index( it_._index ), _engine( it_._engine ) {}
	HIterator& operator = ( HIterator const& it_ )
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
		if ( _index < ( _engine.get<typename set_t::elem_t>().second - 1 ) )
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
	key_type const& operator* ( void ) const
		{ return ( _engine.get<typename set_t::elem_t>().first ); }
	key_type const* operator-> ( void ) const
		{ return ( &_engine.get<typename set_t::elem_t>().first ); }
	bool operator == ( HIterator const& it ) const
		{ return ( ( _engine == it._engine ) && ( _index == it._index )  ); }
	bool operator != ( HIterator const& it ) const
		{ return ( ( _engine != it._engine ) || ( _index != it._index ) ); }
private:
	friend class HHashMultiSet<key_type, hash_function_t>;
	explicit HIterator( HHashContainer::HIterator const& it ) : _index( 0 ), _engine( it ) {};
	};

}

template<typename key_type, typename hash_function_t>
inline void swap( yaal::hcore::HHashMultiSet<key_type, hash_function_t>& a, yaal::hcore::HHashMultiSet<key_type, hash_function_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED */


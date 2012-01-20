/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashset.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HHASHSET_HXX_INCLUDED
#define YAAL_HCORE_HHASHSET_HXX_INCLUDED 1

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace hcore {

/*! \brief Hash set container implementation.
 */
template<typename type_t, typename hash_function_t = int long(*)( type_t const& )>
class HHashSet {
public:
	typedef type_t key_type;
/* cppcheck-suppress variableHidingTypedef */
	typedef type_t value_type;
	class HIterator : public iterator_interface<type_t, iterator_category::forward> {
		typedef type_t key_type;
		typedef HHashSet<key_type, hash_function_t> set_t;
		HHashContainer::HIterator _engine;
	public:
		typedef iterator_interface<type_t, iterator_category::forward> base_type;
		HIterator( void ) : base_type(), _engine() {}
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
			HIterator it( _engine );
			++ _engine;
			return ( it );
		}
		HIterator& operator -- ( void ) {
			-- _engine;
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( _engine );
			-- _engine;
			return ( it );
		}
		key_type const& operator* ( void ) const
			{ return ( _engine.get<typename set_t::value_type>() ); }
		key_type const* operator-> ( void ) const
			{ return ( &_engine.get<typename set_t::value_type>() ); }
		bool operator == ( HIterator const& it ) const
			{ return ( _engine == it._engine ); }
		bool operator != ( HIterator const& it ) const
			{ return ( _engine != it._engine ); }
	private:
		friend class HHashSet<key_type, hash_function_t>;
		explicit HIterator( HHashContainer::HIterator const& it ) : base_type(), _engine( it ) {};
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	typedef HPair<iterator, bool> insert_result;
private:
	struct hasher {
		typedef typename HHashSet<type_t, hash_function_t>::value_type value_type;
		hash_function_t _hasher;
		hasher( hash_function_t hashFunction_ ) : _hasher( hashFunction_ ) {}
		int long operator()( value_type const& val_ ) const
			{ return ( _hasher( val_ ) ); }
		bool operator()( value_type const& a_, value_type const& b_ ) const
			{ return ( a_ == b_ ); }
	};
	typedef HHashSet<type_t, hash_function_t> this_type;
	hasher _hasher;
	HHashContainer _engine;
public:
	HHashSet( void )
		: _hasher( &hash ), _engine()
		{}
	/*! \brief Lower bound of size of map's table */
	HHashSet( int long size_ )
		: _hasher( &hash ), _engine() {
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
	}
	HHashSet( int long size_, hash_function_t hasher_ )
		: _hasher( hasher_ ), _engine() {
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashSet( iterator_t first, iterator_t last )
		: _hasher( &hash ), _engine() {
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashSet( iterator_t first, iterator_t last, int long size_ )
		: _hasher( &hash ), _engine() {
		M_PROLOG
		resize( size_, _hasher );
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	HHashSet( HHashSet const& set_ )
		: _hasher( set_._hasher ), _engine() {
		M_PROLOG
		_engine.copy_from( set_._engine );
		return;
		M_EPILOG
	}
	virtual ~HHashSet( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HHashSet& operator = ( HHashSet const& set_ ) {
		M_PROLOG
		int i( 0 );
		if ( &set_ != this ) {
			HHashSet tmp( set_ );
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
	insert_result insert( value_type const& val_ ) {
		M_PROLOG
		HPair<HHashContainer::HIterator, bool> it( _engine.insert( val_, _hasher ) );
		return ( make_pair( iterator( it.first ), it.second ) );
		M_EPILOG
	}
	void resize( int long size_ ) {
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void insert( iterator_t first, iterator_t last ) {
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	void erase( iterator it ) {
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
	int long erase( type_t const& key_ ) {
		M_PROLOG
		iterator it( find( key_ ) );
		bool erased( it != end() );
		if ( erased )
			erase( it );
		return ( erased ? 1 : 0 );
		M_EPILOG
	}
	void erase( iterator first_, iterator const& last_ ) {
		M_PROLOG
		while ( first_ != last_ )
			first_ = erase( first_ );
		return;
		M_EPILOG
	}
	int long count( type_t const& key_ ) const
		{ M_PROLOG return ( find( key_ ) != end() ? 1 : 0 ); M_EPILOG }
	void clear( void )
		{ M_PROLOG _engine.clear(); return; M_EPILOG }
	int long get_size( void ) const
		{ M_PROLOG return ( _engine.get_size() ); M_EPILOG }
	int long size( void ) const
		{ M_PROLOG return ( _engine.get_size() ); M_EPILOG }
	bool is_empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	bool empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	void swap( HHashSet& set_ ) {
		if ( &set_ != this ) {
			using yaal::swap;
			swap( _engine, set_._engine );
			swap( _hasher, set_._hasher );
		}
		return;
	}
	bool operator == ( HHashSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HHashSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ != this ) && lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
private:
};

}

template<typename key_type, typename hash_function_t>
inline void swap( yaal::hcore::HHashSet<key_type, hash_function_t>& a, yaal::hcore::HHashSet<key_type, hash_function_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HHASHSET_HXX_INCLUDED */


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashmultiset.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED 1

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

/*! \brief HHashContainer util, a helper for HHashSet<> instatiations.
 */
template<typename type_t>
struct hashmultiset_helper {
	typedef typename type_t::first_type key_type;
	inline static key_type const& key( type_t const& key_ )
		{	return ( key_.first );	}
};

/*! \brief Hash set container implementation.
 */
template<typename type_t, typename hasher_t = hash<type_t>, typename allocator_t = allocator::system<HPair<type_t, int long> > >
class HHashMultiSet {
public:
	typedef type_t key_type;
/* cppcheck-suppress variableHidingTypedef */
	typedef type_t value_type;
	typedef hasher_t hasher_type;
	typedef HPair<type_t, int long> elem_t;
	typedef HHashContainer<elem_t, hasher_type, hashmultiset_helper<elem_t>, allocator_t> engine_t;
	typedef typename engine_t::allocator_type allocator_type;
	typedef HHashMultiSet<value_type, hasher_type, allocator_t> this_type;
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		int long _index;
		engine_t const* _owner;
		typename engine_t::HIterator _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void )
			: base_type(), _index( 0 ), _owner( NULL ), _engine()
			{}
		HIterator( HIterator const& it_ )
			: base_type(), _index( it_._index ), _owner( it_._owner ), _engine( it_._engine )
			{}
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_index = it_._index;
				_engine = it_._engine;
			}
			return ( *this );
		}
		HIterator& operator ++ ( void ) {
			if ( _index < ( _engine.get().second - 1 ) )
				++ _index;
			else {
				_index = 0;
				++ _engine;
			}
			return ( *this );
		}
		HIterator const operator ++ ( int ) {
			HIterator it( *this );
			operator ++ ();
			return ( it );
		}
		HIterator& operator -- ( void ) {
			if ( _index > 0 ) {
				-- _index;
			} else {
				-- _engine;
				if ( _engine != _owner->end() )
					_index = _engine.get().second - 1;
			}
			return ( *this );
		}
		HIterator const operator -- ( int ) {
			HIterator it( *this );
			operator -- ();
			return ( it );
		}
		key_type const& operator* ( void ) const
			{ return ( _engine.get().first ); }
		key_type const* operator-> ( void ) const
			{ return ( &_engine.get().first ); }
		bool operator == ( HIterator const& it ) const
			{ return ( ( _engine == it._engine ) && ( _index == it._index )  ); }
		bool operator != ( HIterator const& it ) const
			{ return ( ( _engine != it._engine ) || ( _index != it._index ) ); }
	private:
		friend class HHashMultiSet<key_type, hasher_type, allocator_t>;
		explicit HIterator( engine_t const* owner_, typename engine_t::HIterator const& it, int long index_ )
			: base_type(), _index( index_ ), _owner( owner_ ), _engine( it )
			{}
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	engine_t _engine;
public:
	HHashMultiSet( void )
		: _engine( hasher_type(), allocator_type() )
		{}
	explicit HHashMultiSet( hasher_type const& hasher_ )
		: _engine( hasher_, allocator_type() )
		{}
	explicit HHashMultiSet( allocator_type const& allocator_ )
		: _engine( hasher_type(), allocator_ )
		{}
	/*! \brief Lower bound of size of map's table */
	explicit HHashMultiSet( int long size_ )
		: _engine( hasher_type(), allocator_type() ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	HHashMultiSet( int long size_, hasher_type const& hasher_, allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, allocator_ ) {
		M_PROLOG
		_engine.resize( size_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashMultiSet( iterator_t first, iterator_t last, hasher_type const& hasher_ = hasher_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, allocator_ ) {
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HHashMultiSet( iterator_t first, iterator_t last, int long size_, hasher_type const& hasher_ = hasher_type(), allocator_type const& allocator_ = allocator_type() )
		: _engine( hasher_, allocator_ ) {
		M_PROLOG
		resize( size_ );
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
	}
	HHashMultiSet( HHashMultiSet const& set_ )
		: _engine( set_._engine.hasher(), set_._engine.get_allocator() ) {
		M_PROLOG
		_engine.copy_from( set_._engine );
		return;
		M_EPILOG
	}
	virtual ~HHashMultiSet( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HHashMultiSet& operator = ( HHashMultiSet const& set_ ) {
		M_PROLOG
		int i( 0 );
		if ( &set_ != this ) {
			HHashMultiSet tmp( set_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _engine.get_allocator() );
	}
	iterator begin( void ) const {
		M_PROLOG
		return ( iterator( &_engine, _engine.begin(), 0 ) );
		M_EPILOG
	}
	iterator end( void ) const {
		M_PROLOG
		return ( iterator( &_engine, _engine.end(), 0 ) );
		M_EPILOG
	}
	iterator find( type_t const& key_ ) const {
		M_PROLOG
		return ( iterator( &_engine, _engine.find( key_ ), 0 ) );
		M_EPILOG
	}
	iterator insert( value_type const& val_ ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( val_, 1L ) ) );
		if ( ! p.second )
			++ p.first.get().second;
		return ( iterator( &_engine, p.first, p.first.get().second - 1 ) );
		M_EPILOG
	}
	void resize( int long size_ ) {
		M_PROLOG
		_engine.resize( size_ );
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
	int long count( value_type const& elem ) const {
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine.get().second : 0 );
		M_EPILOG
	}
	void clear( void )
		{ M_PROLOG _engine.clear(); return; M_EPILOG }
	int long get_size( void ) const {
		M_PROLOG
		int long sizeAcc( 0 );
		for ( typename engine_t::HIterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it.get().second;
		return ( sizeAcc );
		M_EPILOG
	}
	int long size( void ) const
		{ M_PROLOG return ( get_size() ); M_EPILOG }
	bool is_empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	bool empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	void swap( HHashMultiSet& set_ ) {
		if ( &set_ != this ) {
			using yaal::swap;
			swap( _engine, set_._engine );
		}
		return;
	}
	bool operator == ( HHashMultiSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HHashMultiSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ != this ) && lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
private:
};

}

template<typename key_type, typename hasher_t, typename allocator_t>
inline void swap( yaal::hcore::HHashMultiSet<key_type, hasher_t, allocator_t>& a, yaal::hcore::HHashMultiSet<key_type, hasher_t, allocator_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HHASHMULTISET_HXX_INCLUDED */


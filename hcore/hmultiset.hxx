/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmultiset.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hmultiset.hxx
 * \brief Declaration of HMultiSet<> class.
 */

#ifndef YAAL_HCORE_HMULTISET_HXX_INCLUDED
#define YAAL_HCORE_HMULTISET_HXX_INCLUDED 1

#include "hcore/hsbbstree.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

/*! \brief HSBBSTree util, a helper for HMultiSet<> instatiations.
 */
template<typename type_t>
struct multiset_helper {
	typedef typename type_t::first_type key_type;
	inline static key_type const& key( type_t const& key_ )
		{	return ( key_.first );	}
};

/*! \brief Binary tree based set.
 *
 * HMultiSet<> is a template representing self balancing binary search tree
 * data structure that holds set of keys.
 *
 * \tparam type_t - type of values held in set.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename type_t, typename compare_t = less<type_t> >
class HMultiSet {
public:
	typedef type_t value_type;
	typedef type_t key_type;
	typedef compare_t compare_type;
private:
	typedef HPair<type_t, int long> elem_t;
	typedef HSBBSTree<elem_t, compare_type, multiset_helper<elem_t> > engine_t;
	/*! \brief Iterator for HMultiSet<> data structure.
	 */
	class HIterator : public iterator_interface<value_type const, iterator_category::forward> {
		typedef HPair<value_type, int long> elem_t;
		int long _index;
		engine_t const* _owner;
		typename engine_t::HIterator _engine;
	public:
		typedef iterator_interface<value_type const, iterator_category::forward> base_type;
		HIterator( void ) : base_type(), _index( 0 ), _owner( NULL ), _engine() {}
		HIterator( HIterator const& it_ ) : base_type(), _index( it_._index ), _owner( it_._owner ), _engine( it_._engine ) {}
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_index = it_._index;
				_owner = it_._owner;
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
			HIterator it( _engine, _index );
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
			HIterator it( _engine, _index );
			operator -- ();
			return ( it );
		}
		value_type const& operator * ( void ) const {
			M_ASSERT( _owner );
			return ( _engine.get().first );
		}
		value_type const* operator -> ( void ) const {
			M_ASSERT( _owner );
			return ( &_engine.get().first );
		}
		bool operator == ( HIterator const& it ) const {
			M_ASSERT( it._owner == _owner );
			return ( ( _engine == it._engine ) && ( _index == it._index )  );
		}
		bool operator != ( HIterator const& it ) const {
			M_ASSERT( it._owner == _owner );
			return ( ( _engine != it._engine ) || ( _index != it._index ) );
		}
	private:
		friend class HMultiSet<value_type, compare_type>;
		explicit HIterator( engine_t const* owner_, typename engine_t::HIterator const& it, int long index_ )
			: base_type(), _index( index_ ), _owner( owner_ ), _engine( it ) {};
	};
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	engine_t _engine;
public:
	HMultiSet( void )
		: _engine( compare_type() )
		{}
	HMultiSet( compare_type const& compare_ )
		: _engine( compare_ )
		{}
	template<typename iterator_t>
	HMultiSet( iterator_t first, iterator_t last, compare_type const& compare_ = compare_type() )
		: _engine( compare_ ) {
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
	}
	HMultiSet( HMultiSet const& source )
		: _engine( source._engine.compare() ) {
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
	}
	HMultiSet& operator = ( HMultiSet const& multiset_ ) {
		M_PROLOG
		if ( &multiset_ != this ) {
			HMultiSet tmp( multiset_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const {
		M_PROLOG
		int long sizeAcc( 0 );
		for ( typename engine_t::HIterator it( _engine.begin() ), endIt( _engine.end() ); it != endIt; ++ it )
			sizeAcc += it.get().second;
		return ( sizeAcc );
		M_EPILOG
	}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	HIterator insert( value_type const& elem ) {
		M_PROLOG
		HPair<typename engine_t::HIterator, bool> p( _engine.insert( make_pair( elem, 1 ) ) );
		if ( ! p.second )
			++ p.first.get().second;
		return ( HIterator( &_engine, p.first, p.first.get().second - 1 ) );
		M_EPILOG
	}
	template<typename iter_t>
	void insert( iter_t i, iter_t endIt ) {
		M_PROLOG
		for ( ; i != endIt; ++ i )
			insert( *i );
		return;
		M_EPILOG
	}
	int long count( value_type const& elem ) const {
		M_PROLOG
		HIterator it( find( elem ) );
		return ( it != end() ? it._engine.get().second : 0 );
		M_EPILOG
	}
	int long erase( value_type const& elem ) {
		M_PROLOG
		HIterator it( find( elem ) );
		int long erased( 0 );
		if ( it != end() ) {
			erased = it._engine.get().second;
			_engine.remove( it._engine );
		}
		return ( erased );
		M_EPILOG
	}
	HIterator erase( HIterator const& it ) {
		M_PROLOG
		HIterator newIt( it );
		++ newIt;
		_engine.remove( it._engine );
		return ( newIt );
		M_EPILOG
	}
	void erase( HIterator first_, HIterator const& last_ ) {
		M_PROLOG
		while ( first_ != last_ )
			first_ = erase( first_ );
		return;
		M_EPILOG
	}
	HIterator find( value_type const& e ) const
		{ return ( HIterator( &_engine, _engine.find( e ), 0 ) ); }
	HIterator lower_bound( value_type const& e ) const
		{ return ( HIterator( &_engine, _engine.lower_bound( e ), 0 ) ); }
	HIterator upper_bound( value_type const& e ) const
		{ return ( HIterator( &_engine, _engine.upper_bound( e ), 0 ) ); }
	HIterator begin( void ) const
		{ return ( HIterator( &_engine, _engine.begin(), 0 ) ); }
	HIterator end( void ) const
		{ return ( HIterator( &_engine, _engine.end(), 0 ) ); }
	reverse_iterator rbegin( void ) const
		{ return ( HIterator( end() ) ); }
	reverse_iterator rend( void ) const
		{ return ( HIterator( begin() ) ); }
	void clear( void )
		{ _engine.clear(); }
	void swap( HMultiSet& other ) {
		if ( &other != this ) {
			using yaal::swap;
			_engine.swap( other._engine );
		}
	}
	bool operator == ( HMultiSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ == this ) || safe_equal( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
	bool operator < ( HMultiSet const& set_ ) const
		{ M_PROLOG return ( ( &set_ != this ) && lexicographical_compare( begin(), end(), set_.begin(), set_.end() ) ); M_EPILOG }
};

}

template<typename value_type, typename compare_t>
inline void swap( yaal::hcore::HMultiSet<value_type, compare_t>& a, yaal::hcore::HMultiSet<value_type, compare_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HMULTISET_HXX_INCLUDED */


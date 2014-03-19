/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	iterator.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want redistribute binary package you can not demand any fees
     demand any fees for it. You cannot even demand
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_ITERATOR_HXX_INCLUDED
#define YAAL_HCORE_ITERATOR_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/trait.hxx"

namespace yaal {

namespace hcore {

struct iterator_category {
	struct forward {};
	struct random_access {};
};

template<typename T, typename category>
struct iterator_interface {
	typedef T value_type;
	typedef typename trait::make_reference<T>::type reference;
	typedef typename trait::add_pointer<T>::type pointer;
	typedef category category_type;
	virtual ~iterator_interface( void ) {}
};

template<typename iterator>
struct iterator_traits {
	typedef typename iterator::value_type value_type;
	typedef typename iterator::reference reference;
	typedef typename iterator::pointer pointer;
	template<typename iterator_t>
	static trait::true_type has_category( typename iterator_t::category_type* );
	template<typename iterator_t>
	static trait::false_type has_category( ... );
	template<typename U, bool const has_type>
	struct get_category
		{ typedef iterator_category::forward type; };

	template<typename U>
	struct get_category<U, true>
		{ typedef typename U::category_type type; };
	typedef typename get_category<iterator, sizeof ( has_category<iterator>( 0 ) ) == sizeof ( trait::true_type )>::type category_type;
};

template<typename T>
struct iterator_traits<T*> {
	typedef T value_type;
	typedef T& reference;
	typedef T* pointer;
	typedef typename iterator_category::random_access category_type;
};

}

/*! \cond */
template<typename tType>
inline int long distance( tType* first_, tType* last_ ) {
	M_ASSERT( ! ( last_ < first_ ) );
	return ( last_ - first_ );
}
template<typename iter_t>
inline int long distance( iter_t first_, iter_t last_, hcore::iterator_category::random_access ) {
	M_ASSERT( ! ( last_ < first_ ) );
	return ( last_ - first_ );
}
template<typename iter_t>
inline int long distance( iter_t first_, iter_t last_, hcore::iterator_category::forward ) {
	int long dist( 0 );
	while ( first_ != last_ )
		++ first_, ++ dist;
	return ( dist );
}
/*! \endcond */
/*! \brief Calculate distance between two iterators.
 *
 * \param first - iterator.
 * \param last - iterator.
 * \return last - first.
 */
template<typename iter_t>
inline int long distance( iter_t first, iter_t last ) {
	return ( distance( first, last, typename hcore::iterator_traits<iter_t>::category_type() ) );
}

/*! \cond */
template<typename tType>
inline void advance( tType*& it_, int long distance_ ) {
	it_ += distance_;
}
template<typename iter_t>
inline void advance( iter_t& it_, int long distance_, hcore::iterator_category::random_access ) {
	it_ += distance_;
}
template<typename type_t>
inline void advance( type_t& it_, int long distance_, hcore::iterator_category::forward ) {
	for ( int long i( 0 ); i < distance_; ++ i, ++ it_ )
		;
}
/*! \endcond */
/*! \brief Move iterator forward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
void advance( iter_t& it, int long dist ) {
	advance( it, dist, typename hcore::iterator_traits<iter_t>::category_type() );
	return;
}

template<typename T>
typename T::const_iterator cbegin( T const& cont_ ) {
	return ( cont_.begin() );
}

template<typename T>
typename T::const_iterator begin( T const& cont_ ) {
	return ( cont_.begin() );
}

template<typename T>
typename T::iterator begin( T& cont_ ) {
	return ( cont_.begin() );
}

template<typename T>
typename T::const_iterator cend( T const& cont_ ) {
	return ( cont_.end() );
}

template<typename T>
typename T::const_iterator end( T const& cont_ ) {
	return ( cont_.end() );
}

template<typename T>
typename T::iterator end( T& cont_ ) {
	return ( cont_.end() );
}

template<typename T, int const N>
T const* begin( T const (&tab_)[N] ) {
	return ( tab_ );
}

template<typename T, int const N>
T const* end( T const (&tab_)[N] ) {
	return ( tab_ + N );
}

template<typename T, int const N>
T* begin( T (&tab_)[N] ) {
	return ( tab_ );
}

template<typename T, int const N>
T* end( T (&tab_)[N] ) {
	return ( tab_ + N );
}

template<typename T, int const N>
T const* cbegin( T (&tab_)[N] ) {
	return ( tab_ );
}

template<typename T, int const N>
T const* cend( T (&tab_)[N] ) {
	return ( tab_ + N );
}

namespace hcore {

template<typename iterator_t>
class HReverseIterator {
	iterator_t _iterator;
public:
	typedef typename iterator_t::value_type value_type;
	typedef typename iterator_t::pointer pointer;
	typedef typename iterator_t::reference reference;
	typedef typename iterator_t::category_type category_type;
	HReverseIterator( void )
		: _iterator() {}
	HReverseIterator( HReverseIterator const& iterator_ )
		: _iterator( iterator_._iterator ) {}
	template<typename other_iterator_t>
	HReverseIterator( HReverseIterator<other_iterator_t> const& iterator_ )
		: _iterator( iterator_._iterator ) {}
	HReverseIterator( iterator_t const& iterator_ )
		: _iterator( iterator_ )
		{ -- _iterator; }
	HReverseIterator& operator = ( HReverseIterator const& iterator_ ) {
		if ( &iterator_ != this )
			_iterator = iterator_._iterator;
		return ( *this );
	}
	HReverseIterator& operator ++ ( void ) {
		-- _iterator;
		return ( *this );
	}
	HReverseIterator operator ++ ( int ) {
		HReverseIterator it( *this );
		-- _iterator;
		return ( it );
	}
	HReverseIterator& operator -- ( void ) {
		++ _iterator;
		return ( *this );
	}
	HReverseIterator operator -- ( int ) {
		HReverseIterator it( *this );
		++ _iterator;
		return ( it );
	}
	iterator_t base( void ) const {
		return ( _iterator );
	}
	typename iterator_t::reference operator*( void ) const {
		return ( *_iterator );
	}
	typename iterator_t::pointer operator->( void ) const {
		return ( &*_iterator );
	}
	bool operator == ( HReverseIterator const& iterator_ ) const {
		return ( _iterator == iterator_._iterator );
	}
	bool operator != ( HReverseIterator const& iterator_ ) const {
		return ( _iterator != iterator_._iterator );
	}
private:
	template<typename other_iterator_t>
	friend class HReverseIterator;
};

/*! \brief (Back)Insertion concept for HInsertIterator.
 *
 * May be used for collections that support back insertion.
 */
template<typename tType>
class HBackInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.push_back( elem ); }
};

/*! \brief (Front)Insertion concept for HInsertIterator.
 *
 * May be used for collections that support front insertion.
 */
template<typename tType>
class HFrontInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.push_front( elem ); }
};

/*! \brief Insertion concept for HInsertIterator.
 *
 * Shall be used when insertion to sets/maps is required.
 */
template<typename tType>
class HInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.insert( elem ); }
};

/*! \brief (Push)Insertion concept for HInsertIterator.
 *
 * May be used for collections that support push insertion.
 */
template<typename tType>
class HPushInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.push( elem ); }
};

/*! \brief Iterator class that allows extending existing collections.
 *
 * \tparam tType - collection type.
 * \tparam inserter - insertion concept, tell how collection shall be extended.
 */
template<typename tType, typename inserter>
class HInsertIterator : public iterator_interface<typename tType::value_type, iterator_category::forward> {
public:
	typedef HInsertIterator this_type;
	typedef iterator_interface<typename tType::value_type, iterator_category::forward> base_type;
private:
	tType& _coll;
public:
	HInsertIterator( tType& coll ) : _coll( coll ) {}
	HInsertIterator& operator ++ ( void )
		{ return ( *this ); }
	HInsertIterator& operator* ( void )
		{ return ( *this ); }
	template<typename arg_t>
	HInsertIterator& operator = ( arg_t const& elem ) {
		inserter::insert( _coll, elem );
		return ( *this );
	}
};

template<typename tType>
HInsertIterator<tType, HBackInsertionConcept<tType> > back_insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HBackInsertionConcept<tType> >( coll ) ); }

template<typename tType>
HInsertIterator<tType, HFrontInsertionConcept<tType> > front_insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HFrontInsertionConcept<tType> >( coll ) ); }

template<typename tType>
HInsertIterator<tType, HInsertionConcept<tType> > insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HInsertionConcept<tType> >( coll ) ); }

template<typename tType>
HInsertIterator<tType, HPushInsertionConcept<tType> > push_insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HPushInsertionConcept<tType> >( coll ) ); }

}

}

#endif /* #ifndef YAAL_HCORE_ITERATOR_HXX_INCLUDED */


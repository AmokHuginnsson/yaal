/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/iterator.hxx
 * \brief All iterator concept related classes and free functions belong here.
 *
 * iterator_category, iterator_interface<>, iterator_traits<>,
 * HReverseIterator, HMoveIterator, HCyclicIterator can be found here.
 */

#ifndef YAAL_HCORE_ITERATOR_HXX_INCLUDED
#define YAAL_HCORE_ITERATOR_HXX_INCLUDED 1

#include "hcore/assert.hxx"
#include "hcore/trait.hxx"
#include "hcore/algorithm_low.hxx"

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
	typedef int long size_type;
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
	typedef typename get_category<iterator, sizeof ( has_category<iterator>( nullptr ) ) == sizeof ( trait::true_type )>::type category_type;
	typedef typename iterator::size_type size_type;
};

template<typename T>
struct iterator_traits<T*> {
	typedef T value_type;
	typedef T& reference;
	typedef T* pointer;
	typedef typename iterator_category::random_access category_type;
	typedef int long size_type;
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
	return dist;
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
	if ( distance_ >= 0 ) {
		for ( int long i( 0 ); i < distance_; ++ i, ++ it_ ) {
		}
	} else {
		for ( int long i( 0 ); i < -distance_; ++ i, -- it_ ) {
		}
	}
}
/*! \endcond */

/*! \brief Move iterator around.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
void advance( iter_t& it, int long dist ) {
	advance( it, dist, typename hcore::iterator_traits<iter_t>::category_type() );
	return;
}

/*! \brief Move iterator forward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
iter_t next( iter_t it, int long dist = 1 ) {
	advance( it, dist );
	return it;
}

/*! \brief Move iterator backward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
iter_t prev( iter_t it, int long dist = 1 ) {
	advance( it, -dist );
	return it;
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

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T const* begin( T const (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return tab_;
}
#else
template<typename T, int /* fuck */ unsigned const N>
T const* begin( T const (&tab_)[N] ) {
	return tab_;
}
#endif

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T const* end( T const (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return ( tab_ + N );
}
#else
template<typename T, int /* fuck */ unsigned const N>
T const* end( T const (&tab_)[N] ) {
	return ( tab_ + N );
}
#endif

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T* begin( T (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return tab_;
}
#else
template<typename T, int /* fuck */ unsigned const N>
T* begin( T (&tab_)[N] ) {
	return tab_;
}
#endif

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T* end( T (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return ( tab_ + N );
}
#else
template<typename T, int /* fuck */ unsigned const N>
T* end( T (&tab_)[N] ) {
	return ( tab_ + N );
}
#endif

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T const* cbegin( T (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return tab_;
}
#else
template<typename T, int /* fuck */ unsigned const N>
T const* cbegin( T (&tab_)[N] ) {
	return tab_;
}
#endif

#if ! defined( __GNUC__ ) || defined( __clang__ )
template<typename T, int const N>
T const* cend( T (&tab_)[N] ) {
	static_assert( N >= 0, "Invalid array size." );
	return ( tab_ + N );
}
#else
template<typename T, int /* fuck */ unsigned const N>
T const* cend( T (&tab_)[N] ) {
	return ( tab_ + N );
}
#endif

namespace hcore {

template<typename iterator_t>
class HReverseIterator {
	iterator_t _iterator;
public:
	typedef typename iterator_traits<iterator_t>::value_type value_type;
	typedef typename iterator_traits<iterator_t>::pointer pointer;
	typedef typename iterator_traits<iterator_t>::reference reference;
	typedef typename iterator_traits<iterator_t>::category_type category_type;
	typedef typename iterator_traits<iterator_t>::size_type size_type;
	HReverseIterator( void )
		: _iterator() {
		return;
	}
	HReverseIterator( HReverseIterator const& iterator_ )
		: _iterator( iterator_._iterator ) {
		return;
	}
	template<typename other_iterator_t>
	HReverseIterator( HReverseIterator<other_iterator_t> const& iterator_ )
		: _iterator( iterator_._iterator ) {
		return;
	}
	HReverseIterator( iterator_t const& iterator_ )
		: _iterator( iterator_ ) {
		-- _iterator;
		return;
	}
	HReverseIterator& operator = ( HReverseIterator const& iterator_ ) {
		if ( &iterator_ != this ) {
			_iterator = iterator_._iterator;
		}
		return ( *this );
	}
	HReverseIterator& operator ++ ( void ) {
		-- _iterator;
		return ( *this );
	}
	HReverseIterator operator ++ ( int ) {
		HReverseIterator it( *this );
		-- _iterator;
		return it;
	}
	HReverseIterator& operator -- ( void ) {
		++ _iterator;
		return ( *this );
	}
	HReverseIterator operator -- ( int ) {
		HReverseIterator it( *this );
		++ _iterator;
		return it;
	}
	HReverseIterator& operator += ( int long offset_ ) {
		_iterator -= offset_;
		return ( *this );
	}
	HReverseIterator operator + ( int long offset_ ) const {
		HReverseIterator it( *this );
		it += offset_;
		return it;
	}
	HReverseIterator& operator -= ( int long offset_ ) {
		_iterator += offset_;
		return ( *this );
	}
	HReverseIterator operator - ( int long offset_ ) const {
		HReverseIterator it( *this );
		it -= offset_;
		return it;
	}
	int long operator - ( HReverseIterator const& iterator_ ) const {
		return ( iterator_._iterator - _iterator ); /* reversed */
	}
	iterator_t base( void ) const {
		iterator_t it( _iterator );
		++ it;
		return it;
	}
	iterator_t const& raw( void ) const {
		return ( _iterator );
	}
	reference operator*( void ) const {
		return ( *_iterator );
	}
	pointer operator->( void ) const {
		return ( &*_iterator );
	}
	bool operator == ( HReverseIterator const& iterator_ ) const {
		return ( _iterator == iterator_._iterator );
	}
	bool operator != ( HReverseIterator const& iterator_ ) const {
		return ( _iterator != iterator_._iterator );
	}
	bool operator < ( HReverseIterator const& iterator_ ) const {
		return ( ! ( _iterator <= iterator_._iterator ) );
	}
	bool operator <= ( HReverseIterator const& iterator_ ) const {
		return ( ! ( _iterator < iterator_._iterator ) );
	}
	bool operator > ( HReverseIterator const& iterator_ ) const {
		return ( _iterator < iterator_._iterator );
	}
	bool operator >= ( HReverseIterator const& iterator_ ) const {
		return ( _iterator <= iterator_._iterator );
	}
private:
	template<typename other_iterator_t>
	friend class HReverseIterator;
};

template<typename T>
class HReversed {
public:
	typedef T coll_type;
	typedef typename coll_type::value_type value_type;
	typedef typename coll_type::const_reverse_iterator const_iterator;
	typedef typename coll_type::reverse_iterator iterator;
	typedef typename coll_type::const_iterator const_reverse_iterator;
	typedef typename coll_type::iterator reverse_iterator;
private:
	coll_type& _coll;
public:
	HReversed( coll_type& coll_ )
		: _coll( coll_ ) {
	}
	const_iterator begin( void ) const {
		return ( _coll.rbegin() );
	}
	const_iterator end( void ) const {
		return ( _coll.rend() );
	}
	const_iterator cbegin( void ) const {
		return ( _coll.rbegin() );
	}
	const_iterator cend( void ) const {
		return ( _coll.rend() );
	}
	iterator begin( void ) {
		return ( _coll.rbegin() );
	}
	iterator end( void ) {
		return ( _coll.rend() );
	}
	const_iterator rbegin( void ) const {
		return ( _coll.begin() );
	}
	const_iterator rend( void ) const {
		return ( _coll.end() );
	}
	const_iterator crbegin( void ) const {
		return ( _coll.begin() );
	}
	const_iterator crend( void ) const {
		return ( _coll.end() );
	}
	iterator rbegin( void ) {
		return ( _coll.begin() );
	}
	iterator rend( void ) {
		return ( _coll.end() );
	}
};

template<typename T>
HReversed<T> reversed( T& coll_ ) {
	return ( HReversed<T>( coll_ ) );
}

template<typename iterator_t>
class HMoveIterator {
	iterator_t _iterator;
public:
	typedef typename iterator_traits<iterator_t>::value_type value_type;
	typedef typename iterator_traits<iterator_t>::pointer pointer;
	typedef typename trait::strip_reference<typename iterator_traits<iterator_t>::reference>::type&& reference;
	typedef typename iterator_traits<iterator_t>::category_type category_type;
	typedef typename iterator_traits<iterator_t>::size_type size_type;
	HMoveIterator( void )
		: _iterator() {
		return;
	}
	HMoveIterator( HMoveIterator const& iterator_ )
		: _iterator( iterator_._iterator ) {
		return;
	}
	template<typename other_iterator_t>
	HMoveIterator( HMoveIterator<other_iterator_t> const& iterator_ )
		: _iterator( iterator_._iterator ) {
		return;
	}
	HMoveIterator( iterator_t const& iterator_ )
		: _iterator( iterator_ ) {
		return;
	}
	HMoveIterator& operator = ( HMoveIterator const& iterator_ ) {
		if ( &iterator_ != this ) {
			_iterator = iterator_._iterator;
		}
		return ( *this );
	}
	HMoveIterator& operator ++ ( void ) {
		++ _iterator;
		return ( *this );
	}
	HMoveIterator operator ++ ( int ) {
		HMoveIterator it( *this );
		++ _iterator;
		return it;
	}
	HMoveIterator& operator -- ( void ) {
		-- _iterator;
		return ( *this );
	}
	HMoveIterator operator -- ( int ) {
		HMoveIterator it( *this );
		-- _iterator;
		return it;
	}
	HMoveIterator& operator += ( int long offset_ ) {
		_iterator += offset_;
		return ( *this );
	}
	HMoveIterator operator + ( int long offset_ ) const {
		HMoveIterator it( *this );
		it += offset_;
		return it;
	}
	HMoveIterator& operator -= ( int long offset_ ) {
		_iterator -= offset_;
		return ( *this );
	}
	HMoveIterator operator - ( int long offset_ ) const {
		HMoveIterator it( *this );
		it -= offset_;
		return it;
	}
	int long operator - ( HMoveIterator const& it_ ) const {
		return ( _iterator - it_._iterator );
	}
	iterator_t base( void ) const {
		return ( _iterator );
	}
	reference operator*( void ) const {
		return ( yaal::move( *_iterator ) );
	}
	pointer operator->( void ) const {
		return ( &*_iterator );
	}
	bool operator == ( HMoveIterator const& iterator_ ) const {
		return ( _iterator == iterator_._iterator );
	}
	bool operator != ( HMoveIterator const& iterator_ ) const {
		return ( _iterator != iterator_._iterator );
	}
	bool operator < ( HMoveIterator const& iterator_ ) const {
		return ( _iterator < iterator_._iterator );
	}
	bool operator <= ( HMoveIterator const& iterator_ ) const {
		return ( _iterator <= iterator_._iterator );
	}
	bool operator > ( HMoveIterator const& iterator_ ) const {
		return ( _iterator > iterator_._iterator );
	}
	bool operator >= ( HMoveIterator const& iterator_ ) const {
		return ( _iterator >= iterator_._iterator );
	}
private:
	template<typename other_iterator_t>
	friend class HMoveIterator;
};

template<typename iterator_t>
HMoveIterator<iterator_t> make_move_iterator( iterator_t iterator_ ) {
	return ( HMoveIterator<iterator_t>( iterator_ ) );
}

template<typename container_t>
class HCyclicIterator
	: public iterator_interface<
			typename trait::ternary<
				trait::is_const<container_t>::value,
				typename container_t::value_type const,
				typename container_t::value_type
			>::type,
			iterator_category::forward
		> {
public:
	typedef HCyclicIterator<container_t> this_type;
	typedef typename trait::ternary<
				trait::is_const<container_t>::value,
				typename container_t::const_iterator,
				typename container_t::iterator
			>::type iterator_type;
	typedef typename trait::ternary<
				trait::is_const<container_t>::value,
				typename container_t::value_type const,
				typename container_t::value_type
			>::type value_type;
private:
	container_t* _owner;
	iterator_type _it;
public:
	HCyclicIterator( void )
		: _owner( nullptr ), _it() {
		return;
	}
	HCyclicIterator( container_t* owner_, iterator_type it_ )
		: _owner( owner_ ), _it( it_ ) {
		return;
	}
	HCyclicIterator( HCyclicIterator const& it_ )
		: _owner( it_._owner ), _it( it_._it ) {
		return;
	}
	HCyclicIterator& operator = ( HCyclicIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_it = it_._it;
		}
		return ( *this );
	}
	HCyclicIterator& operator ++ ( void ) {
		++ _it;
		if ( _it == end( *_owner ) ) {
			_it = begin( *_owner );
		}
		return ( *this );
	}
	HCyclicIterator operator ++ ( int ) {
		HCyclicIterator it( *this );
		++ _it;
		return it;
	}
	HCyclicIterator& operator -- ( void ) {
		if ( _it == begin( *_owner ) ) {
			_it = end( *_owner );
		}
		-- _it;
		return ( *this );
	}
	HCyclicIterator operator -- ( int ) {
		HCyclicIterator it( *this );
		-- _it;
		return it;
	}
	value_type& operator* ( void ) const {
		return ( *_it );
	}
	value_type* operator-> ( void ) const {
		return ( &*_it );
	}
	iterator_type base( void ) const {
		return ( _it );
	}
	bool operator != ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it != it_._it );
	}
	bool operator == ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it == it_._it );
	}
	bool operator < ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it < it_._it );
	}
	bool operator <= ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it <= it_._it );
	}
	bool operator > ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it > it_._it );
	}
	bool operator >= ( HCyclicIterator const& it_ ) const {
		M_ASSERT( ( _owner == it_._owner ) || ! ( _owner && it_._owner ) );
		return ( _it >= it_._it );
	}
	bool operator != ( typename container_t::const_iterator const& it_ ) const {
		return ( _it != it_ );
	}
	bool operator == ( typename container_t::const_iterator const& it_ ) const {
		return ( _it == it_ );
	}
	bool operator != ( typename container_t::iterator const& it_ ) const {
		return ( _it != it_ );
	}
	bool operator == ( typename container_t::iterator const& it_ ) const {
		return ( _it == it_ );
	}
};

template<typename container_t>
HCyclicIterator<container_t> cyclic_iterator( container_t& container_ ) {
	return ( HCyclicIterator<container_t>( &container_, container_.begin() ) );
}

template<typename iterator_t, typename projection_t>
class HView {
public:
	template<typename T, typename category>
	class HIterator;
	typedef decltype( trait::declval<projection_t>()( trait::declval<typename iterator_t::value_type>() ) ) value_t;
	typedef typename trait::decay<value_t>::type value_type;
	typedef HIterator<value_type, typename hcore::iterator_traits<iterator_t>::category_type> iterator;
	typedef HIterator<value_type const, typename hcore::iterator_traits<iterator_t>::category_type> const_iterator;
private:
	iterator_t _first;
	iterator_t _last;
	projection_t _projection;
public:
	HView( iterator_t first_, iterator_t last_, projection_t projection_ )
		: _first( first_ )
		, _last( last_ )
		, _projection( projection_ ) {
	}
	iterator begin( void ) {
		return ( iterator( this, _first ) );
	}
	iterator end( void ) {
		return ( iterator( this, _last ) );
	}
	const_iterator begin( void ) const {
		return ( const_iterator( this, _first ) );
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, _last ) );
	}
	projection_t const& projection( void ) const {
		return ( _projection );
	}
};

template<typename iterator_t, typename projection_t>
template<typename const_qual_t>
class HView<iterator_t, projection_t>::HIterator<const_qual_t, iterator_category::random_access> : public iterator_interface<const_qual_t, iterator_category::random_access> {
public:
	typedef HView<iterator_t, projection_t> owner_t;
	typedef HIterator<const_qual_t, iterator_category::random_access> this_type;
	typedef iterator_interface<const_qual_t, iterator_category::random_access> base_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::category_type category_type;
	typedef typename base_type::size_type size_type;
private:
	owner_t const* _owner;
	iterator_t _impl;
public:
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _impl() {
		return;
	}
	HIterator( HIterator const& it_ ) = default;
	template<typename other_const_qual_t, typename>
	HIterator( HIterator<other_const_qual_t, iterator_category::random_access> const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _impl( it_._impl ) {
		static_assert(
			( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ),
			"creating non-const iterator instance discards qualifiers"
		);
		return;
	}
	HIterator& operator = ( HIterator const& it_ ) = default;
	HIterator& operator ++ ( void ) {
		++ _impl;
		return ( *this );
	}
	HIterator const operator ++ ( int ) {
		HIterator it( *this );
		operator ++ ();
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _impl;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return it;
	}
	HIterator operator + ( size_type off_ ) const {
		HIterator it( _owner, _impl + off_ );
		return it;
	}
	HIterator& operator += ( size_type off_ ) {
		_impl += off_;
		return ( *this );
	}
	HIterator operator - ( size_type off_ ) const {
		HIterator it( _owner, _impl - off_ );
		return it;
	}
	HIterator& operator -= ( size_type off_ ) {
		_impl -= off_;
		return ( *this );
	}
	size_type operator - ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl - it._impl );
	}
	const_qual_t& operator* ( void ) {
		M_ASSERT( _owner );
		return ( const_cast<const_qual_t&>( _owner->projection()( *_impl ) ) );
	}
	const_qual_t& operator* ( void ) const {
		M_ASSERT( _owner );
		return ( _owner->projection()( *_impl ) );
	}
	const_qual_t* operator-> ( void ) {
		M_ASSERT( _owner );
		return ( const_cast<const_qual_t*>( &_owner->projection()( *_impl ) ) );
	}
	const_qual_t* operator-> ( void ) const {
		M_ASSERT( _owner );
		return ( &_owner->projection()( *_impl ) );
	}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl == it._impl );
	}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl != it._impl );
	}
	template<typename other_const_qual_t>
	bool operator < ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl < it._impl );
	}
	template<typename other_const_qual_t>
	bool operator <= ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl <= it._impl );
	}
	template<typename other_const_qual_t>
	bool operator > ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl > it._impl );
	}
	template<typename other_const_qual_t>
	bool operator >= ( HIterator<other_const_qual_t, iterator_category::random_access> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl >= it._impl );
	}
private:
	HIterator( owner_t const* owner_, iterator_t impl_ )
		: base_type()
		, _owner( owner_ )
		, _impl( impl_ ) {
	}
private:
	template<typename iterator, typename projection>
	friend class HView;
};

template<typename iterator_t, typename projection_t>
template<typename const_qual_t>
class HView<iterator_t, projection_t>::HIterator<const_qual_t, iterator_category::forward> : public iterator_interface<const_qual_t, iterator_category::forward> {
public:
	typedef HView<iterator_t, projection_t> owner_t;
	typedef HIterator<const_qual_t, iterator_category::forward> this_type;
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::category_type category_type;
	typedef typename base_type::size_type size_type;
private:
	owner_t const* _owner;
	iterator_t _impl;
public:
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _impl() {
		return;
	}
	HIterator( HIterator const& it_ ) = default;
	template<typename other_const_qual_t, typename>
	HIterator( HIterator<other_const_qual_t, iterator_category::forward> const& it_ )
		: base_type()
		, _owner( it_._owner )
		, _impl( it_._impl ) {
		static_assert(
			( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ),
			"creating non-const iterator instance discards qualifiers"
		);
		return;
	}
	HIterator& operator = ( HIterator const& it_ ) = default;
	HIterator& operator ++ ( void ) {
		++ _impl;
		return ( *this );
	}
	HIterator const operator ++ ( int ) {
		HIterator it( *this );
		operator ++ ();
		return it;
	}
	HIterator& operator -- ( void ) {
		-- _impl;
		return ( *this );
	}
	HIterator const operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return it;
	}
	const_qual_t& operator* ( void ) {
		M_ASSERT( _owner );
		return ( const_cast<const_qual_t&>( _owner->projection()( *_impl ) ) );
	}
	const_qual_t& operator* ( void ) const {
		M_ASSERT( _owner );
		return ( _owner->projection()( *_impl ) );
	}
	const_qual_t* operator-> ( void ) {
		M_ASSERT( _owner );
		return ( const_cast<const_qual_t*>( &_owner->projection()( *_impl ) ) );
	}
	const_qual_t* operator-> ( void ) const {
		M_ASSERT( _owner );
		return ( &_owner->projection()( *_impl ) );
	}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t, iterator_category::forward> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl == it._impl );
	}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t, iterator_category::forward> const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _impl != it._impl );
	}
private:
	HIterator( owner_t const* owner_, iterator_t impl_ )
		: base_type()
		, _owner( owner_ )
		, _impl( impl_ ) {
	}
private:
	template<typename iterator, typename projection>
	friend class HView;
};

template<typename iterator_t, typename projection_t>
HView<iterator_t, projection_t> view( iterator_t first_, iterator_t last_, projection_t projection_ ) {
	return ( HView<iterator_t, projection_t>( first_, last_, projection_ ) );
}

template<typename collection_t, typename projection_t>
HView<typename collection_t::const_iterator, projection_t> view( collection_t const& collection_, projection_t projection_ ) {
	return ( HView<typename collection_t::const_iterator, projection_t>( begin( collection_ ), end( collection_ ), projection_ ) );
}

/*! \brief (Back)Insertion concept for HInsertingIterator.
 *
 * May be used for collections that support back insertion.
 */
template<typename tType>
class HBackInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem ) {
		coll.push_back( elem );
	}
	template<typename elem_t>
	static void insert( tType& coll, elem_t&& elem ) {
		coll.push_back( yaal::move( elem ) );
	}
};

/*! \brief (Front)Insertion concept for HInsertingIterator.
 *
 * May be used for collections that support front insertion.
 */
template<typename tType>
class HFrontInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem ) {
		coll.push_front( elem );
	}
	template<typename elem_t>
	static void insert( tType& coll, elem_t&& elem ) {
		coll.push_front( yaal::move( elem ) );
	}
};

/*! \brief Insertion concept for HInsertingIterator.
 *
 * Shall be used when insertion to sets/maps is required.
 */
template<typename tType>
class HInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem ) {
		coll.insert( elem );
	}
	template<typename elem_t>
	static void insert( tType& coll, elem_t&& elem ) {
		coll.insert( yaal::move( elem ) );
	}
};

/*! \brief (Push)Insertion concept for HInsertingIterator.
 *
 * May be used for collections that support push insertion.
 */
template<typename tType>
class HPushInsertionConcept {
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem ) {
		coll.push( elem );
	}
	template<typename elem_t>
	static void insert( tType& coll, elem_t&& elem ) {
		coll.push( yaal::move( elem ) );
	}
};

/*! \brief Iterator class that allows extending existing collections.
 *
 * \tparam tType - collection type.
 * \tparam inserter - insertion concept, tell how collection shall be extended.
 */
template<typename tType, typename inserter>
class HInsertingIterator : public iterator_interface<typename tType::value_type, iterator_category::forward> {
public:
	typedef HInsertingIterator this_type;
	typedef iterator_interface<typename tType::value_type, iterator_category::forward> base_type;
protected:
	typedef inserter inserter_type;
	tType& _coll;
public:
	HInsertingIterator( tType& coll )
		: _coll( coll ) {
	}
	HInsertingIterator& operator ++ ( void ) {
		return ( *this );
	}
	HInsertingIterator& operator* ( void ) {
		return ( *this );
	}
	template<typename arg_t>
	this_type& operator = ( arg_t const& elem_ ) {
		inserter_type::insert( _coll, elem_ );
		return ( *this );
	}
	template<typename arg_t>
	this_type& operator = ( arg_t&& elem_ ) {
		inserter_type::insert( _coll, yaal::move( elem_ ) );
		return ( *this );
	}
};

/*! \brief Back inserting iterator class.
 * Iterator class that allows extending existing collections
 * by inserting elements at end of given collection.
 *
 * \tparam tType - collection type.
 */
template<typename tType>
class HBackInsertIterator : public HInsertingIterator<tType, HBackInsertionConcept<tType> > {
public:
	typedef HBackInsertIterator<tType> this_type;
	typedef HInsertingIterator<tType, HBackInsertionConcept<tType> > base_type;
	HBackInsertIterator( tType& coll_ )
		: base_type( coll_ ) {
	}
	HBackInsertIterator( base_type const& insertIterator_ )
		: base_type( insertIterator_ ) {
	}
	template<typename arg_t>
	HBackInsertIterator<tType>& operator = ( arg_t const& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, elem_ );
		return ( *this );
	}
	template<typename arg_t>
	HBackInsertIterator<tType>& operator = ( arg_t&& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, yaal::move( elem_ ) );
		return ( *this );
	}
};

/*! \brief Front inserting iterator class.
 * Iterator class that allows extending existing collections
 * by inserting elements at beginning of given collection.
 *
 * \tparam tType - collection type.
 */
template<typename tType>
class HFrontInsertIterator : public HInsertingIterator<tType, HFrontInsertionConcept<tType> > {
public:
	typedef HFrontInsertIterator<tType> this_type;
	typedef HInsertingIterator<tType, HFrontInsertionConcept<tType> > base_type;
	HFrontInsertIterator( tType& coll_ )
		: base_type( coll_ ) {
	}
	HFrontInsertIterator( base_type const& insertIterator_ )
		: base_type( insertIterator_ ) {
	}
	template<typename arg_t>
	HFrontInsertIterator<tType>& operator = ( arg_t const& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, elem_ );
		return ( *this );
	}
	template<typename arg_t>
	HFrontInsertIterator<tType>& operator = ( arg_t&& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, yaal::move( elem_ ) );
		return ( *this );
	}
};

/*! \brief Key based inserting iterator class.
 * Iterator class that allows extending existing collections
 * by inserting elements using key based insert operation.
 *
 * \tparam tType - collection type.
 */
template<typename tType>
class HInsertIterator : public HInsertingIterator<tType, HInsertionConcept<tType> > {
public:
	typedef HInsertIterator<tType> this_type;
	typedef HInsertingIterator<tType, HInsertionConcept<tType> > base_type;
	HInsertIterator( tType& coll_ )
		: base_type( coll_ ) {
	}
	HInsertIterator( base_type const& insertIterator_ )
		: base_type( insertIterator_ ) {
	}
	template<typename arg_t>
	HInsertIterator<tType>& operator = ( arg_t const& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, elem_ );
		return ( *this );
	}
	template<typename arg_t>
	HInsertIterator<tType>& operator = ( arg_t&& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, yaal::move( elem_ ) );
		return ( *this );
	}
};

/*! \brief Push based inserting iterator class.
 * Iterator class that allows extending existing collections
 * by inserting elements using pushing elements into given collection.
 *
 * \tparam tType - collection type.
 */
template<typename tType>
class HPushInsertIterator : public HInsertingIterator<tType, HPushInsertionConcept<tType> > {
public:
	typedef HPushInsertIterator<tType> this_type;
	typedef HInsertingIterator<tType, HPushInsertionConcept<tType> > base_type;
	HPushInsertIterator( tType& coll_ )
		: base_type( coll_ ) {
	}
	HPushInsertIterator( base_type const& insertIterator_ )
		: base_type( insertIterator_ ) {
	}
	template<typename arg_t>
	HPushInsertIterator<tType>& operator = ( arg_t const& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, elem_ );
		return ( *this );
	}
	template<typename arg_t>
	HPushInsertIterator<tType>& operator = ( arg_t&& elem_ ) {
		typename base_type::inserter_type::insert( this->_coll, yaal::move( elem_ ) );
		return ( *this );
	}
};

template<typename tType>
inline HInsertingIterator<tType, HBackInsertionConcept<tType> > back_insert_iterator( tType& coll ) {
	return ( HInsertingIterator<tType, HBackInsertionConcept<tType> >( coll ) );
}

template<typename tType>
inline HInsertingIterator<tType, HFrontInsertionConcept<tType> > front_insert_iterator( tType& coll ) {
	return ( HInsertingIterator<tType, HFrontInsertionConcept<tType> >( coll ) );
}

template<typename tType>
inline HInsertingIterator<tType, HInsertionConcept<tType> > insert_iterator( tType& coll ) {
	return ( HInsertingIterator<tType, HInsertionConcept<tType> >( coll ) );
}

template<typename tType>
inline HInsertingIterator<tType, HPushInsertionConcept<tType> > push_insert_iterator( tType& coll ) {
	return ( HInsertingIterator<tType, HPushInsertionConcept<tType> >( coll ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_ITERATOR_HXX_INCLUDED */


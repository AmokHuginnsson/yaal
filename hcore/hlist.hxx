/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlist.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HLIST_HXX_INCLUDED
#define YAAL_HCORE_HLIST_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/memory.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/functional.hxx"
#include "hcore/pod.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHList_[];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ general list declaration ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template<typename type_t>
class HList;

/*! \brief Meta-data definition for HList<> class.
 */
struct OListBits {
	/*! \brief Error codes for HList<> operations.
	 */
	struct ERROR {
		/*! \brief Error flags.
		 */
		typedef enum {
			OK = 0,    /*!< No error. */
			BAD_INDEX, /*!< Bad elements index used. */
			EMPTY,     /*!< List is empty. */
			BAD_ORDER  /*!< Bad order flag. */
		} error_t;
	};
	typedef enum {
		TREAT_AS_CLOSED = 1,
		TREAT_AS_OPENED = 2
	} treatment_t;
	typedef enum {
		OK = 0,
		ERROR = 1,
		FINAL_REACHED = 2,
		NOT_FOUND = 4
	} status_t;
	virtual ~OListBits( void ) { }
	/*! \brief HList<>::iterator type constructor.
	 *
	 * \tparam T - type of elements stored in HList<>.
	 * \tparam Q - iterator bahavior.
	 * \retval type - new iterator type.
	 */
	template<typename T, OListBits::treatment_t Q>
	struct iterator {
		typedef typename HList<T>::template HIterator<T,Q> type;
	};
};

/*! \brief Doubly-linked list data structure and its operations.
 */
template<typename type_t>
class HList : public OListBits {
	typedef HList<type_t> this_type;
private:
	class HElement;
	int long _size;           /*!< how many elements this list contains */
	HElement* _hook;    /*!< "begining" of the list ( "first" element ) */
/* for internal use only */
	int long _index;          /*!< this two fiels will allow boost operator[], int holds last */
	HElement* _indexElement;   /*!< index and HElement * holds pointer to this last element */
public:
	typedef type_t value_type;
	template<typename const_qual_t, OListBits::treatment_t const treatment>
	class HIterator;
	typedef class HIterator<type_t, OListBits::TREAT_AS_OPENED> iterator;
	typedef class HIterator<type_t const, OListBits::TREAT_AS_OPENED> const_iterator;
	typedef class HIterator<type_t, OListBits::TREAT_AS_CLOSED> cyclic_iterator;
	typedef class HIterator<type_t const, OListBits::TREAT_AS_CLOSED> const_cyclic_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
	/*! \brief Create an empty list.
	 */
	HList( void );
	/*! \brief Creates list, with specified size.
	 *
	 * \param count - number of element for newly created list.
	 */
	explicit HList( int long count );
	HList( HList const& );
	/*! \brief Create list and initialize it with copies of given value.
	 *
	 * \param count - size of new list.
	 * \param value - list initializer value.
	 */
	HList( int long count, type_t const& value );
	/*! \brief Creates list, from range of elements.
	 */
	template<typename iter_t>
	HList( iter_t, iter_t );
	virtual ~HList( void );
	HList& operator = ( HList const& );
	cyclic_iterator hook( void );
	const_cyclic_iterator hook( void ) const;
	iterator begin( void );
	iterator end( void );
	reverse_iterator rend( void );
	reverse_iterator rbegin( void );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	const_reverse_iterator rend( void ) const;
	const_reverse_iterator rbegin( void ) const;
	void clear( void );
	void resize( int long, type_t const& = type_t() );
	int long size( void ) const;
	int long get_size( void ) const;
	void swap( HList<type_t>& );
	template<typename iterator_t>
	void assign( iterator_t, iterator_t );
	void assign( int long, type_t const& );
	/*! \brief Adds new element at specified position.
	 *
	 * Newly created element will have default value.
	 */
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<type_t, treatment>::type insert( HIterator<type_t, treatment> const& );
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<type_t, treatment>::type insert( HIterator<type_t, treatment> const&, type_t const& );
	template<OListBits::treatment_t const treatment, typename iterator_t>
	void insert( HIterator<type_t, treatment> const&, iterator_t, iterator_t );
	template<OListBits::treatment_t const treatment>
	void insert( HIterator<type_t, treatment> const&, int long, type_t const& );
	/*! \brief Adds new element at beggining of the list
	 *
	 * Newly created element will have default value.
	 */
	type_t& add_head( void );
	/*! \brief Adds new element at end of the list
	 *
	 * Newly created element will have default value.
	 */
	type_t& add_tail( void );
	void push_back( type_t const& );
	void pop_back( void );
	void push_front( type_t const& );
	void pop_front( void );
	/*! \brief Add element in the way that keeps order.
	 */
	template<typename T>
	type_t& add_orderly( type_t const&, T const& );
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<type_t, treatment>::type erase( HIterator<type_t, treatment> const& );
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<type_t, treatment>::type erase( HIterator<type_t, treatment>, HIterator<type_t, treatment> const& );
	/*! \brief Sets cursor at specified index or number
	 */
	iterator n_th( int long );
	type_t& operator[] ( int long );
	type_t& front( void );
	type_t const& front( void ) const;
	type_t& back( void );
	type_t const& back( void ) const;
	type_t& head( void );
	type_t const& head( void ) const;
	type_t& tail( void );
	type_t const& tail( void ) const;
	void exchange( iterator const&, iterator const& );
	void sort( void );
	template<typename T>
	void sort( T const& );
	bool empty( void ) const;
	bool is_empty( void ) const;
	bool operator == ( HList const& ) const;
	bool operator < ( HList const& ) const;
private:
	template<typename iterator_t>
	void initialize( iterator_t, iterator_t, trait::false_type const* );
	void initialize( int long, type_t const&, trait::true_type const* );
	template<typename T>
	void merge_sort( HElement*&, HElement*&, T const& );
	template<typename T>
	void insert_sort( HElement*&, HElement*&, T const& );
	void insert( HElement*, HElement* );
	HElement* element_by_index( int long );
	void exchange( HElement*, HElement* );
	void sub_swap( HElement*, HElement*, HElement* );
	friend class HIterator<type_t, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<type_t const, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<type_t, OListBits::TREAT_AS_CLOSED>;
	friend class HIterator<type_t const, OListBits::TREAT_AS_CLOSED>;
};

/*! \brief HList<> element class provisions basic building block for doubly-linked list.
 */
template<typename type_t>
class HList<type_t>::HElement {
private:
	HElement* _previous;
	HElement* _next;
	type_t _value; /*!< The Object itself. */
	explicit HElement( HElement* );
	explicit HElement( HElement*, type_t const& );
	virtual ~HElement ();
	void connect( HElement* );
	HElement( HElement const & );
	HElement& operator = ( HElement const& );
	friend class HList<type_t>;
	friend class HIterator<type_t, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<type_t const, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<type_t, OListBits::TREAT_AS_CLOSED>;
	friend class HIterator<type_t const, OListBits::TREAT_AS_CLOSED>;
};

/*! \brief Iterator for HList<> data structure.
 */
template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
class HList<type_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::forward> {
private:
	HList<type_t> const* _owner;
	HElement* _current;
public:
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	HIterator( void );
	HIterator( HIterator const& );
	template<typename other_const_qual_t, OListBits::treatment_t family>
	HIterator( HIterator<other_const_qual_t, family> const& );
	HIterator& operator ++ ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( _current ) {
			_current = _current->_next;
			if ( ( treatment == OListBits::TREAT_AS_OPENED ) && ( _current == _owner->_hook ) )
				_current = NULL;
		} else if ( treatment == OListBits::TREAT_AS_OPENED )
			_current = _owner->_hook;
		return ( *this );
		M_EPILOG
	}
	HIterator const operator ++ ( int ) {
		M_PROLOG
		HIterator iterator ( *this );
		++ ( *this );
		return ( iterator );
		M_EPILOG
	}
	HIterator& operator -- ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( _current ) {
			_current = _current->_previous;
			if ( ( treatment == OListBits::TREAT_AS_OPENED ) && ( _current == _owner->_hook->_previous ) )
				_current = NULL;
		} else if ( ( treatment == OListBits::TREAT_AS_OPENED ) && ( _owner->_hook ) )
			_current = _owner->_hook->_previous;
		return ( *this );
		M_EPILOG
	}
	HIterator const operator -- ( int ) {
		M_PROLOG
		HIterator iterator( * this );
		-- ( *this );
		return ( iterator );
		M_EPILOG
	}
	typename HList<type_t>::template HIterator<const_qual_t, treatment>& operator = ( HIterator const& );
	bool operator == ( HIterator const & ) const;
	bool operator != ( HIterator const & ) const;
	const_qual_t& operator* ( void );
	const_qual_t& operator* ( void ) const;
	const_qual_t* operator->( void );
	const_qual_t* operator->( void ) const;
protected:
	friend class HList<type_t>;
	template<typename same_const_qual_t, OListBits::treatment_t const family>
	friend class HIterator;
	HIterator( HList<type_t> const* const, HElement* const );
};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*++++++++++++++++++++++ general list implementation ++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template<typename type_t>
HList<type_t>::HElement::HElement( HElement* element_ )
	: _previous ( NULL ), _next ( NULL ), _value() {
	connect( element_ );
}

template<typename type_t>
HList<type_t>::HElement::HElement( HElement* element_, type_t const& value_ )
	: _previous ( NULL ), _next ( NULL ), _value( value_ ) {
	connect( element_ );
}

template<typename type_t>
HList<type_t>::HElement::~HElement( void ) {
	_previous->_next = _next;
	_next->_previous = _previous;
	return;
}

template<typename type_t>
void HList<type_t>::HElement::connect( HElement* element_ ) {
	if ( element_ == 0 ) {
		_previous = this;
		_next = this;
	} else {
		_previous = element_->_previous;
		_next = element_;
		element_->_previous = this;
		_previous->_next = this;
	}
}

//========================== Iterator ========================================

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
HList<type_t>::HIterator<const_qual_t, treatment>::HIterator( void )
	: base_type(), _owner( NULL ), _current( NULL ) {
	return;
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
HList<type_t>::HIterator<const_qual_t, treatment>::HIterator( HIterator const& iterator_ )
	: base_type(), _owner( iterator_._owner ), _current( iterator_._current ) {
	M_PROLOG
	return;
	M_EPILOG
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
template<typename other_const_qual_t, OListBits::treatment_t family>
HList<type_t>::HIterator<const_qual_t, treatment>::HIterator( HIterator<other_const_qual_t, family> const& iterator_ )
	: base_type(), _owner( iterator_._owner ), _current( iterator_._current ) {
	M_PROLOG
	STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
	return;
	M_EPILOG
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
HList<type_t>::HIterator<const_qual_t, treatment>::HIterator( HList<type_t> const* const owner_,
		HElement* const element_ )
	: base_type(), _owner( owner_ ), _current( element_ ) {
	return;
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
typename HList<type_t>::template HIterator<const_qual_t, treatment>&
HList<type_t>::HIterator<const_qual_t, treatment>::operator = ( HIterator const& iterator_ ) {
	M_PROLOG
	if ( reinterpret_cast<HIterator<const_qual_t, treatment> const*>( &iterator_ ) != this ) {
		_owner = iterator_._owner;
		_current = iterator_._current;
	}
	return ( *this );
	M_EPILOG
}

/*
template<typename type_t>
typename HList<type_t>::HIterator & HList< type_t >::HIterator::operator ++ ( void )

template<typename type_t>
typename HList<type_t>::HIterator const HList< type_t >::HIterator::operator ++ ( int )

template<typename type_t>
typename HList<type_t>::HIterator & HList< type_t >::HIterator::operator -- ( void )

template<typename type_t>
typename HList<type_t>::HIterator const HList< type_t >::HIterator::operator -- ( int )
*/

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
bool HList<type_t>::HIterator<const_qual_t, treatment>::operator == ( HIterator const& iterator_ ) const {
	M_PROLOG
	M_ASSERT( ( ! ( _owner && iterator_._owner ) ) || ( _owner == iterator_._owner ) );
	return ( _current == iterator_._current );
	M_EPILOG
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
bool HList<type_t>::HIterator<const_qual_t, treatment>::operator!= ( HIterator const& iterator_ ) const {
	M_PROLOG
	M_ASSERT( ( ! ( _owner && iterator_._owner ) ) || ( _owner == iterator_._owner ) );
	return ( _current != iterator_._current );
	M_EPILOG
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t& HList<type_t>::HIterator<const_qual_t, treatment>::operator* ( void ) {
	return ( _current->_value );
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t& HList<type_t>::HIterator<const_qual_t, treatment>::operator* ( void ) const {
	return ( _current->_value );
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t* HList<type_t>::HIterator<const_qual_t, treatment>::operator->( void ) {
	return ( &_current->_value );
}

template<typename type_t>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t* HList<type_t>::HIterator<const_qual_t, treatment>::operator->( void ) const {
	return ( &_current->_value );
}

//============================================================================

template<typename type_t>
HList<type_t>::HList( void )
	: OListBits(), _size( 0 ),
	_hook( NULL ), _index( 0 ),
	_indexElement( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

template<typename type_t>
HList<type_t>::HList( int long count_ )
	: OListBits(), _size( 0 ),
	_hook( NULL ), _index( 0 ),
	_indexElement( NULL ) {
	M_PROLOG
	while ( count_ -- )
		add_tail();
	return;
	M_EPILOG
}

template<typename type_t>
HList<type_t>::HList( int long count_, type_t const& value_ )
	: OListBits(), _size( 0 ),
	_hook( NULL ), _index( 0 ),
	_indexElement( NULL ) {
	M_PROLOG
	resize( count_, value_ );
	return;
	M_EPILOG
}

template<typename type_t>
HList<type_t>::~HList( void ) {
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
}

template<typename type_t>
HList<type_t>::HList( HList<type_t> const& list_ )
	: OListBits(), _size( 0 ),
	_hook( NULL ), _index( 0 ),
	_indexElement( NULL ) {
	M_PROLOG
	( *this ) = list_;
	return;
	M_EPILOG
}

template<typename type_t>
template<typename iter_t>
HList<type_t>::HList( iter_t first, iter_t last )
	: OListBits(), _size( 0 ),
	_hook( NULL ), _index( 0 ),
	_indexElement( NULL ) {
	M_PROLOG
	initialize( first, last, typename trait::add_pointer<typename is_integral<iter_t>::type>::type() );
	return;
	M_EPILOG
}

template<typename type_t>
template<typename iterator_t>
void HList<type_t>::initialize( iterator_t first, iterator_t last, trait::false_type const* ) {
	M_PROLOG
	for ( ; first != last; ++ first )
		push_back( *first );
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::initialize( int long size_, type_t const& fillWith_, trait::true_type const* ) {
	M_PROLOG
	resize( size_, fillWith_ );
	return;
	M_EPILOG
}

template<typename type_t>
template<typename iterator_t>
void HList<type_t>::assign( iterator_t first, iterator_t last ) {
	M_PROLOG
	clear();
	for ( ; first != last; ++ first )
		push_back( *first );
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::assign( int long size_, type_t const& fillWith_ ) {
	M_PROLOG
	clear();
	resize( size_, fillWith_ );
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::resize( int long size_, type_t const& value_ ) {
	M_PROLOG
	int long diff( abs( _size - size_ ) );
	if ( _size > size_ ) {
		while ( diff -- )
			pop_back();
	} else if ( _size < size_ ) {
		while ( size_ -- )
			push_back( value_ );
	}
	return;
	M_EPILOG
}

template<typename type_t>
typename HList<type_t>::const_iterator HList<type_t>::begin( void ) const {
	return ( const_iterator( this, _hook ) );
}

template<typename type_t>
typename HList<type_t>::iterator HList<type_t>::begin( void ) {
	return ( iterator( this, _hook ) );
}

template<typename type_t>
typename HList<type_t>::const_iterator HList<type_t>::end( void ) const {
	return ( const_iterator( this, NULL ) );
}

template<typename type_t>
typename HList<type_t>::iterator HList<type_t>::end( void ) {
	return ( iterator( this, NULL ) );
}

template<typename type_t>
typename HList<type_t>::const_reverse_iterator HList<type_t>::rbegin( void ) const {
	return ( end() );
}

template<typename type_t>
typename HList<type_t>::reverse_iterator HList<type_t>::rbegin( void ) {
	return ( end() );
}

template<typename type_t>
typename HList<type_t>::const_reverse_iterator  HList<type_t>::rend( void ) const {
	return ( begin() );
}

template<typename type_t>
typename HList<type_t>::reverse_iterator HList<type_t>::rend( void ) {
	return ( begin() );
}

template<typename type_t>
typename HList<type_t>::const_cyclic_iterator HList<type_t>::hook( void ) const {
	return ( const_cyclic_iterator( this, _hook ) );
}

template<typename type_t>
typename HList<type_t>::cyclic_iterator HList<type_t>::hook( void ) {
	return ( cyclic_iterator( this, _hook ) );
}

template<typename type_t>
HList<type_t>& HList<type_t>::operator = ( HList<type_t> const& list_ ) {
	M_PROLOG
	if ( this != & list_ ) {
		iterator thisIt = begin();
		const_iterator otherIt = list_.begin();
		int long ctr( 0 );
		int long count( _size < list_._size ? _size : list_._size );
		if ( count > 0 ) {
			for ( ; ctr < count; ++ ctr, ++ thisIt, ++ otherIt ) {
				*thisIt = *otherIt;
				if ( otherIt._current == list_._hook )
					_hook = thisIt._current;
				if ( otherIt._current == list_._indexElement )
					_indexElement = thisIt._current;
			}
		}
		if ( _size > list_._size ) {
			count = _size - list_._size;
			while ( count -- )
				pop_back();
		} else if ( _size < list_._size ) {
			for ( ; ctr < list_._size; ++ ctr, ++ otherIt ) {
				push_back( *otherIt );
				if ( otherIt._current == list_._indexElement )
					_indexElement = _hook->_previous;
			}
		}
		_index = list_._index;
		_size = list_._size;
	}
	return ( *this );
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::clear( void ) {
	M_PROLOG
	while ( _size -- )
		M_SAFE( delete _hook->_next );
	_index = 0;
	_indexElement = NULL;
	_hook = NULL;
	_size = 0;
	return ;
	M_EPILOG
}

template<typename type_t>
bool HList<type_t>::is_empty( void ) const {
	return ( ! _size );
}

template<typename type_t>
bool HList<type_t>::empty( void ) const {
	return ( ! _size );
}

template<typename type_t>
int long HList<type_t>::size( void ) const {
	M_PROLOG
	return ( _size );
	M_EPILOG
}

template<typename type_t>
int long HList<type_t>::get_size( void ) const {
	M_PROLOG
	return ( _size );
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::swap( HList<type_t>& other ) {
	if ( &other != this ) {
		using yaal::swap;
		swap( _size, other._size );
		swap( _index, other._index );
		swap( _hook, other._hook );
		swap( _indexElement, other._indexElement );
	}
	return;
}

template<typename type_t>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<type_t, treatment>::type HList<type_t>::insert( HIterator<type_t, treatment> const& it ) {
	M_PROLOG
	HElement* element = new ( memory::yaal ) HElement( it._current ? it._current : _hook );
	if ( ( _size == 0 ) || ( ( it._current == _hook ) && ( treatment == TREAT_AS_OPENED ) ) )
		_hook = element;
	_size ++;
	_index = 0;
	_indexElement = NULL;
	return ( iterator( this, element ) );
	M_EPILOG
}

template<typename type_t>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<type_t, treatment>::type HList<type_t>::insert( HIterator<type_t, treatment> const& it,
		type_t const& val ) {
	M_PROLOG
	HElement* element = new ( memory::yaal ) HElement( it._current ? it._current : _hook, val );
	if ( ( _size == 0 ) || ( ( it._current == _hook ) && ( treatment == TREAT_AS_OPENED ) ) )
		_hook = element;
	_size ++;
	_index = 0;
	_indexElement = NULL;
	return ( iterator( this, element ) );
	M_EPILOG
}

template<typename type_t>
template<OListBits::treatment_t const treatment>
void HList<type_t>::insert( HIterator<type_t, treatment> const& it,
		int long count_, type_t const& val ) {
	M_PROLOG
	for ( int long i = 0; i < count_; ++ i )
		insert( it, val );
	return;
	M_EPILOG
}

template<typename type_t>
template<OListBits::treatment_t const treatment, typename iterator_t>
void HList<type_t>::insert( HIterator<type_t, treatment> const& it,
		iterator_t first, iterator_t last ) {
	M_PROLOG
	for ( ; first != last; ++ first )
		insert( it, *first );
	return;
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::add_head( void ) {
	M_PROLOG
	_hook = new ( memory::yaal ) HElement( _hook );
	++ _size;
	if ( _indexElement )
		_indexElement = _indexElement->_previous;
	return ( _hook->_value );
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::add_tail( void ) {
	M_PROLOG
	HElement* element = new ( memory::yaal ) HElement( _hook );
	if ( _size == 0 )
		_hook = element;
	++ _size;
	return ( element->_value );
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::push_front( type_t const& object_ ) {
	M_PROLOG
	_hook = new ( memory::yaal ) HElement( _hook, object_ );
	++ _size;
	if ( _indexElement )
		_indexElement = _indexElement->_previous;
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::push_back( type_t const& object_ ) {
	M_PROLOG
	HElement* element = new ( memory::yaal ) HElement( _hook, object_ );
	if ( _size == 0 )
		_hook = element;
	++ _size;
	return;
	M_EPILOG
}

template<typename type_t>
template<typename T>
type_t& HList<type_t>::add_orderly( type_t const& object_,
		T const& comp_ ) {
	M_PROLOG
	bool before = false;
	int index = 0, oldIndex = -1, lower = 0, upper = _size;
	HElement* element = new ( memory::yaal ) HElement( NULL, object_ );
	while ( _size && ( oldIndex != index ) ) {
		oldIndex = index;
		index = ( lower + upper ) / 2;
		element_by_index( index );
		if ( comp_( _indexElement->_value, element->_value ) )
			lower = index;
		else
			upper = index;
	}
	if ( _indexElement ) {
		if ( comp_( _indexElement->_value, element->_value ) )
			_indexElement = _indexElement->_next;
		else
			before = true;
	}
	if ( ! _size )
		_hook = _indexElement = element;
	else {
		element->_next = _indexElement;
		element->_previous = _indexElement->_previous;
		_indexElement->_previous->_next = element;
		_indexElement->_previous = element;
	}
	_size ++;
	_indexElement = NULL;
	_index = 0;
	if ( before && ! index )
		_hook = _hook->_previous;
	return ( element->_value );
	M_EPILOG
}

template<typename type_t>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<type_t, treatment>::type
HList<type_t>::erase( HIterator<type_t, treatment> const& iterator_ ) {
	M_PROLOG
	HIterator<type_t, treatment> it = iterator_;
	++ it;
	if ( ! _size )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	/*
	 * What iterator shall be returned.
	 *
	 * common easy:
	 * 1 2 3 4 5 6 7
	 *     ^
	 * 1 2 4 5 6 7
	 *     ^
	 *
	 * common special:
	 * 1 2 3 4 5
	 * ^
	 * 2 3 4 5
	 * ^
	 *
	 * tricky:
	 * 1 2 3 4 5
	 *         ^
	 * 1 2 3 4 (nil)  (opened list)
	 *           ^
	 * 1 2 3 4  (closed list)
	 * ^
	 */
	if ( iterator_._current == _hook )
		_hook = _hook->_next;
	M_SAFE( delete iterator_._current );
	_size --;
	if ( _size == 0 )
		_hook = NULL;
	_index = 0;
	_indexElement = NULL;
	return ( it );
	M_EPILOG
}

template<typename type_t>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<type_t, treatment>::type
HList<type_t>::erase( HIterator<type_t, treatment> first_, HIterator<type_t, treatment> const& last_ ) {
	M_PROLOG
	while ( first_ != last_ )
		first_ = erase( first_ );
	return ( first_ );
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::pop_front( void ) {
	M_PROLOG
	HElement* element = NULL;
	if ( _size > 0 ) {
		element = _hook;
		_hook = _hook->_next;
	} else
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	if ( _indexElement )
		_indexElement = _indexElement->_next;
	M_SAFE( delete element );
	_size--;
	if ( _size == 0 ) {
		_hook = NULL;
		_indexElement = NULL;
		_index = 0;
	}
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::pop_back( void ) {
	M_PROLOG
	HElement* element = NULL;
	if ( _size > 0 ) {
		element = _hook->_previous;
		if ( element == _indexElement ) {
			_indexElement = element->_previous;
			_index --;
		}
		M_SAFE( delete element );
		-- _size;
		if ( _size == 0 ) {
			_hook = NULL;
			_indexElement = NULL;
			_index = 0;
		}
	} else
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	return;
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::operator[] ( int long index_ ) {
	M_PROLOG
	return ( element_by_index( index_ )->_value );
	M_EPILOG
}

template<typename type_t>
typename HList<type_t>::HElement* HList<type_t>::element_by_index( int long index_ ) {
	M_PROLOG
	if ( _size == 0 )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	if ( index_ < 0 )
		index_ += _size;
	if ( ( index_ >= _size ) || ( index_ < 0 ) )
		M_THROW( _errMsgHList_[ ERROR::BAD_INDEX ], index_ );
	if ( ! _indexElement )
		_indexElement = _hook;
/*
we are looking for the smallest distance between index_ and 0, _index, _size.
we have two cases, index_ is between 0 and _index:
	0 ..... index_ ..... _index ..... _size
meaning index_ < _index
and index_ is between _index and _size
	0 ..... _index ..... index_ ..... _size
meaning index_ > _index
lets take closer look at first case  ( index_ < _index )
we have to check if index_ is lowwer or geater than _index/2
*/
	if ( index_ < _index ) {
		if ( index_ < ( _index / 2 ) )
			for ( _index = 0, _indexElement = _hook; _index < index_; ++ _index )
				_indexElement = _indexElement->_next;
		else
			for ( ; _index > index_; _index -- )
				_indexElement = _indexElement->_previous;
	} else {
		if ( index_ < ( ( _size + _index ) / 2 ) )
			for ( ; _index < index_; ++ _index )
				_indexElement = _indexElement->_next;
		else
			for ( _index = _size - 1, _indexElement = _hook->_previous;
					_index > index_; _index -- )
				_indexElement = _indexElement->_previous;
	}
	return ( _indexElement );
	M_EPILOG
}

template<typename type_t>
typename HList<type_t>::iterator HList<type_t>::n_th( int long index_ ) {
	M_PROLOG
	return ( iterator( this, element_by_index( index_ ) ) );
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::exchange( HElement* left_, HElement* right_ ) {
	M_PROLOG
	HElement* next = NULL;
	HElement* previous = NULL;
	if ( left_ == right_ )
		return;
	if ( left_ == _hook )
		_hook = right_;
	else if ( right_ == _hook )
		_hook = left_;
	if ( left_ == _indexElement )
		_indexElement = right_;
	else if ( right_ == _indexElement )
		_indexElement = left_;
/*
 *                         ( p L n )
 *          ( p R n ) <------+   +------> ( p R n )
 *  ( L ) <---+   +---> ( L )     ( L ) <---+   +---> ( L )
 */
	next = right_->_next;
	previous = right_->_previous;
	left_->_next->_previous = right_;
	left_->_previous->_next = right_;
	next->_previous = left_;
	previous->_next = left_;

	next = left_->_next;
	previous = left_->_previous;
	left_->_next = right_->_next;
	left_->_previous = right_->_previous;
	right_->_next = next;
	right_->_previous = previous;
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::exchange( iterator const& left, iterator const& right ) {
	M_PROLOG
	if ( left != right )
		exchange( left._current, right._current );
	return;
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::front( void ) {
	M_PROLOG
	if ( _hook == 0 )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	return ( _hook->_value );
	M_EPILOG
}

template<typename type_t>
type_t const& HList<type_t>::front( void ) const {
	M_PROLOG
	if ( _hook == 0 )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	return ( _hook->_value );
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::head( void ) {
	M_PROLOG
	return ( front() );
	M_EPILOG
}

template<typename type_t>
type_t const& HList<type_t>::head( void ) const {
	M_PROLOG
	return ( front() );
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::back( void ) {
	M_PROLOG
	if ( _hook == 0 )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	return ( _hook->_previous->_value );
	M_EPILOG
}

template<typename type_t>
type_t const& HList<type_t>::back( void ) const {
	M_PROLOG
	if ( _hook == 0 )
		M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
	return ( _hook->_previous->_value );
	M_EPILOG
}

template<typename type_t>
type_t& HList<type_t>::tail( void ) {
	M_PROLOG
	return ( back() );
	M_EPILOG
}

template<typename type_t>
type_t const& HList<type_t>::tail( void ) const {
	M_PROLOG
	return ( back() );
	M_EPILOG
}

template<typename type_t>
template<typename T>
void HList<type_t>::merge_sort( HElement*& left, HElement*& right, T const& comp_ ) {
	M_PROLOG
	HElement* leftIt = left;
	HElement* rightIt = right;
	int long stepsLeft = 0;
	int long stepsRight = 0;
	while ( leftIt != rightIt ) {
		if ( leftIt->_next == rightIt )
			break;
		leftIt = leftIt->_next;
		++ stepsLeft;
		if ( leftIt == rightIt->_previous )
			break;
		rightIt = rightIt->_previous;
		++ stepsRight;
	}
	int const ARBITRARILY_CHOSEN_THRESHOLD = 7;
	if ( ( stepsLeft + stepsRight + 2 ) < ARBITRARILY_CHOSEN_THRESHOLD )
		insert_sort( left, right, comp_ );
	else {
		if ( stepsLeft < ARBITRARILY_CHOSEN_THRESHOLD )
			insert_sort( left, leftIt, comp_ );
		else
			merge_sort( left, leftIt, comp_ );
		if ( stepsRight < ARBITRARILY_CHOSEN_THRESHOLD )
			insert_sort( rightIt, right, comp_ );
		else
			merge_sort( rightIt, right, comp_ );
		HElement* first = NULL;
		++ stepsLeft;
		while ( stepsLeft -- ) {
			if ( comp_( rightIt->_value, left->_value ) ) {
				HElement* ptr = rightIt;
				if ( ! first )
					first = ptr;
				while ( ( rightIt != right ) && comp_( rightIt->_next->_value, left->_value ) )
					rightIt = rightIt->_next;
				HElement* nextRight = rightIt->_next;
				bool to_break = false;
				if ( rightIt == right )
					to_break = true;
				ptr->_previous->_next = rightIt->_next;
				rightIt->_next->_previous = ptr->_previous;
				left->_previous->_next = ptr;
				ptr->_previous = left->_previous;
				left->_previous = rightIt;
				rightIt->_next = left;
				rightIt = nextRight;
				if ( to_break ) {
					right = rightIt->_previous;
					break;
				}
			}
			if ( ! first )
				first = left;
			left = left->_next;
		}
		left = first;
	}
	return;
	M_EPILOG
}

template<typename type_t>
template<typename T>
void HList<type_t>::insert_sort(
		HElement*& baseLower_, HElement*& baseUpper_,
		T const& comp_ ) {
	M_PROLOG
	if ( baseLower_ != baseUpper_ ) {
		HElement* top = baseLower_;
		while ( top != baseUpper_ ) {
			top = top->_next;
			HElement* ptr = top;
			while ( ( ptr != baseLower_ ) && comp_( top->_value, ptr->_previous->_value ) )
				ptr = ptr->_previous;
			if ( ptr != top ) {
				HElement* oldtop = top->_previous;
				insert( ptr, top );
				if ( top == baseUpper_ )
					baseUpper_ = oldtop;
				if ( ptr == baseLower_ )
					baseLower_ = top;
				top = oldtop;
			}
		}
		baseUpper_ = top;
	}
	return;
	M_EPILOG
}

template<typename type_t>
void HList<type_t>::insert( HElement* pos, HElement* elem ) {
	M_ASSERT( pos != elem );
	if ( pos->_next == elem )
		exchange( pos, elem );
	else {
		if ( pos == _hook )
			_hook = elem;
		if ( pos == _indexElement )
			_indexElement = elem;
		if ( pos->_previous != elem ) {
			elem->_next->_previous = elem->_previous;
			elem->_previous->_next = elem->_next;
			elem->_next = pos;
			elem->_previous = pos->_previous;
			pos->_previous->_next = elem;
			pos->_previous = elem;
		}
	}
	return;
}

template<typename type_t>
void HList<type_t>::sort( void ) {
	M_PROLOG
	sort( yaal::less<type_t>() );
	return;
	M_EPILOG
}

template<typename type_t>
template<typename T>
void HList<type_t>::sort( T const& comp_ ) {
	M_PROLOG
	if ( _size > 1 ) {
		HElement* first = _hook;
		HElement* last = _hook->_previous;
		merge_sort( first, last, comp_ );
		_hook = first;
		_indexElement = NULL;
		_index = 0;
	}
	return;
	M_EPILOG;
}

template<typename type_t>
bool HList<type_t>::operator == ( HList const& l_ ) const {
	M_PROLOG
	return ( ( &l_ == this ) || safe_equal( begin(), end(), l_.begin(), l_.end() ) );
	M_EPILOG
}

template<typename type_t>
bool HList<type_t>::operator < ( HList const& l_ ) const {
	M_PROLOG
	return ( ( &l_ != this ) && lexicographical_compare( begin(), end(), l_.begin(), l_.end() ) );
	M_EPILOG
}

}

template<typename type_t>
inline void swap( yaal::hcore::HList<type_t>& a, yaal::hcore::HList<type_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HLIST_HXX_INCLUDED */


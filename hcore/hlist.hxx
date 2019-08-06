/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hlist.hxx
 * \brief Declaration and implementation of HList<> class template.
 */

#ifndef YAAL_HCORE_HLIST_HXX_INCLUDED
#define YAAL_HCORE_HLIST_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/base.hxx"
#include "hcore/allocator.hxx"
#include "hcore/math.hxx"
#include "hcore/functional.hxx"
#include "hcore/pod.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHList_[];

/*! \brief Doubly-linked list data structure and its operations.
 */
template<typename type_t, typename allocator_t = allocator::system<type_t> >
class HList final {
public:
	typedef HList<type_t, allocator_t> this_type;
	typedef type_t value_type;
	typedef int long size_type;
	/*! \brief Error codes for HList<> operations.
	 */
	struct ERROR {
		/*! \brief Error flags.
		 */
		typedef enum {
			OK = 0,     /*!< No error. */
			EMPTY       /*!< List is empty. */
		} error_t;
	};
	template<typename const_qual_t>
	class HIterator;
private:

#ifndef __sun__
#	pragma pack( push, 1 )
#else /* #ifndef __sun__ */
#	pragma pack( 1 )
#endif /* #else #ifndef __sun__ */

	/*! \brief HList<> element class provisions basic building block for doubly-linked list.
	 */
	class HElement {
	private:
		HElement* _previous;
		HElement* _next;
		type_t _value; /*!< The Object itself. */
		template<typename... arg_t>
		explicit HElement( HElement* element_, trait::true_type const*, arg_t&&... arg_ )
			: _previous( nullptr )
			, _next( nullptr )
			, _value( yaal::forward<arg_t>( arg_ )... ) {
			connect( element_ );
		}
		explicit HElement( HElement* element_, trait::false_type const*, type_t const& value_ )
			: _previous( nullptr )
			, _next( nullptr )
			, _value( value_ ) {
			connect( element_ );
		}
		explicit HElement( HElement* element_, trait::false_type const*, type_t&& value_ )
			: _previous( nullptr )
			, _next( nullptr )
			, _value( yaal::move( value_ ) ) {
			connect( element_ );
		}

		~HElement( void ) {
			_previous->_next = _next;
			_next->_previous = _previous;
			return;
		}
		void connect( HElement* element_ ) {
			if ( element_ == nullptr ) {
				_previous = this;
				_next = this;
			} else {
				_previous = element_->_previous;
				_next = element_;
				element_->_previous = this;
				_previous->_next = this;
			}
		}
	private:
		HElement( HElement const& );
		HElement& operator = ( HElement const& );
		friend class HList<type_t, allocator_t>;
		friend class HIterator<type_t>;
	};

#ifndef __sun__
#	pragma pack( pop )
#else /* #ifndef __sun__ */
#	pragma pack()
#endif /* #else #ifndef __sun__ */

public:
	typedef typename allocator_t::template rebind<HElement>::other allocator_type;
	typedef trait::delayed_sizeof<HElement> node_size;
	typedef HElement node_type;

private:
	allocator_type _allocator;
	size_type _size;  /*!< how many elements this list contains */
	HElement* _hook; /*!< "beginning" of the list ( "first" element ) */

public:
	typedef class HIterator<type_t> iterator;
	typedef class HIterator<type_t const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;

	/*! \brief Create an empty list.
	 */
	HList( void )
		: _allocator( allocator_type() )
		, _size( 0 )
		, _hook( nullptr ) {
		return;
	}

	/*! \brief Create an empty list.
	 */
	explicit HList( allocator_type const& allocator_ )
		: _allocator( allocator_ )
		, _size( 0 )
		, _hook( nullptr ) {
		return;
	}

	/*! \brief Creates list, with specified size.
	 *
	 * \param count_ - number of element for newly created list.
	 */
	explicit HList( size_type count_ )
		: _allocator( allocator_type() )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		while ( count_ -- ) {
			add_tail();
		}
		return;
		M_EPILOG
	}

	/*! \brief Creates list, with specified size.
	 *
	 * \param count_ - number of element for newly created list.
	 */
	HList( size_type count_, allocator_type const& allocator_ )
		: _allocator( allocator_ )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		while ( count_ -- ) {
			add_tail();
		}
		return;
		M_EPILOG
	}

	/*! \brief Copy constructor.
	 *
	 * Creates new list by copying contents of given existing one.
	 *
	 * \param list_ - an existing list to copy.
	 */
	HList( HList const& list_ )
		: _allocator( list_._allocator )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		( *this ) = list_;
		return;
		M_EPILOG
	}

	HList( HList&& list_ )
		: _allocator()
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		swap( list_ );
		return;
		M_EPILOG
	}

	HList( HList const& list_, allocator_type const& allocator_ )
		: _allocator( allocator_ )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		( *this ) = list_;
		return;
		M_EPILOG
	}

	/*! \brief Create list and initialize it with copies of given value.
	 *
	 * \param count_ - size of new list.
	 * \param value_ - list initializer value.
	 */
	HList( size_type count_, type_t const& value_, allocator_type const& allocator_ = allocator_type() )
		: _allocator( allocator_ )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		resize( count_, value_ );
		return;
		M_EPILOG
	}

	/*! \brief Creates list, from range of elements.
	 *
	 * \param first_ - beginning of range to copy into newly created list.
	 * \param last_ - one past the end of range to copy into newly created list.
	 */
	template<typename iter_t>
	HList( iter_t first_, iter_t last_, allocator_type const& allocator_ = allocator_type() )
		: _allocator( allocator_ )
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		initialize( first_, last_, typename trait::add_pointer<typename is_integral<iter_t>::type>::type() );
		return;
		M_EPILOG
	}

	/*! \brief Construct list based on compile time constant data set.
	 *
	 * \tparam T - type of compile time constants to insert into this list.
	 * \param constants_ - set of compile time constants to into into this list.
	 */
	template<typename T>
	HList( std::initializer_list<T> constants_ )
		: _allocator()
		, _size( 0 )
		, _hook( nullptr ) {
		M_PROLOG
		initialize( constants_.begin(), constants_.end(), static_cast<trait::false_type*>( nullptr ) );
		return;
		M_EPILOG
	}

	/*! \brief Destructor.
	 *
	 * Destroy list and all its contents.
	 */
	~HList( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HList& operator = ( HList const& list_ ) {
		M_PROLOG
		if ( &list_ != this ) {
			iterator thisIt = begin();
			const_iterator otherIt = list_.begin();
			size_type ctr( 0 );
			size_type count( _size < list_._size ? _size : list_._size );
			if ( count > 0 ) {
				for ( ; ctr < count; ++ ctr, ++ thisIt, ++ otherIt ) {
					*thisIt = *otherIt;
					if ( otherIt._current == list_._hook ) {
						_hook = thisIt._current;
					}
				}
			}
			if ( _size > list_._size ) {
				count = _size - list_._size;
				while ( count -- ) {
					pop_back();
				}
			} else if ( _size < list_._size ) {
				for ( ; ctr < list_._size; ++ ctr, ++ otherIt ) {
					push_back( *otherIt );
				}
			}
			_size = list_._size;
		}
		return ( *this );
		M_EPILOG
	}
	HList& operator = ( HList&& list_ ) {
		M_PROLOG
		if ( &list_ != this ) {
			swap( list_ );
			list_.clear();
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HList& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _size, other._size );
			swap( _hook, other._hook );
			swap( _allocator, other._allocator );
		}
		return;
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	const_iterator begin( void ) const {
		return ( const_iterator( this, _hook ) );
	}
	const_iterator cbegin( void ) const {
		return ( const_iterator( this, _hook ) );
	}
	iterator begin( void ) {
		return ( iterator( this, _hook ) );
	}
	const_iterator end( void ) const {
		return ( const_iterator( this, nullptr ) );
	}
	const_iterator cend( void ) const {
		return ( const_iterator( this, nullptr ) );
	}
	iterator end( void ) {
		return ( iterator( this, nullptr ) );
	}
	const_reverse_iterator rbegin( void ) const {
		return ( end() );
	}
	const_reverse_iterator crbegin( void ) const {
		return ( end() );
	}
	reverse_iterator rbegin( void ) {
		return ( end() );
	}
	const_reverse_iterator  rend( void ) const {
		return ( begin() );
	}
	const_reverse_iterator  crend( void ) const {
		return ( begin() );
	}
	reverse_iterator rend( void ) {
		return ( begin() );
	}
	void clear( void ) {
		M_PROLOG
		while ( _size -- ) {
			HElement* element( _hook->_next );
			M_SAFE( element->~HElement() );
			_allocator.deallocate( element, 1 );
		}
		_hook = nullptr;
		_size = 0;
		return ;
		M_EPILOG
	}
	void resize( size_type size_, type_t const& value_ = type_t() ) {
		M_PROLOG
		size_type diff( math::abs( _size - size_ ) );
		if ( _size > size_ ) {
			while ( diff -- ) {
				pop_back();
			}
		} else if ( _size < size_ ) {
			while ( diff -- ) {
				push_back( value_ );
			}
		}
		return;
		M_EPILOG
	}

	/*! \brief Get list size.
	 *
	 * \return Number of element in this list.
	 */
	size_type size( void ) const {
		return ( _size );
	}

	/*! \brief Get list size.
	 *
	 * \return Number of element in this list.
	 */
	size_type get_size( void ) const {
		return ( _size );
	}
	template<typename iterator_t>
	void assign( iterator_t first, iterator_t last ) {
		M_PROLOG
		assign( first, last, typename trait::add_pointer<typename is_integral<iterator_t>::type>::type() );
		return;
		M_EPILOG
	}
	void assign( size_type size_, type_t const& fillWith_ ) {
		M_PROLOG
		assign( size_, fillWith_, static_cast<trait::true_type const*>( nullptr ) );
		return;
		M_EPILOG
	}

	/*! \brief Adds new element at specified position.
	 *
	 * Newly created element will have default value.
	 */
	iterator insert( iterator const& it ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( it._current ? it._current : _hook, static_cast<trait::true_type const*>( nullptr ) );
		if ( ( _size == 0 ) || ( it._current == _hook ) ) {
			_hook = element;
		}
		++ _size;
		return ( iterator( this, element ) );
		M_EPILOG
	}
	iterator insert( iterator const& it, type_t const& val ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( it._current ? it._current : _hook, static_cast<trait::false_type const*>( nullptr ), val );
		if ( ( _size == 0 ) || ( it._current == _hook ) ) {
			_hook = element;
		}
		++ _size;
		return ( iterator( this, element ) );
		M_EPILOG
	}
	iterator insert( iterator const& it, type_t&& val ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( it._current ? it._current : _hook, static_cast<trait::false_type const*>( nullptr ), yaal::move( val ) );
		if ( ( _size == 0 ) || ( it._current == _hook ) ) {
			_hook = element;
		}
		++ _size;
		return ( iterator( this, element ) );
		M_EPILOG
	}
	template<typename... arg_t>
	iterator emplace( iterator const& it, arg_t&&... arg_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( it._current ? it._current : _hook, static_cast<trait::true_type const*>( nullptr ), yaal::forward<arg_t>( arg_ )... );
		if ( ( _size == 0 ) || ( it._current == _hook ) ) {
			_hook = element;
		}
		++ _size;
		return ( iterator( this, element ) );
		M_EPILOG
	}
	template<typename iterator_t>
	void insert( iterator const& it, iterator_t first, iterator_t last ) {
		M_PROLOG
		for ( ; first != last; ++ first ) {
			insert( it, *first );
		}
		return;
		M_EPILOG
	}
	void insert( iterator const& it, size_type count_, type_t const& val ) {
		M_PROLOG
		for ( size_type i = 0; i < count_; ++ i ) {
			insert( it, val );
		}
		return;
		M_EPILOG
	}

	/*! \brief Adds new element at beginning of the list
	 *
	 * Newly created element will have default value.
	 */
	type_t& add_head( void ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::true_type const*>( nullptr ) );
		_hook = element;
		++ _size;
		return ( _hook->_value );
		M_EPILOG
	}

	/*! \brief Adds new element at end of the list
	 *
	 * Newly created element will have default value.
	 */
	type_t& add_tail( void ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::true_type const*>( nullptr ) );
		if ( _size == 0 ) {
			_hook = element;
		}
		++ _size;
		return ( element->_value );
		M_EPILOG
	}
	void push_back( type_t const& object_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::false_type const*>( nullptr ), object_ );
		if ( _size == 0 ) {
			_hook = element;
		}
		++ _size;
		return;
		M_EPILOG
	}
	void push_back( type_t&& object_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::false_type const*>( nullptr ), yaal::move( object_ ) );
		if ( _size == 0 ) {
			_hook = element;
		}
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void emplace_back( arg_t&&... arg_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::true_type const*>( nullptr ), yaal::forward<arg_t>( arg_ )... );
		if ( _size == 0 ) {
			_hook = element;
		}
		++ _size;
		return;
		M_EPILOG
	}
	void pop_back( void ) {
		M_PROLOG
		if ( _size > 0 ) {
			HElement* element( _hook->_previous );
			M_SAFE( element->~HElement() );
			_allocator.deallocate( element, 1 );
			-- _size;
			if ( _size == 0 ) {
				_hook = nullptr;
			}
		} else
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		return;
		M_EPILOG
	}
	void push_front( type_t const& object_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::false_type const*>( nullptr ), object_ );
		_hook = element;
		++ _size;
		return;
		M_EPILOG
	}
	void push_front( type_t&& object_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::false_type const*>( nullptr ), yaal::move( object_ ) );
		_hook = element;
		++ _size;
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void emplace_front( arg_t&&... arg_ ) {
		M_PROLOG
		HElement* element( _allocator.allocate( 1 ) );
		new ( element ) HElement( _hook, static_cast<trait::true_type const*>( nullptr ), yaal::forward<arg_t>( arg_ )... );
		_hook = element;
		++ _size;
		return;
		M_EPILOG
	}
	void pop_front( void ) {
		M_PROLOG
		HElement* element = nullptr;
		if ( _size > 0 ) {
			element = _hook;
			_hook = _hook->_next;
		} else {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
		M_SAFE( element->~HElement() );
		_allocator.deallocate( element, 1 );
		-- _size;
		if ( _size == 0 ) {
			_hook = nullptr;
		}
		return;
		M_EPILOG
	}
	iterator erase( iterator const& iterator_ ) {
		M_PROLOG
		iterator it( iterator_ );
		++ it;
		if ( ! _size ) {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
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
		if ( iterator_._current == _hook ) {
			_hook = _hook->_next;
		}
		HElement* element( iterator_._current );
		M_SAFE( element->~HElement() );
		_allocator.deallocate( element, 1 );
		-- _size;
		if ( _size == 0 ) {
			_hook = nullptr;
		}
		return ( it );
		M_EPILOG
	}
	iterator erase( iterator first_, iterator const& last_ ) {
		M_PROLOG
		while ( first_ != last_ ) {
			first_ = erase( first_ );
		}
		return ( first_ );
		M_EPILOG
	}
	type_t& front( void ) {
		M_PROLOG
		if ( _hook == nullptr ) {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
		return ( _hook->_value );
		M_EPILOG
	}
	type_t const& front( void ) const {
		M_PROLOG
		if ( _hook == nullptr ) {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
		return ( _hook->_value );
		M_EPILOG
	}
	type_t& back( void ) {
		M_PROLOG
		if ( _hook == nullptr ) {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
		return ( _hook->_previous->_value );
		M_EPILOG
	}
	type_t const& back( void ) const {
		M_PROLOG
		if ( _hook == nullptr ) {
			M_THROW( _errMsgHList_[ ERROR::EMPTY ], errno );
		}
		return ( _hook->_previous->_value );
		M_EPILOG
	}
	type_t& head( void ) {
		M_PROLOG
		return ( front() );
		M_EPILOG
	}
	type_t const& head( void ) const {
		M_PROLOG
		return ( front() );
		M_EPILOG
	}
	type_t& tail( void ) {
		M_PROLOG
		return ( back() );
		M_EPILOG
	}
	type_t const& tail( void ) const {
		M_PROLOG
		return ( back() );
		M_EPILOG
	}
	void splice( iterator it_, HList& list_ ) {
		M_PROLOG
		M_ASSERT( it_._owner == this );
		M_ENSURE( &list_ != this );
		if ( list_._hook ) {
			HElement* e( _hook ? ( it_._current ? it_._current : _hook ) : nullptr );
			if ( e ) {
				HElement* p( e->_previous );
				p->_next = list_._hook;
				e->_previous = list_._hook->_previous;
				list_._hook->_previous->_next = e;
				list_._hook->_previous = p;
			}
			if ( it_._current == _hook ) {
				_hook = list_._hook;
			}
			_size += list_._size;
			list_._hook = nullptr;
			list_._size = 0;
		}
		return;
		M_EPILOG
	}
	void splice( iterator to_, HList& list_, iterator from_ ) {
		M_PROLOG
		M_ASSERT( to_._owner == this );
		M_ASSERT( from_._owner == &list_ );
		M_ASSERT( from_._current && ( list_._size > 0 ) );
		HElement* e( _hook ? ( to_._current ? to_._current : _hook ) : nullptr );
		if ( ( from_._current != to_._current ) && ( from_._current->_next != to_._current ) ) {
			if ( from_._current == list_._hook ) {
				list_._hook = ( list_._hook->_next != list_._hook ) ? list_._hook->_next : nullptr;
			}
			if ( to_._current == _hook ) {
				_hook = from_._current;
			}
			from_._current->_previous->_next = from_._current->_next;
			from_._current->_next->_previous = from_._current->_previous;
			if ( e ) {
				from_._current->_next = e;
				from_._current->_previous = e->_previous;
				e->_previous->_next = from_._current;
				e->_previous = from_._current;
			} else {
				_hook->_next = _hook;
				_hook->_previous = _hook;
			}
			-- list_._size;
			++ _size;
		}
		return;
		M_EPILOG
	}
	void splice( iterator it_, HList& list_, iterator first_, iterator last_ ) {
		M_PROLOG
		M_ASSERT( it_._owner == this );
		M_ASSERT( first_._owner == &list_ );
		M_ASSERT( last_._owner == &list_ );
		M_ASSERT( first_._current || ( first_._current == last_._current ) );
		if ( first_._current != last_._current ) {
			HElement* to( _hook ? ( it_._current ? it_._current : _hook ) : nullptr );
			HElement* last( last_._current ? last_._current : list_._hook );
			if ( ( ( last_._current != to ) || ( &list_ != this ) ) && ( last->_previous != to ) ) {
				size_type count( ( &list_ != this ) ? distance( first_, last_ ) : 0 );
				if ( first_._current == list_._hook ) {
					list_._hook = last_._current;
				}
				HElement* lastPrev( last->_previous );
				first_._current->_previous->_next = last;
				last->_previous = first_._current->_previous;
				if ( to ) {
					first_._current->_previous = to->_previous;
					to->_previous->_next = first_._current;
					lastPrev->_next = to;
					to->_previous = lastPrev;
				} else {
					first_._current->_previous = lastPrev;
					lastPrev->_next = first_._current;
				}
				if ( ! to || ( it_._current == _hook ) ) {
					_hook = first_._current;
				}
				list_._size -= count;
				_size += count;
			}
		}
		return;
		M_EPILOG
	}
	void remove( type_t const& value_ ) {
		M_PROLOG
		for ( iterator it( begin() ), endIt( end() ); it != endIt; ) {
			if ( *it == value_ ) {
				it = erase( it );
			} else {
				++ it;
			}
		}
		return;
		M_EPILOG
	}
	template<typename cond_t>
	void remove_if( cond_t condition_ ) {
		M_PROLOG
		for ( iterator it( begin() ), endIt( end() ); it != endIt; ) {
			if ( condition_( *it ) ) {
				it = erase( it );
			} else {
				++ it;
			}
		}
		return;
		M_EPILOG
	}
	void unique( void ) {
		M_PROLOG
		iterator it( begin() );
		iterator follow( begin() );
		iterator endIt( end() );
		++ it;
		while ( it != endIt ) {
			if ( *it == *follow ) {
				it = erase( it );
			} else {
				++ it;
				++ follow;
			}
		}
		return;
		M_EPILOG
	}
	void reverse( void ) {
		M_PROLOG
		if ( _hook ) {
			_hook = _hook->_previous;
			HElement* e( _hook->_previous );
			using yaal::swap;
			while ( e != _hook ) {
				swap( e->_previous, e->_next );
				e = e->_next;
			}
			swap( _hook->_previous, _hook->_next );
		}
		return;
		M_EPILOG
	}
	void exchange( iterator const& left, iterator const& right ) {
		M_PROLOG
		if ( left != right ) {
			exchange( left._current, right._current );
		}
		return;
		M_EPILOG
	}
	void sort( void ) {
		M_PROLOG
		sort( yaal::less<type_t>() );
		return;
		M_EPILOG
	}
	template<typename T>
	void sort( T const& comp_ ) {
		M_PROLOG
		if ( _size > 1 ) {
			HElement* first = _hook;
			HElement* last = _hook->_previous;
			merge_sort( first, last, comp_ );
			_hook = first;
		}
		return;
		M_EPILOG;
	}
	bool is_empty( void ) const {
		return ( ! _size );
	}
	bool empty( void ) const {
		return ( ! _size );
	}
	bool operator == ( HList const& l_ ) const {
		M_PROLOG
		return ( ( &l_ == this ) || safe_equal( begin(), end(), l_.begin(), l_.end() ) );
		M_EPILOG
	}
	bool operator < ( HList const& l_ ) const {
		M_PROLOG
		return ( ( &l_ != this ) && lexicographical_compare( begin(), end(), l_.begin(), l_.end() ) );
		M_EPILOG
	}
private:
	template<typename iterator_t>
	void initialize( iterator_t first, iterator_t last, trait::false_type const* ) {
		M_PROLOG
		for ( ; first != last; ++ first )
			push_back( *first );
		return;
		M_EPILOG
	}
	void initialize( size_type size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void assign( iterator_t first, iterator_t last, trait::false_type const* ) {
		M_PROLOG
		size_type count( 0 );
		for ( iterator it( begin() ), endIt( end() ); ( it != endIt ) && ( first != last ); ++ it, ++ first, ++ count ) {
			*it = *first;
		}
		if ( first != last ) {
			for ( ; first != last; ++ first ) {
				push_back( *first );
			}
		} else {
			resize( count );
		}
		return;
		M_EPILOG
	}
	void assign( size_type size_, type_t const& fillWith_, trait::true_type const* ) {
		M_PROLOG
		fill_n( begin(), min( size_, _size ), fillWith_ );
		resize( size_, fillWith_ );
		return;
		M_EPILOG
	}
	template<typename T>
	void merge_sort( HElement*& left, HElement*& right, T const& comp_ ) {
		M_PROLOG
		HElement* leftIt = left;
		HElement* rightIt = right;
		size_type stepsLeft = 0;
		size_type stepsRight = 0;
		while ( leftIt != rightIt ) {
			if ( leftIt->_next == rightIt ) {
				break;
			}
			leftIt = leftIt->_next;
			++ stepsLeft;
			if ( leftIt == rightIt->_previous ) {
				break;
			}
			rightIt = rightIt->_previous;
			++ stepsRight;
		}
		int const ARBITRARILY_CHOSEN_THRESHOLD = 7;
		if ( ( stepsLeft + stepsRight + 2 ) < ARBITRARILY_CHOSEN_THRESHOLD ) {
			insert_sort( left, right, comp_ );
		} else {
			if ( stepsLeft < ARBITRARILY_CHOSEN_THRESHOLD ) {
				insert_sort( left, leftIt, comp_ );
			} else {
				merge_sort( left, leftIt, comp_ );
			}
			if ( stepsRight < ARBITRARILY_CHOSEN_THRESHOLD ) {
				insert_sort( rightIt, right, comp_ );
			} else {
				merge_sort( rightIt, right, comp_ );
			}
			HElement* first = nullptr;
			++ stepsLeft;
			while ( stepsLeft -- ) {
				if ( comp_( rightIt->_value, left->_value ) ) {
					HElement* ptr = rightIt;
					if ( ! first ) {
						first = ptr;
					}
					while ( ( rightIt != right ) && comp_( rightIt->_next->_value, left->_value ) ) {
						rightIt = rightIt->_next;
					}
					HElement* nextRight = rightIt->_next;
					bool to_break = false;
					if ( rightIt == right ) {
						to_break = true;
					}
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
				if ( ! first ) {
					first = left;
				}
				left = left->_next;
			}
			left = first;
		}
		return;
		M_EPILOG
	}

	template<typename T>
	void insert_sort(
			HElement*& baseLower_, HElement*& baseUpper_,
			T const& comp_ ) {
		M_PROLOG
		if ( baseLower_ != baseUpper_ ) {
			HElement* top = baseLower_;
			while ( top != baseUpper_ ) {
				top = top->_next;
				HElement* ptr = top;
				while ( ( ptr != baseLower_ ) && comp_( top->_value, ptr->_previous->_value ) ) {
					ptr = ptr->_previous;
				}
				if ( ptr != top ) {
					HElement* oldtop = top->_previous;
					insert( ptr, top );
					if ( top == baseUpper_ ) {
						baseUpper_ = oldtop;
					}
					if ( ptr == baseLower_ ) {
						baseLower_ = top;
					}
					top = oldtop;
				}
			}
			baseUpper_ = top;
		}
		return;
		M_EPILOG
	}
	void insert( HElement* pos, HElement* elem ) {
		M_ASSERT( pos != elem );
		if ( pos->_next == elem ) {
			exchange( pos, elem );
		} else {
			if ( pos == _hook ) {
				_hook = elem;
			}
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
	void exchange( HElement* left_, HElement* right_ ) {
		M_PROLOG
		HElement* next = nullptr;
		HElement* previous = nullptr;
		if ( left_ == right_ ) {
			return;
		}
		if ( left_ == _hook ) {
			_hook = right_;
		} else if ( right_ == _hook ) {
			_hook = left_;
		}
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
	friend class HIterator<type_t>;
	friend class HIterator<type_t const>;
};

/*! \brief Iterator for HList<> data structure.
 */
template<typename type_t, typename allocator_t>
template<typename const_qual_t>
class HList<type_t, allocator_t>::HIterator : public iterator_interface<const_qual_t, iterator_category::forward> {
private:
	typedef HList<type_t, allocator_t> owner_t;
	owner_t const* _owner;
	HElement* _current;
public:
	typedef iterator_interface<const_qual_t, iterator_category::forward> base_type;
	HIterator( void )
		: base_type()
		, _owner( nullptr )
		, _current( nullptr ) {
		return;
	}
	HIterator( HIterator const& iterator_ )
		: base_type()
		, _owner( iterator_._owner )
		, _current( iterator_._current ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& iterator_ )
		: base_type()
		, _owner( iterator_._owner )
		, _current( iterator_._current ) {
		M_PROLOG
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value,
			"creating non-const iterator instance discards qualifiers"
		);
		return;
		M_EPILOG
	}
	HIterator& operator ++ ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( _current ) {
			_current = _current->_next;
			if ( _current == _owner->_hook ) {
				_current = nullptr;
			}
		} else {
			_current = _owner->_hook;
		}
		return ( *this );
		M_EPILOG
	}
	HIterator const operator ++ ( int ) {
		M_PROLOG
		HIterator iterator( *this );
		operator ++ ();
		return ( iterator );
		M_EPILOG
	}
	HIterator& operator -- ( void ) {
		M_PROLOG
		M_ASSERT( _owner );
		if ( _current ) {
			_current = _current->_previous;
			if ( _current == _owner->_hook->_previous ) {
				_current = nullptr;
			}
		} else if ( _owner->_hook ) {
			_current = _owner->_hook->_previous;
		}
		return ( *this );
		M_EPILOG
	}
	HIterator const operator -- ( int ) {
		M_PROLOG
		HIterator iterator( *this );
		operator -- ();
		return ( iterator );
		M_EPILOG
	}
	HIterator& operator = ( HIterator const& iterator_ ) {
		M_PROLOG
		if ( reinterpret_cast<HIterator<const_qual_t> const*>( &iterator_ ) != this ) {
			_owner = iterator_._owner;
			_current = iterator_._current;
		}
		return ( *this );
		M_EPILOG
	}
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& iterator_ ) const {
		M_PROLOG
		M_ASSERT( ( ! ( _owner && iterator_._owner ) ) || ( _owner == iterator_._owner ) );
		return ( _current == iterator_._current );
		M_EPILOG
	}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& iterator_ ) const {
		M_PROLOG
		M_ASSERT( ( ! ( _owner && iterator_._owner ) ) || ( _owner == iterator_._owner ) );
		return ( _current != iterator_._current );
		M_EPILOG
	}
	const_qual_t& operator* ( void ) const {
		return ( _current->_value );
	}
	const_qual_t* operator-> ( void ) const {
		return ( &_current->_value );
	}
	void const* node_id( void ) const {
		return ( _current );
	}
protected:
	friend class HList<type_t, allocator_t>;
	HIterator( owner_t const* owner_, HElement* element_ )
		: base_type()
		, _owner( owner_ )
		, _current( element_ ) {
		return;
	}
};

template<typename type_t, typename allocator_t>
inline void swap( yaal::hcore::HList<type_t, allocator_t>& a, yaal::hcore::HList<type_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HLIST_HXX_INCLUDED */


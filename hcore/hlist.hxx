/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlist.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HLIST_HXX_INCLUDED
#define YAAL_HCORE_HLIST_HXX_INCLUDED

#include "hcore/base.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

extern char const * const g_ppcErrMsgHList [ ];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ general list declaration ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template<typename value_type> 
class HList;

/*! \brief Meta-data definition for HList<> class.
 */
struct OListBits
	{
	/*! \brief Error codes for HList<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error flags.
		 */
		typedef enum
			{
			OK = 0,    /*!< No error. */
			BAD_INDEX, /*!< Bad elements index used. */
			EMPTY,     /*!< List is empty. */
			BAD_ORDER  /*!< Bad order flag. */
			} error_t;
		};
	typedef enum
		{
		TREAT_AS_CLOSED = 1,
		TREAT_AS_OPENED = 2
		} treatment_t;
	typedef enum
		{
		OK = 0,
		ERROR = 1,
		FINAL_REACHED = 2,
		NOT_FOUND = 4
		} status_t;
	typedef enum
		{
		UNSORTED,
		ASCENDING,
		DESCENDING
		} sort_order_t;
	virtual ~OListBits( void ) { } 
	/*! \brief HList<>::iterator type constructor.
	 *
	 * \tparam T - type of elements stored in HList<>.
	 * \tparam Q - iterator bahavior.
	 * \retval type - new iterator type.
	 */
	template<typename T, OListBits::treatment_t Q>
	struct iterator
		{
		typedef typename HList<T>::template HIterator<T,Q> type;
		};
	};

/*! \brief Doubly-linked list data structure and its operations.
 */
template<typename value_type> 
class HList : public OListBits
	{
	typedef HList<value_type> self_t;
private:
	class HElement;
	int f_iSize;           /*!< how many elements this list contains */
	HElement* f_poHook;    /*!< "begining" of the list ( "first" element ) */
/* for internal use only */
	sort_order_t f_eOrder; /*!< last-to-current sort order */
	int f_iIndex;          /*!< this two fiels will allow boost operator[], int holds last */
	HElement* f_poIndex;   /*!< index and HElement * holds pointer to this last element */
public:
	typedef value_type value_t;
	template<typename const_qual_t, OListBits::treatment_t const treatment>
	class HIterator;
	typedef class HIterator<value_type, OListBits::TREAT_AS_OPENED> iterator;
	typedef class HIterator<value_type const, OListBits::TREAT_AS_OPENED> const_iterator;
	typedef class HIterator<value_type, OListBits::TREAT_AS_CLOSED> cyclic_iterator;
	typedef class HIterator<value_type const, OListBits::TREAT_AS_CLOSED> const_cyclic_iterator;
	/*! \brief Creates list, with specified size.
	 *
	 * \param count - number of element for newly created list.
	 */
	HList( int count = 0 );
	virtual ~HList( void );
	HList( HList const& );
	HList& operator = ( HList const& );
	/*! \brief Creates list, from range of elements.
	 */
	template<typename iter_t>
	HList( iter_t, iter_t );
	cyclic_iterator hook( void );
	const_cyclic_iterator hook( void ) const;
	iterator begin( void );
	iterator end( void );
	iterator rend( void );
	iterator rbegin( void );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	const_iterator rend( void ) const;
	const_iterator rbegin( void ) const;
	void clear( void );
	int long size( void ) const;
	int long get_size( void ) const;
	void swap( HList<value_type>& );
	/*! \brief Adds new element at specified position.
	 */
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<value_type, treatment>::type insert( HIterator<value_type, treatment> const&, value_type const* = NULL );
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<value_type, treatment>::type insert( HIterator<value_type, treatment> const&, value_type const& );
	/*! \brief Adds new element at beggining of the list
	 */
	value_type& add_head( value_type const* = NULL );
	/*! \brief Adds new element at end of the list
	 */
	value_type& add_tail( value_type const* = NULL );
	void push_back( value_type const& );
	void pop_back( void );
	void push_front( value_type const& );
	void pop_front( void );
	/*! \brief Add element in the way that keeps order.
	 */
	template<typename T>
	value_type& add_orderly( value_type const&, T const&, sort_order_t = ASCENDING );
	status_t remove_head( value_type** = NULL );
	status_t remove_tail( value_type** = NULL );
	template<OListBits::treatment_t const treatment>
	typename OListBits::iterator<value_type, treatment>::type erase( HIterator<value_type, treatment> const& );
	/*! \brief Sets cursor at specified index or number
	 */
	iterator n_th( int );
	value_type& operator[] ( int );
	value_type const& operator[] ( int ) const;
	value_type& front( void );
	value_type const& front( void ) const;
	value_type& back( void );
	value_type const& back( void ) const;
	value_type& head( void );
	value_type const& head( void ) const;
	value_type& tail( void );
	value_type const& tail( void ) const;
	void exchange( iterator const&, iterator const& );
	void sort( sort_order_t = ASCENDING );
	template<typename T>
	void sort( T const&, sort_order_t = ASCENDING );
	bool empty( void ) const;
	bool is_empty( void ) const;
private:
	template<typename T>
	void merge_sort( HElement*&, HElement*&, T const& );
	template<typename T>
	void insert_sort( HElement*&, HElement*&, T const& );
	void insert( HElement*, HElement* );
	template<typename T>
	void select_sort( HElement*&, HElement*&, int, T const& );
	HElement* element_by_index ( int );
	void exchange( HElement*, HElement* );
	void sub_swap( HElement*, HElement*, HElement* );
	friend class HIterator<value_type, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<value_type const, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<value_type, OListBits::TREAT_AS_CLOSED>;
	friend class HIterator<value_type const, OListBits::TREAT_AS_CLOSED>;
	};

/*! \brief HList<> element class provisions basic building block for doubly-linked list.
 */
template<typename value_type>
class HList<value_type>::HElement
	{
private:
	HElement* f_poPrevious;
	HElement* f_poNext;
	value_type f_tObject; /*!< The Object itself. */
	explicit HElement( HElement*, value_type const* );
	virtual ~HElement ();
	HElement( HElement const & );
	HElement& operator = ( HElement const& );
	friend class HList<value_type>;
	friend class HIterator<value_type, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<value_type const, OListBits::TREAT_AS_OPENED>;
	friend class HIterator<value_type, OListBits::TREAT_AS_CLOSED>;
	friend class HIterator<value_type const, OListBits::TREAT_AS_CLOSED>;
	};

/*! \brief Iterator for HList<> data structure.
 */
template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
class HList<value_type>::HIterator
	{
private:
	HList<value_type> const* f_poOwner;
	HElement* f_poCurrent;
public:
	typedef value_type value_t;
	HIterator( void );
	template<typename other_const_qual_t, OListBits::treatment_t family>
	HIterator( HIterator<other_const_qual_t, family> const& );
	HIterator& operator ++ ( void )
		{
		M_PROLOG
		if ( f_poCurrent )
			{
			f_poCurrent = f_poCurrent->f_poNext;
			if ( ( treatment == OListBits::TREAT_AS_OPENED ) && ( f_poCurrent == f_poOwner->f_poHook ) )
				f_poCurrent = NULL;
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator ++ ( int )
		{
		M_PROLOG
		HIterator l_oIterator ( *this );
		++ ( *this );
		return ( l_oIterator );
		M_EPILOG
		}
	HIterator& operator -- ( void )
		{
		M_PROLOG
		if ( f_poCurrent )
			{
			f_poCurrent = f_poCurrent->f_poPrevious;
			if ( ( treatment == OListBits::TREAT_AS_OPENED ) && ( f_poCurrent == f_poOwner->f_poHook->f_poPrevious ) )
				f_poCurrent = NULL;
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator -- ( int )
		{
		M_PROLOG
		HIterator l_oIterator( * this );
		-- ( *this );
		return ( l_oIterator );
		M_EPILOG
		}
	typename HList<value_type>::template HIterator<const_qual_t, treatment>& operator = ( HIterator const& );
	bool operator == ( HIterator const & ) const;
	bool operator != ( HIterator const & ) const;
	const_qual_t& operator* ( void );
	const_qual_t& operator* ( void ) const;
	const_qual_t* operator->( void );
	const_qual_t* operator->( void ) const;
	bool is_valid( void ) const;
protected:
	friend class HList<value_type>;
	template<typename same_const_qual_t, OListBits::treatment_t const family>
	friend class HIterator;
	HIterator( HList<value_type> const* const, HElement* const );
	};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*++++++++++++++++++++++ general list implementation ++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template<typename value_type>
HList<value_type>::HElement::HElement( HElement* a_poElement, value_type const* a_poValue )
	: f_poPrevious ( NULL ), f_poNext ( NULL ), f_tObject( a_poValue ? *a_poValue : value_type() )
	{
	if ( a_poElement == 0 )
		{
		f_poPrevious = this;
		f_poNext = this;
		}
	else
		{
		f_poPrevious = a_poElement->f_poPrevious;
		f_poNext = a_poElement;
		a_poElement->f_poPrevious = this;
		f_poPrevious->f_poNext = this;
		}
	}

template<typename value_type>
HList<value_type>::HElement::~HElement( void )
	{
	f_poPrevious->f_poNext = f_poNext;
	f_poNext->f_poPrevious = f_poPrevious;
	return;
	}

//========================== Iterator ========================================

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
HList<value_type>::HIterator<const_qual_t, treatment>::HIterator( void )
	: f_poOwner( NULL ), f_poCurrent( NULL )
	{
	return;
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
template<typename other_const_qual_t, OListBits::treatment_t family>
HList<value_type>::HIterator<const_qual_t, treatment>::HIterator( HIterator<other_const_qual_t, family> const& a_roIterator )
	: f_poOwner( a_roIterator.f_poOwner ), f_poCurrent( a_roIterator.f_poCurrent )
	{
	M_PROLOG
	STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
	return;
	M_EPILOG
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
HList<value_type>::HIterator<const_qual_t, treatment>::HIterator( HList<value_type> const* const a_poOwner,
		HElement* const a_poElement ) : f_poOwner( a_poOwner ),
	f_poCurrent( a_poElement )
	{
	return;
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
typename HList<value_type>::template HIterator<const_qual_t, treatment>&
HList<value_type>::HIterator<const_qual_t, treatment>::operator = ( HIterator const& a_roIterator )
	{
	M_PROLOG
	if ( reinterpret_cast<HIterator<const_qual_t, treatment> const*>( &a_roIterator ) != this )
		{
		f_poOwner = a_roIterator.f_poOwner;
		f_poCurrent = a_roIterator.f_poCurrent;
		}
	return ( *this );
	M_EPILOG
	}

/*
template<typename value_type>
typename HList<value_type>::HIterator & HList< value_type >::HIterator::operator ++ ( void )

template<typename value_type>
typename HList<value_type>::HIterator const HList< value_type >::HIterator::operator ++ ( int )

template<typename value_type>
typename HList<value_type>::HIterator & HList< value_type >::HIterator::operator -- ( void )

template<typename value_type>
typename HList<value_type>::HIterator const HList< value_type >::HIterator::operator -- ( int )
*/

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
bool HList<value_type>::HIterator<const_qual_t, treatment>::operator == ( HIterator const& a_roIterator ) const
	{
	M_PROLOG
	M_ASSERT( ( ! ( f_poOwner && a_roIterator.f_poOwner ) ) || ( f_poOwner == a_roIterator.f_poOwner ) );
	return ( f_poCurrent == a_roIterator.f_poCurrent );
	M_EPILOG
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
bool HList<value_type>::HIterator<const_qual_t, treatment>::operator!= ( HIterator const& a_roIterator ) const
	{
	M_PROLOG
	M_ASSERT( ( ! ( f_poOwner && a_roIterator.f_poOwner ) ) || ( f_poOwner == a_roIterator.f_poOwner ) );
	return ( f_poCurrent != a_roIterator.f_poCurrent );
	M_EPILOG
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t& HList<value_type>::HIterator<const_qual_t, treatment>::operator* ( void )
	{
	return ( f_poCurrent->f_tObject );
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t& HList<value_type>::HIterator<const_qual_t, treatment>::operator* ( void ) const
	{
	return ( f_poCurrent->f_tObject );
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t* HList<value_type>::HIterator<const_qual_t, treatment>::operator->( void )
	{
	return ( &f_poCurrent->f_tObject );
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
const_qual_t* HList<value_type>::HIterator<const_qual_t, treatment>::operator->( void ) const
	{
	return ( &f_poCurrent->f_tObject );
	}

template<typename value_type>
template<typename const_qual_t, OListBits::treatment_t const treatment>
bool HList<value_type>::HIterator<const_qual_t, treatment>::is_valid( void ) const
	{
	return ( f_poOwner && f_poOwner->f_poHook && f_poCurrent );
	}

//============================================================================

template<typename value_type>
HList<value_type>::HList( int a_iSize )
	: OListBits(), f_iSize( 0 ),
	f_poHook( NULL ), f_eOrder( UNSORTED ),
	f_iIndex( 0 ), f_poIndex( NULL )
	{
	M_PROLOG
	while ( a_iSize -- )
		add_tail();
	return;
	M_EPILOG
	}

template<typename value_type>
HList<value_type>::~HList( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename value_type>
HList<value_type>::HList( HList<value_type> const& a_roList )
	: OListBits(), f_iSize( 0 ),
	f_poHook( NULL ), f_eOrder( UNSORTED ),
	f_iIndex( 0 ), f_poIndex( NULL )
	{
	M_PROLOG
	( *this ) = a_roList;
	return;
	M_EPILOG
	}

template<typename value_type>
template<typename iter_t>
HList<value_type>::HList( iter_t i, iter_t endIt )
	: OListBits(), f_iSize( 0 ),
	f_poHook( NULL ), f_eOrder( UNSORTED ),
	f_iIndex( 0 ), f_poIndex( NULL )
	{
	M_PROLOG
	for ( ; i != endIt; ++ i )
		push_back( *i );
	return;
	M_EPILOG
	}

template<typename value_type>
typename HList<value_type>::const_iterator HList<value_type>::begin( void ) const
	{
	return ( const_iterator( this, f_poHook ) );
	}

template<typename value_type>
typename HList<value_type>::iterator HList<value_type>::begin( void )
	{
	return ( iterator( this, f_poHook ) );
	}

template<typename value_type>
typename HList<value_type>::const_iterator HList<value_type>::end( void ) const
	{
	return ( const_iterator( this, NULL ) );
	}

template<typename value_type>
typename HList<value_type>::iterator HList<value_type>::end( void )
	{
	return ( iterator( this, NULL ) );
	}

template<typename value_type>
typename HList<value_type>::const_iterator HList<value_type>::rbegin( void ) const
	{
	return ( const_iterator( this, f_poHook ? f_poHook->f_poPrevious : NULL ) );
	}

template<typename value_type>
typename HList<value_type>::iterator HList<value_type>::rbegin( void )
	{
	return ( iterator( this, f_poHook ? f_poHook->f_poPrevious : NULL ) );
	}

template<typename value_type>
typename HList<value_type>::const_iterator  HList<value_type>::rend( void ) const
	{
	return ( const_iterator( this, NULL ) );
	}

template<typename value_type>
typename HList<value_type>::iterator HList<value_type>::rend( void )
	{
	return ( iterator( this, NULL ) );
	}

template<typename value_type>
typename HList<value_type>::const_cyclic_iterator HList<value_type>::hook( void ) const
	{
	return ( const_cyclic_iterator( this, f_poHook ) );
	}

template<typename value_type>
typename HList<value_type>::cyclic_iterator HList<value_type>::hook( void )
	{
	return ( cyclic_iterator( this, f_poHook ) );
	}

template<typename value_type>
HList<value_type>& HList<value_type>::operator = ( HList<value_type> const& a_roList )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCount = 0;
	if ( this != & a_roList )
		{
		l_iCount = f_iSize < a_roList.f_iSize ? f_iSize : a_roList.f_iSize;
		iterator thisIt = begin();
		const_iterator otherIt = a_roList.begin();
		if ( l_iCount )
			{
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )	
				{
				*thisIt = *otherIt;
				if ( otherIt.f_poCurrent == a_roList.f_poHook )
					f_poHook = thisIt.f_poCurrent;
				if ( otherIt.f_poCurrent == a_roList.f_poIndex )
					f_poIndex = thisIt.f_poCurrent;
				++ thisIt; ++ otherIt;
				}
			}
		if ( f_iSize > a_roList.f_iSize )
			{
			l_iCount = f_iSize - a_roList.f_iSize;
			while ( l_iCount -- )
				remove_tail();
			}
		else if ( f_iSize < a_roList.f_iSize )
			{
			for ( ; l_iCtr < a_roList.f_iSize; l_iCtr ++ )	
				{
				add_tail( &*otherIt );
				if ( otherIt.f_poCurrent == a_roList.f_poHook )
					f_poHook = f_poHook->f_poPrevious;
				if ( otherIt.f_poCurrent == a_roList.f_poIndex )
					f_poIndex = f_poHook->f_poPrevious;
				++ otherIt;
				}
			}
		f_iIndex = a_roList.f_iIndex;
		f_eOrder = a_roList.f_eOrder;
		f_iSize = a_roList.f_iSize;
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::clear( void )
	{
	M_PROLOG
	while ( f_iSize -- )
		delete f_poHook->f_poNext;
	f_iIndex = 0;
	f_poIndex = NULL;
	f_poHook = NULL;
	f_iSize = 0;
	return ;
	M_EPILOG
	}

template<typename value_type>
bool HList<value_type>::is_empty( void ) const
	{
	return ( ! f_iSize );
	}

template<typename value_type>
bool HList<value_type>::empty( void ) const
	{
	return ( ! f_iSize );
	}

template<typename value_type>
int long HList<value_type>::size( void ) const
	{
	M_PROLOG
	return ( f_iSize );
	M_EPILOG
	}

template<typename value_type>
int long HList<value_type>::get_size( void ) const
	{
	M_PROLOG
	return ( f_iSize );
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::swap( HList<value_type>& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_iSize, other.f_iSize );
		swap( f_iIndex, other.f_iIndex );
		swap( f_eOrder, other.f_eOrder );
		swap( f_poHook, other.f_poHook );
		swap( f_poIndex, other.f_poIndex );
		}
	return;
	}

template<typename value_type>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<value_type, treatment>::type HList<value_type>::insert( HIterator<value_type, treatment> const& it,
		value_type const* a_ptObject )
	{
	M_PROLOG
	HElement* l_poElement = new HElement( it.f_poCurrent, a_ptObject );
	if ( ( f_iSize == 0 ) || ( ( it.f_poCurrent == f_poHook ) && ( treatment == TREAT_AS_OPENED ) ) )
		f_poHook = l_poElement;
	f_iSize ++;
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( iterator( this, l_poElement ) );
	M_EPILOG
	}

template<typename value_type>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<value_type, treatment>::type HList<value_type>::insert( HIterator<value_type, treatment> const& it,
		value_type const& val )
	{
	M_PROLOG
	return ( insert( it, &val ) );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::add_head( value_type const* a_ptObject )
	{
	M_PROLOG
	f_poHook = new HElement( f_poHook, a_ptObject );
	f_iSize ++;
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::add_tail( value_type const* a_ptObject )
	{
	M_PROLOG
	HElement* l_poElement = new HElement( f_poHook, a_ptObject );
	if ( f_iSize == 0 )
		f_poHook = l_poElement;
	f_iSize ++;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::push_back( value_type const& a_rtObject )
	{
	M_PROLOG
	add_tail( &a_rtObject );
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::pop_back( void )
	{
	M_PROLOG
	remove_tail();
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::push_front( value_type const& a_rtObject )
	{
	M_PROLOG
	add_head( &a_rtObject );
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::pop_front( void )
	{
	M_PROLOG
	remove_head();
	return;
	M_EPILOG
	}

namespace
{

template<typename value_type, typename T>
bool asc_less( value_type const& a, value_type const& b, T const& comp )
	{
	return ( comp( a, b ) );
	}

template<typename value_type, typename T>
bool desc_less( value_type const& a, value_type const& b, T const& comp )
	{
	return ( comp( b, a ) );
	}

}

template<typename value_type>
template<typename T>
value_type& HList<value_type>::add_orderly( value_type const& a_rtObject,
		T const& less, sort_order_t a_eOrder )
	{
	M_PROLOG
	bool l_bBefore = false;
	int l_iIndex = 0, l_iOldIndex = -1, l_iLower = 0, l_iUpper = f_iSize;
	HElement* l_poElement = new HElement( NULL, &a_rtObject );
	if ( ( f_eOrder != UNSORTED ) && ( f_eOrder != a_eOrder ) )
		M_THROW( g_ppcErrMsgHList [ ERROR::BAD_ORDER ], a_eOrder );
	f_eOrder = a_eOrder;
	typedef bool ( *comp_t )( value_type const&, value_type const&, T const& );
	comp_t my_comp;
	if ( f_eOrder == ASCENDING )
		my_comp = asc_less<value_type, T>;
	else
		my_comp = desc_less<value_type, T>;
	while ( f_iSize && ( l_iOldIndex != l_iIndex ) )
		{
		l_iOldIndex = l_iIndex;
		l_iIndex = ( l_iLower + l_iUpper ) / 2;
		element_by_index( l_iIndex );
		if ( my_comp( f_poIndex->f_tObject, l_poElement->f_tObject, less ) )
			l_iLower = l_iIndex;
		else
			l_iUpper = l_iIndex;
		}
	if ( f_poIndex )
		{
		if ( my_comp( f_poIndex->f_tObject, l_poElement->f_tObject, less ) )
			f_poIndex = f_poIndex->f_poNext;
		else
			l_bBefore = true;
		}
	if ( ! f_iSize )
		f_poHook = f_poIndex = l_poElement;
	else
		{
		l_poElement->f_poNext = f_poIndex;
		l_poElement->f_poPrevious = f_poIndex->f_poPrevious;
		f_poIndex->f_poPrevious->f_poNext = l_poElement;
		f_poIndex->f_poPrevious = l_poElement;
		}
	f_iSize ++;
	f_poIndex = NULL;
	f_iIndex = 0;
	if ( l_bBefore && ! l_iIndex )
		f_poHook = f_poHook->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template<typename value_type>
template<OListBits::treatment_t const treatment>
typename OListBits::iterator<value_type, treatment>::type
HList<value_type>::erase( HIterator<value_type, treatment> const& a_roIterator )
	{
	M_PROLOG
	HIterator<value_type, treatment> it = a_roIterator;
	++ it;
	if ( ! f_iSize )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
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
	if ( a_roIterator.f_poCurrent == f_poHook )
		f_poHook = f_poHook->f_poNext;
	delete a_roIterator.f_poCurrent;
	f_iSize --;
	if ( f_iSize == 0 )
		f_poHook = NULL;
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( it );
	M_EPILOG
	}

template<typename value_type>
OListBits::status_t HList<value_type>::remove_head( value_type** a_pptObject )
	{
	M_PROLOG
	status_t l_eError = OK;
	HElement* l_poElement = NULL;
	if ( f_iSize > 0 )
		{
		l_poElement = f_poHook;
		if ( a_pptObject )
			( *a_pptObject ) = &l_poElement->f_tObject;
		f_poHook = f_poHook->f_poNext;
		}
	else
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poNext;
	delete l_poElement;
	f_iSize--;
	if ( f_iSize == 0 )
		{
		f_poHook = NULL;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return ( l_eError );
	M_EPILOG
	}

template<typename value_type>
OListBits::status_t HList<value_type>::remove_tail( value_type** a_pptObject )
	{
	M_PROLOG
	status_t l_eError = OK;
	HElement * l_poElement = NULL;
	if ( f_iSize > 0 )
		{
		l_poElement = f_poHook->f_poPrevious;
		if ( a_pptObject )
			( *a_pptObject ) = &l_poElement->f_tObject;
		if ( l_poElement == f_poIndex )
			{
			f_poIndex = l_poElement->f_poPrevious;
			f_iIndex --;
			}
		delete l_poElement;
		-- f_iSize;
		if ( f_iSize == 0 )
			{
			f_poHook = NULL;
			f_poIndex = NULL;
			f_iIndex = 0;
			}
		}
	else
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	return ( l_eError );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::operator[] ( int a_iIndex )
	{
	M_PROLOG
	return ( element_by_index( a_iIndex )->f_tObject );
	M_EPILOG
	}

template<typename value_type>
typename HList<value_type>::HElement* HList<value_type>::element_by_index( int a_iIndex )
	{
	M_PROLOG
	if ( f_iSize == 0 )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	if ( a_iIndex < 0 )
		a_iIndex += f_iSize;
	if ( ( a_iIndex >= f_iSize ) || ( a_iIndex < 0 ) )
		M_THROW( g_ppcErrMsgHList[ ERROR::BAD_INDEX ], a_iIndex );
	if ( ! f_poIndex )
		f_poIndex = f_poHook;
/*
we are looking for the smallest distance between a_iIndex and 0, f_iIndex, f_iSize.
we have two cases, a_iIndex is between 0 and f_iIndex:
	0 ..... a_iIndex ..... f_iIndex ..... f_iSize
meaning a_iIndex < f_iIndex
and a_iIndex is between f_iIndex and f_iSize
	0 ..... f_iIndex ..... a_iIndex ..... f_iSize
meaning a_iIndex > f_iIndex
lets take closer look at first case  ( a_iIndex < f_iIndex )
we have to check if a_iIndex is lowwer or geater than f_iIndex/2
*/
	if ( a_iIndex < f_iIndex )
		{
		if ( a_iIndex < ( f_iIndex / 2 ) )
			for ( f_iIndex = 0, f_poIndex = f_poHook; f_iIndex < a_iIndex; f_iIndex ++ )
				f_poIndex = f_poIndex->f_poNext;
		else
			for ( ; f_iIndex > a_iIndex; f_iIndex -- )
				f_poIndex = f_poIndex->f_poPrevious;
		}
	else
		{
		if ( a_iIndex < ( ( f_iSize + f_iIndex ) / 2 ) )
			for ( ; f_iIndex < a_iIndex; f_iIndex ++ )
				f_poIndex = f_poIndex->f_poNext;
		else
			for ( f_iIndex = f_iSize - 1, f_poIndex = f_poHook->f_poPrevious;
					f_iIndex > a_iIndex; f_iIndex -- )
				f_poIndex = f_poIndex->f_poPrevious;
		}
	return ( f_poIndex );
	M_EPILOG
	}

template<typename value_type>
typename HList<value_type>::iterator HList<value_type>::n_th( int a_iIndex )
	{
	M_PROLOG
	return ( iterator( this, element_by_index( a_iIndex ) ) );
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::exchange( HElement* a_poLeft, HElement* a_poRight )
	{
	M_PROLOG
	HElement* l_poNext = NULL;
	HElement* l_poPrevious = NULL;
	if ( a_poLeft == a_poRight )
		return;
	if ( a_poLeft == f_poHook )
		f_poHook = a_poRight;
	else if ( a_poRight == f_poHook )
		f_poHook = a_poLeft;
	if ( a_poLeft == f_poIndex )
		f_poIndex = a_poRight;
	else if ( a_poRight == f_poIndex )
		f_poIndex = a_poLeft;
/*
 *                         ( p L n )
 *          ( p R n ) <------+   +------> ( p R n )
 *  ( L ) <---+   +---> ( L )     ( L ) <---+   +---> ( L )
 */
	l_poNext = a_poRight->f_poNext;
	l_poPrevious = a_poRight->f_poPrevious;
	a_poLeft->f_poNext->f_poPrevious = a_poRight;
	a_poLeft->f_poPrevious->f_poNext = a_poRight;
	l_poNext->f_poPrevious = a_poLeft;
	l_poPrevious->f_poNext = a_poLeft;

	l_poNext = a_poLeft->f_poNext;
	l_poPrevious = a_poLeft->f_poPrevious;
	a_poLeft->f_poNext = a_poRight->f_poNext;
	a_poLeft->f_poPrevious = a_poRight->f_poPrevious;
	a_poRight->f_poNext = l_poNext;
	a_poRight->f_poPrevious = l_poPrevious;
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::exchange( iterator const& left, iterator const& right )
	{
	M_PROLOG
	if ( left != right )
		exchange( left.f_poCurrent, right.f_poCurrent );
	return;
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::front( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template<typename value_type>
value_type const& HList<value_type>::front( void ) const
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::head( void )
	{
	M_PROLOG
	return ( front() );
	M_EPILOG
	}

template<typename value_type>
value_type const& HList<value_type>::head( void ) const
	{
	M_PROLOG
	return ( front() );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::back( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	return ( f_poHook->f_poPrevious->f_tObject );
	M_EPILOG
	}

template<typename value_type>
value_type const& HList<value_type>::back( void ) const
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW( g_ppcErrMsgHList[ ERROR::EMPTY ], errno );
	return ( f_poHook->f_poPrevious->f_tObject );
	M_EPILOG
	}

template<typename value_type>
value_type& HList<value_type>::tail( void )
	{
	M_PROLOG
	return ( back() );
	M_EPILOG
	}

template<typename value_type>
value_type const& HList<value_type>::tail( void ) const
	{
	M_PROLOG
	return ( back() );
	M_EPILOG
	}

template<typename value_type>
template<typename T>
void HList<value_type>::merge_sort( HElement*& left, HElement*& right, T const& less )
	{
	M_PROLOG
	HElement* leftIt = left;
	HElement* rightIt = right;
	int long stepsLeft = 0;
	int long stepsRight = 0;
	while ( leftIt != rightIt )
		{
		if ( leftIt->f_poNext == rightIt )
			break;
		leftIt = leftIt->f_poNext;
		++ stepsLeft;
		if ( leftIt == rightIt->f_poPrevious )
			break;
		rightIt = rightIt->f_poPrevious;
		++ stepsRight;
		}
	int const ARBITRARILY_CHOSEN_THRESHOLD = 7;
	if ( ( stepsLeft + stepsRight + 2 ) < ARBITRARILY_CHOSEN_THRESHOLD )
		insert_sort( left, right, less );
	else
		{
		if ( stepsLeft < ARBITRARILY_CHOSEN_THRESHOLD )
			insert_sort( left, leftIt, less );
		else
			merge_sort( left, leftIt, less );
		if ( stepsRight < ARBITRARILY_CHOSEN_THRESHOLD )
			insert_sort( rightIt, right, less );
		else
			merge_sort( rightIt, right, less );
		HElement* first = NULL;
		++ stepsLeft;
		typedef bool ( *comp_t )( value_type const&, value_type const&, T const& );
		comp_t my_comp;
		if ( f_eOrder == ASCENDING )
			my_comp = asc_less<value_type, T>;
		else
			my_comp = desc_less<value_type, T>;
		while ( stepsLeft -- )
			{
			if ( my_comp( rightIt->f_tObject, left->f_tObject, less ) )
				{
				HElement* ptr = rightIt;
				if ( ! first )
					first = ptr;
				while ( ( rightIt != right ) && my_comp( rightIt->f_poNext->f_tObject, left->f_tObject, less ) )
					rightIt = rightIt->f_poNext;
				HElement* nextRight = rightIt->f_poNext;
				bool to_break = false;
				if ( rightIt == right )
					to_break = true;
				ptr->f_poPrevious->f_poNext = rightIt->f_poNext;
				rightIt->f_poNext->f_poPrevious = ptr->f_poPrevious;
				left->f_poPrevious->f_poNext = ptr;
				ptr->f_poPrevious = left->f_poPrevious;
				left->f_poPrevious = rightIt;
				rightIt->f_poNext = left;
				rightIt = nextRight;
				if ( to_break )
					{
					right = rightIt->f_poPrevious;
					break;
					}
				}
			if ( ! first )
				first = left;
			left = left->f_poNext;
			}
		left = first;
		}
	return;
	M_EPILOG
	}

template<typename value_type>
template<typename T>
void HList<value_type>::insert_sort(
		HElement*& a_rpoBaseLower, HElement*& a_rpoBaseUpper,
		T const& less )
	{
	M_PROLOG
	if ( a_rpoBaseLower != a_rpoBaseUpper )
		{
		HElement* top = a_rpoBaseLower;
		typedef bool ( *comp_t )( value_type const&, value_type const&, T const& );
		comp_t my_comp;
		if ( f_eOrder == ASCENDING )
			my_comp = asc_less<value_type, T>;
		else
			my_comp = desc_less<value_type, T>;
		while ( top != a_rpoBaseUpper )
			{
			top = top->f_poNext;
			HElement* ptr = top;
			while ( ( ptr != a_rpoBaseLower ) && my_comp( top->f_tObject, ptr->f_poPrevious->f_tObject, less ) )
				ptr = ptr->f_poPrevious;
			if ( ptr != top )
				{
				HElement* oldtop = top->f_poPrevious;
				insert( ptr, top );
				if ( top == a_rpoBaseUpper )
					a_rpoBaseUpper = oldtop;
				if ( ptr == a_rpoBaseLower )
					a_rpoBaseLower = top;
				top = oldtop;
				}
			}
		a_rpoBaseUpper = top;
		}
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::insert( HElement* pos, HElement* elem )
	{
	M_ASSERT( pos != elem );
	if ( pos->f_poNext == elem )
		exchange( pos, elem );
	else
		{
		if ( pos == f_poHook )
			f_poHook = elem;
		if ( pos == f_poIndex )
			f_poIndex = elem;
		if ( pos->f_poPrevious != elem )
			{
			elem->f_poNext->f_poPrevious = elem->f_poPrevious;
			elem->f_poPrevious->f_poNext = elem->f_poNext;
			elem->f_poNext = pos;
			elem->f_poPrevious = pos->f_poPrevious;
			pos->f_poPrevious->f_poNext = elem;
			pos->f_poPrevious = elem;
			}
		}
	return;
	}

template<typename value_type>
template<typename T>
void HList<value_type>::select_sort(
		HElement*& a_rpoBaseLower, HElement*& a_rpoBaseUpper,
		int distance, T const& less )
	{
	M_PROLOG
	int l_iCtrLoc = 0;
	HElement* l_poExtreamLower = NULL;
	HElement* l_poExtreamUpper = NULL;
	HElement* l_poBaseLower = a_rpoBaseLower;
	HElement* l_poBaseUpper = a_rpoBaseUpper;
	HElement* l_poPointer = NULL;
	int ctr = distance;
	typedef bool ( *comp_t )( value_type const&, value_type const&, T const& );
	comp_t my_comp;
	if ( f_eOrder == ASCENDING )
		my_comp = asc_less<value_type, T>;
	else
		my_comp = desc_less<value_type, T>;
	while ( ctr >= 0 )
		{
		l_iCtrLoc = ctr;
		l_poExtreamLower = l_poBaseLower;
		l_poExtreamUpper = l_poBaseUpper;
		l_poPointer = l_poBaseLower;
		while ( l_iCtrLoc -- )
			{
			if ( ( l_poPointer != l_poExtreamLower )
					&& my_comp( l_poPointer->f_tObject, l_poExtreamLower->f_tObject, less ) )
				l_poExtreamLower = l_poPointer;
			if ( ( l_poPointer != l_poExtreamUpper )
					&& my_comp( l_poExtreamUpper->f_tObject, l_poPointer->f_tObject, less ) )
				l_poExtreamUpper = l_poPointer;
			l_poPointer = l_poPointer->f_poNext;
			}
		if ( l_poExtreamLower != l_poBaseLower )
			{
			exchange( l_poBaseLower, l_poExtreamLower );
			if ( l_poExtreamLower == l_poBaseUpper )
				l_poBaseUpper = l_poBaseLower;
			l_poBaseLower = l_poExtreamLower;
			}
		if ( l_poExtreamUpper != l_poBaseUpper )
			{
			exchange( l_poBaseUpper, l_poExtreamUpper );
			l_poBaseUpper = l_poExtreamUpper;
			}
		if ( ctr == distance )
			{
			a_rpoBaseLower = l_poBaseLower;
			a_rpoBaseUpper = l_poBaseUpper;
			}
		l_poBaseLower = l_poBaseLower->f_poNext;
		l_poBaseUpper = l_poBaseUpper->f_poPrevious;
		ctr -= 2;
		}
	return;
	M_EPILOG
	}

template<typename value_type>
void HList<value_type>::sort( sort_order_t a_eOrder )
	{
	M_PROLOG
	sort( yaal::less<value_type>, a_eOrder );
	return;
	M_EPILOG
	}

template<typename value_type>
template<typename T>
void HList<value_type>::sort( T const& less, sort_order_t a_eOrder )
	{
	M_PROLOG
	f_eOrder = a_eOrder;
	if ( ( f_eOrder != ASCENDING ) && ( f_eOrder != DESCENDING ) )
		M_THROW( g_ppcErrMsgHList [ ERROR::BAD_ORDER ], f_eOrder );
	if ( f_iSize > 1 )
		{
		HElement* first = f_poHook;
		HElement* last = f_poHook->f_poPrevious;
		merge_sort( first, last, less );
		f_poHook = first;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return;
	M_EPILOG;
	}

}

template<typename value_type> 
inline void swap( yaal::hcore::HList<value_type>& a, yaal::hcore::HList<value_type>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HLIST_HXX_INCLUDED */


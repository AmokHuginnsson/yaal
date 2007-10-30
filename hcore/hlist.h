/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlist.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HLIST_H
#define __YAAL_HCORE_HLIST_H

#line 31 "hlist.h"

#define D_VCSID_HLIST_H "$Id$"

#include "hcore/hexception.h"

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

struct OListBits
	{
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_EMPTYELEMENT,
			E_BADINDEX,
			E_BADFLAG,
			E_EMPTY,
			E_BADOFFSET,
			E_BADNUMBER,
			E_BADORDER
			} error_t;
		};
	typedef enum
		{
		D_TREAT_AS_CLOSED = 1,
		D_TREAT_AS_OPENED = 2
		} treatment_t;
	typedef enum
		{
		D_OK = 0,
		D_ERROR = 1,
		D_WAS_EMPTIED = 2,
		D_WAS_NOT_EMPTIED = 4,
		D_FINAL_REACHED = 8,
		D_NOT_FOUND = 16
		} status_t;
	typedef enum
		{
		D_UNSORTED,
		D_ASCENDING,
		D_DESCENDING
		} sort_order_t;
	virtual ~OListBits ( void ) { } 
	};

template<typename tType> 
class HList : public OListBits
	{
	class HElement;
	int f_iSize;					/* how many elements this list contains */
	HElement * f_poHook;			/* "begining" of the list ( "first" element ) */
	HElement * f_poSelected;	/* local temporary pointer, "cursor" */
/* for internal use only */
	sort_order_t f_eOrder;		/* last-to-current sort order */
	int f_iIndex;							/* this two fiels will allow boost operator [ ],
															 int holds last */
	HElement * f_poIndex;			/* index and HElement * holds pointer to this
															 last element */
public:
	template<OListBits::treatment_t const treatment = OListBits::D_TREAT_AS_OPENED>
	class HIterator;
	typedef HIterator<> iterator;
	typedef HIterator<OListBits::D_TREAT_AS_CLOSED> cyclic_iterator;
	HList( int = 0 );                 /* Creates list, with specified size */
	virtual ~HList( void );
	HList( HList const& );
	HList& operator = ( HList const& );
	cyclic_iterator hook( void ) const;
	iterator begin( void ) const;
	iterator end( void ) const;
	iterator rend( void ) const;
	iterator rbegin( void ) const;
	void flush( void );
	int size( void );
	tType& add_element( tType* = NULL ); /* adds new element at current cursor position */
	tType& add_head( tType const* = NULL );    /* adds new element at beggining of the list */
	tType& add_tail( tType const* = NULL );	/* adds new element at end of the list */
	void push_back( tType const& );
	void pop_back( void );
	void push_front( tType const& );
	tType& add_at( int, tType* = NULL ); /* adds new element at specified position */
/* adds element in the way that keeps order */
	template<typename T>
	tType& add_orderly( tType const&, T const&, sort_order_t = D_ASCENDING );
	status_t remove_element( treatment_t const& = D_TREAT_AS_CLOSED,
			tType** = NULL );	/* removes element at current cursor position */
	status_t remove_at ( int, treatment_t const& = D_TREAT_AS_CLOSED,
			tType** = NULL );
	status_t remove_head( tType** = NULL );
	status_t remove_tail( tType** = NULL );
	template<OListBits::treatment_t treatment>
	status_t erase( HIterator<treatment>& );
	/* sets cursor at specified index or number */
	tType& go( int );
	tType& operator[] ( int );
	tType const& operator[] ( int ) const;
	tType& present( void );
	tType& head( void );
	tType& tail( void );
	tType* to_head( int = 1, treatment_t const& = D_TREAT_AS_CLOSED );
	tType* to_tail( int = 1, treatment_t const& = D_TREAT_AS_CLOSED );
	void exchange( int, int );
	void sort_by_contents( sort_order_t = D_ASCENDING );
	bool empty( void );
	template<typename T>
	void sort( T const&, sort_order_t = D_ASCENDING );
protected:
	bool to_head( HElement*&, int = 1, treatment_t const& = D_TREAT_AS_CLOSED );
	bool to_tail( HElement*&, int = 1, treatment_t const& = D_TREAT_AS_CLOSED );
	HElement* element_by_index ( int );
	void exchange( HElement*, HElement* );
	void sub_swap ( HElement*, HElement*, HElement* );
	friend class HList<tType>::iterator;
	friend class HList<tType>::cyclic_iterator;
	};

template<typename tType>
class HList<tType>::HElement
	{
	HElement* f_poPrevious;
	HElement* f_poNext;
	tType f_tObject; /* The Object itself. */
	explicit HElement ( HElement* );
	virtual ~HElement ();
	HElement ( HElement const & );
	HElement& operator = ( HElement const& );
	friend class HList<tType>;
	friend class HList<tType>::iterator;
	friend class HList<tType>::cyclic_iterator;
	};

template<typename tType>
template<OListBits::treatment_t const treatment>
class HList<tType>::HIterator
	{
protected:
	/*{*/
	HList<tType> const* f_poOwner;
	HElement* f_poCurrent;
	/*}*/
public:
	/*{*/
	HIterator( void );
	template<OListBits::treatment_t family>
	HIterator( HIterator<family> const& );
	HIterator& operator ++ ( void )
		{
		M_PROLOG
		if ( f_poCurrent )
			{
			f_poCurrent = f_poCurrent->f_poNext;
			if ( ( treatment == OListBits::D_TREAT_AS_OPENED ) && ( f_poCurrent == f_poOwner->f_poHook ) )
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
			if ( ( treatment == OListBits::D_TREAT_AS_OPENED ) && ( f_poCurrent == f_poOwner->f_poHook->f_poPrevious ) )
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
	template<OListBits::treatment_t family>
	HIterator& operator= ( HIterator<family> const& );
	bool operator == ( HIterator const & ) const;
	bool operator != ( HIterator const & ) const;
	tType& operator* ( void );
	tType const& operator* ( void ) const;
	tType* operator -> ( void );
	bool is_valid( void ) const;
	/*}*/
protected:
	/*{*/
	friend class HList<tType>;
	HIterator( HList<tType> const* const, HElement* const );
	/*}*/
	};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*++++++++++++++++++++++ general list implementation ++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template<typename tType>
HList<tType>::HElement::HElement ( HElement* a_poElement )
	: f_poPrevious ( NULL ), f_poNext ( NULL ), f_tObject()
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

template<typename tType>
HList< tType >::HElement::~HElement ( void )
	{
	f_poPrevious->f_poNext = f_poNext;
	f_poNext->f_poPrevious = f_poPrevious;
	return;
	}

//========================== Iterator ========================================

template<typename tType>
template<OListBits::treatment_t const treatment>
HList<tType>::HIterator<treatment>::HIterator( void ) : f_poOwner( NULL ), f_poCurrent( NULL )
	{
	return;
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
template<OListBits::treatment_t family>
HList<tType>::HIterator<treatment>::HIterator( HIterator<family> const& a_roIterator ) : f_poOwner( NULL ), f_poCurrent( NULL )
	{
	M_PROLOG
	operator = ( a_roIterator );
	return;
	M_EPILOG
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
HList<tType>::HIterator<treatment>::HIterator ( HList<tType> const* const a_poOwner,
		HElement* const a_poElement ) : f_poOwner( a_poOwner ),
	f_poCurrent( a_poElement )
	{
	return;
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
template<OListBits::treatment_t family>
typename HList<tType>::template HIterator<treatment>& HList<tType>::HIterator<treatment>::operator= ( HIterator<family> const& a_roIterator )
	{
	M_PROLOG
	if ( reinterpret_cast<HIterator<treatment> const*>( &a_roIterator ) != this )
		{
		f_poOwner = a_roIterator.f_poOwner;
		f_poCurrent = a_roIterator.f_poCurrent;
		}
	return ( *this );
	M_EPILOG
	}

/*
template<typename tType>
typename HList< tType >::HIterator & HList< tType >::HIterator::operator ++ ( void )

template<typename tType>
typename HList< tType >::HIterator const HList< tType >::HIterator::operator ++ ( int )

template<typename tType>
typename HList< tType >::HIterator & HList< tType >::HIterator::operator -- ( void )

template<typename tType>
typename HList< tType >::HIterator const HList< tType >::HIterator::operator -- ( int )
*/

template<typename tType>
template<OListBits::treatment_t const treatment>
bool HList< tType >::HIterator<treatment>::operator == ( HIterator const& a_roIterator ) const
	{
	M_PROLOG
	return ( f_poCurrent == a_roIterator.f_poCurrent );
	M_EPILOG
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
bool HList<tType>::HIterator<treatment>::operator!= ( HIterator const& a_roIterator ) const
	{
	M_PROLOG
	return ( f_poCurrent != a_roIterator.f_poCurrent );
	M_EPILOG
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
tType& HList<tType>::HIterator<treatment>::operator* ( void )
	{
	return ( f_poCurrent->f_tObject );
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
tType const & HList<tType>::HIterator<treatment>::operator* ( void ) const
	{
	return ( f_poCurrent->f_tObject );
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
tType * HList<tType>::HIterator<treatment>::operator -> ( void )
	{
	return ( & f_poCurrent->f_tObject );
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
bool HList<tType>::HIterator<treatment>::is_valid( void ) const
	{
	return ( f_poOwner && f_poOwner->f_poHook && f_poCurrent );
	}

//============================================================================

template<typename tType>
HList< tType >::HList ( int a_iSize )
	: OListBits(), f_iSize ( 0 ),
	f_poHook ( NULL ), f_poSelected ( NULL ), f_eOrder ( D_UNSORTED ),
	f_iIndex ( 0 ), f_poIndex ( NULL )
	{
	M_PROLOG
	while ( a_iSize -- )
		HList::add_tail();
	return ;
	M_EPILOG
	}

template<typename tType>
HList< tType >::~HList ( void )
	{
	M_PROLOG
	HList::flush ();
	return ;
	M_EPILOG
	}

template<typename tType>
HList< tType >::HList ( HList<tType> const & a_roList )
	: OListBits(), f_iSize ( 0 ),
	f_poHook ( NULL ), f_poSelected ( NULL ), f_eOrder ( D_UNSORTED ),
	f_iIndex ( 0 ), f_poIndex ( NULL )
	{
	M_PROLOG
	( * this ) = a_roList;
	return;
	M_EPILOG
	}

template<typename tType>
typename HList<tType>::iterator HList<tType>::begin( void ) const
	{
	return ( iterator( this, f_poHook ) );
	}

template<typename tType>
typename HList<tType>::iterator HList<tType>::end( void ) const
	{
	return ( iterator( this, NULL ) );
	}

template<typename tType>
typename HList<tType>::iterator HList<tType>::rend( void ) const
	{
	return ( iterator( this, NULL ) );
	}

template<typename tType>
typename HList<tType>::iterator HList<tType>::rbegin( void ) const
	{
	return ( iterator( this, f_poHook->f_poPrevious ) );
	}

template<typename tType>
typename HList<tType>::cyclic_iterator HList<tType>::hook( void ) const
	{
	return ( cyclic_iterator( this, f_poHook ) );
	}

template<typename tType>
HList<tType>& HList< tType >::operator = ( HList<tType> const& a_roList )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCount = 0;
	int l_iIndex = 0;
	HList * l_poList = NULL;
	HElement * l_poSelected = NULL;
	HElement * l_poNewSelected = NULL;
	HElement * l_poIndex = NULL;
	if ( this != & a_roList )
		{
		l_iCount = f_iSize < a_roList.f_iSize ? f_iSize : a_roList.f_iSize;
		/* I have to do this cast because to_tail modifies f_poSelected and
		 * declaring it (to_tail) const would be false, but after full loop
		 * of to_tail's obejct is unmodified */
		l_poList = const_cast < HList * > ( & a_roList );
		l_poSelected = a_roList.f_poSelected;
		l_poIndex = a_roList.f_poIndex;
		l_iIndex = a_roList.f_iIndex;
		if ( l_iCount )
			{
			go ( - 1 );
			l_poList->go ( - 1 );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )	
				{
				( * to_tail() ) = ( * l_poList->to_tail() );
				if ( a_roList.f_poSelected == a_roList.f_poHook )
					f_poHook = f_poSelected;
				if ( a_roList.f_poSelected == l_poIndex )
					l_poIndex = f_poSelected;
				if ( a_roList.f_poSelected == l_poSelected )
					l_poNewSelected = f_poSelected;
				}
			}
		else if ( a_roList.f_iSize )
			l_poList->go ( - 1 );
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
				add_tail ( l_poList->to_tail() );
				if ( a_roList.f_poSelected == a_roList.f_poHook )
					f_poHook = f_poSelected;
				if ( a_roList.f_poSelected == l_poIndex )
					l_poIndex = f_poSelected;
				if ( a_roList.f_poSelected == l_poSelected )
					l_poNewSelected = f_poSelected;
				}
			}
		f_poSelected = l_poNewSelected ? l_poNewSelected : f_poHook;
		f_iIndex = l_iIndex;
		f_poIndex = l_poIndex;
		f_eOrder = a_roList.f_eOrder;
		f_iSize = a_roList.f_iSize;
		}
	return ( * this );
	M_EPILOG
	}

template<typename tType>
void HList<tType>::flush ( void )
	{
	M_PROLOG
	while ( f_iSize -- )
		delete f_poHook->f_poNext;
	f_iIndex = 0;
	f_poIndex = NULL;
	f_poHook = NULL;
	f_poSelected = NULL;
	f_iSize = 0;
	return ;
	M_EPILOG
	}

template<typename tType>
bool HList<tType>::empty ( void )
	{
	return ( ! f_iSize );
	}

template<typename tType>
int HList<tType>::size ( void )
	{
	M_PROLOG
	return ( f_iSize );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::add_element ( tType* a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = new HElement ( f_poSelected );
	if ( f_iSize == 0 )
		f_poHook = f_poSelected = l_poElement;
	f_iSize ++;
	if ( a_ptObject )
		l_poElement->f_tObject = * a_ptObject;
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::add_head ( tType const* a_ptObject )
	{
	M_PROLOG
	f_poHook = new HElement ( f_poHook );
	if ( f_iSize == 0 )
		f_poSelected = f_poHook;
	f_iSize ++;
	if ( a_ptObject )
		f_poHook->f_tObject = * a_ptObject;
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::add_tail ( tType const* a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = new HElement ( f_poHook );
	if ( f_iSize == 0 )
		f_poHook = f_poSelected = l_poElement;
	f_iSize ++;
	if ( a_ptObject )
		l_poElement->f_tObject = * a_ptObject;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template<typename tType>
void HList<tType>::push_back( tType const& a_rtObject )
	{
	M_PROLOG
	add_tail( &a_rtObject );
	return;
	M_EPILOG
	}

template<typename tType>
void HList<tType>::pop_back( void )
	{
	M_PROLOG
	remove_tail();
	return;
	M_EPILOG
	}

template<typename tType>
void HList<tType>::push_front( tType const& a_rtObject )
	{
	M_PROLOG
	add_head( &a_rtObject );
	return;
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::add_at ( int a_iIndex, tType* a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( a_iIndex > f_iSize )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADINDEX ], a_iIndex );
	if ( f_iSize )
		{
		if ( a_iIndex < 0 )
			l_poElement = f_poHook;
		else
			l_poElement = element_by_index ( a_iIndex );
		}
	f_poSelected = l_poElement = new HElement ( l_poElement );
	if ( f_iSize && ! a_iIndex )
		f_poHook = l_poElement;
	if ( f_iSize == 0 )
		f_poHook = f_poSelected = l_poElement;
	f_iSize ++;
	if ( a_ptObject )
		l_poElement->f_tObject = * a_ptObject;
	if ( f_poIndex && ( a_iIndex >= 0 ) && ( a_iIndex <= f_iIndex ) )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template<typename tType>
template<typename T>
tType& HList<tType>::add_orderly ( tType const& a_rtObject,
		T const& less, sort_order_t a_eOrder )
	{
	M_PROLOG
	bool l_bBefore = false;
	int l_iIndex = 0, l_iOldIndex = -1, l_iLower = 0, l_iUpper = f_iSize;
	HElement * l_poElement = new HElement ( NULL );
	l_poElement->f_tObject = a_rtObject;
	if ( ( f_eOrder != D_UNSORTED ) && ( f_eOrder != a_eOrder ) )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADORDER ], a_eOrder );
	f_eOrder = a_eOrder;
	while ( f_iSize && ( l_iOldIndex != l_iIndex ) )
		{
		l_iOldIndex = l_iIndex;
		l_iIndex = ( l_iLower + l_iUpper ) / 2;
		element_by_index ( l_iIndex );
		if ( less ( f_poIndex->f_tObject, l_poElement->f_tObject ) )
			l_iLower = l_iIndex;
		else
			l_iUpper = l_iIndex;
		}
	if ( f_poIndex )
		{
		if ( less ( f_poIndex->f_tObject, l_poElement->f_tObject ) )
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

template<typename tType>
OListBits::status_t HList< tType >::remove_at ( int a_iIndex, treatment_t const& a_eFlag, tType** a_pptObject )
	{
	M_PROLOG
	treatment_t l_eTreat = a_eFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
	status_t l_eError = D_OK;
	HElement * l_poElement = NULL;
	l_poElement = f_poHook;
	if ( f_iSize > 0 )
		{
		if ( a_iIndex >= f_iSize )
			M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADINDEX ], a_iIndex );
		l_poElement = element_by_index ( a_iIndex );
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		}
	else
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( l_poElement == f_poHook )
		f_poHook = f_poHook->f_poNext;
	if ( l_poElement == f_poSelected )
		{
		if ( ( l_eTreat == D_TREAT_AS_OPENED )
				&& ( f_poSelected->f_poNext == f_poHook ) )
			f_poSelected = f_poSelected->f_poPrevious;
		else
			f_poSelected = f_poSelected->f_poNext;
		}
	if ( f_poIndex && ( a_iIndex <= f_iIndex ) )
		f_poIndex = f_poIndex->f_poNext;
	delete l_poElement;
	-- f_iSize;
	if ( f_iSize == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return ( l_eError );
	M_EPILOG
	}

template<typename tType>
OListBits::status_t HList< tType >::remove_element ( treatment_t const& a_eFlag, tType** a_pptObject )
	{
	M_PROLOG
	treatment_t l_eTreat = a_eFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
	status_t l_eError = D_OK;
	HElement * l_poElement = NULL;
	if ( f_iSize > 0 )
		{
		l_poElement = f_poSelected;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		}
	else
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	/* 1 2 3 4 5 6 7
	 *     ^
	 * 1 2 4 5 6 7
	 *     ^
	 * 1 2 3 4 5
	 * ^
	 * 2 3 4 5
	 * ^
	 * 1 2 3 4 5
	 *         ^
	 * 1 2 3 4  (o)
	 *       ^
	 * 1 2 3 4  (c) 
	 * ^
	 */
	if ( ( l_eTreat == D_TREAT_AS_OPENED )
			&& ( f_poSelected->f_poNext == f_poHook ) )
		{
		f_poSelected = f_poSelected->f_poPrevious;
		if ( a_pptObject )
			( * a_pptObject ) = NULL;
		}
	else
		f_poSelected = f_poSelected->f_poNext;
	if ( l_poElement == f_poHook )
		f_poHook = f_poHook->f_poNext;
	delete l_poElement;
	f_iSize --;
	if ( f_iSize == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		}
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_eError );
	M_EPILOG
	}

template<typename tType>
OListBits::status_t HList< tType >::remove_head ( tType** a_pptObject )
	{
	M_PROLOG
	status_t l_eError = D_OK;
	HElement * l_poElement = NULL;
	if ( f_iSize > 0 )
		{
		l_poElement = f_poHook;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		f_poHook = f_poHook->f_poNext;
		}
	else
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( l_poElement == f_poSelected )
		f_poSelected = l_poElement->f_poNext;
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poNext;
	delete l_poElement;
	f_iSize--;
	if ( f_iSize == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return ( l_eError );
	M_EPILOG
	}

template<typename tType>
OListBits::status_t HList< tType >::remove_tail ( tType** a_pptObject )
	{
	M_PROLOG
	status_t l_eError = D_OK;
	HElement * l_poElement = NULL;
	if ( f_iSize > 0 )
		{
		l_poElement = f_poHook->f_poPrevious;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		if ( l_poElement == f_poSelected )
			f_poSelected = l_poElement->f_poPrevious;
		if ( l_poElement == f_poIndex )
			{
			f_poIndex = l_poElement->f_poPrevious;
			f_iIndex --;
			}
		delete l_poElement;
		f_iSize--;
		if ( f_iSize == 0 )
			{
			f_poHook = NULL;
			f_poSelected = NULL;
			f_poIndex = NULL;
			f_iIndex = 0;
			}
		}
	else
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	return ( l_eError );
	M_EPILOG
	}

template<typename tType>
template<OListBits::treatment_t const treatment>
OListBits::status_t HList<tType>::erase( HIterator<treatment>& a_roIterator )
	{
	M_PROLOG
	f_poSelected = a_roIterator.f_poCurrent;
	return ( remove_element() );
	M_EPILOG
	}

template<typename tType>
bool HList< tType >::to_head ( HElement*& a_rpoElement, int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOk = true;
	int l_iCtr = 0;
	if ( ! f_iSize )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADOFFSET ], a_iOffset );
	switch ( a_eFlag )
		{
		case ( D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				a_rpoElement = a_rpoElement->f_poPrevious;
			}
		break;
		case ( D_TREAT_AS_OPENED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				{
				if ( a_rpoElement == f_poHook )
					{
					l_bOk = false;
					break;
					}
				a_rpoElement = a_rpoElement->f_poPrevious;
				}
			}
		break;
		default :
			M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADFLAG ], a_eFlag );
		}
	return ( l_bOk );
	M_EPILOG
	}

template<typename tType>
tType* HList<tType>::to_head ( int a_iOffset, treatment_t const& a_eFlag )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( f_poSelected == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( a_iOffset < 0 )
		l_bOk = to_tail ( f_poSelected, - a_iOffset, a_eFlag );
	else
		l_bOk = to_head ( f_poSelected, a_iOffset, a_eFlag );
	if ( l_bOk )
		l_ptObject = & f_poSelected->f_tObject;
	return ( l_ptObject );
	M_EPILOG
	}

template<typename tType>
bool HList<tType>::to_tail ( HElement * & a_rpoElement, int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOK = true;
	int l_iCtr = 0;
	if ( ! f_iSize )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADOFFSET ], a_iOffset );
	switch ( a_eFlag )
		{
		case ( D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				a_rpoElement = a_rpoElement->f_poNext;
			}
		break;
		case ( D_TREAT_AS_OPENED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				{
				if ( a_rpoElement->f_poNext == f_poHook )
					{
					l_bOK = false;
					break;
					}
				a_rpoElement = a_rpoElement->f_poNext;
				}
			}
		break;
		default :
			M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADFLAG ], a_eFlag );
		}
	return ( l_bOK );
	M_EPILOG
	}

template<typename tType>
tType* HList<tType>::to_tail ( int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( f_poSelected == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( a_iOffset < 0 )
		l_bOk = to_head ( f_poSelected, - a_iOffset, a_eFlag );
	else
		l_bOk = to_tail ( f_poSelected, a_iOffset, a_eFlag );
	if ( l_bOk )
		l_ptObject = & f_poSelected->f_tObject;
	return ( l_ptObject );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	return ( element_by_index ( a_iIndex )->f_tObject );
	M_EPILOG
	}

template<typename tType>
typename HList<tType>::HElement * HList<tType>::element_by_index ( int a_iIndex )
	{
	M_PROLOG
	if ( f_iSize == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	if ( a_iIndex < 0 )
		a_iIndex += f_iSize;
	if ( ( a_iIndex >= f_iSize ) || ( a_iIndex < 0 ) )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADINDEX ], a_iIndex );
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

template<typename tType>
tType& HList<tType>::go ( int a_iNumber )
	{
	M_PROLOG
	f_poSelected = element_by_index ( a_iNumber );
	return ( f_poSelected->f_tObject );
	M_EPILOG
	}

template<typename tType>
void HList<tType>::exchange ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	HElement * l_poNext = NULL, * l_poPrevious = NULL;
	if ( a_poLeft == a_poRight )
		return;
	if ( a_poLeft == f_poHook )
		f_poHook = a_poRight;
	else if ( a_poRight == f_poHook )
		f_poHook = a_poLeft;
	if ( a_poLeft == f_poSelected )
		f_poSelected = a_poRight;
	else if ( a_poRight == f_poSelected )
		f_poSelected = a_poLeft;
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

template<typename tType>
void HList<tType>::exchange ( int a_iLeft, int a_iRight )
	{
	M_PROLOG
	HElement * l_poLeft = NULL, * l_poRight = NULL;
	if ( a_iLeft == a_iRight )
		return;
	l_poLeft = element_by_index ( a_iLeft );
	l_poRight = element_by_index ( a_iRight );
	exchange ( l_poLeft, l_poRight );
	return;
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::present ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	return ( f_poSelected->f_tObject );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::head ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template<typename tType>
tType& HList<tType>::tail ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_EMPTY ], errno );
	return ( f_poHook->f_poPrevious->f_tObject );
	M_EPILOG
	}

template<typename tType>
template<typename T>
void HList<tType>::sort( T const& less, sort_order_t a_eOrder )
	{
	M_PROLOG
	int l_iCtr = f_iSize;
	int l_iCtrLoc = 0;
	HElement* l_poBaseLower = f_poHook;
	HElement* l_poBaseUpper = f_poHook->f_poPrevious;
	HElement* l_poExtreamLower = NULL;
	HElement* l_poExtreamUpper = NULL;
	HElement* l_poPointer = NULL;
	f_eOrder = a_eOrder;
	if ( ( f_eOrder != D_ASCENDING ) && ( f_eOrder != D_DESCENDING ) )
		M_THROW ( g_ppcErrMsgHList [ ERROR::E_BADORDER ], f_eOrder );
	while ( l_iCtr >= 0 )
		{
		l_iCtrLoc = l_iCtr;
		l_poExtreamLower = l_poBaseLower;
		l_poExtreamUpper = l_poBaseUpper;
		l_poPointer = l_poBaseLower;
		while ( l_iCtrLoc -- )
			{
			if ( ( l_poPointer != l_poExtreamLower )
					&& less ( l_poPointer->f_tObject, l_poExtreamLower->f_tObject ) )
				l_poExtreamLower = l_poPointer;
			if ( ( l_poPointer != l_poExtreamUpper )
					&& less ( l_poExtreamUpper->f_tObject, l_poPointer->f_tObject ) )
				l_poExtreamUpper = l_poPointer;
			l_poPointer = l_poPointer->f_poNext;
			}
		if ( l_poExtreamLower != l_poBaseLower )
			{
			exchange ( l_poBaseLower, l_poExtreamLower );
			if ( l_poExtreamLower == l_poBaseUpper )
				l_poBaseUpper = l_poBaseLower;
			l_poBaseLower = l_poExtreamLower;
			}
		if ( l_poExtreamUpper != l_poBaseUpper )
			{
			exchange ( l_poBaseUpper, l_poExtreamUpper );
			l_poBaseUpper = l_poExtreamUpper;
			}
		l_poBaseLower = l_poBaseLower->f_poNext;
		l_poBaseUpper = l_poBaseUpper->f_poPrevious;
		l_iCtr -= 2;
		}
	return;
	M_EPILOG
	}

template<typename tType>
void HList<tType>::sub_swap ( HElement * a_poLeft, HElement * a_poCenter,
		HElement* a_poRight )
	{
	M_PROLOG
	HElement * l_poCenterPrevious = NULL, * l_poRightNext = NULL;
	HElement * l_poLeftPrevious = NULL;
	if ( a_poLeft == a_poCenter )
		return;
	if ( a_poLeft == f_poHook )
		f_poHook = a_poCenter;
	else if ( a_poCenter == f_poHook )
		f_poHook = a_poLeft;
	if ( a_poLeft == f_poSelected )
		f_poSelected = a_poCenter;
	else if ( a_poCenter == f_poSelected )
		f_poSelected = a_poLeft;
	if ( a_poLeft->f_poPrevious == a_poRight )
		return;
/*
 *   wwwwq lxxxxxx cyyyyyyr pwwww -> wwwwq cyyyyyyr lxxxxxx pwwww
 */
	
	l_poCenterPrevious = a_poCenter->f_poPrevious;
	l_poRightNext = a_poRight->f_poNext;
	l_poLeftPrevious = a_poLeft->f_poPrevious;
	
	l_poLeftPrevious->f_poNext = a_poCenter;
	l_poCenterPrevious->f_poNext = l_poRightNext;
	l_poRightNext->f_poPrevious = l_poCenterPrevious;
	a_poLeft->f_poPrevious = a_poRight;
	a_poRight->f_poNext = a_poLeft;
	a_poCenter->f_poPrevious = l_poLeftPrevious;
	
	return;
	M_EPILOG
	}

template<typename tType>
void HList<tType>::sort_by_contents ( sort_order_t a_eOrder )
	{
	M_PROLOG
	f_eOrder = a_eOrder;
	sort ( yaal::less<tType> );
	return ;
	M_EPILOG
	}

}

}

#endif /* not __YAAL_HCORE_HLIST_H */


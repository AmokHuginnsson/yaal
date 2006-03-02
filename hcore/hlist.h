/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hlist.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HLIST_H
#define __HCORE_HLIST_H

#line 31 "hlist.h"

#include "./hcore/hexception.h"

namespace stdhapi
{

namespace hcore
{

#define D_CVSID_HLIST_H "$CVSHeader$"

#ifndef NULL
#define NULL	0
#endif /* NULL */

#define E_HLIST_EMPTYELEMENT	1
#define E_HLIST_BADINDEX			2
#define E_HLIST_BADFLAG				3
#define E_HLIST_EMPTY					4
#define E_HLIST_BADOFFSET			5
#define E_HLIST_BADNUMBER			6
#define E_HLIST_BADORDER			7

extern char const * const g_ppcErrMsgHList [ ];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ general list declaration ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct OListTreatment
	{
	typedef enum
		{
		D_TREAT_AS_CLOSED = 1,
		D_TREAT_AS_OPENED = 2,
		D_BLOCK_IF_NOT_EMPTIED = 4,
		D_EMPTY_IF_NOT_EMPTIED = 8,
		D_FORCE_REMOVE_ELEMENT = 16
		} treatment_t;
	virtual ~OListTreatment ( void ) { } 
	};

template struct enum_t < OListTreatment::treatment_t >;

template < typename tType > 
class HList : public OListTreatment
	{
public:
	typedef enum
		{
		D_UNSORTED,
		D_ASCENDING,
		D_DESCENDING
		} sort_order_t;
	typedef enum
		{
		D_SEARCH_AFTER_ORDER,
		D_SEARCH_AFTER_NUMBER
		} search_after_t;
	typedef enum
		{
		D_OK = 0,
		D_ERROR = 1,
		D_WAS_EMPTIED = 2,
		D_WAS_NOT_EMPTIED = 4,
		D_FINAL_REACHED = 8,
		D_NOT_FOUND = 16
		} status_t;
protected:
	class HElement
		{
	protected:
		/*{*/
		HElement * f_poPrevious;
		/****************************************/
		/* here we have the most important part */
		/*           \      |     /             */
		/**/          tType f_tObject;        /**//**  The Object itself  **/
		/*           /      |     \             */
		/****************************************/
		HElement * f_poNext;
		int f_iNumber;				/* serial number */
		int long f_lHits;			/* how many times element's object was accessed */
		/*}*/
	public:
		/*{*/
		inline tType & get_object ( void ) /* this is special get used for  */
			{                                /* all derived classes (f.e. in  */
			return ( f_tObject );            /* compare methods)              */
			}                                /* this get does not modify hits */
		/*}*/
	protected:
		/*{*/
		HElement ( HElement *, int );
		virtual ~HElement ();
		void put ( tType );
		tType& get ( void );
		/*}*/
	private:
		/*{*/
		HElement ( HElement const & );
		HElement & operator = ( HElement const & );
		/*}*/
		friend class HList< tType >;
		};
private:
protected:
	/*{*/
	int f_iQuantity;					/* how many elements this list contains */
	int f_iHighestNumber;			/* serial number of last added element */
	int f_iError;							/* numer of errors in list access */
	HElement * f_poHook;			/* "begining" of the list ( "first" element ) */
	HElement * f_poSelected;	/* local temporary pointer, "cursor" */
/* for internal use only */
	sort_order_t f_eOrder;		/* last-to-current sort order */
	int f_iIndex;							/* this two fiels will allow boost operator [ ],
															 int holds last */
	HElement * f_poIndex;			/* index and HElement * holds pointer to this
															 last element */
	/*}*/
public:
	/*{*/
	HList ( int = 0 );                 /* Creates list, with specified size */
	virtual ~HList ( void );
	HList ( HList const & );
	HList & operator = ( HList const & );
	virtual void flush ( void );
	virtual int quantity ( void );
	virtual tType & add_element ( tType * = NULL ); /* adds new element at
																										 current cursor position */
	virtual tType & add_head ( tType * = NULL );    /* adds new element at
																										 beggining of the list */
	virtual tType & add_tail ( tType * = NULL );	/* adds new element at
																									 end of the list */
	virtual tType & add_at ( int, tType * = NULL ); /* adds new element
																										 at specified position */
/* adds element in the way that keeps order */
	virtual tType & add_orderly ( tType &, sort_order_t = D_ASCENDING );
	virtual int remove_element ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED | D_TREAT_AS_CLOSED,
			tType * * = NULL );	/* rmoves element at current cursor position */
	virtual int remove_at ( int, treatment_t const & = D_BLOCK_IF_NOT_EMPTIED | D_TREAT_AS_CLOSED,
			tType * * = NULL );
	virtual int remove_head ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, tType * * = NULL );
	virtual int remove_tail ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, tType * * = NULL );
	/* sets cursor at specified index or number */
	virtual tType & go ( int, search_after_t = D_SEARCH_AFTER_ORDER );
	virtual tType & operator [ ] ( int );
	virtual tType & present ( void );
	virtual tType & head ( void );
	virtual tType & tail ( void );
	virtual tType * to_head ( int = 1, treatment_t const & = D_TREAT_AS_CLOSED );
	virtual tType * to_tail ( int = 1, treatment_t const & = D_TREAT_AS_CLOSED );
	virtual void exchange ( int, int, search_after_t = D_SEARCH_AFTER_ORDER );
	virtual void sort_by_hits ( sort_order_t = D_ASCENDING );
	virtual void sort_by_number ( sort_order_t = D_ASCENDING );
	virtual void sort_by_contents ( sort_order_t = D_ASCENDING );
	operator bool ( void ) const;
	/*}*/
protected:
	/*{*/
	virtual bool to_head ( HElement * &, int = 1, treatment_t const & = D_TREAT_AS_CLOSED );
	virtual bool to_tail ( HElement * &, int = 1, treatment_t const & = D_TREAT_AS_CLOSED );
	virtual HElement * element_by_index ( int );
	virtual HElement * element_by_number ( int );
	bool ( HList< tType >::* IS_ABOVE ) ( HElement *, HElement * );
	virtual int long empty ( HElement * );
	virtual void exchange ( HElement *, HElement * );
	virtual void sub_swap ( HElement *, HElement *, HElement * );
	virtual bool is_above_h ( HElement *, HElement * );
	virtual bool is_above_n ( HElement *, HElement * );
	virtual bool is_above_c ( HElement *, HElement * );
	virtual void sort ( void );
	/*}*/
	};

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*++++++++++++++++++++++ general list implementation ++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template < typename tType >
HList < tType >::HElement::HElement ( HElement * a_poElement,
		int a_iNumber ) : f_poPrevious ( NULL ), f_tObject ( ), f_poNext ( NULL ),
											f_iNumber ( a_iNumber ), f_lHits ( 0 )
	{
	M_PROLOG
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
	M_EPILOG
	}

template < typename tType >
HList< tType >::HElement::~HElement ( void )
	{
	M_PROLOG
	f_poPrevious->f_poNext = f_poNext;
	f_poNext->f_poPrevious = f_poPrevious;
	return;
	M_EPILOG
	}

template < typename tType >
void HList< tType >::HElement::put ( tType a_tObject )
	{
	M_PROLOG
	f_lHits ++;
	f_tObject = a_tObject;
	return;
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::HElement::get ( void )
	{
	M_PROLOG
	f_lHits ++;
	return ( f_tObject );
	M_EPILOG
	}

//============================================================================

template < typename tType >
HList< tType >::HList ( int a_iSize )
	: OListTreatment ( ),
	f_iQuantity ( 0 ), f_iHighestNumber ( 0 ), f_iError ( 0 ),
	f_poHook ( NULL ), f_poSelected ( NULL ), f_eOrder ( D_UNSORTED ),
	f_iIndex ( 0 ), f_poIndex ( NULL ), IS_ABOVE ( NULL )
	{
	M_PROLOG
	while ( a_iSize -- )
		M_IRV ( HList::add_tail ( ) );
	return ;
	M_EPILOG
	}

template < typename tType >
HList< tType >::~HList ( void )
	{
	M_PROLOG
	HList::flush ();
	return ;
	M_EPILOG
	}

template < typename tType >
HList< tType >::HList ( HList < tType > const & a_roList )
	: OListTreatment ( ),
	f_iQuantity ( 0 ), f_iHighestNumber ( 0 ), f_iError ( 0 ),
	f_poHook ( NULL ), f_poSelected ( NULL ), f_eOrder ( D_UNSORTED ),
	f_iIndex ( 0 ), f_poIndex ( NULL ), IS_ABOVE ( NULL )
	{
	M_PROLOG
	( * this ) = a_roList;
	return;
	M_EPILOG
	}

template < typename tType >
HList< tType > & HList< tType >::operator = ( HList < tType > const & a_roList )
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
		l_iCount = f_iQuantity < a_roList.f_iQuantity ? f_iQuantity
																										: a_roList.f_iQuantity;
		/* I have to do this cast because to_tail modifies f_poSelected and
		 * declaring it (to_tail) const would be false, but after full loop
		 * of to_tail's obejct is unmodified */
		l_poList = const_cast < HList * > ( & a_roList );
		l_poSelected = a_roList.f_poSelected;
		l_poIndex = a_roList.f_poIndex;
		l_iIndex = a_roList.f_iIndex;
		if ( l_iCount )
			{
			M_IRV ( go ( - 1 ) );
			M_IRV ( l_poList->go ( - 1 ) );
			for ( l_iCtr = 0; l_iCtr < l_iCount; l_iCtr ++ )	
				{
				( * to_tail ( ) ) = ( * l_poList->to_tail ( ) );
				if ( a_roList.f_poSelected == a_roList.f_poHook )
					f_poHook = f_poSelected;
				if ( a_roList.f_poSelected == l_poIndex )
					l_poIndex = f_poSelected;
				if ( a_roList.f_poSelected == l_poSelected )
					l_poNewSelected = f_poSelected;
				}
			}
		else if ( a_roList.f_iQuantity )
			l_poList->go ( - 1 );
		if ( f_iQuantity > a_roList.f_iQuantity )
			{
			l_iCount = f_iQuantity - a_roList.f_iQuantity;
			while ( l_iCount -- )
				M_IRV ( remove_tail ( D_FORCE_REMOVE_ELEMENT ) );
			}
		else if ( f_iQuantity < a_roList.f_iQuantity )
			{
			for ( ; l_iCtr < a_roList.f_iQuantity; l_iCtr ++ )	
				{
				M_IRV ( add_tail ( l_poList->to_tail ( ) ) );
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
		f_iQuantity = a_roList.f_iQuantity;
		f_iHighestNumber = a_roList.f_iHighestNumber;
		f_iError = a_roList.f_iError;
		}
	return ( * this );
	M_EPILOG
	}

template < typename tType >
void HList < tType >::flush ( void )
	{
	M_PROLOG
	while ( f_iQuantity -- )
		{
		M_IRV ( empty ( f_poHook->f_poNext ) );
		delete f_poHook->f_poNext;
		}
	f_iIndex = 0;
	f_poIndex = NULL;
	f_poHook = NULL;
	f_poSelected = NULL;
	f_iQuantity = 0;
	f_iHighestNumber = 0;
	f_iError = 0;
	return ;
	M_EPILOG
	}

template < typename tType >
int long HList< tType >::empty ( HElement * a_poElement )
	{
	M_PROLOG
	int long l_lTemp = 0;
	l_lTemp = a_poElement->f_lHits;
	a_poElement->f_lHits = 0;
	return ( l_lTemp );
	M_EPILOG
	}

template < typename tType >
int HList< tType >::quantity ( void )
	{
	M_PROLOG
	return ( f_iQuantity );
	M_EPILOG
	}

template < typename tType >
bool HList< tType >::is_above_h ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	return ( f_eOrder == D_ASCENDING
			? a_poLeft->f_lHits > a_poRight->f_lHits
			: a_poRight->f_lHits > a_poLeft->f_lHits );
	M_EPILOG
	}

template < typename tType >
bool HList< tType >::is_above_n ( HElement * a_poLeft, HElement * a_poRight )	
	{
	M_PROLOG
	return ( f_eOrder == D_ASCENDING
			? a_poLeft->f_iNumber - a_poRight->f_iNumber
			: a_poRight->f_iNumber > a_poLeft->f_iNumber );
	M_EPILOG
	}

template < typename tType >
bool const operator > ( tType const &, tType const & )
#ifdef __STDHAPI_BUILD__
	;
#else /* __STDHAPI_BUILD__ */
	{
	if ( true == false )
		M_ASSERT ( ! "operator not defined" );
	return ( false );
	}
#endif /* not __STDHAPI_BUILD__ */


template < typename tType >
bool HList< tType >::is_above_c ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	return ( f_eOrder == D_ASCENDING
			? a_poLeft->f_tObject > a_poRight->f_tObject
			: a_poRight->f_tObject > a_poLeft->f_tObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::add_element ( tType * a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected = 
			new HElement ( NULL, f_iHighestNumber );
	else
		l_poElement = new HElement ( f_poSelected, f_iHighestNumber );
	f_iQuantity ++;
	f_iHighestNumber ++;
	if ( a_ptObject )
		l_poElement->put ( * a_ptObject );
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::add_head ( tType * a_ptObject )
	{
	M_PROLOG
	f_poHook = new HElement ( f_poHook, f_iHighestNumber );
	if ( f_iQuantity == 0 )
		f_poSelected = f_poHook;
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )
		f_poHook->put ( * a_ptObject );
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::add_tail ( tType * a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected =
			new HElement ( NULL, f_iHighestNumber );
	else
		l_poElement = new HElement ( f_poHook, f_iHighestNumber );
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )
		l_poElement->put ( * a_ptObject );
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::add_at ( int a_iIndex, tType * a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( a_iIndex > f_iQuantity )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADINDEX ], a_iIndex );
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected =
			new HElement ( NULL, f_iHighestNumber );
	else
		{
		if ( a_iIndex < 0 )
			l_poElement = f_poHook;
		else
			l_poElement = element_by_index ( a_iIndex );
		l_poElement = new HElement ( l_poElement, f_iHighestNumber );
		if ( ! a_iIndex )
			f_poHook = l_poElement;
		}
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )
		l_poElement->put ( * a_ptObject );
	if ( f_poIndex &&  ( a_iIndex >= 0 ) && ( a_iIndex <= f_iIndex ) )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::add_orderly ( tType & a_rtObject,
		sort_order_t a_eOrder )
	{
	M_PROLOG
	bool l_bBefore = false;
	int l_iIndex = 0, l_iOldIndex = -1, l_iLower = 0, l_iUpper = f_iQuantity;
	HElement * l_poElement = new HElement ( NULL, f_iHighestNumber );
	l_poElement->put ( a_rtObject );
	if ( ( f_eOrder != D_UNSORTED ) && ( f_eOrder != a_eOrder ) )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADORDER ],
				static_cast < int > ( a_eOrder ) );
	f_eOrder = a_eOrder;
	while ( f_iQuantity && ( l_iOldIndex != l_iIndex ) )
		{
		l_iOldIndex = l_iIndex;
		l_iIndex = ( l_iLower + l_iUpper ) / 2;
		M_IRV ( element_by_index ( l_iIndex ) );
		if ( is_above_c ( f_poIndex, l_poElement ) )
			l_iLower = l_iIndex;
		else
			l_iUpper = l_iIndex;
		}
	if ( f_poIndex )
		{
		if ( is_above_c ( f_poIndex, l_poElement ) )
			f_poIndex = f_poIndex->f_poNext;
		else
			l_bBefore = true;
		}
	if ( ! f_iQuantity )
		f_poHook = f_poIndex = l_poElement;
	else
		{
		l_poElement->f_poNext = f_poIndex;
		l_poElement->f_poPrevious = f_poIndex->f_poPrevious;
		f_poIndex->f_poPrevious->f_poNext = l_poElement;
		f_poIndex->f_poPrevious = l_poElement;
		}
	f_iQuantity ++;
	f_iHighestNumber ++;
	f_poIndex = NULL;
	f_iIndex = 0;
	if ( l_bBefore && ! l_iIndex )
		f_poHook = f_poHook->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < typename tType >
int HList< tType >::remove_at ( int a_iIndex, treatment_t const & a_eFlag, tType * * a_pptObject )
	{
	M_PROLOG
	treatment_t l_eFlag = a_eFlag & ( D_BLOCK_IF_NOT_EMPTIED | D_EMPTY_IF_NOT_EMPTIED | D_FORCE_REMOVE_ELEMENT );
	treatment_t l_eTreat = a_eFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
	int l_iError = 0;
	HElement * l_poElement = NULL;
	l_poElement = f_poHook;
	if ( f_iQuantity > 0 )
		{
		if ( a_iIndex >= f_iQuantity )
			{
			f_iError++;
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADINDEX ], a_iIndex );
			}
		l_poElement = element_by_index ( a_iIndex );
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		if ( l_poElement->f_lHits )
			{
			switch ( l_eFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					return ( D_ERROR );
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					M_IRV ( empty ( l_poElement ) );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( l_eFlag ) );
					break;
					}
				}
			}
		}
	else
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
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
	f_iQuantity--;
	if ( f_iQuantity == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return ( l_iError );
	M_EPILOG
	}

template < typename tType >
int HList< tType >::remove_element ( treatment_t const & a_eFlag, tType * * a_pptObject )
	{
	M_PROLOG
	treatment_t l_eFlag =  a_eFlag & ( D_BLOCK_IF_NOT_EMPTIED | D_EMPTY_IF_NOT_EMPTIED | D_FORCE_REMOVE_ELEMENT );
	treatment_t l_eTreat = a_eFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
	int l_iError = 0;
	HElement * l_poElement = NULL;
	if ( f_iQuantity > 0 )
		{
		l_poElement = f_poSelected;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		if ( l_poElement->f_lHits )
			{
			switch ( l_eFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					return ( D_ERROR );
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					M_IRV ( empty ( l_poElement ) );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( l_eFlag ) );
					break;
					}
				}
			}
		}
	else
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
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
	f_iQuantity --;
	if ( f_iQuantity == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		}
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_iError );
	M_EPILOG
	}

template < typename tType >
int HList< tType >::remove_head ( treatment_t const & a_eFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iError = 0;
	HElement * l_poElement = NULL;
	if ( f_iQuantity > 0 )
		{
		l_poElement = f_poHook;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		f_poHook = f_poHook->f_poNext;
		if ( l_poElement->f_lHits )
			{
			switch ( a_eFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					return ( D_ERROR );
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					M_IRV ( empty ( l_poElement ) );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( a_eFlag ) );
					break;
					}
				}
			}
		}
	else
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( l_poElement == f_poSelected )
		f_poSelected = l_poElement->f_poNext;
	if ( f_poIndex )
		f_poIndex = f_poIndex->f_poNext;
	delete l_poElement;
	f_iQuantity--;
	if ( f_iQuantity == 0 )
		{
		f_poHook = NULL;
		f_poSelected = NULL;
		f_poIndex = NULL;
		f_iIndex = 0;
		}
	return ( l_iError );
	M_EPILOG
	}

template < typename tType >
int HList< tType >::remove_tail ( treatment_t const & a_eFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iError = 0;
	HElement * l_poElement = NULL;
	if ( f_iQuantity > 0 )
		{
		l_poElement = f_poHook->f_poPrevious;
		if ( a_pptObject )
			( * a_pptObject ) = & l_poElement->f_tObject;
		if ( l_poElement->f_lHits )
			{
			switch ( a_eFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					return ( D_ERROR );
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					M_IRV ( empty ( l_poElement ) );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( a_eFlag ) );
					break;
					}
				}
			}
		if ( l_poElement == f_poSelected )
			f_poSelected = l_poElement->f_poPrevious;
		if ( l_poElement == f_poIndex )
			{
			f_poIndex = l_poElement->f_poPrevious;
			f_iIndex --;
			}
		delete l_poElement;
		f_iQuantity--;
		if ( f_iQuantity == 0 )
			{
			f_poHook = NULL;
			f_poSelected = NULL;
			f_poIndex = NULL;
			f_iIndex = 0;
			}
		}
	else
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( l_iError );
	M_EPILOG
	}

template < typename tType >
bool HList< tType >::to_head ( HElement * & a_rpoElement, int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOk = true;
	int l_iCtr = 0;
	if ( ! f_iQuantity )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADOFFSET ], a_iOffset );
	switch ( a_eFlag )
		{
		case ( D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				a_rpoElement = a_rpoElement->f_poPrevious;
			break;
			}
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
			break;
			}
		default :
			{
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( a_eFlag ) );
			break;
			}
		}
	return ( l_bOk );
	M_EPILOG
	}

template < typename tType >
tType * HList< tType >::to_head ( int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( f_poSelected == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 0 )
		l_bOk = to_tail ( f_poSelected, - a_iOffset, a_eFlag );
	else
		l_bOk = to_head ( f_poSelected, a_iOffset, a_eFlag );
	if ( l_bOk )
		l_ptObject = & f_poSelected->get ( );
	return ( l_ptObject );
	M_EPILOG
	}

template < typename tType >
bool HList< tType >::to_tail ( HElement * & a_rpoElement, int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOK = true;
	int l_iCtr = 0;
	if ( ! f_iQuantity )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADOFFSET ], a_iOffset );
	switch ( a_eFlag )
		{
		case ( D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				a_rpoElement = a_rpoElement->f_poNext;
			break;
			}
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
			break;
			}
		default :
			{
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], static_cast < int > ( a_eFlag ) );
			break;
			}
		}
	return ( l_bOK );
	M_EPILOG
	}

template < typename tType >
tType * HList< tType >::to_tail ( int a_iOffset, treatment_t const & a_eFlag )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( f_poSelected == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 0 )
		l_bOk = to_head ( f_poSelected, - a_iOffset, a_eFlag );
	else
		l_bOk = to_tail ( f_poSelected, a_iOffset, a_eFlag );
	if ( l_bOk )
		l_ptObject = & f_poSelected->get ( );
	return ( l_ptObject );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	return ( element_by_index ( a_iIndex )->get ( ) );
	M_EPILOG
	}

template < typename tType >
typename HList< tType >::HElement * HList < tType >::element_by_index ( int a_iIndex )
	{
	M_PROLOG
	if ( f_iQuantity == 0 )
		{
		f_iError ++;
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
		}
	if ( a_iIndex < 0 )
		a_iIndex += f_iQuantity;
	if ( ( a_iIndex >= f_iQuantity ) || ( a_iIndex < 0 ) )
		{
		f_iError ++;
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADINDEX ], a_iIndex );
		}
	if ( ! f_poIndex )
		f_poIndex = f_poHook;
/*
we are looking for the smallest distance between a_iIndex and 0, f_iIndex, f_iQuantity.
we have two cases, a_iIndex is between 0 and f_iIndex:
	0 ..... a_iIndex ..... f_iIndex ..... f_iQuantity
meaning a_iIndex < f_iIndex
and a_iIndex is between f_iIndex and f_iQuantity
	0 ..... f_iIndex ..... a_iIndex ..... f_iQuantity
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
		if ( a_iIndex < ( ( f_iQuantity + f_iIndex ) / 2 ) )
			for ( ; f_iIndex < a_iIndex; f_iIndex ++ )
				f_poIndex = f_poIndex->f_poNext;
		else
			for ( f_iIndex = f_iQuantity - 1, f_poIndex = f_poHook->f_poPrevious;
					f_iIndex > a_iIndex; f_iIndex -- )
				f_poIndex = f_poIndex->f_poPrevious;
		}
	return ( f_poIndex );
	M_EPILOG
	}

template < typename tType >
typename HList< tType >::HElement * HList < tType >::element_by_number ( int a_iNumber )
	{
	M_PROLOG
	int l_iCtr = 0;
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		{
		f_iError ++;
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
		}
	l_poElement = f_poHook;
	for ( l_iCtr = 0; l_iCtr < f_iQuantity; l_iCtr++ )
		{
		if ( l_poElement->f_iNumber == a_iNumber )
			return ( l_poElement );
		l_poElement = l_poElement->f_poNext;
		}
	M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADNUMBER ], a_iNumber );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::go ( int a_iNumber, search_after_t a_eFlag )
	{
	M_PROLOG
	/* Here we have another function with error code returned by reference */
	switch ( a_eFlag )
		{
		case ( D_SEARCH_AFTER_NUMBER ):
			{
			f_poSelected = element_by_number ( a_iNumber );
			break;
			}
		case ( D_SEARCH_AFTER_ORDER ):
			{
			f_poSelected = element_by_index ( a_iNumber );
			break;
			}
		default :
			{
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_eFlag );
			break;
			}
		}
	return ( f_poSelected->get ( ) );
	M_EPILOG
	}

template < typename tType >
void HList< tType >::exchange ( HElement * a_poLeft, HElement * a_poRight )
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

template < typename tType >
void HList< tType >::exchange ( int a_iLeft, int a_iRight, search_after_t a_eFlag )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iOther = 0;
	HElement * l_poLeft = NULL, * l_poRight = NULL;
	if ( a_iLeft == a_iRight )
		return;
	switch ( a_eFlag )
		{
		case ( D_SEARCH_AFTER_NUMBER ):
			{
			l_poLeft = f_poHook;
			for ( l_iCtr = 0; l_iCtr < f_iQuantity; l_iCtr++ )
				{
				if ( ( l_poLeft->f_iNumber == a_iLeft )
						|| ( l_poLeft->f_iNumber == a_iRight  ) )
					break;
				l_poLeft = l_poLeft->f_poNext;
				}
			l_poRight = l_poLeft;
			if ( l_poLeft->f_iNumber == a_iLeft )
				l_iOther = a_iRight;
			else
				l_iOther = a_iLeft;
			for ( ; l_iCtr < f_iQuantity; l_iCtr++ )
				{
				if ( l_poRight->f_iNumber == l_iOther )
					break;
				l_poRight = l_poRight->f_poNext;
				}
			if ( l_iCtr >= f_iQuantity )
				{
				f_iError++;
				return ;
				}
			break;
			}
		case ( D_SEARCH_AFTER_ORDER ):
			{
			l_poLeft = element_by_index ( a_iLeft );
			l_poRight = element_by_index ( a_iRight );
			break;
			}
		default :
			{
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_eFlag );
			break;
			}
		}
	exchange ( l_poLeft, l_poRight );
	return;
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::present ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poSelected->get ( ) );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::head ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poHook->get ( ) );
	M_EPILOG
	}

template < typename tType >
tType & HList< tType >::tail ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poHook->f_poPrevious->get ( ) );
	M_EPILOG
	}

template < typename tType >
void HList< tType >::sort ( void )
	{
	M_PROLOG
	int l_iCtr = f_iQuantity;
	int l_iCtrLoc = 0;
	HElement * l_poBaseLower = f_poHook;
	HElement * l_poBaseUpper = f_poHook->f_poPrevious;
	HElement * l_poExtreamLower = NULL;
	HElement * l_poExtreamUpper = NULL;
	HElement * l_poPointer = NULL;
	if ( ( f_eOrder != D_ASCENDING ) && ( f_eOrder != D_DESCENDING ) )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADORDER ],
				static_cast < int > ( f_eOrder ) );
	while ( l_iCtr >= 0 )
		{
		l_iCtrLoc = l_iCtr;
		l_poExtreamLower = l_poBaseLower;
		l_poExtreamUpper = l_poBaseUpper;
		l_poPointer = l_poBaseLower;
		while ( l_iCtrLoc -- )
			{
			if ( ( l_poPointer != l_poExtreamLower )
					&& ( ( this->* IS_ABOVE ) ( l_poExtreamLower, l_poPointer ) ) )
				l_poExtreamLower = l_poPointer;
			if ( ( l_poPointer != l_poExtreamUpper )
					&& ( ( this->* IS_ABOVE ) ( l_poPointer, l_poExtreamUpper ) ) )
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

template < typename tType >
void HList< tType >::sub_swap ( HElement * a_poLeft, HElement * a_poCenter,
		HElement * a_poRight )
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

template < typename tType >
void HList< tType >::sort_by_hits ( sort_order_t a_eOrder )
	{
	M_PROLOG
	f_eOrder = a_eOrder;
	IS_ABOVE = & HList< tType >::is_above_h ;
	sort ( );
	return ;
	M_EPILOG
	}

template < typename tType >
void HList < tType >:: sort_by_number ( sort_order_t a_eOrder )
	{
	M_PROLOG
	f_eOrder = a_eOrder;
	IS_ABOVE = & HList< tType >::is_above_n;
	sort ( );
	return ;
	M_EPILOG
	}

template < typename tType >
void HList < tType >:: sort_by_contents ( sort_order_t a_eOrder )
	{
	M_PROLOG
	f_eOrder = a_eOrder;
	IS_ABOVE = & HList< tType >::is_above_c;
	sort ( );
	return ;
	M_EPILOG
	}

template < typename tType >
HList< tType >::operator bool ( void ) const
	{
	M_PROLOG
	return ( f_iQuantity ? true : false );
	M_EPILOG
	}

}

}

#endif /* not __HCORE_HLIST_H */

/*
#ifdef __DEBUG__
#include <hlog.h>
#endif / * __DEBUG__ * /
	M_LOG ( "one" );
*/


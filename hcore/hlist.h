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

#ifndef __HLIST_H
#define __HLIST_H

#line 31 "hlist.h"

#define D_ERROR									1
#define D_TREAT_AS_CLOSED				2
#define D_TREAT_AS_OPENED				4
#define D_SEARCH_AFTER_ORDER		8
#define D_SEARCH_AFTER_NUMBER		16
#define D_BLOCK_IF_NOT_EMPTIED	32
#define D_EMPTY_IF_NOT_EMPTIED	64
#define D_FORCE_REMOVE_ELEMENT	128
#define	D_WAS_EMPTIED						256
#define	D_WAS_NOT_EMPTIED				512
#define D_FINAL_REACHED					1024
#define D_NOT_FOUND							2048
#define D_UNSORTED							0
#define D_ASCENDING							8
#define D_DESCENDING						-8

#ifndef NULL
#define NULL	0
#endif /* NULL */

#define D_CVSID_HLIST_H "$CVSHeader$"

#define E_HLIST_EMPTYELEMENT	0
#define E_HLIST_BADINDEX			1
#define E_HLIST_BADFLAG				2
#define E_HLIST_EMPTY					3
#define E_HLIST_BADOFFSET			4
#define E_HLIST_BADNUMBER			5

extern const char * g_ppcErrMsgHList [ ];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ general list declaration ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++                          ++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template < class tType > 
class HList
	{
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
		long int f_lHits;			/* how many times element's object was accessed */
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
		friend class HList< tType >;
		};
protected:
	/*{*/
	int f_iQuantity;					/* how many elements this list contains */
	int f_iHighestNumber;			/* serial number of last added element */
	int f_iError;							/* numer of errors in list access */
	HElement * f_poHook;			/* "begining" of the list ( "first" element ) */
	HElement * f_poSelected;	/* local temporary pointer, "cursor" */
/* for internal use only */
	int f_iOrder;							/* last-to-current sort order */
	int f_iIndex;							/* this two fiels will allow boost operator [ ],
															 int holds last */
	HElement * f_poIndex;			/* index and HElement * holds pointer to this
															 last element */
	/*}*/
public:
	/*{*/
	HList ( int = 0 );                 /* Creates list, with specified size */
	virtual ~HList ( void );
	virtual void flush ( void );
	virtual int quantity ( void );
	virtual tType & add_element ( tType * = NULL ); /* adds new element at current cursor 
																						position */
	virtual tType & add_head ( tType * = NULL );    /* adds new element at beggining of 
																						the list */
	virtual tType & add_tail ( tType * = NULL );	/* adds new element at end of the list */
	virtual tType & add_at ( int, tType * = NULL ); /* adds new element at specified 
																						position */
	virtual tType & add_orderly ( tType, int = D_ASCENDING ); /* adds element in
																															 the way that
																															 keeps order */
	virtual int remove_element ( int = D_BLOCK_IF_NOT_EMPTIED | D_TREAT_AS_CLOSED,
			tType * * = NULL );	/* rmoves element at current cursor position */
	virtual int remove_at ( int, int = D_BLOCK_IF_NOT_EMPTIED | D_TREAT_AS_CLOSED,
			tType * * = NULL );
	virtual int remove_head ( int = D_BLOCK_IF_NOT_EMPTIED, tType * * = NULL );
	virtual int remove_tail ( int = D_BLOCK_IF_NOT_EMPTIED, tType * * = NULL );
	/* sets cursor at specified index or number */
	virtual int go ( int, tType * * = NULL, int = D_SEARCH_AFTER_ORDER );
	virtual tType & operator [ ] ( int );
	virtual tType & present ( void );
	virtual tType & head ( void );
	virtual tType & tail ( void );
	virtual bool to_head ( int = 1, int = D_TREAT_AS_CLOSED, tType * * = NULL );
	virtual bool to_tail ( int = 1, int = D_TREAT_AS_CLOSED, tType * * = NULL );
	virtual void exchange ( int, int, int = D_SEARCH_AFTER_ORDER );
	virtual void sort_by_hits ( int = D_ASCENDING );
	virtual void sort_by_number ( int = D_ASCENDING );
	virtual void sort_by_contents ( int = D_ASCENDING );
	/*}*/
protected:
	/*{*/
	virtual bool to_head ( HElement * &, int = 1, int = D_TREAT_AS_CLOSED );
	virtual bool to_tail ( HElement * &, int = 1, int = D_TREAT_AS_CLOSED );
	virtual HElement * element_by_index ( int );
	virtual HElement * element_by_number ( int );
	int ( HList< tType >::*cmp ) ( HElement *, HElement * );	/* not realy a
																																	 function */
	virtual long int empty ( HElement * );
	virtual void exchange ( HElement *, HElement * );
	virtual void sub_swap ( HElement *, HElement *, HElement * );
	virtual int cmph ( HElement *, HElement * );
	virtual int cmpn ( HElement *, HElement * );
	virtual int cmpc ( HElement *, HElement * );
	virtual void sort ( );
	/*}*/
	};

#include "./hcore/hexception.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*++++++++++++++++++++++ general list implementation ++++++++++++++++++++++*/
/*++++++++++++++++++++++                             ++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template < class tType >
HList < tType >::HElement::HElement ( HElement * a_poElement,
		int a_iNumber )
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
	f_iNumber = a_iNumber;
	f_lHits = 0;
	M_EPILOG
	}

template < class tType >
HList< tType >::HElement::~HElement ( void )
	{
	M_PROLOG
	f_poPrevious->f_poNext = f_poNext;
	f_poNext->f_poPrevious = f_poPrevious;
	return;
	M_EPILOG
	}

template < class tType >
void HList< tType >::HElement::put ( tType a_tObject )
	{
	M_PROLOG
	f_lHits ++;
	f_tObject = a_tObject;
	return;
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::HElement::get ( void )
	{
	M_PROLOG
	f_lHits ++;
	return ( f_tObject );
	M_EPILOG
	}

//============================================================================

template < class tType >
HList< tType >::HList ( int a_iSize )
	{
	M_PROLOG
	f_iOrder = D_UNSORTED;
	f_iIndex = 0;
	f_poIndex = NULL;
	f_iError = 0;
	f_iQuantity = 0;
	f_iHighestNumber = 0;
	f_poHook = 0;
	f_poSelected = 0;
	while ( a_iSize -- )add_tail ( );
	return ;
	M_EPILOG
	}

template < class tType >
HList< tType >::~HList ( void )
	{
	M_PROLOG
	flush ();
	return ;
	M_EPILOG
	}

template < class tType >
void HList < tType >::flush ( void )
	{
	M_PROLOG
	while ( f_iQuantity -- )
		{
		empty ( f_poHook->f_poNext );
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

template < class tType >
long int HList< tType >::empty ( HElement * a_poElement )
	{
	M_PROLOG
	long int l_lTemp;
	l_lTemp = a_poElement->f_lHits;
	a_poElement->f_lHits = 0;
	return ( l_lTemp );
	M_EPILOG
	}

template < class tType >
int HList< tType >::quantity ( void )
	{
	M_PROLOG
	return ( f_iQuantity );
	M_EPILOG
	}

template < class tType >
int HList< tType >::cmph ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	return ( a_poLeft->f_lHits - a_poRight->f_lHits );
	M_EPILOG
	}

template < class tType >
int HList< tType >::cmpn ( HElement * a_poLeft, HElement * a_poRight )	
	{
	M_PROLOG
	return ( a_poLeft->f_iNumber - a_poRight->f_iNumber );
	M_EPILOG
	}

template < class tType >
int compare_contents ( tType & a_tLeft, tType & a_tRight )
	{
	return ( a_tLeft - a_tRight );
	}

template < class tType >
int HList< tType >::cmpc ( HElement * a_poLeft, HElement * a_poRight )	
	{
	M_PROLOG
	return ( compare_contents ( a_poLeft->f_tObject, a_poRight->f_tObject ) );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_element ( tType * a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected = 
			new HElement ( NULL, f_iHighestNumber );
	else l_poElement = new HElement ( f_poSelected, f_iHighestNumber );
	f_iQuantity ++;
	f_iHighestNumber ++;
	if ( a_ptObject )l_poElement->put ( * a_ptObject );
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_head ( tType * a_ptObject )
	{
	M_PROLOG
	f_poHook = new HElement ( f_poHook, f_iHighestNumber );
	if ( f_iQuantity == 0 ) f_poSelected = f_poHook;
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )f_poHook->put ( * a_ptObject );
	if ( f_poIndex )f_poIndex = f_poIndex->f_poPrevious;
	return ( f_poHook->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_tail ( tType * a_ptObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected =
			new HElement ( NULL, f_iHighestNumber );
	else l_poElement = new HElement ( f_poHook, f_iHighestNumber );
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )l_poElement->put ( * a_ptObject );
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
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
		if ( a_iIndex < 0 )l_poElement = f_poHook;
		else l_poElement = element_by_index ( a_iIndex );
		l_poElement = new HElement ( l_poElement, f_iHighestNumber );
		if ( ! a_iIndex )f_poHook = l_poElement;
		}
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_ptObject )l_poElement->put ( * a_ptObject );
	if ( f_poIndex &&  ( a_iIndex >= 0 ) && ( a_iIndex <= f_iIndex ) )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_orderly ( tType a_tObject, int a_iOrder )
	{
	M_PROLOG
#define M_SWITCH ( cmpc ( f_poIndex, l_poElement ) * a_iOrder ) < 0
	bool l_bBefore = false;
	int l_iIndex = 0, l_iOldIndex = -1, l_iLower = 0, l_iUpper = f_iQuantity;
	HElement * l_poElement = new HElement ( NULL, f_iHighestNumber );
	l_poElement->put ( a_tObject );
	while ( f_iQuantity && ( l_iOldIndex != l_iIndex ) )
		{
		l_iOldIndex = l_iIndex;
		l_iIndex = ( l_iLower + l_iUpper ) / 2;
		element_by_index ( l_iIndex );
		if ( M_SWITCH )l_iLower = l_iIndex;
		else l_iUpper = l_iIndex;
		}
	if ( f_poIndex )
		{
		if ( M_SWITCH )f_poIndex = f_poIndex->f_poNext;
		else l_bBefore = true;
		}
	if ( ! f_iQuantity )f_poHook = f_poIndex = l_poElement;
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
	if ( l_bBefore && ! l_iIndex )f_poHook = f_poHook->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
int HList< tType >::remove_at ( int a_iIndex, int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iFlag = 0, l_iTreat = 0, l_iError = 0;
	HElement * l_poElement = NULL;
	l_iFlag = a_iFlag & ( D_BLOCK_IF_NOT_EMPTIED | D_EMPTY_IF_NOT_EMPTIED | D_FORCE_REMOVE_ELEMENT );
	l_iTreat = a_iFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
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
			switch ( l_iFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					{
					return ( D_ERROR );
					break;
					}
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], l_iFlag );
					break;
					}
				}
			}
		}
	else M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( l_poElement == f_poHook )f_poHook = f_poHook->f_poNext;
	if ( l_poElement == f_poSelected )
		{
		if ( ( l_iTreat == D_TREAT_AS_OPENED )
				&& ( f_poSelected->f_poNext == f_poHook ) )
			f_poSelected = f_poSelected->f_poPrevious;
		else
			f_poSelected = f_poSelected->f_poNext;
		}
	if ( f_poIndex && ( a_iIndex <= f_iIndex ) )f_poIndex = f_poIndex->f_poNext;
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

template < class tType >
int HList< tType >::remove_element ( int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iFlag = 0, l_iTreat = 0, l_iError = 0;
	HElement * l_poElement = NULL;
	l_iFlag = a_iFlag & ( D_BLOCK_IF_NOT_EMPTIED | D_EMPTY_IF_NOT_EMPTIED | D_FORCE_REMOVE_ELEMENT );
	l_iTreat = a_iFlag & ( D_TREAT_AS_CLOSED | D_TREAT_AS_OPENED );
	l_poElement = f_poSelected;
	if ( a_pptObject )
		( * a_pptObject ) = & l_poElement->f_tObject;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( l_iFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					{
					return ( D_ERROR );
					break;
					}
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], l_iFlag );
					break;
					}
				}
			}
		}
	else M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( ( l_iTreat == D_TREAT_AS_OPENED )
			&& ( f_poSelected->f_poNext == f_poHook ) )
		f_poSelected = f_poSelected->f_poPrevious;
	else
		f_poSelected = f_poSelected->f_poNext;
	if ( l_poElement == f_poHook )f_poHook = f_poHook->f_poNext;
	delete l_poElement;
	f_iQuantity--;
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

template < class tType >
int HList< tType >::remove_head ( int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iError = 0;
	HElement * l_poElement = NULL;
	l_poElement = f_poHook;
	if ( a_pptObject )
		( * a_pptObject ) = & l_poElement->f_tObject;
	f_poHook = f_poHook->f_poNext;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( a_iFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					{
					return ( D_ERROR );
					break;
					}
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
					break;
					}
				}
			}
		}
	else M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( l_poElement == f_poSelected ) f_poSelected = l_poElement->f_poNext;
	if ( f_poIndex )f_poIndex = f_poIndex->f_poNext;
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

template < class tType >
int HList< tType >::remove_tail ( int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	int l_iError = 0;
	HElement * l_poElement = NULL;
	l_poElement = f_poHook->f_poPrevious;
	if ( a_pptObject )
		( * a_pptObject ) = & l_poElement->f_tObject;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( a_iFlag )
				{
				case ( D_BLOCK_IF_NOT_EMPTIED ):
					{
					return ( D_ERROR );
					break;
					}
				case ( D_EMPTY_IF_NOT_EMPTIED ):
					{
					l_iError = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( D_FORCE_REMOVE_ELEMENT ):
					{
					l_iError = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
					break;
					}
				}
			}
		if ( l_poElement == f_poSelected )f_poSelected = l_poElement->f_poPrevious;
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
	else M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( l_iError );
	M_EPILOG
	}

template < class tType >
bool HList< tType >::to_head ( HElement * & a_rpoElement, int a_iOffset, int a_iFlag )
	{
	M_PROLOG
	bool l_bOk = true;
	int l_iCtr = 0;
	if ( ! f_iQuantity )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADOFFSET ], a_iOffset );
	switch ( a_iFlag )
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
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
			break;
			}
		}
	return ( l_bOk );
	M_EPILOG
	}

template < class tType >
bool HList< tType >::to_head ( int a_iOffset, int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( a_pptObject )
		l_ptObject = & f_poSelected->get ( );
	if ( a_iOffset < 0 )
		l_bOk = to_tail ( f_poSelected, - a_iOffset, a_iFlag );
	else
		l_bOk = to_head ( f_poSelected, a_iOffset, a_iFlag );
	if ( l_bOk && a_pptObject )
		( * a_pptObject ) = l_ptObject;
	return ( l_bOk );
	M_EPILOG
	}

template < class tType >
bool HList< tType >::to_tail ( HElement * & a_rpoElement, int a_iOffset, int a_iFlag )
	{
	M_PROLOG
	bool l_bOK = true;
	int l_iCtr = 0;
	if ( ! f_iQuantity )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	if ( a_iOffset < 1 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADOFFSET ], a_iOffset );
	switch ( a_iFlag )
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
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
			break;
			}
		}
	return ( l_bOK );
	M_EPILOG
	}

template < class tType >
bool HList< tType >::to_tail ( int a_iOffset, int a_iFlag, tType * * a_pptObject )
	{
	M_PROLOG
	bool l_bOk = false;
	tType * l_ptObject = NULL;
	if ( a_pptObject )
		l_ptObject = & f_poSelected->get ( );
	if ( a_iOffset < 0 )
		l_bOk = to_head ( f_poSelected, - a_iOffset, a_iFlag );
	else
		l_bOk = to_tail ( f_poSelected, a_iOffset, a_iFlag );
	if ( l_bOk && a_pptObject )
		( * a_pptObject ) = l_ptObject;
	return ( l_bOk );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	return ( element_by_index ( a_iIndex )->get ( ) );
	M_EPILOG
	}

template < class tType >
typename HList< tType >::HElement * HList < tType >::element_by_index ( int a_iIndex )
	{
	M_PROLOG
	if ( f_iQuantity == 0 )
		{
		f_iError ++;
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
		}
	if ( a_iIndex < 0 )a_iIndex += f_iQuantity;
	if ( ( a_iIndex >= f_iQuantity ) || ( a_iIndex < 0 ) )
		{
		f_iError ++;
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADINDEX ], a_iIndex );
		}
	if ( ! f_poIndex )f_poIndex = f_poHook;
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

template < class tType >
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
		if ( l_poElement->f_iNumber == a_iNumber ) return ( l_poElement );
		l_poElement = l_poElement->f_poNext;
		}
	M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADNUMBER ], a_iNumber );
	return ( NULL );
	M_EPILOG
	}

template < class tType >
int HList< tType >::go ( int a_iNumber, tType * * a_pptObject, int a_iFlag )
	{
	M_PROLOG
	/* Here we have another function with error code returned by reference */
	switch ( a_iFlag )
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
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
			break;
			}
		}
	if ( a_pptObject )
		( * a_pptObject ) = & f_poSelected->get ( );
	return ( 0 );
	M_EPILOG
	}

template < class tType >
void HList< tType >::exchange ( HElement * a_poLeft, HElement * a_poRight )
	{
	M_PROLOG
	HElement * l_poNext = NULL, * l_poPrevious = NULL;
	if ( a_poLeft == a_poRight )return;
	if ( a_poLeft == f_poHook )f_poHook = a_poRight;
	else if ( a_poRight == f_poHook )f_poHook = a_poLeft;
	if ( a_poLeft == f_poSelected )f_poSelected = a_poRight;
	else if ( a_poRight == f_poSelected )f_poSelected = a_poLeft;
	if ( a_poLeft == f_poIndex )f_poIndex = a_poRight;
	else if ( a_poRight == f_poIndex )f_poIndex = a_poLeft;
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

template < class tType >
void HList< tType >::exchange ( int a_iLeft, int a_iRight, int a_iFlag )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iOther = 0;
	HElement * l_poLeft = NULL, * l_poRight = NULL;
	if ( a_iLeft == a_iRight )return;
	switch ( a_iFlag )
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
			if ( l_poLeft->f_iNumber == a_iLeft )l_iOther = a_iRight;
			else l_iOther = a_iLeft;
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
			M_THROW ( g_ppcErrMsgHList [ E_HLIST_BADFLAG ], a_iFlag );
			break;
			}
		}
	exchange ( l_poLeft, l_poRight );
	return;
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::present ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poSelected->get ( ) );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::head ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poHook->get ( ) );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::tail ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		M_THROW ( g_ppcErrMsgHList [ E_HLIST_EMPTY ], g_iErrNo );
	return ( f_poHook->f_poPrevious->get ( ) );
	M_EPILOG
	}

template < class tType >
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
	while ( l_iCtr >= 0 )
		{
		l_iCtrLoc = l_iCtr;
		l_poExtreamLower = l_poBaseLower;
		l_poExtreamUpper = l_poBaseUpper;
		l_poPointer = l_poBaseLower;
		while ( l_iCtrLoc -- )
			{
			if ( ( l_poPointer != l_poExtreamLower )
					&& ( ( ( this->* cmp ) ( l_poExtreamLower,
								l_poPointer ) * f_iOrder ) > 0 ) )
				l_poExtreamLower = l_poPointer;
			if ( ( l_poPointer != l_poExtreamUpper )
					&& ( ( ( this->* cmp ) ( l_poExtreamUpper,
								l_poPointer ) * f_iOrder ) < 0 ) )
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

template < class tType >
void HList< tType >::sub_swap ( HElement * a_poLeft, HElement * a_poCenter,
		HElement * a_poRight )
	{
	M_PROLOG
	HElement * l_poCenterPrevious = NULL, * l_poRightNext = NULL;
	HElement * l_poLeftPrevious = NULL;
	if ( a_poLeft == a_poCenter )return;
	if ( a_poLeft == f_poHook )f_poHook = a_poCenter;
	else if ( a_poCenter == f_poHook )f_poHook = a_poLeft;
	if ( a_poLeft == f_poSelected )f_poSelected = a_poCenter;
	else if ( a_poCenter == f_poSelected )f_poSelected = a_poLeft;
	if ( a_poLeft->f_poPrevious == a_poRight )return;
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

template < class tType >
void HList< tType >::sort_by_hits ( int a_iOrder )
	{
	M_PROLOG
	f_iOrder = a_iOrder;
	cmp = & HList< tType >::cmph ;
	sort ( );
	return ;
	M_EPILOG
	}

template < class tType >
void HList < tType >:: sort_by_number ( int a_iOrder )
	{
	M_PROLOG
	f_iOrder = a_iOrder;
	cmp = & HList< tType >::cmpn;
	sort ( );
	return ;
	M_EPILOG
	}

template < class tType >
void HList < tType >:: sort_by_contents ( int a_iOrder )
	{
	M_PROLOG
	f_iOrder = a_iOrder;
	cmp = & HList< tType >::cmpc;
	sort ( );
	return ;
	M_EPILOG
	}

#endif /* not __HLIST_H */

/*
#ifdef __DEBUG__
#include <hlog.h>
#endif / * __DEBUG__ * /
	M_LOG ( "one" );
*/


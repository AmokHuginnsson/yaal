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

#define D_ERROR									-1
#define D_TREAT_AS_CLOSED				( int * ) 1
#define D_TREAT_AS_OPENED				( int * ) 2
#define D_SEARCH_AFTER_ORDER    ( int * ) 3
#define D_SEARCH_AFTER_NUMBER   ( int * ) 4
#define D_BLOCK_IF_NOT_EMPTIED	( int * ) 5
#define D_EMPTY_IF_NOT_EMPTIED	( int * ) 6
#define D_FORCE_REMOVE_ELEMENT	( int * ) 7
#define D_UNSORTED							0
#define D_ASCENDING							8
#define D_DESCENDING						-8
#define	D_WAS_EMPTIED						9
#define	D_WAS_NOT_EMPTIED				10
#define D_FINAL_REACHED					11
#define D_NOT_FOUND							12
#define D_DEFINE_LIMIT					16

#ifndef NULL
#define NULL	0
#endif /* NULL */

#define D_CVSID_HLIST_H "$CVSHeader$"

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
	virtual tType & add_element ( tType ); /* adds new element at current cursor 
																						position */
	virtual tType & add_element ( void );
	virtual tType & add_head ( tType );    /* adds new element at beggining of 
																						the list */
	virtual tType & add_head ( void );
	virtual tType & add_tail ( tType );	/* adds new element at end of the list */
	virtual tType & add_tail ( void );
	virtual tType & add_at ( int, tType ); /* adds new element at specified 
																						position */
	virtual tType & add_at ( int );
	virtual tType & add_orderly ( tType, int = D_ASCENDING ); /* adds element in
																															 the way that
																															 keeps order */
	virtual tType * remove_element ( int * = D_BLOCK_IF_NOT_EMPTIED,
			int * = D_TREAT_AS_CLOSED );	/* rmoves element at current cursor position */
	virtual tType * remove_at ( int, int * = D_BLOCK_IF_NOT_EMPTIED,
			int * = D_TREAT_AS_CLOSED );
	virtual tType * remove_head ( int * = D_BLOCK_IF_NOT_EMPTIED );
	virtual tType * remove_tail ( int * = D_BLOCK_IF_NOT_EMPTIED );
	/* sets cursor at specified index or number */
	virtual tType & go ( int, int * = D_SEARCH_AFTER_ORDER );
	virtual tType & operator [ ] ( int );
	virtual tType & present ( void );
	virtual tType & head ( void );
	virtual tType & tail ( void );
	virtual tType & to_head ( int = 1, int * = D_TREAT_AS_CLOSED );
	virtual tType & to_tail ( int = 1, int * = D_TREAT_AS_CLOSED );
	virtual void exchange ( int, int, int * = D_SEARCH_AFTER_ORDER );
	virtual void sort_by_hits ( int = D_ASCENDING );
	virtual void sort_by_number ( int = D_ASCENDING );
	virtual void sort_by_contents ( int = D_ASCENDING );
	/*}*/
protected:
	/*{*/
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
	f_lHits++;
	f_tObject = a_tObject;
	return ;
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::HElement::get ( void )
	{
	M_PROLOG
	f_lHits++;
	return ( f_tObject );
	M_EPILOG
	}
	
//============================================================================

template < class tType >
HList< tType >::HList ( int a_iSize )
	{
	M_PROLOG
	tType l_tDummyObject;
	f_iOrder = D_UNSORTED;
	f_iIndex = 0;
	f_poIndex = NULL;
	f_iError = 0;
	f_iQuantity = 0;
	f_iHighestNumber = 0;
	f_poHook = 0;
	f_poSelected = 0;
	cmp = & HList< tType >::cmpc;
	while ( a_iSize -- )add_tail ( l_tDummyObject );
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
int compare_contents ( tType & a_tLeft, tType a_tRight )
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
tType & HList< tType >::add_element ( tType a_tObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected = 
			new HElement ( NULL, f_iHighestNumber );
	else l_poElement = new HElement ( f_poSelected, f_iHighestNumber );
	f_iQuantity ++;
	f_iHighestNumber ++;
	if ( a_tObject )l_poElement->put ( a_tObject );
	else l_poElement->get_object ( ) = a_tObject;
	f_iIndex = 0;
	f_poIndex = NULL;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_element ( void )
	{
	M_PROLOG
	tType l_tDummy;
	return ( add_element ( l_tDummy ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::add_head ( tType a_tObject )
	{
	M_PROLOG
	f_poHook = new HElement ( f_poHook, f_iHighestNumber );
	if ( f_iQuantity == 0 ) f_poSelected = f_poHook;
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_tObject )f_poHook->put ( a_tObject );
	else f_poHook->get_object ( ) = a_tObject;
	if ( f_poIndex )f_poIndex = f_poIndex->f_poPrevious;
	return ( f_poHook->f_tObject );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::add_head ( void )
	{
	M_PROLOG
	tType l_tDummy;
	return ( add_head ( l_tDummy ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::add_tail ( tType a_tObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( f_iQuantity == 0 )
		f_poHook = l_poElement = f_poSelected =
			new HElement ( NULL, f_iHighestNumber );
	else l_poElement = new HElement ( f_poHook, f_iHighestNumber );
	f_iHighestNumber ++;
	f_iQuantity ++;
	if ( a_tObject )l_poElement->put ( a_tObject );
	else l_poElement->get_object ( ) = a_tObject;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_tail ( void )
	{
	M_PROLOG
	tType l_tDummy;
	return ( add_tail ( l_tDummy ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::add_at ( int a_iIndex, tType a_tObject )
	{
	M_PROLOG
	HElement * l_poElement = NULL;
	if ( a_iIndex > f_iQuantity )
		throw new HException ( __WHERE__, "index excides list size", a_iIndex );
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
	if ( a_tObject )l_poElement->put ( a_tObject );
	else l_poElement->get_object ( ) = a_tObject;
	if ( f_poIndex &&  ( a_iIndex >= 0 ) && ( a_iIndex <= f_iIndex ) )
		f_poIndex = f_poIndex->f_poPrevious;
	return ( l_poElement->f_tObject );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_at ( int a_iIndex )
	{
	M_PROLOG
	tType l_tDummy;
	return ( add_at ( a_iIndex, l_tDummy ) );
	M_EPILOG
	}

template < class tType >
tType & HList< tType >::add_orderly ( tType a_tObject, int a_iOrder )
	{
	M_PROLOG
#define M_SWITCH ( ( this->* cmp ) ( f_poIndex, l_poElement ) * a_iOrder ) < 0
	bool l_bBefore = false;
	int l_iIndex = 0, l_iOldIndex = -1, l_iLower = 0, l_iUpper = f_iQuantity;
	HElement * l_poElement = new HElement ( NULL, f_iHighestNumber );
	l_poElement->f_tObject = a_tObject;
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
tType * HList< tType >::remove_at ( int a_iIndex, int * a_piFlag, int * a_piTreat )
	{
	M_PROLOG
	/* This one cant be initialised cause we don't know what it is,
	 * or the other approach is to force all inheritances to be able to
	 * cast from ( long int ).
	 * Why do we need this at all?
	 * We want remove_* functions to return removed element object at the same
	 * time as error code, hmm we could do this by reference rather then
	 * with simple return value in which case library user does know if
	 * there was really error or Object was really -1 :(
	 * So thats it, I ( Amok ) decided that remove_* functions will return
	 * error code by reference. */
	int l_iFlag, * l_piFlag = & l_iFlag;
	int l_iTreat, * l_piTreat = & l_iTreat;
	tType * l_ptObject = NULL;
	HElement * l_poElement = NULL;
	/* watch out, this is sticky ! */
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )l_piFlag = a_piFlag;
	else * l_piFlag = ( int ) a_piFlag;
	if ( ( unsigned int ) a_piTreat > D_DEFINE_LIMIT )l_piTreat = a_piTreat;
	else * l_piTreat = ( int ) a_piTreat;
	l_poElement = f_poHook;
	if ( f_iQuantity > 0 )
		{
		if ( a_iIndex >= f_iQuantity )
			{
			f_iError++;
			throw new HException ( __WHERE__, "index excides list size", a_iIndex );
			}
		l_poElement = element_by_index ( a_iIndex );
		if ( l_poElement->f_lHits )
			{
			switch ( * l_piFlag )
				{
				case ( ( int ) D_BLOCK_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_ERROR;
					return ( & l_poElement->f_tObject );
					break;
					}
				case ( ( int ) D_EMPTY_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( ( int ) D_FORCE_REMOVE_ELEMENT ):
					{
					l_ptObject = & l_poElement->f_tObject;
					* l_piFlag = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					throw new HException ( __WHERE__, "unknown flag", * l_piFlag );
					break;
					}
				}
			}
		}
	else throw new HException ( __WHERE__, "list was empty", g_iErrNo );
	if ( l_poElement == f_poHook )f_poHook = f_poHook->f_poNext;
	if ( l_poElement == f_poSelected )
		{
		if ( ( ( * l_piTreat ) == ( ( int ) D_TREAT_AS_OPENED ) )
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
	return ( l_ptObject );
	M_EPILOG
	}

template < class tType >
tType * HList< tType >::remove_element ( int * a_piFlag, int * a_piTreat )
	{
	M_PROLOG
	int l_iFlag, * l_piFlag = & l_iFlag;
	int l_iTreat, * l_piTreat = & l_iTreat;
	tType * l_ptObject = NULL;
	HElement * l_poElement = NULL;
	/* watch out, this is sticky ! */
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )l_piFlag = a_piFlag;
	else * l_piFlag = ( int ) a_piFlag;
	if ( ( unsigned int ) a_piTreat > D_DEFINE_LIMIT )l_piTreat = a_piTreat;
	else * l_piTreat = ( int ) a_piTreat;
	l_poElement = f_poSelected;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( * l_piFlag )
				{
				case ( ( int ) D_BLOCK_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_ERROR;
					return ( & l_poElement->f_tObject );
					break;
					}
				case ( ( int ) D_EMPTY_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( ( int ) D_FORCE_REMOVE_ELEMENT ):
					{
					l_ptObject = & l_poElement->f_tObject;
					* l_piFlag = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					throw new HException ( __WHERE__, "unknown flag", * l_piFlag );
					break;
					}
				}
			}
		}
	else throw new HException ( __WHERE__, "list was empty", g_iErrNo );
	if ( ( ( * l_piTreat ) == ( ( int ) D_TREAT_AS_OPENED ) )
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
	return ( l_ptObject );
	M_EPILOG
	}

template < class tType >
tType * HList< tType >::remove_head ( int * a_piFlag )
	{
	M_PROLOG
	int l_iFlag, * l_piFlag = & l_iFlag;
	tType * l_ptObject = NULL;
	HElement* l_poElement;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	l_poElement = f_poHook;
	f_poHook = f_poHook->f_poNext;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( * l_piFlag )
				{
				case ( ( int ) D_BLOCK_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_ERROR;
					return ( & l_poElement->f_tObject );
					break;
					}
				case ( ( int ) D_EMPTY_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( ( int ) D_FORCE_REMOVE_ELEMENT ):
					{
					l_ptObject = & l_poElement->f_tObject;
					* l_piFlag = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					throw new HException ( __WHERE__, "unknown flag", * l_piFlag );
					break;
					}
				}
			}
		}
	else throw new HException ( __WHERE__, "list was empty", g_iErrNo );
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
	return ( l_ptObject );
	M_EPILOG
	}
	
template < class tType >
tType * HList< tType >::remove_tail ( int * a_piFlag )
	{
	M_PROLOG
	int l_iFlag, * l_piFlag = & l_iFlag;
	tType * l_ptObject = NULL;
	HElement* l_poElement;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	l_poElement = f_poHook->f_poPrevious;
	if ( f_iQuantity > 0 )
		{
		if ( l_poElement->f_lHits )
			{
			switch ( * l_piFlag )
				{
				case ( ( int ) D_BLOCK_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_ERROR;
					return ( & l_poElement->f_tObject );
					break;
					}
				case ( ( int ) D_EMPTY_IF_NOT_EMPTIED ):
					{
					* l_piFlag = D_WAS_EMPTIED;
					empty ( l_poElement );
					break;
					}
				case ( ( int ) D_FORCE_REMOVE_ELEMENT ):
					{
					l_ptObject = & l_poElement->f_tObject;
					* l_piFlag = D_WAS_NOT_EMPTIED;
					break;
					}
				default :
					{
					throw new HException ( __WHERE__, "unknown flag.", * l_piFlag );
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
	else throw new HException ( __WHERE__, "list was empty.", g_iErrNo );
	return ( l_ptObject );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::to_head ( int a_iOffset, int * a_piFlag )
	{
	M_PROLOG
	int l_iCtr = 0, l_iFlag = 0, * l_piFlag = & l_iFlag;
	HElement * l_poElement = NULL;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	if ( !f_iQuantity )
		throw new HException ( __WHERE__, "list is empty.", g_iErrNo );
	if ( a_iOffset < 1 )
		throw new HException ( __WHERE__, "incorrect offset.", a_iOffset );
	l_poElement = f_poSelected;
	switch ( * l_piFlag )
		{
		case ( ( int ) D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				f_poSelected = f_poSelected->f_poPrevious;
			break;
			}
		case ( ( int ) D_TREAT_AS_OPENED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				{
				if ( f_poSelected == f_poHook )
					{
					* l_piFlag = D_FINAL_REACHED;
					break;
					}
				f_poSelected = f_poSelected->f_poPrevious;
				}
			break;
			}
		default :
			{
			throw new HException ( __WHERE__,
					"HList< tType >::to_head ( ), unknown flag.", * l_piFlag );
			break;
			}
		}
	return ( l_poElement->get ( ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::to_tail ( int a_iOffset, int * a_piFlag )
	{
	M_PROLOG
	int l_iCtr = 0, l_iFlag = 0, * l_piFlag = & l_iFlag;
	HElement * l_poElement = NULL;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	if ( !f_iQuantity )
		throw new HException ( __WHERE__, "list is empty.", g_iErrNo );
	if ( a_iOffset < 1 )
		throw new HException ( __WHERE__, "incorrect offset.", a_iOffset );
	l_poElement = f_poSelected;
	switch ( * l_piFlag )
		{
		case ( ( int ) D_TREAT_AS_CLOSED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				f_poSelected = f_poSelected->f_poNext;
			break;
			}
		case ( ( int ) D_TREAT_AS_OPENED ):
			{
			for ( l_iCtr = 0; l_iCtr < a_iOffset; l_iCtr ++ )
				{
				if ( f_poSelected->f_poNext == f_poHook )
					{
					* l_piFlag = D_FINAL_REACHED;
					break;
					}
				f_poSelected = f_poSelected->f_poNext;
				}
			break;
			}
		default :
			{
			throw new HException ( __WHERE__,
					"HList< tType >::to_tail ( ), unknown flag.", * l_piFlag );
			break;
			}
		}
	return ( l_poElement->get ( ) );
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
		throw new HException ( __WHERE__, "list is empty", g_iErrNo );
		}
	if ( a_iIndex < 0 )a_iIndex += f_iQuantity;
	if ( ( a_iIndex >= f_iQuantity ) || ( a_iIndex < 0 ) )
		{
		f_iError ++;
		throw new HException ( __WHERE__, "bad index", a_iIndex );
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
		throw new HException ( __WHERE__, "list is empty", g_iErrNo );
		}
	l_poElement = f_poHook;
	for ( l_iCtr = 0; l_iCtr < f_iQuantity; l_iCtr++ )
		{
		if ( l_poElement->f_iNumber == a_iNumber ) return ( l_poElement );
		l_poElement = l_poElement->f_poNext;
		}
	throw new HException ( __WHERE__, "wrong number", a_iNumber );
	return ( NULL );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::go ( int a_iNumber, int * a_piFlag )
	{
	M_PROLOG
	/* Here we have another function with error code returned by reference */
	int l_iFlag, * l_piFlag = & l_iFlag;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	switch ( * l_piFlag )
		{
		case ( ( int ) D_SEARCH_AFTER_NUMBER ):
			{
			f_poSelected = element_by_number ( a_iNumber );
			break;
			}
		case ( ( int ) D_SEARCH_AFTER_ORDER ):
			{
			f_poSelected = element_by_index ( a_iNumber );
			break;
			}
		default :
			{
			throw new HException ( __WHERE__, "unknown flag", * l_piFlag );
			break;
			}
		}
	return ( f_poSelected->get ( ) );
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
void HList< tType >::exchange ( int a_iLeft, int a_iRight, int * a_piFlag )
	{
	M_PROLOG
	int l_iFlag, * l_piFlag = & l_iFlag;
	int l_iCtr = 0;
	int l_iOther = 0;
	HElement * l_poLeft, * l_poRight;
	if ( a_iLeft == a_iRight )return;
	if ( ( unsigned int ) a_piFlag > D_DEFINE_LIMIT )
		l_piFlag = a_piFlag;							/* watch out ! */
	else * l_piFlag = ( int ) a_piFlag;	/* this is sticky ! */
	switch ( * l_piFlag )
		{
		case ( ( int ) D_SEARCH_AFTER_NUMBER ):
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
		case ( ( int ) D_SEARCH_AFTER_ORDER ):
			{
			l_poLeft = element_by_index ( a_iLeft );
			l_poRight = element_by_index ( a_iRight );
			break;
			}
		default :
			{
			throw new HException ( __WHERE__, "unknown flag", * l_piFlag );
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
		throw new HException ( __WHERE__, "list is empty", g_iErrNo );
	return ( f_poSelected->get ( ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::head ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		throw new HException ( __WHERE__, "list is empty", g_iErrNo );
	return ( f_poHook->get ( ) );
	M_EPILOG
	}
	
template < class tType >
tType & HList< tType >::tail ( void )
	{
	M_PROLOG
	if ( f_poHook == 0 )
		throw new HException ( __WHERE__, "list is empty", g_iErrNo );
	return ( f_poHook->f_poPrevious->get ( ) );
	M_EPILOG
	}
	
template < class tType >
void HList< tType >::sort ( void )
	{
	M_PROLOG
	int l_iCtr = f_iQuantity;
	int l_iCtrLoc = 0;
	HElement * l_poBase = f_poHook;
	HElement * l_poExtream = NULL;
	HElement * l_poPointer = NULL;
	while ( l_iCtr -- )
		{
		l_iCtrLoc = l_iCtr;
		l_poExtream = l_poBase;
		l_poPointer = l_poExtream->f_poNext;
		while ( l_iCtrLoc -- )
			{
			if ( ( ( this->* cmp ) ( l_poExtream, l_poPointer ) * f_iOrder ) > 0 )
				l_poExtream = l_poPointer;
			l_poPointer = l_poPointer->f_poNext;
			}
		if ( l_poBase != l_poExtream )
			{
			exchange ( l_poBase, l_poExtream );
			l_poBase = l_poExtream;
			}
		l_poBase = l_poBase->f_poNext;
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
	/ * log from HLog conflicts with log from math.h in analyser.c:) *
#ifndef _MATH_H
#include <hlog.h>
#endif / * not _MATH_H * /
#endif / * __DEBUG__ * /

	/ * log from HLog conflicts with log from math.h in analyser.c:) * /
#ifndef _MATH_H
	M_LOG ( "one" );
#endif / * not _MATH_H * /
*/

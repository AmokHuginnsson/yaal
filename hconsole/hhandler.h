/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.h - this file is integral part of `stdhapi' project.

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

#ifndef __HHANDLER_H
#define __HHANDLER_H

#line 31 "hhandler.h"

#include <stdlib.h>

template < class tType >
class HHandler
	{
protected:
	/*{*/
	int		f_iCodeCount;	/* number of event codes */
	int *	f_piCodes;			/* event codes for that causes handler to run */
	tType	f_tHandler;		/* caller itself */
	/*}*/
public:
	/*{*/
	HHandler ( void );
	HHandler ( int, int *, tType );
	HHandler ( const HHandler &, int = 0 );
	virtual ~HHandler ( void );
	HHandler & operator = ( const HHandler & );
	int operator [ ] ( int );
	operator const tType ( void );
	void add ( int, int * = NULL );
	operator bool ( void );
	/*}*/
	};

#include "./hcore/hexception.h"
#include "./hcore/xalloc.h"

template < class tType >
HHandler < tType >::HHandler ( void )
	{
	M_PROLOG
	f_piCodes = NULL;
	f_iCodeCount = 0;
	f_tHandler = NULL;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::HHandler ( const HHandler & a_roHandler, int )
	{
	M_PROLOG
	f_piCodes = NULL;
	( * this ) = a_roHandler;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::HHandler ( int a_iCodeCount, int * a_piCodes, tType a_tHandler )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_piCodes = NULL;
	f_iCodeCount = a_iCodeCount + ( a_piCodes ? 1 : 0 ); /* 1 is for terminating 0 */
	if ( a_piCodes )
		{
		f_piCodes = ( int * ) xcalloc ( f_iCodeCount * sizeof ( int ) );
		for ( l_iCtr = 0; l_iCtr < a_iCodeCount; l_iCtr ++ )
			f_piCodes [ l_iCtr ] = a_piCodes [ l_iCtr ];
		f_piCodes [ a_iCodeCount ] = 0;
		}
	f_tHandler= a_tHandler;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::~HHandler ( void )
	{
	M_PROLOG
	if ( f_piCodes )xfree ( ( void * ) f_piCodes );
	f_piCodes = NULL;
	return;
	M_EPILOG
	}

template < class tType >
HHandler<tType> & HHandler <tType>::operator = ( const HHandler & a_roHandler )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_iCodeCount = a_roHandler.f_iCodeCount;
	if ( a_roHandler.f_piCodes )
		{
		f_piCodes = ( int * ) xrealloc ( f_piCodes, f_iCodeCount * sizeof ( int ) );
		for ( l_iCtr = 0; l_iCtr < f_iCodeCount; l_iCtr ++ )
			f_piCodes [ l_iCtr ] = a_roHandler.f_piCodes [ l_iCtr ];
		}
	f_tHandler = a_roHandler.f_tHandler;
	return ( * this );
	M_EPILOG
	}

template < class tType >
int HHandler < tType >::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	if ( f_piCodes && ( a_iIndex >= 0 ) && ( a_iIndex < f_iCodeCount ) )
		return ( f_piCodes [ a_iIndex ] );
	if ( a_iIndex == 0 )return ( f_iCodeCount );
	return ( 0 );
	M_EPILOG
	}

template < class tType >
HHandler < tType >::operator const tType ( void )
	{
	M_PROLOG
	return ( f_tHandler );
	M_EPILOG
	}

template < class tType >
void HHandler < tType >::add ( int a_iCodeCount, int * a_piCodes )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCodeCount = 0;
	int * l_piCodes = NULL;
	if ( f_piCodes && a_piCodes )
		{
		l_iCodeCount = f_iCodeCount + a_iCodeCount;
		f_iCodeCount -= 2;
		}
	else if ( ! ( f_piCodes || a_piCodes ) )
		l_iCodeCount = 2 + ( f_iCodeCount > 0 ? 1 : 0 );
	else if ( f_piCodes && ! a_piCodes )
		l_iCodeCount = f_iCodeCount + 1;
	else if ( ! f_piCodes && a_piCodes )l_iCodeCount = a_iCodeCount + 2;
	l_piCodes = f_piCodes;
	f_piCodes = ( int * ) xrealloc ( f_piCodes, l_iCodeCount * sizeof ( int ) );
	if ( ! l_piCodes )
		{
		f_piCodes [ 0 ] = f_iCodeCount;
		f_iCodeCount = 0;
		}
	if ( a_piCodes )
		for ( l_iCtr = f_iCodeCount; l_iCtr < ( l_iCodeCount - 1 ); l_iCtr ++ )
			f_piCodes [ l_iCtr ] = a_piCodes [ l_iCtr - f_iCodeCount ];
	else f_piCodes [ l_iCodeCount - 2 ] = a_iCodeCount;
	f_iCodeCount = l_iCodeCount;
	f_piCodes [ f_iCodeCount - 1 ] = 0;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::operator bool ( void )
	{
	M_PROLOG
	return ( f_iCodeCount && f_piCodes && f_tHandler );
	M_EPILOG
	}

#endif /* not __HHANDLER_H */


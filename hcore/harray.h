/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	harray.h - this file is integral part of `stdhapi' project.

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

#ifndef __HARRAY_H
#define __HARRAY_H

#line 31

#include "hexception.h"

template < class tType >
class HArray
	{
protected:
	/*{*/
	int f_iSize;
	tType * f_ptArray;
	/*}*/
public:
	/*{*/
	HArray ( int );
	virtual ~HArray ( void );
	HArray ( const HArray &, int = 0 );
	HArray & operator = ( const HArray & );
	tType & operator [ ] ( int );
	int get_size ( void );
	operator bool ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

template < class tType >
HArray < tType >::HArray ( int a_iSize )
	{
	M_PROLOG
	f_iSize = 0;
	f_ptArray = NULL;
	if ( a_iSize < 0 )
		throw new HException ( __WHERE__, "bad size", a_iSize );
	f_iSize = a_iSize;
	if ( a_iSize )
		{
		f_ptArray = new tType [ f_iSize ];
		if ( ! f_ptArray )
			throw new HException ( __WHERE__, "can alloc memory with new", a_iSize );
		}
	return;
	M_EPILOG
	}

template < class tType >
HArray < tType >::~HArray ( void )
	{
	M_PROLOG
	if ( f_ptArray )delete [ ] f_ptArray;
	f_iSize = 0;
	f_ptArray = NULL;
	return;
	M_EPILOG
	}

template < class tType >
HArray < tType >::HArray ( const HArray & a_roArray, int )
	{
	M_PROLOG
	f_iSize = 0;
	f_ptArray = NULL;
	( * this ) = a_roArray;
	return;
	M_EPILOG
	}

template < class tType >
HArray < tType > & HArray < tType >::operator = ( const HArray & a_roArray )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_roArray.f_iSize != f_iSize )
		{
		if ( f_ptArray )
			{
			delete [ ] f_ptArray;
			f_ptArray = NULL;
			f_iSize = 0;
			}
		f_iSize = a_roArray.f_iSize;
		}
	if ( f_iSize )
		{
		f_ptArray = new tType [ f_iSize ];
		if ( ! f_ptArray )
			throw new HException ( __WHERE__, "can alloc memory with new", f_iSize );
		}
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] = a_roArray.f_ptArray [ l_iCtr ];
	return ( * this );
	M_EPILOG
	}

template < class tType >
tType & HArray < tType >::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	if ( a_iIndex < 0 )a_iIndex += f_iSize;
	if ( ( a_iIndex >= f_iSize ) || ( a_iIndex < 0 ) )
		throw new HException ( __WHERE__, "index excides array size", a_iIndex );
	return ( f_ptArray [ a_iIndex ] );
	M_EPILOG
	}

template < class tType >
HArray < tType > ::operator bool ( void )
	{
	M_PROLOG
	return ( f_iSize ? true : false );
	M_EPILOG
	}

template < class tType >
int HArray < tType > ::get_size ( void )
	{
	M_PROLOG
	return ( f_iSize );
	M_EPILOG
	}

#endif /* __HARRAY_H */

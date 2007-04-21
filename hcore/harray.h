/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	harray.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HARRAY_H
#define __YAAL_HCORE_HARRAY_H

#line 31

#define D_VCSID_HARRAY_H "$Id$"

#include <new>

#include "hcore/hexception.h"

namespace yaal
{

namespace hcore
{

extern char const * const n_ppcErrMsgHArray [ ];

template < typename tType >
class HArray
	{
	struct OPointerConversion { int f_iValid; };
	typedef int OPointerConversion::* opcm_t;
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_BADSIZE,
			E_NOMEM,
			E_BADINDEX
			} error_t;
		};
protected:
	int f_iSize;
	tType* f_ptArray;
public:
	HArray( int );
	HArray( int const&, tType const& );
	virtual ~HArray( void );
	HArray( HArray const& );
	HArray& operator = ( HArray const& );
	tType& operator [] ( int );
	tType const& operator [] ( int ) const;
	int get_size( void ) const;
	operator opcm_t ( void ) const;
private:
	tType& get( int ) const;
	};

template<typename tType>
HArray<tType>::HArray( int a_iSize ) : f_iSize( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	if ( a_iSize < 0 )
		M_THROW ( n_ppcErrMsgHArray[ ERROR::E_BADSIZE ], a_iSize );
	f_iSize = a_iSize;
	if ( a_iSize )
		{
		f_ptArray = new ( std::nothrow ) tType[ f_iSize ];
		if ( ! f_ptArray )
			M_THROW( n_ppcErrMsgHArray[ ERROR::E_NOMEM ], a_iSize );
		}
	return;
	M_EPILOG
	}

template<typename tType>
HArray<tType>::HArray( int const& a_iSize, tType const& a_tFillWith )
	: f_iSize( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_iSize < 0 )
		M_THROW( n_ppcErrMsgHArray[ ERROR::E_BADSIZE ], a_iSize );
	f_iSize = a_iSize;
	if ( a_iSize )
		{
		f_ptArray = new ( std::nothrow ) tType [ f_iSize ];
		if ( ! f_ptArray )
			M_THROW ( n_ppcErrMsgHArray[ ERROR::E_NOMEM ], a_iSize );
		for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
			f_ptArray[ l_iCtr ] = a_tFillWith;
		}
	return;
	M_EPILOG
	}

template<typename tType>
HArray<tType>::~HArray( void )
	{
	M_PROLOG
	if ( f_ptArray )
		delete [] f_ptArray;
	f_iSize = 0;
	f_ptArray = NULL;
	return;
	M_EPILOG
	}

template<typename tType>
HArray<tType>::HArray( HArray const& a_roArray ) : f_iSize( 0 ), f_ptArray( NULL )
	{
	M_PROLOG
	( *this ) = a_roArray;
	return;
	M_EPILOG
	}

template<typename tType>
HArray<tType>& HArray<tType>::operator = ( HArray const& a_roArray )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( this != & a_roArray )
		{
		if ( a_roArray.f_iSize != f_iSize )
			{
			if ( f_ptArray )
				{
				delete [] f_ptArray;
				f_ptArray = NULL;
				f_iSize = 0;
				}
			f_iSize = a_roArray.f_iSize;
			}
		if ( f_iSize && ! f_ptArray )
			{
			f_ptArray = new ( std::nothrow ) tType [ f_iSize ];
			if ( ! f_ptArray )
				M_THROW( n_ppcErrMsgHArray[ ERROR::E_NOMEM ], f_iSize );
			}
		for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
			f_ptArray[ l_iCtr ] = a_roArray.f_ptArray[ l_iCtr ];
		}
	return ( *this );
	M_EPILOG
	}

template<typename tType>
tType& HArray<tType>::get( int a_iIndex ) const
	{
	M_PROLOG
	if ( a_iIndex < 0 )
		a_iIndex += f_iSize;
	if ( ( a_iIndex >= f_iSize ) || ( a_iIndex < 0 ) )
		M_THROW( n_ppcErrMsgHArray[ ERROR::E_BADINDEX ], a_iIndex );
	return ( f_ptArray[ a_iIndex ] );
	M_EPILOG
	}

template<typename tType>
tType& HArray<tType>::operator[] ( int a_iIndex )
	{
	M_PROLOG
	return ( get( a_iIndex ) );
	M_EPILOG
	}

template<typename tType>
tType const& HArray<tType>::operator[] ( int a_iIndex ) const
	{
	M_PROLOG
	return ( get( a_iIndex ) );
	M_EPILOG
	}

template<typename tType>
HArray<tType> ::operator opcm_t ( void ) const
	{
	M_PROLOG
	return ( f_iSize ? &OPointerConversion::f_iValid : NULL );
	M_EPILOG
	}

template<typename tType>
int HArray<tType>::get_size( void ) const
	{
	M_PROLOG
	return ( f_iSize );
	M_EPILOG
	}

}

}

#endif /* __YAAL_HCORE_HARRAY_H */


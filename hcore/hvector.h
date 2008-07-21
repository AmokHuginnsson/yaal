/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hvector.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HVECTOR_H
#define __YAAL_HCORE_HVECTOR_H

#line 31

#include <cmath>

#include "hcore/harray.h"
#include "hcore/hexception.h"

namespace yaal
{

namespace hcore
{

#define D_VCSID_HVECTOR_H "$Id$"


extern char const* g_ppcErrMsgHVector[];

template<typename tType>
class HVector : public HArray<tType>
	{
protected:
	typedef HVector<tType> self_t;
	typedef HArray<tType> hier_t;
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_DIMNOTMATCH
			} error_t;
		};
	HVector ( int = 0 );
	HVector ( HVector const & );
	virtual ~HVector ( void );
	int dim ( void );
	int set ( tType const * );
	int normalize ( void );
	tType norm ( void ) const;
	HVector& operator = ( HVector const& );
	HVector& operator = ( tType const );
	HVector operator + ( HVector const& ) const;
	HVector operator - ( HVector const& ) const;
	HVector operator - ( void ) const;
	HVector operator * ( tType const ) const;
	HVector operator / ( tType const ) const;
	HVector& operator += ( HVector const& );
	HVector& operator -= ( HVector const& );
	HVector& operator *= ( tType const );
	HVector& operator /= ( tType const );
	tType operator | ( HVector const& ) const;
	tType operator ! ( void ) const;
	bool operator == ( HVector const& ) const;
	bool operator != ( HVector const& ) const;
	template < typename ttType >
	friend HVector<ttType> operator * ( ttType const,
			HVector<ttType> const& );
private:
	inline void check_dimensions ( int a_iSizeAnother ) const
		{
		M_PROLOG
		if ( this->f_iSize != a_iSizeAnother )
			M_THROW ( g_ppcErrMsgHVector [ ERROR::E_DIMNOTMATCH ],
					this->f_iSize - a_iSizeAnother );
		return;
		M_EPILOG
		}
	};

template < typename tType >
HVector < tType > ::HVector ( int const a_iDimension )
				: HArray < tType > ( a_iDimension )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] = 0;
	return ;
	M_EPILOG
	}

template < typename tType >
HVector < tType > ::~HVector ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < typename tType >
HVector < tType > ::HVector ( HVector const & a_roVector )
				: HArray < tType > ( a_roVector )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < typename tType >
int HVector < tType > ::set ( tType const * a_pdBody )
	{
	M_PROLOG
	for ( int l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] = a_pdBody [ l_iCtr ];
	return ( 0 );
	M_EPILOG
	}

template < typename tType >
tType HVector < tType > ::norm ( void ) const
	{
	M_PROLOG
	int l_iCtr;
	tType l_tScalar = 0;
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		l_tScalar += ( this->f_ptArray [ l_iCtr ] * this->f_ptArray [ l_iCtr ] );
	return ( sqrt ( l_tScalar ) );
	M_EPILOG
	}

template < typename tType >
int HVector < tType > ::normalize ( void )
	{
	M_PROLOG
	int l_iCtr;
	tType l_tLenght;
	l_tLenght = norm();
	if ( ! l_tLenght )
		return ( 1 );
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] /= l_tLenght;
	return ( 0 );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & 
HVector < tType > ::operator = ( HVector const & a_roVector )
	{
	M_PROLOG
	if ( this->f_iSize )
		check_dimensions ( a_roVector.f_iSize );
	( * this ).HArray < tType > ::operator = ( a_roVector );
	return ( * this );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & HVector < tType > ::operator = ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] = a_tScalar;
	return ( *this );
	M_EPILOG
	}

template < typename tType >
HVector < tType > HVector < tType > ::operator + ( HVector const & a_roVector ) const
	{
	M_PROLOG
	check_dimensions ( a_roVector.f_iSize );
	HVector l_oVector ( * this );
	l_oVector += a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template < typename tType >
HVector < tType > HVector < tType > ::operator - ( HVector const & a_roVector ) const
	{
	M_PROLOG
	check_dimensions ( a_roVector.f_iSize );
	HVector l_oVector ( * this );
	l_oVector -= a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template < typename tType >
HVector < tType > HVector < tType > ::operator - ( void ) const
	{
	M_PROLOG
	HVector l_oVector ( this->f_iSize );
	l_oVector = 0;
	l_oVector -= ( * this );
	return ( l_oVector );
	M_EPILOG
	}

template < typename tType >
HVector < tType > HVector < tType > ::operator * ( tType const a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector ( * this );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template < typename tType >
HVector < tType > HVector < tType > ::operator / ( tType const a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector ( * this );
	l_oVector /= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & HVector < tType > ::operator += ( HVector const & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	check_dimensions ( a_roVector.f_iSize );
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ ) 
		this->f_ptArray [ l_iCtr ] += a_roVector.f_ptArray [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & HVector < tType > ::operator -= ( HVector const & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	check_dimensions ( a_roVector.f_iSize );
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] -= a_roVector.f_ptArray [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & HVector < tType > ::operator *= ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] *= a_tScalar;
	return ( *this );
	M_EPILOG
	}

template < typename tType >
HVector < tType > & HVector < tType > ::operator /= ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_tScalar )
		for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ )
			this->f_ptArray [ l_iCtr ] /= a_tScalar;
	return ( *this );
	M_EPILOG
	}

template < typename tType >
tType HVector < tType > ::operator | ( HVector const & a_roVector ) const
	{
	M_PROLOG
	tType l_tScalar = 0;
	int l_iCtr = 0;
	check_dimensions ( a_roVector.f_iSize );
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ ) 
		l_tScalar += ( this->f_ptArray [ l_iCtr ] * a_roVector.f_ptArray [ l_iCtr ] );
	return ( l_tScalar );
	M_EPILOG
	}

template < typename tType >
tType HVector < tType > ::operator ! ( void ) const
	{
	M_PROLOG
	return ( norm() );
	M_EPILOG
	}

template < typename tType >
bool HVector < tType > ::operator == ( HVector const & a_roVector ) const
	{
	M_PROLOG
	int l_iCtr = 0;
	check_dimensions ( a_roVector.f_iSize );
	for ( l_iCtr = 0; l_iCtr < this->f_iSize; l_iCtr ++ ) 
		if ( this->f_ptArray [ l_iCtr ] != a_roVector.f_ptArray [ l_iCtr ] )
			return ( false );
	return ( true );
	M_EPILOG
	}

template < typename tType >
bool HVector < tType > ::operator != ( HVector const & a_roVector ) const
	{
	M_PROLOG
	return ( ! ( * this == a_roVector ) );
	M_EPILOG
	}

template < typename tType >
HVector < tType > operator * ( tType const a_tScalar,
		HVector < tType > const & a_roVector )
	{
	M_PROLOG
	HVector < tType > l_oVector ( a_roVector );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

}

}

#endif /* not __YAAL_HCORE_HVECTOR_H */


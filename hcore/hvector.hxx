/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hvector.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HVECTOR_HXX_INCLUDED
#define YAAL_HCORE_HVECTOR_HXX_INCLUDED

#line 31

#include <cmath>

#include "hcore/harray.hxx"
#include "hcore/hexception.hxx"

namespace yaal
{

namespace hcore
{

#define D_VCSID_HVECTOR_H "$Id$"

extern char const* g_ppcErrMsgHVector[];

/*! \brief Vector class for linear algebra calculus.
 */
template<typename tType>
class HVector
	{
	typedef HArray<tType> data_t;
	typedef HVector<tType> self_t;
	data_t f_oData;
public:
	/*! \brief Error type of HVector<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error type flags.
		 */
		typedef enum
			{
			E_OK = 0, /*!< no error. */
			E_DIMNOTMATCH /*!< binary operation applied to two vectors with different dimensions. */
			} error_t;
		};
	typedef typename data_t::iterator iterator;
	typedef typename data_t::const_iterator const_iterator;
public:
	HVector( int long const& = 0 );
	HVector( HVector const& );
	~HVector( void );
	void set( data_t const& );
	bool normalize( void );
	int long dim( void ) const;
	tType norm( void ) const;
	HVector& operator = ( HVector const& );
	HVector& operator = ( tType const& );
	HVector operator + ( HVector const& ) const;
	HVector operator - ( HVector const& ) const;
	HVector operator - ( void ) const;
	HVector operator * ( tType const& ) const;
	HVector operator / ( tType const& ) const;
	HVector& operator += ( HVector const& );
	HVector& operator -= ( HVector const& );
	HVector& operator *= ( tType const& );
	HVector& operator /= ( tType const& );
	tType operator | ( HVector const& ) const;
	tType operator ! ( void ) const;
	tType const& operator[] ( int long const& ) const;
	tType& operator[] ( int long const& );
	bool operator == ( HVector const& ) const;
	bool operator != ( HVector const& ) const;
	iterator begin( void );
	const_iterator begin( void ) const;
	iterator end( void );
	const_iterator end( void ) const;
private:
	inline void check_dimensions( int a_iSizeAnother ) const
		{
		M_PROLOG
		if ( f_oData.size() != a_iSizeAnother )
			M_THROW( g_ppcErrMsgHVector[ ERROR::E_DIMNOTMATCH ],
					f_oData.size() - a_iSizeAnother );
		return;
		M_EPILOG
		}
	};

template<typename tType>
HVector<tType>::HVector( int long const& a_lDimension ) : f_oData( a_lDimension, 0.0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
HVector<tType>::~HVector( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
HVector<tType>::HVector( HVector const& a_roVector ) : f_oData( a_roVector.f_oData )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType>
void HVector<tType>::set( data_t const& rawData )
	{
	M_PROLOG
	f_oData = rawData;
	M_EPILOG
	}

template<typename tType>
tType HVector<tType>::norm( void ) const
	{
	M_PROLOG
	tType l_tScalar = accumulate( f_oData.begin(), f_oData.end(), 0.0 );
	return ( sqrt( l_tScalar ) );
	M_EPILOG
	}

template<typename tType>
bool HVector<tType>::normalize( void )
	{
	M_PROLOG
	tType l_tLenght = norm();
	if ( ! l_tLenght )
		return ( true );
	int long size = f_oData.size();
	for ( int long i = 0; i < size; ++ i )
		f_oData[ i ] /= l_tLenght;
	return ( false );
	M_EPILOG
	}

template<typename tType>
int long HVector<tType>::dim( void ) const
	{
	M_PROLOG
	return ( f_oData.size() );
	M_EPILOG
	}

template<typename tType>
typename HVector<tType>::iterator HVector<tType>::begin( void )
	{
	return ( f_oData.begin() );
	}

template<typename tType>
typename HVector<tType>::const_iterator HVector<tType>::begin( void ) const
	{
	return ( f_oData.begin() );
	}

template<typename tType>
typename HVector<tType>::iterator HVector<tType>::end( void )
	{
	return ( f_oData.end() );
	}

template<typename tType>
typename HVector<tType>::const_iterator HVector<tType>::end( void ) const
	{
	return ( f_oData.end() );
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator = ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	if ( f_oData.size() && size )
		check_dimensions( size );
	f_oData = a_roVector.f_oData;
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator = ( tType const& a_tScalar )
	{
	M_PROLOG
	fill( f_oData.begin(), f_oData.end(), a_tScalar );
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HVector<tType> HVector<tType>::operator + ( HVector const& a_roVector ) const
	{
	M_PROLOG
	check_dimensions( a_roVector.f_oData.size() );
	HVector l_oVector ( *this );
	l_oVector += a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template<typename tType>
HVector<tType> HVector<tType>::operator - ( HVector const& a_roVector ) const
	{
	M_PROLOG
	check_dimensions( a_roVector.f_oData.size() );
	HVector l_oVector( *this );
	l_oVector -= a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template<typename tType>
HVector<tType> HVector<tType>::operator - ( void ) const
	{
	M_PROLOG
	HVector l_oVector( f_oData.size() );
	l_oVector = 0;
	l_oVector -= ( *this );
	return ( l_oVector );
	M_EPILOG
	}

template<typename tType>
HVector<tType> HVector<tType>::operator * ( tType const& a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector( *this );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template<typename tType>
HVector<tType>HVector<tType>::operator / ( tType const& a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector ( * this );
	l_oVector /= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator += ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	yaal::transform( f_oData.begin(), f_oData.end(), a_roVector.f_oData.begin(), f_oData.begin(), yaal::plus<tType>() );
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator -= ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	yaal::transform( f_oData.begin(), f_oData.end(), a_roVector.f_oData.begin(), f_oData.begin(), yaal::minus<tType>() );
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator *= ( tType const& a_tScalar )
	{
	M_PROLOG
	yaal::transform( f_oData.begin(), f_oData.end(), f_oData.begin(), bind2nd( yaal::multiplies<tType>(), a_tScalar ) );
	return ( *this );
	M_EPILOG
	}

template<typename tType>
HVector<tType>& HVector<tType>::operator /= ( tType const& a_tScalar )
	{
	M_PROLOG
	int long size = f_oData.size();
	if ( a_tScalar )
		yaal::transform( f_oData.begin(), f_oData.end(), f_oData.begin(), bind2nd( yaal::divides<tType>(), a_tScalar ) );
	return ( *this );
	M_EPILOG
	}

template<typename tType>
tType HVector<tType>::operator | ( HVector const& a_roVector ) const
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	tType l_tScalar = 0;
	for ( int long i = 0; i < size; i ++ ) 
		l_tScalar += ( f_oData[ i ] * a_roVector.f_oData[ i ] );
	return ( l_tScalar );
	M_EPILOG
	}

template<typename tType>
tType HVector<tType>::operator ! ( void ) const
	{
	M_PROLOG
	return ( norm() );
	M_EPILOG
	}

template<typename tType>
tType const& HVector<tType>::operator[] ( int long const& idx ) const
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename tType>
tType& HVector<tType>::operator[] ( int long const& idx )
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename tType>
bool HVector<tType>::operator == ( HVector const& a_roVector ) const
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	for ( int long i = 0; i < size; ++ i ) 
		if ( f_oData[ i ] != a_roVector.f_oData[ i ] )
			return ( false );
	return ( true );
	M_EPILOG
	}

template<typename tType>
bool HVector<tType>::operator != ( HVector const& a_roVector ) const
	{
	M_PROLOG
	return ( ! ( *this == a_roVector ) );
	M_EPILOG
	}

template<typename tType>
HVector<tType> operator * ( tType const a_tScalar,
		HVector<tType>const& a_roVector )
	{
	M_PROLOG
	HVector<tType>l_oVector( a_roVector );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HVECTOR_HXX_INCLUDED */


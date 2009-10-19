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

#include <cmath>

#include "hcore/harray.hxx"
#include "hcore/hexception.hxx"
#include "hcore/functional.hxx"

namespace yaal
{

namespace hcore
{

extern char const* g_ppcErrMsgHVector[];

/*! \brief Vector class for linear algebra calculus.
 */
template<typename value_type>
class HVector
	{
	typedef HArray<value_type> data_t;
	typedef HVector<value_type> self_t;
	data_t f_oData;
public:
	typedef value_type value_t;
	/*! \brief Error type of HVector<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error type flags.
		 */
		typedef enum
			{
			OK = 0, /*!< no error. */
			DIMNOTMATCH /*!< binary operation applied to two vectors with different dimensions. */
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
	value_type norm( void ) const;
	HVector& operator = ( HVector const& );
	HVector& operator = ( value_type const& );
	HVector operator + ( HVector const& ) const;
	HVector operator - ( HVector const& ) const;
	HVector operator - ( void ) const;
	HVector operator * ( value_type const& ) const;
	HVector operator / ( value_type const& ) const;
	HVector& operator += ( HVector const& );
	HVector& operator -= ( HVector const& );
	HVector& operator *= ( value_type const& );
	HVector& operator /= ( value_type const& );
	value_type operator | ( HVector const& ) const;
	value_type operator ! ( void ) const;
	value_type const& operator[] ( int long const& ) const;
	value_type& operator[] ( int long const& );
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
			M_THROW( g_ppcErrMsgHVector[ ERROR::DIMNOTMATCH ],
					f_oData.size() - a_iSizeAnother );
		return;
		M_EPILOG
		}
	};

template<typename value_type>
HVector<value_type>::HVector( int long const& a_lDimension ) : f_oData( a_lDimension, 0.0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>::~HVector( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>::HVector( HVector const& a_roVector ) : f_oData( a_roVector.f_oData )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_type>
void HVector<value_type>::set( data_t const& rawData )
	{
	M_PROLOG
	f_oData = rawData;
	M_EPILOG
	}

template<typename value_type>
value_type HVector<value_type>::norm( void ) const
	{
	M_PROLOG
	value_type l_tScalar = accumulate( f_oData.begin(), f_oData.end(), 0.0 );
	return ( sqrt( l_tScalar ) );
	M_EPILOG
	}

template<typename value_type>
bool HVector<value_type>::normalize( void )
	{
	M_PROLOG
	value_type l_tLenght = norm();
	if ( ! l_tLenght )
		return ( true );
	int long size = f_oData.size();
	for ( int long i = 0; i < size; ++ i )
		f_oData[ i ] /= l_tLenght;
	return ( false );
	M_EPILOG
	}

template<typename value_type>
int long HVector<value_type>::dim( void ) const
	{
	M_PROLOG
	return ( f_oData.size() );
	M_EPILOG
	}

template<typename value_type>
typename HVector<value_type>::iterator HVector<value_type>::begin( void )
	{
	return ( f_oData.begin() );
	}

template<typename value_type>
typename HVector<value_type>::const_iterator HVector<value_type>::begin( void ) const
	{
	return ( f_oData.begin() );
	}

template<typename value_type>
typename HVector<value_type>::iterator HVector<value_type>::end( void )
	{
	return ( f_oData.end() );
	}

template<typename value_type>
typename HVector<value_type>::const_iterator HVector<value_type>::end( void ) const
	{
	return ( f_oData.end() );
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator = ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	if ( f_oData.size() && size )
		check_dimensions( size );
	f_oData = a_roVector.f_oData;
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator = ( value_type const& a_tScalar )
	{
	M_PROLOG
	fill( f_oData.begin(), f_oData.end(), a_tScalar );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator + ( HVector const& a_roVector ) const
	{
	M_PROLOG
	check_dimensions( a_roVector.f_oData.size() );
	HVector l_oVector ( *this );
	l_oVector += a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator - ( HVector const& a_roVector ) const
	{
	M_PROLOG
	check_dimensions( a_roVector.f_oData.size() );
	HVector l_oVector( *this );
	l_oVector -= a_roVector;
	return ( l_oVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator - ( void ) const
	{
	M_PROLOG
	HVector l_oVector( f_oData.size() );
	l_oVector = 0;
	l_oVector -= ( *this );
	return ( l_oVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator * ( value_type const& a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector( *this );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>HVector<value_type>::operator / ( value_type const& a_tScalar ) const
	{
	M_PROLOG
	HVector l_oVector ( * this );
	l_oVector /= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator += ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	yaal::transform( f_oData.begin(), f_oData.end(), a_roVector.f_oData.begin(), f_oData.begin(), yaal::plus<value_type>() );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator -= ( HVector const& a_roVector )
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	yaal::transform( f_oData.begin(), f_oData.end(), a_roVector.f_oData.begin(), f_oData.begin(), yaal::minus<value_type>() );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator *= ( value_type const& a_tScalar )
	{
	M_PROLOG
	yaal::transform( f_oData.begin(), f_oData.end(), f_oData.begin(), bind2nd( yaal::multiplies<value_type>(), a_tScalar ) );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator /= ( value_type const& a_tScalar )
	{
	M_PROLOG
	int long size = f_oData.size();
	if ( a_tScalar )
		yaal::transform( f_oData.begin(), f_oData.end(), f_oData.begin(), bind2nd( yaal::divides<value_type>(), a_tScalar ) );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
value_type HVector<value_type>::operator | ( HVector const& a_roVector ) const
	{
	M_PROLOG
	int long size = a_roVector.f_oData.size();
	check_dimensions( size );
	value_type l_tScalar = 0;
	for ( int long i = 0; i < size; i ++ ) 
		l_tScalar += ( f_oData[ i ] * a_roVector.f_oData[ i ] );
	return ( l_tScalar );
	M_EPILOG
	}

template<typename value_type>
value_type HVector<value_type>::operator ! ( void ) const
	{
	M_PROLOG
	return ( norm() );
	M_EPILOG
	}

template<typename value_type>
value_type const& HVector<value_type>::operator[] ( int long const& idx ) const
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename value_type>
value_type& HVector<value_type>::operator[] ( int long const& idx )
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename value_type>
bool HVector<value_type>::operator == ( HVector const& a_roVector ) const
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

template<typename value_type>
bool HVector<value_type>::operator != ( HVector const& a_roVector ) const
	{
	M_PROLOG
	return ( ! ( *this == a_roVector ) );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> operator * ( value_type const a_tScalar,
		HVector<value_type>const& a_roVector )
	{
	M_PROLOG
	HVector<value_type>l_oVector( a_roVector );
	l_oVector *= a_tScalar;
	return ( l_oVector );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HVECTOR_HXX_INCLUDED */


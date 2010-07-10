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

extern M_YAAL_HCORE_PUBLIC_API char const* _errMsgHVector_[];

/*! \brief Vector class for linear algebra calculus.
 */
template<typename value_type>
class HVector
	{
	typedef HArray<value_type> data_t;
	typedef HVector<value_type> this_type;
	data_t _data;
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
	inline void check_dimensions( int sizeAnother_ ) const
		{
		M_PROLOG
		if ( _data.size() != sizeAnother_ )
			M_THROW( _errMsgHVector_[ ERROR::DIMNOTMATCH ],
					_data.size() - sizeAnother_ );
		return;
		M_EPILOG
		}
	};

template<typename value_type>
HVector<value_type>::HVector( int long const& dimension_ ) : _data( dimension_, 0.0 )
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
HVector<value_type>::HVector( HVector const& vector_ ) : _data( vector_._data )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_type>
void HVector<value_type>::set( data_t const& rawData )
	{
	M_PROLOG
	_data = rawData;
	M_EPILOG
	}

template<typename value_type>
value_type HVector<value_type>::norm( void ) const
	{
	M_PROLOG
	value_type scalar = accumulate( _data.begin(), _data.end(), 0.0 );
	return ( sqrt( scalar ) );
	M_EPILOG
	}

template<typename value_type>
bool HVector<value_type>::normalize( void )
	{
	M_PROLOG
	value_type lenght = norm();
	if ( ! lenght )
		return ( true );
	int long size = _data.size();
	for ( int long i = 0; i < size; ++ i )
		_data[ i ] /= lenght;
	return ( false );
	M_EPILOG
	}

template<typename value_type>
int long HVector<value_type>::dim( void ) const
	{
	M_PROLOG
	return ( _data.size() );
	M_EPILOG
	}

template<typename value_type>
typename HVector<value_type>::iterator HVector<value_type>::begin( void )
	{
	return ( _data.begin() );
	}

template<typename value_type>
typename HVector<value_type>::const_iterator HVector<value_type>::begin( void ) const
	{
	return ( _data.begin() );
	}

template<typename value_type>
typename HVector<value_type>::iterator HVector<value_type>::end( void )
	{
	return ( _data.end() );
	}

template<typename value_type>
typename HVector<value_type>::const_iterator HVector<value_type>::end( void ) const
	{
	return ( _data.end() );
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator = ( HVector const& vector_ )
	{
	M_PROLOG
	int long size = vector_._data.size();
	if ( _data.size() && size )
		check_dimensions( size );
	_data = vector_._data;
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator = ( value_type const& scalar_ )
	{
	M_PROLOG
	fill( _data.begin(), _data.end(), scalar_ );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator + ( HVector const& vector_ ) const
	{
	M_PROLOG
	check_dimensions( vector_._data.size() );
	HVector vector ( *this );
	vector += vector_;
	return ( vector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator - ( HVector const& vector_ ) const
	{
	M_PROLOG
	check_dimensions( vector_._data.size() );
	HVector vector( *this );
	vector -= vector_;
	return ( vector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator - ( void ) const
	{
	M_PROLOG
	HVector vector( _data.size() );
	vector = 0;
	vector -= ( *this );
	return ( vector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> HVector<value_type>::operator * ( value_type const& scalar_ ) const
	{
	M_PROLOG
	HVector vector( *this );
	vector *= scalar_;
	return ( vector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>HVector<value_type>::operator / ( value_type const& scalar_ ) const
	{
	M_PROLOG
	HVector vector ( * this );
	vector /= scalar_;
	return ( vector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator += ( HVector const& vector_ )
	{
	M_PROLOG
	int long size = vector_._data.size();
	check_dimensions( size );
	yaal::transform( _data.begin(), _data.end(), vector_._data.begin(), _data.begin(), yaal::plus<value_type>() );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator -= ( HVector const& vector_ )
	{
	M_PROLOG
	int long size = vector_._data.size();
	check_dimensions( size );
	yaal::transform( _data.begin(), _data.end(), vector_._data.begin(), _data.begin(), yaal::minus<value_type>() );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator *= ( value_type const& scalar_ )
	{
	M_PROLOG
	yaal::transform( _data.begin(), _data.end(), _data.begin(), bind2nd( yaal::multiplies<value_type>(), scalar_ ) );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type>& HVector<value_type>::operator /= ( value_type const& scalar_ )
	{
	M_PROLOG
	int long size = _data.size();
	if ( scalar_ )
		yaal::transform( _data.begin(), _data.end(), _data.begin(), bind2nd( yaal::divides<value_type>(), scalar_ ) );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
value_type HVector<value_type>::operator | ( HVector const& vector_ ) const
	{
	M_PROLOG
	int long size = vector_._data.size();
	check_dimensions( size );
	value_type scalar = 0;
	for ( int long i = 0; i < size; i ++ ) 
		scalar += ( _data[ i ] * vector_._data[ i ] );
	return ( scalar );
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
	return ( _data[ idx ] );
	M_EPILOG
	}

template<typename value_type>
value_type& HVector<value_type>::operator[] ( int long const& idx )
	{
	M_PROLOG
	return ( _data[ idx ] );
	M_EPILOG
	}

template<typename value_type>
bool HVector<value_type>::operator == ( HVector const& vector_ ) const
	{
	M_PROLOG
	int long size = vector_._data.size();
	check_dimensions( size );
	for ( int long i = 0; i < size; ++ i ) 
		if ( _data[ i ] != vector_._data[ i ] )
			return ( false );
	return ( true );
	M_EPILOG
	}

template<typename value_type>
bool HVector<value_type>::operator != ( HVector const& vector_ ) const
	{
	M_PROLOG
	return ( ! ( *this == vector_ ) );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> operator * ( value_type const scalar_,
		HVector<value_type>const& vector_ )
	{
	M_PROLOG
	HVector<value_type>vector( vector_ );
	vector *= scalar_;
	return ( vector );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HVECTOR_HXX_INCLUDED */


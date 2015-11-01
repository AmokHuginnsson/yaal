/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hvector.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/*! \file hcore/hvector.hxx
 * \brief Declaration and implementation of HVector<> class template.
 */

#ifndef YAAL_HCORE_HVECTOR_HXX_INCLUDED
#define YAAL_HCORE_HVECTOR_HXX_INCLUDED 1

#include <cmath>

#include "hcore/harray.hxx"
#include "hcore/hexception.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* _errMsgHVector_[];

/*! \brief Vector class for linear algebra calculus.
 */
template<typename value_t>
class HVector final {
public:
	typedef value_t value_type;
	typedef HArray<value_type> data_t;
	typedef HVector<value_type> this_type;
private:
	data_t _data;
	/*! \brief Error type of HVector<> operations.
	 */
	struct ERROR {
		/*! \brief Error type flags.
		 */
		typedef enum {
			OK = 0, /*!< no error. */
			DIMNOTMATCH /*!< binary operation applied to two vectors with different dimensions. */
		} error_t;
	};
public:
	typedef typename data_t::iterator iterator;
	typedef typename data_t::const_iterator const_iterator;
	HVector( int long dimension_ = 0 )
		: _data( dimension_, 0.0 ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	HVector( HVector const& vector_ )
		: _data( vector_._data ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	~HVector( void ) {
		M_PROLOG
		return;
		M_EPILOG
	}
	void set( data_t const& rawData ) {
		M_PROLOG
		_data = rawData;
		return;
		M_EPILOG
	}
	bool normalize( void ) {
		M_PROLOG
		value_type length = norm();
		bool normalized( false );
		if ( length != 0 ) {
			int long size = _data.size();
			for ( int long i = 0; i < size; ++ i ) {
				_data[ i ] /= length;
			}
			normalized = true;
		}
		return ( normalized );
		M_EPILOG
	}
	int long dim( void ) const {
		M_PROLOG
		return ( _data.size() );
		M_EPILOG
	}
	value_type norm( void ) const {
		M_PROLOG
		value_type scalar( inner_product( _data.begin(), _data.end(), _data.begin(), 0.0 ) );
		return ( square_root( scalar ) );
		M_EPILOG
	}
	HVector& operator = ( HVector const& vector_ ) {
		M_PROLOG
		if ( &vector_ != this ) {
			int long size( vector_._data.size() );
			if ( _data.size() && size ) {
				check_dimensions( size );
			}
			HVector tmp( vector_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HVector& vector_ ) {
		M_PROLOG
		if ( &vector_ != this ) {
			using yaal::swap;
			swap( _data, vector_._data );
		}
		return;
		M_EPILOG
	}
	HVector& operator = ( value_type const& scalar_ ) {
		M_PROLOG
		fill( _data.begin(), _data.end(), scalar_ );
		return ( *this );
		M_EPILOG
	}
	HVector operator + ( HVector const& vector_ ) const {
		M_PROLOG
		check_dimensions( vector_._data.size() );
		HVector vector ( *this );
		vector += vector_;
		return ( vector );
		M_EPILOG
	}
	HVector operator - ( HVector const& vector_ ) const {
		M_PROLOG
		check_dimensions( vector_._data.size() );
		HVector vector( *this );
		vector -= vector_;
		return ( vector );
		M_EPILOG
	}
	HVector operator - ( void ) const {
		M_PROLOG
		HVector vector( _data.size() );
		vector = 0;
		vector -= ( *this );
		return ( vector );
		M_EPILOG
	}
	HVector operator * ( value_type const& scalar_ ) const {
		M_PROLOG
		HVector vector( *this );
		vector *= scalar_;
		return ( vector );
		M_EPILOG
	}
	HVector operator / ( value_type const& scalar_ ) const {
		M_PROLOG
		HVector vector ( * this );
		vector /= scalar_;
		return ( vector );
		M_EPILOG
	}
	HVector& operator += ( HVector const& vector_ ) {
		M_PROLOG
		int long size = vector_._data.size();
		check_dimensions( size );
		yaal::transform( _data.begin(), _data.end(), vector_._data.begin(), _data.begin(), yaal::plus<value_type>() );
		return ( *this );
		M_EPILOG
	}
	HVector& operator -= ( HVector const& vector_ ) {
		M_PROLOG
		int long size = vector_._data.size();
		check_dimensions( size );
		yaal::transform( _data.begin(), _data.end(), vector_._data.begin(), _data.begin(), yaal::minus<value_type>() );
		return ( *this );
		M_EPILOG
	}
	HVector& operator *= ( value_type const& scalar_ ) {
		M_PROLOG
		yaal::transform( _data.begin(), _data.end(), _data.begin(), bind2nd( yaal::multiplies<value_type>(), scalar_ ) );
		return ( *this );
		M_EPILOG
	}
	HVector& operator /= ( value_type const& scalar_ ) {
		M_PROLOG
		if ( scalar_ )
			yaal::transform( _data.begin(), _data.end(), _data.begin(), bind2nd( yaal::divides<value_type>(), scalar_ ) );
		return ( *this );
		M_EPILOG
	}
	value_type operator | ( HVector const& vector_ ) const {
		M_PROLOG
		int long size = vector_._data.size();
		check_dimensions( size );
		value_type scalar = 0;
		for ( int long i = 0; i < size; i ++ )
			scalar += ( _data[ i ] * vector_._data[ i ] );
		return ( scalar );
		M_EPILOG
	}
	value_type operator ! ( void ) const {
		M_PROLOG
		return ( norm() );
		M_EPILOG
	}
	value_type const& operator[] ( int long idx ) const {
		M_PROLOG
		return ( _data[ idx ] );
		M_EPILOG
	}
	value_type& operator[] ( int long idx ) {
		M_PROLOG
		return ( _data[ idx ] );
		M_EPILOG
	}
	bool operator == ( HVector const& vector_ ) const {
		M_PROLOG
		int long size = vector_._data.size();
		check_dimensions( size );
		for ( int long i = 0; i < size; ++ i )
			if ( _data[ i ] != vector_._data[ i ] )
				return ( false );
		return ( true );
		M_EPILOG
	}
	bool operator != ( HVector const& vector_ ) const {
		M_PROLOG
		return ( ! ( *this == vector_ ) );
		M_EPILOG
	}
	iterator begin( void ) {
		return ( _data.begin() );
	}
	const_iterator begin( void ) const {
		return ( _data.begin() );
	}
	iterator end( void ) {
		return ( _data.end() );
	}
	const_iterator end( void ) const {
		return ( _data.end() );
	}
private:
	inline void check_dimensions( int sizeAnother_ ) const {
		M_PROLOG
		if ( _data.size() != sizeAnother_ )
			M_THROW( _errMsgHVector_[ ERROR::DIMNOTMATCH ],
					_data.size() - sizeAnother_ );
		return;
		M_EPILOG
	}
};

template<typename value_t>
HVector<value_t> operator * ( value_t const scalar_,
		HVector<value_t>const& vector_ ) {
	M_PROLOG
	HVector<value_t> vector( vector_ );
	vector *= scalar_;
	return ( vector );
	M_EPILOG
}

template<typename value_t>
inline void swap( yaal::hcore::HVector<value_t>& a, yaal::hcore::HVector<value_t>& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HVECTOR_HXX_INCLUDED */


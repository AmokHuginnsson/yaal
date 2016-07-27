/*
---           `yaal' (column_) 1978 by Marcin 'Amok' Konarski            ---

  hmatrix.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hmatrix.hxx
 * \brief Declaration and implementation of HMatrix class template.
 */

#ifndef YAAL_HCORE_HMATRIX_HXX_INCLUDED
#define YAAL_HCORE_HMATRIX_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hvector.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHMatrix_[];

/*! \brief Abstraction of Matrix, and all relevant algebraic operations.
 */
template<typename value_type>
class HMatrix final {
	typedef HMatrix<value_type> this_type;
public:
	typedef value_type value_t;
	/*! \brief Error codes for HMatrix<> operations.
	 */
	struct ERROR {
		/*! \brief Error codes for HMatrix<> operations.
		 */
		typedef enum {
			OK = 0,       /*!< No error. */
			BAD_ROWS,     /*!< Bad row index. */
			BAD_COLUMNS,  /*!< Bad column index. */
			DIM_NOT_MATCH_ROWS, /*!< Number of rows do not match for binary operation. */
			DIM_NOT_MATCH_COLUMNS, /*!< Number of columns do not match for binary operation. */
			NOT_A_SQUARE, /*!< Non-square matrix used where square necessary. */
			ODD,          /*!< Odd matrix while counting inverse. */
			DIM_NOT_MATCH_COLUMNS_ROWS, /*!< Number of rows does not match numbers of columns. */
			ROW_OUT_OF_RANGE,
			COLUMN_OUT_OF_RANGE,
			DIVISION_BY_ZERO
		} error_t;
	};
	typedef HVector<value_type> row_t;
	class HRowRef {
		row_t& _row;
		HRowRef( row_t& row_ )
			: _row( row_ )
			{}
	public:
		value_type& operator[]( int long index_ ) {
			return ( _row[index_] );
		}
	friend class HMatrix<value_type>;
	};
private:
	typedef HArray<row_t> data_t;
	int _rows;
	int _columns;
	data_t _data;
public:
	HMatrix( int rows_, int columns_ )
		: _rows( rows_ )
		, _columns( columns_ )
		, _data( _rows, row_t( columns_ ) ) {
		M_PROLOG
		if ( rows_ < 1 ) {
			M_THROW( _errMsgHMatrix_[ ERROR::BAD_ROWS ], rows_ );
		} else {
			_rows = rows_;
		}
		if ( columns_ < 1 ) {
			M_THROW( _errMsgHMatrix_[ ERROR::BAD_COLUMNS ], columns_ );
		} else {
			_columns = columns_;
		}
		return;
		M_EPILOG
	}

	HMatrix( HMatrix const& matrix_ )
		: _rows( matrix_._rows )
		, _columns( matrix_._columns )
		, _data() {
		M_PROLOG
		if ( ( _rows > 0 ) || ( _columns > 0 ) ) {
			check_dimensions_rows_columns( matrix_._rows,
					matrix_._columns );
		}
		data_t tmp( matrix_._data );
		_data.swap( tmp );
		return;
		M_EPILOG
	}
	void swap( HMatrix& matrix_ ) {
		M_PROLOG
		if ( &matrix_ != this ) {
			using yaal::swap;
			swap( _rows, matrix_._rows );
			swap( _columns, matrix_._columns );
			swap( _data, matrix_._data );
		}
		return;
		M_EPILOG
	}
	void set( value_type const** scalar_ ) {
		M_PROLOG
		int ctr;
		for ( ctr = 0; ctr < _rows; ctr++ ) {
			this->_data[ ctr ]->set( scalar_[ ctr ] );
		}
		return;
		M_EPILOG
	}
	int row( void ) const {
		return ( _rows );
	}

	int col( void ) const {
		return ( _columns );
	}
	value_type det( void ) const;
	value_type M( int const, int const );
	HMatrix T( void ) {
		M_PROLOG
		int ctrRow = 0, ctrColumn = 0;
		HMatrix matrix ( _columns, _rows );
		for ( ctrRow = 0; ctrRow < _rows; ctrRow ++ )
			for ( ctrColumn = 0; ctrColumn < _columns; ctrColumn ++ )
				matrix._data[ ctrColumn ][ ctrRow ] = _data[ ctrRow ][ ctrColumn ];
		return ( matrix );
		M_EPILOG
	}
	HMatrix _1( void );
	HMatrix& operator = ( HMatrix const& matrix_ ) {
		M_PROLOG
		if ( &matrix_ != this ) {
			HMatrix<value_type> tmp( matrix_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator = ( value_type scalar_ ) {
		M_PROLOG
		for ( typename data_t::iterator it( _data.begin() ), end( _data.end() ); it != end; ++ it ) {
			yaal::fill( it->begin(), it->end(), scalar_ );
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator += ( HMatrix const& matrix_ ) {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		int ctr = 0;
		for ( ctr = 0; ctr < _rows; ctr ++ ) {
			_data[ ctr ] += matrix_._data[ ctr ];
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator -= ( HMatrix const& matrix_ ) {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		int ctr = 0;
		for ( ctr = 0; ctr < _rows; ctr++ ) {
			_data[ ctr ] -= matrix_._data[ ctr ];
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator *= ( HMatrix const& matrix_ ) {
		M_PROLOG
		check_dimensions_columns_rows( matrix_._rows );
		value_type scalar = 0;
		int ctrRow = 0, ctrColumn = 0, ctrRowColumn = 0;
		data_t data( _rows, matrix_._columns );
		for ( ctrRow = 0; ctrRow < _rows; ctrRow ++ ) {
			for ( ctrColumn = 0; ctrColumn < matrix_._columns; ctrColumn ++, scalar = 0 ) {
				for ( ctrRowColumn = 0; ctrRowColumn < _columns; ctrRowColumn ++ ) {
					scalar += ( _data[ ctrRow ][ ctrRowColumn ] * matrix_._data[ ctrRowColumn ][ ctrColumn ] );
				}
				data[ ctrRow ][ ctrColumn ] = scalar;
			}
		}
		_data.swap( data );
		_columns = matrix_._columns;
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator *= ( value_type const& scalar_ ) {
		M_PROLOG
		for ( typename data_t::iterator it( _data.begin() ), end( _data.end() ); it != end; ++ it ) {
			yaal::transform( it->begin(), it->end(), it->begin(), bind2nd( yaal::multiplies<value_type>(), scalar_ ) );
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator /= ( value_type const& scalar_ ) {
		M_PROLOG
		if ( scalar_ == 0 ) {
			M_THROW( _errMsgHMatrix_[ERROR::DIVISION_BY_ZERO], 0 );
		}
		for ( typename data_t::iterator it( _data.begin() ), end( _data.end() ); it != end; ++ it ) {
			yaal::transform( it->begin(), it->end(), it->begin(), bind2nd( yaal::divides<value_type>(), scalar_ ) );
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix operator + ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		HMatrix matrix ( *this );
		matrix += matrix_;
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator - ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		HMatrix matrix ( *this );
		matrix -= matrix_;
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator - ( void ) const {
		M_PROLOG
		HMatrix<value_type> matrix( _rows, _columns );
		matrix = 0;
		matrix -= ( *this );
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator * ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_columns_rows( matrix_._rows );
		HMatrix matrix ( *this );
		matrix *= matrix_;
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator * ( value_type scalar_ ) const {
		M_PROLOG
		HMatrix matrix ( *this );
		matrix *= scalar_;
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator / ( value_type scalar_ ) const {
		M_PROLOG
		HMatrix matrix ( *this );
		matrix /= scalar_;
		return ( matrix );
		M_EPILOG
	}

	HMatrix operator ~ ( void ) const {
		M_PROLOG
		HMatrix matrix( _rows, _columns );
		for ( int ctr( 0 ); ctr < _rows; ++ ctr ) {
			matrix [ ctr ] = ~ _data[ ctr ];
		}
		return ( matrix );
		M_EPILOG
	}
	bool operator ! ( void ) const {
		M_PROLOG
		return ( ! det() );
		M_EPILOG
	}
	bool operator == ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		for ( int ctr( 0 ); ctr < _rows; ++ ctr ) {
			if ( this->_data[ ctr ] != matrix_ [ ctr ] ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}

	bool operator != ( HMatrix const& matrix_ ) const {
		M_PROLOG
		return ( ! ( * this == matrix_ ) );
		M_EPILOG
	}
	HRowRef operator[] ( int idx ) {
		M_PROLOG
		return ( HRowRef( _data[ idx ] ) );
		M_EPILOG
	}
	row_t const& operator[] ( int idx ) const {
		M_PROLOG
		return ( _data[ idx ] );
		M_EPILOG
	}
	template<typename ttType>
	friend HVector<ttType> operator * ( HVector<ttType> const&,
			HMatrix const& );
	template<typename ttType>
	friend HVector<ttType> operator *= ( HVector<ttType> const&,
			HMatrix const& );
	template<typename ttType>
	friend HMatrix operator * ( ttType const, HMatrix<ttType> const& );
private:
	inline void check_dimensions_columns_rows( int rowsAnother_ ) const {
		M_PROLOG
		if ( _columns != rowsAnother_ ) {
			M_THROW( _errMsgHMatrix_[ ERROR::DIM_NOT_MATCH_COLUMNS_ROWS ], _columns - rowsAnother_ );
		}
		return;
		M_EPILOG
	}
	inline void check_dimensions_rows_columns( int rowsAnother_,
			int columnsAnother_ ) const {
		M_PROLOG
		if ( _rows != rowsAnother_ ) {
			M_THROW( _errMsgHMatrix_[ ERROR::DIM_NOT_MATCH_ROWS ], _rows - rowsAnother_ );
		}
		if ( _columns != columnsAnother_ ) {
			M_THROW( _errMsgHMatrix_[ ERROR::DIM_NOT_MATCH_COLUMNS ], _columns - columnsAnother_ );
		}
		return;
		M_EPILOG
	}
	inline void check_dimensions_square( void ) const {
		M_PROLOG
		if ( _rows != _columns ) {
			M_THROW( _errMsgHMatrix_[ ERROR::NOT_A_SQUARE ], _rows - _columns );
		}
		return;
		M_EPILOG
	}
};

template<typename value_type>
HVector<value_type> T( HMatrix<value_type> const&, HVector<value_type> const& );

template<typename value_type>
value_type HMatrix<value_type>::det( void ) const {
	M_PROLOG
	check_dimensions_square();
	int exchanges( 0 );
	value_type scalar;
	HMatrix<value_type> matrix( *this );
	for ( int ctrRow( 0 ); ctrRow < _rows; ctrRow ++ ) {
		if ( matrix [ ctrRow ] [ ctrRow ] != 0 )
			continue;
		using yaal::swap;
		int ctrLocRow( 0 );
		for ( ctrLocRow = 0; ctrLocRow < ctrRow; ctrLocRow ++ )
			if ( ( matrix [ ctrRow ] [ ctrLocRow ] != 0 )
					&& ( matrix [ ctrLocRow ] [ ctrRow ] != 0 ) ) {
				swap( matrix._data[ ctrLocRow ], matrix._data[ ctrRow ] );
				++ exchanges ;
				break;
			}
		if ( ctrLocRow == ctrRow ) {
			for ( ctrLocRow = ctrRow; ctrLocRow < _rows; ctrLocRow ++ ) {
				if ( matrix[ ctrRow ][ ctrLocRow ] != 0 ) {
					swap( matrix._data[ ctrLocRow ], matrix._data[ ctrRow ] );
					++ exchanges;
					break;
				}
			}
		}
	}
	row_t tmp( _columns );
	for ( int ctrRow( 0 ); ctrRow < _rows; ctrRow ++ )
		if ( matrix [ ctrRow ] [ ctrRow ] == 0 )
			return ( 0 );
	for ( int ctrRow( 0 ); ctrRow < _rows; ctrRow ++ ) {
		if ( matrix[ ctrRow ][ ctrRow ] == 0 )
			return ( 0 );
		for ( int ctrLocRow( ctrRow + 1 ); ctrLocRow < _rows; ctrLocRow ++ ) {
			scalar = -matrix[ ctrLocRow ][ ctrRow ] / matrix[ ctrRow ][ ctrRow ];
			tmp = matrix._data[ ctrRow ];
			tmp *= scalar;
			matrix._data[ ctrLocRow ] += tmp;
		}
	}
	scalar = 1;
	for ( int ctrRow( 0 ); ctrRow < _rows; ctrRow ++ )
		scalar *= matrix [ ctrRow ] [ ctrRow ];
	if ( exchanges % 2 )
		scalar = -scalar;
	return ( scalar );
	M_EPILOG
}

template<typename value_type>
value_type HMatrix<value_type>::M( int const row_, int const column_ ) {
	M_PROLOG
	check_dimensions_square();
	int ctrRow = 0, ctrColumn = 0, ctrRowVirtual = 0, ctrColumnVirtual = 0;
	if ( row_ >= _rows )
		M_THROW( _errMsgHMatrix_[ ERROR::ROW_OUT_OF_RANGE ], row_ - _rows );
	if ( column_ >= _columns )
		M_THROW( _errMsgHMatrix_[ ERROR::COLUMN_OUT_OF_RANGE ], column_ - _columns );
	if ( _rows == 1 )
		return ( 0 );
	HMatrix matrix ( _rows - 1, _columns - 1 );
	for ( ctrRow = 0; ctrRow < _rows; ctrRow ++, ctrColumnVirtual = 0 ) {
		if ( ctrRow == row_ )
			continue;
		for ( ctrColumn = 0; ctrColumn < _columns; ctrColumn ++ ) {
			if ( ctrColumn == column_ )
				continue;
			matrix._data[ ctrRowVirtual ][ ctrColumnVirtual ] = _data[ ctrRow ][ ctrColumn ];
			ctrColumnVirtual ++;
		}
		ctrRowVirtual ++;
	}
	return ( matrix.det() );
	M_EPILOG
}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::_1( void ) {
	M_PROLOG
	check_dimensions_square();
	value_type scalar( 0 );
	if ( ( scalar = det() ) == 0 ) {
		M_THROW( _errMsgHMatrix_[ ERROR::ODD ], 0 );
	}
	HMatrix matrix( *this );
	for ( int ctrRow( 0 ); ctrRow < _rows; ctrRow ++ ) {
		for ( int ctrColumn( 0 ); ctrColumn < _columns; ctrColumn ++ ) {
			matrix._data[ ctrRow ][ ctrColumn ] = M( ctrRow,
					ctrColumn ) * ( ( ( ctrRow + ctrColumn ) % 2 ) ? -1 : 1 );
		}
	}
	return ( matrix.T() / scalar );
	M_EPILOG
}

template<typename value_type>
HVector<value_type> operator * ( HVector<value_type> const& vector_,
		HMatrix<value_type> const & matrix_ ) {
	M_PROLOG
	value_type scalar;
	if ( vector_.dim() == matrix_._rows ) {
		HVector<value_type> vector ( matrix_._columns );
		for ( int j( 0 ); j < matrix_._columns; ++ j ) {
			scalar = 0;
			for ( int k( 0 ); k < matrix_._columns; ++ k )
				scalar += vector_ [ k ] * matrix_[ k ][ j ];
			vector [ j ] = scalar;
		}
		return ( vector );
	}
	return ( vector_ );
	M_EPILOG
}

template<typename value_type>
HVector<value_type> operator *= ( HVector<value_type> const& vector_,
		HMatrix<value_type> const& matrix_ ) {
	M_PROLOG
	if ( vector_.dim() == matrix_._rows == matrix_._columns ) {
		HVector<value_type> vector = vector_ * matrix_;
		vector_ = vector;
	}
	return ( vector_ );
	M_EPILOG
}

template<typename value_type>
HMatrix<value_type> operator * ( value_type const scalar_,
		HMatrix<value_type> const& matrix_ ) {
	M_PROLOG
	HMatrix<value_type> matrix ( matrix_ );
	matrix *= scalar_;
	return ( matrix );
	M_EPILOG
}

template<typename value_type>
HVector<value_type> T ( HMatrix<value_type> const& matrix_,
		HVector<value_type> const& vector_ ) {
	M_PROLOG
	HMatrix<value_type> matrix = matrix_.T();
	return ( vector_ * matrix );
	M_EPILOG
}

template<typename value_type>
inline void swap( yaal::hcore::HMatrix<value_type>& a, yaal::hcore::HMatrix<value_type>& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HMATRIX_HXX_INCLUDED */


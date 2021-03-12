/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
template<typename value_type_t>
class HMatrix final {
public:
	typedef HMatrix<value_type_t> this_type;
	typedef value_type_t value_type;
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
			: _row( row_ ) {
		}
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
	HMatrix( HMatrix&& matrix_ )
		: _rows( matrix_._rows )
		, _columns( matrix_._columns )
		, _data( yaal::move( matrix_._data ) ) {
		M_PROLOG
		matrix_._rows = 0;
		matrix_._columns = 0;
		return;
		M_EPILOG
	}
	HMatrix( std::initializer_list<row_t> constants_ )
		: _rows( constants_.size() )
		, _columns( constants_.begin()->dim() )
		, _data( constants_ ) {
		int rn( 0 );
		for ( row_t const& r : _data ) {
			if ( r.dim() != _columns ) {
				M_THROW( _errMsgHMatrix_[ERROR::DIM_NOT_MATCH_COLUMNS], rn );
			}
			++ rn;
		}
		return;
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
	int rows( void ) const {
		return ( _rows );
	}

	int columns( void ) const {
		return ( _columns );
	}
	value_type det( void ) const;
	HMatrix inverse( void ) const;
	HMatrix transpose( void ) {
		M_PROLOG
		HMatrix matrix ( _columns, _rows );
		for ( int r( 0 ); r < _rows; ++ r ) {
			for ( int c( 0 ); c < _columns; ++ c ) {
				matrix._data[ c ][ r ] = _data[ r ][ c ];
			}
		}
		return matrix;
		M_EPILOG
	}
	HMatrix& operator = ( HMatrix const& matrix_ ) {
		M_PROLOG
		if ( &matrix_ != this ) {
			HMatrix tmp( matrix_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	HMatrix& operator = ( HMatrix&& matrix_ ) {
		M_PROLOG
		if ( &matrix_ != this ) {
			swap( matrix_ );
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
		for ( int r( 0 ); r < _rows; ++ r ) {
			_data[ r ] += matrix_._data[ r ];
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator -= ( HMatrix const& matrix_ ) {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		for ( int r( 0 ); r < _rows; ++ r ) {
			_data[ r ] -= matrix_._data[ r ];
		}
		return ( *this );
		M_EPILOG
	}

	HMatrix& operator *= ( HMatrix const& matrix_ ) {
		M_PROLOG
		check_dimensions_columns_rows( matrix_._rows );
		value_type scalar = 0;
		data_t data( _rows, matrix_._columns );
		for ( int r( 0 ); r < _rows; ++ r ) {
			for ( int c( 0 ); c < matrix_._columns; ++ c, scalar = 0 ) {
				for ( int rc( 0 ); rc < _columns; ++ rc ) {
					scalar += ( _data[ r ][ rc ] * matrix_._data[ rc ][ c ] );
				}
				data[ r ][ c ] = scalar;
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
		return matrix;
		M_EPILOG
	}

	HMatrix operator - ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		HMatrix matrix ( *this );
		matrix -= matrix_;
		return matrix;
		M_EPILOG
	}

	HMatrix operator - ( void ) const {
		M_PROLOG
		this_type matrix( _rows, _columns );
		matrix = 0;
		matrix -= ( *this );
		return matrix;
		M_EPILOG
	}

	HMatrix operator * ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_columns_rows( matrix_._rows );
		HMatrix matrix ( *this );
		matrix *= matrix_;
		return matrix;
		M_EPILOG
	}

	HMatrix operator * ( value_type scalar_ ) const {
		M_PROLOG
		HMatrix matrix ( *this );
		matrix *= scalar_;
		return matrix;
		M_EPILOG
	}

	HMatrix operator / ( value_type scalar_ ) const {
		M_PROLOG
		HMatrix matrix ( *this );
		matrix /= scalar_;
		return matrix;
		M_EPILOG
	}

	HMatrix operator ~ ( void ) const {
		M_PROLOG
		HMatrix matrix( _rows, _columns );
		for ( int r( 0 ); r < _rows; ++ r ) {
			matrix [ r ] = ~ _data[ r ];
		}
		return matrix;
		M_EPILOG
	}
	explicit operator bool ( void ) const {
		M_PROLOG
		return ( det() != 0.L );
		M_EPILOG
	}
	bool operator == ( HMatrix const& matrix_ ) const {
		M_PROLOG
		check_dimensions_rows_columns( matrix_._rows, matrix_._columns );
		for ( int r( 0 ); r < _rows; ++ r ) {
			if ( this->_data[ r ] != matrix_._data[ r ] ) {
				return ( false );
			}
		}
		return ( true );
		M_EPILOG
	}

	bool operator != ( HMatrix const& matrix_ ) const {
		M_PROLOG
		return ( ! operator == ( matrix_ ) );
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

template<typename value_type_t>
HVector<value_type_t> transpose( HMatrix<value_type_t> const&, HVector<value_type_t> const& );

template<typename value_type_t>
typename HMatrix<value_type_t>::value_type HMatrix<value_type_t>::det( void ) const {
	M_PROLOG
	check_dimensions_square();
	int exchanges( 0 );
	value_type scalar;
	this_type matrix( *this );
	row_t tmp( _columns );
	for ( int r( 0 ); r < _rows; ++ r ) {
		value_type maxVal( 0 );
		value_type tmpVal( 0 );
		int maxRow( 0 );
		for ( int k( r ); k < _rows; ++ k ) {
			tmpVal = math::abs( matrix[k][r] );
			if ( tmpVal > maxVal ) {
				maxVal = tmpVal;
				maxRow = k;
			}
		}
		if ( maxVal == 0 ) {
			return ( 0 );
		}
		if ( r != maxRow ) {
			using yaal::swap;
			swap( matrix._data[r], matrix._data[maxRow] );
			++ exchanges;
		}
		value_type const& pivot( matrix[ r ][ r ] );
		for ( int k( r + 1 ); k < _rows; ++ k ) {
			scalar = -matrix[ k ][ r ] / pivot;
			tmp = matrix._data[ r ];
			tmp *= scalar;
			matrix._data[ k ] += tmp;
		}
	}
	scalar = 1;
	for ( int r( 0 ); r < _rows; ++ r ) {
		scalar *= matrix[ r ][ r ];
	}
	if ( exchanges % 2 ) {
		scalar = -scalar;
	}
	return scalar;
	M_EPILOG
}

template<typename value_type_t>
HMatrix<value_type_t> HMatrix<value_type_t>::inverse( void ) const {
	M_PROLOG
	check_dimensions_square();
	value_type scalar;
	this_type matrix( *this );
	this_type inversed( _rows, _columns );
	typedef HPair<int, int> swap_t;
	typedef HArray<swap_t> swaps_t;
	swaps_t swaps;
	for ( int r( 0 ); r < _rows; ++ r ) {
		inversed[r][r] = 1;
	}
	row_t tmp( _columns );
	row_t vec( _columns );
	using yaal::swap;
	for ( int r( 0 ); r < _rows; ++ r ) {
		value_type maxVal( 0 );
		value_type tmpVal( 0 );
		int maxRow( 0 );
		for ( int k( r ); k < _rows; ++ k ) {
			tmpVal = math::abs( matrix[k][r] );
			if ( tmpVal > maxVal ) {
				maxVal = tmpVal;
				maxRow = k;
			}
		}
		if ( maxVal == 0 ) {
			M_THROW( _errMsgHMatrix_[ ERROR::ODD ], 0 );
		}
		if ( r != maxRow ) {
			swap( matrix._data[r], matrix._data[maxRow] );
			swap( inversed._data[r], inversed._data[maxRow] );
			swaps.emplace_back( r, maxRow );
		}
		value_type const& pivot( matrix[ r ][ r ] );
		for ( int k( r + 1 ); k < _rows; ++ k ) {
			scalar = -matrix[ k ][ r ] / pivot;
			tmp = matrix._data[ r ];
			tmp *= scalar;
			vec = inversed._data[ r ];
			vec *= scalar;
			matrix._data[ k ] += tmp;
			inversed._data[ k ] += vec;
		}
	}
	for ( int r( _rows - 1 ); r >= 0; -- r ) {
		value_type const& pivot( matrix[ r ][ r ] );
		for ( int k( r - 1 ); k >= 0; -- k ) {
			scalar = -matrix[ k ][ r ] / pivot;
			tmp = matrix._data[ r ];
			tmp *= scalar;
			vec = inversed._data[ r ];
			vec *= scalar;
			matrix._data[ k ] += tmp;
			inversed._data[ k ] += vec;
		}
	}
	for ( int r( 0 ); r < _rows; ++ r ) {
		inversed._data[r] /= matrix._data[r][r];
	}
#if 0
	while ( ! swaps.is_empty() ) {
		swap_t const& s( swaps.back() );
		swap( inversed._data[s.first], inversed._data[s.second] );
		swaps.pop_back();
	}
#endif
	return inversed;
	M_EPILOG
}

template<typename value_type_t>
HVector<value_type_t> operator * ( HVector<value_type_t> const& vector_,
		HMatrix<value_type_t> const& matrix_ ) {
	M_PROLOG
	typename HMatrix<value_type_t>::value_type scalar;
	if ( vector_.dim() == matrix_._rows ) {
		HVector<value_type_t> vector( matrix_._columns );
		for ( int j( 0 ); j < matrix_._columns; ++ j ) {
			scalar = 0;
			for ( int k( 0 ); k < matrix_._columns; ++ k ) {
				scalar += vector_[ k ] * matrix_[ k ][ j ];
			}
			vector [ j ] = scalar;
		}
		return vector;
	}
	return vector_;
	M_EPILOG
}

template<typename value_type_t>
HVector<value_type_t> operator *= ( HVector<value_type_t> const& vector_,
		HMatrix<value_type_t> const& matrix_ ) {
	M_PROLOG
	if ( ( vector_.dim() == matrix_.rows() ) && ( matrix_.rows() == matrix_.columns() ) ) {
		HVector<value_type_t> vector( vector_ * matrix_ );
		vector_ = vector;
	}
	return vector_;
	M_EPILOG
}

template<typename value_type_t>
HMatrix<value_type_t> operator * ( typename HMatrix<value_type_t>::value_type const& scalar_,
		HMatrix<value_type_t> const& matrix_ ) {
	M_PROLOG
	HMatrix<value_type_t> matrix( matrix_ );
	matrix *= scalar_;
	return matrix;
	M_EPILOG
}

template<typename value_type_t>
HVector<value_type_t> transpose( HMatrix<value_type_t> const& matrix_, HVector<value_type_t> const& vector_ ) {
	M_PROLOG
	HMatrix<value_type_t> matrix = matrix_.transpose();
	return ( vector_ * matrix );
	M_EPILOG
}

template<typename value_type_t>
inline void swap( yaal::hcore::HMatrix<value_type_t>& a, yaal::hcore::HMatrix<value_type_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HMATRIX_HXX_INCLUDED */


/*
---           `yaal' (a_iColumn) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HMATRIX_HXX_INCLUDED
#define YAAL_HCORE_HMATRIX_HXX_INCLUDED

#line 31

#include "hcore/hexception.hxx"
#include "hcore/hvector.hxx"

namespace yaal
{

namespace hcore
{

extern char const* const g_ppcErrMsgHMatrix[];

/*! \brief Abstraction of Matrix, and all relevant algebraic operations.
 */
template<typename value_type>
class HMatrix
	{
	typedef HMatrix<value_type> self_t;
public:
	typedef value_type value_t;
	/*! \brief Error codes for HMatrix<> operations.
	 */
	struct ERROR
		{
		/*! \brief Error codes for HMatrix<> operations.
		 */
		typedef enum
			{
			OK = 0,       /*!< No error. */
			BAD_ROWS,     /*!< Bad row index. */
			BAD_COLUMNS,  /*!< Bad column index. */
			DIM_NOT_MATCH_ROWS, /*!< Number of rows do not match for binary operation. */
			DIM_NOT_MATCH_COLUMNS, /*!< Number of columns do not match for binary operation. */
			NOT_A_SQUARE, /*!< Non-square matrix used where square necessary. */
			ODD,          /*!< Odd matrix while counting inverse. */
			DIM_NOT_MATCH_COLUMNS_ROWS, /*!< Number of rows does not match numbers of columns. */
			DIM_NOT_MATCH_COLUMNS_ROWS_COLUMNS,
			ROW_OUT_OF_RANGE,
			COLUMN_OUT_OF_RANGE
			} error_t;
		};
	typedef HVector<value_type> row_t;
private:
	typedef HArray<row_t> data_t;
	int f_iRows;
	int f_iColumns;
	data_t f_oData;
public:
	HMatrix( int const, int const );
	HMatrix( HMatrix const& );
	virtual ~HMatrix( void );
	void swap( HMatrix& );
	int set( value_type const** );
	int row( void ) const;
	int col( void ) const;
	value_type det( void ) const;
	value_type M( int const, int const );
	HMatrix T( void );
	HMatrix _1( void );
	HMatrix& operator = ( HMatrix const & );
	HMatrix& operator = ( value_type const );
	HMatrix operator + ( HMatrix const & ) const;
	HMatrix operator - ( HMatrix const & ) const;
	HMatrix operator - ( void ) const;
	HMatrix operator * ( HMatrix const & ) const;
	HMatrix operator * ( value_type const ) const;
	HMatrix operator / ( value_type const ) const;
	HMatrix& operator += ( HMatrix const & );
	HMatrix& operator -= ( HMatrix const & );
	HMatrix& operator *= ( HMatrix const & );
	HMatrix& operator *= ( value_type const );
	HMatrix& operator /= ( value_type const );
	HMatrix operator ~ ( void ) const;
	value_type operator ! ( void ) const;
	bool operator == ( HMatrix const & ) const;
	bool operator != ( HMatrix const & ) const;
	row_t& operator[] ( int const& );
	row_t const& operator[] ( int const& ) const;
	template<typename ttType>
	friend HVector<ttType> operator * ( HVector<ttType> const&,
			HMatrix const& );
	template<typename ttType>
	friend HVector<ttType> operator *= ( HVector<ttType> const&,
			HMatrix const& );
	template<typename ttType>
	friend HMatrix operator * ( ttType const, HMatrix<ttType> const& );
private:
	inline void check_dimensions_columns_rows( int a_iRowsAnother ) const
		{
		M_PROLOG
		if ( f_iColumns != a_iRowsAnother )
			M_THROW( g_ppcErrMsgHMatrix[ ERROR::DIM_NOT_MATCH_COLUMNS_ROWS ],
					f_iColumns - a_iRowsAnother );
		return;
		M_EPILOG
		}
	inline void check_dimensions_rows_columns( int a_iRowsAnother,
			int a_iColumnsAnother ) const
		{
		M_PROLOG
		if ( f_iRows != a_iRowsAnother )
			M_THROW( g_ppcErrMsgHMatrix[ ERROR::DIM_NOT_MATCH_ROWS ],
					f_iRows - a_iRowsAnother );
		if ( f_iColumns != a_iColumnsAnother )
			M_THROW( g_ppcErrMsgHMatrix[ ERROR::DIM_NOT_MATCH_COLUMNS ],
					f_iColumns - a_iColumnsAnother );
		return;
		M_EPILOG
		}
	inline void check_dimensions_square( void ) const
		{
		M_PROLOG
		if ( f_iRows != f_iColumns )
			M_THROW( g_ppcErrMsgHMatrix[ ERROR::NOT_A_SQUARE ], f_iRows - f_iColumns );
		return;
		M_EPILOG
		}
	};

template<typename value_type>
HVector<value_type> T( HMatrix<value_type> const&, HVector<value_type> const& );

template<typename value_type>
HMatrix<value_type>::HMatrix( int const a_iRows, int const a_iColumns )
	: f_iRows( a_iRows ), f_iColumns( a_iColumns ),
	f_oData( f_iRows, row_t( a_iColumns ) )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_iRows < 1 )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::BAD_ROWS ], a_iRows );
	else
		f_iRows = a_iRows;
	if ( a_iColumns < 1 )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::BAD_COLUMNS ], a_iColumns );
	else
		f_iColumns = a_iColumns;
	return ;
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>::HMatrix( HMatrix const& a_roMatrix )
	: f_iRows( a_roMatrix.f_iRows ), f_iColumns( a_roMatrix.f_iColumns ), f_oData()
	{
	M_PROLOG
	if ( ( f_iRows > 0 ) || ( f_iColumns > 0 ) )
		check_dimensions_rows_columns( a_roMatrix.f_iRows,
				a_roMatrix.f_iColumns );
	data_t tmp( a_roMatrix.f_oData );
	f_oData.swap( tmp );
	return;
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>::~HMatrix( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename value_type>
int HMatrix<value_type>::set( value_type const** a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		this->f_ptArray [ l_iCtr ]->set( a_tScalar[ l_iCtr ] );
	return ( 0 );
	M_EPILOG
	}

template<typename value_type>
int HMatrix<value_type>::row( void ) const
	{
	M_PROLOG
	return ( f_iRows );
	M_EPILOG
	}

template<typename value_type>
int HMatrix<value_type>::col( void ) const
	{
	M_PROLOG
	return ( f_iColumns );
	M_EPILOG
	}

template<typename value_type>
value_type HMatrix<value_type>::det( void ) const
	{
	M_PROLOG
	check_dimensions_square();
	int l_iCtrLocRow = 0, l_iCtrRow = 0, l_iExchanges = 0;
	value_type l_tScalar;
	HMatrix<value_type> l_oMatrix ( * this );
	HVector<value_type> l_oVector ( f_iColumns );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		{
		if ( l_oMatrix [ l_iCtrRow ] [ l_iCtrRow ] != 0 )
			continue;
		for ( l_iCtrLocRow = 0; l_iCtrLocRow < l_iCtrRow; l_iCtrLocRow ++ )
			if ( ( l_oMatrix [ l_iCtrRow ] [ l_iCtrLocRow ] != 0 )
					&& ( l_oMatrix [ l_iCtrLocRow ] [ l_iCtrRow ] != 0 ) )
				{
				l_oVector = l_oMatrix [ l_iCtrLocRow ];
				l_oMatrix [ l_iCtrLocRow ] = l_oMatrix [ l_iCtrRow ];
				l_oMatrix [ l_iCtrRow ] = l_oVector;
				l_iExchanges ++;
				break;
				}
		if ( l_iCtrLocRow == l_iCtrRow )
			for ( l_iCtrLocRow = l_iCtrRow; l_iCtrLocRow < f_iRows; l_iCtrLocRow ++ )
				if ( l_oMatrix [ l_iCtrRow ] [ l_iCtrLocRow ] != 0 )
					{
					l_oVector = l_oMatrix [ l_iCtrLocRow ];
					l_oMatrix [ l_iCtrLocRow ] = l_oMatrix [ l_iCtrRow ];
					l_oMatrix [ l_iCtrRow ] = l_oVector;
					l_iExchanges++;
					break;
					}
		}
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		if ( l_oMatrix [ l_iCtrRow ] [ l_iCtrRow ] == 0 )
			return ( 0 );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		{
		if ( l_oMatrix [ l_iCtrRow ] [ l_iCtrRow ] == 0 )
			return ( 0 );
		for ( l_iCtrLocRow = l_iCtrRow + 1; l_iCtrLocRow < f_iRows; l_iCtrLocRow ++ )
			{
			l_tScalar = -l_oMatrix [ l_iCtrLocRow ] [ l_iCtrRow ] / l_oMatrix [ l_iCtrRow ] [ l_iCtrRow ];
			l_oVector = ( l_tScalar * l_oMatrix [ l_iCtrRow ] );
			l_oMatrix [ l_iCtrLocRow ] += l_oVector;
			}
		}
	l_tScalar = 1;
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		l_tScalar *= l_oMatrix [ l_iCtrRow ] [ l_iCtrRow ];
	if ( l_iExchanges % 2 )
		l_tScalar = -l_tScalar;
	return ( l_tScalar );
	M_EPILOG
	}

template<typename value_type>
value_type HMatrix<value_type>::M( int const a_iRow, int const a_iColumn )
	{
	M_PROLOG
	check_dimensions_square();
	int l_iCtrRow = 0, l_iCtrColumn = 0, l_iCtrRowVirtual = 0, l_iCtrColumnVirtual = 0;
	if ( a_iRow >= f_iRows )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::ROW_OUT_OF_RANGE ], a_iRow - f_iRows );
	if ( a_iColumn >= f_iColumns )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::COLUMN_OUT_OF_RANGE ], a_iColumn - f_iColumns );
	if ( f_iRows == 1 )
		return ( 0 );
	HMatrix l_oMatrix ( f_iRows - 1, f_iColumns - 1 );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++, l_iCtrColumnVirtual = 0 )
		{
		if ( l_iCtrRow == a_iRow )
			continue;
		for ( l_iCtrColumn = 0; l_iCtrColumn < f_iColumns; l_iCtrColumn ++ )
			{
			if ( l_iCtrColumn == a_iColumn )
				continue;
			l_oMatrix.f_oData[ l_iCtrRowVirtual ][ l_iCtrColumnVirtual ] = f_oData[ l_iCtrRow ][ l_iCtrColumn ];
			l_iCtrColumnVirtual ++;
			}
		l_iCtrRowVirtual ++;
		}
	return ( l_oMatrix.det() );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::T( void )
	{
	M_PROLOG
	int l_iCtrRow = 0, l_iCtrColumn = 0;
	HMatrix l_oMatrix ( f_iColumns, f_iRows );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		for ( l_iCtrColumn = 0; l_iCtrColumn < f_iColumns; l_iCtrColumn ++ ) 
			l_oMatrix.f_oData[ l_iCtrColumn ][ l_iCtrRow ] = f_oData[ l_iCtrRow ][ l_iCtrColumn ];
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::_1( void )
	{
	M_PROLOG
	check_dimensions_square();
	value_type l_tScalar = 0;
	int l_iCtrRow = 0, l_iCtrColumn = 0;
	if ( ! ( l_tScalar = det() ) )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::ODD ], 0 );
	HMatrix l_oMatrix( *this );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		{
		for ( l_iCtrColumn = 0; l_iCtrColumn < f_iColumns; l_iCtrColumn ++ )
			{
			l_oMatrix.f_oData[ l_iCtrRow ][ l_iCtrColumn ] = M( l_iCtrRow,
					l_iCtrColumn ) * ( ( ( l_iCtrRow + l_iCtrColumn ) % 2 ) ? -1 : 1 );
			}
		}
	return ( l_oMatrix.T() / l_tScalar );
	M_EPILOG
	}

template<typename value_type>
void HMatrix<value_type>::swap( HMatrix& a_roMatrix )
	{
	M_PROLOG
	if ( &a_roMatrix != this )
		{
		using yaal::swap;
		swap( f_iRows, a_roMatrix.f_iRows );
		swap( f_iColumns, a_roMatrix.f_iColumns );
		f_oData.swap( a_roMatrix.f_oData );
		}
	return;
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>& HMatrix<value_type>::operator = ( HMatrix const& a_roMatrix )
	{
	M_PROLOG
	if ( &a_roMatrix != this )
		{
		HMatrix<value_type> tmp( a_roMatrix );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
typename HMatrix<value_type>::row_t& HMatrix<value_type>::operator[] ( int const& idx )
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename value_type>
typename HMatrix<value_type>::row_t const& HMatrix<value_type>::operator[] ( int const& idx ) const
	{
	M_PROLOG
	return ( f_oData[ idx ] );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>& HMatrix<value_type>::operator = ( value_type const a_tScalar )
	{
	M_PROLOG
	for ( typename data_t::iterator it = f_oData.begin(); it != f_oData.end(); ++ it )
		yaal::fill( it->begin(), it->end(), a_tScalar );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator + ( HMatrix const& a_roMatrix ) const
	{
	M_PROLOG
	check_dimensions_rows_columns( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	HMatrix l_oMatrix ( *this );
	l_oMatrix += a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator - ( HMatrix const& a_roMatrix ) const
	{
	M_PROLOG
	check_dimensions_rows_columns( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	HMatrix l_oMatrix ( *this );
	l_oMatrix -= a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator - ( void ) const
	{
	M_PROLOG
	HMatrix<value_type> l_oMatrix( f_iRows, f_iColumns );
	l_oMatrix = 0;
	l_oMatrix -= ( *this );
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator * ( HMatrix const& a_roMatrix ) const
	{
	M_PROLOG
	check_dimensions_columns_rows( a_roMatrix.f_iRows );
	value_type l_tScalar = 0;
	int l_iCtrRow = 0, l_iCtrColumn = 0, l_iCtrRowColumn = 0;
	HMatrix l_oMatrix( f_iRows, a_roMatrix.f_iColumns );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ ) 
		for ( l_iCtrColumn = 0; l_iCtrColumn < a_roMatrix.f_iColumns; l_iCtrColumn ++, l_tScalar = 0 )
			{
			for ( l_iCtrRowColumn = 0; l_iCtrRowColumn < f_iColumns; l_iCtrRowColumn ++ ) 
				l_tScalar += ( f_oData[ l_iCtrRow ][ l_iCtrRowColumn ] * a_roMatrix.f_oData[ l_iCtrRowColumn ][ l_iCtrColumn ] );
			l_oMatrix.f_oData[ l_iCtrRow ][ l_iCtrColumn ] = l_tScalar;
			}
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator * ( value_type const a_tScalar ) const
	{
	M_PROLOG
	HMatrix l_oMatrix ( *this );
	l_oMatrix *= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator / ( value_type const a_tScalar ) const
	{
	M_PROLOG
	HMatrix l_oMatrix ( *this );
	l_oMatrix /= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> & HMatrix<value_type>::operator += ( HMatrix const& a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr ++ )
		f_oData[ l_iCtr ] += a_roMatrix.f_oData[ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> & HMatrix<value_type>::operator -= ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		f_oData[ l_iCtr ] -= a_roMatrix.f_oData[ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>& HMatrix<value_type>::operator *= ( HMatrix const& a_roMatrix )
	{
	M_PROLOG
	check_dimensions_columns_rows( a_roMatrix.f_iRows );
	if ( a_roMatrix.f_iRows != a_roMatrix.f_iColumns )
		M_THROW( g_ppcErrMsgHMatrix[ ERROR::DIM_NOT_MATCH_COLUMNS_ROWS_COLUMNS ],
				a_roMatrix.f_iRows - a_roMatrix.f_iColumns );
	( *this ) = ( *this ) * a_roMatrix;
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type>& HMatrix<value_type>::operator *= ( value_type const a_tScalar )
	{
	M_PROLOG
	for ( typename data_t::iterator it = f_oData.begin(); it != f_oData.end(); ++ it )
		yaal::transform( it->begin(), it->end(), it->begin(), bind2nd( yaal::multiplies<value_type>(), a_tScalar ) );
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> & HMatrix<value_type>::operator /= ( value_type const a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	if ( a_tScalar )
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			f_oData[ l_iCtr ] /= a_tScalar;
	return ( *this );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> HMatrix<value_type>::operator ~ ( void ) const
	{
	M_PROLOG
	int l_iCtr;
	HMatrix l_oMatrix( f_iRows, f_iColumns );
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		l_oMatrix [ l_iCtr ] = ~ f_oData[ l_iCtr ];
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
value_type HMatrix<value_type>::operator ! ( void ) const
	{
	M_PROLOG
	return ( det() );
	M_EPILOG
	}

template<typename value_type>
bool HMatrix<value_type>::operator == ( HMatrix const & a_roMatrix ) const
	{
	M_PROLOG
	check_dimensions_rows_columns ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) 
		if ( this->f_ptArray [ l_iCtr ] != a_roMatrix [ l_iCtr ] )
			return ( false );
	return ( true );
	M_EPILOG
	}

template<typename value_type>
bool HMatrix<value_type>::operator != ( HMatrix const & a_roMatrix ) const
	{
	M_PROLOG
	return ( ! ( * this == a_roMatrix ) );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> operator * ( HVector<value_type> const & a_roVector,
		HMatrix<value_type> const & a_roMatrix )
	{
	M_PROLOG
	value_type l_tScalar;
	int j, k;
	if ( a_roVector.dim() == a_roMatrix.f_iRows )
		{
		HVector<value_type> l_oVector ( a_roMatrix.f_iColumns );
		for ( j = 0; j < a_roMatrix.f_iColumns; j++ )
			{
			l_tScalar = 0;
			for ( k = 0; k < a_roMatrix.f_iColumns; k++ ) 
				l_tScalar += a_roVector [ k ] * a_roMatrix [ k ] [ j ];
			l_oVector [ j ] = l_tScalar;
			}
		return ( l_oVector );
		}
	return ( a_roVector );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> operator *= ( HVector<value_type> const& a_roVector,
		HMatrix<value_type> const& a_roMatrix )
	{
	M_PROLOG
	if ( a_roVector.dim() == a_roMatrix.f_iRows == a_roMatrix.f_iColumns )
		{
		HVector<value_type> l_oVector = a_roVector * a_roMatrix;
		a_roVector = l_oVector;
		}
	return ( a_roVector );
	M_EPILOG
	}

template<typename value_type>
HMatrix<value_type> operator * ( value_type const a_tScalar,
		HMatrix<value_type> const& a_roMatrix )
	{
	M_PROLOG
	HMatrix<value_type> l_oMatrix ( a_roMatrix );
	l_oMatrix *= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}

template<typename value_type>
HVector<value_type> T ( HMatrix<value_type> const& a_roMatrix,
		HVector<value_type> const& a_roVector )
	{
	M_PROLOG
	HMatrix<value_type> l_oMatrix = a_roMatrix.T();
	return ( a_roVector * l_oMatrix );
	M_EPILOG
	}

}

template<typename value_type> 
inline void swap( yaal::hcore::HMatrix<value_type>& a, yaal::hcore::HMatrix<value_type>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HMATRIX_HXX_INCLUDED */


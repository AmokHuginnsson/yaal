/*
---           `yaal' 0.0.0 (a_iColumn) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HMATRIX_H
#define __YAAL_HCORE_HMATRIX_H

#line 31

#include "hexception.h"
#include "hvector.h"

namespace yaal
{

namespace hcore
{

#define D_VCSID_HMATRIX_H "$Id$"

extern char const * const g_ppcErrMsgHMatrix [ ];

template < typename tType >
class HMatrix : public HArray < HVector < tType > >
	{
public:
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_BADROWS,
			E_BADCOLUMNS,
			E_NEWRETURNEDNULL,
			E_DIMNOTMATCH_ROWS,
			E_DIMNOTMATCH_COLUMNS,
			E_NOTASQUARE,
			E_ODD,
			E_DIMNOTMATCH_COLUMNSROWS,
			E_DIMNOTMATCH_COLUMNSROWSCOLUMNS,
			E_ROW_OUTOFRANGE,
			E_COLUMN_OUTOFRANGE
			} error_t;
		};
protected:
	/* { */
	int f_iRows;
	int f_iColumns;
	/* } */
public:
	/* { */
	HMatrix ( int const, int const );
	HMatrix ( HMatrix const & );
	virtual ~HMatrix ( void );
	int set ( tType const * * );
	int row ( void );
	int col ( void );
	tType det ( void );
	tType M ( int const, int const );
	HMatrix T ( void );
	HMatrix _1 ( void );
	HMatrix & operator = ( HMatrix const & );
	HMatrix & operator = ( tType const );
	HMatrix operator + ( HMatrix const & );
	HMatrix operator - ( HMatrix const & );
	HMatrix operator - ( void );
	HMatrix operator * ( HMatrix const & );
	HMatrix operator * ( tType const );
	HMatrix operator / ( tType const );
	HMatrix & operator += ( HMatrix const & );
	HMatrix & operator -= ( HMatrix const & );
	HMatrix & operator *= ( HMatrix const & );
	HMatrix & operator *= ( tType const );
	HMatrix & operator /= ( tType const );
	HMatrix operator ~ ( void );
	tType operator ! ( void );
	bool operator == ( HMatrix const & );
	bool operator != ( HMatrix const & );
template < typename ttType >
	friend HVector < ttType > operator * ( HVector < ttType > const &,
			HMatrix const & );
template < typename ttType >
	friend HVector < ttType > operator *= ( HVector < ttType > const &,
			HMatrix const & );
template < typename ttType >
	friend HMatrix operator * ( ttType const, HMatrix < ttType > const & );
	/* } */
private:
	inline void check_dimensions_columns_rows ( int a_iRowsAnother )
		{
		M_PROLOG
		if ( f_iColumns != a_iRowsAnother )
			M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_DIMNOTMATCH_COLUMNSROWS ],
					f_iColumns - a_iRowsAnother );
		return;
		M_EPILOG
		}
	inline void check_dimensions_rows_columns ( int a_iRowsAnother,
			int a_iColumnsAnother )
		{
		M_PROLOG
		if ( f_iRows != a_iRowsAnother )
			M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_DIMNOTMATCH_ROWS ],
					f_iRows - a_iRowsAnother );
		if ( f_iColumns != a_iColumnsAnother )
			M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_DIMNOTMATCH_COLUMNS ],
					f_iColumns - a_iColumnsAnother );
		return;
		M_EPILOG
		}
	inline void check_dimensions_square ( void )
		{
		M_PROLOG
		if ( f_iRows != f_iColumns )
			M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_NOTASQUARE ], f_iRows - f_iColumns );
		return;
		M_EPILOG
		}
	};
	
template < typename tType >
HVector < tType > T ( HMatrix < tType > const &, HVector < tType > const & );

template < typename tType >
HMatrix < tType > ::HMatrix ( int const a_iRows, int const a_iColumns )
									:	HArray < HVector < tType > > ( a_iRows,
											HVector < tType > ( a_iColumns ) )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_iRows < 1 )
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_BADROWS ], a_iRows );
	else
		f_iRows = a_iRows;
	if ( a_iColumns < 1 )
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_BADCOLUMNS ], a_iColumns );
	else
		f_iColumns = a_iColumns;
	return ;
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > ::HMatrix ( HMatrix const & a_roMatrix )
									:	HArray < HVector < tType > > ( a_roMatrix.f_iRows,
											HVector < tType > ( a_roMatrix.f_iColumns ) )
	{
	M_PROLOG
	f_iRows = a_roMatrix.f_iRows;
	f_iColumns = a_roMatrix.f_iColumns;
	( * this ) = a_roMatrix;
	return;
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > :: ~ HMatrix ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}
	
template < typename tType >
int HMatrix < tType > ::set ( tType const * * a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		this->f_ptArray [ l_iCtr ]->set ( a_tScalar [ l_iCtr ] );
	return ( 0 );
	M_EPILOG
	}
	
template < typename tType >
int HMatrix < tType > ::row ( void )
	{
	M_PROLOG
	return ( f_iRows );
	M_EPILOG
	}
	
template < typename tType >
int HMatrix < tType > ::col ( void )
	{
	M_PROLOG
	return ( f_iColumns );
	M_EPILOG
	}
	
template < typename tType >
tType HMatrix < tType > ::det ( void )
	{
	M_PROLOG
	check_dimensions_square ( );
	int l_iCtrLocRow = 0, l_iCtrRow = 0, l_iExchanges = 0;
	tType l_tScalar;
	HMatrix < tType > l_oMatrix ( * this );
	HVector < tType > l_oVector ( f_iColumns );
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
	
template < typename tType >
tType HMatrix < tType > ::M ( int const a_iRow, int const a_iColumn )
	{
	M_PROLOG
	check_dimensions_square ( );
	int l_iCtrRow = 0, l_iCtrColumn = 0, l_iCtrRowVirtual = 0, l_iCtrColumnVirtual = 0;
	if ( a_iRow >= f_iRows )
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_ROW_OUTOFRANGE ], a_iRow - f_iRows );
	if ( a_iColumn >= f_iColumns )
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_COLUMN_OUTOFRANGE ], a_iColumn - f_iColumns );
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
			l_oMatrix.f_ptArray [ l_iCtrRowVirtual ] [ l_iCtrColumnVirtual ] = this->f_ptArray [ l_iCtrRow ] [ l_iCtrColumn ];
			l_iCtrColumnVirtual ++;
			}
		l_iCtrRowVirtual ++;
		}
	return ( l_oMatrix.det ( ) );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::T ( void )
	{
	M_PROLOG
	int l_iCtrRow = 0, l_iCtrColumn = 0;
	HMatrix l_oMatrix ( f_iColumns, f_iRows );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
		for ( l_iCtrColumn = 0; l_iCtrColumn < f_iColumns; l_iCtrColumn ++ ) 
			l_oMatrix.f_ptArray [ l_iCtrColumn ] [ l_iCtrRow ] = this->f_ptArray [ l_iCtrRow ] [ l_iCtrColumn ];
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::_1 ( void )
	{
	M_PROLOG
	check_dimensions_square ( );
	tType l_tScalar = 0;
	int l_iCtrRow = 0, l_iCtrColumn = 0;
	if ( ( l_tScalar = det ( ) ) != 0 )
		{
		HMatrix l_oMatrix ( * this );
		for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ )
			for ( l_iCtrColumn = 0; l_iCtrColumn < f_iColumns; l_iCtrColumn ++ )
				l_oMatrix.f_ptArray [ l_iCtrRow ] [ l_iCtrColumn ] = M ( l_iCtrRow,
						l_iCtrColumn ) * ( ( ( l_iCtrRow + l_iCtrColumn ) % 2 ) ? -1 : 1 );
		return ( l_oMatrix.T ( ) / l_tScalar );
		}
	else
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_ODD ], 0 );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator = ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( & a_roMatrix == this )
		return ( * this );
	if ( ( f_iRows > 0 ) || ( f_iColumns > 0 ) )
		check_dimensions_rows_columns ( a_roMatrix.f_iRows,
				a_roMatrix.f_iColumns );
	( * this ).HArray < HVector < tType > > ::operator = ( a_roMatrix );
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator = ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		( * this->f_ptArray  ) [ l_iCtr ] = a_tScalar;
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator + ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	HMatrix l_oMatrix ( * this );
	l_oMatrix += a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator - ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	HMatrix l_oMatrix ( * this );
	l_oMatrix -= a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator - ( void )
	{
	M_PROLOG
	HMatrix < tType > l_oMatrix ( f_iRows, f_iColumns );
	l_oMatrix = 0;
	l_oMatrix -= ( * this );
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator * ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_columns_rows ( a_roMatrix.f_iRows );
	tType l_tScalar = 0;
	int l_iCtrRow = 0, l_iCtrColumn = 0, l_iCtrRowColumn = 0;
	HMatrix l_oMatrix ( f_iRows, a_roMatrix.f_iColumns );
	for ( l_iCtrRow = 0; l_iCtrRow < f_iRows; l_iCtrRow ++ ) 
		for ( l_iCtrColumn = 0; l_iCtrColumn < a_roMatrix.f_iColumns; l_iCtrColumn ++, l_tScalar = 0 )
			{
			for ( l_iCtrRowColumn = 0; l_iCtrRowColumn < f_iColumns; l_iCtrRowColumn ++ ) 
				l_tScalar += ( this->f_ptArray [ l_iCtrRow ] [ l_iCtrRowColumn ] * a_roMatrix.f_ptArray [ l_iCtrRowColumn ] [ l_iCtrColumn ] );
			l_oMatrix.f_ptArray [ l_iCtrRow ] [ l_iCtrColumn ] = l_tScalar;
			}
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator * ( tType const a_tScalar )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix *= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator / ( tType const a_tScalar )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix /= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator += ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr ++ )
		this->f_ptArray [ l_iCtr ] += a_roMatrix.f_ptArray [ l_iCtr ];
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator -= ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_rows_columns ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		* this->f_ptArray [ l_iCtr ] -= a_roMatrix [ l_iCtr ];
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator *= ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	check_dimensions_columns_rows ( a_roMatrix.f_iRows );
	if ( a_roMatrix.f_iRows != a_roMatrix.f_iColumns )
		M_THROW ( g_ppcErrMsgHMatrix [ ERROR::E_DIMNOTMATCH_COLUMNSROWSCOLUMNS ],
				a_roMatrix.f_iRows - a_roMatrix.f_iColumns );
	( * this ) = ( * this ) * a_roMatrix;
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator *= ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		this->f_ptArray [ l_iCtr ] *= a_tScalar;
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > & HMatrix < tType > ::operator /= ( tType const a_tScalar )
	{
	M_PROLOG
	int l_iCtr;
	if ( a_tScalar )
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			this->f_ptArray [ l_iCtr ] /= a_tScalar;
	return ( * this );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > HMatrix < tType > ::operator ~ ( void )
	{
	M_PROLOG
	int l_iCtr;
	HMatrix l_oMatrix ( f_iRows, f_iColumns );
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		l_oMatrix [ l_iCtr ] = ~ this->f_ptArray [ l_iCtr ];
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
tType HMatrix < tType > ::operator ! ( void )
	{
	M_PROLOG
	return ( det ( ) );
	M_EPILOG
	}
	
template < typename tType >
bool HMatrix < tType > ::operator == ( HMatrix const & a_roMatrix )
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
	
template < typename tType >
bool HMatrix < tType > ::operator != ( HMatrix const & a_roMatrix )
	{
	M_PROLOG
	return ( ! ( * this == a_roMatrix ) );
	M_EPILOG
	}
	
template < typename tType >
HVector < tType > operator * ( HVector < tType > const & a_roVector,
		HMatrix < tType > const & a_roMatrix )
	{
	M_PROLOG
	tType l_tScalar;
	int j, k;
	if ( a_roVector.dim ( ) == a_roMatrix.f_iRows )
		{
		HVector < tType > l_oVector ( a_roMatrix.f_iColumns );
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
	
template < typename tType >
HVector < tType > operator *= ( HVector < tType > const & a_roVector,
		HMatrix < tType > const & a_roMatrix )
	{
	M_PROLOG
	if ( a_roVector.dim ( ) == a_roMatrix.f_iRows == a_roMatrix.f_iColumns )
		{
		HVector < tType > l_oVector = a_roVector * a_roMatrix;
		a_roVector = l_oVector;
		}
	return ( a_roVector );
	M_EPILOG
	}
	
template < typename tType >
HMatrix < tType > operator * ( tType const a_tScalar,
		HMatrix < tType > const & a_roMatrix )
	{
	M_PROLOG
	HMatrix < tType > l_oMatrix ( a_roMatrix );
	l_oMatrix *= a_tScalar;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < typename tType >
HVector < tType > T ( HMatrix < tType > const & a_roMatrix,
		HVector < tType > const & a_roVector )
	{
	M_PROLOG
	HMatrix < tType > l_oMatrix = a_roMatrix.T ( );
	return ( a_roVector * l_oMatrix );
	M_EPILOG
	}

}

}
	
#endif /* not __YAAL_HCORE_HMATRIX_H */


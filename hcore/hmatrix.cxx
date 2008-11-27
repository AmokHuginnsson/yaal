/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hmatrix.h"
M_VCSTID ( D_VCSID_HMATRIX_H )

namespace yaal
{

namespace hcore
{

char const * const g_ppcErrMsgHMatrix [ 12 ] =
	{
	_( "ok" ),
/* HMatrix::ERROR::E_BAD_ROWS */                           _( "rows < 1" ),
/* HMatrix::ERROR::E_BAD_COLUMNS */                        _( "columns < 1" ),
/* HMatrix::ERROR::E_DIM_NOT_MATCH_ROWS */                 _( "rows dimensions do not match" ),
/* HMatrix::ERROR::E_DIM_NOT_MATCH_COLUMNS */              _( "columns dimensions do not match" ),
/* HMatrix::ERROR::E_NOT_A_SQUARE */                       _( "matrix is not square" ),
/* HMatrix::ERROR::E_ODD */                                _( "matrix is odd" ),
/* HMatrix::ERROR::E_DIM_NOT_MATCH_COLUMNS_ROWS */         _( "columns does not match rows" ),
/* HMatrix::ERROR::E_DIM_NOT_MATCH_COLUMNS_ROWS_COLUMNS */ _( "columns does not match rows and columns" ),
/* HMatrix::ERROR::E_ROW_OUT_OF_RANGE */                   _( "row too big for minor" ),
/* HMatrix::ERROR::E_COLUMN_OUT_OF_RANGE */                _( "column too big for minor" )
	};

}

}


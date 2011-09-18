/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hmatrix.hxx"

namespace yaal {

namespace hcore {

char const * const _errMsgHMatrix_ [ 12 ] = {
	_( "ok" ),
/* HMatrix::ERROR::BAD_ROWS */                           _( "rows < 1" ),
/* HMatrix::ERROR::BAD_COLUMNS */                        _( "columns < 1" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_ROWS */                 _( "rows dimensions do not match" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_COLUMNS */              _( "columns dimensions do not match" ),
/* HMatrix::ERROR::NOT_A_SQUARE */                       _( "matrix is not square" ),
/* HMatrix::ERROR::ODD */                                _( "matrix is odd" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_COLUMNS_ROWS */         _( "columns does not match rows" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_COLUMNS_ROWS_COLUMNS */ _( "columns does not match rows and columns" ),
/* HMatrix::ERROR::ROW_OUT_OF_RANGE */                   _( "row too big for minor" ),
/* HMatrix::ERROR::COLUMN_OUT_OF_RANGE */                _( "column too big for minor" )
};

}

}


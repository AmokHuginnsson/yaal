/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.cxx - this file is integral part of `yaal' project.

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


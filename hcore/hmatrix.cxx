/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmatrix.hxx"

namespace yaal {

namespace hcore {

char const * const _errMsgHMatrix_ [ 11 ] = {
	_( "ok" ),
/* HMatrix::ERROR::BAD_ROWS */                           _( "rows < 1" ),
/* HMatrix::ERROR::BAD_COLUMNS */                        _( "columns < 1" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_ROWS */                 _( "rows dimensions do not match" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_COLUMNS */              _( "columns dimensions do not match" ),
/* HMatrix::ERROR::NOT_A_SQUARE */                       _( "matrix is not square" ),
/* HMatrix::ERROR::ODD */                                _( "matrix is odd" ),
/* HMatrix::ERROR::DIM_NOT_MATCH_COLUMNS_ROWS */         _( "columns does not match rows" ),
/* HMatrix::ERROR::ROW_OUT_OF_RANGE */                   _( "row too big for minor" ),
/* HMatrix::ERROR::COLUMN_OUT_OF_RANGE */                _( "column too big for minor" ),
/* HMatrix::ERROR::DIVISION_BY_ZERO */                   _( "division by 0" )
};

}

}


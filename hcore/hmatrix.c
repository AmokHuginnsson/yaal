/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.c - this file is integral part of `stdhapi' project.

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

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hmatrix.h"
M_CVSTID ( D_CVSID_HMATRIX_H );

const char * g_ppcErrMsgHMatrix [ 24 ] =
	{
/* E_BADROWS */													_ ( "rows < 1" ),
/* E_BADCOLUMNS */											_ ( "columns < 1" ),
/* E_NEWRETURNEDNULL */									_ ( "new returned NULL" ),
/* E_DIMNOTMATCH_ROWS */								_ ( "rows dimensions do not match" ),
/* E_DIMNOTMATCH_COLUMNS */							_ ( "columns dimensions do not match" ),
/* E_NOTASQUARE */											_ ( "matrix is not square" ),
/* E_ODD */															_ ( "matrix is odd" ),
/* E_DIMNOTMATCH_COLUMNSROWS */					_ ( "columns does not match rows" ),
/* E_DIMNOTMATCH_COLUMNSROWSCOLUMNS */	_ ( "columns does not match rows and columns" ),
/* E_ROW_OUTOFRANGE */									_ ( "row too big for minor" ),
/* E_COLUMN_OUTOFRANGE */								_ ( "column too big for minor" )
	};


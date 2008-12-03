/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpool.cxx - this file is integral part of `yaal' project.

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

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hpool.hxx"

M_VCSTID ( D_VCSID_HPOOL_H )

namespace yaal
{

namespace hcore
{

char const* n_ppcErrMsgHPool[ 5 ] =
	{
	_( "ok" ),
/* HPool::ERROR::E_BAD_SIZE */      _( "bad size" ),
/* HPool::ERROR::E_REALLOC_FIXED */ _( "can not realloc fixed size pool" ),
/* HPool::ERROR::E_BAD_INDEX */			_( "index does not fit into array" )
	};

}

}


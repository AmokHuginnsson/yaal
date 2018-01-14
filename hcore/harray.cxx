/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "harray.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )

namespace yaal {

namespace hcore {

char const* const _errMsgHArray_[ 4 ] = {
	_( "ok" ),
/* HArray::ERROR::BAD_SIZE */  _( "bad size requested" ),
/* HArray::ERROR::BAD_INDEX */ _( "index does not fit into array" ),
/* HArray::ERROR::INVALID_ITERATOR */ _( "invalid iterator used for operation" )
};

}

}


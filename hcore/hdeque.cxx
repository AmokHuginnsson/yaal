/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdeque.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHDeque_[ 4 ] = {
	_( "ok" ),
/* HDeque::ERROR::BAD_SIZE */         _( "bad size requested" ),
/* HDeque::ERROR::BAD_INDEX */        _( "index does not fit into deque" ),
/* HDeque::ERROR::INVALID_ITERATOR */ _( "invalid iterator used for operation" )
};

}

}


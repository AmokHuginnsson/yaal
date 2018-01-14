/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hring.hxx"

namespace yaal {

namespace tools {

char const* const _errMsgHRing_[ 6 ] = {
	_( "ok" ),
/* HRing::ERROR::BAD_SIZE */         _( "bad size requested" ),
/* HRing::ERROR::BAD_INDEX */        _( "index does not fit into ring" ),
/* HRing::ERROR::INVALID_ITERATOR */ _( "invalid iterator used for operation" ),
/* HRing::ERROR::RING_IS_FULL */     _( "ring is already full" ),
/* HRing::ERROR::RING_IS_EMPTY */    _( "ring is now empty" )
};

}

}


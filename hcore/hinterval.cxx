/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hinterval.hxx"

namespace yaal {

namespace hcore {

char const* _errMsgHInterval_[ 3 ] = {
	_( "ok" ),
/* HInterval::ERROR::REVERSED_ENDPOINTS */ _( "reversed endpoints" ),
/* HInterval::ERROR::DIVISION_BY_ZERO */   _( "division by zero" )
};

}

}


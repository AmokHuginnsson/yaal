/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hvector.hxx"

namespace yaal {

namespace hcore {

char const* _errMsgHVector_[ 3 ] = {
	_( "ok" ),
/* HVector::ERROR::DIMNOTMATCH */	_( "dimensions do not match" ),
/* HVector::ERROR::DIVISION_BY_ZERO */	_( "division by 0" )
};

}

}


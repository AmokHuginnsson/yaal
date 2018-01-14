/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmap.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHMap_[ 4 ] = {
	_( "ok" ),
	/* HMap::ERROR::INVALID_KEY */  _( "Dereferencing non-existing key" )
};

}

}


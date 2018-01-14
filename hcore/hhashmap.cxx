/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhashmap.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHHashMap_[ 4 ] = {
	_( "ok" ),
	/* HHashMap::ERROR::INVALID_KEY */  _( "Dereferencing non-existing key" )
};

}

}


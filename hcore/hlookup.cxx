/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlookup.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHLookup_[ 4 ] = {
	_( "ok" ),
/* HLookup::NON_EXISTING_KEY */ _( "key does not exists" ),
/* HLookup::NIL_ITERATOR */     _( "dereferencing nil iterator" )
};

}

}


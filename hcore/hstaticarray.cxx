/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstaticarray.hxx"

namespace yaal {

namespace hcore {

char const* const _errMsgHStaticArray_[ 4 ] = {
	_( "ok" ),
/* HStaticArray::ERROR::BAD_INDEX */ _( "index does not fit into static array" ),
/* HStaticArray::ERROR::BAD_SEQUENCE_SIZE */ _( "Paired sequence size is invalid." ),
/* HStaticArray::ERROR::COUNT_NOT_MATCH */ _( "Specified count does not match with container size." )
};

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hauxiliarybuffer.hxx"
#include "hcore/system.hxx"

namespace yaal {

namespace hcore {

namespace auxiliary_buffer {

i64_t available_memory( void ) {
	return ( hcore::system::get_memory_size_info().available() );
}

}

}

}


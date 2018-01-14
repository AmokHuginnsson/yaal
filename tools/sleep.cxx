/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cerrno>
#include <cstdlib>
#include <sys/time.h> /* timeval (for FreeBSD) */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "sleep.hxx"
#include "hcore/hclock.hxx"
#include "hcore/si.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

bool sleep_for( yaal::hcore::time::duration_t duration_, bool ignoreInterrupts_ ) {
	HClock c;
	int err( 0 );
	timeval wait{ 0, 0 };
	do {
		i64_t left( duration_.get() - c.get_time_elapsed( time::UNIT::NANOSECOND ) );
		if ( left <= 0 ) {
			break;
		}
		wait = {
			static_cast<time_t>( left / si::NANO_IN_WHOLE ),
			static_cast<suseconds_t>( ( left % si::NANO_IN_WHOLE ) / si::NANO_IN_MICRO )
		};
	} while ( ( ( err = ::select( 0, nullptr, nullptr, nullptr, &wait ) ) == -1 ) && ( errno == EINTR ) && ignoreInterrupts_ );
	return ( ( c.get_time_elapsed( time::UNIT::NANOSECOND ) >= duration_ ) && ( err == 0 ) );
}

}

}


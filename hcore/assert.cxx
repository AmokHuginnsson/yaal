/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "assert.hxx"
#include "algorithm_low.hxx"
#include "introspect.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

extern FILE* ERROR_STREAM;

void failed_assert( char const* fileName_, int line_, char const* functionName_, char const* message_ ) {
	M_PROLOG
	hcore::log << "Failed assertion: " << message_ << " -> " << fileName_ << "(" << line_ << "): " << functionName_ << endl;
	fprintf( ERROR_STREAM, "Failed assertion: `%s' at: %s:%d: %s\n", message_, fileName_, line_, functionName_ );
	if ( ! errno ) {
		++ errno;
	}
	if ( _debugLevel_ >= DEBUG_LEVEL::ABORT_ON_ASSERT ) {
		::abort();
	}
	static int const DUMP_DEPTH = 64;
	dump_call_stack( clog, DUMP_DEPTH );
	throw ( HFailedAssertion( message_ ) );
	M_EPILOG
}

}

}


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	assert.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "assert.hxx"
#include "algorithm_low.hxx"
#include "introspect.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

extern FILE* ERROR_STREAM;

void HFailedAssertion::swap( HFailedAssertion& other ) {
	using yaal::swap;
	if ( &other != this )
		swap( _what, other._what );
	return;
}

void failed_assert( char const* const fileName_,
		int line_, char const* const functionName_,
		char const* const message_ ) {
	M_PROLOG
	hcore::log << "Failed assertion: " << message_ << " -> " << fileName_ << "(" << line_ << "): " << functionName_ << endl;
	fprintf( ERROR_STREAM, "Failed assertion: `%s' at: %s: %4d : %s\n",
			message_, fileName_, line_, functionName_ );
	if ( ! errno )
		++ errno;
	if ( _debugLevel_ >= DEBUG_LEVEL::ABORT_ON_ASSERT )
		::abort();
	static int const DUMP_DEPTH = 64;
	dump_call_stack( clog, DUMP_DEPTH );
	throw ( HFailedAssertion( message_ ) );
	M_EPILOG
}

}

}


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  assert.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
	fprintf( ERROR_STREAM, "Failed assertion: `%s' at: %s:%d: %s\n",
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


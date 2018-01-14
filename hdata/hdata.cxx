/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/hcore.hxx"
#include "hconsole/hconsole.hxx"
#include "tools/tools.hxx"
#include "dbwrapper/dbwrapper.hxx"
#include "hdata.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

void banner( void ) {
	::printf( "\thdata\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_hdata_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_hdata_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		yaal::tools::banner();
		yaal::dbwrapper::banner();
		yaal::hconsole::banner();
		yaal::hdata::banner();
		::printf( "\n" );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_hdata_main( int, char** );
extern "C"
int yaal_hdata_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */


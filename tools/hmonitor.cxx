/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmonitor.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HMonitor::HMonitor( void )
	: _mutexes(), _mutex() {
	return;
}

HMonitor::~HMonitor( void ) {
}

HLock HMonitor::acquire( HString const& name_ ) {
	M_PROLOG
	HMutex* m( nullptr );
	/* scope for mutexes' container mutex */ {
		HLock l( _mutex );
		mutexes_t::iterator it( _mutexes.find( name_ ) );
		if ( it == _mutexes.end() ) {
			it = _mutexes.insert( make_pair( name_, make_pointer<HMutex>() ) ).first;
		}
		m = it->second.raw();
	}
	return ( HLock( ref( *m ) ) );
	M_EPILOG
}

int HMonitor::life_time( int ) {
	return ( 95 );
}

}

}


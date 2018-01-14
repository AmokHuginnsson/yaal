/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hthreadpool.hxx"
#include "hcore/system.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HThreadPool::HThreadPool( void )
	: HWorkFlow( system::get_core_count_info() ) {
	return;
}

HThreadPool::~HThreadPool( void ) {
	M_PROLOG
	stop();
	M_DESTRUCTOR_EPILOG
}

void HThreadPool::stop( void ) {
	M_PROLOG
	_isKilled_ = true;
	if ( state() == STATE::RUNNING ) {
		windup( WINDUP_MODE::ABORT );
	}
	M_ASSERT( state() == STATE::CLOSED );
	return;
	M_EPILOG
}

}

}


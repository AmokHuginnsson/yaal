/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/halarm.hxx
 * \brief Declaration of HAlarm class.
 */

#ifndef YAAL_TOOLS_HALARM_HXX_INCLUDED
#define YAAL_TOOLS_HALARM_HXX_INCLUDED 1

#include <ctime>

#include "hcore/compat.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hthread.hxx"

namespace yaal {

namespace tools {

/*! \brief HAlarm class provides automatic registration and deregistration of system alerts.
 */
class HAlarm final {
public:
	typedef HAlarm this_type;
	typedef i64_t timer_handle_t;
private:
	timer_handle_t _timer;
	yaal::hcore::HChunk _action;
	yaal::hcore::HLock _lock;
	M_YAAL_TOOLS_PUBLIC_API static yaal::hcore::HMutex _mutex;
public:
	/*! \brief Register an alarm.
	 *
	 * milliseconds_ - alarm shall come in milliseconds_ milliseconds.
	 */
	HAlarm( i64_t milliseconds_ );

	/*! \brief Deregister alarm if it has not came.
	 */
	~HAlarm( void );
private:
	HAlarm( HAlarm const& );
	HAlarm& operator = ( HAlarm const& );
	void cleanup_sigsetup( int );
};

typedef yaal::hcore::HExceptionT<HAlarm> HAlarmException;

}

}

#endif /* #ifndef YAAL_TOOLS_HALARM_HXX_INCLUDED */


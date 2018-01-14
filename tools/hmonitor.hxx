/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hmonitor.hxx
 * \brief Declaration of HMonitor class.
 */

#ifndef YAAL_TOOLS_HMONITOR_HXX_INCLUDED
#define YAAL_TOOLS_HMONITOR_HXX_INCLUDED 1

#include "hcore/hhashmap.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

/*! \brief Named critical section manager.
 */
class M_YAAL_TOOLS_PUBLIC_API HMonitor : public yaal::hcore::HSingleton<HMonitor> {
public:
	typedef HMonitor this_type;
	typedef yaal::hcore::HSingleton<HMonitor> base_type;
private:
	typedef yaal::hcore::HPointer<yaal::hcore::HMutex> mutex_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, mutex_t> mutexes_t;
	mutexes_t _mutexes;
	yaal::hcore::HMutex _mutex;
public:
	yaal::hcore::HLock acquire( yaal::hcore::HString const& );
private:
	HMonitor( void );
	virtual ~HMonitor( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HMonitor>;
	friend class yaal::hcore::HDestructor<HMonitor>;
};

typedef yaal::hcore::HExceptionT<HMonitor> HMonitorException;

}

}

#endif /* #ifndef YAAL_TOOLS_HMONITOR_HXX_INCLUDED */


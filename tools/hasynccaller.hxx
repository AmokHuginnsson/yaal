/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED
#define YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {


/*! \brief Invoke function or method asynchronously.
 */
class M_YAAL_TOOLS_PUBLIC_API HAsyncCaller : public yaal::hcore::HSingleton<HAsyncCaller> {
public:
	typedef HAsyncCaller this_type;
	typedef int long priority_t;
	typedef yaal::hcore::HBoundCall<> call_t;
	void register_call( priority_t, call_t );
	static int life_time( int );
private:
	typedef yaal::hcore::HMultiMap<priority_t, call_t> queue_t;
	queue_t _queue;
	int _stopRequests;
	yaal::hcore::HSemaphore _semaphore;
	yaal::hcore::HMutex _mutex;
	HAsyncCaller( void );
	virtual ~HAsyncCaller( void );
	void run( void );
	void stop( void );
	bool want_restart( void ) const;
	friend class yaal::hcore::HSingleton<HAsyncCaller>;
	friend class yaal::hcore::HDestructor<HAsyncCaller>;
};

typedef yaal::hcore::HExceptionT<HAsyncCaller> HAsyncCallerException;

}

}

#endif /* #ifndef YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED */


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED
#define YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

/*! \brief Task scheduler.
 *
 * HScheduledAsyncCaller is capable of invocation of any method of any class
 * at precisely specified moment in time.
 */
class M_YAAL_TOOLS_PUBLIC_API HScheduledAsyncCaller : public yaal::hcore::HSingleton<HScheduledAsyncCaller> {
public:
	typedef HScheduledAsyncCaller this_type;
	typedef yaal::i64_t time_point_t;
	typedef yaal::hcore::HBoundCall<> call_t;
private:
	typedef yaal::hcore::HMultiMap<time_point_t, call_t> queue_t;
	queue_t _queue;
	yaal::hcore::HMutex _mutex;
	yaal::hcore::HCondition _condition;
	bool _loop;
public:
	void call_at( yaal::hcore::HTime const&, call_t );
	void call_in( yaal::hcore::time::duration_t, call_t );
	void flush( void* );
private:
	void run( void );
	void stop( void );
	bool want_restart( void ) const;
	HScheduledAsyncCaller( void );
	virtual ~HScheduledAsyncCaller( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HScheduledAsyncCaller>;
	friend class yaal::hcore::HDestructor<HScheduledAsyncCaller>;
};

typedef yaal::hcore::HExceptionT<HScheduledAsyncCaller> HScheduledAsyncCallerException;

}

}

#endif /* #ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED */


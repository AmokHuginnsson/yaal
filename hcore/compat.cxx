/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>

#include "compat.hxx"

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 ) \
	|| ! defined( HAVE_DECL_EXPL )    || ( HAVE_DECL_EXPL    == 0 ) \
	|| ! defined( HAVE_DECL_LOGL )    || ( HAVE_DECL_LOGL    == 0 ) \
	|| ! defined( HAVE_DECL_SINL )    || ( HAVE_DECL_SINL    == 0 ) \
	|| ! defined( HAVE_DECL_COSL )    || ( HAVE_DECL_COSL    == 0 ) \
	|| ! defined( HAVE_DECL_TANL )    || ( HAVE_DECL_TANL    == 0 ) \
	|| ! defined( HAVE_DECL_ASINL )   || ( HAVE_DECL_ASINL   == 0 ) \
	|| ! defined( HAVE_DECL_ACOSL )   || ( HAVE_DECL_ACOSL   == 0 ) \
	|| ! defined( HAVE_DECL_ATANL )   || ( HAVE_DECL_ATANL   == 0 ) \
	|| ! defined( HAVE_DECL_SINCOSL ) || ( HAVE_DECL_SINCOSL == 0 ) \
	|| ! defined( HAVE_DECL_FMODL )   || ( HAVE_DECL_FMODL   == 0 ) \
	|| ! defined( HAVE_DECL_FLOORL )  || ( HAVE_DECL_FLOORL  == 0 ) \
	|| ! defined( HAVE_DECL_CEILL )   || ( HAVE_DECL_CEILL   == 0 ) \
	|| ! defined( HAVE_DECL_ROUNDL )  || ( HAVE_DECL_ROUNDL  == 0 )
#include <cmath>
#endif /* not HAVE_DECL_(POWL|EXPL|LOGL|SINL|TANL|ASINL|ACOSL|ATANL|SINCOSL|FMODL|FLOORL|CEILL|ROUNDL) */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) || ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
#include <cstring>
#endif /* #if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) || ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if ! defined( HAVE_BASENAME_ARG_CONST )
#	undef basename
#	ifndef _LIBGEN_H_
#		ifndef HAVE_BASENAME_IN_CSTRING
#			ifdef HAVE_LIBGEN_H
#				include <libgen.h>
#			endif /* #ifdef HAVE_LIBGEN_H */
#		endif /* not HAVE_BASENAME_IN_CSTRING */
#	endif /* #	ifndef _LIBGEN_H_ */
char const* basename_nc_impl( char const* x ) {
	return ( basename( const_cast<char*>( x ) ) );
}
#endif /* #if ! defined( HAVE_BASENAME_ARG_CONST ) */

#if ( defined( HAVE_GETHOSTBYNAME_R ) && ( ! defined( HAVE_GNU_GETHOSTBYNAME_R ) ) ) || ( defined( HAVE_GETHOSTBYADDR_R ) && ( ! defined( HAVE_GNU_GETHOSTBYADDR_R ) ) )
#include <cerrno>
#endif /* ( HAVE_GETHOSTBYNAME_R && not HAVE_GNU_GETHOSTBYNAME_R ) || ( HAVE_GETHOSTBYADDR_R && not HAVE_GNU_GETHOSTBYADDR_R ) */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
#include "hstring.hxx"
#endif /* not HAVE_STRCASESTR */

#include "base.hxx"

M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const* ptr, int what, int long unsigned from ) {
	char const* str( static_cast<char const*>( ptr ) );
	bool found( false );
	while ( from -- > 0 ) {
		if ( str[ from ] == what ) {
			found = true;
			break;
		}
	}
	return ( found ? const_cast<char*>( str + from ) : nullptr );
}
#endif /* not HAVE_MEMRCHR */

#if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
int long strnlen( char const* str_, int long maxLen_ ) {
	char const* end( static_cast<char const*>( ::memchr( str_, 0, static_cast<size_t>( maxLen_ ) ) ) );
	return ( end ? end - str_ : maxLen_ );
}
#endif /* #if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if defined( HAVE_GETHOSTBYNAME_R )
#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
#include <cstdlib>
#include <netdb.h>
int gethostbyname_r( char const* a0, struct hostent* a1, char* a2, long unsigned a3, struct hostent** a4, int* a5 ) {
	hostent* h = ::gethostbyname_r( a0, a1, a2, static_cast<int>( a3 ), a5 );
	if ( a4 ) {
		*a4 = h;
	}
	return ( h ? 0 : errno );
}
#endif /* not HAVE_GNU_GETHOSTBYNAME_R */
#endif /* HAVE_GETHOSTBYNAME_R */

#if defined( HAVE_GETHOSTBYADDR_R )
#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
#include <cstdlib>
#include <netdb.h>
int gethostbyaddr_r( void const* a0, int a1, int a2, struct hostent* a3, char* a4, long unsigned a5, struct hostent** a6, int* a7 ) {
	hostent* h = ::gethostbyaddr_r( static_cast<char const*>( a0 ), a1, a2, a3, a4, static_cast<int>( a5 ), a7 );
	if ( a6 )
		*a6 = h;
	return ( h ? 0 : errno );
}
#endif /* not HAVE_GNU_GETHOSTBYADDR_R */
#endif /* HAVE_GETHOSTBYADDR_R */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long a, double long b ) {
	return ( ::std::pow( a, b ) );
}
#endif /* not HAVE_POWL */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long x ) {
	return ( ::std::floor( x ) );
}
#endif /* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_DECL_CEILL ) || ( HAVE_DECL_CEILL == 0 )
double long ceill( double long x ) {
	return ( ::std::ceil( x ) );
}
#endif /* not HAVE_DECL_CEILL */

#if ! defined( HAVE_DECL_ROUNDL ) || ( HAVE_DECL_ROUNDL == 0 )
double long roundl( double long x ) {
	return ( ::round( static_cast<double>( x ) ) );
}
#endif /* not HAVE_DECL_ROUNDL */

#if ! defined( HAVE_DECL_EXPL ) || ( HAVE_DECL_EXPL == 0 )
double long expl( double long x ) {
	return ( ::std::exp( static_cast<double>( x ) ) );
}
#endif /* not HAVE_DECL_EXPL */

#if ! defined( HAVE_DECL_LOGL ) || ( HAVE_DECL_LOGL == 0 )
double long logl( double long x ) {
	return ( ::std::log( static_cast<double>( x ) ) );
}
#endif /* not HAVE_DECL_LOGL */

#if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 )
double long sqrtl( double long x ) {
	return ( ::std::sqrt( x ) );
}
#endif /* not HAVE_DECL_SQRTL */

#if ! defined( HAVE_DECL_SINL ) || ( HAVE_DECL_SINL == 0 )
double long sinl( double long x ) {
	return ( sin( static_cast<double>( x ) ) );
}
#endif /* #if ! defined( HAVE_DECL_SINL ) || ( HAVE_DECL_SINL == 0 ) */

#if ! defined( HAVE_DECL_COSL ) || ( HAVE_DECL_COSL == 0 )
double long cosl( double long x ) {
	return ( cos( static_cast<double>( x ) ) );
}
#endif /* #if ! defined( HAVE_DECL_COSL ) || ( HAVE_DECL_COSL == 0 ) */

#if ! defined( HAVE_DECL_TANL ) || ( HAVE_DECL_TANL == 0 )
double long tanl( double long x ) {
	return ( ::std::tan( static_cast<double>( x ) ) );
}
#endif /* not HAVE_DECL_TANL */

#if ! defined( HAVE_DECL_ASINL ) || ( HAVE_DECL_ASINL == 0 )
double long asinl( double long x ) {
	return ( asin( static_cast<double>( x ) ) );
}
#endif /* #if ! defined( HAVE_DECL_ASINL ) || ( HAVE_DECL_ASINL == 0 ) */

#if ! defined( HAVE_DECL_ACOSL ) || ( HAVE_DECL_ACOSL == 0 )
double long acosl( double long x ) {
	return ( acos( static_cast<double>( x ) ) );
}
#endif /* #if ! defined( HAVE_DECL_ACOSL ) || ( HAVE_DECL_ACOSL == 0 ) */

#if ! defined( HAVE_DECL_ATANL ) || ( HAVE_DECL_ATANL == 0 )
double long atanl( double long x ) {
	return ( ::std::atan( static_cast<double>( x ) ) );
}
#endif /* not HAVE_DECL_ATANL */

#if ! defined( HAVE_DECL_SINCOSL ) || ( HAVE_DECL_SINCOSL == 0 )
void sincosl( double long arg_, double long* sinVal_, double long* cosVal_ ) {
	*sinVal_ = sinl( arg_ );
	*cosVal_ = cosl( arg_ );
	return;
}
#endif /* #if ! defined( HAVE_DECL_SINCOSL ) || ( HAVE_DECL_SINCOSL == 0 ) */

#if ! defined( HAVE_DECL_FMODL ) || ( HAVE_DECL_FMODL == 0 )
double long fmodl( double long x, double long y ) {
	return ( fmod( static_cast<double>( x ), static_cast<double>( y ) ) );
}
#endif /* #if ! defined( HAVE_DECL_FMODL ) || ( HAVE_DECL_FMODL == 0 ) */

#if ! defined( HAVE_DECL_STRTOLD ) || ( HAVE_DECL_STRTOLD == 0 )
double long strtold( char const* str, char** tail ) {
	return ( strtod( str, tail ) );
}
#endif /* not HAVE_DECL_STRTOLD */

#if ! defined( HAVE_CLOCK_GETTIME )
#include <sys/time.h>
int clock_gettime( clockid_t, struct timespec* time_ ) {
	timeval now;
	int err( gettimeofday( &now, nullptr ) );
	if ( !err ) {
		time_->tv_sec  = now.tv_sec;
		time_->tv_nsec = now.tv_usec * 1000;
	}
	return ( err );
}
#endif /* #if ! defined( HAVE_CLOCK_GETTIME ) */

#if ! defined( HAVE_TIMER_CREATE )
#include <csignal>
#include <signal.h>
#include <unistd.h>
#include "hcore/hthread.hxx"
#include "hcore/si.hxx"
class HDarwinTimer {
	yaal::hcore::HThread _thread;
	yaal::hcore::HMutex _mutex;
	yaal::hcore::HCondition _cond;
	int _sigNo;
	itimerspec _timerSpec;
	bool _loop;
public:
	HDarwinTimer( int sigNo_ )
		: _thread()
		, _mutex()
		, _cond( _mutex )
		, _sigNo( sigNo_ )
		, _timerSpec()
		, _loop( false ) {
	}
	virtual ~HDarwinTimer( void ) {
		yaal::hcore::HLock l( _mutex );
		_loop = false;
		_cond.signal();
		l.unlock();
		_thread.finish();
	}
	void set_time( itimerspec const* timerSpec_ ) {
		yaal::hcore::HLock l( _mutex );
		_timerSpec = *timerSpec_;
		if ( ! _loop ) {
			_loop = true;
			_thread.spawn( yaal::hcore::call( &HDarwinTimer::wait, this ) );
		} else {
			_cond.signal();
		}
	}
	void wait( void ) {
		yaal::hcore::HLock l( _mutex );
		yaal::hcore::HThread::set_name( "HDarwinTimer" );
		while ( _loop ) {
			yaal::hcore::HCondition::status_t status(
				_cond.wait_for(
					duration(
						_timerSpec.it_value.tv_sec * yaal::hcore::si::NANO_IN_WHOLE + _timerSpec.it_value.tv_nsec,
						yaal::hcore::time::UNIT::NANOSECOND
					)
				)
			);
			if ( status != yaal::hcore::HCondition::SIGNALED ) {
				::kill( getpid(), _sigNo );
				break;
			}
		}
	}
};
int timer_create( clockid_t, struct sigevent* event_, timer_t* timer_ ) {
	int err( event_->sigev_notify == SIGEV_SIGNAL ? 0 : -1 );
	if ( !err ) {
		*timer_ = new HDarwinTimer( event_->sigev_signo );
	} else {
		errno = EINVAL;
	}
	return ( err );
}
int timer_settime( timer_t timer_, int flags_, itimerspec const* timerSpec_, itimerspec* ) {
	int err( 0 );
	if ( ( flags_ == 0 ) && ( timerSpec_->it_interval.tv_sec == 0 ) && ( timerSpec_->it_interval.tv_nsec == 0 ) ) {
		timer_->set_time( timerSpec_ );
	} else {
		err = -1;
		errno = EINVAL;
	}
	return ( err );
}
int timer_delete( timer_t timer_ ) {
	delete timer_;
	return ( 0 );
}
#endif /* #if ! defined( HAVE_TIMER_CREATE ) */


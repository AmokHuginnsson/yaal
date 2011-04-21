#include <stack>
#include <sys/cdefs.h>
#include <process.h>

#define pthread_sigmask pthread_sigmask_off
#define execl execl_off
#define execle execle_off
#define execv execv_off
#define execve execve_off
#define execlp execlp_off
#define execvp execvp_off
#define access access_off
#define lseek lseek_off
#define dup dup_off
#define dup2 dup2_off
#define getpid getpid_off
#define isatty isatty_off
#define getpwuid_r getpwuid_r_off

#include <csignal>

#undef getpwuid_r
#undef pthread_sigmask
#undef getpid

#include <iostream>

#include "synchronizedqueue.hxx"
#include "hcore/base.hxx"
#include "hcore/hboundcall.hxx"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;

template<typename TLS>
class tls_ptr
	{
	typedef tls_ptr<TLS> this_type;
	typedef stack<TLS*> tls_instances_t;
	typedef HBoundCall<> tls_constructor_t;
	static _declspec( thread ) TLS* _tls;
	CMutex _mutex;
	tls_instances_t _tlsInstances;
	tls_constructor_t _tlsConstructor;
public:
	tls_ptr( void ) : _mutex(), _tlsInstances(), _tlsConstructor( call( static_cast<void ( this_type::* )( void )>( &this_type::construct ), this ) ) {}
	void construct( void )
		{
		CLock l( _mutex );
		M_ASSERT( ! _tls );
		_tls = new TLS;
		_tlsInstances.push( _tls );
		}
	template<typename T0>
	tls_ptr( T0 const& a0 ) : _mutex(), _tlsInstances(), _tlsConstructor( call( static_cast<void ( this_type::* )( T0 const& )>( &this_type::construct ), this, a0 ) ) {}
	template<typename T0>
	void construct( T0 const& a0 )
		{
		CLock l( _mutex );
		M_ASSERT( ! _tls );
		_tls = new TLS( a0 );
		_tlsInstances.push( _tls );
		}
	template<typename T0, typename T1>
	tls_ptr( T0 const& a0, T1 const& a1 )
		: _mutex(), _tlsInstances(),
		_tlsConstructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const& )>( &this_type::construct ), this, a0, a1 ) ) {}
	template<typename T0, typename T1>
	void construct( T0 const& a0, T1 const& a1 )
		{
		CLock l( _mutex );
		M_ASSERT( ! _tls );
		_tls = new TLS( a0, a1 );
		_tlsInstances.push( _tls );
		}
	template<typename T0, typename T1, typename T2>
	tls_ptr( T0 const& a0, T1 const& a1, T2 const& a2 )
		: _mutex(), _tlsInstances(),
		_tlsConstructor( call( static_cast<void ( this_type::* )( T0 const&, T1 const&, T2 const& )>( &this_type::construct ), this, a0, a1, a2 ) ) {}
	template<typename T0, typename T1, typename T2>
	void construct( T0 const& a0, T1 const& a1, T2 const& a2 )
		{
		CLock l( _mutex );
		M_ASSERT( ! _tls );
		_tls = new TLS( a0, a1, a2 );
		_tlsInstances.push( _tls );
		}
	~tls_ptr( void )
		{
		CLock l( _mutex );
		while ( ! _tlsInstances.empty() )
			{
			delete _tlsInstances.top();
			_tlsInstances.pop();
			}
		_tls = NULL;
		}
	TLS* operator->( void )
		{
		if ( ! _tls )
			_tlsConstructor();
		return ( _tls );
		}
	};
template<typename TLS>
__declspec( thread ) TLS* tls_ptr<TLS>::_tls = NULL;

typedef SynchronizedQueue<int> signal_queue_t;

class SignalsSetup
	{
public:
	SignalsSetup( void );
	~SignalsSetup( void );
	void set_mask( sigset_t const* );
	};

SignalsSetup::SignalsSetup( void )
	{
	}

SignalsSetup::~SignalsSetup( void )
	{
	}

void SignalsSetup::set_mask( sigset_t const* )
	{
	}

typedef tls_ptr<SignalsSetup> TLSSignalsSetup;

TLSSignalsSetup _tlsSignalsSetup_;

signal_queue_t _signalQueue_;

M_EXPORT_SYMBOL
int kill( int pid_, int sigNo_ )
	{
	int err( 0 );
	if ( pid_ == getpid() )
		_signalQueue_.push( sigNo_ );
	else if ( sigNo_ )
		{
		HANDLE process( ::OpenProcess( PROCESS_TERMINATE, false, pid_ ) );
		if ( ! process )
			err = -1;
		else
			{
			::TerminateProcess( process, sigNo_ );
			::CloseHandle( process );
			}
		}
	else
		{
		HANDLE process( ::OpenProcess( PROCESS_QUERY_INFORMATION, false, pid_ ) );
		if ( ! process )
			{
			int code( ::GetLastError() );
			err = -1;
			}
		else
			::CloseHandle( process );
		}
	return ( err );
	}

M_EXPORT_SYMBOL
int sigwait( sigset_t*, int* signo )
	{
	if ( _signalQueue_.pop( *signo ) )
		*signo = SIGURG;
	return ( 0 );
	}

int sigaddset( sigset_t*, int )
	{
	return ( 0 );
	}

int sigdelset( sigset_t*, int )
	{
	return ( 0 );
	}

int sigemptyset( sigset_t* )
	{
	return ( 0 );
	}

void win_signal_handler( int signo )
	{
	_signalQueue_.push( signo );
	}

int sigaction( int signo, struct sigaction*, void* )
	{
	if ( ( signo != SIGURG ) && ( signo != SIGBUS ) && ( signo != SIGTRAP ) && ( signo != SIGSYS ) && ( signo != SIGALRM ) ) /*&& ( signo != 12 ) ) */
		signal( signo, win_signal_handler );
	return ( 0 );
	}

extern "C"
M_EXPORT_SYMBOL
int pthread_sigmask( int, sigset_t* set_, void* )
	{
	_tlsSignalsSetup_->set_mask( set_ );
	return ( 0 );
	}

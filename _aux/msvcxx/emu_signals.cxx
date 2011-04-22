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
#include "hcore/htls.hxx"
#include "hcore/base.hxx"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;


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

typedef HTLS<SignalsSetup> TLSSignalsSetup;

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


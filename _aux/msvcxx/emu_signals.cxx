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

#include "synchronizedqueue.hxx"
#include "hcore/base.hxx"
#include "emu_signals.hxx"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace msvcxx;

namespace msvcxx
{

SignalsSetup::SignalsSetup( void )
	: _mask(), _interrupt( ::CreateEvent( NULL, false, false, NULL ) )
	{
	}

SignalsSetup::~SignalsSetup( void )
	{
	::CloseHandle( _interrupt );
	}

void SignalsSetup::set_mask( int how_, sigset_t const* set_ )
	{
	switch ( how_ )
		{
		case ( SIG_SETMASK ):
			_mask = *set_;
		break;
		case ( SIG_BLOCK ):
			_mask |= *set_;
		break;
		case ( SIG_UNBLOCK ):
			_mask &= ~*set_;
		break;
		default:
			M_ASSERT( !"Bad how_ argument value." );
		break;
		}
	}

bool SignalsSetup::is_blocked( int sigNo_ ) const
	{
	sigset_t copy( _mask );
	return ( sigismember( &copy, sigNo_ ) ? true : false );
	}

HANDLE SignalsSetup::interrupt( void )
	{
	return ( _interrupt );
	}

void SignalsSetup::signal( void )
	{
	::SetEvent( _interrupt );
	}

class SignalDispatcher
	{
	typedef SynchronizedQueue<int> signal_queue_t;
	signal_queue_t _queue;
	sigset_t _mask;
public:
	SignalDispatcher( void );
	void dispatch( int );
	bool sigwait( sigset_t const*, int& );
	};

TLSSignalsSetup _tlsSignalsSetup_;
SignalDispatcher _signalDispatcher_;

SignalDispatcher::SignalDispatcher( void )
	: _queue(), _mask()
	{
	}

void SignalDispatcher::dispatch( int sigNo_ )
	{
	if ( sigismember( &_mask, sigNo_ ) )
		{
		_queue.push( sigNo_ );
		}
	else
		{
		external_lock_t l( _tlsSignalsSetup_.acquire() );
		for ( TLSSignalsSetup::iterator it( _tlsSignalsSetup_.begin() ), end( _tlsSignalsSetup_.end() );
			it != end; ++ it )
			{
			if ( ! (*it)->is_blocked( sigNo_ ) )
				{
				(*it)->signal();
				break;
				}
			}
		}
	}

bool SignalDispatcher::sigwait( sigset_t const* mask_, int& sigNo_ )
	{
	_mask = *mask_;
	return ( _queue.pop( sigNo_ ) );
	}

}

M_EXPORT_SYMBOL
int kill( int pid_, int sigNo_ )
	{
	int err( 0 );
	if ( pid_ == getpid() )
		_signalDispatcher_.dispatch( sigNo_ );
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
int sigwait( sigset_t* mask_, int* signo )
	{
	if ( _signalDispatcher_.sigwait( mask_, *signo ) )
		*signo = SIGURG;
	return ( 0 );
	}

int sigaddset( sigset_t* set_, int sig_ )
	{
	return ( __sigaddset( set_, sig_ ) );
	}

int sigdelset( sigset_t* set_, int sig_ )
	{
	return ( __sigdelset( set_, sig_ ) );
	}

int sigismember( sigset_t* set_, int sig_ )
	{
	return ( __sigismember( set_, sig_ ) );
	}

int sigemptyset( sigset_t* set_ )
	{
	return ( __sigemptyset( set_ ) );
	}

void win_signal_handler( int signo )
	{
	_signalDispatcher_.dispatch( signo );
	}

int sigaction( int signo, struct sigaction*, void* )
	{
	if ( ( signo != SIGURG ) && ( signo != SIGBUS ) && ( signo != SIGTRAP ) && ( signo != SIGSYS ) && ( signo != SIGALRM ) ) /*&& ( signo != 12 ) ) */
		signal( signo, win_signal_handler );
	return ( 0 );
	}

extern "C"
M_EXPORT_SYMBOL
int pthread_sigmask( int how_, sigset_t* set_, void* )
	{
	_tlsSignalsSetup_->set_mask( how_, set_ );
	return ( 0 );
	}


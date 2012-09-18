#include <sys/cdefs.h>
#include <process.h>

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
#undef getpid

#include "synchronizedqueue.hxx"
#include "hcore/base.hxx"
#include "emu_signals.hxx"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace msvcxx;

namespace msvcxx {

class SignalDispatcher {
	typedef SynchronizedQueue<int> signal_queue_t;
	bool _started;
	signal_queue_t _queue;
	sigset_t _block;
	sigset_t _accept;
public:
	SignalDispatcher( void );
	void dispatch( int );
	bool sigwait( sigset_t const*, int& );
	sigset_t get_mask( void ) const;
	void set_mask( int, sigset_t const* );
	bool is_started( void ) const;
} _signalDispatcher_;

SignalsSetup::SignalsSetup( void )
	: _mask( _signalDispatcher_.get_mask() ), _interrupt( ::CreateEvent( NULL, false, false, NULL ) ) {
}

SignalsSetup::~SignalsSetup( void ) {
	::CloseHandle( _interrupt );
}

void SignalsSetup::set_mask( int how_, sigset_t const* set_ ) {
	switch ( how_ ) {
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

sigset_t SignalsSetup::get_mask( void ) const {
	return ( _mask );
}

bool SignalsSetup::is_blocked( int sigNo_ ) const {
	sigset_t copy( _mask );
	return ( sigismember( &copy, sigNo_ ) ? true : false );
}

HANDLE SignalsSetup::interrupt( void ) {
	return ( _interrupt );
}

void SignalsSetup::signal( void ) {
	::SetEvent( _interrupt );
}

TLSSignalsSetup _tlsSignalsSetup_;

SignalDispatcher::SignalDispatcher( void )
	: _started( false ), _queue(), _block(), _accept() {
}

void SignalDispatcher::dispatch( int sigNo_ ) {
	if ( sigismember( &_accept, sigNo_ ) ) {
		_queue.push( sigNo_ );
	} else {
		external_lock_t l( _tlsSignalsSetup_.acquire() );
		for ( TLSSignalsSetup::iterator it( _tlsSignalsSetup_.begin() ), end( _tlsSignalsSetup_.end() );
			it != end; ++ it ) {
			if ( ! (**it)->is_blocked( sigNo_ ) ) {
				(**it)->signal();
				break;
			}
		}
	}
}

bool SignalDispatcher::sigwait( sigset_t const* set_, int& sigNo_ ) {
	_started = true;
	_accept = *set_;
	return ( _queue.pop( sigNo_ ) );
}

sigset_t SignalDispatcher::get_mask( void ) const {
	return ( _block );
}

void SignalDispatcher::set_mask( int how_, sigset_t const* set_ ) {
	switch ( how_ ) {
		case ( SIG_SETMASK ):
			_block = *set_;
		break;
		case ( SIG_BLOCK ):
			_block |= *set_;
		break;
		case ( SIG_UNBLOCK ):
			_block &= ~*set_;
		break;
		default:
			M_ASSERT( !"Bad how_ argument value." );
		break;
	}
}

bool SignalDispatcher::is_started( void ) const {
	return ( _started );
}

}

M_EXPORT_SYMBOL
int kill( int pid_, int sigNo_ ) {
	if ( sigNo_ == SIGABRT_COMPAT )
		sigNo_ = SIGABRT;
	int err( 0 );
	if ( pid_ == getpid() )
		_signalDispatcher_.dispatch( sigNo_ );
	else if ( sigNo_ ) {
		HANDLE process( ::OpenProcess( PROCESS_TERMINATE, false, pid_ ) );
		if ( ! process )
			err = -1;
		else {
			::TerminateProcess( process, sigNo_ );
			::CloseHandle( process );
		}
	} else {
		HANDLE process( ::OpenProcess( PROCESS_QUERY_INFORMATION, false, pid_ ) );
		if ( ! process ) {
			int code( ::GetLastError() );
			err = -1;
		} else
			::CloseHandle( process );
	}
	return ( err );
}

M_EXPORT_SYMBOL
int sigwait( sigset_t* set_, int* signo ) {
	if ( _signalDispatcher_.sigwait( set_, *signo ) )
		*signo = SIGURG;
	return ( 0 );
}

inline sigset_t sigmask( int sig_ ) {
	sigset_t mask( 1 );
	return ( mask << ( sig_ - 1 ) );
}

int sigaddset( sigset_t* set_, int sig_ ) {
	*set_ |= sigmask( sig_ );
	return ( 0 );
}

int sigdelset( sigset_t* set_, int sig_ ) {
	*set_ &= ~sigmask( sig_ );
	return ( 0 );
}

int sigismember( sigset_t* set_, int sig_ ) {
	return ( *set_ & sigmask( sig_ ) ? 1 : 0 );
}

int sigemptyset( sigset_t* set_ ) {
	return ( *set_ = 0 );
}

void win_signal_handler( int signo ) {
	if ( signo == SIGABRT_COMPAT )
		signo = SIGABRT;
	_signalDispatcher_.dispatch( signo );
}

int sigaction( int signo, struct sigaction* sa_, void* ) {
	if ( ( signo != SIGURG )
			&& ( signo != SIGBUS )
			&& ( signo != SIGTRAP )
			&& ( signo != SIGSYS )
			&& ( signo != SIGALRM ) ) /*&& ( signo != 12 ) ) */ {
		if ( sa_->sa_handler != SIG_DFL ) {
			signal( signo, win_signal_handler );
			if ( signo == SIGABRT )
				signal( SIGABRT_COMPAT, win_signal_handler );
		} else {
			signal( signo, sa_->sa_handler );
			if ( signo == SIGABRT )
				signal( SIGABRT_COMPAT, sa_->sa_handler );
		}
	}
	return ( 0 );
}

int pthread_sigmask( int how_, sigset_t* set_, void* ) {
	if ( sigismember( set_, SIGALRM ) )
	if ( _signalDispatcher_.is_started() )
		_tlsSignalsSetup_->set_mask( how_, set_ );
	else
		_signalDispatcher_.set_mask( how_, set_ );
	return ( 0 );
}


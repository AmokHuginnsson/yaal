#define EMU_SIGNALS_IMPL 1
#ifndef _CMATH_
#define _CMATH_ 1
#endif
#include <pthread.h>

#include "_aux/msvcxx/csignal"
#include "_aux/msvcxx/unistd.h"
#include "hcore/base.hxx"
#include "_aux/msvcxx/emu_signals.hxx"
#include "_aux/msvcxx/cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace msvcxx;

namespace msvcxx {

SignalsSetup::SignalsSetup( void )
	: _mask( _signalDispatcher_.get_mask() )
	, _interrupt( ::CreateEvent( nullptr, false, false, nullptr ) ) {
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

SignalDispatcher::SignalDispatcher( void )
	: _started( false )
	, _queue()
	, _block()
	, _accept()
	, _acceptedConsoleEvents() {
}

void SignalDispatcher::dispatch( int sigNo_ ) {
	if ( sigismember( &_accept, sigNo_ ) ) {
		_queue.push( sigNo_ );
	} else {
		HLock l( _tlsSignalsSetup_.acquire() );
		for ( TLSSignalsSetup::instances_t::value_type const& signalSetup : _tlsSignalsSetup_ ) {
			if ( ! (*signalSetup)->is_blocked( sigNo_ ) ) {
				(*signalSetup)->signal();
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

void SignalDispatcher::set_enabled_console_event( DWORD controlType_, bool enabled_ ) {
	switch ( controlType_ ) {
		case ( CTRL_C_EVENT ):        _acceptedConsoleEvents._ctrlC    = enabled_; break;
		case ( CTRL_BREAK_EVENT ):    _acceptedConsoleEvents._break    = enabled_; break;
		case ( CTRL_CLOSE_EVENT ):    _acceptedConsoleEvents._close    = enabled_; break;
		case ( CTRL_LOGOFF_EVENT ):   _acceptedConsoleEvents._logoff   = enabled_; break;
		case ( CTRL_SHUTDOWN_EVENT ): _acceptedConsoleEvents._shutdown = enabled_; break;
	}
}

bool SignalDispatcher::is_console_event_enabled( DWORD controlType_ ) const {
	bool enabled( false );
	switch ( controlType_ ) {
		case ( CTRL_C_EVENT ):        enabled = _acceptedConsoleEvents._ctrlC;    break;
		case ( CTRL_BREAK_EVENT ):    enabled = _acceptedConsoleEvents._break;    break;
		case ( CTRL_CLOSE_EVENT ):    enabled = _acceptedConsoleEvents._close;    break;
		case ( CTRL_LOGOFF_EVENT ):   enabled = _acceptedConsoleEvents._logoff;   break;
		case ( CTRL_SHUTDOWN_EVENT ): enabled = _acceptedConsoleEvents._shutdown; break;
	}
	return enabled;
}

bool SignalDispatcher::is_console_event_enabled( void ) const {
	return (
		_acceptedConsoleEvents._ctrlC
		|| _acceptedConsoleEvents._break
		|| _acceptedConsoleEvents._close
		|| _acceptedConsoleEvents._logoff
		|| _acceptedConsoleEvents._shutdown
	);
}

}

int kill( int pid_, int sigNo_ ) {
	if ( sigNo_ == SIGABRT_COMPAT )
		sigNo_ = SIGABRT;
	int err( 0 );
	if ( pid_ == getpid() ) {
		_signalDispatcher_.dispatch( sigNo_ );
	} else if ( sigNo_ ) {
		HANDLE process( ::OpenProcess( PROCESS_TERMINATE, false, pid_ ) );
		if ( ! process ) {
			err = -1;
		} else {
			::TerminateProcess( process, sigNo_ );
			::CloseHandle( process );
		}
	} else {
		HANDLE process( ::OpenProcess( PROCESS_QUERY_INFORMATION, false, pid_ ) );
		if ( ! process ) {
			int code( ::GetLastError() );
			err = -1;
		} else {
			::CloseHandle( process );
		}
	}
	return err;
}

int sigwait( sigset_t* set_, int* signo ) {
	if ( _signalDispatcher_.sigwait( set_, *signo ) ) {
		*signo = SIGURG;
	}
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

int sigismember( sigset_t const* set_, int sig_ ) {
	return ( *set_ & sigmask( sig_ ) ? 1 : 0 );
}

int sigemptyset( sigset_t* set_ ) {
	*set_ = 0;
	return ( 0 );
}

int sigfillset( sigset_t* set_ ) {
	*set_ = 0xffffffffffffffffULL;
	return ( 0 );
}

int sigpending( sigset_t* ) {
	return ( 0 );
}

void win_signal_handler( int signo ) {
	if ( signo == SIGABRT_COMPAT )
		signo = SIGABRT;
	_signalDispatcher_.dispatch( signo );
}

BOOL WINAPI win_console_handler( DWORD controlType_ ) {
	int signo( 0 );
	if ( _signalDispatcher_.is_console_event_enabled( controlType_ ) ) {
		switch ( controlType_ ) {
			case ( CTRL_C_EVENT ): signo = SIGINT; break;
			case ( CTRL_BREAK_EVENT ): signo = SIGQUIT; break;
			case ( CTRL_CLOSE_EVENT ): signo = SIGKILL; break;
			case ( CTRL_LOGOFF_EVENT ): signo = SIGTERM; break;
			case ( CTRL_SHUTDOWN_EVENT ): signo = SIGPWR; break;
		}
		if ( signo )
			_signalDispatcher_.dispatch( signo );
	}
	return ( 1 );
}

DWORD signo_to_console_control( int signo_ ) {
	DWORD consoleControl( 0 );
	switch ( signo_ ) {
		case ( SIGINT ): consoleControl = CTRL_C_EVENT; break;
		case ( SIGQUIT ): consoleControl = CTRL_BREAK_EVENT; break;
		case ( SIGKILL ): consoleControl = CTRL_CLOSE_EVENT; break;
		case ( SIGTERM ): consoleControl = CTRL_LOGOFF_EVENT; break;
		case ( SIGPWR ): consoleControl = CTRL_SHUTDOWN_EVENT; break;
	}
	return consoleControl;
}

int sigaction( int signo, struct sigaction* sa_, void* ) {
	if ( ( signo != SIGURG )
			&& ( signo != SIGBUS )
			&& ( signo != SIGTRAP )
			&& ( signo != SIGSYS )
			&& ( signo != SIGWINCH )
			&& ( signo != SIGALRM ) ) {
		if ( sa_->sa_handler != SIG_DFL ) {
			if ( ( signo == SIGINT ) || ( signo == SIGQUIT ) || ( signo == SIGKILL ) || ( signo == SIGTERM ) || ( signo == SIGTERM ) || ( signo == SIGPWR ) ) {
				if ( ! _signalDispatcher_.is_console_event_enabled() )
					SetConsoleCtrlHandler( &win_console_handler, 1 );
				_signalDispatcher_.set_enabled_console_event( signo_to_console_control( signo ), true );
			}
			signal( signo, win_signal_handler );
			if ( signo == SIGABRT )
				signal( SIGABRT_COMPAT, win_signal_handler );
		} else {
			if ( ( signo == SIGINT ) || ( signo == SIGQUIT ) || ( signo == SIGKILL ) || ( signo == SIGTERM ) || ( signo == SIGTERM ) || ( signo == SIGPWR ) ) {
				if ( _signalDispatcher_.is_console_event_enabled() ) {
					_signalDispatcher_.set_enabled_console_event( signo_to_console_control( signo ), false );
					if ( ! _signalDispatcher_.is_console_event_enabled() )
						SetConsoleCtrlHandler( &win_console_handler, 0 );
				}
			}
			signal( signo, sa_->sa_handler );
			if ( signo == SIGABRT ) {
				signal( SIGABRT_COMPAT, sa_->sa_handler );
			}
		}
	}
	return ( 0 );
}

int pthread_sigmask( int how_, sigset_t const* set_, sigset_t* old_ ) {
	if ( old_ ) {
		*old_ = _tlsSignalsSetup_->get_mask();
	}
	if ( sigismember( set_, SIGALRM ) ) {
		_tlsSignalsSetup_->set_mask( how_, set_ );
		if ( ! _signalDispatcher_.is_started() ) {
			_signalDispatcher_.set_mask( how_, set_ );
		}
	}
	return ( 0 );
}

int sigprocmask( int how_, sigset_t const* set_, sigset_t* old_ ) {
	return ( pthread_sigmask( how_, set_, old_ ) );
}

char const* strsignal( int signum_ ) {
	char const* name( "unknown" );
	switch ( signum_ ) {
		case ( SIGINT ):  name = "interrupted";          break;
		case ( SIGKILL ): name = "killed";               break;
		case ( SIGQUIT ): name = "exited";               break;
		case ( SIGTERM ): name = "terminated";           break;
		case ( SIGILL ):  name = "illegal instruction";  break;
		case ( SIGABRT_COMPAT ):
		case ( SIGABRT ): name = "aborted";              break;
		case ( SIGSEGV ): name = "segvfaulted";          break;
		case ( SIGFPE ):  name = "floating point error"; break;
		case ( SIGALRM ): name = "alarmed";              break;
		case ( SIGUSR1 ): name = "user signal 1";        break;
		case ( SIGSTOP ): name = "stopped";              break;
		case ( SIGTSTP ): name = "suspended";            break;
		case ( SIGCONT ): name = "continued";            break;
	}
	return name;
}

namespace msvcxx {
void sendalarm() {
	kill( getpid(), SIGALRM );
	return;
}
}


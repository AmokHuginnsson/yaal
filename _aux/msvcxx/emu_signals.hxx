#ifndef YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED 1

#include "hcore/htls.hxx"
#include "_aux/msvcxx/synchronizedqueue.hxx"
#include "csignal"

namespace msvcxx {

class SignalDispatcher {
	typedef SynchronizedQueue<int> signal_queue_t;
	bool _started;
	signal_queue_t _queue;
	sigset_t _block;
	sigset_t _accept;
	struct ConsoleEvents {
		bool _ctrlC;
		bool _break;
		bool _close;
		bool _logoff;
		bool _shutdown;
		ConsoleEvents( void )
			: _ctrlC( false )
			, _break( false )
			, _close( false )
			, _logoff( false )
			, _shutdown( false ) {
		}
	} _acceptedConsoleEvents;
public:
	SignalDispatcher( void );
	void dispatch( int );
	bool sigwait( sigset_t const*, int& );
	sigset_t get_mask( void ) const;
	void set_mask( int, sigset_t const* );
	bool is_started( void ) const;
	void set_enabled_console_event( DWORD, bool );
	bool is_console_event_enabled( DWORD ) const;
	bool is_console_event_enabled( void ) const;
};

class SignalsSetup {
	sigset_t _mask;
	HANDLE _interrupt;
public:
	SignalsSetup( void );
	~SignalsSetup( void );
	void set_mask( int, sigset_t const* );
	sigset_t get_mask( void ) const;
	bool is_blocked( int ) const;
	HANDLE interrupt( void );
	void signal( void );
};

extern SignalDispatcher _signalDispatcher_;
typedef yaal::hcore::HTLS<SignalsSetup> TLSSignalsSetup;
extern TLSSignalsSetup _tlsSignalsSetup_;

} /* namespace msvcxx */

#endif /* #ifndef YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED */

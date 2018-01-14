/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HPROCESS_HXX_INCLUDED
#define YAAL_TOOLS_HPROCESS_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/htuple.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hpipe.hxx"
#include "tools/signals.hxx"

namespace yaal {

namespace tools {

/*! \brief Input/output (fd) event listener and notification dispatcher.
 */
class HIODispatcher {
public:
	typedef HIODispatcher this_type;
	enum class FD_TYPE {
		READER,
		WRITER
	};
	typedef yaal::hcore::HStreamInterface::ptr_t stream_t;
	typedef yaal::hcore::HBoundCall<void ( stream_t& )> callback_t;
	typedef yaal::hcore::HPair<stream_t, callback_t> io_handler_t;
	typedef yaal::hcore::HArray<io_handler_t> io_handlers_t;
private:
	typedef yaal::hcore::HBoundCall<> delayed_call_t;
	typedef yaal::hcore::HTuple<FD_TYPE, stream_t, callback_t> new_io_handler_t;
	typedef yaal::hcore::HArray<stream_t> dropped_io_handlers_t;
	typedef yaal::hcore::HArray<delayed_call_t> delayed_calls_t;
	typedef yaal::hcore::HArray<new_io_handler_t> new_io_handlers_t;
	bool _initialized; /*!< did process has necessary initialization */
	bool _loop;        /*!< indicates if main loop continues */
	int _idleCycles;   /*!< full select()'s without i/o activity */
	int long _latency; /*!< timeout between recall (milliseconds) */
	yaal::hcore::HChunk _select;
	io_handlers_t _readers;
	io_handlers_t _writers;
	delayed_calls_t _alert;
	delayed_calls_t _idle;
	dropped_io_handlers_t _droppedIOHandlers;
	new_io_handlers_t _newIOHandlers;
	bool _callbackContext;
	yaal::hcore::HPipe _event;
	yaal::hcore::HMutex _mutex;
public:
	/*! \brief Create new I/O-dispatcher object.
	 *
	 * \param initialFdBuckets - initial number of file descriptor buckets.
	 * \param latency - how long should dispatcher sleep during select, between invoking idle actions.
	 */
	HIODispatcher( int initialFdBuckets, int long latency );
	virtual ~HIODispatcher( void );
	void run( void );
	void stop( void );
	int idle_cycles( void ) const;
	void register_file_descriptor_handler( stream_t, callback_t, FD_TYPE = FD_TYPE::READER );
	void unregister_file_descriptor_handler( stream_t const& );
	void add_alert_handle( delayed_call_t );
	void add_idle_handle( delayed_call_t );
private:
	void reconstruct_fdset( void );
	/*! \brief Process incoming events from interrupt socket.
	 */
	void process_interrupt( stream_t& );
	/*! \brief Callback for SignalService.
	 */
	int handler_interrupt( int );
	void handle_alerts( void );
	void handle_idle( void );
	HIODispatcher( HIODispatcher const& );
	HIODispatcher& operator = ( HIODispatcher const& );
};

typedef yaal::hcore::HExceptionT<HIODispatcher> HIODispatcherException;

}

}

#endif /* #ifndef YAAL_TOOLS_HPROCESS_HXX_INCLUDED */


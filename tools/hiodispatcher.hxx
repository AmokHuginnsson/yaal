/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HPROCESS_HXX_INCLUDED
#define YAAL_TOOLS_HPROCESS_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/htuple.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hpipe.hxx"
#include "hcore/system.hxx"
#include "tools/signals.hxx"

namespace yaal {

namespace tools {

/*! \brief Input/output (fd) event listener and notification dispatcher.
 */
class HIODispatcher {
public:
	typedef HIODispatcher this_type;
private:
	class HIOHandler {
	public:
		typedef yaal::hcore::HStreamInterface::ptr_t stream_t;
		typedef yaal::hcore::HBoundCall<void ( stream_t&, yaal::hcore::system::IO_EVENT_TYPE )> callback_t;
		typedef yaal::hcore::HStreamInterface const* stream_id_t;
	private:
		stream_t _stream;
		callback_t _callback;
		yaal::hcore::system::IO_EVENT_TYPE _reqIOEventType;
	public:
		HIOHandler( stream_t const&, callback_t const&, yaal::hcore::system::IO_EVENT_TYPE );
		HIOHandler( HIOHandler const& ) = default;
		HIOHandler( HIOHandler&& ) = default;
		HIOHandler& operator = ( HIOHandler const& ) = default;
		HIOHandler& operator = ( HIOHandler&& ) = default;
		void call( yaal::hcore::system::IO_EVENT_TYPE );
		stream_id_t id( void ) const;
		yaal::hcore::system::IO_EVENT_TYPE req_io_event_type( void ) const;
		bool can_read_now( void ) const;
	};
	typedef yaal::hcore::HHashSet<HIOHandler::stream_id_t> dropped_io_handlers_t;
public:
	typedef HIOHandler::stream_t stream_t;
	typedef HIOHandler::callback_t callback_t;
private:
	typedef yaal::hcore::HBoundCall<> delayed_call_t;
	typedef yaal::hcore::HTuple<yaal::hcore::system::IO_EVENT_TYPE, HIOHandler::stream_t, HIOHandler::callback_t> new_io_handler_t;
	typedef yaal::hcore::HHashMap<HIOHandler::stream_id_t, HIOHandler> io_handlers_t;
	typedef yaal::hcore::HArray<HIOHandler*> io_handler_refs_t;
	typedef yaal::hcore::HArray<delayed_call_t> delayed_calls_t;
	typedef yaal::hcore::HArray<new_io_handler_t> new_io_handlers_t;
	bool _initialized; /*!< did process has necessary initialization */
	bool _loop;        /*!< indicates if main loop continues */
	int _idleCycles;   /*!< full select()'s without i/o activity */
	int _latency; /*!< timeout between recall (milliseconds) */
	io_handlers_t _handlers;
	io_handler_refs_t _refs;
	yaal::hcore::HChunk _pollfds;
	bool _valid;
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
	HIODispatcher( int initialFdBuckets, int latency );
	virtual ~HIODispatcher( void );
	void run( void );
	void stop( void );
	int idle_cycles( void ) const;
	void register_file_descriptor_handler( HIOHandler::stream_t, HIOHandler::callback_t, yaal::hcore::system::IO_EVENT_TYPE );
	void unregister_file_descriptor_handler( HIOHandler::stream_t const& );
	void add_alert_handle( delayed_call_t );
	void add_idle_handle( delayed_call_t );
private:
	void reconstruct( void );
	bool dispatch( dropped_io_handlers_t const& );
	bool dispatch_pending_reads( dropped_io_handlers_t const& );
	void do_unregister_file_descriptor_handler( HIOHandler::stream_id_t );
	/*! \brief Process incoming events from interrupt socket.
	 */
	void process_interrupt( HIOHandler::stream_t&, yaal::hcore::system::IO_EVENT_TYPE );
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


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hiodispatcher.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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

/*! \brief Input/output (fd) event listner and notification dispatcher.
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
	bool _initialised; /*!< did process has necessery initialisation */
	bool _loop;        /*!< indicates if main loop continues */
	int _idleCycles;   /*!< full select()'s without io activity */
	int long _latency; /*!< timeout between recall (miliseconds) */
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
	/*! \brief Create new io-dispatcher object.
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


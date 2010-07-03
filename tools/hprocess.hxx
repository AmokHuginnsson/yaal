/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_TOOLS_HPROCESS_HXX_INCLUDED
#define YAAL_TOOLS_HPROCESS_HXX_INCLUDED

#include "hcore/harray.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hpipe.hxx"
#include "tools/signals.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Class that provision framework for creation of multiplexing based applications.
 */
class HProcess
	{
protected:
	typedef HProcess this_type;
private:
	typedef yaal::hcore::HBoundCall<0, void> delayed_call_t;
	typedef yaal::hcore::HArray<delayed_call_t> delayed_calls_t;
	typedef yaal::hcore::HBoundCall<1, void, int> process_filedes_handler_t;
	typedef yaal::hcore::HHashMap<int, process_filedes_handler_t> process_filedes_map_t;
	typedef yaal::hcore::HArray<int> dropped_fd_t;
	bool _initialised;					/* did process has necessery initialisation */
	bool _loop; 								/* indicates if main loop continues */
	int _idleCycles;					/* full select()'s without io activity */
	int _latencySeconds;			/* timeout between recall */
	int _latencyMicroseconds;	/* of handler_idle */
	yaal::hcore::HChunk _select;
	process_filedes_map_t _fileDescriptorHandlers;
	delayed_calls_t _alert;
	delayed_calls_t _idle;
	dropped_fd_t _droppedFd;
	bool _callbackContext;
	yaal::hcore::HPipe _event;
	yaal::hcore::HMutex _mutex;
public:
	HProcess( int, int, int = 0 );
	virtual ~HProcess( void );
	int run( void );
	void stop( void );
	int idle_cycles( void ) const;
	int register_file_descriptor_handler( int, process_filedes_handler_t );
	void unregister_file_descriptor_handler( int );
	void add_alert_handle( delayed_call_t );
	void add_idle_handle( delayed_call_t );
private:
	int reconstruct_fdset( void );
	/*! \brief Process incoming events from interrupt socket.
	 */
	void process_interrupt( int );
	/*! \brief Callback for SignalService.
	 */
	int handler_interrupt( int );
	void handle_alerts( void );
	void handle_idle( void );
	HProcess( HProcess const& );
	HProcess& operator = ( HProcess const& );
	};

typedef yaal::hcore::HExceptionT<HProcess> HProcessException;

}

}

#endif /* not YAAL_TOOLS_HPROCESS_HXX_INCLUDED */


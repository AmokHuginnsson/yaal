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

#include <sys/time.h> /* timeval */
#include <sys/types.h>

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
class HProcess : public yaal::tools::HSignalHandlerInterface
	{
protected:
	typedef HProcess self_t;
	typedef HSignalHandlerInterface hier_t;
	typedef yaal::hcore::HBoundCallInterface<0, void>::ptr_t delayed_call_t;
	typedef yaal::hcore::HArray<delayed_call_t> delayed_calls_t;
	typedef yaal::hcore::HBoundCallInterface<1, void, int>::ptr_t process_filedes_handler_t;
	typedef yaal::hcore::HHashMap<int, process_filedes_handler_t> process_filedes_map_t;
	typedef yaal::hcore::HArray<int> dropped_fd_t;
	bool			f_bInitialised;					/* did process has necessery initialisation */
	bool			f_bLoop; 								/* indicates if main loop continues */
	int				f_iIdleCycles;					/* full select()'s without io activity */
	int				f_iLatencySeconds;			/* timeout between recall */
	int				f_iLatencyMicroseconds;	/* of handler_idle */
	timeval		f_sLatency;							/* sleep between re-selects (helper) */
	fd_set		f_xFileDescriptorSet; 	/* keyboard and eventual sockets */
	process_filedes_map_t f_oFileDescriptorHandlers;
	delayed_calls_t _alert;
	delayed_calls_t _idle;
	dropped_fd_t f_oDroppedFd;
	bool f_bCallbackContext;
	yaal::hcore::HPipe f_oEvent;
	HProcess( size_t );
	virtual ~HProcess( void );
public:
	int run( void );
	int init( int, int = 0 );
	int register_file_descriptor_handler( int, process_filedes_handler_t );
	void unregister_file_descriptor_handler( int );
	void add_alert_handle( delayed_call_t );
	void add_idle_handle( delayed_call_t );
protected:
	int reconstruct_fdset( void );
	/*! \brief Process incoming events from interrupt socket.
	 */
	void process_interrupt( int );
	/*! \brief Callback for SignalService.
	 */
	int handler_interrupt( int );
	void handle_alerts( void );
	void handle_idle( void );
	virtual int do_init( void );
	virtual int do_cleanup( void );
private:
	HProcess( HProcess const& );
	HProcess& operator = ( HProcess const& );
	};

typedef yaal::hcore::HExceptionT<HProcess, HSignalHandlerInterfaceException> Exception;

}

}

#endif /* not YAAL_TOOLS_HPROCESS_HXX_INCLUDED */


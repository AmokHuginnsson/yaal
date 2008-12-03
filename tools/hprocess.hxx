/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_TOOLS_HPROCESS_HXX_INCLUDED
#define YAAL_TOOLS_HPROCESS_HXX_INCLUDED

#include <sys/time.h> /* timeval */
#include <sys/types.h>

#include "hcore/hhashmap.hxx"
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
	typedef int ( HProcess::* process_handler_filedes_t ) ( int );
	typedef yaal::hcore::HHashMap<int, process_handler_filedes_t> process_filedes_map_t;
	typedef yaal::hcore::HPool<int> dropped_fd_t;
	bool			f_bInitialised;					/* did process has necessery initialisation */
	bool			f_bLoop; 								/* indicates if main loop continues */
	int				f_iIdleCycles;					/* full select()'s without io activity */
	int				f_iLatencySeconds;			/* timeout between recall */
	int				f_iLatencyMicroseconds;	/* of handler_idle */
	timeval		f_sLatency;							/* sleep between re-selects (helper) */
	fd_set		f_xFileDescriptorSet; 	/* keyboard and eventual sockets */
	process_filedes_map_t f_oFileDescriptorHandlers;
	dropped_fd_t f_oDroppedFd;
	bool f_bCallbackContext;
	yaal::hcore::HPipe f_oEvent;
	HProcess( size_t );
	virtual ~HProcess( void );
public:
	int run( void );
	int init( int, int = 0 );
protected:
	int reconstruct_fdset( void );
	template<typename tType>
	int register_file_descriptor_handler( int a_iFileDes, tType HANDLER )
		{
		return ( register_file_descriptor_handler_internal( a_iFileDes, static_cast<process_handler_filedes_t>( HANDLER ) ) );
		}
	int register_file_descriptor_handler_internal( int, process_handler_filedes_t );
	void unregister_file_descriptor_handler( int );
	/*! \brief Process incoming events from interrupt socket.
	 */
	int process_interrupt( int );
	/*! \brief Callback for SignalService.
	 */
	int handler_interrupt( int );
	virtual int handler_alert( int, void const* = NULL );
	virtual int handler_idle( int, void const* = NULL );
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


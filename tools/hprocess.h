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

#ifndef __YAAL_HCORE_HPROCESS_H
#define __YAAL_HCORE_HPROCESS_H

#include <sys/types.h>

#include "hcore/hhashmap.h"

namespace yaal
{

namespace hcore
{

class HProcess
	{
protected:
	typedef int ( HProcess::* process_handler_filedes_t ) ( int );
	typedef hcore::HHashMap < int, process_handler_filedes_t > process_filedes_map_t;
	bool			f_bInitialised;					/* did process has necessery initialisation */
	bool			f_bLoop; 								/* indicates if main loop continues */
	int				f_iIdleCycles;					/* full select()'s without io activity */
	int				f_iLatencySeconds;			/* timeout between recall */
	int				f_iLatencyMicroseconds;	/* of handler_idle */
	timeval		f_sLatency;							/* sleep between re-selects (helper) */
	fd_set		f_xFileDescriptorSet; 	/* keyboard and eventual sockets */
	process_filedes_map_t f_oFileDescriptorHandlers;
	HProcess ( size_t );
	virtual ~HProcess ( void );
	int init ( int, int = 0 );
	int run ( void );
	virtual int reconstruct_fdset ( void );
	template < typename tType >
	int register_file_descriptor_handler ( int a_iFileDes, tType HANDLER )
		{
		return ( register_file_descriptor_handler_internal ( a_iFileDes, static_cast < process_handler_filedes_t > ( HANDLER ) ) );
		}
	int register_file_descriptor_handler_internal ( int, process_handler_filedes_t );
	int unregister_file_descriptor_handler ( int );
	virtual int handler_alert ( int, void const* = NULL );
	virtual int handler_idle ( int, void const* = NULL );
private:
	HProcess ( HProcess const & );
	HProcess & operator = ( HProcess const & );
	};

}

}

#endif /* not __YAAL_HCORE_HPROCESS_H */


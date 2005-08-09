/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HPROCESS_H
#define __HCORE_HPROCESS_H

#include <sys/types.h>

#include "hmap.h"

namespace stdhapi
{

namespace hcore
{

#define M_REGISTER_FILE_DESCRIPTOR_HANDLER( fd, handler ) \
	register_file_descriptor_handler ( fd, static_cast < process_handler_filedes_t > ( & handler ) )

class HProcess
	{
protected:
	typedef int ( HProcess::* process_handler_filedes_t ) ( int );
	typedef hcore::HMap < int, process_handler_filedes_t > process_filedes_map_t;
	/*{*/
	bool			f_bInitialised;					/* did process has necessery initialisation */
	bool			f_bLoop; 								/* indicates if main loop continues */
	int				f_iIdleCycles;					/* full select()'s without io activity */
	int				f_iLatencySeconds;			/* timeout between recall */
	int				f_iLatencyMicroseconds;	/* of handler_idle */
	timeval		f_sLatency;							/* sleep between re-selects (helper) */
	fd_set		f_xFileDescriptorSet; 	/* keyboard and eventual sockets */
	process_filedes_map_t f_oFileDescriptorHandlers;
	/*}*/
public:
	/*{*/
	/*}*/
protected:
	/*{*/
	HProcess ( size_t );
	virtual ~HProcess ( void );
	int init ( int, int = 0 );
	int run ( void );
	virtual int reconstruct_fdset ( void );
	int register_file_descriptor_handler ( int, process_handler_filedes_t );
	int unregister_file_descriptor_handler ( int );
	virtual int handler_alert ( int, void * = NULL );
	virtual int handler_interrupt ( int, void * = NULL );
	virtual int handler_idle ( int, void * = NULL );
	/*}*/
private:
	/*{*/
	HProcess ( HProcess const & );
	HProcess & operator = ( HProcess const & );
	/*}*/
	};

}

}

#endif /* not __HCORE_HPROCESS_H */


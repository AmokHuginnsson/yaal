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

#ifndef __HCONSOLE_HPROCESS_H
#define __HCONSOLE_HPROCESS_H

#include <sys/types.h>

#include "hhandler.h"
#include "hwindow.h"
#include "hwindowlistcontrol.h"

namespace stdhapi
{

namespace hconsole
{

class HProcess : public HHandler
	{
	typedef int ( HProcess::* PROCESS_HANDLER_FILEDES_t ) ( int );
	typedef hcore::HMap < int, PROCESS_HANDLER_FILEDES_t > process_filedes_map_t;
protected:
	/*{*/
	bool			f_bInitialised;				/* did process has necessery initialisation */
	bool			f_bLoop; 							/* indicates if main loop continues */
	int				f_iIdleCycles;				/* full select()'s with out input */
	timeval		f_sLatency;						/* sleep between re-selects */
	fd_set		f_xFileDescriptorSet; /* keyboard and eventual sockets */
	HWindow *	f_poForegroundWindow; /* sefl explanary */
	HWindowListControl * f_poWindows;			/* current existing windows */
	process_filedes_map_t f_oFileDescriptorHandlers;
	/*}*/
public:
	/*{*/
	HProcess ( size_t = 8, size_t = 32, size_t = 32 );
	virtual ~HProcess ( void );
	virtual int init ( char const * = "" );
	int run ( void );
	/*}*/
protected:
	/*{*/
	virtual int reconstruct_fdset ( void );
	int process_stdin ( int );
	int process_mouse ( int );
	int process_commands ( void );
	int register_file_descriptor_handler ( int, PROCESS_HANDLER_FILEDES_t );
	int unregister_file_descriptor_handler ( int );
	int add_window ( HWindow *, char const * );
	virtual int handler_idle ( int, void * = NULL );
	virtual int handler_mouse ( int, void * = NULL );
	virtual int handler_refresh ( int, void * = NULL );
	virtual int handler_quit ( int, void * = NULL );
	virtual int handler_jump_meta_tab ( int, void * = NULL );
	virtual int handler_jump_meta_direct ( int, void * = NULL );
	virtual int handler_close_window ( int, void * = NULL );
	/*}*/
private:
	/*{*/
	HProcess ( const HProcess & );
	HProcess & operator = ( const HProcess & );
	/*}*/
	};

}

}

#endif /* not __HCONSOLE_HPROCESS_H */


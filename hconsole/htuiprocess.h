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

#ifndef __HPROCESS_H
#define __HPROCESS_H

#include <sys/types.h>

#include "hhandler.h"
#include "hwindow.h"
#include "hwindowlistcontrol.h"

class HProcess
	{
	typedef int ( HProcess::* PROCESS_HANDLER_t ) ( int, void * = NULL );
	typedef int ( HProcess::* PROCESS_HANDLER_FILEDES_t ) ( int );
protected:
	/*{*/
	bool			f_bInitialised;				/* did process has necessery initialisation */
	bool			f_bLoop; 							/* indicates if main loop continues */
	int				f_iIdleCycles;				/* full select()'s with out input */
	timeval		f_sLatency;						/* sleep between re-selects */
	fd_set		f_xFileDescriptorSet; /* keyboard and eventual sockets */
	HWindow *	f_poForegroundWindow; /* sefl explanary */
	HWindowListControl * f_poWindows;			/* current existing windows */
	HList < HHandler < PROCESS_HANDLER_FILEDES_t > > f_oFileDescriptorHandlers;
	HList < HHandler < PROCESS_HANDLER_t > > f_oPreprocessHandlers;
	HList < HHandler < PROCESS_HANDLER_t > > f_oPostprocessHandlers;
	/*}*/
public:
	/*{*/
	HProcess ( void );
	virtual ~HProcess ( void );
	virtual int init ( const char * = "" );
	int run ( void );
	/*}*/
protected:
	/*{*/
	virtual int reconstruct_fdset ( void );
	int process_stdin ( int );
	int process_mouse ( int );
	int preprocess_input ( int );
	int postprocess_input ( int );
	int register_file_descriptor_handler ( int, PROCESS_HANDLER_FILEDES_t );
	int unregister_file_descriptor_handler ( int );
	int register_preprocess_handler ( int, int *, PROCESS_HANDLER_t );
	int register_postprocess_handler ( int, int *, PROCESS_HANDLER_t );
	int add_window ( HWindow *, const char * );
	virtual int handler_idle ( int, void * = NULL );
	virtual int handler_mouse ( int, void * = NULL );
	virtual int handler_refresh ( int, void * = NULL );
	virtual int handler_quit ( int, void * = NULL );
	virtual int handler_jump_meta_tab ( int, void * = NULL );
	virtual int handler_jump_meta_direct ( int, void * = NULL );
	virtual int handler_close_window ( int, void * = NULL );
	/*}*/
	};

#endif /* not __HPROCESS_H */

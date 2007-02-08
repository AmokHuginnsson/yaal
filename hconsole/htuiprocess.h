/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HTUIPROCESS_H
#define __YAAL_HCONSOLE_HTUIPROCESS_H

#include "hcore/hprocess.h"
#include "hconsole/hhandler.h"
#include "hconsole/hwindow.h"
#include "hconsole/hwindowlistcontrol.h"

namespace yaal
{

namespace hconsole
{

class HTUIProcess : public HHandler, protected yaal::hcore::HProcess
	{
protected:
	HWindow::ptr_t f_oMainWindow; /* self explanary */
	HWindowListControl::model_t::cyclic_iterator f_oForegroundWindow; /* self explanary */
	typedef HWindowListControl::model_t model_t;
	HWindowListControl::model_ptr_t f_oWindows;	/* current existing windows */
public:
	HTUIProcess ( size_t = 8, size_t = 32, size_t = 32 );
	virtual ~HTUIProcess ( void );
	int init_tui ( char const * = "", HWindow::ptr_t = HWindow::ptr_t() );
	using hcore::HProcess::run;
protected:
	int process_stdin ( int );
	int process_mouse ( int );
	int process_commands ( void );
	int add_window ( HWindow::ptr_t );
	void select( HWindow const* );
	virtual int handler_alert ( int, void * = NULL );
	virtual int handler_interrupt ( int, void * = NULL );
	virtual int handler_idle ( int, void * = NULL );
	virtual int handler_mouse ( int, void * = NULL );
	virtual int handler_refresh ( int, void * = NULL );
	virtual int handler_quit ( int, void * = NULL );
	virtual int handler_jump_meta_tab ( int, void * = NULL );
	virtual int handler_jump_meta_direct ( int, void * = NULL );
	virtual int handler_close_window ( int, void * = NULL );
private:
	HTUIProcess ( HTUIProcess const & );
	HTUIProcess & operator = ( HTUIProcess const & );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HTUIPROCESS_H */


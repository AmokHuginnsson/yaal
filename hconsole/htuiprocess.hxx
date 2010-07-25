/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/htuiprocess.hxx
 * \brief Declaration of HTUIProcess class.
 */

#ifndef YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED
#define YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED

#include "tools/hiodispatcher.hxx"
#include "hconsole/hhandler.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hwindowlistcontrol.hxx"

namespace yaal
{

namespace hconsole
{

/*! \brief A backbone of TUI based application.
 */
class HTUIProcess : public HHandler
	{
protected:
	typedef HTUIProcess this_type;
	typedef HHandler base_type;
public:
	typedef yaal::hcore::HList<HWindow::ptr_t> model_t;
	typedef yaal::hcore::HPointer<model_t> model_ptr_t;
protected:
	yaal::tools::HIODispatcher _dispatcher;
	HWindow::ptr_t _mainWindow; /* self explanary */
	model_t::cyclic_iterator _foregroundWindow; /* self explanary */
	model_ptr_t _windows;	/* current existing windows */
public:
	HTUIProcess ( int = 8, size_t = 32, size_t = 32 );
	virtual ~HTUIProcess ( void );
	int init_tui( char const* = "", HWindow::ptr_t = HWindow::ptr_t() );
	int run( void );
protected:
	void process_stdin( int );
	void process_mouse( int );
	void process_terminal_event( int );
	int process_commands( void );
	int add_window( HWindow::ptr_t );
	void select( HWindow const* const );
	void refresh( bool = false );
	void handler_alert( void );
	void handler_idle( void );
	virtual int handler_mouse( int, void const* = NULL );
	virtual int handler_refresh( int, void const* = NULL );
	virtual int handler_quit( int, void const* = NULL );
	virtual int handler_jump_meta_tab( int, void const* = NULL );
	virtual int handler_jump_meta_direct( int, void const* = NULL );
	virtual int handler_close_window( int, void const* = NULL );
private:
	HTUIProcess( HTUIProcess const& );
	HTUIProcess& operator = ( HTUIProcess const& );
	};

typedef yaal::hcore::HExceptionT<HTUIProcess, HHandlerException> HTUIProcessException;

}

}

#endif /* not YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED */


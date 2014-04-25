/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/htuiprocess.hxx
 * \brief Declaration of HTUIProcess class.
 */

#ifndef YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED
#define YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED 1

#include "tools/hiodispatcher.hxx"
#include "hconsole/hhandler.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hwindowlistcontrol.hxx"

namespace yaal {

namespace hconsole {

/*! \brief A backbone of TUI based application.
 */
class HTUIProcess : public HHandler {
public:
	typedef HTUIProcess this_type;
	typedef HHandler base_type;
	typedef yaal::hcore::HList<HWindow::ptr_t> model_t;
	typedef yaal::hcore::HPointer<model_t> model_ptr_t;
protected:
	yaal::tools::HIODispatcher _dispatcher;
	HWindow::ptr_t _mainWindow; /* self explanary */
	model_t::cyclic_iterator _foregroundWindow; /* self explanary */
	model_ptr_t _windows;	/* current existing windows */
public:
	HTUIProcess( int = 8, int = 32, int = 32 );
	virtual ~HTUIProcess ( void );
	int init_tui( char const* = "", HWindow::ptr_t = HWindow::ptr_t() );
	void run( void );
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

#endif /* #ifndef YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED */


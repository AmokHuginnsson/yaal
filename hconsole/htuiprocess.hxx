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

#include "hcore/hqueue.hxx"
#include "tools/hiodispatcher.hxx"
#include "hconsole/hhandler.hxx"
#include "hconsole/hresource.hxx"

namespace yaal {

namespace hconsole {

class HWindow;
class HMainWindow;

/*! \brief A backbone of TUI based application.
 */
class HTUIProcess : public HHandler {
public:
	typedef HTUIProcess this_type;
	typedef HHandler base_type;
	typedef yaal::hcore::HPointer<HWindow> window_t;
	typedef yaal::hcore::HList<window_t> model_t;
	typedef yaal::hcore::HPointer<model_t> model_ptr_t;
	typedef yaal::hcore::HBoundCall<> call_t;
	typedef yaal::hcore::HQueue<call_t> call_queue_t;
	typedef hcore::HCyclicIterator<model_t> cyclic_iterator;
protected:
	yaal::tools::HIODispatcher _dispatcher;
	window_t _mainWindow;                 /* self explanary */
	cyclic_iterator _foregroundWindow; /* self explanary */
	model_ptr_t _windows;                       /* current existing windows */
	bool _needRepaint;
	call_queue_t _callQueue;
	HResource::ptr_t _resource;
public:
	HTUIProcess( int = 8, int = 32, int = 32 );
	virtual ~HTUIProcess ( void );
	void init_tui( yaal::hcore::HString const& = yaal::hcore::HString(), window_t = window_t() );
	void init_xrc( yaal::hcore::HString const&, yaal::hcore::HString const& );
	void run( void );
	void schedule_repaint( void );
	void schedule_call( call_t );
	void quit( void );
	void close_window( void );
	void flush_call_queue( void );
	void add_window( window_t );
	HMainWindow* main_window( void );
protected:
	void process_stdin( tools::HIODispatcher::stream_t& );
	void process_mouse( tools::HIODispatcher::stream_t& );
	void process_terminal_event( tools::HIODispatcher::stream_t& );
	int process_commands( void );
	void select( HWindow const* );
	void repaint( bool = false );
	void handler_alert( void );
	void handler_idle( void );
	bool handler_mouse( HEvent const& );
	bool handler_refresh( HEvent const& );
	bool handler_quit( HEvent const& );
	bool handler_jump_meta_tab( HEvent const& );
	bool handler_jump_meta_direct( HEvent const& );
	bool handler_close_window( HEvent const& );
	virtual void do_quit( void );
	virtual void do_close_window( void );
	virtual void do_init_xrc( yaal::hcore::HString const&, yaal::hcore::HString const& );
private:
	HTUIProcess( HTUIProcess const& );
	HTUIProcess& operator = ( HTUIProcess const& );
};

typedef yaal::hcore::HExceptionT<HTUIProcess, HHandlerException> HTUIProcessException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTUIPROCESS_HXX_INCLUDED */


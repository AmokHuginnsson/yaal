/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hwindow.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED
#define YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hconsole/hcontrol.hxx"
#include "hconsole/hcontrollist.hxx"
#include "hconsole/hhandler.hxx"
#include "hconsole/hstatusbarcontrol.hxx"

namespace yaal
{

namespace hconsole
{

class HControlList;

/*! \brief TUI Window implementation.
 */
class HWindow : public HHandler
	{
public:
	typedef HWindow this_type;
	typedef HHandler base_type;
	typedef yaal::hcore::HPointer<HWindow> ptr_t;
protected:
	bool								_initialised;		/*!< was window properly initialised? */
	bool								_needRepaint;		/*!< \brief Does this window need to be repainted? */
	hcore::HString			_title;					/*!< title of window */
	HControlList::model_t::cyclic_iterator _focusedChild; /*!< points to control that has focus */
	HControlList::model_t::cyclic_iterator _previousFocusedChild; /*!< control that had focus before
																									 focus went to status bar */	
	HControlList				_controls;	/*!< list of all control inside _this_ wind */
	HStatusBarControl::ptr_t		_statusBar;
public:
	HWindow( char const* ); /* title */
	virtual ~HWindow( void );
	virtual int init( void );
	void refresh( void );
	int process_input( int );
	virtual int handler_jump_tab( int, void const* ); /* jump thru controlos with tab key */
	virtual int handler_jump_direct ( int, void const* ); /* direct jump to specified cntrl */
	virtual int handler_command( int, void const* ); /* put window into command awaiting */
	virtual int handler_search( int, void const* ); /* put window into search pattern scan */
	virtual int click( mouse::OMouse& );
	int add_control( HControl::ptr_t, int );
	HStatusBarControl::ptr_t& status_bar( void );
	hcore::HString get_command( void );
	bool is_initialised( void ) const;
	void update_all( void );
	yaal::hcore::HString const& get_title( void ) const;

/*! \brief Schedule full refresh on next refresh cycle.
 */
	void schedule_refresh( void );
private:
	friend int HControl::set_focus( char );
	friend void HStatusBarControl::set_prompt( char const *,
			HStatusBarControl::PROMPT::mode_t,
			HStatusBarControl::PROMPT::restrict_t );
	friend void HStatusBarControl::end_prompt( void );
	friend int HStatusBarControl::process_input_normal( int );
	void acquire_focus( HControl const* const );
	HWindow( HWindow const& );
	HWindow& operator = ( HWindow const& );
	};

typedef yaal::hcore::HExceptionT<HWindow, HHandlerException> HWindowException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWINDOW_HXX_INCLUDED */


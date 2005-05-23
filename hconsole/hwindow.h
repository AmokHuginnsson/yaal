/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hwindow.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCONSOLE_HWINDOW_H
#define __HCONSOLE_HWINDOW_H

#include "hcontrol.h"
#include "hcontrollist.h"
#include "hhandler.h"
#include "hstatusbarcontrol.h"

namespace stdhapi
{

namespace hconsole
{

class HControlList;

class HWindow : public HHandler
	{
protected:
	/*{*/
	hcore::HString							f_oTitle;					/* title of window */
	HControl * 					f_poFocusedChild;	/* points to control that has focus */
	HControl *					f_poPreviousFocusedChild; /* control that had focus before
																									 focus went to status bar */	
	HControlList				f_oControls;	/* list of all control inside _this_ wind */
	HStatusBarControl * f_poStatusBar;
	/*}*/
public:
	/*{*/
	HWindow ( char const * ); /* title */
	virtual ~HWindow ( void );
	virtual int init ( void );
	virtual HStatusBarControl * init_bar ( char const * );
	void refresh ( void );
	int process_input ( int );
	virtual int handler_jump_tab ( int, void * ); /* jump thru controlos with tab key */
	virtual int handler_jump_direct ( int, void * ); /* direct jump to specified cntrl */
	virtual int handler_command ( int, void * ); /* put window into command awaiting */
	virtual int handler_search ( int, void * ); /* put window into search pattern scan */
	virtual int click ( mouse::OMouse & );
	int add_control ( HControl *, int );
	HStatusBarControl * status_bar ( void );
	hcore::HString get_command ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	friend int HControl::set_focus ( char );
	friend void HStatusBarControl::set_prompt ( char const *, int, int );
	friend void HStatusBarControl::end_prompt ( void );
	friend int HStatusBarControl::process_input_normal ( int );
	void set_focus ( HControl * );
	HWindow ( const HWindow & );
	HWindow & operator = ( const HWindow & );
	/*}*/
	};

}

}

#endif /* not __HCONSOLE_HWINDOW_H */

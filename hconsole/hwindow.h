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

#ifndef __HWINDOW_H
#define __HWINDOW_H

#include "hcontrol.h"
#include "hcontrollist.h"
#include "hhandler.h"
#include "hstatusbarcontrol.h"

class HControl;
class HControlList;

class HWindow
	{
	typedef int ( HWindow::* WINDOW_HANDLER_t ) ( int );
protected:
	/*{*/
	HString							f_oTitle;					/* title of window */
	HControl * 					f_poFocusedChild;	/* points to control that has focus */
	HControl *					f_poPreviousFocusedChild; /* control that had focus before
																									 focus went to status bar */	
	HControlList				f_oControls;	/* list of all control inside _this_ wind */
	HStatusBarControl * f_poStatusBar;
	HList < HHandler < int ( HWindow::* ) ( int ) > > f_oPreprocessHandlers;
	HList < HHandler < int ( HWindow::* ) ( int ) > > f_oPostprocessHandlers;
	/*}*/
public:
	/*{*/
	HWindow ( const char * ); /* title */
	virtual ~HWindow ( void );
	virtual int init ( void );
	virtual HStatusBarControl * init_bar ( const char * );
	void refresh ( void );
	int preprocess_input ( int );
	int postprocess_input ( int );
	int process_input ( int );
	virtual int handler_jump_tab ( int ); /* jump thru controlos with tab key */
	virtual int handler_jump_direct ( int ); /* direct jump to specified cntrl */
	virtual int handler_command ( int ); /* put window into command awaiting */
	virtual int handler_search ( int ); /* put window into search pattern scan */
	virtual int click ( mouse::OMouse & );
	int add_control ( HControl *, int );
	HStatusBarControl * status_bar ( void );
	/*}*/
protected:
	/*{*/
	int register_preprocess_handler ( int, int *, WINDOW_HANDLER_t );
	int register_postprocess_handler ( int, int *, WINDOW_HANDLER_t );
	/*}*/
private:
	/*{*/
	friend int HControl::set_focus ( char );
	friend void HStatusBarControl::set_prompt ( const char * );
	void set_focus ( HControl * );
	/*}*/
	};

#endif /* not __HWINDOW_H */

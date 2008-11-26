/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hstatusbarcontrol.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HSTATUSBARCONTROL_H
#define __YAAL_HCONSOLE_HSTATUSBARCONTROL_H

#include "hconsole/heditcontrol.h"
#include "hcore/hpointer.h"

namespace yaal
{

namespace hconsole
{

/*! \brief Implementation of TUI Statusbar control class.
 *
 * Statusbar control provides display of progress bars, allows
 * invocation of commands and specyfing search patterns.
 */
class HStatusBarControl : public HEditControl
	{
public:
	typedef yaal::hcore::HPointer<HStatusBarControl> ptr_t;
	struct PROMPT
		{
		typedef enum
			{
			D_NORMAL,
			D_COMMAND,
			D_SEARCH,
			D_MENU
			} mode_t;
		typedef enum
			{
			D_RELAXED,
			D_ONLY_ENTER_CAN_QUIT,
			D_MUST_CANCEL
			} restrict_t;
		};
protected:
	int			f_iStatusBarAttribute;
	int			f_iPromptLength;
	PROMPT::mode_t f_eMode;					/* prompt mode */
	PROMPT::restrict_t f_eRestrict;	/* prompt restrict mode */
	hcore::HString	f_oPrompt;
	/* progress bar data */
	bool		f_bDone;
	bool		f_bEstimate;			/* was time left count ? */
	double	f_dProgressSize;	/* number of steps in progess bar */
	int			f_iLastProgress;	/* last drawed step */
	int			f_iLastPercent;		/* last writen percent */
	int			f_iLastMinute;		/* all last* variables help */
	int			f_iLastSecond;		/* keep progress bar refresh rate low */
	int			f_iLastStep;
	hcore::HString	f_oMessage;
	hcore::HTime		f_oStart;
	/* end of progress bar data */
public:
	HStatusBarControl( HWindow*, char const* const, int = -1 );
	virtual ~HStatusBarControl ( void );
	void setup( char const*, char const*, int );
	void set_prompt( char const * = NULL, PROMPT::mode_t = PROMPT::D_NORMAL,
			PROMPT::restrict_t = PROMPT::D_RELAXED );
	void end_prompt( void );
	void init_progress( double, char const*, bool = true );
	void update_progress( double = -1, char const * = NULL );
	void message( int, char const*, ... );
	void message( char const*, ... );
	void bar( char const* = NULL );
	int ask( char const*, char const* );
	bool confirm( char const* );
	virtual int process_input_normal( int );
	virtual int process_input_menu( int );
protected:
	virtual void do_refresh( void );
	virtual void do_draw_label( void );
	virtual int verify( void );
	virtual int do_process_input( int );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HSTATUSBARCONTROL_H */


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hstatusbarcontrol.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hstatusbarcontrol.hxx
 * \brief Declaration of HStatusBarControl class.
 */

#ifndef YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED 1

#include "hconsole/heditcontrol.hxx"
#include "hcore/hpointer.hxx"

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
	typedef HStatusBarControl this_type;
	typedef HEditControl base_type;
	typedef yaal::hcore::HPointer<HStatusBarControl> ptr_t;
	/*! \brief HStatusBarControl prompt configuration.
	 */
	struct PROMPT
		{
		/*! \brief HStatusBarControl prompt context.
		 */
		typedef enum
			{
			NORMAL,
			COMMAND,
			SEARCH,
			MENU
			} mode_t;
		/*! \brief HStatusBarControl prompt type.
		 */
		typedef enum
			{
			RELAXED,
			ONLY_ENTER_CAN_QUIT,
			MUST_CANCEL
			} restrict_t;
		};
protected:
	int			_statusBarAttribute;
	int			_promptLength;
	PROMPT::mode_t _mode;					/* prompt mode */
	PROMPT::restrict_t _restrict;	/* prompt restrict mode */
	hcore::HString	_prompt;
	/* progress bar data */
	bool		_done;
	bool		_estimate;			/* was time left count ? */
	double	_progressSize;	/* number of steps in progess bar */
	int			_lastProgress;	/* last drawed step */
	int			_lastPercent;		/* last writen percent */
	int			_lastMinute;		/* all last* variables help */
	int			_lastSecond;		/* keep progress bar refresh rate low */
	int			_lastStep;
	hcore::HString	_message;
	hcore::HTime		_start;
	/* end of progress bar data */
public:
	HStatusBarControl( HWindow*, char const* const, int = -1 );
	virtual ~HStatusBarControl ( void );
	void setup( char const*, char const*, int );
	void set_prompt( char const * = NULL, PROMPT::mode_t = PROMPT::NORMAL,
			PROMPT::restrict_t = PROMPT::RELAXED );
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
	virtual bool do_hit_test( int, int ) const;
	virtual int verify( void );
	virtual int do_process_input( int );
	};

typedef yaal::hcore::HExceptionT<HStatusBarControl, HEditControlException> HStatusBarControlException; 

}

}

#endif /* #ifndef YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED */


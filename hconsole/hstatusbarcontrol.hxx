/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hstatusbarcontrol.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hstatusbarcontrol.hxx
 * \brief Declaration of HStatusBarControl class.
 */

#ifndef YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED 1

#include "hconsole/heditcontrol.hxx"
#include "hconsole/htuiprocess.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Statusbar control class.
 *
 * Statusbar control provides display of progress bars, allows
 * invocation of commands and specyfing search patterns.
 */
class HStatusBarControl : public HEditControl {
public:
	typedef HStatusBarControl this_type;
	typedef HEditControl base_type;
	typedef yaal::hcore::HPointer<HStatusBarControl> ptr_t;
	typedef yaal::hcore::HPair<yaal::hcore::HString, HTUIProcess::call_t> choice_t;
	typedef yaal::hcore::HArray<choice_t> choices_t;
	/*! \brief HStatusBarControl prompt configuration.
	 */
	struct PROMPT {
		/*! \brief HStatusBarControl prompt mode.
		 */
		typedef enum {
			NORMAL,
			COMMAND,
			SEARCH,
			DIALOG,
			MENU
		} mode_t;
	};
protected:
	int    _statusBarAttribute;
	int    _promptLength;
	PROMPT::mode_t _mode;         /* prompt mode */
	hcore::HString _prompt;
	/* progress bar data */
	bool   _done;
	bool   _estimate;     /* was time left count ? */
	double _progressSize; /* number of steps in progess bar */
	int    _lastProgress; /* last drawed step */
	int    _lastPercent;  /* last writen percent */
	int    _lastMinute;   /* all last* variables help */
	int    _lastSecond;   /* keep progress bar paint rate low */
	int    _lastStep;
	int    _currentChoice;
	hcore::HString _message;
	hcore::HTime   _start;
	choices_t _choices;
	/* end of progress bar data */
public:
	HStatusBarControl( HWindow*, char const* const, int = -1 );
	virtual ~HStatusBarControl ( void );
	void setup( char const*, char const*, int );
	void set_prompt( yaal::hcore::HString const&, PROMPT::mode_t = PROMPT::NORMAL );
	void end_prompt( void );
	void init_progress( double, char const*, bool = true );
	void update_progress( double = -1, char const * = NULL );
	void message( int, char const*, ... ) __attribute__(( format( printf, 3, 4 ) ));
	void message( char const*, ... ) __attribute__(( format( printf, 2, 3 ) ));
	void clear( int );
	void bar( char const* = NULL );
	void ask( char const*, choices_t const&, int = -1 );
	void confirm( char const*, HTUIProcess::call_t, HTUIProcess::call_t );
	bool dialog( yaal::hcore::HString const& );
	virtual int process_input_normal( int );
	virtual int process_input_menu( int );
protected:
	virtual void do_paint( void );
	virtual void do_draw_label( void );
	virtual bool do_hit_test( int, int ) const;
	virtual int verify( void );
	virtual int do_process_input( int );
};

typedef yaal::hcore::HExceptionT<HStatusBarControl, HEditControlException> HStatusBarControlException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HSTATUSBARCONTROL_HXX_INCLUDED */


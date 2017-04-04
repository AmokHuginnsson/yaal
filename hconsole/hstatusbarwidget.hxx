/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

  hstatusbarwidget.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hstatusbarwidget.hxx
 * \brief Declaration of HStatusBarWidget class.
 */

#ifndef YAAL_HCONSOLE_HSTATUSBARWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HSTATUSBARWIDGET_HXX_INCLUDED 1

#include "hconsole/heditwidget.hxx"
#include "hconsole/htuiprocess.hxx"
#include "hconsole/hconsole.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hformat.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Statusbar control class.
 *
 * Statusbar control provides display of progress bars, allows
 * invocation of commands and specifying search patterns.
 */
class HStatusBarWidget : public HEditWidget, yaal::hcore::HFormatter {
public:
	typedef HStatusBarWidget this_type;
	typedef HEditWidget base_type;
	typedef yaal::hcore::HPointer<HStatusBarWidget> ptr_t;
	typedef yaal::hcore::HPair<yaal::hcore::HString, HTUIProcess::call_t> choice_t;
	typedef yaal::hcore::HArray<choice_t> choices_t;
	/*! \brief HStatusBarWidget prompt mode configuration.
	 */
	enum class PROMPT {
		NORMAL,
		COMMAND,
		SEARCH,
		DIALOG,
		MENU
	};
protected:
	OAttribute _statusBarAttribute;
	int    _promptLength;
	PROMPT _mode;         /* prompt mode */
	hcore::HString _prompt;
	/* progress bar data */
	bool   _done;
	bool   _estimate;     /* was time left count ? */
	double _progressSize; /* number of steps in progress bar */
	int    _lastProgress; /* last drawn step */
	int    _lastPercent;  /* last written percent */
	int    _lastMinute;   /* all last* variables help */
	int    _lastSecond;   /* keep progress bar paint rate low */
	int    _lastStep;
	int    _currentChoice;
	hcore::HString _message;
	hcore::HTime   _start;
	choices_t _choices;
	/* end of progress bar data */
public:
	HStatusBarWidget( HWindow*, yaal::hcore::HString const&, HWidget::OAttribute const& = _attributeStatusBar_ );
	virtual ~HStatusBarWidget ( void );
	void setup( char const*, char const*, int );
	void set_prompt( yaal::hcore::HString const&, PROMPT = PROMPT::NORMAL );
	void end_prompt( void );
	void init_progress( double, char const*, bool = true );
	void update_progress( double = -1, char const * = nullptr );
	template<typename... T>
	void message( COLOR::color_t color_, T const&... a_ ) {
		return ( do_message( color_, do_format( a_... ) ) );
	}
	template<typename... T>
	void message( T const&... a_ ) {
		return ( do_message( do_format( a_... ) ) );
	}
	void clear( COLOR::color_t );
	void bar( char const* = nullptr );
	void ask( char const*, choices_t const&, int = -1 );
	void confirm( char const*, HTUIProcess::call_t, HTUIProcess::call_t );
	bool dialog( yaal::hcore::HString const& );
	virtual int process_input_normal( int );
	virtual int process_input_menu( int );
protected:
	void do_message( COLOR::color_t, yaal::hcore::HString const& );
	void do_message( yaal::hcore::HString const& );
	virtual void do_paint( void ) override;
	virtual void do_draw_label( void ) override;
	virtual bool do_hit_test( int, int ) const override;
	virtual int do_process_input( int ) override;
};

typedef yaal::hcore::HExceptionT<HStatusBarWidget, HEditWidgetException> HStatusBarWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HSTATUSBARWIDGET_HXX_INCLUDED */


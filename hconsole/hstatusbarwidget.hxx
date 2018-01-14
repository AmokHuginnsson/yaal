/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
	void message( tools::COLOR::color_t color_, T const&... a_ ) {
		return ( do_message( color_, do_format( a_... ) ) );
	}
	template<typename... T>
	void message( T const&... a_ ) {
		return ( do_message( do_format( a_... ) ) );
	}
	void clear( tools::COLOR::color_t );
	void bar( char const* = nullptr );
	void ask( char const*, choices_t const&, int = -1 );
	void confirm( char const*, HTUIProcess::call_t, HTUIProcess::call_t );
	bool dialog( yaal::hcore::HString const& );
	virtual int process_input_normal( int );
	virtual int process_input_menu( int );
protected:
	void do_message( tools::COLOR::color_t, yaal::hcore::HString const& );
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


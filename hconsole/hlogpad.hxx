/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED
#define YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI LogPad control class.
 *
 * LogPad is read only TUI control that is able to display formatted text.
 */
class HLogPad : public HWidget {
public:
	typedef HLogPad this_type;
	typedef HWidget base_type;
private:
	/*! \brief Basic unit of data for HLogPad display.
	 */
	class HLogLine {
		typedef enum {
			NONE,
			ATTRIBUTE,
			TEXT,
			TEXT_EOL
		} type_t;
		type_t _type;
		tools::COLOR::color_t _attribute;
		hcore::HString _text;
	public:
		HLogLine( void );
		virtual ~HLogLine( void );
		friend class HLogPad;
	};
	typedef hcore::HList<HLogLine> contents_t;
	int _lines;
	int _offsetRow;
	int _offsetColumn;
	tools::COLOR::color_t _attribute;
	contents_t _contents;
public:
	HLogPad( HWindow*, int, int, int, int, yaal::hcore::HString const&,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HLogPad( void );
	void add( tools::COLOR::color_t, yaal::hcore::HString const& );
	void add( yaal::hcore::HString const& );
	void add( tools::COLOR::color_t );
protected:
	void add( HLogLine const& );
	virtual int do_process_input( int ) override;
	virtual void do_paint( void ) override;
};

typedef yaal::hcore::HExceptionT<HLogPad, HWidgetException> HLogPadException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED */


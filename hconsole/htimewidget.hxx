/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED 1

#include "hcore/htime.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Time widget class.
 *
 * Time widget allows setting and/or getting time.
 */
class HTimeWidget : public virtual HWidget {
public:
	typedef HTimeWidget this_type;
	typedef HWidget base_type;
private:
	yaal::hcore::HTime _time;
	yaal::hcore::HTime _selectedTime;
	int _activeItemIdx;
public:
	HTimeWidget( HWindow*, int, int, yaal::hcore::HString const&,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HTimeWidget( void );
protected:
	virtual void do_paint( void ) override;
	virtual int do_process_input( int ) override;
	virtual bool do_click( mouse::OMouse& ) override;
	virtual void do_set_data( HInfo const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTIMEWIDGET_HXX_INCLUDED */


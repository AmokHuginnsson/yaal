/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdatadatewidget.hxx
 * \brief Declaration of HDataDateWidget class.
 */

#ifndef YAAL_HDATA_HDATADATEWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATADATEWIDGET_HXX_INCLUDED 1

#include "hconsole/hdatewidget.hxx"
#include "hdata/hdatawindow.hxx"

namespace yaal {

namespace hdata {

/*! \brief Database enabled TUI Edit widget.
 *
 * This TUI widget support automatic synchronization with document query column.
 */
class HDataDateWidget : public HDataWidget, public virtual hconsole::HDateWidget {
public:
	typedef HDataDateWidget this_type;
	typedef HDateWidget base_type;
private:
public:
	HDataDateWidget( HDataWindow*, /* parent */
			int, /* top ( parent relative ) */
			int, /* left */
			int, /* height */
			int, /* width */
			yaal::hcore::HString const& = "", /* label */
			hconsole::HWidgetAttributesInterface const& = hconsole::HWidgetAttributesInterface() );
};

class HDataDateWidgetCreator : virtual public yaal::hconsole::HDateWidgetCreator {
protected:
	virtual hconsole::HWidget::ptr_t do_new_instance( hconsole::HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATADATEWIDGET_HXX_INCLUDED */


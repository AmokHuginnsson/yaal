/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HDATA_HDATATREEWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATATREEWIDGET_HXX_INCLUDED 1

#include "hconsole/htreewidget.hxx"
#include "hdata/hdatawidget.hxx"

namespace yaal {

namespace hdata {

class HDataWindow;

/*! \brief Database connected tree based TUI widget.
 */
class HDataTreeWidget : public HDataWidget, public virtual hconsole::HTreeWidget {
public:
	HDataTreeWidget( HDataWindow*, int, int, int, int, yaal::hcore::HString const&,
			hconsole::HWidgetAttributesInterface const& = hconsole::HWidgetAttributesInterface() );
	virtual ~HDataTreeWidget( void );
	virtual void do_load( int long ) override;
};

class HDataTreeWidgetCreator : virtual public yaal::hconsole::HTreeWidgetCreator {
protected:
	virtual hconsole::HWidget::ptr_t do_new_instance( hconsole::HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATATREEWIDGET_HXX_INCLUDED */


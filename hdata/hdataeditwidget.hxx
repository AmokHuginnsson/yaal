/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdataeditwidget.hxx
 * \brief Declaration of HDataEditWidget class.
 */

#ifndef YAAL_HDATA_HDATAEDITWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATAEDITWIDGET_HXX_INCLUDED 1

#include "hconsole/heditwidget.hxx"
#include "hdata/hdatawindow.hxx"

namespace yaal {

namespace hdata {

/*! \brief Database enabled TUI Edit widget.
 *
 * This TUI widget support automatic synchronization with document query column.
 */
class HDataEditWidget : public HDataWidget, public virtual hconsole::HEditWidget {
public:
	typedef HDataEditWidget this_type;
	typedef HEditWidget base_type;
private:
	hcore::HRegex _valid;
	OAttribute _attributeCache;
public:
	HDataEditWidget( HDataWindow*, /* parent */
			int, /* top ( parent relative ) */
			int, /* left */
			int, /* height */
			int, /* width */
			yaal::hcore::HString const& = "", /* label */
			hconsole::HWidgetAttributesInterface const& = hconsole::HWidgetAttributesInterface() );
	void set_validator( yaal::hcore::HString const& );
	bool is_valid( void ) const;
	virtual int do_process_input( int ) override;
};

class HDataEditWidgetAttributes : virtual public hconsole::HEditWidgetAttributes {
	yaal::hcore::HString _valid;
	bool _validSet;
protected:
	virtual void do_apply( hconsole::HWidget& ) const override;
public:
	HDataEditWidgetAttributes( void );
	HDataEditWidgetAttributes& valid( yaal::hcore::HString const& );
};

class HDataEditWidgetCreator : virtual public yaal::hconsole::HEditWidgetCreator {
protected:
	virtual hconsole::HWidget::ptr_t do_new_instance( hconsole::HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_prepare_attributes( hconsole::HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAEDITWIDGET_HXX_INCLUDED */


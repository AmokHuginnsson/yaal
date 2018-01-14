/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdatacomboboxwidget.hxx
 * \brief Declaration of HDataComboboxWidget class.
 */

#ifndef YAAL_HDATA_HDATACOMBOBOXWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATACOMBOBOXWIDGET_HXX_INCLUDED 1

#include "tools/htwowaymap.hxx"
#include "hconsole/hcomboboxwidget.hxx"
#include "hdata/hdatawindow.hxx"

namespace yaal {

namespace hdata {

/*! \brief Database enabled TUI Edit widget.
 *
 * This TUI widget support automatic synchronization with document query column.
 */
class HDataComboboxWidget : public HDataWidget, public virtual hconsole::HComboboxWidget {
public:
	typedef HDataComboboxWidget this_type;
	typedef HComboboxWidget base_type;
	typedef tools::HTwoWayMap<int, int> index_id_t;
private:
	index_id_t _indexId;
	yaal::hcore::HString _dictName;
public:
	HDataComboboxWidget( HDataWindow*, /* parent */
			int, /* top ( parent relative ) */
			int, /* left */
			int, /* height */
			int, /* width */
			yaal::hcore::HString const& = "", /* label */
			hconsole::HWidgetAttributesInterface const& = hconsole::HWidgetAttributesInterface() );
	void set_dict_name( yaal::hcore::HString const& );
protected:
	virtual void do_update( void ) override;
	virtual hconsole::HInfo const& do_get_data( void ) const override;
	virtual void do_set_data( hconsole::HInfo const& ) override;
private:
	void init( void );
};

class HDataComboboxWidgetAttributes : public virtual hconsole::HComboboxWidgetAttributes {
	yaal::hcore::HString _dictName;
	bool _dictNameSet;
protected:
	virtual void do_apply( hconsole::HWidget& ) const override;
public:
	HDataComboboxWidgetAttributes( void );
	HDataComboboxWidgetAttributes& dict_name( yaal::hcore::HString const& );
};

class HDataComboboxWidgetCreator : virtual public yaal::hconsole::HComboboxWidgetCreator {
protected:
	virtual hconsole::HWidget::ptr_t do_new_instance( hconsole::HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_prepare_attributes( hconsole::HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATACOMBOBOXWIDGET_HXX_INCLUDED */


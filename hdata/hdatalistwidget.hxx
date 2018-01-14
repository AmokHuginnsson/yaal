/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdatalistwidget.hxx
 * \brief Declaration of HDataListWidget class.
 */

#ifndef YAAL_HDATA_HDATALISTWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATALISTWIDGET_HXX_INCLUDED 1

#include "hconsole/hlistwidget.hxx"
#include "hdata/hdatawidget.hxx"
#include "hdata/hdictionary.hxx"

namespace yaal {

namespace hdata {

class HDataWindow;

/*! \brief Database enabled list based TUI widget.
 *
 * This TUI widget support automatic display of database content.
 */
class HDataListWidget : public HDataWidget, public virtual yaal::hconsole::HListWidget {
public:
	typedef HDataListWidget this_type;
	typedef yaal::hconsole::HListWidget base_type;
	class HDataColumnInfo : public HListWidget::HColumnInfo {
	public:
		HDictionary::ptr_t _dict;
		HDataColumnInfo(
				yaal::hcore::HString const& columnName,
				int width,
				BITS::ALIGN align = BITS::ALIGN::LEFT,
				yaal::TYPE type = yaal::TYPE::HSTRING,
				yaal::hcore::HString const& format_ = hcore::HString(),
				HWidget* associatedWidget = nullptr,
				HDictionary::ptr_t const& = HDictionary::ptr_t() );
	};
private:
	yaal::hcore::HString _table;
	yaal::hcore::HString _columns;
	yaal::hcore::HString _filterColumn;
	yaal::hcore::HString _idColumn;
	yaal::hcore::HString _sort;
public:
	yaal::hconsole::list_widget_helper::HAsIsValueListModel<>::ptr_t _dataModel;
	HDataListWidget( HDataWindow*, int, int, int, int, yaal::hcore::HString const&,
			hconsole::HWidgetAttributesInterface const& = hconsole::HWidgetAttributesInterface() );
	virtual ~HDataListWidget( void );
	void set_record_descriptor(
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const& );
protected:
	virtual int long do_get_current_id( void ) override;
	virtual void do_load( int long ) override;
	virtual void do_save( int long ) override;
	virtual void do_add_new( void ) override;
	virtual void do_cancel_new( void ) override;
private:
	void make_crud( int long );
};

class HDataListWidgetCreator : virtual public yaal::hconsole::HListWidgetCreator {
protected:
	virtual hconsole::HWidget::ptr_t do_new_instance( hconsole::HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_apply_resources( hconsole::HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual hconsole::HListWidget::HColumnInfo::ptr_t do_make_column(
			yaal::tools::HXml::HConstNodeProxy const&,
			hconsole::HListWidget*,
			yaal::hcore::HString const& columnName,
			int width,
			hconsole::HListWidget::BITS::ALIGN align,
			yaal::TYPE type,
			yaal::hcore::HString const&,
			hconsole::HWidget* associatedWidget ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATALISTWIDGET_HXX_INCLUDED */


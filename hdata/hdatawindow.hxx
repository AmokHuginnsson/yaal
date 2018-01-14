/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED
#define YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED 1

#include "dbwrapper/hdatabase.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hlistwidget.hxx"
#include "hdata/hdatawidget.hxx"
#include "hdata/hdatalistwidget.hxx"
#include "hdata/hdictionary.hxx"

namespace yaal {

namespace hdata {

class HDataProcess;

/*! \brief Data connected TUI Window implementation.
 *
 * This class automates DML operations.
 */
class HDataWindow : public hconsole::HWindow {
public:
	typedef HDataWindow this_type;
	typedef HWindow base_type;
	/*! \brief Data window operation mode.
	 */
	enum DOCUMENT {
		VIEW, /*!< Browse data. */
		EDIT  /*!< Edit current record. */
	};
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, HDictionary::ptr_t> dictionaries_t;
protected:
	bool _modified;
	DOCUMENT _documentMode;
	HDataWidget* _mainWidget;
	typedef yaal::hcore::HList<HDataWidget*> controls_t;
	controls_t _viewModeWidgets;
	controls_t _editModeWidgets;
	yaal::dbwrapper::HCRUDDescriptor::ptr_t _crud;
	yaal::dbwrapper::HCRUDDescriptor::MODE _mode;
	yaal::dbwrapper::HCRUDDescriptor::field_names_t _columns;
	yaal::hcore::HString _idColumnName;
	dictionaries_t _dictionaries;
public:
	HDataWindow( yaal::hcore::HString const&, HDataProcess* );
	virtual ~HDataWindow( void );
	bool is_modified( void ) const;
	void set_modified( bool = true );
	void sync( yaal::dbwrapper::HRecordSet::iterator );
	void set_widget_role( HDataWidget*, HDataWidget::ROLE );
	void set_record_descriptor(
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const& );
	yaal::hcore::HString const& id_column_name( void ) const;
	int long get_current_id( void ) const;
	void add_dictionary( yaal::hcore::HString const&, HDictionary::ptr_t const& );
	HDictionary::ptr_t get_dictionary( yaal::hcore::HString const& );
protected:
	void set_mode( DOCUMENT );
	void sync( void );
	void sync( int, hconsole::HEditWidget& );
	void sync( int, HDataListWidget& );
	bool handler_add_new( hconsole::HEvent const& );
	bool handler_edit( hconsole::HEvent const& );
	bool handler_delete( hconsole::HEvent const& );
	bool handler_save( hconsole::HEvent const& );
	bool handler_requery( hconsole::HEvent const& );
	bool handler_cancel( hconsole::HEvent const& );
	bool on_sel_change( yaal::hconsole::HEvent const& );
	virtual void do_init( void ) override;
	virtual hconsole::HStatusBarWidget* do_init_bar( yaal::hcore::HString const& ) override;
private:
	HDataWindow( HDataWindow const& ) = delete;
	HDataWindow& operator = ( HDataWindow const& ) = delete;
	void reload_record( void );
};

class HDataWindowCreator : public hconsole::HWindowCreator {
protected:
	virtual hconsole::HWindow::ptr_t do_new_instance( hconsole::HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_apply_resources( hconsole::HTUIProcess*, window_ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED */


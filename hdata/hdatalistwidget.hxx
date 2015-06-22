/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatalistwidget.hxx - this file is integral part of `yaal' project.

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
				BITS::ALIGN::align_t const& align = BITS::ALIGN::LEFT,
				yaal::TYPE type = yaal::TYPE::HSTRING,
				yaal::hcore::HString const& format_ = hcore::HString(),
				HWidget* associatedWidget = NULL,
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
			hconsole::HListWidget::BITS::ALIGN::align_t const& align,
			yaal::TYPE type,
			yaal::hcore::HString const&,
			hconsole::HWidget* associatedWidget ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATALISTWIDGET_HXX_INCLUDED */


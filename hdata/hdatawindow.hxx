/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.hxx - this file is integral part of `yaal' project.

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
	struct DOCUMENT {
		/*! \brief Data window operation mode.
		 */
		typedef enum {
			VIEW, /*!< Browse data. */
			EDIT  /*!< Edit current record. */
		} mode_t;
	};
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, HDictionary::ptr_t> dictionaries_t;
protected:
	bool _modified;
	DOCUMENT::mode_t _documentMode;
	HDataWidget* _mainWidget;
	typedef yaal::hcore::HList<HDataWidget*> controls_t;
	controls_t _viewModeWidgets;
	controls_t _editModeWidgets;
	HDataProcess* _owner;
	yaal::dbwrapper::HCRUDDescriptor::ptr_t _crud;
	yaal::dbwrapper::HCRUDDescriptor::MODE::mode_t _mode;
	yaal::hcore::HString _idColumnName;
	dictionaries_t _dictionaries;
public:
	HDataWindow( yaal::hcore::HString const&, HDataProcess* );
	virtual ~HDataWindow( void );
	bool is_modified( void ) const;
	void set_modified( bool = true );
	void sync( yaal::dbwrapper::HRecordSet::iterator );
	void set_widget_role( HDataWidget*, HDataWidget::ROLE::role_t );
	void set_record_descriptor(
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const& );
	yaal::hcore::HString const& id_column_name( void ) const;
	void add_dictionary( yaal::hcore::HString const&, HDictionary::ptr_t const& );
	HDictionary::ptr_t get_dictionary( yaal::hcore::HString const& );
protected:
	void set_mode( DOCUMENT::mode_t );
	void sync( void );
	void sync( int, hconsole::HEditWidget& );
	void sync( int, HDataListWidget& );
	bool handler_add_new( hconsole::HEvent const& );
	bool handler_edit( hconsole::HEvent const& );
	bool handler_delete( hconsole::HEvent const& );
	bool handler_save( hconsole::HEvent const& );
	bool handler_requery( hconsole::HEvent const& );
	bool handler_cancel( hconsole::HEvent const& );
	virtual void do_init( void );
	virtual hconsole::HStatusBarWidget* do_init_bar( char const* );
private:
	HDataWindow( HDataWindow const& );
	HDataWindow& operator = ( HDataWindow const& );
};

class HDataWindowCreator : public hconsole::HWindowCreator {
protected:
	virtual hconsole::HWindow::ptr_t do_new_instance( hconsole::HTUIProcess*, yaal::tools::HXml::HConstNodeProxy const& );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAWINDOW_HXX_INCLUDED */


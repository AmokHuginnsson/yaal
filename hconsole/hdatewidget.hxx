/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatewidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HDATEWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HDATEWIDGET_HXX_INCLUDED 1

#include "hcore/htime.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hinfotime.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Date widget class.
 *
 * Date widget allows setting and/or getting dates.
 */
class HDateWidget : public virtual HWidget {
public:
	typedef HDateWidget this_type;
	typedef HWidget base_type;
private:
	enum class MODE {
		VIEW,
		EDIT
	};
	enum class ACTION {
		APPLY,
		CANCEL
	};
	yaal::hcore::HTime _time;
	yaal::hcore::HTime _selectedTime;
	HInfoTime _infoTime;
	MODE _mode;
public:
	HDateWidget( HWindow*, int, int, yaal::hcore::HString const&,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HDateWidget( void );
protected:
	virtual void do_paint( void ) override;
	virtual int do_process_input( int ) override;
	virtual bool do_click( mouse::OMouse& ) override;
	virtual HInfo const& do_get_data( void ) const override;
	virtual void do_set_data( HInfo const& ) override;
	virtual void do_kill_focus( void ) override;
	void on_key_down( void );
	void on_key_up( void );
	void on_key_left( void );
	void on_key_right( void );
	void on_key_page_down( void );
	void on_key_page_up( void );
	void on_key_enter( void );
	void close_calendar( ACTION );
	yaal::hcore::HString const& week_day_name( int );
	int get_first_day_of_month_in_week_index( void ) const;
};

class HDateWidgetCreator : virtual public HWidgetCreatorInterface {
protected:
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HDATEWIDGET_HXX_INCLUDED */


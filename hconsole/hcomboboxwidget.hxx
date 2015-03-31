/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxwidget.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hcomboboxwidget.hxx
 * \brief Declaration of HComboboxWidget class.
 */

#ifndef YAAL_HCONSOLE_HCOMBOBOXWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HCOMBOBOXWIDGET_HXX_INCLUDED 1

#include "hconsole/heditwidget.hxx"
#include "hconsole/hlistwidget.hxx"
#include "hconsole/hinfointeger.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Combobox control class.
 *
 * Combobox control allows fancy representation of row based data with handful
 * of display alteration methods and convenient text editing together with
 * advanced automatic text operations.
 */
class HComboboxWidget : public virtual HEditWidget,
	public virtual HListWidget {
public:
	typedef HComboboxWidget this_type;
	typedef HListWidget base_type;
	/*! \brief Current combobox state.
	 */
	struct MODE {
		/*! \brief Current combobox state.
		 */
		typedef enum {
			EDITCONTROL, /*!< Combobox retracted. */
			LISTCONTROL  /*!< Combobox expanded. */
		} mode_t;
	};
	struct ACTION {
		typedef enum {
			APPLY,
			CANCEL
		} action_t;
	};
protected:
	MODE::mode_t _mode; /*!< operating mode (MODE_EDITCONTROL|MODE_LISTCONTROL) */
	int _droppedWidth;  /*!< width of dropped list */
	struct OSelection {
		iterator_t _cursor;
		iterator_t _firstVisibleRow;
		int  _widgetOffset;
		int  _cursorPosition;
		OSelection( void )
			: _cursor(), _firstVisibleRow(), _widgetOffset( 0 ), _cursorPosition( 0 ) {
		}
	} _origSelection;
	mutable HInfoInteger _infoInteger;
	HInfoItem* _none;
	yaal::hcore::HString _noneText;
public:
	HComboboxWidget( HWindow* parent,
			int row, int col, int height, int width,
			yaal::hcore::HString const& label,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface()
	);
	virtual ~HComboboxWidget ( void );
	void set_dropped_width( int );
	void select_by_index( int );
	void set_none_text( yaal::hcore::HString const& );
	int get_selected_index( void ) const;
	yaal::hcore::HString const& get_selected_text( void ) const;
protected:
	void save_selection( void );
	void restore_selection( void );
	void prepend_none( void );
	virtual void do_update( void ) override;
	virtual int do_process_input( int ) override;
	virtual void do_paint( void ) override;
	virtual bool do_hit_test( int, int ) const override;
	virtual bool do_click( mouse::OMouse& ) override;
	virtual void do_kill_focus( void ) override; /* this one will be used here */
	virtual HInfo const& do_get_data( void ) const override;
	virtual void do_set_data( HInfo const& ) override;
private:
	void close_combo( ACTION::action_t );
	HComboboxWidget( HComboboxWidget const& ) = delete;
	HComboboxWidget& operator = ( HComboboxWidget const& ) = delete;
};

typedef yaal::hcore::HExceptionT<HComboboxWidget, HListWidgetException> HComboboxWidgetException;

class HComboboxWidgetAttributes : public virtual HEditWidgetAttributes, public virtual HListWidgetAttributes {
	int _droppedWidth;
	bool _droppedWidthSet;
	yaal::hcore::HString _noneText;
	bool _noneTextSet;
protected:
	virtual void do_apply( HWidget& ) const override;
public:
	HComboboxWidgetAttributes( void );
	HComboboxWidgetAttributes& dropped_width( int );
	HComboboxWidgetAttributes& none_text( yaal::hcore::HString const& );
};

class HComboboxWidgetCreator : public virtual HListWidgetCreator, public virtual HEditWidgetCreator {
protected:
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HCOMBOBOXWIDGET_HXX_INCLUDED */


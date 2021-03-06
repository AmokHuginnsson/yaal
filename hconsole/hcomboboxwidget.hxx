/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
	enum class MODE {
		EDITCONTROL, /*!< Combobox retracted. */
		LISTCONTROL  /*!< Combobox expanded. */
	};
	enum class ACTION {
		APPLY,
		CANCEL
	};
protected:
	MODE _mode; /*!< operating mode (MODE_EDITCONTROL|MODE_LISTCONTROL) */
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
	void close_combo( ACTION );
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


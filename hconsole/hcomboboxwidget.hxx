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
protected:
	MODE::mode_t _mode; /* operating mode (MODE_EDITCONTROL|MODE_LISTCONTROL) */
	int _droppedWidth;	/* width of dropped list */
public:
	HComboboxWidget ( HWindow* parent,
										 int row,
										 int col,
										 int height,
										 int width,
										 char const * label,
										 int droppedWidth = 0,
										 int maxStringLength = 127,
										 char const* regexp = _maskDefault_,
										 bool searchable = true );
	virtual ~HComboboxWidget ( void );
	virtual int kill_focus( void ); /* this one will be used here */
protected:
	virtual int do_process_input( int );
	virtual void do_paint( void );
	virtual bool do_click( mouse::OMouse& );
private:
	void close_combo ( void );
};

typedef yaal::hcore::HExceptionT<HComboboxWidget, HListWidgetException> HComboboxWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HCOMBOBOXWIDGET_HXX_INCLUDED */


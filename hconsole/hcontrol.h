/*
---          `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCONSOLE_HCONTROL_H
#define __YAAL_HCONSOLE_HCONTROL_H

#include "hconsole/console.h"
#include "hcore/hpointer.h"
#include "hcore/hstring.h"
#include "hcore/hinfo.h"

namespace yaal
{

namespace hconsole
{

class HWindow;

/*! \brief Base class for all TUI classes.
 *
 * This class is a common interface for all TUI classes.
 */
class HControl
	{
public:
	struct BITS
		{
		struct ALIGN
			{
			typedef enum
				{
				D_LEFT,
				D_CENTER,
				D_RIGHT
				} align_t;
			};
		};
	static int const D_DEFAULT_ATTRS = -1;
	typedef yaal::hcore::HPointer<HControl,yaal::hcore::HPointerScalar,yaal::hcore::HPointerRelaxed> ptr_t;
protected:
	bool			f_bEnabled;						/*!< Tells if control is enabled, focus can go only to enabled control. */
	bool			f_bFocused;						/*!< Tells if control has focus. */
	bool			f_bDrawLabel;					/*!< Will be label driven? */
	bool 			f_bSingleLine;				/*!< Is label in the same line as top of control? */
	/** \name Control attributes.
	 * High byte of attribute, in all
	 * three cases keeps label (control title)
	 * color, low byte keeps work place color */
	//@{
	int unsigned f_uiAttributeDisabled;	/*!< The attribute of disabled cotrol. */
	int unsigned f_uiAttributeEnabled;	/*!< The attribute of enabled cotrol. */	
	int unsigned f_uiAttributeFocused;	/*!< The attribute of focused cotrol. */
	//@}
	int				f_iRow;								/*!< Control top coordinate relative
																		 to parent window (top or bottom border). */
	int				f_iColumn;						/*!< Control left coordinate relative
																		 to parent window (left or right border). */
	int				f_iHeight;						/*!< Height or coorinate of bottom of control. */
	int				f_iWidth;							/*!< Width or coorinate of right of control. */
	int				f_iRowRaw;						/*!< Control top coordinate relative
																		 to parent window (after repositioning). */
	int				f_iColumnRaw;					/*!< Control left coordinate relative
																		 to parent window (after repositioning). */
	int				f_iHeightRaw;					/*!< Drawing state keeping helper. */
	int				f_iWidthRaw;					/*!< Drawing state keeping helper. */
	hcore::HString	f_oLabel;					/*!< Control title. */
	hcore::HString	f_oVarTmpBuffer;	/*!< Buffer for temporary operations, used
																			 to keep memory fragmentation low. */
	HWindow* f_poParent;						/*!< Window that will hold this control. */
/* this is used locally, there is no way to modify this variables explicitly */
	int				f_iLabelLength;	 			/*!< Length of the label. */
	int				f_iShortcutIndex;			/*!< Index of shortcut char in label. */
	bool			f_bValid;
public:
/** \brief Control constructor.
 *
 * When library user wants to create new control he must provision
 * basic parameters shared by all types of TUI controls.
 *
 * \param parent - The parent window for this control.
 * \param row - Control top coordinate relative to parent window (top or bottom border)
 * \param col - Control left coordinate relative to parent window (left or right border).
 * \param height - Height or coorinate of bottom of control.
 * \param width - Width or coorinate of right of control.
 * \param label - Control title.
 */
	HControl( HWindow* parent, int row, int col, int height, int width, char const* label );

/** \brief Control destructor.
 *
 * Trivial implementation.
 */
	virtual ~HControl( void );

/*! \brief Change enabled/disabled state of the control.
 *
 * \param enabled - Tells if control shall be enabled.
 */
	void enable( bool enabled );

/*! \brief Process keyboard input.
 *
 * All keyboard input for this control shall be handled inside this method call.
 *
 * \param keycode - Internal code for keypress event.
 * \return Unhandled portion of key press.
 */
	int process_input( int keycode );

/*! \brief Focus managing method (gain).
 *
 * Set focus if control is enabled or if shortcut char is specified.
 *
 * \param shortcut - Alphanumeric portion of keypress event that possibly holds shortcut to this control.
 * \return If focus was successfuly switched return 0, otherwise return 1.
 */
	virtual int set_focus( char shortcut = 0 ); /* focused and shortcut char match */

/*! \brief Focus managing method (lose).
 *
 * Allows for focus removal from control owner context.
 *
 * \return Return 0 if focus was really dropped, otherwise return 1.
 */
	virtual int kill_focus( void );

/*! \brief Interface for setting control data.
 *
 * Some controls store data directly, this interface allows for setting
 * data from control owner context.
 *
 * \param data - Data that shall be stored inside of this control.
 */
	virtual void set( hcore::HInfo const& data );

/*! \brief Interface for getting control data.
 *
 * Some controls store data directly, this interface allows for getting
 * data from control from control owner context.
 *
 * \return Data that has been stored inside of this control.
 */
	virtual hcore::HInfo get( void );

/*! \brief Interface for checking is particular control instance support searching.
 *
 * Some control types support search operation, this method allows for checking
 * if this particular control can be searched.
 *
 * \return True if control can be searched, false otherwise.
 */
	virtual bool is_searchable( void );

/*! \brief Refresh TUI control screen representation.
 */
	void refresh( void );

/*! \brief Update control internals to comply with new Controler data.
 *
 * The external Controler for this control changed its data,
 * now control must update its internal data structures to resychronize with
 * its Controler.
 */
	void update( void );

/*! \brief Process mouse input.
 *
 * All mouse input for this control shall be handled inside this method call.
 *
 * \param eventdesc - Full description of mouse event that occured in this control.
 * \return Return 1 if control can be focused or already has a focus, return 0 otherwise.
 */
	int click( mouse::OMouse& eventdesc );

/*! \brief Test if some point belongs to this control.
 *
 * \param row - Row of the point to be tested.
 * \param col - Column of the point to be tested.
 * \return True if point with row, col coordinates is inside of this control.
 */
	bool hit_test( int row, int col ) const;
	void set_attributes( int = D_DEFAULT_ATTRS, int = D_DEFAULT_ATTRS, int = D_DEFAULT_ATTRS );
	void set_draw_label( bool );
	void move( int, int, int, int );
	int attr_label( void ) const;
	int attr_shortcut( void ) const;
	int attr_data( void ) const;
	void set_attr_label( void ) const;
	void set_attr_shortcut( void ) const;
	void set_attr_data( void ) const;
	void schedule_refresh( void ) const;
	void invalidate( void );
protected:
	virtual int do_process_input( int );
	virtual void do_refresh( void ) = 0;
	virtual void do_update( void );
	virtual int do_click( mouse::OMouse& );
	virtual void do_draw_label( void );
	void draw_label( void );
private:
	HControl ( HControl const& );
	HControl& operator = ( HControl const& );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HCONTROL_H */


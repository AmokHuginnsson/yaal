/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCONSOLE_HCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HCONTROL_HXX_INCLUDED

#include "hconsole/console.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hinfo.hxx"

namespace yaal
{

namespace hconsole
{

class HWindow;

/*! \brief Base class for all TUI control classes.
 *
 * This class is a common interface for all TUI control classes.
 */
class HControl
	{
	typedef HControl this_type;
public:
/*! \brief Various configuration bits for HControl hierarchy.
 */
	struct BITS
		{
		/*! \brief Possible alignment types of text in HControl.
		 */
		struct ALIGN
			{
			/*! \brief Align type.
			 */
			typedef enum
				{
				LEFT, /*!< text ought to be left aligned */
				CENTER, /*!< text ought to be centered */
				RIGHT /*!< text ought to be right aligned */
				} align_t;
			};
		};
	static int const DEFAULT_ATTRS = -1; /*!< Default HControl attribites (colors of label and data fore/back-ground) */
	typedef yaal::hcore::HPointer<HControl> ptr_t;
protected:
	bool			_enabled;						/*!< Tells if control is enabled, focus can go only to enabled control. */
	bool			_focused;						/*!< Tells if control has focus. */
	bool			_drawLabel;					/*!< Will be label driven? */
	bool 			_singleLine;				/*!< Is label in the same line as top of control? */
	/** \name Control attributes.
	 * High byte of attribute, in all
	 * three cases keeps label (control title)
	 * color, low byte keeps work place color */
	//@{
	int unsigned _attributeDisabled;	/*!< The attribute of disabled cotrol. */
	int unsigned _attributeEnabled;	/*!< The attribute of enabled cotrol. */	
	int unsigned _attributeFocused;	/*!< The attribute of focused cotrol. */
	//@}
	int				_row;								/*!< Control top coordinate relative
																		 to parent window (top or bottom border). */
	int				_column;						/*!< Control left coordinate relative
																		 to parent window (left or right border). */
	int				_height;						/*!< Height or coorinate of bottom of control. */
	int				_width;							/*!< Width or coorinate of right of control. */
	int				_rowRaw;						/*!< Control top coordinate relative
																		 to parent window (after repositioning). */
	int				_columnRaw;					/*!< Control left coordinate relative
																		 to parent window (after repositioning). */
	int				_heightRaw;					/*!< Drawing state keeping helper. */
	int				_widthRaw;					/*!< Drawing state keeping helper. */
	hcore::HString	_label;					/*!< Control title. */
	hcore::HString	_varTmpBuffer;	/*!< Buffer for temporary operations, used
																			 to keep memory fragmentation low. */
	HWindow* _parent;						/*!< Window that will hold this control. */
/* this is used locally, there is no way to modify this variables explicitly */
	int				_labelLength;	 			/*!< Length of the label. */
	int				_shortcutIndex;			/*!< Index of shortcut char in label. */
	bool			_valid;							/*!< Is controler data in sync with control internals. */
	bool			_needRepaint;				/*!< Does this control need to be repainted. */
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

/*! \brief Set various foreground/background colors for a control.
 *
 * Each attribute holds information about foreground/background colors of three
 * specific parts of a control.
 *
 * Those parts are:
 *  - label
 *  - data
 *
 * \param disabled - Colors for control in disabled state.
 * \param enabled - Colors for control in enabled state.
 * \param focused - Colors for control while its focused.
 */
	void set_attributes( int disabled = DEFAULT_ATTRS, int enabled = DEFAULT_ATTRS, int focused = DEFAULT_ATTRS );

/*! \brief Decide if label shall be drawn for this particular instance of a control.
 *
 * \param draw - If set to true the label is drawn.
 */
	void set_draw_label( bool draw );

/*! \brief Set new coordinates and size for a control.
 *
 * \param row - New control vertical position.
 * \param col - New control horizontal position.
 * \param height - New control height.
 * \param width -  New control width.
 */
	void move( int row, int col, int height, int width );

/*! \brief Retrieve attribute (colors) of a controls label.
 *
 * \return Attribute of controls label.
 */
	int attr_label( void ) const;

/*! \brief Retrieve attribute (controls) of a controls shortcut highlight.
 *
 * \return Attribute of controls shortcut highlight.
 */
	int attr_shortcut( void ) const;

/*! \brief Retrieve attribute (controls) of a controls data.
 *
 * \return Attribute of controls data.
 */
	int attr_data( void ) const;

/*! \brief Schedule full refresh on next refresh cycle.
 */
	void schedule_refresh( void );

/*! \brief Mark this control as no longer synchronized with its internel controler.
 */
	void invalidate( void );

/*! \brief Tells if this control needs to be repainted.
 *
 * \return True if refresh is required.
 */
	bool need_repaint( void ) const;

protected:
	void set_attr_label( void ) const;
	void set_attr_shortcut( void ) const;
	void set_attr_data( void ) const;
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

#endif /* not YAAL_HCONSOLE_HCONTROL_HXX_INCLUDED */


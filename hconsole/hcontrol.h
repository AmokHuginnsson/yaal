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
	HControl ( HWindow* parent, int row, int col, int height, int width, char const* label );

/** \brief Control destructor.
 *
 * Trivial implementation.
 */
	virtual ~HControl ( void );

/*! \brief Change enabled/disabled state of the control.
 *
 * \param enabled - Tells if control shall be enabled.
 */
	void enable ( bool enabled );
	int process_input ( int );
	/* set focus if control is enabled or if shortcut char is specified */
	virtual int set_focus ( char = 0 ); /* focused and shortcut char match */
	virtual int kill_focus ( void );
	virtual void set ( hcore::HInfo const& );
	virtual hcore::HInfo get ( void );
	virtual bool is_searchable ( void );
	void refresh ( void );
	void update( void );
	int click ( mouse::OMouse& );
	bool hit_test ( int, int ) const;
	void set_attributes ( int = D_DEFAULT_ATTRS, int = D_DEFAULT_ATTRS, int = D_DEFAULT_ATTRS );
	void set_draw_label ( bool );
	void move ( int, int, int, int );
	int attr_label ( void ) const;
	int attr_shortcut ( void ) const;
	int attr_data ( void ) const;
	void set_attr_label ( void ) const;
	void set_attr_shortcut ( void ) const;
	void set_attr_data ( void ) const;
	void schedule_refresh( void ) const;
	void invalidate( void );
protected:
	virtual int do_process_input ( int );
	virtual void do_refresh ( void ) = 0;
	virtual void do_update( void );
	virtual int do_click ( mouse::OMouse& );
	virtual void do_draw_label ( void );
	void draw_label ( void );
private:
	HControl ( HControl const& );
	HControl& operator = ( HControl const& );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HCONTROL_H */


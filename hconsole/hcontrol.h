/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCONTROL_H
#define __HCONTROL_H

#include "console.h"
#include "../hcore/hstring.h"
#include "../hcore/hinfo.h"

class HWindow;

class HControl
	{
protected:
	/*{*/
	bool			f_bEnabled;						/* is cotrol enabled, focus can go 
																		 only to enabled control */
	bool			f_bFocused;						/* has control focus */
	bool 			f_bSingleLine;				/* is label in the same line as top of
																		 control */
	int				f_iDisabledAttribute;	/* attribute of disabled cotrol */
	int				f_iEnabledAttribute;	/* attribute of enabled cotrol */	
	int				f_iFocusedAttribute;	/* attribute of focused cotrol */
																	/* high byte of attribute in all
																		 three cases keeps label (control title)
																 		 color, low byte keeps work place color */
	int				f_iRow;								/* control top coordinate relative
																		 to parent window (top or bottom border) */
	int				f_iColumn;						/* control left coordinate relative
																		 to parent window (left or right border) */
	int				f_iHeight;						/* height or coorinate of bottom of control */
	int				f_iWidth;							/* width or coorinate of right of control */
	int				f_iRowRaw;						/* control top coordinate relative
																		 to parent window (after repositioning) */
	int				f_iColumnRaw;					/* control left coordinate relative
																		 to parent window (after repositioning) */
	int				f_iHeightRaw;					/* self explanary */
	int				f_iWidthRaw;					/* self explanary */
	HString 	f_oLabel;							/* control title */
	HString		f_oVarTmpBuffer;			/* buffer for temporary operations, used
																		 to keep memory fragmentation low */
	HWindow * f_poParent;						/* window that will hold this control */
/* this is used locally, there is no way to modify this variables explictly */
	int				f_iLabelLength;	 			/* lenght of the label */
	int				f_iShortcutIndex;			/* index of shortcut char in label */
	/*}*/
public:
	/*{*/
/* parent, row, col, height, width, label */
	HControl ( HWindow *, int, int, int, int, const char *,
/* disabled attribute, enabled-attribute, focused-attribute */
			int = -1, int = -1,	int = -1 );
	virtual ~HControl ( void );
	void enable ( bool );
	virtual int process_input ( int );
	/* set focus if control is enabled or if shortcut char is specified */
	virtual int set_focus ( char = 0 ); /* focused and shortcut char match */
	virtual int kill_focus ( void );
	virtual void refresh ( void ) = 0;
	virtual HInfo operator = ( const HInfo & );
	virtual operator HInfo ( void );
	void draw_label ( void );
	void set_attributes ( int, int, int );
	void move ( int, int, int, int );
#define M_SET_ATTR_LABEL( )	console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute >> 8 : f_iEnabledAttribute >> 8 ) : f_iDisabledAttribute >> 8 )
#define M_SET_ATTR_SHORTCUT( )	console::set_attr ( ! f_bEnabled ? ( ! f_bFocused ? f_iFocusedAttribute >> 8 : f_iEnabledAttribute >> 8 ) : f_iDisabledAttribute >> 8 )
#define M_SET_ATTR_DATA( )	console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute : f_iEnabledAttribute ) : f_iDisabledAttribute )
	/*}*/
	};

#endif /* not __HCONTROL_H */


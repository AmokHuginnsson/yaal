/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	HEditControl.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HEDITCONTROL_H
#define __YAAL_HCONSOLE_HEDITCONTROL_H

#include "hcore/hlist.h"
#include "hcore/hpattern.h"
#include "hconsole/hcontrol.h"

namespace yaal
{

namespace hconsole
{

extern char const * const n_pcMaskLetters;
extern char const * const n_pcMaskDigits;
extern char const * const n_pcMaskAlpha;
extern char const * const n_pcMaskExtended;
extern char const * const n_pcMaskLoose;
extern char const * const n_pcMaskDefault;

class HEditControl : public virtual HControl
	{
protected:
	bool				f_bReplace;						/* current edit mode */
	bool				f_bMultiLine;					/* is control multiline */
	bool				f_bReadOnly;					/* shall editing be disabled */
	bool				f_bRightAligned;			/* is control content right aligned,
																			 control content can be right aligned
																			 only in single line mode */
	bool				f_bPassword;					/* should be entered text visible */
	int					f_iMaxStringSize;			/* self explenary */
	int					f_iCursorPosition;		/* cursor position relative to control
																			 begining */
	int					f_iControlOffset;			/* when content is bigger than control size
																			 this variable keeps offset of first
																			 character shown */
	int					f_iMaxHistoryLevel;	/* how many instertions should history keep */
	hcore::HPattern			f_oPattern;		/* regular expression describing what
																			 characters and
																			 in what way can be entered */
	hcore::HString			f_oString;		/* control content */
	typedef hcore::HList< hcore::HString >	history_t;
	history_t	f_oHistory;							/* history of insertions */
	history_t::cyclic_iterator f_oHistoryIt;
public:
	HEditControl ( HWindow *,					/* parent */
								int,								/* top ( parent relative ) */
								int,								/* left */
								int,								/* height */
								int,								/* width */
								char const * = "",	/* label */
								int = 127,					/* maximum string length */
								char const * = "",	/* initial content */
								char const * = n_pcMaskDefault, /* mask */
								bool = false,				/* replace */
								bool = false,				/* multiline */
								bool = false,				/* readonly */
								bool = false,				/* right alignment */
								bool = false,				/* password */
								int = 8 );						/* max history level */
	virtual ~HEditControl ( void );
	void set_flags ( bool = false, bool = false );
	virtual hcore::HInfo get ( void );
	virtual int set_focus ( char = 0 );
	virtual void set ( hcore::HInfo const & );
protected:
	virtual void do_refresh ( void );
	virtual int do_process_input( int );
	virtual int do_click ( mouse::OMouse & );
private:
	int find_eow( int );
	int go_to_eow( int );
	int kill_line( void );
	int move_right( int );
	int move_left( void );
	int go_to_end( int );
	int delete_char( int );
	int kill_char( void );
	int find_bow( int );
	int go_to_bow( int );
	int delete_word( int );
	int kill_word( int );
	int insert_char( int, int );
	int update_from_history( void );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HEDITCONTROL_H */

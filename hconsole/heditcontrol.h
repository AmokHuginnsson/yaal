/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	HEditControl.h - this file is integral part of `stdhapi' project.

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

#ifndef __HEDITCONTROL_H
#define __HEDITCONTROL_H

#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */

#include "../hcore/hstringlist.h"
#include "hcontrol.h"

class HEditControl : public virtual HControl
	{
protected:
	/*{*/
	bool				f_bReplace;					/* current edit mode */
	bool				f_bMultiLine;				/* is control multiline */
	bool				f_bPassword;				/* should be entered text visible */
	int					f_iMaxStringSize;		/* self explenary */
	int					f_iCursorPosition;	/* cursor position relative to control
																		 begining */
	int					f_iControlOffset;		/* when content is bigger than control size
																		 this variable keeps offset of first
																		 character shown */
	int					f_iMaxHistoryLevel;	/* how many instertions should history keep */
	regex_t			f_sMask;						/* regular expression describing what
																		 characters and
																		 in what way can be entered */
	HString			f_oString;					/* control content */
	HStringList f_oHistory; 				/* history of insertions */
	/*}*/
public:
	/*{*/
	HEditControl ( HWindow *,				/* parent */
								int,							/* top ( parent relative ) */
								int,							/* left */
								int,							/* width */ 
								int = 128,				/* maximum string length */
								const char * = "",	/* label */
								const char * = "", /* initial content */
								const char * = "^[a-zA-Z±°Ê∆Í ≥£Ò—Û”∂¶º¨øØ]*$", /* mask */
								bool = false,			/* replace */
								bool = false,			/* multiline */
								bool = false,			/* password */
								int = 8 );				/* max history level */
	virtual ~HEditControl ( void );
	virtual void refresh ( void );
	virtual int set_focus ( char = 0 );
	virtual HInfo operator = ( const HInfo & );
	void set_flags ( bool = false, bool = false );
	int process_input( int );
	HString & operator = ( const HString & );
	operator HString ( );
	/*}*/
	};

#endif /* __TREMINAL_H */

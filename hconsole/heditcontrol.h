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

#ifndef __HCONSOLE_HEDITCONTROL_H
#define __HCONSOLE_HEDITCONTROL_H

#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */

#include "hcore/hstringlist.h"
#include "hcontrol.h"

namespace stdhapi
{

namespace hconsole
{

#define D_MASK_LETTERS	"^[a-zA-Z±°Ê∆Í ≥£Ò—Û”∂¶º¨øØ]*$"
#define D_MASK_CIPHERS	"^[0-9]*$"
#define D_MASK_APLPHA		"^[a-zA-Z0-9]*$"
#define D_MASK_EXTENDED	"^[0-9a-zA-Z±°Ê∆Í ≥£Ò—Û”∂¶º¨øØ\\.\\(\\) -]*$"
#define D_MASK_LOOSE		".*"
#define D_MASK_DEFAULT	D_MASK_LETTERS

class HEditControl : public virtual HControl
	{
protected:
	/*{*/
	bool				f_bReplace;						/* current edit mode */
	bool				f_bMultiLine;					/* is control multiline */
	bool				f_bPassword;					/* should be entered text visible */
	bool				f_bRightAligned;			/* is control content right aligned,
																			 control content can be right aligned
																			 only in single line mode */
	int					f_iMaxStringSize;			/* self explenary */
	int					f_iCursorPosition;		/* cursor position relative to control
																			 begining */
	int					f_iControlOffset;			/* when content is bigger than control size
																			 this variable keeps offset of first
																			 character shown */
	int					f_iMaxHistoryLevel;		/* how many instertions should history keep */
	regex_t			f_sMask;							/* regular expression describing what
																			 characters and
																			 in what way can be entered */
	hcore::HString			f_oString;						/* control content */
	hcore::HStringList f_oHistory; 					/* history of insertions */
	/*}*/
public:
	/*{*/
	HEditControl ( HWindow *,					/* parent */
								int,								/* top ( parent relative ) */
								int,								/* left */
								int,								/* height */
								int,								/* width */
								char const * = "",	/* label */
								size_t = 127,					/* maximum string length */
								char const * = "",	/* initial content */
								char const * = D_MASK_DEFAULT, /* mask */
								bool = false,				/* replace */
								bool = false,				/* right alignment */
								bool = false,				/* multiline */
								bool = false,				/* password */
								int = 8,						/* max history level */
								bool = true,				/* draw label */
								int = -1, 					/* disabled attribute */
								int = -1,						/* enabled attribute */
								int = -1 );					/* focused attribute */
	virtual ~HEditControl ( void );
	virtual void refresh ( void );
	virtual int set_focus ( char = 0 );
	virtual HControl & operator = ( const hcore::HInfo & );
	virtual operator hcore::HInfo ( void );
	void set_flags ( bool = false, bool = false );
	virtual int process_input( int );
	HControl & operator = ( char const * );
	operator hcore::HString ( );
	virtual int click ( mouse::OMouse & );
	/*}*/
	};

}

}

#endif /* not __HCONSOLE_HEDITCONTROL_H */

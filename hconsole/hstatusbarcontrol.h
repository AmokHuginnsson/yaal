/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hstatusbarcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HSTATUSBARCONTROL_H
#define __HSTATUSBARCONTROL_H

#include "heditcontrol.h"

class HStatusBarControl : public HEditControl
	{
protected:
	/*{*/
	int			f_iStatusBarAttribute;
	int			f_iPromptLength;
	HString	f_oPrompt;
	/* progress bar data */
	bool		f_bDone;
	bool		f_bEstimate;			/* was time left count ? */
	double	f_dProgressSize;	/* number of steps in progess bar */
	int			f_iLastProgress;	/* last drawed step */
	int			f_iLastPercent;		/* last writen percent */
	int			f_iLastMinute;		/* all last* variables help */
	int			f_iLastSecond;		/* keep progress bar refresh rate low */
	int			f_iLastStep;
	HString	f_oMessage;
	HTime		f_oStart;
	/* end of progress bar data */
	/*}*/
public:
	/*{*/
	HStatusBarControl ( HWindow *, const char *, int = -1 );
	virtual ~HStatusBarControl ( void );
	virtual void draw_label ( void );
	virtual void refresh ( void );
	virtual int verify ( void );
	virtual int process_input( int );
	void setup ( const char *, const char *, int );
	void set_prompt ( const char * = NULL );
	void init_progress ( double, const char *, bool = true );
	void update_progress ( double = -1, const char * = NULL );
	void message ( int, const char *, ... );
	void message ( const char *, ... );
	void bar ( const char * = NULL );
	int ask ( const char *, const char * );
	bool confirm ( const char * );
	/*}*/
	};

#endif /* not __HSTATUSBARCONTROL_H */


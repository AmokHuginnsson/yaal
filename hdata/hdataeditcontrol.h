/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HDATAEDITCONTROL_H
#define __HDATAEDITCONTROL_H

#include "../hconsole/heditcontrol.h"
#include "hdatawindow.h"

class HDataEditControl : public HEditControl
	{
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	HDataEditControl ( HDataWindow *,					/* parent */
								int,								/* top ( parent relative ) */
								int,								/* left */
								int,								/* height */
								int,								/* width */
								const char * = "",	/* label */
								int = 128,					/* maximum string length */
								const char * = "",	/* initial content */
								const char * = D_DEFAULT_MASK, /* mask */
								bool = false,				/* replace */
								bool = false,				/* right alignment */
								bool = false,				/* multiline */
								bool = false,				/* password */
								int = 8,						/* max history level */
								bool = true,				/* draw label */
								int = -1, 					/* disabled attribute */
								int = -1,						/* enabled attribute */
								int = -1 );					/* focused attribute */
	virtual int process_input( int );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

#endif /* __HDATAEDITCONTROL_H */


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatacontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HDATACONTROL_H
#define __HDATACONTROL_H

#include "../dbwrapper/hrecordset.h"
#include "../hconsole/hcontrol.h"

class HDataControl : public virtual HControl
	{
protected:
	/*{*/
	HRecordSet * f_poRecordSet;
	/*}*/
public:
	/*{*/
	HDataControl ( HRecordSet *,	/* record-set (table) that will be displayed */
								 HWindow *,			/* parent window */
								 int, int, int, int, /* coordinates */
								 const char *,	/* label */
								 int = -1, int = -1, int = -1 );	/* attributes */
	virtual ~HDataControl ( void );
	virtual void populate ( long int = 0 );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

#endif /* not __HDATACONTROL_H */

/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCONSOLE_HSEARCHABLECONTROL_H
#define __HCONSOLE_HSEARCHABLECONTROL_H

#include "../hcore/hpattern.h"
#include "hcontrol.h"

class HSearchableControl : public virtual HControl
	{
protected:
	/*{*/
	bool			f_bSearchable;		/* are items searchable */
	bool			f_bSearchActived;	/* should we highlight last search */
	bool			f_bFiltered;			/* is content of list filtered
																 thru pattern */
	bool			f_bBackwards;			/* last search direction */
	HPattern	f_oPattern;				/* used for searching */
	/*}*/
public:
	/*{*/
	HSearchableControl ( bool );
	virtual ~HSearchableControl ( void );
	virtual void search ( const HString &, bool );
	/*}*/
protected:
	/*{*/
	void highlight ( int, int, int, bool );
	virtual void go_to_match ( void ) = 0;
	virtual void go_to_match_previous ( void ) = 0;
	/*}*/
	};

#endif /* not __HCONSOLE_HSEARCHABLECONTROL_H */


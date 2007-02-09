/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxcontrol.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HCOMBOBOXCONTROL_H
#define __YAAL_HCONSOLE_HCOMBOBOXCONTROL_H

#include "hconsole/heditcontrol.h"
#include "hconsole/hlistcontrol.h"

namespace yaal
{

namespace hconsole
{

class HComboboxControl : public virtual HEditControl,
	public virtual HListControl
	{
public:
	struct MODE
		{
		typedef enum
			{
			D_EDITCONTROL,
			D_LISTCONTROL
			} mode_t;
		};
protected:
	MODE::mode_t f_eMode; /* operating mode (D_MODE_EDITCONTROL|D_MODE_LISTCONTROL) */
	int f_iDroppedWidth;	/* width of dropped list */
public:
	HComboboxControl ( HWindow *,			/* parent */
										 int,						/* row */
										 int,						/* col */
										 int,						/* height */
										 int,						/* width */
										 char const *,	/* label */
										 int = 0, 			/* dropped width */
										 int = 127,			/* max string length */
										 char const * = n_pcMaskDefault,	/* regexp (mask) */
										 bool = true		/* searchable */ );		/* focused attribute */
	virtual ~HComboboxControl ( void );
	virtual int process_input ( int );
	virtual int set_focus ( char = 0 );
	virtual int kill_focus ( void ); /* this one will be used here */
protected:
	virtual void do_refresh ( void );
	virtual int do_click ( mouse::OMouse & );
private:
	void close_combo ( void );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HCOMBOBOXCONTROL_H */


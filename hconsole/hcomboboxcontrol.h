/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCOMBOBOXCONTROL_H
#define __HCOMBOBOXCONTROL_H

#include "heditcontrol.h"
#include "hlistcontrol.h"

#define D_MODE_EDITCONTROL	0
#define D_MODE_LISTCONTROL	1

class HComboboxControl : virtual public HEditControl,
	virtual public HListControl
	{
protected:
	/*{*/
	int f_iMode; 					/* operating mode (D_MODE_EDITCONTROL|D_MODE_LISTCONTROL) */
	int f_iDroppedWidth;	/* width of dropped list */
	/*}*/
public:
	/*{*/
	HComboboxControl ( HWindow *,			/* parent */
										 int,						/* row */
										 int,						/* col */
										 int,						/* height */
										 int,						/* width */
										 const char *,	/* label */
										 int = 0, 			/* dropped width */
										 int = 128,			/* max string length */
										 const char * = D_MASK_DEFAULT,	/* regexp (mask) */
										 bool = false,	/* checkable */
										 bool = true,		/* sortable */
										 bool = true,		/* searchable */
										 bool = true,		/* draw label */
										 int = -1, 			/* disabled attribute */
										 int = -1,			/* enabled attribute */
										 int = -1 );		/* focused attribute */
	virtual ~HComboboxControl ( void );
	virtual int process_input ( int );
	virtual int set_focus ( char = 0 );
	virtual int kill_focus ( void ); /* this one will be used here */
	virtual void refresh ( void );
	virtual int click ( mouse::OMouse & );
/*	
	virtual HInfo operator = ( const HInfo & );
	virtual operator HInfo ( void );
*/
	/*}*/
protected:
	/*{*/
	/*}*/
	};

#endif /* not __HCOMBOBOXCONTROL_H */

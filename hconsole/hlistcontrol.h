/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HLISTCONTROL_H
#define __HLISTCONTROL_H

#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */

#include "../hcore/hinfolist.h"
#include "hitem.h"
#include "hwindow.h"

#define D_ALIGN_LEFT			0x0010000
#define D_ALIGN_CENTER		0x0020000
#define D_ALIGN_RIGHT			0x0040000
#define D_ALIGN_MASK			0x0ff0000

class HListControl : public HList < HItem >, public virtual HControl
	{
protected:
	/*{*/
	bool				f_bCheckable;					/* can items be checked/unchecked */
	bool				f_bEditable;					/* is list interactively editable */
	bool				f_bFiltered;					/* is content of list filtered
																			 thru pattern */
	int					f_iControlOffset;			/* when content is bigger than control
																			 size this variable keeps
																			 offset of first row shown */
	int					f_iCursorPosition;		/* cursor position relative to control
																			 begining */
	int					f_iSumForOne;					/* sum of percentage columns width */
	regex_t 		f_sMask;							/* regular expression for search patterns */
	HString			f_oPattern;						/* current search pattern */
	HElement *	f_poFirstVisibleRow;	/* pointer to first visible row */
	HInfoList 	f_oHeader;						/* list header info */
/* for internal use only */
	int					f_iSortColumn;				/* column used for current sort operation */
	/*}*/
public:
	/*{*/
	HListControl ( HWindow *,		 		/* parent */
								 int,							/* row */
								 int,							/* col */
								 int,							/* height */
								 int,							/* width */
								 const char *,		/* label */
								 int = -1, 				/* disabled attribute */
								 int = -1,				/* enabled attribute */
								 int = -1,				/* focused attribute */
								 bool = false );	/* checkable */
	virtual ~HListControl ( void );
	virtual void refresh ( void );
	virtual int set_focus ( char = 0 );
	int process_input( int );
	void add_column ( const int &,									/* at position */
										const char *,									/* column name */
										const int &,									/* width */
										const int & = D_ALIGN_LEFT,		/* align */
										const int & = D_TYPE_HSTRING,	/* type */
										HControl * = NULL );					/* control associated */
	virtual HItem & add_tail ( void );
	virtual HItem & add_tail ( HItem & );
	virtual HItem remove_element ( int * = D_BLOCK_IF_NOT_EMPTIED );
	virtual HItem remove_tail ( int * = D_BLOCK_IF_NOT_EMPTIED );
	/*}*/
protected:
	/*{*/
	int cmpc ( HElement *, HElement * );
	void sort_by_contents ( int, int = D_ASCENDING );
	void recalculate_column_widths ( void );
	/*}*/
	};

#endif /* not __HLISTCONTROL_H */

/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	oresource.h - this file is integral part of `stdhapi' project.

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

#ifndef __ORESOURCE_H
#define __ORESOURCE_H

/* control flags */

/* main control of a window, contents of this control holds
	 data of main record-set of a window */
#define D_CONTROL_MAIN		1

/* this kind of control is meant to setup filter for main
	 control for `view mode' */
#define D_CONTROL_FILTER	2

/* this kind of control is used to store record-set data in `edit mode' */
#define D_CONTROL_DATA		4

/* control types */
#define D_CONTROL_EDIT	1
#define D_CONTROL_LIST	2
#define D_CONTROL_TREE	3
#define D_CONTROL_COMBO	4
#define D_CONTROL_DATE	5
#define D_CONTROL_CHECK	6

struct OResource
	{
/* data part */
	char *	f_pcTable;						/* name of table in database asociated with contorl */
	char *	f_pcSQL;							/* general SQL quarey for contorl (read-only mode) */
	char *	f_pcFilter;						/* additional filter for record-set (WHERE clause) */
	char *	f_pcSort;							/* additional sort for record-set (ORDER BY clause) */
/* control part */
	int			f_iRow;								/* top coordinate of contorl */
	int			f_iColumn;						/* left coordinate of contorl */
	int			f_iHeight;						/* height of control */
	int			f_iWidth;							/* width of control */
	char *	f_pcLabel;						/* control label */
	int			f_iAttribute;					/* attribute of control in disabled state */
	int			f_iEnabledAttribute;	/* attribute of control in enabled state */
	int			f_iFocusedAttribute;	/* attribute of control in focused state */
	int			f_iFlags;							/* tell how control should be treaded */
	int			f_iType;							/* predefined type of control */
	int			f_iControlParent;			/* index of parent control (tree data structure) */
	void *	f_pvControlSpecific;	/* additional initialization values control
																	 specific */
	};

/* control type specific */
struct OEditControlResource
	{
	char *	f_pcValue;						/* initial value for control */
	char *	f_pcMask;							/* tell what can be inserted into control */
	bool		f_bMultiLine;					/* is this contorl a multiline edit? */
	bool		f_bReplace;						/* is control in replace mode? */
	bool		f_bPassword;					/* is control in password mode? (no echo) */
	int			f_iMaxHistoryLevel;		/* history buffer size */
	};

#endif /* not __ORESOURCE_H */

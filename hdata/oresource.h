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

#ifndef __HDATA_ORESOURCE_H
#define __HDATA_ORESOURCE_H

namespace stdhapi
{

namespace hdata
{

#define D_CVSID_ORESOURCE_H "$CVSHeader$"

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

struct OAttributes
	{
	bool	f_bDrawLabel;				/* should be label driven */
	int		f_iDisabledAttribute;	/* attribute of control in disabled state */
	int		f_iEnabledAttribute;	/* attribute of control in enabled state */
	int		f_iFocusedAttribute;	/* attribute of control in focused state */
	};

struct OColumnInfo
	{
	int f_iPlacement; 			/* what place should this new column take */
	char const * f_pcName;	/* column name */
	int f_iWidth;						/* column width */
	int f_iAlign;						/* column alignment */
	hcore::type_t f_eType;						/* data type stored in that column */
	};

class HDataControl;

struct OResource
	{
/* data part */
	char const *	f_pcTable;			/* name of table in database asociated with contorl */
	char const *	f_pcColumns;		/* what columns from record-set we are looking for */
	char const *	f_pcFilter;			/* additional filter for record-set (WHERE clause) */
	char const *	f_pcSort;				/* additional sort for record-set (ORDER BY clause) */
/* control part */
	int			f_iRow;								/* top coordinate of contorl */
	int			f_iColumn;						/* left coordinate of contorl */
	int			f_iHeight;						/* height of control */
	int			f_iWidth;							/* width of control */
	char const *	f_pcLabel;			/* control label */
	OAttributes *	f_psAttributes;	/* control's attributes */
/* relative functionality configuration options */
	int			f_iFlags;							/* tell how control should be treaded */
	int			f_iType;							/* predefined type of control */
	void *	f_pvTypeSpecific;			/* additional initialization values control
																	 type specific */
	OColumnInfo * f_psColumnInfo;	/* if this control is a child control this field
																	 holds initialization information about
																	 parent's record's field-column */
	int			f_iParent;						/* index of parent control (tree data structure) */
	};

/* control type specific */
struct OEditControlResource
	{
	int			f_iMaxStringSize;		/* maximum length of contained string */
	char const *	f_pcValue;		/* initial value for control */
	char const *	f_pcMask;			/* tell what can be inserted into control */
	bool		f_bReplace;					/* is control in replace mode? */
	bool		f_bMultiLine;				/* is this contorl a multiline edit? */
	bool		f_bReadOnly;				/* should editing be disabled ? */
	bool		f_bRightAligned;		/* is control content right aligned */
	bool		f_bPassword;				/* is control in password mode? (no echo) */
	int			f_iMaxHistoryLevel;	/* history buffer size */
	};

struct OListControlResource
	{
	bool	f_bCheckable;					/* can control items be checked/unchecked */
	bool	f_bSortable;					/* is control content sortable */
	bool	f_bSearchable;				/* can be item searched */
	bool	f_bDrawHeader;				/* should be header droven */
	};

}

}

#endif /* not __HDATA_ORESOURCE_H */

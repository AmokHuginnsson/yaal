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

#include "../hcore/hinfolist.h"
#include "hitem.h"
#include "hwindow.h"
#include "hsearchablecontrol.h"

#define D_ALIGN_LEFT			0x0010000
#define D_ALIGN_CENTER		0x0020000
#define D_ALIGN_RIGHT			0x0040000

class HListControl : public HList < HItem >, public virtual HSearchableControl
	{
	class HColumnInfo
		{
	protected:
		/*{*/
		int f_iWidthRaw;
		int f_iWidth;
		int f_iType;
		int f_iAlign;
		int f_iShortcutIndex;
		char f_cShortcut;
		HString f_oName;
		HControl * f_poControl;
		/*}*/
	public:
		/*{*/
		HColumnInfo ( void );
		virtual ~HColumnInfo ( void );
		HColumnInfo ( const HColumnInfo & );
		HColumnInfo & operator = ( const HColumnInfo & );
		int operator - ( const HColumnInfo & );
		/*}*/
	protected:
		/*{*/
		/*}*/
		friend class HListControl;
		};
protected:
	/*{*/
	bool				f_bCheckable;					/* can items be checked/unchecked */
	bool        f_bSortable;					/* can control content be sorted */
	bool				f_bDrawHeader;				/* should be header driven */
	bool				f_bEditable;					/* is list interactively editable */
	int					f_iControlOffset;			/* when content is bigger than control
																			 size this variable keeps
																			 offset of first row shown */
	int					f_iCursorPosition;		/* cursor position relative to control
																			 begining */
	int					f_iSumForOne;					/* sum of percentage columns width */
	HList < HColumnInfo >	f_oHeader;	/* list header info */
	HElement *	f_poFirstVisibleRow;	/* pointer to first visible row */
	struct
		{
		int f_iColumnWithMatch;
		int f_iMatchNumber;
		HElement *	f_poCurrentMatch;		/* row that has current pattern match */
		} f_sMatch;
/* for internal use only */
	int					f_iSortColumn;				/* column used for current sort operation */
	long int		f_lComparedItems;			/* items already compared during sorting */
	/*}*/
public:
	/*{*/
	HListControl ( HWindow *,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 const char *,	/* label */
								 bool = false,	/* checkable */
								 bool = true,		/* sortable */
								 bool = true, 	/* searchable */
								 bool = true,		/* draw header */
								 bool = true,		/* draw label */
								 int = -1, 			/* disabled attribute */
								 int = -1,			/* enabled attribute */
								 int = -1 );		/* focused attribute */
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
	virtual HItem & add_orderly ( HItem & );
	virtual int remove_element ( int = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	virtual int remove_tail ( int = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	virtual bool is_searchable ( void );
	virtual int click ( mouse::OMouse & );
	/*}*/
protected:
	/*{*/
	int cmpc ( HElement *, HElement * );
	void sort_by_contents ( int, int = D_ASCENDING );
	void recalculate_column_widths ( void );
	virtual void go_to_match ( void );
	virtual void go_to_match_previous ( void );
	/*}*/
	};

#endif /* not __HLISTCONTROL_H */

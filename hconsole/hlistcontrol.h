/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.h - this file is integral part of `yaal' project.

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

#ifndef __HCONSOLE_HLISTCONTROL_H
#define __HCONSOLE_HLISTCONTROL_H

#include "hitem.h"
#include "hwindow.h"
#include "hsearchablecontrol.h"

namespace yaal
{

namespace hconsole
{

class HListControl : public hcore::HList < HItem >, public virtual HSearchableControl
	{
	class HColumnInfo
		{
	protected:
		/*{*/
		bool f_bDescending;
		int f_iWidthRaw;
		int f_iWidth;
		BITS::ALIGN::align_t f_eAlign;
		int f_iShortcutIndex;
		char f_cShortcut;
		type_t f_eType;
		hcore::HString f_oName;
		HControl * f_poControl;
		/*}*/
	public:
		/*{*/
		HColumnInfo ( void );
		virtual ~HColumnInfo ( void );
		HColumnInfo ( HColumnInfo const & );
		HColumnInfo & operator = ( HColumnInfo const & );
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
	hcore::HList < HColumnInfo >	f_oHeader;	/* list header info */
	HElement *	f_poFirstVisibleRow;	/* pointer to first visible row */
	struct
		{
		int f_iColumnWithMatch;
		int f_iMatchNumber;
		HElement *	f_poCurrentMatch;		/* row that has current pattern match */
		} f_sMatch;
/* for internal use only */
	int					f_iSortColumn;				/* column used for current sort operation */
	int long		f_lComparedItems;			/* items already compared during sorting */
	/*}*/
public:
	/*{*/
	HListControl ( HWindow *,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const *,	/* label */
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
	virtual int process_input( int );
	void add_column ( int const &,									/* at position */
										char const *,									/* column name */
										int const &,									/* width */
										BITS::ALIGN::align_t const & = BITS::ALIGN::D_LEFT,		/* align */
										const type_t & = D_HSTRING,	/* type */
										HControl * = NULL );					/* control associated */
	virtual HItem & add_tail ( HItem * = NULL );
	virtual HItem & add_orderly ( HItem &, sort_order_t = D_ASCENDING );
	virtual yaal::hcore::OListBits::status_t remove_element ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	virtual yaal::hcore::OListBits::status_t remove_tail ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	virtual bool is_searchable ( void );
	virtual int click ( mouse::OMouse & );
	/*}*/
protected:
	/*{*/
	virtual bool is_above_c ( HElement *, HElement * );
	void sort_by_column ( int, sort_order_t = D_ASCENDING );
	void recalculate_column_widths ( void );
	virtual void go_to_match ( void );
	virtual void go_to_match_previous ( void );
	/*}*/
private:
	/*{*/
	HListControl ( HListControl const & );
	HListControl & operator = ( HListControl const & );
	/*}*/
	};

}

}

#endif /* not __HCONSOLE_HLISTCONTROL_H */

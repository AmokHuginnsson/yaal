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

#ifndef __YAAL_HCONSOLE_HLISTCONTROL_H
#define __YAAL_HCONSOLE_HLISTCONTROL_H

#include "hcore/hpointer.h"
#include "hconsole/hitem.h"
#include "hconsole/hwindow.h"
#include "hconsole/hsearchablecontrol.h"

namespace yaal
{

namespace hconsole
{

class HListControl : virtual public HSearchableControl
	{
public:
	typedef hcore::HList < HItem > item_list_t;
	typedef yaal::hcore::HPointer<item_list_t> item_list_ptr_t;
protected:
	class HColumnInfo
		{
		bool f_bDescending;
		int f_iWidthRaw;
		int f_iWidth;
		BITS::ALIGN::align_t f_eAlign;
		int f_iShortcutIndex;
		char f_cShortcut;
		type_t f_eType;
		hcore::HString f_oName;
		HControl * f_poControl;
	public:
		HColumnInfo ( void );
		virtual ~HColumnInfo ( void );
		HColumnInfo ( HColumnInfo const & );
		HColumnInfo & operator = ( HColumnInfo const & );
		friend class HListControl;
		};
	struct FLAGS
		{
		typedef enum
			{
			D_NONE = 0,
			D_CHECKABLE = 1,
			D_SORTABLE = 2,
			D_EDITABLE = 4,
			D_DRAW_HEADER = 8,
			D_ALL = -1
			} list_flags_t;
		};
	item_list_ptr_t f_oList;
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
	hcore::HList<HColumnInfo>	f_oHeader;	/* list header info */
/* for internal use only */
	int					f_iSortColumn;				/* column used for current sort operation */
	item_list_t::HIterator	f_oFirstVisibleRow;	/* pointer to first visible row */
	struct match_t
		{
		int f_iColumnWithMatch;
		int f_iMatchNumber;
		item_list_t::HIterator	f_oCurrentMatch;		/* row that has current pattern match */
		match_t ( ) : f_iColumnWithMatch ( 0 ), f_iMatchNumber ( - 1 ), f_oCurrentMatch ( ) { }
		} f_sMatch;
public:
	HListControl ( HWindow *,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const *, item_list_ptr_t = item_list_ptr_t() );	/* label */
	virtual ~HListControl ( void );
	void add_column ( int const &,									/* at position */
										char const *,									/* column name */
										int const &,									/* width */
										BITS::ALIGN::align_t const & = BITS::ALIGN::D_LEFT,		/* align */
										const type_t & = D_HSTRING,	/* type */
										HControl * = NULL );					/* control associated */
//	virtual yaal::hcore::OListBits::status_t remove_element ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	void add_tail ( HItem const & );
//	virtual HItem & add_orderly ( HItem &, sort_order_t = D_ASCENDING );
	virtual int set_focus ( char = 0 );
	void set_flags ( FLAGS::list_flags_t, FLAGS::list_flags_t );
	item_list_t const & get_data ( void ) const;
protected:
	virtual void refresh ( void );
	virtual int process_input( int );
	//virtual yaal::hcore::OListBits::status_t remove_tail ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem * * = NULL );
	virtual bool is_searchable ( void );
	virtual int click ( mouse::OMouse & );
//	virtual bool is_above_c ( HElement *, HElement * );
	virtual void go_to_match ( void );
	virtual void go_to_match_previous ( void );
	void handle_key_page_up ( void );
	void handle_key_page_down ( void );
	void handle_key_up ( void );
	void handle_key_home ( void );
	void handle_key_end ( void );
	void handle_key_down ( void );
	void handle_key_ctrl_n ( void );
	void handle_key_ctrl_p ( void );
	void handle_key_space ( void );
	void handle_key_tab ( void );
private:
	void sort_by_column ( int, hcore::OListBits::sort_order_t = hcore::OListBits::D_ASCENDING );
	void recalculate_column_widths ( void );
	HListControl ( HListControl const & );
	HListControl & operator = ( HListControl const & );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HLISTCONTROL_H */

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
#include "hcore/hlist.h"
#include "hconsole/hitem.h"
#include "hconsole/hwindow.h"
#include "hconsole/hsearchablecontrol.h"

namespace yaal
{

namespace hconsole
{


namespace list_control_helper
	{
	yaal::hcore::HString const GetLongFromCell( yaal::hcore::HInfo const & );
	yaal::hcore::HString const GetDoubleFromCell( yaal::hcore::HInfo const & );
	yaal::hcore::HString const GetStringFromCell( yaal::hcore::HInfo const & );
	char const * GetTimeFromCell( yaal::hcore::HInfo const & );
	int long GetIdFromCell( HItem const & );
	bool GetStateFromCell( HItem const & );
	struct OSortHelper
		{
		int f_iSortColumn;
		yaal::hcore::OListBits::sort_order_t f_eOrder;
		type_t f_eType;
		int long f_lComparedItems;
		int long f_iSize;
		HWindow * f_poWindow;
		void progress ( void );
		};
	bool compare_cells( yaal::hcore::HInfo const &, yaal::hcore::HInfo const &, OSortHelper & );
	}

class HBaseListControl : virtual public HSearchableControl
	{
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
		friend class HBaseListControl;
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
	yaal::hcore::HList<HColumnInfo>	f_oHeader;	/* list header info */
/* for internal use only */
	int					f_iSortColumn;				/* column used for current sort operation */
	struct match_t
		{
		int f_iColumnWithMatch;
		int f_iMatchNumber;
		match_t ( ) : f_iColumnWithMatch ( 0 ), f_iMatchNumber ( - 1 ) { }
		} f_sMatch;
public:
	HBaseListControl ( HWindow *,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const * );	/* label */
	virtual ~HBaseListControl ( void );
	void add_column ( int const &,									/* at position */
										char const *,									/* column name */
										int const &,									/* width */
										BITS::ALIGN::align_t const & = BITS::ALIGN::D_LEFT,		/* align */
										const type_t & = D_HSTRING,	/* type */
										HControl * = NULL );					/* control associated */
//	virtual yaal::hcore::OListBits::status_t remove_element ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem_t * * = NULL );
//	virtual HItem_t & add_orderly ( HItem_t &, sort_order_t = D_ASCENDING );
	virtual int set_focus ( char = 0 );
	void set_flags ( FLAGS::list_flags_t, FLAGS::list_flags_t );
protected:
	virtual int long do_size( void ) = 0;
	virtual void do_first_item( void ) = 0;
	virtual void do_next_item( void ) = 0;
	virtual void do_switch_state( void ) = 0;
	virtual bool do_is_current_match( void ) = 0;
	virtual void do_sort( list_control_helper::OSortHelper & ) = 0;
	virtual bool get_text_for_cell( int, type_t ) = 0;
	virtual void set_child_control_data_for_cell( int, HControl * ) = 0;
	virtual void refresh ( void );
	void draw_cell ( int, int, int, HColumnInfo const * const, bool );
	virtual int process_input( int );
	//virtual yaal::hcore::OListBits::status_t remove_tail ( treatment_t const & = D_BLOCK_IF_NOT_EMPTIED, HItem_t * * = NULL );
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
	HBaseListControl ( HBaseListControl const & );
	HBaseListControl & operator = ( HBaseListControl const & );
	};

template <typename tType = yaal::hcore::HInfo>
class HListControl_t : public HBaseListControl
	{
public:
	typedef HItem_t<tType> row_t;
	typedef yaal::hcore::HList<row_t> item_list_t;
	typedef typename item_list_t::HIterator iterator_t;
	typedef yaal::hcore::HPointer<item_list_t, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> item_list_ptr_t;
	HListControl_t ( HWindow *,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const *, item_list_ptr_t = item_list_ptr_t() );	/* label */
	item_list_t const & get_data ( void ) const;
	void add_tail ( row_t & );
protected:
	item_list_ptr_t f_oList;
	iterator_t	f_oFirstVisibleRow;	/* pointer to first visible row */
	iterator_t	f_oCurrentMatch;		/* row that has current pattern match */
	iterator_t	f_oIterator;		/* row that has current pattern match */
	virtual int long do_size( void );
	virtual void do_first_item( void );
	virtual void do_next_item( void );
	virtual void do_switch_state( void );
	virtual bool do_is_current_match( void );
	virtual void do_sort( list_control_helper::OSortHelper & );
	virtual bool get_text_for_cell( int, type_t );
	virtual void set_child_control_data_for_cell( int, HControl * );
	};

template <typename tType>
HListControl_t<tType>::HListControl_t ( HWindow * a_poParent, int a_iRow, int a_iColumn,
		int a_iHeight, int a_iWidth, char const * a_pcLabel, item_list_ptr_t a_oData )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
								a_pcLabel ),
							HSearchableControl ( true ),
							HBaseListControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth, a_pcLabel ),
	f_oList ( ( !!a_oData ) ? a_oData : item_list_ptr_t ( new item_list_t() ) ),
	f_oFirstVisibleRow ( ), f_oCurrentMatch ( ), f_oIterator()
	{
	return;
	}

template <typename tType>
int long HListControl_t<tType>::do_size( void )
	{
	return ( (*f_oList).size() );
	}

template <typename tType>
void HListControl_t<tType>::do_first_item( void )
	{
	f_oIterator = f_oFirstVisibleRow;
	return;
	}

template <typename tType>
void HListControl_t<tType>::do_next_item( void )
	{
	++ f_oIterator;
	return;
	}

template <typename tType>
void HListControl_t<tType>::do_switch_state( void )
	{
	if ( f_bCheckable && ! (*f_oList).empty ( ) )
		f_oIterator->m_bChecked = ! f_oIterator->m_bChecked;
	}

template <typename tType>
bool HListControl_t<tType>::do_is_current_match( void )
	{
	return ( f_oIterator == f_oCurrentMatch );
	}

template <typename tType>
bool HListControl_t<tType>::get_text_for_cell( int a_iColumn, type_t a_eType )
	{
	row_t & l_oItem = *f_oIterator;
	switch ( a_eType )
		{
		case ( D_LONG_INT ):
			f_oVarTmpBuffer = list_control_helper::GetLongFromCell ( l_oItem [ a_iColumn ] );
		break;
		case ( D_DOUBLE ):
			f_oVarTmpBuffer = list_control_helper::GetDoubleFromCell ( l_oItem [ a_iColumn ] );
		break;
		case ( D_HSTRING ):
			f_oVarTmpBuffer = list_control_helper::GetStringFromCell ( l_oItem [ a_iColumn ] );
		break;
		case ( D_HTIME ):
			f_oVarTmpBuffer = list_control_helper::GetTimeFromCell ( l_oItem [ a_iColumn ] );
		break;
		default :
			M_THROW ( "unknown type", a_eType );
		}
	return ( l_oItem.m_bChecked );
	}

template <typename tType>
void HListControl_t<tType>::set_child_control_data_for_cell( int a_iColumn, HControl * a_poControl )
	{
	row_t l_oItem = *f_oIterator;
	a_poControl->set( l_oItem [ a_iColumn ] );
	return;
	}

template <typename tType>
void HListControl_t<tType>::add_tail( row_t & a_tRow )
	{
	M_PROLOG
	(*f_oList).push_back ( a_tRow );
	int l_iSize = (*f_oList).size ( );
	if ( l_iSize > f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = l_iSize - f_iHeightRaw;
		if ( f_oFirstVisibleRow != (*f_oList).end ( ) )
			++ f_oFirstVisibleRow;
		}
	else
		f_iCursorPosition = l_iSize - 1;
	if ( f_oFirstVisibleRow == (*f_oList).end ( ) )
		f_oFirstVisibleRow = (*f_oList).begin ( );
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

template <typename tType>
typename HListControl_t<tType>::item_list_t const & HListControl_t<tType>::get_data ( void ) const
	{
	return ( (*f_oList) );
	}

template <typename tType>
class CompareListControlItems
	{
	list_control_helper::OSortHelper & f_roSortHelper;
public:
	CompareListControlItems ( list_control_helper::OSortHelper & a_roSortHelper )
		: f_roSortHelper ( a_roSortHelper ) { }
	bool operator() ( typename HListControl_t<tType>::item_list_t::HIterator const &, typename HListControl_t<tType>::item_list_t::HIterator const & ) const;
	};

template <typename tType>
bool CompareListControlItems<tType>::operator() ( typename HListControl_t<tType>::item_list_t::HIterator const & a_oLeft,
		typename HListControl_t<tType>::item_list_t::HIterator const & a_oRight ) const
	{
	M_PROLOG
	typename HListControl_t<tType>::item_list_t::HIterator const & l_oLeft = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oLeft : a_oRight;
	typename HListControl_t<tType>::item_list_t::HIterator const & l_oRight = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oRight : a_oLeft;
	yaal::hcore::HInfo const & l_roLeftInfo = ( *l_oLeft ) [ f_roSortHelper.f_iSortColumn ];
	yaal::hcore::HInfo const & l_roRightInfo = ( *l_oRight ) [ f_roSortHelper.f_iSortColumn ];
	return ( list_control_helper::compare_cells( l_roLeftInfo, l_roRightInfo, f_roSortHelper ) );
	M_EPILOG
	}

template <typename tType>
void HListControl_t<tType>::do_sort( list_control_helper::OSortHelper & a_roHelper )
	{
	(*f_oList).sort ( CompareListControlItems<tType> ( a_roHelper ) );
	f_oFirstVisibleRow = (*f_oList).begin();
	}

typedef HListControl_t<> HListControl;

}

}

#endif /* not __YAAL_HCONSOLE_HLISTCONTROL_H */

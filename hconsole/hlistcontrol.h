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

#include "hcore/hlog.h"

namespace yaal
{

namespace hconsole
{

namespace list_control_helper
{

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
template<typename tType>
bool compare_cells( tType const&, tType const&, OSortHelper& );

class HAbstractCell
	{
public:
	virtual ~HAbstractCell( void );
	virtual yaal::hcore::HString const get_long( void );
	virtual yaal::hcore::HString const get_double( void );
	virtual yaal::hcore::HString const get_string( void );
	virtual char const* get_time( void );
	virtual int long get_id( void );
	virtual void set_child_control_data( HControl* );
	virtual HAbstractCell& operator= ( HAbstractCell const& );
	};

template<typename tType = yaal::hcore::HInfo>
class HCell : public HAbstractCell
	{
	tType& f_rtData;
public:
	explicit HCell( yaal::hcore::HInfo const& );
	virtual ~HCell( void );
	virtual yaal::hcore::HString const get_long( void ) = 0;
	virtual yaal::hcore::HString const get_double( void ) = 0;
	virtual yaal::hcore::HString const get_string( void ) = 0;
	virtual char const* get_time( void ) = 0;
	virtual int long get_id( void ) = 0;
	virtual void set_child_control_data( HControl* ) = 0;
	};

class HAbstractRow
	{
public:
	virtual ~HAbstractRow( void );
	virtual void switch_state( void ) = 0;
	virtual int long get_id( void ) = 0;
	virtual bool get_checked( void ) = 0;
	virtual HAbstractCell& operator[]( int ) = 0;
	};

template<typename tType = HItem>
class HRow : public HAbstractRow
	{
	typedef yaal::hcore::HList<tType> model_t;
	typedef typename model_t::iterator iterator_t;
	iterator_t& f_roIterator;
public:
	HRow( iterator_t& );
	virtual HCell<tType>& operator[]( int );
	virtual void switch_state( void );
	virtual int long get_id( void );
	virtual bool get_checked( void );
	};

class HAbstractControler
	{
public:
	class HModelIteratorWrapper;
private:
	class HAbstractModelIterator;
	typedef yaal::hcore::HPointer<HAbstractModelIterator,yaal::hcore::HPointerScalar,yaal::hcore::HPointerRelaxed> iterator_ptr_t;
	class HAbstractModelIterator
		{
		HAbstractModelIterator( HAbstractModelIterator const& );
		virtual HAbstractRow& dereference( void );
		virtual HAbstractRow* call( void );
		virtual void next( void );
		virtual void previous( void );
		virtual iterator_ptr_t& clone( void );
		virtual bool is_equal( HAbstractModelIterator const& );
		virtual bool is_not_equal( HAbstractModelIterator const& );
		virtual bool is_valid( void );
		friend class HModelIteratorWrapper;
	public:
		HAbstractModelIterator( void );
		virtual ~HAbstractModelIterator( void );
		};
protected:
	HControl* f_poControl;
public:
	typedef yaal::hcore::HPointer<HAbstractControler,yaal::hcore::HPointerScalar,yaal::hcore::HPointerRelaxed> ptr_t;
	class HModelIteratorWrapper
		{
		iterator_ptr_t f_oIterator;
	public:
		HModelIteratorWrapper( void );
		explicit HModelIteratorWrapper( iterator_ptr_t const& );
		HModelIteratorWrapper( HModelIteratorWrapper const& );
		HAbstractRow& operator* ( void );
		HAbstractRow* operator-> ( void );
		HModelIteratorWrapper& operator++ ( void );
		HModelIteratorWrapper& operator-- ( void );
		HModelIteratorWrapper& operator= ( HModelIteratorWrapper const& );
		bool operator== ( HModelIteratorWrapper const& );
		bool operator!= ( HModelIteratorWrapper const& );
		bool is_valid( void );
		};
	HAbstractControler( void );
	virtual ~HAbstractControler( void );
	virtual int long size( void ) = 0;
	virtual bool empty( void ) = 0;
	virtual HModelIteratorWrapper begin() = 0;
	virtual HModelIteratorWrapper end() = 0;
	void set_control( HControl* );
private:
	HAbstractControler( HAbstractControler const& );
	HAbstractControler& operator=( HAbstractControler const& );
	};

/*
 * We can decide to give user possibility to choose "model record type"
 * or we can assume that each model record is made of some kind of array
 * and allow user to specify type of elements stored in this array.
 *
 * Former approach is more flexible while the latter is easier to implement.
 * Unfortunatelly latter approach badly limits model type traits
 * (i.e. one would have to use list of arrays of window pointers instead
 * of plain list of window pointers, wide HWindowListControl).
 *
 * We will go for the former.
 */

template<typename tType = HItem>
class HListControler : public HAbstractControler
	{
public:
	typedef yaal::hcore::HList<tType> model_t;
	typedef typename model_t::iterator iterator_t;
	typedef HRow<tType> row_t;
private:
	class HModelIterator : public HAbstractModelIterator
		{
		iterator_t f_oIterator;
		row_t f_oRow;
		HModelIterator( void );
		HModelIterator( HModelIterator const& );
		explicit HModelIterator( iterator_t const& );
		virtual HAbstractRow& dereference( void );
		virtual HAbstractRow* call( void );
		virtual void next( void );
		virtual void previous( void );
		virtual iterator_ptr_t& clone( void );
		virtual bool is_equal( HModelIterator const& );
		virtual bool is_not_equal( HModelIterator const& );
		virtual bool is_valid( void );
		friend class HModelIteratorWrapper;
		friend class HListControler<tType>;
		};
public:
	typedef yaal::hcore::HPointer<HModelIterator,yaal::hcore::HPointerScalar,yaal::hcore::HPointerRelaxed> iterator_ptr_t;
	typedef yaal::hcore::HPointer<HListControler<tType>,yaal::hcore::HPointerScalar,yaal::hcore::HPointerRelaxed> ptr_t;
	typedef yaal::hcore::HPointer<model_t, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> model_ptr_t;
private:
	model_ptr_t f_oList;
public:
	HListControler( model_ptr_t = model_ptr_t() );
	void add_tail( tType const& );
	void add_orderly( tType const&, yaal::hcore::OListBits::sort_order_t = yaal::hcore::OListBits::D_ASCENDING );
	void remove_tail( void );
	model_ptr_t get_model( void );
	virtual bool empty( void );
	virtual int long size( void );
	virtual HModelIteratorWrapper begin();
	virtual HModelIteratorWrapper end();
	};

}

class HListControl : virtual public HSearchableControl
	{
public:
	typedef list_control_helper::HAbstractControler::HModelIteratorWrapper iterator_t;
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
		HColumnInfo ( HColumnInfo const& );
		HColumnInfo& operator = ( HColumnInfo const& );
		friend class HListControl;
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
		iterator_t	f_oCurrentMatch;		/* row that has current pattern match */
		match_t ( ) : f_iColumnWithMatch ( 0 ), f_iMatchNumber ( - 1 ), f_oCurrentMatch() { }
		} f_sMatch;
	iterator_t	f_oIterator; /* helper */ 
	iterator_t	f_oCursor; /* current row highlight (selection or mark or what ever you name it) */
	iterator_t	f_oFirstVisibleRow;	/* pointer to first visible row */
	list_control_helper::HAbstractControler::ptr_t f_oControler;
public:
	HListControl ( HWindow*,		 	/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const*, list_control_helper::HAbstractControler::ptr_t const& = list_control_helper::HListControler<>::ptr_t() );	/* label */
	virtual ~HListControl ( void );
	void add_column ( int const&,									/* at position */
										char const*,									/* column name */
										int const&,									/* width */
										BITS::ALIGN::align_t const& = BITS::ALIGN::D_LEFT,		/* align */
										const type_t& = D_HSTRING,	/* type */
										HControl * = NULL );					/* control associated */
	virtual int set_focus ( char = 0 );
	void set_flags ( FLAGS::list_flags_t, FLAGS::list_flags_t );
	void reset( void );
	list_control_helper::HAbstractControler::ptr_t const& get_controler ( void ) const;
	void remove_current_row();
	int long get_row_count( void );
	void sort( list_control_helper::OSortHelper& );
protected:
	virtual bool get_text_for_cell( int, type_t );
	virtual void set_child_control_data_for_cell( int, HControl* );
	virtual void do_refresh ( void );
	void draw_cell ( int, int, int, HColumnInfo const* const, bool );
	virtual int do_process_input( int );
	virtual bool is_searchable ( void );
	virtual int do_click ( mouse::OMouse& );
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
	HListControl ( HListControl const& );
	HListControl& operator = ( HListControl const& );
	};

namespace list_control_helper
{

template<typename tType>
HListControler<tType>::HListControler( model_ptr_t a_oModel ) : HAbstractControler(), f_oList( a_oModel )
	{
	}

template<typename tType>
void HListControler<tType>::add_tail( tType const& a_tRow )
	{
	M_PROLOG
	f_oList->push_back( a_tRow );
	f_poControl->invalidate();
/*
	int l_iSize = f_oList->size();
	if ( l_iSize > f_iHeightRaw )
		{
		f_iCursorPosition = f_iHeightRaw - 1;
		f_iControlOffset = l_iSize - f_iHeightRaw;
		if ( f_oFirstVisibleRow != f_oList->end() )
			++ (*f_oFirstVisibleRow);
		}
	else
		f_iCursorPosition = l_iSize - 1;
	log_trace << " already has a row " << yaal::hcore::endl;
	if ( f_oFirstVisibleRow == f_oList->end() )
		{
		log_trace << " setting iterator " << yaal::hcore::endl;
		f_oFirstVisibleRow = f_oList->begin();
		}
	n_bNeedRepaint = true;
*/
	return;
	M_EPILOG
	}

template<typename tType>
void HListControler<tType>::remove_tail( void )
	{
	M_PROLOG
	f_oList->remove_tail();
	f_poControl->invalidate();
	return;
	M_EPILOG
	}

template<typename tType>
int long HListControler<tType>::size( void )
	{
	return ( f_oList->size() );
	}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::begin( void )
	{
	return ( HModelIteratorWrapper( iterator_ptr_t( new HModelIterator( f_oList->begin() ) ) ) );
	}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::end( void )
	{
	return ( HModelIteratorWrapper( iterator_ptr_t( new HModelIterator( f_oList->end() ) ) ) );
	}

template<typename tType>
bool HListControler<tType>::empty( void )
	{
	return ( f_oList->empty() );
	}

template<typename tType>
HListControler<tType>::HModelIterator::HModelIterator( iterator_t const& a_oIt ) : f_oIterator( a_oIt ), f_oRow( f_oIterator )
	{
	return;
	}

template<typename tType>
bool HListControler<tType>::HModelIterator::is_valid( void )
	{
	return ( f_oIterator.is_valid() );
	}

template<typename tType>
void HListControler<tType>::HModelIterator::next( void )
	{
	++ f_oIterator;
	return;
	}

template<typename tType>
void HListControler<tType>::HModelIterator::previous( void )
	{
	-- f_oIterator;
	return;
	}

template<typename tType>
HAbstractRow& HListControler<tType>::HModelIterator::dereference( void )
	{
	return ( f_oRow );
	}

template<typename tType>
HRow<tType>::HRow( iterator_t& a_oIt ) : f_roIterator( a_oIt )
	{
	return;
	}

}

/*
template<typename tType>
bool HListControl<tType>::do_is_current_match( void )
	{
	}


template<typename tType>
void HListControl<tType>::add_orderly ( row_t& a_tRow, yaal::hcore::OListBits::sort_order_t a_eOrder )
	{
	M_PROLOG
	f_oList->add_orderly( a_tRow, a_eOrder );
	f_iCursorPosition = 0;
	f_iControlOffset = 0;
	f_oCursor = f_oFirstVisibleRow = f_oList->begin();
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

template<typename tType>
void HListControl<tType>::remove_current_row ( void )
	{
	M_PROLOG
	bool l_bFlag = true;
	if ( f_iControlOffset
			&& ( ( f_iControlOffset + f_iHeightRaw ) == f_oList->size() ) )
		{
		f_iControlOffset --;
		++ (*f_oFirstVisibleRow);
		}
	else if ( f_iCursorPosition && ( f_iCursorPosition == ( f_oList->size() - 1 ) ) )
		f_iCursorPosition --;
	else
		l_bFlag = false;
	if ( (*f_oCursor) == (*f_oFirstVisibleRow) )
		++ (*f_oFirstVisibleRow);
	n_bNeedRepaint = true;
	iterator_t it = f_oCursor;
	if ( l_bFlag )
		++ (*f_oCursor);
	f_oList->erase ( it );
	refresh();
	return;
	M_EPILOG
	}

template<typename tType>
void HListControl<tType>::set_current_row_cell ( int a_iColumn, tType a_tValue )
	{
	(*f_oCursor)[ a_iColumn ] = a_tValue;
	return;
	}

template<typename tType>
typename HListControl<tType>::model_t const& HListControl<tType>::get_data ( void ) const
	{
	return ( (*f_oList) );
	}

template<typename tType>
class CompareListControlItems
	{
	list_control_helper::OSortHelper& f_roSortHelper;
public:
	CompareListControlItems ( list_control_helper::OSortHelper& a_roSortHelper )
		: f_roSortHelper ( a_roSortHelper ) { }
	bool operator() ( tType const&, tType const& ) const;
	};

template<typename tType>
bool CompareListControlItems<tType>::operator() ( tType const& a_oLeft,
		tType const& a_oRight ) const
	{
	M_PROLOG
	tType const& l_oLeft = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oLeft : a_oRight;
	tType const& l_oRight = f_roSortHelper.f_eOrder == yaal::hcore::OListBits::D_ASCENDING ? a_oRight : a_oLeft;
	return ( list_control_helper::compare_cells( l_oLeft[ f_roSortHelper.f_iSortColumn ],
				l_oRight[ f_roSortHelper.f_iSortColumn ], f_roSortHelper ) );
	M_EPILOG
	}

template<typename tType>
void HListControl<tType>::do_sort( list_control_helper::OSortHelper& a_roHelper )
	{
	(*f_oList).sort ( CompareListControlItems<HItem_t<tType> > ( a_roHelper ) );
	f_oFirstVisibleRow = (*f_oList).begin();
	}

*/

}

}

#endif /* not __YAAL_HCONSOLE_HLISTCONTROL_H */

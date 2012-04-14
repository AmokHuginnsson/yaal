/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistcontrol.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCONSOLE_HLISTCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HLISTCONTROL_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hlist.hxx"
#include "hcore/memory.hxx"
#include "tools/collections.hxx"
#include "hconsole/hitem.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/hsearchablecontrol.hxx"

namespace yaal {

namespace hconsole {

class HListControl;

/*! \brief Pack of helpers for "list control" concept.
 *
 * List control helpers are provided as a means of customization
 * of HListControler class.
 */
namespace list_control_helper {

class HListControlModelListenerInterface {
public:
	typedef HListControlModelListenerInterface this_type;
	void refresh( void )
		{ do_refresh(); }
	virtual ~HListControlModelListenerInterface( void )
		{}
protected:
	virtual void do_refresh( void ) = 0;
};

class HListControlModelInterface {
	HListControlModelListenerInterface* _listener;
public:
	typedef HListControlModelInterface this_type;
	typedef yaal::hcore::HPointer<HListControlModelInterface> ptr_t;
	HListControlModelInterface( void )
		: _listener( NULL )
		{}
	virtual ~HListControlModelInterface( void ) {}
	int get_column_count( void ) const {
		M_PROLOG
		return ( do_get_column_count() );
		M_EPILOG
	}
	int long get_row_count( void ) const {
		M_PROLOG
		return ( do_get_row_count() );
		M_EPILOG
	}
	yaal::hcore::HString get_value( int long row_, int col_ ) const {
		M_PROLOG
		return ( do_get_value( row_, col_ ) );
		M_EPILOG
	}
	bool sort( int column_, bool descending_ ) {
		M_PROLOG
		return ( do_sort( column_, descending_ ) );
		M_EPILOG
	}
	void register_listener( HListControlModelListenerInterface* listener_ )
		{ _listener = listener_; }
	void refresh( void ) {
		M_PROLOG
		do_refresh();
		if ( _listener )
			_listener->refresh();
		return;
		M_EPILOG
	}
protected:
	virtual int do_get_column_count( void ) const = 0;
	virtual int long do_get_row_count( void ) const = 0;
	virtual yaal::hcore::HString do_get_value( int long, int ) const = 0;
	virtual bool do_sort( int, bool )
		{ return ( true ); }
	virtual void do_refresh( void ) {}
private:
	HListControlModelInterface( HListControlModelInterface const& );
	HListControlModelInterface& operator = ( HListControlModelInterface const& );
};

template<typename sequence_t>
class HListControlModel : public HListControlModelInterface {
public:
	typedef HListControlModel this_type;
	typedef HListControlModelInterface base_type;
	typedef yaal::hcore::HPointer<sequence_t> sequence_ptr_t;
private:
	int _columnCount;
	sequence_ptr_t _sequence;
public:
	HListControlModel( sequence_ptr_t sequence_, int columnCount_ )
		: HListControlModelInterface(), _columnCount( columnCount_ ), _sequence( sequence_ )
		{}
protected:
	int get_column_count( void ) const
		{ return ( _columnCount ); }
	int long do_get_row_count( void ) const {
		M_PROLOG
		return ( _sequence->get_size() );
		M_EPILOG
	}
	virtual yaal::hcore::HString do_get_value( int long row_, int col_ ) const {
		M_PROLOG
		return ( (*_sequence)[ row_ ][ col_ ] );
		M_EPILOG
	}
};

template<typename list_t>
class HListControlModelListAdaptor : public HListControlModelInterface {
public:
	typedef HListControlModelListAdaptor this_type;
	typedef HListControlModelInterface base_type;
	typedef yaal::hcore::HPointer<list_t> list_ptr_t;
	typedef typename list_ptr_t::value_type value_type;
	typedef yaal::hcore::HArray<value_type*> view_t;
private:
	int _columnCount;
	list_ptr_t _list;
	view_t _view;
public:
	HListControlModelListAdaptor( list_ptr_t list_, int columnCount_ )
		: HListControlModelInterface(), _columnCount( columnCount_ ), _list( list_ ), _view()
		{}
protected:
	int get_column_count( void ) const
		{ return ( _columnCount ); }
	int long do_get_row_count( void ) const {
		M_PROLOG
		M_ASSERT( _list->get_size() >= _view.get_size() );
		return ( _view.get_size() );
		M_EPILOG
	}
	virtual yaal::hcore::HString do_get_value( int long row_, int col_ ) const {
		M_PROLOG
		M_ASSERT( _list->get_size() >= _view.get_size() );
		return ( *(_view[ row_ ][ col_ ]) );
		M_EPILOG
	}
	virtual void do_refresh( void ) {
		M_PROLOG
		_view.clear();
		for ( typename list_t::iterator it( _list->begin() ), end( _list->end() ); it != end; ++ it )
			_view.push_back( &*it );
		return;
		M_EPILOG
	}
};

/*! \brief HListControler sort helper.
 *
 * OSortHelper abstracts compare operation.
 */
struct OSortHelper {
	typedef enum {
		ASCENDING,
		DESCENDING
	} sort_order_t;
	int _sortColumn;
	sort_order_t _order;
	type_id_t _type;
	int long _comparedItems;
	int long _size;
	HWindow* _window;
	void progress( void );
};
template<typename tType>
bool compare_cells( tType const&, tType const&, OSortHelper& );

/*! \brief Interface for basic type of model for HListControl.
 */
class HAbstractCell {
public:
	typedef yaal::hcore::HPointer<HAbstractCell> ptr_t;
	virtual ~HAbstractCell( void );
	virtual yaal::hcore::HString const get_long( void ) = 0;
	virtual yaal::hcore::HString const get_double( void ) = 0;
	virtual yaal::hcore::HString const get_string( void ) = 0;
	virtual yaal::hcore::HString get_time( void ) = 0;
	virtual void set_child_control_data( HControl* ) = 0;
};

/*! \brief Concretization of HAbstractCell.
 *
 * \tparam tType - basic unit of data held in list model.
 */
template<typename tType>
class HCell : public HAbstractCell {
	int _column;
	tType& _data;
public:
	explicit HCell( tType&, int );
	virtual ~HCell( void );
	virtual yaal::hcore::HString const get_long( void );
	virtual yaal::hcore::HString const get_double( void );
	virtual yaal::hcore::HString const get_string( void );
	virtual yaal::hcore::HString get_time( void );
	virtual void set_child_control_data( HControl* );
};

/*! \brief Interface for row based access to data (model) in HListControl.
 */
class HAbstractRow {
public:
	virtual ~HAbstractRow( void );
	virtual void switch_state( void ) = 0;
	virtual int long get_id( void ) = 0;
	virtual bool get_checked( void ) = 0;
	virtual HAbstractCell& operator[]( int ) = 0;
};

/*! \brief Concretization of HAbstractRow.
 *
 * \tparam tType - iterator to basic unit of data held in list model.
 */
template<typename tType>
class HRow : public HAbstractRow {
	typedef tType iterator_t;
	typedef yaal::hcore::HArray<HAbstractCell::ptr_t> cells_t;
	tType& _iterator;
	cells_t _cells;
public:
	HRow( tType& );
	virtual HAbstractCell& operator[]( int );
	virtual void switch_state( void );
	virtual int long get_id( void );
	virtual bool get_checked( void );
};

/*! \brief Interface for HListControl controler from MVC pattern.
 */
class HAbstractControler {
public:
	class HModelIteratorWrapper;
	class HAbstractModelIterator;
	typedef yaal::hcore::HPointer<HAbstractModelIterator> iterator_ptr_t;
	/*! \brief Interface for HListControl mode iterator.
	 */
	class HAbstractModelIterator {
		HAbstractModelIterator( HAbstractModelIterator const& );
		virtual HAbstractRow& dereference( void ) = 0;
		virtual HAbstractRow* call( void ) = 0;
		virtual void next( void ) = 0;
		virtual void previous( void ) = 0;
		virtual void assign_to( iterator_ptr_t& ) const = 0;
		virtual bool is_equal( HAbstractModelIterator const& ) const = 0;
		virtual bool is_not_equal( HAbstractModelIterator const& ) const = 0;
		virtual bool is_valid( void ) const = 0;
		friend class HModelIteratorWrapper;
	public:
		HAbstractModelIterator( void );
		virtual ~HAbstractModelIterator( void );
	};
protected:
	HListControl* _control;
public:
	typedef yaal::hcore::HPointer<HAbstractControler> ptr_t;
	/*! \brief Model iterator proxy.
	 */
	class HModelIteratorWrapper {
		iterator_ptr_t _iteratorPtr;
	public:
		HModelIteratorWrapper( void );
		explicit HModelIteratorWrapper( iterator_ptr_t const& );
		HModelIteratorWrapper( HModelIteratorWrapper const& );
		HAbstractRow& operator* ( void );
		HAbstractRow* operator-> ( void );
		HModelIteratorWrapper& operator++ ( void );
		HModelIteratorWrapper& operator-- ( void );
/* cppcheck-suppress operatorEq */
		HModelIteratorWrapper& operator = ( HModelIteratorWrapper const& );
		iterator_ptr_t& raw( void );
		bool operator == ( HModelIteratorWrapper const& );
		bool operator != ( HModelIteratorWrapper const& );
		bool is_valid( void ) const;
	};
	HAbstractControler( void );
	virtual ~HAbstractControler( void );
	virtual void sort( list_control_helper::OSortHelper& ) = 0;
	virtual int long size( void ) = 0;
	virtual bool empty( void ) = 0;
	virtual HModelIteratorWrapper begin() = 0;
	virtual HModelIteratorWrapper end() = 0;
	virtual HModelIteratorWrapper rbegin() = 0;
	virtual HModelIteratorWrapper rend() = 0;
	virtual void erase( HModelIteratorWrapper& );
	virtual void add_tail( void );
	void set_control( HListControl* );
private:
	HAbstractControler( HAbstractControler const& );
	HAbstractControler& operator = ( HAbstractControler const& );
};

/*! \brief Controler of HListControl in MVC idiom.
 *
 * We can decide to give user possibility to choose "model record type"
 * or we can assume that each model record is made of some kind of array
 * and allow user to specify type of elements stored in this array.
 *
 * Former approach is more flexible while the latter is easier to implement.
 * Unfortunatelly latter approach badly limits model type traits
 * (i.e. one would have to use list of arrays of window pointers instead
 * of plain list of window pointers, vide HWindowListControl).
 *
 * We will go for the former.
 *
 * \tparam tType - basic unit of data held in list model.
 */
template<typename tType = HInfoItem>
class HListControler : public HAbstractControler {
public:
	typedef yaal::hcore::HList<tType> model_t;
	typedef typename model_t::iterator iterator_t;
	typedef HRow<iterator_t> row_t;
private:
	class HModelIterator : public HAbstractModelIterator {
		typedef yaal::hcore::HPointer<HModelIterator> ptr_t;
		HListControler const* _owner;
		iterator_t _iterator;
		row_t _row;
		HModelIterator( void );
		HModelIterator( HModelIterator const& );
		explicit HModelIterator( HListControler const*, iterator_t const& );
		HModelIterator& operator = ( HModelIterator const& );
		virtual HAbstractRow& dereference( void );
		virtual HAbstractRow* call( void );
		virtual void next( void );
		virtual void previous( void );
		virtual void assign_to( HAbstractControler::iterator_ptr_t& ) const;
		virtual bool is_equal( HAbstractModelIterator const& ) const;
		virtual bool is_not_equal( HAbstractModelIterator const& ) const;
		virtual bool is_valid( void ) const;
		iterator_t& raw( void );
		friend class HModelIteratorWrapper;
		friend class HListControler<tType>;
#ifndef _MSC_VER
		friend ptr_t yaal::hcore::make_pointer<HModelIterator>( HListControler<tType>* const&, typename model_t::iterator const& );
		friend ptr_t yaal::hcore::make_pointer<HModelIterator>( HListControler<tType> const* const&, typename model_t::iterator const& );
#else /* #ifndef _MSC_VER */
		template<typename T1, typename T2, typename T3>
		friend yaal::hcore::HPointer<T1> yaal::hcore::make_pointer<T1>( T2 const&, T3 const& );
#endif /* #else #ifndef _MSC_VER */
	public:
		~HModelIterator( void );
	};
public:
	typedef yaal::hcore::HPointer<HModelIterator> iterator_ptr_t;
	typedef yaal::hcore::HPointer<HListControler<tType> > ptr_t;
	typedef yaal::hcore::HPointer<model_t> model_ptr_t;
private:
	model_ptr_t _list;
public:
	HListControler( model_ptr_t = model_ptr_t() );
	void add_tail( tType const& );
	void add_orderly( tType const&, int, OSortHelper::sort_order_t = OSortHelper::ASCENDING );
	void remove_tail( void );
	model_ptr_t get_model( void );
	virtual void sort( list_control_helper::OSortHelper& );
	virtual bool empty( void );
	virtual int long size( void );
	virtual HModelIteratorWrapper begin();
	virtual HModelIteratorWrapper end();
	virtual HModelIteratorWrapper rbegin();
	virtual HModelIteratorWrapper rend();
	virtual void erase( HModelIteratorWrapper& );
	virtual void add_tail( void );
	friend class HModelIterator;
};

}

/*! \brief Implementation of TUI List control class.
 *
 * List control allows fancy representation of row based data with handful
 * of display alteration methods.
 */
class HListControl : virtual public HSearchableControl, public list_control_helper::HListControlModelListenerInterface {
public:
	typedef HListControl this_type;
	typedef HSearchableControl base_type;
	typedef list_control_helper::HAbstractControler::HModelIteratorWrapper iterator_t;
	/*! \brief HListControl settings flags.
	 */
	struct FLAG {
		typedef enum {
			NONE = 0,
			CHECKABLE = 1,
			SORTABLE = 2,
			EDITABLE = 4,
			DRAW_HEADER = 8,
			ALL = -1
		} enum_t;
	};
	typedef yaal::hcore::HStrongEnum<FLAG> flag_t;
	/*! \brief Description of HListControl column meta-data.
	 */
	class HColumnInfo {
		bool _descending;
		int _widthRaw;
		int _width;
		BITS::ALIGN::align_t _align;
		int _shortcutIndex;
		char _shortcut;
		type_id_t _type;
		hcore::HString _name;
		HControl * _control;
	public:
		HColumnInfo( void );
		virtual ~HColumnInfo( void );
		HColumnInfo( HColumnInfo const& );
		HColumnInfo& operator = ( HColumnInfo const& );
		friend class HListControl;
		static int const ADD_AT_THE_END = -1;
	};
protected:
	bool _checkable;      /*!< can items be checked/unchecked */
	bool _sortable;       /*!< can control content be sorted */
	bool _drawHeader;     /*!< should be header driven */
	bool _editable;       /*!< is list interactively editable */
	int  _controlOffset;  /*!< when content is bigger than control
														size this variable keeps
														offset of first row shown */
	int  _cursorPosition; /*!< cursor position relative to control
														begining */
	int  _sumForOne;      /*!< sum of percentage columns width */
	typedef yaal::hcore::HArray<HColumnInfo> header_t;
	header_t _header; /*!< list header info */
/* for internal use only */
	int  _sortColumn;     /*!< column used for current sort operation */
	/*! \brief HListControl search match description.
	 */
	struct match_t {
		int _columnWithMatch;
		int _matchNumber;
		iterator_t _currentMatch; /*!< row that has current pattern match */
		match_t() : _columnWithMatch( 0 ), _matchNumber( -1 ), _currentMatch() { }
	} _match;
	iterator_t _cursor; /*!< current row highlight (selection or mark or what ever you name it) */
	iterator_t _firstVisibleRow;	/*!< pointer to first visible row */
	list_control_helper::HAbstractControler::ptr_t _controler;
public:
	HListControl( HWindow* /* parent */,
			int /* row */,
			int /* col */,
			int /* height */,
			int /* width */,
			char const*,
#ifndef _MSC_VER
			list_control_helper::HAbstractControler::ptr_t const& = hcore::make_pointer<list_control_helper::HListControler<> >( hcore::make_pointer<list_control_helper::HListControler<>::model_t>() ) );	/* label */
#else /* #ifndef _MSC_VER */
			list_control_helper::HAbstractControler::ptr_t const& =  list_control_helper::HListControler<>::ptr_t( new ( memory::yaal ) list_control_helper::HListControler<>( list_control_helper::HListControler<>::model_ptr_t( new ( memory::yaal ) list_control_helper::HListControler<>::model_t() ) ) ) );	/* label */
#endif /* #else #ifndef _MSC_VER */
	virtual ~HListControl ( void );
	void add_column( int,									/* at position */
			char const*,									/* column name */
			int,									/* width */
			BITS::ALIGN::align_t const& = BITS::ALIGN::LEFT,		/* align */
			type_id_t = TYPE::HSTRING,	/* type */
			HControl * = NULL );					/* control associated */
	virtual int set_focus( char = 0 );
	void set_flags( flag_t, flag_t );
	void reset( void );
	list_control_helper::HAbstractControler::ptr_t& get_controler( void );
	void remove_current_row();
	int long get_row_count( void );
	type_id_t get_column_type( int );
protected:
	virtual bool get_text_for_cell( iterator_t&, int, type_id_t );
	virtual void do_refresh( void );
	void draw_cell( iterator_t&, int, int, int, HColumnInfo const* const, bool );
	virtual int do_process_input( int );
	virtual bool is_searchable( void );
	virtual void do_update( void );
	virtual int do_click( mouse::OMouse& );
	virtual void go_to_match( void );
	virtual void go_to_match_previous( void );
	void handle_key_page_up( void );
	void handle_key_page_down( void );
	void handle_key_up( void );
	void handle_key_home( void );
	void handle_key_end( void );
	void handle_key_down( void );
	void handle_key_ctrl_n( void );
	void handle_key_ctrl_p( void );
	void handle_key_space( void );
	void handle_key_tab( void );
private:
	void sort_by_column( int, list_control_helper::OSortHelper::sort_order_t = list_control_helper::OSortHelper::ASCENDING );
	void recalculate_column_widths( void );
	void draw_background( int );
	void draw_header( int );
	void draw_scroll( int );
	HListControl( HListControl const& );
	HListControl& operator = ( HListControl const& );
};

/*! \brief HListControl helper utilities.
 */
namespace list_control_helper {

/*! \brief HListControl sort helper functor.
 */
template<typename tType>
class CompareListControlItems {
	list_control_helper::OSortHelper& _sortHelper;
public:
	CompareListControlItems ( list_control_helper::OSortHelper& sortHelper_ )
		: _sortHelper ( sortHelper_ ) { }
	bool operator() ( tType const&, tType const& ) const;
};

template<typename tType>
HListControler<tType>::HListControler( model_ptr_t model_ ) : HAbstractControler(), _list( model_ ) {
}

template<typename tType>
void HListControler<tType>::add_tail( tType const& row_ ) {
	M_PROLOG
	_list->push_back( row_ );
	_control->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
void HListControler<tType>::add_orderly( tType const& row_, int column_, list_control_helper::OSortHelper::sort_order_t order_ ) {
	M_PROLOG
	list_control_helper::OSortHelper helper =
		{ column_, order_, _control->get_column_type( column_ ),
		0, size(), NULL };
	tools::collections::add_orderly( *_list, row_, CompareListControlItems<tType>( helper ) );
	_control->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
void HListControler<tType>::remove_tail( void ) {
	M_PROLOG
	_list->pop_back();
	_control->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
int long HListControler<tType>::size( void ) {
	return ( _list->size() );
}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::begin( void ) {
	return ( HModelIteratorWrapper( _list->begin() != _list->end() ? hcore::make_pointer<HModelIterator>( this, _list->begin() ) : iterator_ptr_t() ) );
}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::end( void ) {
	return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->end() ) ) );
}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::rbegin( void ) {
	return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->rbegin().base() ) ) );
}

template<typename tType>
HAbstractControler::HModelIteratorWrapper HListControler<tType>::rend( void ) {
	return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->rend().base() ) ) );
}

template<typename tType>
bool HListControler<tType>::empty( void ) {
	return ( _list->empty() );
}

template<typename tType>
HListControler<tType>::HModelIterator::HModelIterator( HListControler const* owner_, iterator_t const& it_ )
	: _owner( owner_ ), _iterator( it_ ), _row( _iterator ) {
	return;
}

template<typename tType>
HListControler<tType>::HModelIterator::~HModelIterator( void ) {
	return;
}

template<typename tType>
bool HListControler<tType>::HModelIterator::is_valid( void ) const {
	HListControler* lc( const_cast<HListControler*>( _owner ) );
	return ( ( _iterator != iterator_t() ) && ( _iterator != lc->_list->end() ) );
}

template<typename tType>
void HListControler<tType>::HModelIterator::next( void ) {
	++ _iterator;
	return;
}

template<typename tType>
void HListControler<tType>::HModelIterator::previous( void ) {
	-- _iterator;
	return;
}

template<typename tType>
HAbstractRow& HListControler<tType>::HModelIterator::dereference( void ) {
	return ( _row );
}

template<typename tType>
HAbstractRow* HListControler<tType>::HModelIterator::call( void ) {
	return ( &_row );
}

template<typename tType>
bool HListControler<tType>::HModelIterator::is_equal( typename HListControler<tType>::HAbstractModelIterator const& it_ ) const {
	return ( _iterator == static_cast<typename HListControler<tType>::HModelIterator const&>( it_ )._iterator );
}

template<typename tType>
bool HListControler<tType>::HModelIterator::is_not_equal( typename HListControler<tType>::HAbstractModelIterator const& it_ ) const {
	return ( _iterator != static_cast<typename HListControler<tType>::HModelIterator const&>( it_ )._iterator );
}

template<typename tType>
void HListControler<tType>::HModelIterator::assign_to( HAbstractControler::iterator_ptr_t& it_ ) const {
	it_ = iterator_ptr_t( hcore::make_pointer<HModelIterator>( _owner, _iterator ) );
	return;
}

template<typename tType>
typename HListControler<tType>::iterator_t& HListControler<tType>::HModelIterator::raw( void ) {
	return ( _iterator );
}

template<typename tType>
HAbstractCell& HRow<tType>::operator[] ( int column_ ) {
	return ( *(_cells[ column_ ]) );
}

template<typename tType>
HCell<tType>::HCell( tType& it_, int column_ ) : _column( column_ ), _data( it_ ) {
	return;
}

template<typename tType>
HCell<tType>::~HCell( void ) {
	return;
}

template<typename tType>
void HListControler<tType>::sort( list_control_helper::OSortHelper& helper_ ) {
	M_PROLOG
	_list->sort( CompareListControlItems<tType>( helper_ ) );
	_control->invalidate();
	M_EPILOG
}

template<typename tType>
void HListControler<tType>::erase( HAbstractControler::HModelIteratorWrapper& it_ ) {
	M_PROLOG
	typename HListControler<tType>::iterator_ptr_t it = it_.raw();
	_list->erase( it->raw() );
	return;
	M_EPILOG
}

template<typename tType>
void HListControler<tType>::add_tail( void ) {
	return;
}

}

typedef yaal::hcore::HExceptionT<HListControl, HSearchableControlException> HListControlException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HLISTCONTROL_HXX_INCLUDED */


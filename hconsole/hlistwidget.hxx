/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlistwidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HLISTWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HLISTWIDGET_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hlist.hxx"
#include "hcore/memory.hxx"
#include "hcore/hbitflag.hxx"
#include "tools/collections.hxx"
#include "hconsole/hitem.hxx"
#include "hconsole/hsearchablewidget.hxx"

namespace yaal {

namespace hconsole {

class HWindow;
class HListWidget;

/*! \brief Pack of helpers for "list widget" concept.
 *
 * List widget helpers are provided as a means of customization
 * of HAsIsValueListModel class.
 */
namespace list_widget_helper {

/*! \brief HAsIsValueListModel sort helper.
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

/*! \brief Interface for basic type of model for HListWidget.
 */
class HAbstractCell {
public:
	typedef yaal::hcore::HPointer<HAbstractCell> ptr_t;
	virtual ~HAbstractCell( void );
	virtual yaal::hcore::HString get_long( void ) const = 0;
	virtual yaal::hcore::HString get_double( void ) const = 0;
	virtual yaal::hcore::HString get_string( void ) const = 0;
	virtual yaal::hcore::HString get_time( void ) const = 0;
	virtual void set_child_widget_data( HWidget* ) = 0;
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
	explicit HCell( tType& it_, int column_ )
		: _column( column_ ), _data( it_ ) {
		return;
	}
	virtual ~HCell( void ) {
		return;
	}
	virtual yaal::hcore::HString get_long( void ) const;
	virtual yaal::hcore::HString get_double( void ) const;
	virtual yaal::hcore::HString get_string( void ) const;
	virtual yaal::hcore::HString get_time( void ) const;
	virtual void set_child_widget_data( HWidget* );
};

/*! \brief Interface for row based access to data (model) in HListWidget.
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
	virtual HAbstractCell& operator[]( int column_ ) {
		return ( *(_cells[ column_ ]) );
	}
	virtual void switch_state( void );
	virtual int long get_id( void );
	virtual bool get_checked( void );
};

/*! \brief Interface for HListWidget model from MVC pattern.
 */
class HAbstractListModel {
public:
	class HModelIteratorWrapper;
	class HAbstractModelIterator;
	typedef yaal::hcore::HPointer<HAbstractModelIterator> iterator_ptr_t;
	/*! \brief Interface for HListWidget mode iterator.
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
	HListWidget* _widget;
public:
	typedef yaal::hcore::HPointer<HAbstractListModel> ptr_t;
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
	HAbstractListModel( void );
	virtual ~HAbstractListModel( void );
	virtual void sort( list_widget_helper::OSortHelper& ) = 0;
	virtual int long size( void ) const = 0;
	virtual bool is_empty( void ) const = 0;
	virtual HModelIteratorWrapper begin() = 0;
	virtual HModelIteratorWrapper end() = 0;
	virtual HModelIteratorWrapper rbegin() = 0;
	virtual HModelIteratorWrapper rend() = 0;
	virtual void erase( HModelIteratorWrapper& );
	virtual void add_tail( void );
	void set_widget( HListWidget* );
private:
	HAbstractListModel( HAbstractListModel const& );
	HAbstractListModel& operator = ( HAbstractListModel const& );
};

/*! \brief Model of HListWidget in MVC idiom.
 *
 * We can decide to give user possibility to choose "model record type"
 * or we can assume that each model record is made of some kind of array
 * and allow user to specify type of elements stored in this array.
 *
 * Former approach is more flexible while the latter is easier to implement.
 * Unfortunatelly latter approach badly limits model type traits
 * (i.e. one would have to use list of arrays of window pointers instead
 * of plain list of window pointers, vide HWindowListWidget).
 *
 * We will go for the former.
 *
 * \tparam tType - basic unit of data held in list model.
 */
template<typename tType = HInfoItem>
class HAsIsValueListModel : public HAbstractListModel {
public:
	typedef yaal::hcore::HList<tType> data_t;
	typedef typename data_t::iterator iterator_t;
	typedef HRow<iterator_t> row_t;
private:
	class HModelIterator;
public:
	typedef yaal::hcore::HPointer<HModelIterator> iterator_ptr_t;
	typedef yaal::hcore::HPointer<HAsIsValueListModel<tType> > ptr_t;
	typedef yaal::hcore::HPointer<data_t> data_ptr_t;
private:
	data_ptr_t _list;
public:
	HAsIsValueListModel( data_ptr_t data_ = data_ptr_t() )
		: HAbstractListModel(),
		_list( data_ ) {
	}
	void add_tail( tType const& row_ );
	void add_orderly( tType const& row_, int column_, list_widget_helper::OSortHelper::sort_order_t order_ = OSortHelper::ASCENDING );
	void remove_tail( void );
	virtual int long size( void ) const {
		return ( _list->size() );
	}
	data_ptr_t get_data( void );
	virtual void sort( list_widget_helper::OSortHelper& );
	virtual bool is_empty( void ) const {
		return ( _list->is_empty() );
	}
	virtual HAbstractListModel::HModelIteratorWrapper begin( void ) {
		return ( HModelIteratorWrapper( _list->begin() != _list->end()
					? hcore::make_pointer<HModelIterator>( this, _list->begin() )
					: iterator_ptr_t() ) );
	}
	virtual HAbstractListModel::HModelIteratorWrapper end( void ) {
		return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->end() ) ) );
	}
	virtual HAbstractListModel::HModelIteratorWrapper rbegin( void ) {
		return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->rbegin().base() ) ) );
	}
	virtual HAbstractListModel::HModelIteratorWrapper rend( void ) {
		return ( HModelIteratorWrapper( hcore::make_pointer<HModelIterator>( this, _list->rend().base() ) ) );
	}
	virtual void erase( HModelIteratorWrapper& );
	virtual void add_tail( void );
	friend class HModelIterator;
};

template<typename tType>
class HAsIsValueListModel<tType>::HModelIterator : public HAbstractModelIterator {
	typedef yaal::hcore::HPointer<HModelIterator> ptr_t;
	HAsIsValueListModel const* _owner;
	iterator_t _iterator;
	row_t _row;
	HModelIterator( void );
	explicit HModelIterator( HAsIsValueListModel const* owner_, iterator_t const& it_ )
		: _owner( owner_ ), _iterator( it_ ), _row( _iterator ) {
		return;
	}
	HModelIterator( HModelIterator const& );
	HModelIterator& operator = ( HModelIterator const& );
	virtual HAbstractRow& dereference( void ) {
		return ( _row );
	}
	virtual HAbstractRow* call( void ) {
		return ( &_row );
	}
	virtual void next( void ) {
		++ _iterator;
		return;
	}
	virtual void previous( void ) {
		-- _iterator;
		return;
	}
	virtual void assign_to( HAbstractListModel::iterator_ptr_t& it_ ) const {
		it_ = iterator_ptr_t( hcore::make_pointer<HModelIterator>( _owner, _iterator ) );
		return;
	}
	virtual bool is_equal( HAbstractModelIterator const& it_ ) const {
		return ( _iterator == static_cast<typename HAsIsValueListModel<tType>::HModelIterator const&>( it_ )._iterator );
	}
	virtual bool is_not_equal( HAbstractModelIterator const& it_ ) const {
		return ( _iterator != static_cast<typename HAsIsValueListModel<tType>::HModelIterator const&>( it_ )._iterator );
	}

	virtual bool is_valid( void ) const {
		HAsIsValueListModel* lc( const_cast<HAsIsValueListModel*>( _owner ) );
		return ( ( _iterator != iterator_t() ) && ( _iterator != lc->_list->end() ) );
	}
	iterator_t& raw( void ) {
		return ( _iterator );
	}
	friend class HModelIteratorWrapper;
	friend class HAsIsValueListModel<tType>;
#ifndef _MSC_VER
	friend ptr_t yaal::hcore::make_pointer<HModelIterator>( HAsIsValueListModel<tType>* const&, typename data_t::iterator const& );
	friend ptr_t yaal::hcore::make_pointer<HModelIterator>( HAsIsValueListModel<tType> const* const&, typename data_t::iterator const& );
#else /* #ifndef _MSC_VER */
	template<typename T1, typename T2, typename T3>
	friend yaal::hcore::HPointer<T1> yaal::hcore::make_pointer<T1>( T2 const&, T3 const& );
#endif /* #else #ifndef _MSC_VER */
public:
	virtual ~HModelIterator( void ) {
		return;
	}
};

}

/*! \brief Implementation of TUI List widget class.
 *
 * List widget allows fancy representation of row based data with handful
 * of display alteration methods.
 */
class HListWidget : virtual public HSearchableWidget {
public:
	typedef HListWidget this_type;
	typedef HSearchableWidget base_type;
	typedef list_widget_helper::HAbstractListModel::HModelIteratorWrapper iterator_t;
	/*! \brief HListWidget settings flags.
	 */
	struct FLAG;
	typedef yaal::hcore::HBitFlag<FLAG> flag_t;
	struct FLAG {
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const NONE;
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const CHECKABLE;
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const SORTABLE;
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const EDITABLE;
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const DRAW_HEADER;
		static M_YAAL_HCONSOLE_PUBLIC_API flag_t const ALL;
	};
	/*! \brief Description of HListWidget column meta-data.
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
		HWidget* _widget;
	public:
		HColumnInfo( void );
		virtual ~HColumnInfo( void );
		HColumnInfo( HColumnInfo const& );
		HColumnInfo& operator = ( HColumnInfo const& );
		friend class HListWidget;
		static int const ADD_AT_THE_END = -1;
	};
protected:
	bool _checkable;      /*!< can items be checked/unchecked */
	bool _sortable;       /*!< can widget content be sorted */
	bool _drawHeader;     /*!< should be header driven */
	bool _editable;       /*!< is list interactively editable */
	int  _widgetOffset;   /*!< when content is bigger than widget
	                           size this variable keeps
	                           offset of first row shown */
	int  _cursorPosition; /*!< cursor position relative to widget
	                           begining */
	int  _sumForOne;      /*!< sum of percentage columns width */
	typedef yaal::hcore::HArray<HColumnInfo> header_t;
	header_t _header; /*!< list header info */
/* for internal use only */
	int  _sortColumn;     /*!< column used for current sort operation */
	/*! \brief HListWidget search match description.
	 */
	struct match_t {
		int _columnWithMatch;
		int _matchNumber;
		iterator_t _currentMatch; /*!< row that has current pattern match */
		match_t() : _columnWithMatch( 0 ), _matchNumber( -1 ), _currentMatch() { }
	} _match;
	iterator_t _cursor; /*!< current row highlight (selection or mark or what ever you name it) */
	iterator_t _firstVisibleRow;	/*!< pointer to first visible row */
	list_widget_helper::HAbstractListModel::ptr_t _model;
public:
	HListWidget( HWindow* parent,
			int row,
			int col,
			int height,
			int width,
			yaal::hcore::HString const& label,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface(),
#ifndef _MSC_VER
			list_widget_helper::HAbstractListModel::ptr_t const& = hcore::make_pointer<list_widget_helper::HAsIsValueListModel<> >( hcore::make_pointer<list_widget_helper::HAsIsValueListModel<>::data_t>() )
#else /* #ifndef _MSC_VER */
			list_widget_helper::HAbstractListModel::ptr_t const& = list_widget_helper::HAsIsValueListModel<>::ptr_t( new ( memory::yaal ) list_widget_helper::HAsIsValueListModel<>( list_widget_helper::HAsIsValueListModel<>::data_ptr_t( new ( memory::yaal ) list_widget_helper::HAsIsValueListModel<>::data_t() ) ) )
#endif /* #else #ifndef _MSC_VER */
	);
	virtual ~HListWidget ( void );
	void add_column( int atPosition,
			yaal::hcore::HString const& columnName,
			int width,
			BITS::ALIGN::align_t const& align = BITS::ALIGN::LEFT,
			type_id_t type = TYPE::HSTRING,
			HWidget* associatedWidget = NULL );
	void set_flags( flag_t, flag_t );
	void reset( void );
	list_widget_helper::HAbstractListModel::ptr_t& get_model( void );
	void remove_current_row();
	int long get_row_count( void );
	type_id_t get_column_type( int );
protected:
	virtual bool get_text_for_cell( iterator_t&, int, type_id_t );
	virtual void do_paint( void );
	void draw_cell( iterator_t&, int, int, int, HColumnInfo const* const, bool );
	virtual int do_process_input( int );
	virtual bool is_searchable( void );
	virtual void do_update( void );
	virtual bool do_click( mouse::OMouse& );
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
	void scroll_up( void );
	void scroll_down( void );
private:
	void sort_by_column( int, list_widget_helper::OSortHelper::sort_order_t = list_widget_helper::OSortHelper::ASCENDING );
	void recalculate_column_widths( void );
	void draw_background( int );
	void draw_header( int );
	void draw_scroll( int );
	HListWidget( HListWidget const& );
	HListWidget& operator = ( HListWidget const& );
};

/*! \brief HListWidget helper utilities.
 */
namespace list_widget_helper {

/*! \brief HListWidget sort helper functor.
 */
template<typename tType>
class CompareListWidgetItems {
	list_widget_helper::OSortHelper& _sortHelper;
public:
	CompareListWidgetItems ( list_widget_helper::OSortHelper& sortHelper_ )
		: _sortHelper ( sortHelper_ ) { }
	bool operator() ( tType const&, tType const& ) const;
};

template<typename tType>
void HAsIsValueListModel<tType>::add_tail( tType const& row_ ) {
	M_PROLOG
	_list->push_back( row_ );
	_widget->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
void HAsIsValueListModel<tType>::add_orderly( tType const& row_, int column_, list_widget_helper::OSortHelper::sort_order_t order_ ) {
	M_PROLOG
	list_widget_helper::OSortHelper helper =
		{ column_, order_, _widget->get_column_type( column_ ),
		0, size(), NULL };
	tools::collections::add_orderly( *_list, row_, CompareListWidgetItems<tType>( helper ) );
	_widget->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
void HAsIsValueListModel<tType>::remove_tail( void ) {
	M_PROLOG
	_list->pop_back();
	_widget->invalidate();
	return;
	M_EPILOG
}

template<typename tType>
void HAsIsValueListModel<tType>::sort( list_widget_helper::OSortHelper& helper_ ) {
	M_PROLOG
	_list->sort( CompareListWidgetItems<tType>( helper_ ) );
	_widget->invalidate();
	M_EPILOG
}

template<typename tType>
void HAsIsValueListModel<tType>::erase( HAbstractListModel::HModelIteratorWrapper& it_ ) {
	M_PROLOG
	typename HAsIsValueListModel<tType>::iterator_ptr_t it = it_.raw();
	_list->erase( it->raw() );
	return;
	M_EPILOG
}

template<typename tType>
void HAsIsValueListModel<tType>::add_tail( void ) {
	return;
}

}

typedef yaal::hcore::HExceptionT<HListWidget, HSearchableWidgetException> HListWidgetException;

class HListWidgetAttributes : virtual public HSearchableWidgetAttributes {
	bool _checkable;  /*!< can items be checked/unchecked */
	bool _checkableSet;
	bool _sortable;   /*!< can widget content be sorted */
	bool _sortableSet;
	bool _drawHeader; /*!< should be header driven */
	bool _drawHeaderSet;
	bool _editable;   /*!< is list interactively editable */
	bool _editableSet;
protected:
	virtual void do_apply( HWidget& ) const;
public:
	HListWidgetAttributes( void );
	HListWidgetAttributes& checkable( bool );
	HListWidgetAttributes& sortable( bool );
	HListWidgetAttributes& drawheader( bool );
	HListWidgetAttributes& editable( bool );
};

class HListWidgetCreator : virtual public HSearchableWidgetCreator {
protected:
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HLISTWIDGET_HXX_INCLUDED */


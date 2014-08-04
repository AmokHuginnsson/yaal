/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindowlist.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwindowlistwidget.hxx"
#include "hcore/memory.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::list_widget_helper;

namespace yaal {

namespace hconsole {

HWindowListWidget::HWindowListWidget ( HWindow * parent_, int row_,
		int column_, int height_, int width_, char const * label_,
		HAbstractListModel::ptr_t const& model_, model_t::cyclic_iterator& foregroundWindow_ )
	:	HWidget( parent_, row_, column_, height_, width_, label_,
			HWidgetAttributes().label_position( HWidget::LABEL::POSITION::STACKED ) ),
		HSearchableWidget( false ),
		HListWidget( parent_, row_, column_, height_,
				width_, label_, HWidgetAttributesInterface(), model_ ),
		_foregroundWindow( foregroundWindow_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HWindowListWidget::do_process_input( int code_ ) {
	M_PROLOG
	code_ = HListWidget::do_process_input( code_ );
	int size = static_cast<int>( _model->size() );
	if ( size > 0 ) {
		if ( ( code_ == '\r' ) || ( code_ == ' ' ) ) {
			code_ = KEY<'\t'>::meta;
			iterator_t it = _model->begin();
			while ( it != _cursor )
				++ it, ++ _foregroundWindow;
			-- _foregroundWindow;
		}
	}
	return ( code_ );
	M_EPILOG
}

namespace list_widget_helper {

template<>
yaal::hcore::HString const HCell<HWindow::ptr_t>::get_string( void ) {
	M_PROLOG
	return ( _data->get_title() );
	M_EPILOG
}

template<>
bool compare_cells( HWindow::ptr_t const& left_, HWindow::ptr_t const& right_, OSortHelper & sortHelper_ ) {
	M_PROLOG
	sortHelper_.progress();
	return ( strcasecmp ( left_->get_title(),
			 right_->get_title() ) > 0 );
	M_EPILOG
}

typedef yaal::hcore::HList<HWindow::ptr_t>::iterator window_iterator_t;

template<>
HRow<window_iterator_t>::HRow( iterator_t& it_ ) : _iterator( it_ ), _cells( 1 ) {
	_cells[ 0 ] = make_pointer<HCell<window_iterator_t> >( ref( _iterator ), 0 );
	return;
}

template<>
void HRow<window_iterator_t>::switch_state( void ) {
	return;
}

template<>
int long HRow<window_iterator_t>::get_id( void ) {
	return ( -1 );
}

template<>
bool HRow<window_iterator_t>::get_checked( void ) {
	return ( false );
}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_long( void ) {
	return ( 0 );
}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_double( void ) {
	return ( 0 );
}

template<>
yaal::hcore::HString const HCell<window_iterator_t>::get_string( void ) {
	M_PROLOG
	return ( (*_data)->get_title() );
	M_EPILOG
}

template<>
HString HCell<window_iterator_t>::get_time( void ) {
	return ( "" );
}

template<>
void HCell<window_iterator_t>::set_child_widget_data( HWidget* ) {
	return;
}

template<>
bool CompareListWidgetItems<HWindow::ptr_t>::operator() ( HWindow::ptr_t const& left_,
		HWindow::ptr_t const& right_ ) const {
	M_PROLOG
	HWindow::ptr_t const& left = _sortHelper._order == OSortHelper::ASCENDING ? left_ : right_;
	HWindow::ptr_t const& right = _sortHelper._order == OSortHelper::ASCENDING ? right_ : left_;
	return ( strcasecmp( left->get_title(), right->get_title() ) > 0 );
	M_EPILOG
}

}

}

}


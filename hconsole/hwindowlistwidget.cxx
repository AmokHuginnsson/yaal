/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
		HAbstractListModel::ptr_t const& model_, cyclic_iterator& foregroundWindow_ )
	:	HWidget( parent_, row_, column_, height_, width_, label_, HWidgetAttributes().label_position( HWidget::LABEL::POSITION::STACKED ) )
	, HSearchableWidget( HSearchableWidgetAttributes() )
	, HListWidget( parent_, row_, column_, height_, width_, label_, HListWidgetAttributes().sortable( false ), model_ )
	, _foregroundWindow( foregroundWindow_ ) {
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
			iterator_t it( _model->begin() );
			while ( it != _cursor ) {
				++ it;
				++ _foregroundWindow;
			}
			(*_foregroundWindow)->schedule_repaint( true );
			_window->get_tui()->schedule_repaint();
			code_ = 0;
		}
	}
	return code_;
	M_EPILOG
}

namespace list_widget_helper {

template<>
yaal::hcore::HString HCell<HWindow::ptr_t>::get_string( void ) const {
	M_PROLOG
	return ( _data->get_title() );
	M_EPILOG
}

template<>
bool compare_cells( HWindow::ptr_t const& left_, HWindow::ptr_t const& right_, OSortHelper & sortHelper_ ) {
	M_PROLOG
	sortHelper_.progress();
	return ( stricasecmp( left_->get_title(), right_->get_title() ) > 0 );
	M_EPILOG
}

typedef yaal::hcore::HList<HWindow::ptr_t>::iterator window_iterator_t;

template<>
HRow<window_iterator_t>::HRow( iterator_t* it_ ) : _iterator( it_ ), _cells( 1 ) {
	_cells[ 0 ] = make_pointer<HCell<window_iterator_t> >( ref( *_iterator ), 0 );
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
int long long HCell<window_iterator_t>::get_integer( void ) const {
	return ( 0 );
}

template<>
double long HCell<window_iterator_t>::get_real( void ) const {
	return ( 0 );
}

template<>
yaal::hcore::HString HCell<window_iterator_t>::get_string( void ) const {
	M_PROLOG
	return ( (*_data)->get_title() );
	M_EPILOG
}

template<>
HString HCell<window_iterator_t>::get_time( void ) const {
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
	return ( stricasecmp( left->get_title(), right->get_title() ) > 0 );
	M_EPILOG
}

}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/hlog.hxx"
#include "hmenuwidget.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::mouse;

namespace yaal {

namespace hconsole {

HMenuWidget::HMenuWidget( HWindow* parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_ )
	: HWidget( parent_, row_, column_, height_, width_, label_,
			HWidgetAttributes().label_position( HWidget::LABEL::POSITION::STACKED ) ),
	HTreeWidget( parent_, row_, column_, height_, width_, label_ ),
	_process( nullptr ) {
	M_PROLOG
	return;
	M_EPILOG
}

HMenuWidget::~HMenuWidget ( void ) {
	M_PROLOG
	_process = nullptr;
	return;
	M_DESTRUCTOR_EPILOG
}

void HMenuWidget::init( HTUIProcess* process_ ) {
	M_PROLOG
	M_ASSERT( process_ );
	if ( _view.get_root() ) {
		M_THROW( "menu already initialized", errno );
	}
	_process = process_;
	return;
	M_EPILOG
}

OMenuItem const* HMenuWidget::get_selected( void ) {
	M_PROLOG
	menu_model_t::HAsIsValueTreeModelNode* node( static_cast<menu_model_t::HAsIsValueTreeModelNode*>( (**_selected).data().get() ) );
	OMenuItem const* menu( &node->get() );
	return menu;
	M_EPILOG
}

int HMenuWidget::do_process_input( int code_ ) {
	M_PROLOG
	if ( ! _view.get_root() ) {
		M_THROW( _( "menu not initialized" ), errno );
	}
	code_ = HTreeWidget::do_process_input ( code_ );
	if ( ( code_ == '\r' ) || ( code_ == ' ' ) ) {
		OMenuItem const* menu( get_selected() );
		if ( !! menu->_call ) {
			menu->call();
		}
		code_ = 0;
	}
	return code_;
	M_EPILOG
}

bool HMenuWidget::do_click( OMouse& mouse_ ) {
	M_PROLOG
	tree_view_t::node_t selected( _selected );
	bool unfolded( (**_selected).is_unfolded() );
	bool handled( HTreeWidget::do_click( mouse_ ) );
	if ( handled && ( _selected == selected ) && ( (**_selected).is_unfolded() == unfolded ) ) {
		OMenuItem const* menu( get_selected() );
		if ( !! menu->_call ) {
			menu->_call();
		}
	}
	return handled;
	M_EPILOG
}

void HMenuWidget::set_data( HMenuWidget::data_ptr_t data_ ) {
	M_PROLOG
	_model = make_pointer<HAsIsValueTreeModel<OMenuItem> >( data_ );
	menu_model_t::data_t::node_t node( nullptr );
	_view.clear();
	node = static_cast<menu_model_t*>( _model.get() )->get_data()->get_root();
	on_model_changed();
	if ( ! _selected && node->has_children() ) {
		_selected = &*_view.get_root()->begin();
	}
	schedule_repaint();
	return;
	M_EPILOG
}

}

}


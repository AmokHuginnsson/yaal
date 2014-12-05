/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenuwidget.cxx - this file is integral part of `yaal' project.

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
	_process( NULL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HMenuWidget::~HMenuWidget ( void ) {
	M_PROLOG
	_process = NULL;
	return;
	M_EPILOG
}

void HMenuWidget::init( HTUIProcess* process_ ) {
	M_PROLOG
	M_ASSERT( process_ );
	if ( _view.get_root() ) {
		M_THROW( "menu already initialised", errno );
	}
	_process = process_;
	return;
	M_EPILOG
}

OMenuItem const* HMenuWidget::get_selected( void ) {
	M_PROLOG
	menu_model_t::HAsIsValueTreeModelNode* node( static_cast<menu_model_t::HAsIsValueTreeModelNode*>( (**_selected).data().get() ) );
	OMenuItem const* menu( &node->get() );
	return ( menu );
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
			menu->_call();
		}
		code_ = 0;
	}
	return ( code_ );
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
	return ( handled );
	M_EPILOG
}

void HMenuWidget::set_data( HMenuWidget::data_ptr_t data_ ) {
	M_PROLOG
	_model = make_pointer<HAsIsValueTreeModel<OMenuItem> >( data_ );
	menu_model_t::data_t::node_t node( NULL );
	_view.clear();
	node = static_cast<menu_model_t*>( _model.get() )->get_data()->get_root();
	on_model_changed();
	if ( ! _selected && node->has_childs() ) {
		_selected = &*_view.get_root()->begin();
	}
	schedule_repaint();
	return;
	M_EPILOG
}

}

}


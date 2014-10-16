/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomboboxwidget.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcomboboxwidget.hxx"
#include "hcore/foreach.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hwindow.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HComboboxWidget::HComboboxWidget ( HWindow * parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HEditWidget( NULL, 0, 0, 0, 0, HString() ),
		HSearchableWidget( attr_ ),
		HListWidget( NULL, 0, 0, 0, 0, HString() ),
		_mode( MODE::EDITCONTROL ),
		_droppedWidth( 0 ),
		_origSelection() {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HComboboxWidget::~HComboboxWidget ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HComboboxWidget::set_dropped_width( int droppedWidth_ ) {
	M_PROLOG
	_droppedWidth = droppedWidth_;
	return;
	M_EPILOG
}

void HComboboxWidget::do_kill_focus( void ) {
	M_PROLOG
	if ( _mode == MODE::LISTCONTROL ) {
		close_combo( ACTION::CANCEL );
	}
	HWidget::do_kill_focus();
	return;
	M_EPILOG
}

void HComboboxWidget::do_paint( void ) {
	M_PROLOG
	int width( 0 );
	HConsole& cons( HConsole::get_instance() );
	if ( _mode == MODE::EDITCONTROL ) {
/* ripped from HWidget::draw_label() */
		width = ( _width > 0 ) ? _width
			: cons.get_width() + _width - _columnRaw;
/* end of ripped part */
		HEditWidget::do_paint();
		cons.move( _rowRaw, _columnRaw + width - 1 );
		set_attr_label();
		cons.addch( GLYPHS::ARROW::DOWN );
		cons.move( _rowRaw, _columnRaw + HEditWidget::_cursorPosition );
		_heightRaw = 0;
	} else {
		int height( _height );
		width = _width;
		_width = _droppedWidth;
		int size = static_cast<int>( _model->size() );
		int hR = ( _drawLabel ? 1 : 0 ) + ( _drawHeader && ( _labelPosition == LABEL::POSITION::STACKED ) ? 1 : 0 );
		if ( size < ( _height - hR ) )
			_height = size + hR;
		HListWidget::do_paint();
		_height = height;
		_width = width;
	}
	return;
	M_EPILOG
}

int HComboboxWidget::do_process_input( int code_ ) {
	M_PROLOG
	int code( 0 );
	if ( _mode == MODE::EDITCONTROL ) {
		switch ( code_ ) {
			case ( KEY_CODES::UP ): {
				_mode = MODE::LISTCONTROL;
				save_selection();
			} break;
			case ( KEY_CODES::DOWN ): {
				_mode = MODE::LISTCONTROL;
				save_selection();
			} break;
			default: {
				code = HEditWidget::do_process_input( code_ );
			}
		}
		schedule_repaint();
	} else {
		if ( code_ == '\r' ) {
			close_combo( ACTION::APPLY );
		} else if ( code_ == KEY_CODES::ESC ) {
			close_combo( ACTION::CANCEL );
		} else {
			code = HListWidget::do_process_input( code_ );
		}
	}
	return ( code );
	M_EPILOG
}

bool HComboboxWidget::do_click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	bool handled( false );
	if ( _mode == MODE::EDITCONTROL ) {
		handled = HEditWidget::do_click( mouse_ );
		if ( ! handled ) {
			_widthRaw = ( _width > 0 ) ? _width
				: HConsole::get_instance().get_width() + _width - _columnRaw;
			if ( mouse_._column == ( _columnRaw + _widthRaw - 1 ) ) {
				_mode = MODE::LISTCONTROL;
				schedule_repaint();
				handled = true;
			}
		}
	} else {
		handled = HListWidget::do_click( mouse_ );
		if ( ! handled ) {
			close_combo( ACTION::APPLY );
			handled = true;
		}
	}
	return ( handled );
	M_EPILOG
}

void HComboboxWidget::close_combo( ACTION::action_t action_ ) {
	M_PROLOG
	_mode = MODE::EDITCONTROL;
	if ( action_ == ACTION::APPLY ) {
		if ( ! _model->is_empty() ) {
			set_text( (*_cursor)[ 0 ].get_string() );
		}
		save_selection();
	} else {
		restore_selection();
	}
	_window->schedule_repaint( true );
	return;
	M_EPILOG
}

void HComboboxWidget::save_selection( void ) {
	M_PROLOG
	_origSelection._cursor = _cursor;
	_origSelection._firstVisibleRow = _firstVisibleRow;
	_origSelection._widgetOffset = HListWidget::_widgetOffset;
	_origSelection._cursorPosition = HListWidget::_cursorPosition;
	return;
	M_EPILOG
}

void HComboboxWidget::restore_selection( void ) {
	M_PROLOG
	_cursor = _origSelection._cursor;
	_firstVisibleRow = _origSelection._firstVisibleRow;
	HListWidget::_widgetOffset = _origSelection._widgetOffset;
	HListWidget::_cursorPosition = _origSelection._cursorPosition;
	schedule_repaint();
	return;
	M_EPILOG
}

void HComboboxWidget::select_by_index( int ) {
	save_selection();
}

int HComboboxWidget::get_selected_index( void ) const {
	return ( _origSelection._widgetOffset + _origSelection._cursorPosition );
}

HInfo const& HComboboxWidget::do_get_data( void ) const {
	return (  );
}

void HComboboxWidget::do_set_data( HInfo const& ) {
}

yaal::hcore::HString const& HComboboxWidget::get_selected_text( void ) const {
	return ( _string );
}


HComboboxWidgetAttributes::HComboboxWidgetAttributes( void )
	: HEditWidgetAttributes(), HListWidgetAttributes(),
	_droppedWidth( 0 ),
	_droppedWidthSet( false ) {
}

void HComboboxWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HListWidgetAttributes::do_apply( widget_ );
	HEditWidgetAttributes::do_apply( widget_ );
	HComboboxWidget* widget( dynamic_cast<HComboboxWidget*>( &widget_ ) );
	if ( widget ) {
		if ( _droppedWidthSet ) {
			widget->set_dropped_width( _droppedWidth );
		}
	}
	return;
	M_EPILOG
}

HComboboxWidgetAttributes& HComboboxWidgetAttributes::dropped_width( int droppedWidth_ ) {
	M_PROLOG
	_droppedWidth = droppedWidth_;
	_droppedWidthSet = true;
	return ( *this );
	M_EPILOG
}

class HComboboxWidgetCreator : public virtual HListWidgetCreator, public virtual HEditWidgetCreator {
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
};

HWidget::ptr_t HComboboxWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HComboboxWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	attrs.label_position( r._labelPosition ).label_decoration( r._labelDecoration );
	HWidget* combobox( new HComboboxWidget( window_, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( combobox->get_pointer(), node_ );
	return ( combobox->get_pointer() );
	M_EPILOG
}

bool HComboboxWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	bool ok( false );
	if ( !ok ) {
		ok = HListWidgetCreator::do_prepare_attributes( attributes_, node_ );
	}
	if ( !ok ) {
		ok = HEditWidgetCreator::do_prepare_attributes( attributes_, node_ );
	}
	if ( !ok ) {
		HComboboxWidgetAttributes& attrs( dynamic_cast<HComboboxWidgetAttributes&>( attributes_ ) );
		HString const& name( node_.get_name() );
		if ( name == "dropped_width" ) {
			attrs.dropped_width( lexical_cast<int>( xml::node_val( node_ ) ) );
			ok = true;
		}
	}
	return ( ok );
	M_EPILOG
}

bool HComboboxWidgetCreator::do_apply_resources( HWidget::ptr_t widget_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	bool ok( false );
	if ( !ok ) {
		ok = HListWidgetCreator::do_apply_resources( widget_, node_ );
	}
	if ( !ok ) {
		ok = HEditWidgetCreator::do_apply_resources( widget_, node_ );
	}
	return ( ok );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "combobox",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new HComboboxWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


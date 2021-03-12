/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcomboboxwidget.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hwindow.hxx"
#include "tools/keycode.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HComboboxWidget::HComboboxWidget( HWindow* parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HEditWidget( nullptr, 0, 0, 0, 0, HString() ),
		HSearchableWidget( attr_ ),
		HListWidget( nullptr, 0, 0, 0, 0, HString() ),
		_mode( MODE::EDITCONTROL ),
		_droppedWidth( 0 ),
		_origSelection(),
		_infoInteger( 0 ),
		_none( nullptr ),
		_noneText() {
	M_PROLOG
	attr_.apply( *this );
	_readOnly = true;
	add_column( -1, HColumnInfo::ptr_t( new ( memory::yaal ) HColumnInfo( "", 1, HWidget::BITS::ALIGN::LEFT, TYPE::HSTRING ) ) );
	HListWidget::set_flags(
			HListWidget::FLAG::NONE,
			HListWidget::FLAG::DRAW_HEADER
			| HListWidget::FLAG::EDITABLE
			| HListWidget::FLAG::CHECKABLE
			| HListWidget::FLAG::SORTABLE
	);
	return;
	M_EPILOG
}

HComboboxWidget::~HComboboxWidget ( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
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
	HConsole& cons( HConsole::get_instance() );
	if ( _mode == MODE::EDITCONTROL ) {
/* ripped from HWidget::draw_label() */
		int width( ( _width > 0 ) ? _width : cons.get_width() + _width - _columnRaw );
/* end of ripped part */
		HEditWidget::do_paint();
		cons.move( _rowRaw, _columnRaw + width - 1 );
		set_attr_label();
		cons.addch( GLYPH::ARROW::DOWN );
		cons.move( _rowRaw, _columnRaw + HEditWidget::_cursorPosition );
	} else {
		int height( _height );
		int width( _width );
		if ( _droppedWidth ) {
			_width = _droppedWidth;
		}
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

void HComboboxWidget::do_update( void ) {
	M_PROLOG
	prepend_none();
	HListWidget::do_update();
	save_selection();
	return;
	M_EPILOG
}

int HComboboxWidget::do_process_input( int code_ ) {
	M_PROLOG
	int code( 0 );
	if ( _mode == MODE::EDITCONTROL ) {
		switch ( code_ ) {
			case ( KEY_CODE::UP ): {
				_mode = MODE::LISTCONTROL;
				save_selection();
			} break;
			case ( KEY_CODE::DOWN ): {
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
		} else if ( code_ == KEY_CODE::ESCAPE ) {
			close_combo( ACTION::CANCEL );
		} else {
			code = HListWidget::do_process_input( code_ );
		}
	}
	return code;
	M_EPILOG
}

bool HComboboxWidget::do_hit_test( int row_, int col_ ) const {
	M_PROLOG
	bool hit( false );
	if ( _mode == MODE::EDITCONTROL ) {
		/*
		 * We try to reuse as much of HWidget::do_hit_test() as possible.
		 * We virtually move mouse cursor to the very bottom of expanded
		 * combobox.
		 *
		 * HListWidget in HComboboxWidget does not draw column headers
		 * so we need to compensate for that too.
		 *
		 * _heightRaw is compensated for disabled column headers in HListWidget::do_paint(),
		 * but for MODE::EDITCONTROL we do not call it, so _heightRaw calculated in draw_label()
		 * is 1 too big here.
		 */
		hit = HEditWidget::do_hit_test( row_ + _heightRaw - 1, col_ );
		if ( row_ != _rowRaw ) {
			hit = false;
		}
	} else {
		hit = HListWidget::do_hit_test( row_, col_ );
	}
	return hit;
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
	return handled;
	M_EPILOG
}

void HComboboxWidget::close_combo( ACTION action_ ) {
	M_PROLOG
	_mode = MODE::EDITCONTROL;
	if ( action_ == ACTION::APPLY ) {
		save_selection();
	} else {
		restore_selection();
	}
	if ( _window ) {
		_window->schedule_repaint( true );
	}
	return;
	M_EPILOG
}

void HComboboxWidget::save_selection( void ) {
	M_PROLOG
	_origSelection._cursor = _cursor;
	_origSelection._firstVisibleRow = _firstVisibleRow;
	_origSelection._widgetOffset = HListWidget::_widgetOffset;
	_origSelection._cursorPosition = HListWidget::_cursorPosition;
	if ( ! _model->is_empty() ) {
		set_text( (*_cursor)[ 0 ].get_string() );
	}
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

void HComboboxWidget::prepend_none( void ) {
	M_PROLOG
	list_widget_helper::HAsIsValueListModel<>* model( dynamic_cast<list_widget_helper::HAsIsValueListModel<>*>( get_model().raw() ) );
	if ( model ) {
		list_widget_helper::HAsIsValueListModel<>::data_ptr_t data( model->get_data() );
		if ( data->is_empty() || ! _none || ( &data->head() != _none ) ) {
			data->emplace_front( 1 );
			_none = &data->head();
			(*_none)[0].set_string( _noneText );
			_cursor = _origSelection._cursor = model->begin();
			_firstVisibleRow = _origSelection._firstVisibleRow = model->begin();
			HListWidget::_widgetOffset = _origSelection._widgetOffset = 0;
			HListWidget::_cursorPosition = _origSelection._cursorPosition = 0;
		}
	}
	return;
	M_EPILOG
}

void HComboboxWidget::set_none_text( yaal::hcore::HString const& text_ ) {
	_noneText = text_;
	return;
}

void HComboboxWidget::select_by_index( int index_ ) {
	M_PROLOG
	HScopedValueReplacement<int> heightRaw( _heightRaw, _height > 0 ? _height : HConsole::get_instance().get_height() + _height - _rowRaw );
	set_cursor_position( index_ + 1 );
	save_selection();
	return;
	M_EPILOG
}

int HComboboxWidget::get_selected_index( void ) const {
	return ( _origSelection._widgetOffset + _origSelection._cursorPosition - 1 );
}

void HComboboxWidget::do_set_data( HInfo const& data_ ) {
	M_PROLOG
	select_by_index( static_cast<int>( data_.get_integer() ) );
	return;
	M_EPILOG
}

HInfo const& HComboboxWidget::do_get_data( void ) const {
	M_PROLOG
	_infoInteger.set_integer( get_selected_index() );
	return ( _infoInteger );
	M_EPILOG
}

yaal::hcore::HString const& HComboboxWidget::get_selected_text( void ) const {
	return ( _string );
}

HComboboxWidgetAttributes::HComboboxWidgetAttributes( void )
	: HEditWidgetAttributes(), HListWidgetAttributes(),
	_droppedWidth( 0 ),
	_droppedWidthSet( false ),
	_noneText(),
	_noneTextSet( false ) {
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
		if ( _noneTextSet ) {
			widget->set_none_text( _noneText );
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

HComboboxWidgetAttributes& HComboboxWidgetAttributes::none_text( yaal::hcore::HString const& noneText_ ) {
	M_PROLOG
	_noneText = noneText_;
	_noneTextSet = true;
	return ( *this );
	M_EPILOG
}

HWidget::ptr_t HComboboxWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HComboboxWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HComboboxWidget* combobox( create_widget<HComboboxWidget>( window_, r._row, r._column, r._height, r._width, r._label, attrs ) );
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
		} else if ( name == "none_text" ) {
			attrs.none_text( xml::node_val( node_ ) );
		}
	}
	return ok;
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
	return ok;
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "combobox",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HComboboxWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


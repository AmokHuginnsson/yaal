/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwidget.hxx"
#include "hconsole.hxx"
#include "hwindow.hxx"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

static int const LABEL_DECORATION_SIZE( static_cast<int>( sizeof ( "  :" ) - 1 ) );

HWidget::OAttribute const HWidget::DEFAULT_ATTRS = { COLOR::ATTR_DEFAULT, COLOR::ATTR_DEFAULT };
bool HWidget::OAttribute::operator == ( OAttribute const& attr_ ) const {
	return ( ( _label == attr_._label ) && ( _data == attr_._data ) );
}

HWidget::HWidget( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: _enabled( false )
	, _focused( false )
	, _drawLabel( true )
	, _labelPosition( LABEL::POSITION::STACKED )
	, _labelDecoration( LABEL::DECORATION::AUTO )
	, _attributeDisabled( _attributeDisabled_ )
	, _attributeEnabled( _attributeEnabled_ )
	, _attributeFocused( _attributeFocused_ )
	, _row( row_ )
	, _column( column_ )
	, _height( height_ )
	, _width( width_ )
	, _rowRaw( 0 )
	, _columnRaw( 0 )
	, _heightRaw( 0 )
	, _widthRaw( 0 )
	, _id()
	, _label( label_ )
	, _varTmpBuffer()
	, _window( parent_ )
	, _labelLength( 0 )
	, _shortcutIndex( 0 )
	, _valid( false )
	, _needRepaint( false )
	, _eventDispatcher() {
	M_PROLOG
	if ( ! HConsole::get_instance().is_enabled() ) {
		M_THROW( "not in curses mode.", errno );
	}
	_shortcutIndex = static_cast<int>( _label.find( '&'_ycp ) );
	if ( _shortcutIndex > -1 ) {
		_label.set_at( _shortcutIndex, 0_ycp );
		_label += label_.substr( _shortcutIndex + 1 );
	} else {
		_shortcutIndex = 0;
	}
	_labelLength = ! label_.is_empty() ? static_cast<int>( _label.get_length() ) : 0;
	if ( _labelLength ) {
		if ( _label[ _labelLength - 1 ] != '\n' ) {
			_labelPosition = LABEL::POSITION::SIDE_BY_SIDE;
		} else {
			_labelPosition = LABEL::POSITION::STACKED;
			_label.set_at( -- _labelLength, 0_ycp );
		}
		if ( _labelDecoration == LABEL::DECORATION::AUTO ) {
			_labelLength += LABEL_DECORATION_SIZE;
		}
	} else {
		_labelPosition = LABEL::POSITION::SIDE_BY_SIDE;
	}
	set_attributes( attr_ );
	return;
	M_EPILOG
}

HWidget::~HWidget( void ) {
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroying widget: " << _label << endl;
#endif /* __DEBUGGER_BABUNI__ */
	_window = nullptr;
	return;
	M_DESTRUCTOR_EPILOG
}

void HWidget::enable( bool enable_ ) {
	M_PROLOG
	_enabled = enable_;
	if ( ! _enabled ) {
		_focused = false;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

int HWidget::process_input( int code_ ) {
	M_PROLOG
	if ( ! _valid ) {
		update();
	}
	return ( do_process_input( code_ ) );
	M_EPILOG
}

int HWidget::do_process_input( int code_ ) {
	M_PROLOG
	if ( ! _focused )
		M_THROW( "input in widget without focus", code_ );
	return code_;
	M_EPILOG
}

bool HWidget::set_focus( int shortCut_ ) {
	M_PROLOG
	bool focusChanged( false );
	if ( _enabled && ( ( shortCut_ <= 0 ) || ( _label[ _shortcutIndex ] == code_point_t( static_cast<yaal::u32_t>( shortCut_ ) ) ) ) ) {
		bool oldFocus( _focused );
		_focused = true;
		if ( ! shortCut_ && _window ) {
			_window->acquire_focus( this );
		}
		if ( oldFocus != _focused ) {
			schedule_repaint();
			focusChanged = true;
		}
	}
	return focusChanged;
	M_EPILOG
}

void HWidget::kill_focus( void ) {
	M_PROLOG
	do_kill_focus();
	return;
	M_EPILOG
}

void HWidget::do_kill_focus( void ) {
	M_PROLOG
	if ( _focused ) {
		_focused = false;
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HWidget::paint( void ) {
	M_PROLOG
	if ( ! _valid )
		update();
	do_paint();
	return;
	M_EPILOG
}

void HWidget::update( void ) {
	M_PROLOG
	do_update();
	_valid = true;
	return;
	M_EPILOG
}

void HWidget::set_data( HInfo const& data_ ) {
	M_PROLOG
	do_set_data( data_ );
	return;
	M_EPILOG
}

void HWidget::do_set_data( HInfo const & ) {
	M_PROLOG
	bool volatile always( true );
	if ( always ) /* We need something that evaluates to `true' but compiler does not know it. */
		M_THROW( "Abstract method called!", errno );
	return;
	M_EPILOG
}

HInfo const& HWidget::get_data( void ) const {
	M_PROLOG
	return ( do_get_data() );
	M_EPILOG
}

HInfo const& HWidget::do_get_data( void ) const {
	M_PROLOG
	static HInfoMultiVal info;
	return info;
	M_EPILOG
}

bool HWidget::is_searchable( void ) {
	M_PROLOG
	return ( false );
	M_EPILOG
}

void HWidget::draw_label( void ) {
	M_PROLOG
	do_draw_label();
	_needRepaint = false;
	return;
	M_EPILOG
}

void HWidget::do_draw_label( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	schedule_repaint();
/* reposition widget accordingly to current parent window size */
	_rowRaw = ( _row >= 0 ) ? _row : cons.get_height() + _row;
	_columnRaw = ( _column >= 0 ) ? _column
		: cons.get_width() + _column;
	_heightRaw = ( _height > 0 ) ? _height
		: cons.get_height() + _height - _rowRaw;
	_widthRaw = ( _width > 0 ) ? _width
		: cons.get_width() + _width - _columnRaw;
/* done */
	if ( ! _drawLabel ) {
		set_attr_data();
		return;
	}
	set_attr_label();
	int col( _columnRaw );
	if ( _labelDecoration == LABEL::DECORATION::AUTO ) {
		cons.mvprintf( _rowRaw, col, " %*c: ", _labelLength - LABEL_DECORATION_SIZE, ' ' );
		++ col;
	}

	cons.mvprintf( _rowRaw, col, _label );
	set_attr_shortcut();
	cons.mvprintf( _rowRaw, col + _shortcutIndex,
				"%C", _label[ _shortcutIndex ] );
	set_attr_data();
	if ( _labelPosition == LABEL::POSITION::SIDE_BY_SIDE ) {
		_columnRaw += _labelLength;
		if ( _width < 0 ) {
			_widthRaw -= _labelLength;
		}
	} else {
		++ _rowRaw;
		if ( _height < 0 ) {
			-- _heightRaw;
		}
	}
	if ( ( _columnRaw + _widthRaw ) >= cons.get_width() ) {
		_widthRaw = cons.get_width() - _columnRaw;
	}
	if ( ( _rowRaw + _heightRaw ) >= cons.get_height() ) {
		_heightRaw = cons.get_height() - _heightRaw;
	}
	return;
	M_EPILOG
}

void HWidget::set_attributes( HWidgetAttributesInterface const& attr_ ) {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

void HWidget::set_attribute_enabled( OAttribute attribute_ ) {
	M_PROLOG
	if ( attribute_ == DEFAULT_ATTRS ) {
		_attributeEnabled = _attributeEnabled_;
	} else {
		_attributeEnabled = attribute_;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::set_attribute_disabled( OAttribute attribute_ ) {
	M_PROLOG
	if ( attribute_ == DEFAULT_ATTRS ) {
		_attributeDisabled = _attributeDisabled_;
	} else {
		_attributeDisabled = attribute_;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::set_attribute_focused( OAttribute attribute_ ) {
	M_PROLOG
	if ( attribute_ == DEFAULT_ATTRS ) {
		_attributeFocused = _attributeFocused_;
	} else {
		_attributeFocused = attribute_;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::set_attributes( OAttribute attributeDisabled_,
		OAttribute attributeEnabled_, OAttribute attributeFocused_ ) {
	M_PROLOG
	set_attribute_enabled( attributeEnabled_ );
	set_attribute_disabled( attributeDisabled_ );
	set_attribute_focused( attributeFocused_ );
	return;
	M_EPILOG
}

void HWidget::move( int row_, int column_, int height_, int width_ ) {
	M_PROLOG
	_row = row_;
	_column = column_;
	_height = height_;
	_width = width_;
	schedule_repaint();
	return;
	M_EPILOG
}

bool HWidget::click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	bool handled( false );
	if ( _enabled ) {
		if ( ! _valid ) {
			update();
		}
		handled = do_click( mouse_ );
	}
	return handled;
	M_EPILOG
}

bool HWidget::do_click( mouse::OMouse& ) {
	M_PROLOG
	bool handled( false );
	if ( ! _focused ) {
		set_focus();
		handled = true;
	}
	return handled;
	M_EPILOG
}

void HWidget::do_update( void ) {
	return;
}

bool HWidget::hit_test( int row_, int column_ ) const {
	M_PROLOG
	bool hit( false );
	if ( _enabled ) {
		hit = do_hit_test( row_, column_ );
	}
	return hit;
	M_EPILOG
}

bool HWidget::do_hit_test( int row_, int column_ ) const {
	M_PROLOG
	bool hit( true );
	if ( ( row_ < _rowRaw ) || ( row_ >= ( _rowRaw + _heightRaw ) ) ) {
		hit = false;
	} if ( ( column_ < _columnRaw )
			|| ( column_ >= ( _columnRaw + _widthRaw ) ) ) {
		hit = false;
	}
	return hit;
	M_EPILOG
}

COLOR::color_t HWidget::attr_label( void ) const {
	M_PROLOG
	return ( _enabled ? ( _focused ? _attributeFocused._label : _attributeEnabled._label ) : _attributeDisabled._label );
	M_EPILOG
}

COLOR::color_t HWidget::attr_shortcut( void ) const {
	M_PROLOG
	/*
	 * *FIXME*
	 *
	 * Shortcut does not have separate attribute,
	 * it is just label attribute reverted with respect to enabled/focused state.
	 * (maybe?) Add true shortcut attribute.
	 */
	return ( ! _enabled ? ( ! _focused ? _attributeFocused._label : _attributeEnabled._label ) : _attributeDisabled._label );
	M_EPILOG
}

COLOR::color_t HWidget::attr_data( void ) const {
	M_PROLOG
	return ( _enabled ? ( _focused ? _attributeFocused._data : _attributeEnabled._data ) : _attributeDisabled._data );
	M_EPILOG
}

void HWidget::set_attr_label( void ) const {
	M_PROLOG
	HConsole::get_instance().set_attr( attr_label() );
	return;
	M_EPILOG
}

void HWidget::set_attr_shortcut( void ) const {
	M_PROLOG
	HConsole::get_instance().set_attr( attr_shortcut() );
	return;
	M_EPILOG
}

void HWidget::set_attr_data( void ) const {
	M_PROLOG
	HConsole::get_instance().set_attr( attr_data() );
	return;
	M_EPILOG
}

void HWidget::set_draw_label( bool drawLabel_ ) {
	M_PROLOG
	_drawLabel = drawLabel_;
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::set_id( hcore::HString const& id_ ) {
	M_PROLOG
	_id = id_;
	return;
	M_EPILOG
}

void HWidget::set_label_position( LABEL::POSITION labelPosition_ ) {
	M_PROLOG
	_labelPosition = labelPosition_;
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::set_label_decoration( LABEL::DECORATION decoration_ ) {
	M_PROLOG
	_labelDecoration = decoration_;
	_labelLength = static_cast<int>( _label.get_size() );
	if ( _labelDecoration == LABEL::DECORATION::AUTO ) {
		_labelLength += LABEL_DECORATION_SIZE;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HWidget::schedule_repaint( void ) {
	M_PROLOG
	_needRepaint = true;
	if ( _window ) {
		_window->schedule_repaint( false );
	}
	return;
	M_EPILOG
}

void HWidget::invalidate( void ) {
	_valid = false;
	return;
}

bool HWidget::need_repaint( void ) const {
	return ( _needRepaint );
}

bool HWidget::has_focus( void ) const {
	return ( _enabled && _focused );
}

yaal::hcore::HString const& HWidget::resource_id( void ) const {
	return ( _id );
}

yaal::hcore::HString const& HWidget::get_label( void ) const {
	return ( _label );
}

int HWidget::get_shortcut( void ) const {
	return ( static_cast<int>( _label[_shortcutIndex].get() ) );
}

HWindow* HWidget::get_window( void ) const {
	return ( _window );
}

void HWidget::register_event_listener( event_listener_t const& listener_ ) {
	M_PROLOG
	_eventDispatcher.connect( listener_ );
	return;
	M_EPILOG
}

void HWidgetAttributesInterface::apply( HWidget& widget_ ) const {
	M_PROLOG
	do_apply( widget_ );
	return;
	M_EPILOG
}

HWidgetAttributes::HWidgetAttributes( void )
	: _id()
	, _idSet( false )
	, _drawLabel( true )
	, _drawLabelSet( false )
	, _labelPosition( HWidget::LABEL::POSITION::STACKED )
	, _labelPositionSet( false )
	, _labelDecoration( HWidget::LABEL::DECORATION::AUTO )
	, _labelDecorationSet( false )
	, _attributeDisabled( _attributeDisabled_ )
	, _attributeDisabledSet( false )
	, _attributeEnabled( _attributeEnabled_ )
	, _attributeEnabledSet( false )
	, _attributeFocused( _attributeFocused_ )
	, _attributeFocusedSet( false ) {
	return;
}

HWidgetAttributes& HWidgetAttributes::id( hcore::HString const& id_ ) {
	_id = id_;
	_idSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::draw_label( bool drawLabel_ ) {
	_drawLabel = drawLabel_;
	_drawLabelSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::label_position( HWidget::LABEL::POSITION labelPosition_ ) {
	_labelPosition = labelPosition_;
	_labelPositionSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::label_decoration( HWidget::LABEL::DECORATION decoration_ ) {
	_labelDecoration = decoration_;
	_labelDecorationSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::attribute_enabled( HWidget::OAttribute const& attributeEnabled_ ) {
	_attributeEnabled = attributeEnabled_;
	_attributeEnabledSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::attribute_disabled( HWidget::OAttribute const& attributeDisabled_ ) {
	_attributeDisabled = attributeDisabled_;
	_attributeDisabledSet = true;
	return ( *this );
}

HWidgetAttributes& HWidgetAttributes::attribute_focused( HWidget::OAttribute const& attributeFocused_ ) {
	_attributeFocused = attributeFocused_;
	_attributeFocusedSet = true;
	return ( *this );
}

void HWidgetAttributes::do_apply( yaal::hconsole::HWidget& widget_ ) const {
	M_PROLOG
	if ( _idSet ) {
		widget_.set_id( _id );
	}
	if ( _drawLabelSet ) {
		widget_.set_draw_label( _drawLabel );
	}
	if ( _labelPositionSet ) {
		widget_.set_label_position( _labelPosition );
	}
	if ( _labelDecorationSet ) {
		widget_.set_label_decoration( _labelDecoration );
	}
	if ( _attributeEnabledSet ) {
		widget_.set_attribute_enabled( _attributeEnabled );
	}
	if ( _attributeDisabledSet ) {
		widget_.set_attribute_disabled( _attributeDisabled );
	}
	if ( _attributeFocusedSet ) {
		widget_.set_attribute_focused( _attributeFocused );
	}
	return;
	M_EPILOG
}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cctype>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlistwidget.hxx"
#include "hcore/memory.hxx"
#include "hcore/hformat.hxx"
#include "hcore/hlog.hxx"
#include "hconsole.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hwindow.hxx"
#include "tools/keycode.hxx"
#include "tools/hexpression.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::hconsole::list_widget_helper;
using namespace yaal::hconsole::mouse;

namespace yaal {

namespace hconsole {

HListWidget::flag_t const HListWidget::FLAG::NONE = HListWidget::flag_t::new_flag();
HListWidget::flag_t const HListWidget::FLAG::CHECKABLE = HListWidget::flag_t::new_flag();
HListWidget::flag_t const HListWidget::FLAG::SORTABLE = HListWidget::flag_t::new_flag();
HListWidget::flag_t const HListWidget::FLAG::EDITABLE = HListWidget::flag_t::new_flag();
HListWidget::flag_t const HListWidget::FLAG::DRAW_HEADER = HListWidget::flag_t::new_flag();
HListWidget::flag_t const HListWidget::FLAG::ALL = ~HListWidget::FLAG::NONE;

namespace {
char const* default_format( TYPE type_ ) {
	char const* df = nullptr;
	switch ( type_ ) {
		case ( TYPE::HSTRING ):       df = "%s";     break;
		case ( TYPE::INT_LONG_LONG ): df = "%lld";   break;
		case ( TYPE::DOUBLE_LONG ):   df = "%.12Lf"; break;
		case ( TYPE::HTIME ):         df = "%s";     break;
		default: {
			M_ASSERT( !"bad type"[0] );
		}
	}
	return ( df );
}
}

HListWidget::HColumnInfo::HColumnInfo( yaal::hcore::HString const& name_,
		int width_, BITS::ALIGN align_, TYPE type_,
		yaal::hcore::HString const& format_, HWidget* widget_ )
	: _descending( false )
	, _widthRaw( 0 )
	, _width( width_ )
	, _align( align_ )
	, _shortcutIndex( 0 )
	, _shortcut( 0 )
	, _type( type_ )
	, _format( ! format_.is_empty() ? format_ : default_format( _type ) )
	, _name( name_ )
	, _widget( widget_ ) {
	M_PROLOG
	int shortcutIndex( static_cast<int>( _name.find( '&'_ycp ) ) );
	if ( shortcutIndex != HString::npos ) {
		_name.erase( shortcutIndex, 1 );
	} else {
		shortcutIndex = 0;
	}
	if ( shortcutIndex < _name.get_length() ) {
		_shortcutIndex = shortcutIndex;
		_shortcut = _name[ shortcutIndex ];
	}
	return;
	M_EPILOG
}

HListWidget::HColumnInfo::~HColumnInfo( void ) {
	M_PROLOG
	_widget = nullptr;
	return;
	M_DESTRUCTOR_EPILOG
}

HListWidget::HListWidget( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_, HWidgetAttributesInterface const& attr_,
		HAbstractListModel::ptr_t const& data_ )
	: HWidget( parent_, row_, column_, height_, width_, label_, attr_ )
	, HSearchableWidget( attr_ )
	, _checkable( false )
	, _sortable( true )
	, _drawHeader( true )
	, _editable( false )
	, _widgetOffset( 0 )
	, _cursorPosition( 0 )
	, _visibleColumn( false )
	, _header()
	, _sortColumn( -1 )
	, _match()
	, _cellEditor()
	, _cursor()
	, _firstVisibleRow()
	, _model( data_ ) {
	M_PROLOG
	attr_.apply( *this );
	_model->set_widget( this );
	schedule_repaint();
	return;
	M_EPILOG
}

HListWidget::~HListWidget( void ) {
	return;
}

void HListWidget::do_paint( void ) {
	M_PROLOG
	int columnOffset( 0 );
	int columns( static_cast<int>( _header.size() ) );
	int hR = _drawHeader ? 1 : 0; /* HR stands for header row */
	int size = static_cast<int>( _model->size() );
	HConsole& cons = HConsole::get_instance();
	int tmp( _widthRaw );
	if ( _focused )
		cons.curs_set( CURSOR::INVISIBLE );
	draw_label(); /* raw* set here */
	_rowRaw += hR;
	if ( ! _visibleColumn ) {
		return;
	}
	if ( _widthRaw != tmp ) {
		recalculate_column_widths();
	}
/*
 * we need to decrement _heightRaw because we have additional row,
 * the list widget header
 */
	if ( _drawHeader ) {
		-- _heightRaw;
	}
	_varTmpBuffer.reserve( _widthRaw );
	int ctr( 0 );
	if ( size > 0 ) {
		iterator_t it = _firstVisibleRow;
		for ( ctr = 0;
					ctr < ( size > _heightRaw ? _heightRaw : size );
					++ ctr, ++ it ) {
			columnOffset = 0;
			for ( int ctrLoc( 0 ); ctrLoc < columns; ++ ctrLoc ) {
				HColumnInfo* columnInfo( _header[ ctrLoc ].get() );
				if ( columnInfo->_widthRaw ) {
					bool checked( get_text_for_cell( it, ctrLoc, columnInfo->_type, &columnInfo->_format ) );
					draw_cell( it, ctr, ctrLoc, columnOffset, columnInfo, checked );
					columnOffset += columnInfo->_widthRaw;
				}
			}
		}
	}
	draw_background( ctr );
	_rowRaw -= hR;
	draw_header( columns );
	_rowRaw += hR;
	cons.set_attr( ! _enabled ?
			( ! _focused ? _attributeFocused._data : _attributeEnabled._data )
			: _attributeDisabled._data );
	if ( size > _heightRaw ) {
		draw_scroll( _columnRaw + columnOffset - 1 );
	}
	_rowRaw -= hR;
	if ( _cellEditor._editing ) {
		_cellEditor._edit->paint();
	}
	return;
	M_EPILOG
}

void HListWidget::draw_header( int columns_ ) {
	M_PROLOG
	int columnOffset( 0 );
	int hR = _drawHeader ? 1 : 0; /* HR stands for header row */
	HConsole& cons( HConsole::get_instance() );
	for ( int colNo( 0 ); colNo < columns_; ++ colNo ) {
		HColumnInfo* columnInfo( _header[ colNo ].get() );
		if ( columnInfo->_widthRaw ) {
			if ( _drawHeader ) {
				_varTmpBuffer = columnInfo->_name;
				set_attr_label();
				_varTmpBuffer = format( "%%-%ds", columnInfo->_widthRaw );
				cons.mvprintf(
					_rowRaw,
					_columnRaw + columnOffset,
					_varTmpBuffer,
					columnInfo->_name.left( columnInfo->_widthRaw )
				);
				set_attr_shortcut();
				cons.mvprintf(
					_rowRaw,
					_columnRaw + columnOffset + columnInfo->_shortcutIndex,
					"%C",
					columnInfo->_shortcut
				);
				if ( _sortColumn == colNo ) {
					cons.mvprintf( _rowRaw,
								_columnRaw + columnOffset
								+ columnInfo->_widthRaw - 2,
								"%c", columnInfo->_descending ? '^' : 'v' );
				}
			}
			columnOffset += columnInfo->_widthRaw;
			if ( colNo < columns_ ) {
				cons.set_attr( _attributeDisabled._data );
				for ( int row = 0; row < ( _heightRaw + hR ); ++ row ) {
					cons.move( _rowRaw + row, _columnRaw + columnOffset - 1 );
					cons.addch( GLYPH::LINE::SINGLE::VERTICAL );
				}
			}
		}
	}
	if ( _labelPosition == LABEL::POSITION::STACKED ) {
		_varTmpBuffer = format( " %d/%d ", _widgetOffset + _cursorPosition + 1, static_cast<int>( _model->size() ) );
		if ( _labelLength < _widthRaw ) {
			int clip = static_cast<int>( ( ( _widthRaw - _labelLength ) < _varTmpBuffer.get_length() ) ? _varTmpBuffer.get_length() - ( _widthRaw - _labelLength ) : 0 );
			cons.mvprintf( _rowRaw - 1, static_cast<int>( _columnRaw + _widthRaw + clip - _varTmpBuffer.get_length() ), _varTmpBuffer.substr( clip ) );
		}
	}
	return;
	M_EPILOG
}

void HListWidget::draw_background( int from_ ) {
	M_PROLOG
	int ctr = from_;
	set_attr_data();
	_varTmpBuffer.reserve( _widthRaw );
	_varTmpBuffer.fillz( '.'_ycp, 0, _widthRaw );
	for ( ; ctr < _heightRaw; ctr ++ ) {
		HConsole::get_instance().mvprintf( _rowRaw + ctr, _columnRaw, _varTmpBuffer );
	}
	return;
	M_EPILOG
}

void HListWidget::draw_scroll( int posX_ ) {
	M_PROLOG
	double scaled = 0;
	int size = static_cast<int>( _model->size() );
	HConsole& cons = HConsole::get_instance();
	if ( _widgetOffset ) {
		cons.move( _rowRaw, posX_ );
		cons.addch( GLYPH::ARROW::UP );
	}
	if ( ( size - _widgetOffset ) > _heightRaw ) {
		cons.move( _rowRaw + _heightRaw - 1, posX_ );
		cons.addch( GLYPH::ARROW::DOWN );
	}
	scaled = _heightRaw - 3;
	scaled *= static_cast<double>(
			_widgetOffset + _cursorPosition );
	scaled /= static_cast<double>( size );
	cons.mvprintf( _rowRaw + static_cast<int>( scaled + 1.5 ), posX_, "#" );
	return;
	M_EPILOG
}

void HListWidget::draw_cell( iterator_t& it_, int row_, int column_, int columnOffset_, HColumnInfo const* columnInfo_, bool checked_ ) {
	M_PROLOG
	HConsole& cons( HConsole::get_instance() );
	int len( static_cast<int>( _varTmpBuffer.get_length() ) );
	switch ( columnInfo_->_align ) {
		case ( BITS::ALIGN::LEFT ): {
			if ( len < columnInfo_->_widthRaw ) {
				_varTmpBuffer.reserve( columnInfo_->_widthRaw );
				_varTmpBuffer.fillz( '_'_ycp, len, columnInfo_->_widthRaw - len );
			} else if ( len > columnInfo_->_widthRaw ) {
				_varTmpBuffer.set_at( columnInfo_->_widthRaw, 0_ycp );
			}
		}
		break;
		case ( BITS::ALIGN::CENTER ): {
			if ( len > columnInfo_->_widthRaw )
				_varTmpBuffer = _varTmpBuffer.right(
						columnInfo_->_widthRaw );
			else if ( len < columnInfo_->_widthRaw ) {
				_varTmpBuffer.insert( 0, ( columnInfo_->_widthRaw - len ) / 2, '_'_ycp );
				len = static_cast<int>( _varTmpBuffer.get_length() );
				_varTmpBuffer.reserve( columnInfo_->_widthRaw );
				_varTmpBuffer.fillz( '_'_ycp, len, columnInfo_->_widthRaw - len );
			}
		}
		break;
		case ( BITS::ALIGN::RIGHT ): {
			if ( len > columnInfo_->_widthRaw )
				_varTmpBuffer.erase( 0, len - columnInfo_->_widthRaw );
			else if ( len < columnInfo_->_widthRaw )
				_varTmpBuffer.insert( 0, ( columnInfo_->_widthRaw - len ) - 1, '_'_ycp );
		}
		break;
		default :
			M_THROW( "unknown align", columnInfo_->_align );
		break;
	}
	if ( row_ == _cursorPosition ) {
		M_ASSERT( it_ == _cursor );
		if ( checked_ ) {
			cons.set_attr(
				COLOR::complementary(
					! _enabled
					? ( ! _focused ? _attributeFocused._label
						: _attributeEnabled._label )
					: _attributeDisabled._label
				)
			);
		} else {
			cons.set_attr(
				COLOR::complementary(
					_enabled ? ( _focused ? _attributeFocused._data
						: _attributeEnabled._data ) : _attributeDisabled._data
				)
			);
		}
	} else {
		if ( checked_ ) {
			cons.set_attr(
				COLOR::complementary(
					_enabled
					? ( _focused ? _attributeFocused._label
						: _attributeEnabled._label )
					: _attributeDisabled._label
				)
			);
		} else {
			set_attr_data();
		}
	}
	cons.mvprintf( _rowRaw + row_, _columnRaw + columnOffset_, _varTmpBuffer );
	if ( _searchActivated ) {
		highlight(
			_rowRaw + row_,
			_columnRaw + columnOffset_, _match._matchNumber,
			( it_ == _match._currentMatch )
			&& ( column_ == _match._columnWithMatch )
		);
	}
	return;
	M_EPILOG
}

namespace {

template<typename tType>
inline void increment( tType& iterator, int count ) {
	for ( int i = 0; i < count; ++ i, ++ iterator ) {
	}
}

template<typename tType>
inline void decrement( tType& iterator, int count ) {
	for ( int i = 0; i < count; ++ i, -- iterator )
		;
}

template<typename tType, typename ttType>
inline void cyclic_increment( ttType& model, tType& iterator, int count ) {
	for ( int i = 0; i < count; ++ i ) {
		++ iterator;
		if ( ! iterator.is_valid() )
			iterator = model->begin();
	}
}

template<typename tType, typename ttType>
inline void cyclic_decrement( ttType& model, tType& iterator, int count ) {
	for ( int i = 0; i < count; ++ i ) {
		-- iterator;
		if ( ! iterator.is_valid() ) {
			iterator = model->last();
		}
	}
}

}

void HListWidget::set_cursor_position( int index_ ) {
	M_PROLOG
	M_ENSURE( ( index_ >= 0 ) && ( index_ < _model->size() ) );
	int currentCursorPosition( get_cursor_position() );
	M_ENSURE( ( currentCursorPosition >= 0 ) && ( currentCursorPosition < _model->size() ) );
	if ( index_ > currentCursorPosition ) {
		for ( int i( 0 ), count( index_ - currentCursorPosition ); i < count; ++ i ) {
			move_cursor_down();
		}
	} else if ( index_ < currentCursorPosition ) {
		for ( int i( 0 ), count( currentCursorPosition - index_ ); i < count; ++ i ) {
			move_cursor_up();
		}
	}
	M_ASSERT( get_cursor_position() == index_ );
	return;
	M_EPILOG
}

int HListWidget::get_cursor_position( void ) const {
	return ( _widgetOffset + _cursorPosition );
}

void HListWidget::handle_key_page_up( void ) {
	M_PROLOG
	if ( ! _cursorPosition ) {
		if ( _widgetOffset ) {
			_widgetOffset -= ( _heightRaw - 1 );
			decrement( _firstVisibleRow, _heightRaw - 1 );
			decrement( _cursor, _heightRaw - 1 );
		} else {
			HConsole::get_instance().bell();
		}
		if ( _widgetOffset < 0 ) {
			_widgetOffset = 0;
			_cursor = _model->begin();
			_firstVisibleRow = _model->begin();
		}
	} else {
		_cursorPosition = 0;
		_cursor = _firstVisibleRow;
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_page_down( void ) {
	M_PROLOG
	int size( static_cast<int>( _model->size() ) );
	if ( size > _heightRaw ) {
		if ( _cursorPosition == ( _heightRaw - 1 ) ) {
			if ( get_cursor_position() < ( size - 1 ) ) {
				_widgetOffset += ( _heightRaw - 1 );
				increment( _firstVisibleRow, _heightRaw - 1 );
				increment( _cursor, _heightRaw - 1 );
				if ( _widgetOffset > ( size - _heightRaw ) ) {
					_widgetOffset = size - _heightRaw;
					_cursor = _firstVisibleRow = _model->last();
					decrement( _firstVisibleRow, _heightRaw - 1 );
				}
			} else {
				HConsole::get_instance().bell();
			}
		} else {
			_cursorPosition = _heightRaw - 1;
			_cursor = _firstVisibleRow;
			increment( _cursor, _heightRaw - 1 );
		}
	} else {
		if ( _cursorPosition != ( size - 1 ) ) {
			_cursorPosition = size - 1;
			_cursor = _model->last();
		} else {
			HConsole::get_instance().bell();
		}
	}
	return;
	M_EPILOG
}

void HListWidget::move_cursor_up( void ) {
	M_PROLOG
	M_ASSERT( get_cursor_position() != 0 );
	if ( _cursorPosition > 0 ) {
		-- _cursorPosition;
		-- _cursor;
	} else if ( _widgetOffset > 0 ) {
		-- _firstVisibleRow;
		-- _cursor;
		-- _widgetOffset;
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_up( void ) {
	M_PROLOG
	if ( get_cursor_position() > 0 ) {
		move_cursor_up();
	} else {
		HConsole::get_instance().bell();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_home( void ) {
	M_PROLOG
	if ( get_cursor_position() > 0 ) {
		reset();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_end( void ) {
	M_PROLOG
	int size( static_cast<int>( _model->size() ) );
	_cursor = _model->last();
	if ( size >= _heightRaw ) {
		_cursorPosition = _heightRaw - 1;
		_widgetOffset = size - _heightRaw;
		_firstVisibleRow = _cursor;
		decrement( _firstVisibleRow, _heightRaw - 1 );
	} else {
		_cursorPosition = size - 1;
		_widgetOffset = 0;
		_firstVisibleRow = _model->begin();
	}
	return;
	M_EPILOG
}

void HListWidget::move_cursor_down( void ) {
	M_PROLOG
	++ _cursorPosition;
	++ _cursor;
	if ( _cursorPosition >= _heightRaw ) {
		_cursorPosition = _heightRaw - 1;
		++ _widgetOffset;
		++ _firstVisibleRow;
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_down( void ) {
	M_PROLOG
	if ( get_cursor_position() < ( _model->size() - 1 ) ) {
		move_cursor_down();
	} else {
		HConsole::get_instance().bell();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_ctrl_n ( void ) {
	M_PROLOG
	if ( _backwards ) {
		go_to_match_previous();
	} else {
		go_to_match();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_ctrl_p( void ) {
	M_PROLOG
	if ( _backwards ) {
		go_to_match();
	} else {
		go_to_match_previous();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_space( void ) {
	M_PROLOG
	if ( _checkable && ! _model->is_empty() ) {
		M_ASSERT( _cursor.is_valid() );
		_cursor->switch_state();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_tab( void ) {
	if ( _cellEditor._editing ) {
		if ( commit_edit() ) {
			++ _cellEditor._currentColumn;
			_cellEditor._currentColumn %= static_cast<int>( _header.get_size() );
			show_edit();
		}
	} else {
		_focused = false;   /* very  */
		schedule_repaint(); /* magic */
	}
	return;
}

void HListWidget::handle_key_insert( void ) {
	M_PROLOG
	M_ASSERT( _editable );
	int size( static_cast<int>( _header.get_size() ) );
	HAsIsValueListModel<>* model( dynamic_cast<HAsIsValueListModel<>*>( _model.get() ) );
	M_ASSERT( model );
	model->get_data()->emplace_back( size );
	handle_key_end();
	_cellEditor._currentColumn = 0;
	return;
	M_EPILOG
}

void HListWidget::handle_key_delete( void ) {
	M_PROLOG
	M_ASSERT( _editable );
	if ( ! _model->is_empty() ) {
		remove_current_row();
	}
	return;
	M_EPILOG
}

void HListWidget::handle_key_edit( void ) {
	M_PROLOG
	M_ASSERT( _editable );
	if ( ! _model->is_empty() && ! _cellEditor._editing ) {
		show_edit();
		_cellEditor._editing = true;
	}
	return;
	M_EPILOG
}

void HListWidget::show_edit( void ) {
	M_PROLOG
	if ( ! _cellEditor._edit ) {
		_cellEditor._edit = make_pointer<HEditWidget>( nullptr, 0, 0, 0, 0, "", HEditWidgetAttributes().draw_label( false ) );
		_cellEditor._edit->enable( true );
		_cellEditor._edit->set_focus();
	}
	_cellEditor._edit->move(
			_rowRaw + _cursorPosition + ( _drawHeader ? 1 : 0 ),
			_columnRaw + column_offset( _cellEditor._currentColumn ),
			1,
			_header[_cellEditor._currentColumn]->_widthRaw - 1 );
	HAsIsValueListModel<>::HModelIterator* it( dynamic_cast<HAsIsValueListModel<>::HModelIterator*>( _cursor.raw().raw() ) );
	M_ASSERT( it );
	_cellEditor._edit->set_data( (*(it->raw()))[_cellEditor._currentColumn] );
	_cellEditor._edit->schedule_repaint();
	schedule_repaint();
	return;
	M_EPILOG
}

bool HListWidget::commit_edit( void ) {
	M_PROLOG
	bool ok( true );
	HAsIsValueListModel<>::HModelIterator* it( dynamic_cast<HAsIsValueListModel<>::HModelIterator*>( _cursor.raw().raw() ) );
	M_ASSERT( it );
	switch ( _header[_cellEditor._currentColumn]->_type ) {
		case ( TYPE::HSTRING ): {
			(*(it->raw()))[_cellEditor._currentColumn].set_string( _cellEditor._edit->get_data().get_string() );
		} break;
		case ( TYPE::INT_LONG_LONG ): {
			try {
				(*(it->raw()))[_cellEditor._currentColumn].set_integer( _cellEditor._edit->get_data().get_integer() );
			} catch ( HLexicalCastException const& e ) {
				ok = false;
				if ( _window ) {
					_window->status_bar()->message( COLOR::FG_RED, "%s", e.what() );
				}
			}
		} break;
		case ( TYPE::DOUBLE_LONG ): {
			try {
				(*(it->raw()))[_cellEditor._currentColumn].set_real( _cellEditor._edit->get_data().get_real() );
			} catch ( HExpressionException const& e ) {
				ok = false;
				if ( _window ) {
					_window->status_bar()->message( COLOR::FG_RED, "%s", e.what() );
				}
			} catch ( HLexicalCastException const& e ) {
				ok = false;
				if ( _window ) {
					_window->status_bar()->message( COLOR::FG_RED, "%s", e.what() );
				}
			}
		} break;
		case ( TYPE::HTIME ): {
			(*(it->raw()))[_cellEditor._currentColumn].set_time( _cellEditor._edit->get_data().get_time() );
		} break;
		default: {
			M_ASSERT( !"bad column type"[0] );
		}
	}
	return ( ok );
	M_EPILOG
}

void HListWidget::selection_change( void ) {
	M_PROLOG
	update_children();
	_eventDispatcher( static_cast<HWidgetEvent const&>( HWidgetEvent( this ) ) );
	return;
	M_EPILOG
}

int HListWidget::do_process_input( int code_ ) {
	M_PROLOG
	if ( _cellEditor._editing ) {
		code_ = process_input_edit( code_ );
	}
	if ( code_ || ! _cellEditor._editing ) {
		code_ = process_input_view( code_ );
	}
	return ( code_ );
	M_EPILOG
}

int HListWidget::process_input_edit( int code_ ) {
	M_PROLOG
	M_ASSERT( _editable && _cellEditor._editing );
	code_ = _cellEditor._edit->process_input( code_ );
	int code( 0 );
	switch ( code_ ) {
		case ( '\t' ): {
			handle_key_tab();
		} break;
		case ( '\r' ): {
			if ( commit_edit() ) {
				_cellEditor._editing = false;
				schedule_repaint();
			}
		} break;
		case ( KEY_CODE::ESCAPE ): {
			_cellEditor._editing = false;
			schedule_repaint();
		} break;
		default: {
			code = code_;
		}
	}
	if ( ! code ) {
		_cellEditor._edit->schedule_repaint();
		schedule_repaint();
	}
	return ( code );
	M_EPILOG
}

int HListWidget::process_input_view( int code_ ) {
	M_PROLOG
	int errorCode( 0 );
	int origCursorPosition( get_cursor_position() );
	code_ = HWidget::do_process_input( code_ );
	_varTmpBuffer.clear();
	switch ( code_ ) {
		case ( KEY_CODE::PAGE_UP ):   handle_key_page_up();   break;
		case ( KEY_CODE::PAGE_DOWN ): handle_key_page_down(); break;
		case ( KEY_CODE::UP ):        handle_key_up();        break;
		case ( KEY_CODE::HOME ):      handle_key_home();      break;
		case ( KEY_CODE::END ):       handle_key_end();       break;
		case ( KEY_CODE::DOWN ):      handle_key_down();      break;
		case ( KEY<'n'>::ctrl ):      handle_key_ctrl_n();    break;
		case ( KEY<'p'>::ctrl ):      handle_key_ctrl_p();    break;
		case ( ' ' ):                 handle_key_space();     break;
		case ( KEY_CODE::INSERT ): {
			if ( _editable ) {
				handle_key_insert();
			}
		} break;
		case ( KEY_CODE::DELETE ): {
			if ( _editable ) {
				handle_key_delete();
			}
		} break;
		case ( KEY_CODE::F2 ): {
			if ( _editable ) {
				handle_key_edit();
			}
		} break;
		case ( '\t' ):                handle_key_tab(); /* fall through */
/* there is no break in previous `case():', because this list must give up
 * its focus and be refreshed and parent window must give focus
 * to another widget */
		default : {
			int columns( static_cast<int>( _header.size() ) );
			int selectedColumn( 0 );
			for ( ; selectedColumn < columns; ++ selectedColumn ) {
				if ( tolower( code_ ) == tolower( static_cast<int>( _header[ selectedColumn ]->_shortcut.get() ) ) ) {
					break;
				}
			}
			if ( selectedColumn < columns ) {
				errorCode = 0;
				sort_by_column( selectedColumn,
						code_ == tolower( code_ ) ? OSortHelper::ASCENDING : OSortHelper::DESCENDING );
			} else {
				errorCode = code_;
			}
		}
		break;
	}
	code_ = errorCode;
	if ( get_cursor_position() != origCursorPosition ) {
		selection_change();
	}
	if ( ! errorCode ) {
		schedule_repaint();
		if ( _window ) {
			_window->status_bar()->message( COLOR::FG_LIGHTGRAY, _varTmpBuffer );
		}
	}
	return ( code_ );
	M_EPILOG
}

void HListWidget::add_column( int columnPosition_, HColumnInfo::ptr_t column_ ) {
	M_PROLOG
	int size( static_cast<int>( _model->size() ) );
	if ( size ) {
		M_THROW( "cannot add new column when list not empty", size );
	}
	if ( column_->_width != 0 ) {
		_visibleColumn = true;
	}
	if ( ! _header.is_empty() && ( columnPosition_ >= 0 ) ) {
		_header.insert( _header.begin() + columnPosition_, yaal::move( column_ ) );
	} else {
		_header.push_back( yaal::move( column_ ) );
	}
	if ( _visibleColumn ) {
		recalculate_column_widths();
	}
	return;
	M_EPILOG
}

int HListWidget::column_offset( int column_ ) const {
	M_PROLOG
	int offset( 0 );
	for ( int i( 0 ); i < column_; ++ i ) {
		offset += _header[ i ]->_widthRaw;
	}
	return ( offset );
	M_EPILOG
}

void HListWidget::recalculate_column_widths( void ) {
	M_PROLOG
	int allColumnsWidth( 0 );
	int columns( static_cast<int>( _header.size() ) );
	int percentages( 0 );
	int absolutes( 0 );
	for ( int i( 0 ); i < columns; ++ i ) {
		HColumnInfo* ci( _header[i].get() );
		if ( ci->_width > 0 ) {
			percentages += ci->_width;
		} else if ( ci->_width < 0 ) {
			absolutes += ( 1 - ci->_width );
		}
	}
	int last( -1 );
	for ( int i( 0 ); i < columns; ++ i ) {
		HColumnInfo* ci( _header[i].get() );
		if ( ci->_width > 0 ) {
			M_ASSERT( percentages > 0 );
			ci->_widthRaw = ( ci->_width * ( _widthRaw - absolutes ) ) / percentages;
			last = i;
		} else if ( ci->_width < 0 ) {
			ci->_widthRaw = 1 - ci->_width;
		}
		allColumnsWidth += ci->_widthRaw;
	}
	/* last column with non zero width should fill space */
	if ( last >= 0 ) {
		allColumnsWidth -= _header[ last ]->_widthRaw;
		_header[ last ]->_widthRaw = ( _widthRaw - allColumnsWidth );
	}
	return;
	M_EPILOG
}

void HListWidget::sort_by_column( int column_, OSortHelper::sort_order_t order_ ) {
	M_PROLOG
	if ( ! _sortable )
		return;
	_sortColumn = column_;
	_header[ column_ ]->_descending = order_ == OSortHelper::DESCENDING;
	long int size = _model->size();
	if ( _window && ( size > 128 ) ) {
		_window->status_bar()->init_progress(
				static_cast<double>( size )
				* static_cast<double>( size ) / 2.,
				" Sorting ..." );
	}
	list_widget_helper::OSortHelper helper =
		{ column_, order_, _header[ _sortColumn ]->_type,
		0, _model->size(), _window };
	_model->sort( helper );
	_widgetOffset = _cursorPosition = 0;
	return;
	M_EPILOG
}

bool HListWidget::do_click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	int columns( static_cast<int>( _header.size() ) );
	int origCursorPosition( get_cursor_position() );
	bool handled( HWidget::do_click( mouse_ ) );
	if ( ! handled ) {
		int row( ( mouse_._row - _rowRaw ) - ( _drawHeader ? 1 : 0 ) );
		if ( row < 0 ) /* header clicked */ {
			int column( mouse_._column + _columnRaw - 1 );
			int width( 0 );
			for ( int ctr( 0 ); ctr < columns; ++ ctr ) {
				HColumnInfo* columnInfo( _header [ ctr ].get() );
				width += columnInfo->_widthRaw;
				if ( column <= width ) {
					sort_by_column( ctr,
							columnInfo->_descending ? OSortHelper::ASCENDING : OSortHelper::DESCENDING );
					schedule_repaint();
					handled = true;
					break;
				}
			}
		} else if ( row < _model->size() ) {
			if ( mouse_._buttons & MOUSE_BITS::BUTTON::WHEEL_UP ) {
				scroll_up();
			} else if ( mouse_._buttons & MOUSE_BITS::BUTTON::WHEEL_DOWN ) {
				scroll_down();
			} else if ( row == _cursorPosition ) {
				return ( false );
			} else {
				if ( row > _cursorPosition ) {
					for ( int i( _cursorPosition ); i < row; ++ i ) {
						++ _cursor;
					}
				} else {
					for ( int i( _cursorPosition ); i > row; -- i ) {
						-- _cursor;
					}
				}
				_cursorPosition = row;
			}
			schedule_repaint();
			handled = true;
		}
	}
	if ( get_cursor_position() != origCursorPosition ) {
		selection_change();
	}
	return ( handled );
	M_EPILOG
}

void HListWidget::scroll_up( void ) {
	M_PROLOG
	if ( _widgetOffset > 0 ) {
		-- _widgetOffset;
		-- _firstVisibleRow;
		if ( _cursorPosition < ( _heightRaw - 1 ) ) {
			++ _cursorPosition;
		} else {
			-- _cursor;
		}
	}
	return;
	M_EPILOG
}

void HListWidget::scroll_down( void ) {
	M_PROLOG
	if ( ( _widgetOffset + _heightRaw ) < _model->size() ) {
		++ _widgetOffset;
		++ _firstVisibleRow;
		if ( _cursorPosition > 0 ) {
			-- _cursorPosition;
		} else {
			++ _cursor;
		}
	}
	return;
	M_EPILOG
}

bool HListWidget::is_searchable( void ) {
	return ( _searchable );
}

void HListWidget::reset( void ) {
	M_PROLOG
	_firstVisibleRow = _cursor = _model->begin();
	_widgetOffset = _cursorPosition = 0;
	selection_change();
	return;
	M_EPILOG
}

void HListWidget::go_to_match( void ) {
	M_PROLOG
	int moveFirstRow = 0;
	int size( static_cast<int>( _model->size() ) );
	int count( size + 1 );
	int columns( static_cast<int>( _header.size() ) );
	int widgetOffsetOrig( _widgetOffset );
	int cursorPositionOrig( _cursorPosition );
	int highlightStart( HPattern::NO_MATCH );
	char const* outcome( nullptr );
	iterator_t cursorOrig = _cursor;
	iterator_t firstVisibleRowOrig = _firstVisibleRow;
	if ( ! _searchActivated ) {
		return;
	}
	if ( _match._currentMatch != _cursor ) {
		_match._matchNumber = -1;
	}
	_match._currentMatch = _cursor;
	int columnWithMatch( 0 );
	int matchNumber( 0 );
	while ( count -- ) {
		for ( columnWithMatch = _match._columnWithMatch; columnWithMatch < columns; ++ columnWithMatch ) {
			get_text_for_cell( _cursor, columnWithMatch, TYPE::HSTRING );
			highlightStart = HPattern::NO_MATCH;
			matchNumber = 0;
			for ( HPattern::HMatchIterator it = _pattern.find( _varTmpBuffer ),
					end = _pattern.end(); it != end; ++ it, ++ matchNumber ) {
				if ( matchNumber > _match._matchNumber ) {
					highlightStart = it->start();
					break;
				}
			}
			if ( highlightStart != HPattern::NO_MATCH ) {
				break;
			}
			_match._matchNumber = -1;
		}
		if ( highlightStart != HPattern::NO_MATCH ) {
			break;
		}
		_match._columnWithMatch = 0;
/* this part is from process_input, but slightly modified */
		if ( get_cursor_position() < ( size - 1 ) ) {
			++ _cursorPosition;
			if ( _cursorPosition >= _heightRaw ) {
				_cursorPosition = _heightRaw - 1;
				++ _widgetOffset;
				++ moveFirstRow;
			}
			++ _cursor;
		} else {
			_cursor = _firstVisibleRow = _model->begin();
			_widgetOffset = _cursorPosition = 0;
			moveFirstRow = 0;
			outcome = _( "search hit BOTTOM, continuing at TOP" );
		}
/* end od it */
	}
	if ( highlightStart != HPattern::NO_MATCH ) {
		if ( moveFirstRow ) {
			increment( _firstVisibleRow, moveFirstRow );
		}
		_match._columnWithMatch = columnWithMatch;
		_match._matchNumber = matchNumber;
		_match._currentMatch = _cursor;
		_varTmpBuffer.clear();
	} else {
		_cursor = cursorOrig;
		_firstVisibleRow = firstVisibleRowOrig;
		_cursorPosition = cursorPositionOrig;
		_widgetOffset = widgetOffsetOrig;
		_match._matchNumber = -1;
		_match._columnWithMatch = 0;
		_varTmpBuffer = HString( _( "pattern not found: " ) ) + _pattern.error();
	}
	if ( outcome ) {
		_varTmpBuffer = outcome;
	}
	return;
	M_EPILOG
}

void HListWidget::go_to_match_previous( void ) {
	M_PROLOG
	int ctr = 0, ctrLoc = 0, moveFirstRow = 0;
	int size = static_cast<int>( _model->size() );
	int count = size + 1;
	int columns = static_cast<int>( _header.size() );
	int widgetOffsetOrig( _widgetOffset );
	int cursorPositionOrig( _cursorPosition );
	int highlightStart( HPattern::NO_MATCH );
	char const* outcome( nullptr );
	iterator_t cursorOrig = _cursor;
	iterator_t firstVisibleRowOrig = _firstVisibleRow;
	if ( ! _searchActivated ) {
		return;
	}
	if ( _match._currentMatch != _cursor ) {
		_match._matchNumber = -1;
	}
	_match._currentMatch = _cursor;
	while ( count -- ) {
		for ( ctr = _match._columnWithMatch; ctr >= 0; ctr -- ) {
			get_text_for_cell( _cursor, ctr, TYPE::HSTRING );
			highlightStart = HPattern::NO_MATCH;
			ctrLoc = 0;
			if ( _match._matchNumber < 0 )
				_match._matchNumber = static_cast<int>( distance( _pattern.find( _varTmpBuffer ), _pattern.end() ) );
			for ( HPattern::HMatchIterator it = _pattern.find( _varTmpBuffer ),
					end = _pattern.end(); it != end; ++ it, ++ ctrLoc ) {
				if ( ctrLoc == ( _match._matchNumber - 1 ) ) {
					highlightStart = it->start();
					break;
				}
				if ( ctrLoc >= _match._matchNumber ) {
					highlightStart = HPattern::NO_MATCH;
					break;
				}
			}
			if ( highlightStart != HPattern::NO_MATCH ) {
				break;
			}
			_match._matchNumber = -1;
		}
		if ( highlightStart != HPattern::NO_MATCH ) {
			break;
		}
		_match._columnWithMatch = columns - 1;
/* this part is from process_input, but slightly modified */
		if ( get_cursor_position() > 0 ) {
			if ( _cursorPosition > 0 ) {
				-- _cursorPosition;
			} else if ( _widgetOffset > 0 ) {
				-- _widgetOffset;
				++ moveFirstRow;
			}
			cyclic_decrement( _model, _cursor, 1 );
		} else {
			if ( size >= _heightRaw ) {
				_cursor = _model->begin();
				cyclic_decrement( _model, _cursor, 1 );
				_firstVisibleRow = _cursor;
				cyclic_decrement( _model, _firstVisibleRow, _heightRaw - 1 );
				_cursorPosition = _heightRaw - 1;
				_widgetOffset = size - _heightRaw;
			} else {
				cyclic_decrement( _model, _cursor, 1 );
				_cursorPosition = size - 1;
			}
			moveFirstRow = 0;
			outcome = _( "search hit TOP, continuing at BOTTOM" );
		}
/* end od it */
	}
	if ( highlightStart != HPattern::NO_MATCH ) {
		if ( moveFirstRow )
			cyclic_decrement( _model, _firstVisibleRow, moveFirstRow );
		_match._columnWithMatch = ctr;
		_match._matchNumber = ctrLoc;
		_match._currentMatch = _cursor;
		_varTmpBuffer.clear();
	} else {
		_cursor = cursorOrig;
		_firstVisibleRow = firstVisibleRowOrig;
		_cursorPosition = cursorPositionOrig;
		_widgetOffset = widgetOffsetOrig;
		_match._matchNumber = -1;
		_match._columnWithMatch = 0;
		_varTmpBuffer = _( "pattern not found" );
	}
	if ( outcome ) {
		_varTmpBuffer = outcome;
	}
	return;
	M_EPILOG
}

void HListWidget::set_flags( flag_t flags_, flag_t mask_ ) {
	M_PROLOG
	if ( !!( mask_ & FLAG::SORTABLE ) )
		_sortable = !!( flags_ & FLAG::SORTABLE ) ? true : false;
	if ( !!( mask_ & FLAG::CHECKABLE ) )
		_checkable = !!( flags_ & FLAG::CHECKABLE ) ? true : false;
	if ( !!( mask_ & FLAG::EDITABLE ) ) {
		_editable = !!( flags_ & FLAG::EDITABLE ) ? true : false;
		if ( _editable ) {
			M_ENSURE( dynamic_cast<HAsIsValueListModel<>*>( _model.get() ) );
		}
	}
	if ( !!( mask_ & FLAG::DRAW_HEADER ) )
		_drawHeader = !!( flags_ & FLAG::DRAW_HEADER ) ? true : false;
	return;
	M_EPILOG
}

bool HListWidget::get_text_for_cell( iterator_t& it_, int column_, TYPE type_, hcore::HFormat* format_ ) {
	M_PROLOG
	M_ASSERT( it_.is_valid() );
	HAbstractRow& item = *it_;
	switch ( type_ ) {
		case ( TYPE::INT_LONG_LONG ): {
			try {
				int long long v( item[ column_ ].get_integer() );
				_varTmpBuffer.assign( format_ ? ( *format_ % v ).string() : v );
			} catch ( HLexicalCastException const& e ) {
				_varTmpBuffer = e.what();
			}
		} break;
		case ( TYPE::DOUBLE_LONG ): {
			try {
				double long v( item[ column_ ].get_real() );
				_varTmpBuffer.assign( format_ ? ( *format_ % v ).string() : v );
			} catch ( HExpressionException const& e ) {
				_varTmpBuffer.assign( "#err# " ).append( e.what() );
			} catch ( HLexicalCastException const& e ) {
				_varTmpBuffer.assign( "#err# " ).append( e.what() );
			}
		} break;
		case ( TYPE::HSTRING ): {
			HString const& v( item[ column_ ].get_string() );
			_varTmpBuffer.assign( format_ ? ( *format_ % v ).string() : v );
		} break;
		case ( TYPE::HTIME ): {
			HString const& v( item[ column_ ].get_time() );
			_varTmpBuffer.assign( format_ ? ( *format_ % v ).string() : v );
		} break;
		default :
			M_THROW( "unknown type", static_cast<u64_t>( type_ ) );
		break;
	}
	return ( item.get_checked() );
	M_EPILOG
}

list_widget_helper::HAbstractListModel::ptr_t& HListWidget::get_model( void ) {
	return ( _model );
}

int long HListWidget::get_row_count( void ) {
	return ( _model->size() );
}

list_widget_helper::HAbstractRow& HListWidget::get_current_row( void ) {
	M_PROLOG
	return ( *_cursor );
	M_EPILOG
}

void HListWidget::remove_current_row( void ) {
	M_PROLOG
	M_ASSERT( ! _model->is_empty() );
	M_ASSERT( _cursor != _model->end() );
	iterator_t it( _cursor );
	if ( _cursor == _firstVisibleRow ) {
		if ( ( _widgetOffset > 0 ) && ( _heightRaw >= ( _model->size() - _widgetOffset ) ) ) {
			-- _firstVisibleRow;
			-- _widgetOffset;
			-- _cursor;
		} else {
			++ _firstVisibleRow;
			++ _cursor;
		}
	} else {
		if ( get_cursor_position() == ( _model->size() - 1 ) ) {
			if ( _widgetOffset > 0 ) {
				-- _firstVisibleRow;
				-- _widgetOffset;
			} else {
				-- _cursorPosition;
			}
			-- _cursor;
		} else {
			if ( ( _widgetOffset > 0 ) && ( _heightRaw >= ( _model->size() - _widgetOffset ) ) ) {
				-- _firstVisibleRow;
				-- _widgetOffset;
				++ _cursorPosition;
			}
			++ _cursor;
		}
	}
	_model->erase( it );
	schedule_repaint();
	selection_change();
	return;
	M_EPILOG
}

void HListWidget::do_update( void ) {
	M_PROLOG
	_cellEditor._currentColumn = 0;
	if ( _model->size() ) {
		_widgetOffset = _cursorPosition = 0;
		_firstVisibleRow = _model->begin();
		_cursor = _model->begin();
		selection_change();
	}
	return;
	M_EPILOG
}

void HListWidget::update_children( void ) {
	M_PROLOG
	int columns( static_cast<int>( _header.size() ) );
	if ( ! _model->is_empty() ) {
		for ( int i( 0 ); i < columns; ++ i ) {
			HColumnInfo* columnInfo( _header[ i ].get() );
			if ( columnInfo->_widget ) {
				(*_cursor)[ i ].set_child_widget_data( columnInfo->_widget );
			}
		}
	}
	return;
	M_EPILOG
}

TYPE HListWidget::get_column_type( int column_ ) {
	M_PROLOG
	return ( _header[ column_ ]->_type );
	M_EPILOG
}

namespace list_widget_helper {

void OSortHelper::progress( void ) {
	M_PROLOG
	++ _comparedItems;
	if ( _window && ( _size > 1024 ) && ! ( _comparedItems % 1024 ) ) {
		_window->status_bar()->update_progress( static_cast<double>( _comparedItems ) );
	}
	return;
	M_EPILOG
}

template<>
bool compare_cells( HInfo const& left_, HInfo const& right_, OSortHelper& sortHelper_ ) {
	M_PROLOG
	if ( sortHelper_._window ) {
		sortHelper_.progress();
	}
	bool lower = false;
	switch ( sortHelper_._type ) {
		case ( TYPE::INT_LONG_LONG ):
			lower = left_.get_integer() < right_.get_integer();
		break;
		case ( TYPE::DOUBLE_LONG ):
			lower = left_.get_real() < right_.get_real();
		break;
		case ( TYPE::HSTRING ):
			lower = stricasecmp( left_.get_string(), right_.get_string() ) < 0;
		break;
		case ( TYPE::HTIME ):
			lower = left_.get_time().raw() < right_.get_time().raw();
		break;
		default:
			break;
	}
	return ( lower );
	M_EPILOG
}

HAbstractListModel::~HAbstractListModel( void ) {
	return;
}

HAbstractListModel::HAbstractListModel( void )
	: _widget( nullptr ) {
	return;
}

void HAbstractListModel::set_widget( HListWidget* widget_ ) {
	_widget = widget_;
	return;
}

template<>
int long long HCell<yaal::hcore::HList<HInfoItem>::iterator>::get_integer( void ) const {
	M_PROLOG
	return ( (*_data)[ _column ].get_integer() );
	M_EPILOG
}

template<>
double long HCell<yaal::hcore::HList<HInfoItem>::iterator>::get_real( void ) const {
	M_PROLOG
	return ( (*_data)[ _column ].get_real() );
	M_EPILOG
}

template<>
yaal::hcore::HString HCell<yaal::hcore::HList<HInfoItem>::iterator>::get_string( void ) const {
	M_PROLOG
	return ( (*_data)[ _column ].get_string() );
	M_EPILOG
}

template<>
yaal::hcore::HString HCell<yaal::hcore::HList<HInfoItem>::iterator>::get_time( void ) const {
	M_PROLOG
	return ( ( (*_data)[ _column ].get_time() ).string() );
	M_EPILOG
}

HAbstractListModel::HModelIteratorWrapper::HModelIteratorWrapper( void )
	: _iteratorPtr() {
	return;
}

HAbstractListModel::HModelIteratorWrapper::HModelIteratorWrapper( iterator_ptr_t const& it_ )
	: _iteratorPtr( it_ ) {
	return;
}

bool HAbstractListModel::HModelIteratorWrapper::is_valid( void ) const {
	return ( !! _iteratorPtr ? _iteratorPtr->is_valid() : false );
}

bool HAbstractListModel::HModelIteratorWrapper::operator==( HAbstractListModel::HModelIteratorWrapper const& it_ ) {
	M_PROLOG
	return ( _iteratorPtr.raw() ? _iteratorPtr->is_equal( *it_._iteratorPtr ) : it_._iteratorPtr.raw() == nullptr );
	M_EPILOG
}

bool HAbstractListModel::HModelIteratorWrapper::operator!=( HAbstractListModel::HModelIteratorWrapper const& it_ ) {
	M_PROLOG
	return ( _iteratorPtr.raw() ? _iteratorPtr->is_not_equal( *it_._iteratorPtr ) : it_._iteratorPtr.raw() != nullptr );
	M_EPILOG
}

HAbstractListModel::HModelIteratorWrapper& HAbstractListModel::HModelIteratorWrapper::operator++( void ) {
	_iteratorPtr->next();
	return ( *this );
}

HAbstractListModel::HModelIteratorWrapper& HAbstractListModel::HModelIteratorWrapper::operator--( void ) {
	_iteratorPtr->previous();
	return ( *this );
}

HAbstractRow& HAbstractListModel::HModelIteratorWrapper::operator*( void ) {
	return ( _iteratorPtr->dereference() );
}

HAbstractRow* HAbstractListModel::HModelIteratorWrapper::operator->( void ) {
	return ( _iteratorPtr->call() );
}

HAbstractListModel::HModelIteratorWrapper::HModelIteratorWrapper( HAbstractListModel::HModelIteratorWrapper const& it_ )
	: _iteratorPtr( it_._iteratorPtr.raw() ? it_._iteratorPtr->clone() : iterator_ptr_t() ) {
	return;
}

HAbstractListModel::HModelIteratorWrapper& HAbstractListModel::HModelIteratorWrapper::operator = ( HAbstractListModel::HModelIteratorWrapper const& it_ ) {
	M_PROLOG
	if ( &it_ != this ) {
		if ( it_.is_valid() ) {
			_iteratorPtr = it_._iteratorPtr->clone();
		} else {
			_iteratorPtr.reset();
		}
	}
	return ( *this );
	M_EPILOG
}

HAbstractListModel::iterator_ptr_t& HAbstractListModel::HModelIteratorWrapper::raw( void ) {
	return ( _iteratorPtr );
}

HAbstractListModel::HAbstractModelIterator::HAbstractModelIterator( void ) {
	return;
}

HAbstractListModel::HAbstractModelIterator::~HAbstractModelIterator( void ) {
	return;
}

HAbstractRow::~HAbstractRow( void ) {
	return;
}

template<>
void HRow<yaal::hcore::HList<HInfoItem>::iterator>::switch_state( void ) {
	(*_iterator)->_checked = ! (*_iterator)->_checked;
	return;
}

template<>
bool HRow<yaal::hcore::HList<HInfoItem>::iterator>::get_checked( void ) {
	return ( (*_iterator)->_checked );
}

template<>
int long HRow<yaal::hcore::HList<HInfoItem>::iterator>::get_id( void ) {
	return ( (*_iterator)->_id );
}

template<>
HRow<yaal::hcore::HList<HInfoItem>::iterator>::HRow( iterator_t* it_ )
	: _iterator( it_ ), _cells( it_ ? (*it_)->get_value_count() : 0 ) {
	M_PROLOG
	int long cellCount = it_ ? (*it_)->get_value_count() : 0;
	for ( int i = 0; i < cellCount; ++ i )
		_cells[ i ] = make_pointer<HCell<yaal::hcore::HList<HInfoItem>::iterator> >( ref( *_iterator ), i );
	return;
	M_EPILOG
}

HAbstractCell::~HAbstractCell( void ) {
	return;
}

template<>
void HCell<yaal::hcore::HList<HInfoItem>::iterator>::set_child_widget_data( HWidget* widget_ ) {
	M_PROLOG
	widget_->set_data( (*_data)[ _column ] );
	widget_->paint();
	return;
	M_EPILOG
}

template<>
bool CompareListWidgetItems<HInfoItem>::operator() ( HInfoItem const& left_,
		HInfoItem const& right_ ) const {
	M_PROLOG
	HInfoItem const& left = _sortHelper._order == OSortHelper::ASCENDING ? left_ : right_;
	HInfoItem const& right = _sortHelper._order == OSortHelper::ASCENDING ? right_ : left_;
	return ( list_widget_helper::compare_cells( left[ _sortHelper._sortColumn ],
				right[ _sortHelper._sortColumn ], _sortHelper ) );
	M_EPILOG
}

}

HListWidgetAttributes::HListWidgetAttributes( void )
	: HSearchableWidgetAttributes()
	, _checkable( false )
	, _checkableSet( false )
	, _sortable( false )
	, _sortableSet( false )
	, _drawHeader( false )
	, _drawHeaderSet( false )
	, _editable( false )
	, _editableSet( false ) {
}

void HListWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HListWidget* widget( dynamic_cast<HListWidget*>( &widget_ ) );
	if ( widget ) {
		HListWidget::flag_t toSet( HListWidget::FLAG::NONE );
		HListWidget::flag_t flag( HListWidget::FLAG::NONE );
		if ( _checkableSet ) {
			toSet |= HListWidget::FLAG::CHECKABLE;
			if ( _checkable ) {
				flag |= HListWidget::FLAG::CHECKABLE;
			}
		}
		if ( _editableSet ) {
			toSet |= HListWidget::FLAG::EDITABLE;
			if ( _editable ) {
				flag |= HListWidget::FLAG::EDITABLE;
			}
		}
		if ( _sortableSet ) {
			toSet |= HListWidget::FLAG::SORTABLE;
			if ( _sortable ) {
				flag |= HListWidget::FLAG::SORTABLE;
			}
		}
		if ( _drawHeaderSet ) {
			toSet |= HListWidget::FLAG::DRAW_HEADER;
			if ( _drawHeader ) {
				flag |= HListWidget::FLAG::DRAW_HEADER;
			}
		}
		if ( toSet != HListWidget::FLAG::NONE ) {
			widget->set_flags( flag, toSet );
		}
	}
	HSearchableWidgetAttributes::do_apply( widget_ );
	return;
	M_EPILOG
}

HListWidgetAttributes& HListWidgetAttributes::editable( bool editable_ ) {
	M_PROLOG
	_editable = editable_;
	_editableSet = true;
	return ( *this );
	M_EPILOG
}

HListWidgetAttributes& HListWidgetAttributes::sortable( bool sortable_ ) {
	M_PROLOG
	_sortable = sortable_;
	_sortableSet = true;
	return ( *this );
	M_EPILOG
}

HListWidgetAttributes& HListWidgetAttributes::drawheader( bool drawHeader_ ) {
	M_PROLOG
	_drawHeader = drawHeader_;
	_drawHeaderSet = true;
	return ( *this );
	M_EPILOG
}

HListWidgetAttributes& HListWidgetAttributes::checkable( bool checkable_ ) {
	M_PROLOG
	_checkable = checkable_;
	_checkableSet = true;
	return ( *this );
	M_EPILOG
}

HWidget::ptr_t HListWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HListWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HListWidget* list( create_widget<HListWidget>( window_, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( list->get_pointer(), node_ );
	return ( list->get_pointer() );
	M_EPILOG
}

bool HListWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	bool ok( HSearchableWidgetCreator::do_prepare_attributes( attributes_, node_ ) );
	if ( !ok ) {
		HListWidgetAttributes& attrs( dynamic_cast<HListWidgetAttributes&>( attributes_ ) );
		HString const& name( node_.get_name() );
		ok = true;
		if ( name == "draw_header" ) {
			attrs.drawheader( lexical_cast<bool>( xml::node_val( node_ ) ) );
		} else if ( name == "editable" ) {
			attrs.editable( lexical_cast<bool>( xml::node_val( node_ ) ) );
		} else if ( name == "checkable" ) {
			attrs.checkable( lexical_cast<bool>( xml::node_val( node_ ) ) );
		} else if ( name == "sortable" ) {
			attrs.sortable( lexical_cast<bool>( xml::node_val( node_ ) ) );
		} else {
			ok = false;
		}
	}
	return ( ok );
	M_EPILOG
}

bool HListWidgetCreator::do_apply_resources( HWidget::ptr_t widget_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	bool ok( false );
	HString const& name( node_.get_name() );
	if ( name == "column" ) {
		int placement( lexical_cast<int>( xml::attr_val( node_, "placement" ) ) );
		HString columnName( xml::attr_val( node_, "name" ) );
		int width( lexical_cast<int>( xml::attr_val( node_, "width" ) ) );
		HString xmlAlign( xml::attr_val( node_, "align" ) );
		HString xmlType( xml::attr_val( node_, "type" ) );
		xml::value_t format( xml::try_attr_val( node_, "format" ) );
		HWidget::BITS::ALIGN align( HWidget::BITS::ALIGN::LEFT );
		if ( xmlAlign == "left" ) {
			align = HWidget::BITS::ALIGN::LEFT;
		} else if ( xmlAlign == "center" ) {
			align = HWidget::BITS::ALIGN::CENTER;
		} else if ( xmlAlign == "right" ) {
			align = HWidget::BITS::ALIGN::RIGHT;
		} else {
			M_THROW( _( "unknown align type" ), node_.get_line() );
		}
		TYPE type( TYPE::HSTRING );
		if ( xmlType == "string" ) {
			type = TYPE::HSTRING;
		} else if ( xmlType == "integer" ) {
			type = TYPE::INT_LONG_LONG;
		} else if ( xmlType == "real" ) {
			type = TYPE::DOUBLE_LONG;
		} else if ( ( xmlType == "date" ) || ( xmlType == "datetime" ) || ( xmlType == "time" ) ) {
			type = TYPE::HTIME;
		} else {
			M_THROW( _( "unknown column type" ), node_.get_line() );
		}
		xml::value_t refid( xml::try_attr_val( node_, "refid" ) );
		HWidget::ptr_t field;
		if ( !! refid ) {
			HXml::HConstNodeProxy xmlField( node_.xml()->get_element_by_id( *refid ) );
			M_ENSURE( !! xmlField );
			field = HWidgetFactory::get_instance().create_widget( widget_->get_window(), xmlField );
		}
		HListWidget* list( dynamic_cast<HListWidget*>( widget_.raw() ) );
		list->add_column( placement, make_column( node_, list, columnName, width, align, type, !! format ? *format : "", field.raw() ) );
		ok = true;
	}
	return ( ok );
	M_EPILOG
}

HListWidget::HColumnInfo::ptr_t HListWidgetCreator::make_column(
		yaal::tools::HXml::HConstNodeProxy const& node_,
		HListWidget* widget_,
		yaal::hcore::HString const& columnName,
		int width,
		HListWidget::BITS::ALIGN align,
		TYPE type,
		yaal::hcore::HString const& format_,
		HWidget* associatedWidget ) {
	M_PROLOG
	return ( do_make_column( node_, widget_, columnName, width, align, type, format_, associatedWidget ) );
	M_EPILOG
}

HListWidget::HColumnInfo::ptr_t HListWidgetCreator::do_make_column(
		yaal::tools::HXml::HConstNodeProxy const&,
		HListWidget*,
		yaal::hcore::HString const& columnName,
		int width,
		HListWidget::BITS::ALIGN align,
		TYPE type,
		yaal::hcore::HString const& format_,
		HWidget* associatedWidget ) {
	M_PROLOG
	return ( make_resource<HListWidget::HColumnInfo>( columnName, width, align, type, format_, associatedWidget ) );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "list",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HListWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


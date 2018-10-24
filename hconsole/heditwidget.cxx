/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "heditwidget.hxx"
#include "hwindow.hxx"
#include "hcore/hlog.hxx"
#include "tools/keycode.hxx"
#include "tools/hxml.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

char const _maskLetters_[]      = "^[a-zA-ZąĄćĆęĘłŁńŃóÓśŚźŹżŻ]*$";
char const _maskDigits_[]       = "^[0-9]*$";
char const _maskAlpha_[]        = "^[a-zA-Z0-9]*$";
char const _maskExtended_[]     = "^[0-9a-zA-ZąĄćĆęĘłŁńŃóÓśŚźŹżŻ\\.\\(\\) -]*$";
char const _maskLoose_[]        = ".*";
char const* const _maskDefault_ = _maskLetters_;

HEditWidget::HEditWidget(
	HWindow* parent_, int row_, int column_, int height_, int width_,
	yaal::hcore::HString const& label_, HWidgetAttributesInterface const& attr_
) : HWidget( parent_, row_, column_, height_, width_, label_, attr_ )
	, _replace( false )
	, _multiLine( height_ > 1 ? true : false )
	, _readOnly( false )
	, _rightAligned( false )
	, _password( false )
	, _maxStringSize( 127 )
	, _cursorPosition ( 0 )
	, _widgetOffset( 0 )
	, _maxHistoryLevel( 8 )
	, _mask()
	, _string()
	, _infoString( _string )
	, _history()
	, _historyIt() {
	M_PROLOG
	attr_.apply( *this );
	_varTmpBuffer.reserve( _maxStringSize );
	if ( _rightAligned && _multiLine ) {
		M_THROW( _( "edit-widget right aligned and multiline at the same time" ), 0 );
	}
	_history.push_back( "" );
	_historyIt = hcore::cyclic_iterator( _history );
	int length( static_cast<int>( _string.get_length() ) );
/* this is part of draw_label() method, we cannot wait with setting up
 * _widthRaw until draw_label(), which is called from paint()
 * because ... see next comment */
	_widthRaw = ( _width > 0 ) ? _width
		: HConsole::get_instance().get_width() + _width - _columnRaw;
/* _widthRaw must be set up properly before setting up _cursorPosition and
 * _widgetOffset whose are used in paint() */
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_widgetOffset = ( length - _widthRaw ) + 1;
	} else {
		_cursorPosition = length;
	}
	return;
	M_EPILOG
}

HEditWidget::~HEditWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HEditWidget::set_mask( yaal::hcore::HString const& mask_ ) {
	M_PROLOG
	if ( ! _mask.compile( mask_, HRegex::COMPILE::NO_EXCEPTION ) ) {
		M_THROW( _mask.error(), _mask.error_code() );
	}
	if ( ! _mask.matches( _string ) ) {
		M_THROW( _mask.error(), _mask.error_code() );
	}
	return;
	M_EPILOG
}

void HEditWidget::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	draw_label();
	if ( ! _password ) {
		_varTmpBuffer = _string.mid( _widgetOffset, _widthRaw );
	} else {
		_varTmpBuffer.clear();
	}
	int long len( _varTmpBuffer.get_length() );
	if ( len < _widthRaw ) {
		_varTmpBuffer.reserve( _widthRaw );
		_varTmpBuffer.fill( ' '_ycp, len, _widthRaw - len );
	}
	cons.mvprintf( _rowRaw, _columnRaw, _varTmpBuffer );
	if ( _focused ) {
		cons.move( _rowRaw, _columnRaw + ( _password ? 0 : _cursorPosition ) );
		cons.curs_set( _replace ? CURSOR::VERY_VISIBLE : CURSOR::VISIBLE );
	}
	return;
	M_EPILOG
}

HString const& HEditWidget::get_text( void ) const {
	M_PROLOG
	return ( _string );
	M_EPILOG
}

HInfo const& HEditWidget::do_get_data( void ) const {
	M_PROLOG
	return ( _infoString );
	M_EPILOG
}

void HEditWidget::set_bits( int const* maxlen,
							yaal::hcore::HString const* val, yaal::hcore::HString const* mask,
							bool const* replace, bool const* multiline,
							bool const* readonly, bool const* rightAlign,
							bool const* password, int const* maxhist ) {
	M_PROLOG
	bool wantRepaint( false );
	if ( maxlen ) {
		if ( *maxlen < 1 ) {
			M_THROW( _( "buffer size is ridiculously low" ), *maxlen );
		}
		_maxStringSize = *maxlen;
	}
	if ( val ) {
		int length( static_cast<int>( val->get_length() ) );
		if ( length > _maxStringSize ) {
			M_THROW( _( "initial value too big" ), length - _maxStringSize );
		}
		set_text( *val );
	}
	if ( mask ) {
		set_mask( *mask );
	}
	if ( replace ) {
		_replace = *replace;
	}
	if ( multiline ) {
		_multiLine = *multiline;
	}
	if ( readonly ) {
		_readOnly = *readonly;
	}
	if ( rightAlign ) {
		wantRepaint = true;
		_rightAligned = *rightAlign;
	}
	if ( password ) {
		_password = *password;
		wantRepaint = true;
	}
	if ( maxhist ) {
		_maxHistoryLevel = *maxhist;
	}
	if ( wantRepaint ) {
		schedule_repaint();
	}
	return;
	M_EPILOG
}

char const _wordSeparator_ [ ] = " \t\n`-=[]\\;',./~!@#$%^&*()+{}|:\"<>?";

int HEditWidget::find_eow( int length_ ) {
	M_PROLOG
	int index = static_cast<int>( _string.find_other_than( _wordSeparator_, _widgetOffset + _cursorPosition ) );
	if ( index >= 0 ) {
		index = static_cast<int>( _string.find_one_of( _wordSeparator_, index ) );
		if ( index < 0 ) {
			index = length_;
		}
	}
	return ( index );
	M_EPILOG
}

int HEditWidget::go_to_eow( int length_ ) {
	M_PROLOG
	int err = ( ( _widgetOffset + _cursorPosition ) < length_ ) ? 0 : 1;
	if ( ! err ) {
		int index = find_eow( length_ );
		err = ( index >= 0 ? 0 : 1 );
		if ( ! err ) {
			_cursorPosition = ( index - _widgetOffset );
			if ( _cursorPosition >= _widthRaw ) {
				_widgetOffset += ( ( _cursorPosition - _widthRaw  ) + 1 );
				_cursorPosition = _widthRaw - 1;
			}
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::kill_line( void ) {
	M_PROLOG
	bool doKill( ! ( _readOnly || _varTmpBuffer.is_empty() ) );
	if ( doKill ) {
		_varTmpBuffer.set_at( 0, 0_ycp );
		_widgetOffset = 0;
		_cursorPosition = 0;
	}
	return ( doKill ? 0 : 1 );
	M_EPILOG
}

int HEditWidget::move_right( int length_ ) {
	M_PROLOG
	int err( ( ( _cursorPosition + _widgetOffset ) < length_ ) ? 0 : 1 );
	if ( ! err ) {
		++ _cursorPosition;
		if ( _cursorPosition >= _widthRaw ) {
			M_ASSERT( _cursorPosition == _widthRaw );
			_cursorPosition = _widthRaw - 1;
			++ _widgetOffset;
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::move_left( void ) {
	M_PROLOG
	int err( ( ( _widgetOffset + _cursorPosition ) > 0 ) ? 0 : 1 );
	if ( ! err ) {
		if ( _cursorPosition > 0 ) {
			-- _cursorPosition;
		} else {
			M_ASSERT( _widgetOffset > 0 );
			-- _widgetOffset;
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::go_to_end( int length_ ) {
	M_PROLOG
	if ( length_ >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_widgetOffset = ( length_ - _widthRaw ) + 1;
	} else {
		_cursorPosition = length_;
	}
	return ( 0 );
	M_EPILOG
}

int HEditWidget::delete_char( int length_ ) {
	M_PROLOG
	int err = ! ( _readOnly || _replace ) ? 0 : 1;
	if ( ! err ) {
		if ( ( _widgetOffset + _cursorPosition ) >= length_ ) {
			_varTmpBuffer.clear();
			_widgetOffset = 0;
			_cursorPosition = 0;
		} else {
			_varTmpBuffer.erase( _widgetOffset + _cursorPosition, 1 );
			if ( ( _widgetOffset > 0 ) && ( ( _widgetOffset + _widthRaw ) >= length_ ) ) {
				_widgetOffset --;
				++ _cursorPosition;
			}
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::kill_char( void ) {
	M_PROLOG
	int err = ( ! _readOnly && ( ( _widgetOffset + _cursorPosition ) > 0 ) ) ? 0 : 1;
	if ( ! err ) {
		if ( _widgetOffset > 0 ) {
			_widgetOffset --;
		} else if ( _cursorPosition > 0 ) {
			_cursorPosition --;
		}
		if ( ! _replace ) {
			_varTmpBuffer.erase( _widgetOffset + _cursorPosition, 1 );
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::find_bow( int length_ ) {
	M_PROLOG
	int index = static_cast<int>( _string.reverse_find_other_than( _wordSeparator_, length_ - ( _widgetOffset + _cursorPosition ) ) );
	if ( index >= 0 ) {
		index = static_cast<int>( _string.reverse_find_one_of( _wordSeparator_, index ) );
		if ( index < 0 ) {
			index = 0;
		} else {
			index = length_ - index;
		}
	}
	return ( index );
	M_EPILOG
}

int HEditWidget::go_to_bow( int length_ ) {
	M_PROLOG
	int err = ( _widgetOffset + _cursorPosition ) ? 0 : 1;
	if ( ! err ) {
		int index = find_bow( length_ );
		if ( index >= 0 ) {
			_cursorPosition = ( index - _widgetOffset );
			if ( _cursorPosition < 0 ) {
				_widgetOffset += _cursorPosition;
				_cursorPosition = 0;
			}
		} else {
			err = 1;
			_widgetOffset = 0;
			_cursorPosition = 0;
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::delete_word( int length_ ) {
	M_PROLOG
	int err = ( ! ( _readOnly || _replace ) ) ? 0 : 1;
	if ( ! err ) {
		int oldIndex = ( _widgetOffset + _cursorPosition );
		if ( oldIndex >= length_ ) {
			_varTmpBuffer.set_at( 0, 0_ycp );
			_widgetOffset = 0;
			_cursorPosition = 0;
		} else {
			int index = find_eow( length_ );
			_varTmpBuffer.erase( oldIndex, index - oldIndex );
		}
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::kill_word( int length_ ) {
	M_PROLOG
	int err = ( ! ( _readOnly || _replace ) ) ? 0 : 1;
	if ( ! err ) {
		int oldIndex = ( _widgetOffset + _cursorPosition );
		if ( oldIndex ) {
			int index = find_bow( length_ );
			if ( index >= 0 ) {
				_cursorPosition = ( index - _widgetOffset );
				if ( _cursorPosition < 0 ) {
					_widgetOffset += _cursorPosition;
					_cursorPosition = 0;
				}
				_varTmpBuffer.erase( index, oldIndex - index );
			} else {
				err = 1;
				_widgetOffset = 0;
				_cursorPosition = 0;
			}
		} else
			err = 1;
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::insert_char( int code_, int length_ ) {
	M_PROLOG
	int err = 0;
	if ( ( ! _readOnly && ( code_ > 31 ) && ( code_ < KEY_CODE::META_BASE ) )
			&& ( ( ! _replace && ( length_ < _maxStringSize ) )
				|| ( _replace && ( ( _widgetOffset + _cursorPosition ) < length_ ) )	) ) {
		if ( ! _replace ) {
			_varTmpBuffer.insert( _widgetOffset+ _cursorPosition, 1 );
		}
		_varTmpBuffer.set_at( _cursorPosition + _widgetOffset, code_point_t( static_cast<yaal::u32_t>( code_ ) ) );
		_cursorPosition ++;
		if ( _cursorPosition >= _widthRaw ) {
			_cursorPosition = _widthRaw - 1;
			_widgetOffset ++;
		}
	} else {
		err = code_;
	}
	return ( err );
	M_EPILOG
}

int HEditWidget::update_from_history( void ) {
	M_PROLOG
	if ( ! _history.is_empty() && ( _historyIt != _history.end() ) ) {
		_varTmpBuffer = *_historyIt;
	}
	int length = static_cast<int>( _varTmpBuffer.get_length() );
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_widgetOffset = ( length - _widthRaw ) + 1;
	} else {
		_widgetOffset = 0;
		_cursorPosition = length;
	}
	return ( 0 );
	M_EPILOG
}

int HEditWidget::do_process_input( int code_ ) {
	M_PROLOG
	static int const HISTORY_OPERATION( -1 );
	static int const DATA_ENTER( -2 );
	int errorCode = 0;
	code_ = HWidget::do_process_input( code_ );
	_varTmpBuffer = _string;
	int oldWidgetOffset( _widgetOffset );
	int oldCursorPosition( _cursorPosition );
	int length( static_cast<int>( _varTmpBuffer.get_length() ) );
	switch ( code_ ) {
		case ( KEY_CODE::PAGE_UP ): {
			_historyIt = hcore::cyclic_iterator( _history );
			errorCode = HISTORY_OPERATION;
		} break;
		case ( KEY_CODE::PAGE_DOWN ): {
			_historyIt = hcore::cyclic_iterator( _history );
			-- _historyIt;
			errorCode = HISTORY_OPERATION;
		} break;
		case ( KEY_CODE::UP ): {
			++ _historyIt;
			errorCode = HISTORY_OPERATION;
		} break;
		case ( KEY_CODE::DOWN ): {
			-- _historyIt;
			errorCode = HISTORY_OPERATION;
		} break;
		case ( '\t' ):
		/* enter works like tab without focus movement */
		case ( '\r' ): {
			int histSize( static_cast<int>( _history.size() ) );
			++ histSize;
			while ( -- histSize ) {
				if ( ( *( ++ _historyIt ) ) == _string ) {
					break;
				}
			}
			if ( _string.get_length() && ( ! histSize ) ) {
				_history.push_front( _string );
				histSize = static_cast<int>( _history.size() );
				while ( histSize -- > _maxHistoryLevel ) {
					_history.pop_back(); /* FIXME investigate if it actually work */
				}
				_historyIt = hcore::cyclic_iterator( _history );
				-- _historyIt;
			} else {
				-- _historyIt;
			}
			errorCode = DATA_ENTER;
		} break;
		case ( KEY_CODE::LEFT ): {
			errorCode = move_left();
		} break;
		case ( KEY<'a'>::ctrl ):
		case ( KEY_CODE::HOME ): {
			_cursorPosition = 0;
			_widgetOffset = 0;
		} break;
		case ( KEY<'e'>::ctrl ):
		case ( KEY_CODE::END ): {
			errorCode = go_to_end( length );
		} break;
		case ( KEY_CODE::RIGHT ): {
			errorCode = move_right( length );
		} break;
		case ( KEY<'u'>::ctrl ): {
			errorCode = kill_line();
		} break;
		case ( KEY_CODE::DELETE ): {
			errorCode = delete_char( length );
		} break;
		case ( KEY_CODE::BACKSPACE ): {
			errorCode = kill_char();
		} break;
		case ( KEY_CODE::INSERT ): {
			_replace = ! _replace;
		} break;
		case ( KEY<'f'>::meta ): {
			errorCode = go_to_eow( length );
		} break;
		case ( KEY<'b'>::meta ): {
			errorCode = go_to_bow( length );
		} break;
		case ( KEY<'d'>::meta ): {
			errorCode = delete_word( length );
		} break;
		case ( KEY<'w'>::ctrl ): {
			errorCode = kill_word( length );
		} break;
		default: {
			errorCode = insert_char( code_, length );
		} break;
	}
	if ( errorCode == HISTORY_OPERATION ) {
		errorCode = update_from_history();
	}
	if ( ! errorCode ) {
		if ( _mask.is_valid() && ! _mask.matches( _varTmpBuffer ) ) {
			if ( _window ) {
				_window->status_bar()->message( COLOR::BG_BROWN, "%s", _mask.error() );
			}
			errorCode = 1;
		} else {
			code_ = 0;
			_string = _varTmpBuffer;
			if ( _window ) {
				_window->status_bar()->clear( COLOR::FG_LIGHTGRAY );
			}
			schedule_repaint();
		}
	}
	if ( errorCode && ( errorCode != DATA_ENTER ) ) {
		_widgetOffset = oldWidgetOffset;
		_cursorPosition = oldCursorPosition;
		HConsole::get_instance().bell();
	}
	return ( code_ );
	M_EPILOG
}

void HEditWidget::set_text( HString const& string_ ) {
	M_PROLOG
	int length( 0 );
	HString errorMessage;
	if ( _mask.is_valid() && ! _mask.matches( string_ ) ) {
		M_THROW( _mask.error(), _mask.error_code() );
	}
	_string = string_;
	length = static_cast<int>( _string.get_length() );
	_widgetOffset = 0;
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_widgetOffset = ( length - _widthRaw ) + 1;
	} else {
		_cursorPosition = length;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HEditWidget::do_set_data( HInfo const& info_ ) {
	M_PROLOG
	set_text( info_.get_string() );
	return;
	M_EPILOG
}

bool HEditWidget::do_click( mouse::OMouse & mouse_ ) {
	M_PROLOG
	bool handled( HWidget::do_click( mouse_ ) );
	if ( ! handled ) {
		int position( 0 );
		position = mouse_._column - _columnRaw;
		if ( position < _string.get_length() ) {
			_cursorPosition = position;
			schedule_repaint();
			handled = true;
		}
	}
	return ( handled );
	M_EPILOG
}

HEditWidgetAttributes::HEditWidgetAttributes( void )
	: HWidgetAttributes()
	, _replace( false )
	, _replaceSet( false )
	, _multiLine( false )
	, _multiLineSet( false )
	, _readOnly( false )
	, _readOnlySet( false )
	, _rightAligned( false )
	, _rightAlignedSet( false )
	, _password( false )
	, _passwordSet( false )
	, _maxStringSize( 127 )
	, _maxStringSizeSet( false )
	, _maxHistoryLevel( 8 )
	, _maxHistoryLevelSet( false )
	, _mask( _maskDefault_ )
	, _maskSet( false )
	, _text( _maskDefault_ )
	, _textSet( false ) {
	return;
}

void HEditWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HEditWidget* widget( dynamic_cast<HEditWidget*>( &widget_ ) );
	if ( widget ) {
		widget->set_bits(
				_maxStringSizeSet   ? &_maxStringSize   : nullptr,
				_textSet            ? &_text            : nullptr,
				_maskSet            ? &_mask            : nullptr,
				_replaceSet         ? &_replace         : nullptr,
				_multiLineSet       ? &_multiLine       : nullptr,
				_readOnlySet        ? &_readOnly        : nullptr,
				_rightAlignedSet    ? &_rightAligned    : nullptr,
				_passwordSet        ? &_password        : nullptr,
				_maxHistoryLevelSet ? &_maxHistoryLevel : nullptr
		);
	}
	HWidgetAttributes::do_apply( widget_ );
	return;
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::replace( bool replace_ ) {
	M_PROLOG
	_replace = replace_;
	_replaceSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::multiline( bool multiline_ ) {
	M_PROLOG
	_multiLine = multiline_;
	_multiLineSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::readonly( bool readonly_ ) {
	M_PROLOG
	_readOnly = readonly_;
	_readOnlySet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::rightaligned( bool rightaligned_ ) {
	M_PROLOG
	_rightAligned = rightaligned_;
	_rightAlignedSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::password( bool password_ ) {
	M_PROLOG
	_password = password_;
	_passwordSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::max_string_size( int maxStringSize_ ) {
	M_PROLOG
	_maxStringSize = maxStringSize_;
	_maxStringSizeSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::max_history_level( int maxHistoryLevel_ ) {
	M_PROLOG
	_maxHistoryLevel = maxHistoryLevel_;
	_maxHistoryLevelSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::mask( yaal::hcore::HString const& mask_ ) {
	M_PROLOG
	_mask = mask_;
	_maskSet = true;
	return ( *this );
	M_EPILOG
}

HEditWidgetAttributes& HEditWidgetAttributes::text( yaal::hcore::HString const& text_ ) {
	M_PROLOG
	_text = text_;
	_textSet = true;
	return ( *this );
	M_EPILOG
}

HWidget::ptr_t HEditWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HEditWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HEditWidget* edit( create_widget<HEditWidget>( window_, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( edit->get_pointer(), node_ );
	return ( edit->get_pointer() );
	M_EPILOG
}

bool HEditWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HEditWidgetAttributes& attrs( dynamic_cast<HEditWidgetAttributes&>( attributes_ ) );
	HString const& name( node_.get_name() );
	bool ok( true );
	if ( name == "max_string_size" ) {
		attrs.max_string_size( lexical_cast<int>( xml::node_val( node_ ) ) );
	} else if ( name == "value" ) {
		xml::value_t value( xml::try_node_val( node_ ) );
		attrs.text( value ? *value : "" );
	} else if ( name == "mask" ) {
		attrs.mask( xml::node_val( node_ ) );
	} else if ( name == "replace" ) {
		attrs.replace( lexical_cast<bool>( xml::node_val( node_ ) ) );
	} else if ( name == "multi_line" ) {
		attrs.multiline( lexical_cast<bool>( xml::node_val( node_ ) ) );
	} else if ( name == "read_only" ) {
		attrs.readonly( lexical_cast<bool>( xml::node_val( node_ ) ) );
	} else if ( name == "right_aligned" ) {
		attrs.rightaligned( lexical_cast<bool>( xml::node_val( node_ ) ) );
	} else if ( name == "password" ) {
		attrs.password( lexical_cast<bool>( xml::node_val( node_ ) ) );
	} else if ( name == "max_history_level" ) {
		attrs.max_history_level( lexical_cast<int>( xml::node_val( node_ ) ) );
	} else {
		ok = false;
	}
	return ( ok );
	M_EPILOG
}

bool HEditWidgetCreator::do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) {
	return ( false );
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "edit",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HEditWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

HInfoString::HInfoString( HString& string_ )
	: _data( string_ ) {
}

HInfoString::~HInfoString( void ) {
}

HString const& HInfoString::do_get_string( void ) const {
	return ( _data );
}

int long long HInfoString::do_get_integer( void ) const {
	return ( lexical_cast<int long long>( _data ) );
}

double long HInfoString::do_get_real( void ) const {
	return ( lexical_cast<double long>( _data ) );
}

HTime const& HInfoString::do_get_time( void ) const {
	M_ASSERT( 0 && "impossible inplace conversion requested" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HTime const dummy( HTime::TZ::LOCAL );
	return ( dummy );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

void HInfoString::do_set_integer( int long long data_ ) {
	_data = data_;
}

void HInfoString::do_set_real( double long data_ ) {
	_data = data_;
}

void HInfoString::do_set_string( HString const& str_ ) {
	_data = str_;
}

void HInfoString::do_set_time( HTime const& time_ ) {
	_data = time_.string();
}

}

}


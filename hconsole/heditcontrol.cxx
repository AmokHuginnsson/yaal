/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	heditcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "heditcontrol.hxx"
#include "hwindow.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

char const* const _maskLetters_  = "^[a-zA-Z±¡æÆêÊ³£ñÑóÓ¶¦¼¬¿¯]*$";
char const* const _maskDigits_   = "^[0-9]*$";
char const* const _maskAlpha_    = "^[a-zA-Z0-9]*$";
char const* const _maskExtended_ = "^[0-9a-zA-Z±¡æÆêÊ³£ñÑóÓ¶¦¼¬¿¯\\.\\(\\) -]*$";
char const* const _maskLoose_    = ".*";
char const* const _maskDefault_  = _maskLetters_;

HEditControl::HEditControl( HWindow* parent_,
		int row_, int column_, int height_, int width_,
		char const* label_, int bufferSize_, char const * value_,
		char const* mask_, bool replace_, bool rightAligned_,
		bool multiLine_, bool readOnly_, bool password_,
		int maxHistoryLevel_ )
					: HControl( parent_, row_, column_, height_,
							width_, label_ ),
					_replace( replace_ ),
					_multiLine( multiLine_ || ( height_ > 1 ) ? true : false ),
					_readOnly( readOnly_ ), _rightAligned( rightAligned_ ),
					_password( password_ ),
					_maxStringSize( bufferSize_ ), _cursorPosition ( 0 ),
					_controlOffset( 0 ), _maxHistoryLevel( maxHistoryLevel_ ),
					_pattern(), _string( bufferSize_, true ),
					_infoString( _string ), _history(), _historyIt() {
	M_PROLOG
	int errorCode = 0;
	int length = 0;
	HString errorMessage;
	if ( bufferSize_ < 1 )
		M_THROW( _( "buffer size is ridiculously low" ), bufferSize_ );
	if ( value_ ) {
		length = static_cast<int>( ::strlen( value_ ) );
		if ( length > bufferSize_ )
			M_THROW( _( "initial value too big" ), length - bufferSize_ );
	}
	_varTmpBuffer.hs_realloc ( bufferSize_ + 1 );
	if ( _rightAligned && _multiLine )
		M_THROW( _( "edit-control right aligned and multiline at the same time" ), 0 );
	_string = value_;
	_history.push_back ( "" );
	_historyIt = _history.hook();
	if ( ( errorCode = _pattern.parse_re( mask_ ) ) )
		M_THROW( _pattern.error(), errorCode );
	( _pattern.find( value_ ? value_ : "" ) == _pattern.end() ) && ( errorCode = _pattern.error_code() );
	if ( errorCode )
		M_THROW( _pattern.error(), errorCode );
	length = static_cast<int>( _string.get_length() );
/* this is part of draw_label() method, we cannot wait with setting up
 * _widthRaw until draw_label(), which is called from refresh()
 * because ... see next comment */
	_widthRaw = ( _width > 0 ) ? _width
		: HConsole::get_instance().get_width() + _width - _columnRaw;
/* _widthRaw must be set up properly before setting up _cursorPosition and
 * _controlOffset whose are used in refresh() */
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_controlOffset = ( length - _widthRaw ) + 1;
	} else
		_cursorPosition = length;
#ifdef __DEBUG__
#endif /* __DEBUG__ */
	return;
	M_EPILOG
}

HEditControl::~HEditControl( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HEditControl::do_refresh( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	draw_label();
	_varTmpBuffer.hs_realloc( _widthRaw + 1 );
	if ( ! _password )
		_varTmpBuffer = _string.mid( _controlOffset, _widthRaw );
	else
		_varTmpBuffer.clear();
	int long len = _varTmpBuffer.get_length();
	if ( len < _widthRaw )
		_varTmpBuffer.fill( ' ', len, _widthRaw - len );
	cons.mvprintf( _rowRaw, _columnRaw, _varTmpBuffer.raw() );
	if ( _focused ) {
		cons.move( _rowRaw, _columnRaw + ( _password ? 0 : _cursorPosition ) );
		cons.curs_set( _replace ? CURSOR::VERY_VISIBLE : CURSOR::VISIBLE );
	}
	return;
	M_EPILOG
}

HString const& HEditControl::get_text( void ) const {
	M_PROLOG
	return ( _string );
	M_EPILOG
}

HInfo const& HEditControl::get( void ) const {
	M_PROLOG
	return ( _infoString );
	M_EPILOG
}

void HEditControl::set_flags( bool replace_, bool password_ ) {
	M_PROLOG
	_replace = replace_;
	_password = password_;
	return;
	M_EPILOG
}

char const _wordSeparator_ [ ] = " \t\n`-=[]\\;',./~!@#$%^&*()+{}|:\"<>?";

int HEditControl::find_eow( int length_ ) {
	M_PROLOG
	int index = static_cast<int>( _string.find_other_than( _wordSeparator_, _controlOffset + _cursorPosition ) );
	if ( index >= 0 ) {
		index = static_cast<int>( _string.find_one_of( _wordSeparator_, index ) );
		if ( index < 0 )
			index = length_;
	}
	return ( index );
	M_EPILOG
}

int HEditControl::go_to_eow( int length_ ) {
	M_PROLOG
	int err = ( ( _controlOffset + _cursorPosition ) < length_ ) ? 0 : 1;
	if ( ! err ) {
		int index = find_eow( length_ );
		err = ( index >= 0 ? 0 : 1 );
		if ( ! err ) {
			_cursorPosition =	( index - _controlOffset );
			if ( _cursorPosition >= _widthRaw ) {
				_controlOffset += ( ( _cursorPosition - _widthRaw  ) + 1 );
				_cursorPosition = _widthRaw - 1;
			}
		}
	}
	return ( err );
	M_EPILOG
}

int HEditControl::kill_line( void ) {
	M_PROLOG
	if ( ! _readOnly ) {
		_varTmpBuffer.set_at( 0, 0 );
		_controlOffset = 0;
		_cursorPosition = 0;
	}
	return ( ! _readOnly ? 0 : 1 );
	M_EPILOG
}

int HEditControl::move_right( int length_ ) {
	M_PROLOG
	int err = ( ( _cursorPosition + _controlOffset ) < length_ ) ? 0 : 1;
	if ( ! err ) {
		_cursorPosition ++;
		if ( _cursorPosition >= _widthRaw ) {
			_cursorPosition = _widthRaw - 1;
			_controlOffset ++;
		}
	}
	return ( err );
	M_EPILOG
}

int HEditControl::move_left( void ) {
	M_PROLOG
	int err = ( ( _controlOffset + _cursorPosition ) > 0 ) ? 0 : 1;
	if ( ! err ) {
		if ( _cursorPosition > 0 )
			_cursorPosition --;
		else if ( _controlOffset > 0 )
			_controlOffset --;
	}
	return ( err );
	M_EPILOG
}

int HEditControl::go_to_end( int length_ ) {
	M_PROLOG
	if ( length_ >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_controlOffset = ( length_ - _widthRaw ) + 1;
	} else
		_cursorPosition = length_;
	return ( 0 );
	M_EPILOG
}

int HEditControl::delete_char( int length_ ) {
	M_PROLOG
	int err = ! ( _readOnly || _replace ) ? 0 : 1;
	if ( ! err ) {
		if ( ( _controlOffset + _cursorPosition ) >= length_ ) {
			_varTmpBuffer.set_at( 0, 0 );
			_controlOffset = 0;
			_cursorPosition = 0;
		} else {
			_varTmpBuffer.erase( _controlOffset + _cursorPosition, 1 );
			if ( ( _controlOffset > 0 ) && ( ( _controlOffset + _widthRaw ) >= length_ ) ) {
				_controlOffset --;
				++ _cursorPosition;
			}
		}
	}
	return ( err );
	M_EPILOG
}

int HEditControl::kill_char( void ) {
	M_PROLOG
	int err = ( ! _readOnly && ( ( _controlOffset + _cursorPosition ) > 0 ) ) ? 0 : 1;
	if ( ! err ) {
		if ( _controlOffset > 0 )
			_controlOffset --;
		else if ( _cursorPosition > 0 )
			_cursorPosition --;
		if ( ! _replace )
			_varTmpBuffer.erase( _controlOffset+ _cursorPosition, 1 );
	}
	return ( err );
	M_EPILOG
}

int HEditControl::find_bow( int length_ ) {
	M_PROLOG
	int index = static_cast<int>( _string.reverse_find_other_than( _wordSeparator_, length_ - ( _controlOffset + _cursorPosition ) ) );
	if ( index >= 0 ) {
		index = static_cast<int>( _string.reverse_find_one_of( _wordSeparator_, index ) );
		if ( index < 0 )
			index = 0;
		else
			index = length_ - index;
	}
	return ( index );
	M_EPILOG
}

int HEditControl::go_to_bow( int length_ ) {
	M_PROLOG
	int err = ( _controlOffset + _cursorPosition ) ? 0 : 1;
	if ( ! err ) {
		int index = find_bow( length_ );
		if ( index >= 0 ) {
			_cursorPosition = ( index - _controlOffset );
			if ( _cursorPosition < 0 ) {
				_controlOffset += _cursorPosition;
				_cursorPosition = 0;
			}
		} else {
			err = 1;
			_controlOffset = 0;
			_cursorPosition = 0;
		}
	}
	return ( err );
	M_EPILOG
}

int HEditControl::delete_word( int length_ ) {
	M_PROLOG
	int err = ( ! ( _readOnly || _replace ) ) ? 0 : 1;
	if ( ! err ) {
		int oldIndex = ( _controlOffset + _cursorPosition );
		if ( oldIndex >= length_ ) {
			_varTmpBuffer.set_at( 0, 0 );
			_controlOffset = 0;
			_cursorPosition = 0;
		} else {
			int index = find_eow( length_ );
			_varTmpBuffer.erase( oldIndex, index - oldIndex );
		}
	}
	return ( err );
	M_EPILOG
}

int HEditControl::kill_word( int length_ ) {
	M_PROLOG
	int err = ( ! ( _readOnly || _replace ) ) ? 0 : 1;
	if ( ! err ) {
		int oldIndex = ( _controlOffset + _cursorPosition );
		if ( oldIndex ) {
			int index = find_bow( length_ );
			if ( index >= 0 ) {
				_cursorPosition = ( index - _controlOffset );
				if ( _cursorPosition < 0 ) {
					_controlOffset += _cursorPosition;
					_cursorPosition = 0;
				}
				_varTmpBuffer.erase( index, oldIndex - index );
			} else {
				err = 1;
				_controlOffset = 0;
				_cursorPosition = 0;
			}
		} else
			err = 1;
	}
	return ( err );
	M_EPILOG
}

int HEditControl::insert_char( int code_, int length_ ) {
	M_PROLOG
	int err = 0;
	if ( ( ! _readOnly && ( code_ > 31 ) && ( code_ < 256 ) )
			&& ( ( ! _replace && ( length_ < _maxStringSize ) )
				|| ( _replace && ( ( _controlOffset + _cursorPosition ) < length_ ) )	) ) {
		if ( ! _replace )
			_varTmpBuffer.insert( _controlOffset+ _cursorPosition, 1 );
		_varTmpBuffer.set_at( _cursorPosition + _controlOffset, static_cast<char>( code_ ) );
		_cursorPosition ++;
		if ( _cursorPosition >= _widthRaw ) {
			_cursorPosition = _widthRaw - 1;
			_controlOffset ++;
		}
	} else
		err = code_;
	return ( err );
	M_EPILOG
}

int HEditControl::update_from_history( void ) {
	M_PROLOG
	if ( ! _history.is_empty() && ( _historyIt != _history.end() ) )
		_varTmpBuffer = *_historyIt;
	int length = static_cast<int>( _varTmpBuffer.get_length() );
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_controlOffset = ( length - _widthRaw ) + 1;
	} else {
		_controlOffset = 0;
		_cursorPosition = length;
	}
	return ( 0 );
	M_EPILOG
}

int HEditControl::do_process_input ( int code_ ) {
	M_PROLOG
	static int const HISTORY_OPERATION = -1;
	static int const DATA_ENTER = -2;
	int length = 0;
	int errorCode = 0;
	int oldControlOffset = 0;
	int oldCursorPosition = 0;
	HConsole& cons = HConsole::get_instance();
	code_ = HControl::do_process_input ( code_ );
	_varTmpBuffer = _string;
	oldControlOffset = _controlOffset;
	oldCursorPosition = _cursorPosition;
	length = static_cast<int>( _varTmpBuffer.get_length() );
	switch ( code_ ) {
		case ( KEY_CODES::PAGE_UP ):
			_historyIt = _history.hook();
			errorCode = HISTORY_OPERATION;
		break;
		case ( KEY_CODES::PAGE_DOWN ):
			_historyIt = _history.hook();
			-- _historyIt;
			errorCode = HISTORY_OPERATION;
		break;
		case ( KEY_CODES::UP ):
			++ _historyIt;
			errorCode = HISTORY_OPERATION;
		break;
		case ( KEY_CODES::DOWN ):
			-- _historyIt;
			errorCode = HISTORY_OPERATION;
		break;
		case ( '\t' ):
		/* enter works like tab without focus movement */
		case ( '\r' ): {
			errorCode = static_cast<int>( _history.size() );
			errorCode ++;
			while ( -- errorCode )
				if ( ( *( ++ _historyIt ) ) == _string )
					break;
			if ( _string.get_length() && ( ! errorCode ) ) {
				_history.push_front( _string );
				errorCode = static_cast<int>( _history.size() );
				while ( errorCode -- > _maxHistoryLevel )
					_history.pop_back(); /* FIXME investigate if it actually work */
				_historyIt = _history.hook();
				-- _historyIt;
			} else
				-- _historyIt;
			errorCode = DATA_ENTER;
		}
		break;
		case ( KEY_CODES::LEFT ):
			errorCode = move_left();
		break;
		case ( KEY<'a'>::ctrl ):
		case ( KEY_CODES::HOME ):
			_cursorPosition = 0;
			_controlOffset = 0;
		break;
		case ( KEY<'e'>::ctrl ):
		case ( KEY_CODES::END ):
			errorCode = go_to_end( length );
		break;
		case ( KEY_CODES::RIGHT ):
			errorCode = move_right( length );
		break;
		case ( KEY<'u'>::ctrl ):
			errorCode = kill_line();
		break;
		case ( KEY_CODES::DELETE ):
			errorCode = delete_char( length );
		break;
		case ( KEY_CODES::BACKSPACE ):
			errorCode = kill_char();
		break;
		case ( KEY_CODES::INSERT ):
			_replace = ! _replace;
		break;
		case ( KEY<'f'>::meta ):
			errorCode = go_to_eow( length );
		break;
		case ( KEY<'b'>::meta ):
			errorCode = go_to_bow( length );
		break;
		case ( KEY<'d'>::meta ):
			errorCode = delete_word( length );
		break;
		case ( KEY<'w'>::ctrl ):
			errorCode = kill_word( length );
		break;
		default:
			errorCode = insert_char( code_, length );
		break;
	}
	if ( errorCode == HISTORY_OPERATION )
		errorCode = update_from_history();
	if ( ! errorCode ) {
		_pattern.find( _varTmpBuffer.raw() );
		errorCode = _pattern.error_code();
		if ( errorCode )
			_parent->status_bar()->message( COLORS::BG_BROWN, "%s", _pattern.error().raw() );
		else {
			code_ = errorCode;
			_string = _varTmpBuffer;
			_parent->status_bar()->clear( COLORS::FG_LIGHTGRAY );
			schedule_refresh();
		}
	}
	if ( errorCode && ( errorCode != DATA_ENTER ) ) {
		_controlOffset = oldControlOffset;
		_cursorPosition = oldCursorPosition;
		cons.bell();
	}
	return ( code_ );
	M_EPILOG
}

void HEditControl::set_text( HString const& string_ ) {
	M_PROLOG
	int length( 0 );
	HString errorMessage;
	_pattern.find( string_.raw() );
	int errorCode = _pattern.error_code();
	if ( errorCode )
		M_THROW( _pattern.error(), errorCode );
	_string = string_;
	length = static_cast<int>( _string.get_length() );
	_controlOffset = 0;
	if ( length >= _widthRaw ) {
		_cursorPosition = _widthRaw - 1;
		_controlOffset = ( length - _widthRaw ) + 1;
	} else
		_cursorPosition = length;
	schedule_refresh();
	return;
	M_EPILOG
}

void HEditControl::set( HInfo const& info_ ) {
	M_PROLOG
	set_text( info_.get_string() );
	return;
	M_EPILOG
}

bool HEditControl::do_click( mouse::OMouse & mouse_ ) {
	M_PROLOG
	bool handled( HControl::do_click( mouse_ ) );
	if ( ! handled ) {
		int position( 0 );
		position = mouse_._column - _columnRaw;
		if ( position < _string.get_length() ) {
			_cursorPosition = position;
			schedule_refresh();
			handled = true;
		}
	}
	return ( handled );
	M_EPILOG
}

}

}


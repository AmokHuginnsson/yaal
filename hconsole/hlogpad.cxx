/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlogpad.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HLogPad::HLogLine::HLogLine( void )
	: _type( NONE )
	, _attribute( COLOR::ATTR_NORMAL )
	, _text() {
	M_PROLOG
	return;
	M_EPILOG
}

HLogPad::HLogLine::~HLogLine( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HLogPad::HLogPad( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, height_, width_, label_ )
	, _lines( 0 )
	, _offsetRow( 0 )
	, _offsetColumn( 0 )
	, _attribute( COLOR::ATTR_NORMAL )
	, _contents() {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HLogPad::~HLogPad( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HLogPad::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	draw_label();
	_varTmpBuffer.reserve( _widthRaw );
	_varTmpBuffer.fillz( ' '_ycp, 0, _widthRaw );
	COLOR::color_t bg( _focused ? COLOR::BG_GRAY : COLOR::BG_BLACK );
	_attribute = COLOR::combine( COLOR::ATTR_NORMAL, bg );
	for ( int i( 0 ); i < _heightRaw; ++ i ) {
		cons.cmvprintf( _rowRaw + i, _columnRaw, _attribute, _varTmpBuffer );
	}
	if ( ! _contents.is_empty() ) {
		int ctr( 0 ); /* number of text lines in _contents iterated so far */
		int row( 0 ); /* number of lines printed so far */
		int cursor( 0 ); /* end of portion of currently printed line */
		int column( 0 ); /* total length of currently printed line */
		for ( contents_t::iterator it( _contents.begin() ), end( _contents.end() ); ( it != end ) && ( row < _heightRaw ); ++ it ) {
			if ( it->_type == HLogLine::ATTRIBUTE ) {
				_attribute = COLOR::combine( it->_attribute, bg );
			} else {
				if ( ( ctr >= _offsetRow ) && ( cursor < _widthRaw ) ) {
					if ( _offsetColumn > column ) {
						_varTmpBuffer = it->_text.mid( _offsetColumn - column );
					} else {
						_varTmpBuffer = it->_text;
					}
					if ( ( cursor + _varTmpBuffer.get_length() ) > _widthRaw ) {
						_varTmpBuffer.set_at( _widthRaw - cursor, 0_ycp );
					}
					if ( ! _varTmpBuffer.is_empty() ) {
						cons.cmvprintf( _rowRaw + row,
								_columnRaw + cursor, _attribute, _varTmpBuffer );
					}
				} else {
					_varTmpBuffer = "";
				}
				if ( it->_type == HLogLine::TEXT_EOL ) {
					cursor = 0;
					column = 0;
					if ( ctr >= _offsetRow ) {
						++ row;
					}
					++ ctr;
				} else {
					cursor += static_cast<int>( _varTmpBuffer.get_length() );
					column += static_cast<int>( it->_text.get_length() );
				}
			}
		}
	}
	return;
	M_EPILOG
}

void HLogPad::add( HLogLine const& ll_ ) {
	M_PROLOG
	if ( _contents.is_empty() || ( _contents.back()._type == HLogLine::TEXT_EOL ) ) {
		++ _lines;
	}
	_contents.push_back( ll_ );
	return;
	M_EPILOG
}

void HLogPad::add( COLOR::color_t attribute_ ) {
	M_PROLOG
	HLogLine logLine;
	logLine._type = HLogLine::ATTRIBUTE;
	logLine._attribute = attribute_;
	add( logLine );
	return;
	M_EPILOG
}

void HLogPad::add( yaal::hcore::HString const& text_ ) {
	M_PROLOG
	int indexNL( 0 ), indexChar( 0 );
	HLogLine logLine;
	HLogLine* it( nullptr );
	if ( ! _contents.is_empty() ) {
		it = &_contents.tail();
	}
	if ( ! it || ( it->_type != HLogLine::TEXT ) ) {
		it = &logLine;
		it->_type = HLogLine::TEXT;
		it->_text = "";
	}
	_varTmpBuffer = text_;
	while ( ! _varTmpBuffer.is_empty() ) {
		indexNL = static_cast<int>( _varTmpBuffer.find_one_of( "\r\n" ) );
		if ( indexNL >= 0 ) {
			it->_text += _varTmpBuffer.left( indexNL );
			it->_type = HLogLine::TEXT_EOL;
			indexChar = static_cast<int>( _varTmpBuffer.find_other_than( "\r\n", indexNL + 1 ) );
			if ( indexChar >= 0 ) {
				_varTmpBuffer.shift_left( indexChar );
			} else {
				_varTmpBuffer.clear();
			}
		} else {
			it->_text += _varTmpBuffer;
			_varTmpBuffer = "";
		}
		if ( it == &logLine ) {
			add( *it );
		}
		it = &logLine;
		it->_type = HLogLine::TEXT;
		it->_text = "";
	}
	if ( ( _heightRaw > 0 ) && ( _lines > _heightRaw ) ) {
		_offsetRow = _lines - _heightRaw;
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HLogPad::add( COLOR::color_t attribute_, yaal::hcore::HString const& text_ ) {
	M_PROLOG
	add( attribute_ );
	add( text_ );
	return;
	M_EPILOG
}

int HLogPad::do_process_input( int code_ ) {
	M_PROLOG
	int code = 0;
	switch ( code_ ) {
		case ( KEY_CODE::DOWN ): {
			if ( _lines > ( _heightRaw + _offsetRow ) ) {
				++ _offsetRow;
			}
		} break;
		case ( KEY_CODE::UP ): {
			if ( _offsetRow > 0 ) {
				-- _offsetRow;
			}
		} break;
		case ( KEY_CODE::LEFT ): {
			if ( _offsetColumn > 0 ) {
				-- _offsetColumn;
			}
		} break;
		case ( KEY_CODE::RIGHT ): {
			++ _offsetColumn;
			if ( _offsetColumn < 0 ) {
				_offsetColumn = 0;
			}
		} break;
		case ( KEY_CODE::HOME ): {
			_offsetRow = 0;
			_offsetColumn = 0;
		} break;
		case ( KEY_CODE::END ): {
			_offsetColumn = 0;
			if ( _lines > _heightRaw ) {
				_offsetRow = _lines - _heightRaw;
			}
		} break;
		default: {
			code = code_;
		} break;
	}
	if ( ! code ) {
		schedule_repaint();
	}
	return ( code );
	M_EPILOG
}

}

}


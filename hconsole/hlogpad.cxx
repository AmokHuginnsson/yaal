/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlogpad.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlogpad.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HLogPad::HLogLine::HLogLine ( void )
	: _type ( NONE ), _attribute ( COLORS::ATTR_NORMAL ), _text() {
	M_PROLOG
	return;
	M_EPILOG
}

HLogPad::HLogLine::~HLogLine ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HLogPad::HLogPad ( HWindow* parent_, int row_, int column_,
		int height_, int width_, yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget ( parent_, row_, column_, height_, width_, label_ ),
	_lines( 0 ), _offsetRow( 0 ), _offsetColumn( 0 ),
	_attribute( 0 ), _contents() {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HLogPad::~HLogPad( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HLogPad::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	draw_label();
	_varTmpBuffer.reserve( _widthRaw );
	_varTmpBuffer.fillz( ' ', 0, _widthRaw );
	int bg( _focused ? COLORS::BG_GRAY : COLORS::BG_BLACK );
	_attribute = COLORS::ATTR_NORMAL | bg;
	for ( int i( 0 ); i < _heightRaw; ++ i )
		cons.cmvprintf( _rowRaw + i, _columnRaw, _attribute, _varTmpBuffer.raw() );
	if ( ! _contents.is_empty() ) {
		int ctr( 0 ); /* number of text lines in _contents iterated so far */
		int row( 0 ); /* number of lines printed so far */
		int cursor( 0 ); /* end of portion of currently printed line */
		int column( 0 ); /* total length of currently printed line */
		for ( contents_t::iterator it( _contents.begin() ), end( _contents.end() ); ( it != end ) && ( row < _heightRaw ); ++ it ) {
			if ( it->_type == HLogLine::ATTRIBUTE )
				_attribute = it->_attribute | bg;
			else {
				if ( ( ctr >= _offsetRow ) && ( cursor < _widthRaw ) ) {
					if ( _offsetColumn > column )
						_varTmpBuffer = it->_text.mid( _offsetColumn - column );
					else
						_varTmpBuffer = it->_text;
					if ( ( cursor + _varTmpBuffer.get_length() ) > _widthRaw )
						_varTmpBuffer.set_at( _widthRaw - cursor, 0 );
					if ( _varTmpBuffer[ 0 ] )
						cons.cmvprintf( _rowRaw + row,
								_columnRaw + cursor, _attribute, _varTmpBuffer.raw() );
				} else
					_varTmpBuffer = "";
				if ( it->_type == HLogLine::TEXT_EOL ) {
					cursor = 0;
					column = 0;
					if ( ctr >= _offsetRow )
						++ row;
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

void HLogPad::add( int attribute_ ) {
	M_PROLOG
	HLogLine logLine;
	logLine._type = HLogLine::ATTRIBUTE;
	logLine._attribute = attribute_;
	_contents.push_back( logLine );
	return;
	M_EPILOG
}

void HLogPad::add( yaal::hcore::HString const& text_ ) {
	M_PROLOG
	int indexNL( 0 ), indexChar( 0 );
	HLogLine logLine;
	HLogLine* it( NULL );
	if ( ! _contents.is_empty() )
		it = &_contents.tail();
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
			++ _lines;
			indexChar = static_cast<int>( _varTmpBuffer.find_other_than( "\r\n", indexNL + 1 ) );
			if ( indexChar >= 0 )
				_varTmpBuffer = _varTmpBuffer.mid( indexChar );
			else
				_varTmpBuffer = "";
		} else {
			it->_text += _varTmpBuffer;
			_varTmpBuffer = "";
		}
		if ( it == &logLine )
			_contents.push_back( *it );
		it = &logLine;
		it->_type = HLogLine::TEXT;
		it->_text = "";
	}
	if ( _lines > _heightRaw )
		_offsetRow = _lines - _heightRaw;
	schedule_repaint();
	return;
	M_EPILOG
}

void HLogPad::add( int attribute_, yaal::hcore::HString const& text_ ) {
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
		case ( KEY_CODES::DOWN ):
			if ( _lines > ( _heightRaw + _offsetRow ) )
				_offsetRow ++;
		break;
		case ( KEY_CODES::UP ):
			if ( _offsetRow > 0 )
				_offsetRow --;
		break;
		case ( KEY_CODES::LEFT ):
			if ( _offsetColumn > 0 )
				_offsetColumn --;
		break;
		case ( KEY_CODES::RIGHT ):
			_offsetColumn ++;
			if ( _offsetColumn < 0 )
				_offsetColumn = 0;
		break;
		case ( KEY_CODES::HOME ):
			_offsetRow = 0;
			_offsetColumn = 0;
		break;
		case ( KEY_CODES::END ):
			_offsetColumn = 0;
			if ( _lines > _heightRaw )
				_offsetRow = _lines - _heightRaw;
		break;
		default :
			code = code_;
		break;
	}
	if ( ! code )
		schedule_repaint();
	return ( code );
	M_EPILOG
}

}

}


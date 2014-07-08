/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstatusbarcontrol.cxx - this file is integral part of `yaal' project.

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
#include <libintl.h>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstatusbarcontrol.hxx"
#include "hwindow.hxx"
#include "hconsole.hxx"
#include "hsearchablecontrol.hxx"
#include "hcore/foreach.hxx"

//#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
//#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HStatusBarControl::HStatusBarControl( HWindow* parent_,
		char const* label_, int statusBarAttribute_ )
	: HControl( parent_, - 2, 0, 2, - 1, label_ ),
	HEditControl( NULL, 0, 0, 0, 0, NULL,
			HEditControlAttrubites().pattern( _maskLoose_ ).max_history_level( 255 ).label_decoration( HControl::LABEL::DECORATION::EXPLICIT ) ),
	_statusBarAttribute( 0 ), _promptLength( 0 ),
	_mode( PROMPT::NORMAL ),
	_prompt(), _done( false ), _estimate( false ), _progressSize( 1 ),
	_lastProgress( -1 ), _lastPercent( - 1 ), _lastMinute( 0 ),
	_lastSecond( 0 ), _lastStep( 0 ),
	_message( "" ), /* initialization of this field is required by bar() meth */
	_start( HTime::LOCAL ), _choices() {
	M_PROLOG
	int attribte = 0;
	if ( statusBarAttribute_ > 0 )
		_statusBarAttribute = statusBarAttribute_;
	else
		_statusBarAttribute = _attributeStatusBar_;
	attribte = _statusBarAttribute;
	attribte &= 0x00ff;
	_attributeFocused._data = attribte;
	_statusBarAttribute &= 0xff00;
	return;
	M_EPILOG
}

HStatusBarControl::~HStatusBarControl( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HStatusBarControl::do_draw_label( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	cons.move( cons.get_height() - 2, 0 );
	cons.clrtoeol();
	HControl::do_draw_label();
	bar();
	_columnRaw += _promptLength;
	_widthRaw -= _promptLength;
	cons.move( _rowRaw, _promptLength );
	return;
	M_EPILOG
}

void HStatusBarControl::do_paint( void ) {
	M_PROLOG
	int origRow = 0;
	int origColumn = 0;
	HConsole& cons = HConsole::get_instance();
	if ( ! _focused )
		cons.getyx( origRow, origColumn );
	if ( _promptLength ) {
		cons.set_attr( _statusBarAttribute >> 8 );
		cons.mvprintf( _rowRaw, 0, _prompt.raw() );
	}
	HEditControl::do_paint();
	if ( ! _focused )
		cons.move( origRow, origColumn );
	if ( _statusBarAttribute & 0xff ) {
		_attributeEnabled._data = ( _statusBarAttribute & 0x00ff );
		_statusBarAttribute &= 0xff00;
	}
	return;
	M_EPILOG
}

int HStatusBarControl::do_process_input( int code_ ) {
	M_PROLOG
	if ( ( code_ == KEY_CODES::BACKSPACE )
			&& ( _mode != PROMPT::MENU ) && ( _mode != PROMPT::DIALOG )
			&& ! _string.get_length() ) {
		end_prompt();
		return ( 0 );
	}
	if ( code_ != '\t' ) {
		code_ = HEditControl::do_process_input( code_ );
	}
	switch ( _mode ) {
		case ( PROMPT::NORMAL ):
		case ( PROMPT::COMMAND ):
		case ( PROMPT::SEARCH ):
			code_ = process_input_normal( code_ );
		break;
		case ( PROMPT::DIALOG ):
			process_input_normal( code_ );
			code_ = 0;
		break;
		case ( PROMPT::MENU ):
			code_ = process_input_menu( code_ );
		break;
		default :
			break;
	}
	return ( code_ );
	M_EPILOG
}

void HStatusBarControl::set_prompt( yaal::hcore::HString const& prompt_, PROMPT::mode_t mode_ ) {
	M_PROLOG
	_mode = mode_;
	_window->_previousFocusedChild = _window->_focusedChild;
	_window->_controls.select( _window->_statusBar );
	(*_window->_previousFocusedChild)->kill_focus();
	set_focus( -1 );
	if ( !prompt_.is_empty() ) {
		_prompt = prompt_;
		_promptLength = static_cast<int>( _prompt.get_length() );
	}
	set_text( "" ); /* paint call inside */
	return;
	M_EPILOG
}

int HStatusBarControl::verify( void ) {
	M_PROLOG
	int length = 0;
	bool ok = false;
	if ( length < _promptLength )
		ok = true;
	return ( ok );
	M_EPILOG
}

void HStatusBarControl::init_progress( double max_, char const* title_,
		bool estimate_ ) {
	M_PROLOG
	_done = false;
	_estimate = estimate_;
	_progressSize = ( max_ > 0 ? max_ : 1 );
	_lastProgress = - 1;
	_lastPercent = - 1;
	_lastStep = 0;
	_message = title_;
	_start.set_now( HTime::LOCAL );
	paint();
	return;
	M_EPILOG
}

void HStatusBarControl::update_progress( double step_, char const* title_ ) {
	M_PROLOG
	HTime now( HTime::LOCAL ), left( HTime::LOCAL );
	HConsole& cons( HConsole::get_instance() );
	now.set_format( "(%T)" );
	if ( _done )
		return;
	if ( step_ < 0 )
		step_ = ++ _lastStep;
	HTime stoper( now - _start );
	if ( _estimate ) {
		if ( step_ != 0.0 )
			now.set( static_cast<time_t>( ( _progressSize / step_ ) * static_cast<double>( stoper.raw() ) ) );
		left = now - stoper;
	}
	/* 6 for "[100%]", 10 for elapse, 10 for estimate, 2 for || */
	int maxBar( _widthRaw - ( 6 + 10 + 2 + ( _estimate ? 10 : 0 ) ) );
	int nextPercent( static_cast<int>( ( 100. * step_ / _progressSize ) ) );
	int nextStep( static_cast<int>( ( maxBar * step_ / _progressSize ) ) );
	int nextMinute( stoper.get_minute() );
	int nextSecond( stoper.get_second() );
	if ( nextStep >= maxBar )
		nextStep = maxBar, _done = true;
	if ( nextPercent >= 100 )
		nextPercent = 100, _done = true;
	if ( title_ || ( _lastProgress != nextStep )
			|| ( _lastPercent != nextPercent )
			|| ( _lastMinute != nextMinute )
			|| ( _lastSecond != nextSecond )) {
		schedule_repaint();
		bar( title_ );
		if ( _estimate ) {
			_varTmpBuffer.format ( "|%%-%ds|%%s%%s[%%3d%%s]", maxBar );
			_string.format( _varTmpBuffer.raw(), "-",
					( step_ != 0.0 ? ( left.string().raw() ) : "(?\?:?\?:?\?)" ),
					stoper.string().raw(),	nextPercent, "%%" );
		} else {
			_varTmpBuffer.format ( "|%%-%ds|%%s[%%3d%%s]", maxBar );
			_string.format( _varTmpBuffer.raw(), "-",
					stoper.string().raw(), nextPercent, "%%" );
		}
		_string.fill( '-', 1, maxBar );
		if ( nextStep > 0 )
			_string.fill( '=', 1, nextStep );
		if ( _done ) {
			int length = static_cast<int>( _string.get_length() );
			_string.erase( length - 6, 5 );
			_string.insert( length - 6, 4, "done" );
		}
		cons.mvprintf( _rowRaw, _columnRaw, _string.raw() );
		_string = "";
		_lastProgress = nextStep;
		_lastPercent = nextPercent;
		_lastMinute = nextMinute;
		_lastSecond = nextSecond;
		cons.refresh();
	}
	return;
	M_EPILOG
}

void HStatusBarControl::message( int attribute_, char const* format_, ... ) {
	M_PROLOG
	if ( ! _focused ) {
		if ( format_ ) {
			va_list ap;
			va_start( ap, format_ );
			_varTmpBuffer.vformat( format_, &ap );
			va_end( ap );
			if ( ! _varTmpBuffer.is_empty() )
				HConsole::get_instance().bell();
		} else
			_varTmpBuffer.clear();
		set_text( _varTmpBuffer );
		if ( ! ( _statusBarAttribute & 0x00ff ) )
			_statusBarAttribute |= _attributeEnabled._data;
		_attributeEnabled._data = ( attribute_ & 0x00ff );
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarControl::message( char const* format_, ... ) {
	M_PROLOG
	if ( ! _focused ) {
		if ( format_ ) {
			va_list ap;
			va_start( ap, format_ );
			_varTmpBuffer.vformat( format_, &ap );
			va_end( ap );
			if ( ! _varTmpBuffer.is_empty() )
				HConsole::get_instance().bell();
		} else
			_varTmpBuffer.clear();
		set_text( _varTmpBuffer );
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarControl::clear( int attribute_ ) {
	M_PROLOG
	if ( ! _focused ) {
		_varTmpBuffer.clear();
		set_text( _varTmpBuffer );
		if ( ! ( _statusBarAttribute & 0x00ff ) )
			_statusBarAttribute |= _attributeEnabled._data;
		_attributeEnabled._data = ( attribute_ & 0x00ff );
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarControl::bar( char const* bar_ ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	set_attr_data();
	if ( bar_ ) {
		_varTmpBuffer.format( " %%-%ds ",
				( cons.get_width() - _labelLength ) - ( _labelPosition == LABEL::POSITION::SIDE_BY_SIDE ? 2 : 1 ) );
		_message.format( _varTmpBuffer.raw(), bar_ );
	}
	cons.mvprintf( cons.get_height() - 2,
			_labelLength + ( _labelPosition == LABEL::POSITION::SIDE_BY_SIDE ? 1 : 0 ), _message.raw() );
	return;
	M_EPILOG
}

void HStatusBarControl::ask( char const* question_,
		choices_t const& choices_ ) {
	M_PROLOG
	_choices = choices_;
	bar( question_ );
	HString prompt( "[" );
	bool first( true );
	YAAL_FOREACH( choice_t const& c, _choices ) {
		if ( ! first ) {
			prompt += "/";
		} else {
			first = false;
		}
		prompt += c.first;
	}
	prompt += "] ";
	set_prompt( prompt, PROMPT::DIALOG );
	return;
	M_EPILOG
}

void HStatusBarControl::confirm( char const* question_,
		HTUIProcess::call_t yes_, HTUIProcess::call_t no_ ) {
	M_PROLOG
	choices_t choices;
	choices.push_back( make_pair<HString>( "yes", yes_ ) );
	choices.push_back( make_pair<HString>( "no", no_ ) );
	ask( question_, choices );
	return;
	M_EPILOG
}

int HStatusBarControl::process_input_normal( int code_ ) {
	M_PROLOG
	int code = code_;
	PROMPT::mode_t mode = _mode;
	code_ = 0;
	switch ( code ) {
		case ( '\r' ): {
			bool backwards( _prompt[ 0 ] == '?' );
			end_prompt();
			if ( mode == PROMPT::COMMAND )
				_window->_command = _string;
			else if ( mode == PROMPT::SEARCH ) {
				HSearchableControl* searchableControl( dynamic_cast<HSearchableControl*>( &(*(*_window->_previousFocusedChild)) ) );
				if ( searchableControl )
					searchableControl->search( _string, backwards );
			}
		}
		break;
		case ( '\t' ):
			HConsole::get_instance().bell();
		break;
		default :
			code_ = code;
		break;
	}
	return ( code_ );
	M_EPILOG
}

int HStatusBarControl::process_input_menu( int code_ ) {
	M_PROLOG
	return ( code_ );
	M_EPILOG
}

void HStatusBarControl::end_prompt( void ) {
	M_PROLOG
	_mode = PROMPT::NORMAL;
	_prompt = "";
	_promptLength = 0;
	_window->_focusedChild = _window->_previousFocusedChild;
	_window->_statusBar->kill_focus();
	(*_window->_focusedChild)->set_focus ( -1 );
	return;
	M_EPILOG
}

bool HStatusBarControl::do_hit_test( int row_, int col_ ) const {
	M_PROLOG
	return ( _focused ? base_type::do_hit_test( row_, col_ ) : false );
	M_EPILOG
}

}

}


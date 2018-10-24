/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <libintl.h>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstatusbarwidget.hxx"
#include "hwindow.hxx"
#include "hconsole.hxx"
#include "hsearchablewidget.hxx"
#include "hcore/hformat.hxx"
#include "hcore/hlog.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HStatusBarWidget::HStatusBarWidget( HWindow* parent_,
		yaal::hcore::HString const& label_, HWidget::OAttribute const& statusBarAttribute_ )
	: HWidget( parent_, - 2, 0, 2, - 1, label_ )
	, HEditWidget( nullptr, 0, 0, 0, 0, HString(),
			HEditWidgetAttributes().mask( _maskLoose_ ).max_history_level( 255 ).label_decoration( HWidget::LABEL::DECORATION::EXPLICIT ) )
	, _statusBarAttribute( statusBarAttribute_ )
	, _promptLength( 0 )
	, _mode( PROMPT::NORMAL )
	, _prompt()
	, _done( false )
	, _estimate( false )
	, _progressSize( 1 )
	, _lastProgress( -1 )
	, _lastPercent( - 1 )
	, _lastMinute( 0 )
	, _lastSecond( 0 )
	, _lastStep( 0 )
	, _currentChoice( -1 )
	, _message( "" ) /* initialization of this field is required by bar() meth */
	, _start( HTime::TZ::LOCAL )
	, _choices() {
	M_PROLOG
	_attributeFocused._data = _statusBarAttribute._data;
	_statusBarAttribute._data = COLOR::ATTR_DEFAULT;
	return;
	M_EPILOG
}

HStatusBarWidget::~HStatusBarWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HStatusBarWidget::do_draw_label( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	cons.move( cons.get_height() - 2, 0 );
	cons.clrtoeol();
	HWidget::do_draw_label();
	bar();
	_columnRaw += _promptLength;
	_widthRaw -= _promptLength;
	cons.move( _rowRaw, _promptLength );
	return;
	M_EPILOG
}

void HStatusBarWidget::do_paint( void ) {
	M_PROLOG
	int origRow = 0;
	int origColumn = 0;
	HConsole& cons = HConsole::get_instance();
	if ( ! _focused )
		cons.getyx( origRow, origColumn );
	if ( _promptLength ) {
		cons.set_attr( _statusBarAttribute._label );
		cons.mvprintf( _rowRaw, 0, _prompt );
	}
	if ( _mode != PROMPT::MENU ) {
		HEditWidget::do_paint();
	} else {
		int colRaw( _columnRaw );
		int current( 0 );
		for ( choice_t const& c : _choices ) {
			if ( current == _currentChoice ) {
				cons.set_attr( _statusBarAttribute._label );
			} else {
				cons.set_attr( COLOR::FG_LIGHTGRAY );
			}
			cons.mvprintf( _rowRaw, colRaw, "[%s] ", c.first );
			colRaw += ( static_cast<int>( c.first.get_length() ) + 3 );
			++ current;
		}
	}
	if ( ! _focused ) {
		cons.move( origRow, origColumn );
	}
	if ( _statusBarAttribute._data != COLOR::ATTR_DEFAULT ) {
		_attributeEnabled._data = _statusBarAttribute._data;
		_statusBarAttribute._data = COLOR::ATTR_DEFAULT;
	}
	return;
	M_EPILOG
}

int HStatusBarWidget::do_process_input( int code_ ) {
	M_PROLOG
	if ( ( code_ == KEY_CODE::BACKSPACE )
			&& ( _mode != PROMPT::MENU ) && ( _mode != PROMPT::DIALOG )
			&& ! _string.get_length() ) {
		end_prompt();
		return ( 0 );
	}
	if ( code_ != '\t' ) {
		code_ = HEditWidget::do_process_input( code_ );
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

void HStatusBarWidget::set_prompt( yaal::hcore::HString const& prompt_, PROMPT mode_ ) {
	M_PROLOG
	_mode = mode_;
	_window->_previousFocusedChild = _window->_focusedChild;
	_window->_widgets.select( _window->_statusBar );
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

void HStatusBarWidget::init_progress( double max_, char const* title_,
		bool estimate_ ) {
	M_PROLOG
	_done = false;
	_estimate = estimate_;
	_progressSize = ( max_ > 0 ? max_ : 1 );
	_lastProgress = - 1;
	_lastPercent = - 1;
	_lastStep = 0;
	_message = title_;
	_start.set_now();
	paint();
	return;
	M_EPILOG
}

void HStatusBarWidget::update_progress( double step_, char const* title_ ) {
	M_PROLOG
	HTime now( HTime::TZ::LOCAL ), left( HTime::TZ::LOCAL );
	HConsole& cons( HConsole::get_instance() );
	now.set_format( "(%T)" );
	if ( _done )
		return;
	if ( step_ < 0 )
		step_ = ++ _lastStep;
	HTime stopper( now - _start );
	if ( _estimate ) {
		if ( step_ != 0.0 )
			now.set( static_cast<time_t>( ( _progressSize / step_ ) * static_cast<double>( stopper.raw() ) ) );
		left = now - stopper;
	}
	/* 6 for "[100%]", 10 for elapse, 10 for estimate, 2 for || */
	int maxBar( _widthRaw - ( 6 + 10 + 2 + ( _estimate ? 10 : 0 ) ) );
	int nextPercent( static_cast<int>( ( 100. * step_ / _progressSize ) ) );
	int nextStep( static_cast<int>( ( maxBar * step_ / _progressSize ) ) );
	int nextMinute( stopper.get_minute() );
	int nextSecond( stopper.get_second() );
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
			_varTmpBuffer = format( "|%%-%ds|%%s%%s[%%3d%%s]", maxBar );
			_string = do_format( _varTmpBuffer, "-",
					( step_ != 0.0 ? ( left.string() ) : "(?\?:?\?:?\?)" ),
					stopper.string(), nextPercent, "%%" );
		} else {
			_varTmpBuffer = format( "|%%-%ds|%%s[%%3d%%s]", maxBar );
			_string = do_format( _varTmpBuffer, "-", stopper.string(), nextPercent, "%%" );
		}
		_string.fill( '-'_ycp, 1, maxBar );
		if ( nextStep > 0 )
			_string.fill( '='_ycp, 1, nextStep );
		if ( _done ) {
			int length = static_cast<int>( _string.get_length() );
			_string.erase( length - 6, 5 );
			_string.insert( length - 6, "done", 4 );
		}
		cons.mvprintf( _rowRaw, _columnRaw, _string );
		_string.clear();
		_lastProgress = nextStep;
		_lastPercent = nextPercent;
		_lastMinute = nextMinute;
		_lastSecond = nextSecond;
		cons.refresh();
	}
	return;
	M_EPILOG
}

void HStatusBarWidget::do_message( COLOR::color_t attribute_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	if ( ! _focused ) {
		_varTmpBuffer = message_;
		if ( ! _varTmpBuffer.is_empty() ) {
			HConsole::get_instance().bell();
		}
		set_text( _varTmpBuffer );
		if ( _statusBarAttribute._data == COLOR::ATTR_DEFAULT ) {
			_statusBarAttribute._data = _attributeEnabled._data;
		}
		_attributeEnabled._data = attribute_;
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarWidget::do_message( yaal::hcore::HString const& message_ ) {
	M_PROLOG
	if ( ! _focused ) {
		_varTmpBuffer = message_;
		if ( ! _varTmpBuffer.is_empty() ) {
			HConsole::get_instance().bell();
		}
		set_text( _varTmpBuffer );
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarWidget::clear( COLOR::color_t attribute_ ) {
	M_PROLOG
	if ( ! _focused ) {
		_varTmpBuffer.clear();
		set_text( _varTmpBuffer );
		if ( _statusBarAttribute._data == COLOR::ATTR_DEFAULT ) {
			_statusBarAttribute._data = _attributeEnabled._data;
		}
		_attributeEnabled._data = attribute_;
		schedule_repaint();
	}
	return;
	M_EPILOG
}

void HStatusBarWidget::bar( char const* bar_ ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	set_attr_data();
	if ( bar_ ) {
		_message = format( "%-*s", cons.get_width() - ( _labelLength + 1 ), bar_ );
	}
	cons.mvprintf( cons.get_height() - 2, _labelLength, _message );
	return;
	M_EPILOG
}

void HStatusBarWidget::ask( char const* question_,
		choices_t const& choices_, int default_ ) {
	M_PROLOG
	_choices = choices_;
	_currentChoice = default_;
	M_ENSURE( default_ < _choices.get_size() );
	PROMPT mode( PROMPT::NORMAL );
	if ( _currentChoice >= 0 ) {
		mode = PROMPT::MENU;
	} else {
		mode = PROMPT::DIALOG;
	}
	bar( question_ );
	HString prompt( "[" );
	bool first( true );
	for ( choice_t const& c : _choices ) {
		if ( ! first ) {
			prompt += "/";
		} else {
			first = false;
		}
		prompt += c.first;
	}
	prompt += "] ";
	set_prompt( prompt, mode );
	return;
	M_EPILOG
}

bool HStatusBarWidget::dialog( yaal::hcore::HString const& answer_ ) {
	M_PROLOG
	M_ENSURE( ( _mode == PROMPT::DIALOG ) || ( _mode == PROMPT::MENU ) );
	bool found( false );
	for  ( choice_t const& c : _choices ) {
		if ( c.first == answer_ ) {
			found = true;
			_window->schedule_call( c.second );
			break;
		}
	}
	return ( found );
	M_EPILOG
}

void HStatusBarWidget::confirm( char const* question_,
		HTUIProcess::call_t yes_, HTUIProcess::call_t no_ ) {
	M_PROLOG
	choices_t choices;
	choices.push_back( make_pair( "yes", yes_ ) );
	choices.push_back( make_pair( "no", no_ ) );
	ask( question_, choices, 1 );
	return;
	M_EPILOG
}

int HStatusBarWidget::process_input_normal( int code_ ) {
	M_PROLOG
	int code = code_;
	code_ = 0;
	switch ( code ) {
		case ( '\r' ): {
			HSearchableWidget* searchableControl( nullptr );
			bool backwards( false );
			if ( _mode == PROMPT::COMMAND ) {
				_window->_command = _string;
			} else if ( _mode == PROMPT::SEARCH ) {
				backwards = _prompt[ 0 ] == '?';
				searchableControl = dynamic_cast<HSearchableWidget*>( &(*(*_window->_previousFocusedChild)) );
			} else if ( _mode == PROMPT::DIALOG ) {
				if ( ! dialog( _string ) ) {
					break;
				}
			}
			end_prompt();
			if ( searchableControl ) {
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

int HStatusBarWidget::process_input_menu( int code_ ) {
	M_PROLOG
	M_ASSERT( _mode == PROMPT::MENU );
	switch ( code_ ) {
		case ( KEY_CODE::LEFT ): {
			-- _currentChoice;
			if ( _currentChoice < 0 ) {
				_currentChoice = static_cast<int>( _choices.get_size() - 1 );
			}
			schedule_repaint();
		} break;
		case ( KEY_CODE::RIGHT ): {
			++ _currentChoice;
			_currentChoice %= static_cast<int>( _choices.get_size() );
			schedule_repaint();
		} break;
		case ( '\r' ): {
			dialog( _choices[_currentChoice].first );
			end_prompt();
		}
	}
	return ( 0 );
	M_EPILOG
}

void HStatusBarWidget::end_prompt( void ) {
	M_PROLOG
	_mode = PROMPT::NORMAL;
	_prompt = "";
	_promptLength = 0;
	_window->_focusedChild = _window->_previousFocusedChild;
	_window->_statusBar->kill_focus();
	(*_window->_focusedChild)->set_focus( -1 );
	return;
	M_EPILOG
}

bool HStatusBarWidget::do_hit_test( int row_, int col_ ) const {
	M_PROLOG
	return ( _focused ? base_type::do_hit_test( row_, col_ ) : false );
	M_EPILOG
}

}

}


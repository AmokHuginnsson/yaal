/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstatusbarcontrol.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <libintl.h>
#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hstatusbarcontrol.hxx"
#include "hwindow.hxx"
#include "hconsole.hxx"
#include "hsearchablecontrol.hxx"

//#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
//#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HStatusBarControl::HStatusBarControl( HWindow* parent_,
		char const* label_, int statusBarAttribute_ )
	: HControl( parent_, - 2, 0, 2, - 1, label_ ),
	HEditControl( NULL, 0, 0, 0, 0, NULL, 127, "", _maskLoose_,
			false, false, false, false, false, 255 ),
	_statusBarAttribute( 0 ), _promptLength( 0 ),
	_mode( PROMPT::NORMAL ), _restrict( PROMPT::RELAXED ),
	_prompt(), _done( false ), _estimate( false ), _progressSize( 1 ),
	_lastProgress( -1 ), _lastPercent( - 1 ), _lastMinute( 0 ),
	_lastSecond( 0 ), _lastStep( 0 ),
	_message( "" ), /* initialization of this field is required by bar() meth */
	_start()
	{
	M_PROLOG
	int attribte = 0;
	if ( statusBarAttribute_ > 0 )
		_statusBarAttribute = statusBarAttribute_;
	else
		_statusBarAttribute = _attributeStatusBar_;
	attribte = _statusBarAttribute;
	attribte &= 0x00ff;
	_attributeFocused &= 0xff00;
	_attributeFocused |= attribte;
	_statusBarAttribute &= 0xff00;
	return;
	M_EPILOG
	}

HStatusBarControl::~HStatusBarControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HStatusBarControl::do_draw_label( void )
	{
	M_PROLOG
	HConsole& cons = HCons::get_instance();
	cons.c_move( cons.get_height() - 2, 0 );
	cons.c_clrtoeol();
	HControl::do_draw_label();
	bar();
	_columnRaw += _promptLength;
	_widthRaw -= _promptLength;
	cons.c_move( _rowRaw, _promptLength );
	return;
	M_EPILOG
	}

void HStatusBarControl::do_refresh( void )
	{
	M_PROLOG
	int origRow = 0;
	int origColumn = 0;
	HConsole& cons = HCons::get_instance();
	if ( ! _focused )
		cons.c_getyx( origRow, origColumn );
	if ( _promptLength )
		{
		cons.set_attr( _statusBarAttribute >> 8 );
		cons.c_mvprintf( _rowRaw, 0, _prompt.raw() );
		}
	HEditControl::do_refresh();
	if ( ! _focused )
		cons.c_move ( origRow, origColumn );
	if ( _statusBarAttribute & 0xff )
		{
		_attributeEnabled &= 0xff00;
		_attributeEnabled |= ( _statusBarAttribute & 0x00ff );
		_statusBarAttribute &= 0xff00;
		}
	return;
	M_EPILOG
	}

int HStatusBarControl::do_process_input( int code_ )
	{
	M_PROLOG
	if ( ( code_ == KEY_CODES::BACKSPACE )
			&& ( _restrict == PROMPT::RELAXED )
			&& ( _mode != PROMPT::MENU )
			&& ! _string.get_length() )
		{
		end_prompt();
		return ( 0 );
		}
	if ( code_ != '\t' )
		code_ = HEditControl::do_process_input ( code_ );
	switch ( _mode )
		{
		case ( PROMPT::NORMAL ):
		case ( PROMPT::COMMAND ):
		case ( PROMPT::SEARCH ):
			code_ = process_input_normal ( code_ );
		break;
		case ( PROMPT::MENU ):
			code_ = process_input_menu ( code_ );
		break;
		default :
			break;
		}
	return ( code_ );
	M_EPILOG
	}

void HStatusBarControl::set_prompt( char const* prompt_, PROMPT::mode_t mode_,
		PROMPT::restrict_t restrict_ )
	{
	M_PROLOG
	_mode = mode_;
	_restrict = restrict_;
	_parent->_previousFocusedChild = _parent->_focusedChild;
	_parent->_controls.select( _parent->_statusBar );
	(*_parent->_previousFocusedChild)->kill_focus();
	set_focus ( -1 );
	if ( prompt_ )
		{
		_prompt = prompt_;
		_promptLength = static_cast<int>( _prompt.get_length() );
		}
	HEditControl::set ( "" ); /* refresh call inside */
	return;
	M_EPILOG
	}

int HStatusBarControl::verify( void )
	{
	M_PROLOG
	int length = 0;
	bool ok = false;
	if ( length < _promptLength )
		ok = true;
	return ( ok );
	M_EPILOG
	}

void HStatusBarControl::init_progress( double max_, char const* title_,
		bool estimate_ )
	{
	M_PROLOG
	_done = false;
	_estimate = estimate_;
	_progressSize = max_;
	_lastProgress = - 1;
	_lastPercent = - 1;
	_lastStep = 0;
	_message = title_;
	_start.set_now();
	refresh();
	return;
	M_EPILOG
	}

void HStatusBarControl::update_progress( double step_,
		char const * title_ )
	{
	M_PROLOG
	int maxBar = 0;
	int nextStep = 0;
	int nextPercent = 0;
	int nextMinute = 0;
	int nextSecond = 0;
	HTime stoper, now, left;
	HConsole& cons = HCons::get_instance();
	now.format ( "(%T)" );
	if ( _done )
		return;
	if ( step_ < 0 )
		step_ = ++ _lastStep;
	stoper = now - _start;
	if ( _estimate )
		{
		if ( step_ )
			now.set( static_cast<time_t>( ( _progressSize / step_ ) * static_cast<double>( stoper ) ) );
		left = now - stoper;
		}
	/* 6 for "[100%]", 10 for elapse, 10 for estimate, 2 for || */
	maxBar = _widthRaw - ( 6 + 10 + 2 + ( _estimate ? 10 : 0 ) );
	nextPercent = static_cast<int>( ( 100. * step_ / _progressSize ) );
	nextStep = static_cast<int>( ( maxBar * step_ / _progressSize ) );
	nextMinute = stoper.get_minute();
	nextSecond = stoper.get_second();
	if ( nextStep >= maxBar )
		nextStep = maxBar, _done = true;
	if ( nextPercent >= 100 )
		nextPercent = 100, _done = true;
	if ( title_ || ( _lastProgress != nextStep )
			|| ( _lastPercent != nextPercent )
			|| ( _lastMinute != nextMinute )
			|| ( _lastSecond != nextSecond ))
		{
		schedule_refresh();
		bar ( title_ );
		if ( _estimate )
			{
			_varTmpBuffer.format ( "|%%-%ds|%%s%%s[%%3d%%s]", maxBar );
			_string.format( _varTmpBuffer.raw(), "-",
					( step_ ? ( static_cast<char const *>( left ) ) : "(?\?:?\?:?\?)" ),
					static_cast<char const *>( stoper ),	nextPercent, "%%" );
			}
		else
			{
			_varTmpBuffer.format ( "|%%-%ds|%%s[%%3d%%s]", maxBar );
			_string.format( _varTmpBuffer.raw(), "-",
					static_cast<char const*>( stoper ), nextPercent, "%%" );
			}
		_string.fill( '-', 1, maxBar );
		if ( nextStep > 0 )
			_string.fill( '=', 1, nextStep );
		if ( _done )
			{
			int length = static_cast<int>( _string.get_length() );
			_string.erase( length - 6, 5 );
			_string.insert( length - 6, 4, "done" );
			}
		cons.c_mvprintf( _rowRaw, _columnRaw, _string.raw() );
		_string = "";
		_lastProgress = nextStep;
		_lastPercent = nextPercent;
		_lastMinute = nextMinute;
		_lastSecond = nextSecond;
		cons.c_refresh();
		}
	return;
	M_EPILOG
	}

void HStatusBarControl::message( int attribute_,
		char const* format_, ... )
	{
	M_PROLOG
	if ( ! _focused )
		{
		va_list ap;
		va_start( ap, format_ );
		if ( format_ && format_ [ 0 ] )
			HCons::get_instance().bell();
		_varTmpBuffer.vformat( format_, &ap );
		set( _varTmpBuffer );
		va_end( ap );
		if ( ! ( _statusBarAttribute & 0x00ff ) )
			_statusBarAttribute |= ( _attributeEnabled & 0x00ff );
		_attributeEnabled &= 0xff00;
		_attributeEnabled |= ( attribute_ & 0x00ff );
		schedule_refresh();
		}
	return;
	M_EPILOG
	}

void HStatusBarControl::message( char const* format_, ... )
	{
	M_PROLOG
	if ( ! _focused )
		{
		va_list ap;
		va_start( ap, format_ );
		if ( format_ && format_ [ 0 ] )
			HCons::get_instance().bell();
		_varTmpBuffer.vformat( format_, &ap );
		set( _varTmpBuffer );
		va_end( ap );
		schedule_refresh();
		}
	return;
	M_EPILOG
	}

void HStatusBarControl::bar( char const* bar_ )
	{
	M_PROLOG
	HConsole& cons = HCons::get_instance();
	set_attr_data();
	if ( bar_ )
		{
		_varTmpBuffer.format ( " %%-%ds ",
				( cons.get_width() - _labelLength ) - ( _singleLine ? 2 : 1 ) );
		_message.format( _varTmpBuffer.raw(), bar_ );
		}
	cons.c_mvprintf( cons.get_height() - 2,
			_labelLength - ( _singleLine ? 0 : 1 ), _message.raw() );
	return;
	M_EPILOG
	}

int HStatusBarControl::ask( char const* question_,
		char const* prompt_ )
	{
	M_PROLOG
	bar ( question_ );
	set_prompt ( prompt_ );
	return ( 0 );
	M_EPILOG
	}

bool HStatusBarControl::confirm( char const* question_ )
	{
	M_PROLOG
	ask ( question_, "[yes/no]: " );
	return ( false );
	M_EPILOG
	}

int HStatusBarControl::process_input_normal( int code_ )
	{
	M_PROLOG
	bool backwards = false;
	int code = code_;
	PROMPT::mode_t mode = _mode;
	HSearchableControl * searchableControl = NULL;
	code_ = 0;
	switch ( code )
		{
		case ( '\r' ):
			{
			backwards = ( _prompt [ 0 ] == '?' );
			end_prompt();
			if ( mode == PROMPT::COMMAND )
				_parent->_command = _string;
			else if ( mode == PROMPT::SEARCH )
				{
				searchableControl = dynamic_cast < HSearchableControl * > ( &(*(*_parent->_previousFocusedChild)) );
				if ( searchableControl )
					searchableControl->search ( _string, backwards );
				}
			}
		break;
		case ( '\t' ):
		HCons::get_instance().bell();
		break;
		default :
			code_ = code;
		}
	return ( code_ );
	M_EPILOG
	}

int HStatusBarControl::process_input_menu ( int code_ )
	{
	M_PROLOG
	return ( code_ );
	M_EPILOG
	}

void HStatusBarControl::end_prompt( void )
	{
	M_PROLOG
	_mode = PROMPT::NORMAL;
	_prompt = "";
	_promptLength = 0;
	_parent->_focusedChild = _parent->_previousFocusedChild;
	_parent->_statusBar->kill_focus();
	(*_parent->_focusedChild)->set_focus ( -1 );
	return;
	M_EPILOG
	}

}

}


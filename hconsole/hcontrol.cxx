/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcontrol.hxx"
#include "hconsole.hxx"
#include "hwindow.hxx"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.hxx"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HControl::HControl( HWindow* parent_, int row_, int column_,
										 int height_, int width_, char const* label_ )
	: _enabled( false ), _focused( false ), _drawLabel( true ),
	_singleLine( false ), _attributeDisabled( _attributeDisabled_ ),
	_attributeEnabled( _attributeEnabled_ ),
	_attributeFocused( _attributeFocused_ ), _row( row_ ), _column( column_ ),
	_height( height_ ), _width( width_ ), _rowRaw( 0 ),
	_columnRaw( 0 ), _heightRaw( 0 ), _widthRaw( 0 ),
	_label( label_ ), _varTmpBuffer(), _parent( parent_ ),
	_labelLength( 0 ), _shortcutIndex( 0 ), _valid( false ), _needRepaint( false )
	{
	M_PROLOG
	if ( ! HConsole::get_instance().is_enabled() )
		M_THROW ( "not in curses mode.", errno );
	if ( ! parent_ )
		M_THROW ( "no parent window.", reinterpret_cast<int long>( parent_ ) );
	_shortcutIndex = static_cast<int>( _label.find( '&' ) );
	if ( _shortcutIndex > -1 )
		{
		_label.set_at( _shortcutIndex, 0 );
		_label += label_ + _shortcutIndex + 1;
		}
	else
		_shortcutIndex = 0;
	_labelLength = label_ ? static_cast<int>( _label.get_length() ) : 0;
	if ( _labelLength )
		{
		if ( _label[ _labelLength - 1 ] != '\n' )
			_singleLine = true;
		else
			{
			_singleLine = false;
			_label.set_at( -- _labelLength, 0 );
			}
		}
	else
		_singleLine = true;
	_parent->add_control( HControl::ptr_t ( this ),
				KEY<>::meta_r ( _label [ _shortcutIndex ] ) );
	return;
	M_EPILOG
	}

HControl::~HControl( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log << "destroing control: " << _label << endl;
#endif /* __DEBUGGER_BABUNI__ */
	_parent = NULL;
	return;
	M_DESTRUCTOR_EPILOG
	}

void HControl::enable( bool enable_ )
	{
	M_PROLOG
	_enabled = enable_;
	if ( ! _enabled )
		_focused = false;
	schedule_refresh();
	return;
	M_EPILOG
	}

int HControl::process_input( int code_ )
	{
	M_PROLOG
	if ( ! _valid )
		update();
	return ( do_process_input( code_ ) );
	M_EPILOG
	}

int HControl::do_process_input ( int code_ )
	{
	M_PROLOG
	if ( ! _focused )
		M_THROW( "input in control without focus", code_ );
	return ( code_ );
	M_EPILOG
	}

int HControl::set_focus( char shortCut_ )
	{
	M_PROLOG
	if ( ! _enabled )
		return ( 1 );
	if ( ( shortCut_ > 0 ) && ( _label[ _shortcutIndex ] != shortCut_ ) )
		return ( 1 );
	_focused = true;
	if ( ! shortCut_ )
		_parent->acquire_focus( this );
	if ( shortCut_ <= 0 )
		schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

int HControl::kill_focus ( void )
	{
	M_PROLOG
	if ( ! _focused )
		return ( 1 );
	_focused = false;
	schedule_refresh();
	return ( 0 );
	M_EPILOG
	}

void HControl::refresh( void )
	{
	M_PROLOG
	if ( ! _valid )
		update();
	do_refresh();
	return;
	M_EPILOG
	}

void HControl::update( void )
	{
	M_PROLOG
	do_update();
	_valid = true;
	return;
	M_EPILOG
	}

void HControl::set( HInfo const & )
	{
	M_PROLOG
	if ( errno || ! errno )
		M_THROW ( "Abstract method called!", errno );
	return;
	M_EPILOG
	}

HInfo HControl::get( void )
	{
	M_PROLOG
	HInfo info;
	return ( info );
	M_EPILOG
	}

bool HControl::is_searchable( void )
	{
	M_PROLOG
	return ( false );
	M_EPILOG
	}

void HControl::draw_label( void )
	{
	M_PROLOG
	do_draw_label();
	_needRepaint = false;
	return;
	M_EPILOG
	}

void HControl::do_draw_label( void )
	{
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	schedule_refresh();
/* reposition control acordingly to current parent window size */
	_rowRaw = ( _row >= 0 ) ? _row : cons.get_height() + _row;
	_columnRaw = ( _column >= 0 ) ? _column
		: cons.get_width() + _column;
	_heightRaw = ( _height > 0 ) ? _height 
		: cons.get_height() + _height - _rowRaw;
	_widthRaw = ( _width > 0 ) ? _width
		: cons.get_width() + _width - _columnRaw;
/* done */
	if ( ! _drawLabel )
		{
		set_attr_data();
		return;
		}
	set_attr_label();
	M_ENSURE( cons.c_mvprintf( _rowRaw, _columnRaw, _label.raw() ) != C_ERR );
	set_attr_shortcut();
	M_ENSURE( cons.c_mvprintf( _rowRaw, _columnRaw + _shortcutIndex,
				"%c", _label[ _shortcutIndex ] ) != C_ERR );
	set_attr_data();
	if ( _singleLine )
		_columnRaw += _labelLength, _widthRaw -= _labelLength;
	else
		_rowRaw ++, _heightRaw --;
	return;
	M_EPILOG
	}

void HControl::set_attributes( int attributeDisabled_,
		int attributeEnabled_, int attributeFocused_ )
	{
	M_PROLOG
	if ( attributeDisabled_ == DEFAULT_ATTRS )
		_attributeDisabled = _attributeDisabled_;
	else
		_attributeDisabled = attributeDisabled_;
	if ( attributeEnabled_ == DEFAULT_ATTRS )
		_attributeEnabled = _attributeEnabled_;
	else
		_attributeEnabled = attributeEnabled_;
	if ( attributeFocused_ == DEFAULT_ATTRS )
		_attributeFocused = _attributeFocused_;
	else
		_attributeFocused = attributeFocused_;
	schedule_refresh();
	return;
	M_EPILOG
	}

void HControl::move( int row_, int column_, int height_, int width_ )
	{
	M_PROLOG
	_row = row_;
	_column = column_;
	_height = height_;
	_width = width_;
	schedule_refresh();
	return;
	M_EPILOG
	}

int HControl::click( mouse::OMouse& mouse_ )
	{
	M_PROLOG
	if ( ! _valid )
		update();
	return ( do_click( mouse_ ) );
	M_EPILOG
	}

int HControl::do_click( mouse::OMouse& )
	{
	M_PROLOG
	if ( _focused )
		return ( 1 );
	set_focus();
	return ( 0 );
	M_EPILOG
	}

void HControl::do_update( void )
	{
	return;
	}

bool HControl::hit_test( int row_, int column_ ) const
	{
	M_PROLOG
	if ( ( row_ < _rowRaw ) || ( row_ > ( _rowRaw + _heightRaw ) ) )
		return ( false );
	if ( ( column_ < _columnRaw )
			|| ( column_ >= ( _columnRaw + _widthRaw ) ) )
		return ( false );
	return ( true );
	M_EPILOG
	}

int HControl::attr_label( void ) const
	{
	M_PROLOG
	return ( _enabled ? ( _focused ? _attributeFocused >> 8 : _attributeEnabled >> 8 ) : _attributeDisabled >> 8 );
	M_EPILOG
	}

int HControl::attr_shortcut( void ) const
	{
	M_PROLOG
	return ( ! _enabled ? ( ! _focused ? _attributeFocused >> 8 : _attributeEnabled >> 8 ) : _attributeDisabled >> 8 );
	M_EPILOG
	}

int HControl::attr_data( void ) const
	{
	M_PROLOG
	return ( _enabled ? ( _focused ? _attributeFocused : _attributeEnabled ) : _attributeDisabled );
	M_EPILOG
	}

void HControl::set_attr_label( void ) const
	{
	M_PROLOG
	HConsole::get_instance().set_attr( attr_label() );
	return;
	M_EPILOG
	}

void HControl::set_attr_shortcut( void ) const
	{
	M_PROLOG
	HConsole::get_instance().set_attr( attr_shortcut() );
	return;
	M_EPILOG
	}

void HControl::set_attr_data( void ) const
	{
	M_PROLOG
	HConsole::get_instance().set_attr( attr_data() );
	return;
	M_EPILOG
	}

void HControl::set_draw_label( bool drawLabel_ )
	{
	_drawLabel = drawLabel_;
	return;
	}

void HControl::schedule_refresh( void )
	{
	_needRepaint = true;
	_needRepaint_ = true;
	return;
	}

void HControl::invalidate( void )
	{
	_valid = false;
	return;
	}

bool HControl::need_repaint( void ) const
	{
	return ( _needRepaint );
	}

}

}


/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htimewidget.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

namespace {
static int const TIME_WIDTH( static_cast<int>( sizeof ( " HH:MM:SS " ) ) - 1 );
static int const IDX_HOUR( 0 );
static int const IDX_MINUTE( 1 );
static int const IDX_SECOND( 2 );
static int const TIME_ITEMS( 3 );
}

HTimeWidget::HTimeWidget( HWindow* parent_, int row_, int column_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, 1, TIME_WIDTH, label_ )
	, _time( HTime::TZ::LOCAL )
	, _selectedTime( HTime::TZ::LOCAL )
	, _activeItemIdx( IDX_HOUR ) {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HTimeWidget::~HTimeWidget ( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HTimeWidget::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	draw_label();
	cons.mvprintf( _rowRaw, _columnRaw, " %s ", _time.string() );
	if ( _focused ) {
		int val( _activeItemIdx == IDX_HOUR ? _time.get_hour() : ( _activeItemIdx == IDX_MINUTE ? _time.get_minute() : _time.get_second() ) );
		set_attr_shortcut();
		cons.mvprintf( _rowRaw, _columnRaw + 1 + _activeItemIdx * 3, "%02d", val );
	}
	return;
	M_EPILOG
}

int HTimeWidget::do_process_input( int code_ ) {
	M_PROLOG
	int code( 0 );
	switch ( code_ ) {
		case ( KEY_CODE::LEFT ): {
			-- _activeItemIdx;
			if ( _activeItemIdx < 0 ) {
				_activeItemIdx += TIME_ITEMS;
			}
		} break;
		case ( KEY_CODE::RIGHT ): {
			++ _activeItemIdx;
			_activeItemIdx %= TIME_ITEMS;
		} break;
		case ( KEY_CODE::UP ):
		case ( KEY_CODE::DOWN ): {
			int mod( code_ == KEY_CODE::UP ? +1 : -1 );
			switch ( _activeItemIdx ) {
				case ( IDX_HOUR ):   _time.mod_hour( mod );   break;
				case ( IDX_MINUTE ): _time.mod_minute( mod ); break;
				case ( IDX_SECOND ): _time.mod_second( mod ); break;
			}
		} break;
		default: {
			code = code_;
		}
	}
	if ( !code ) {
		schedule_repaint();
		if ( _window ) {
			_window->status_bar()->clear( COLOR::FG_LIGHTGRAY );
		}
	}
	return code;
	M_EPILOG
}

bool HTimeWidget::do_click( mouse::OMouse& ) {
	M_PROLOG
	return ( false );
	M_EPILOG
}

void HTimeWidget::do_set_data( HInfo const& data_ ) {
	M_PROLOG
	_time = data_.get_time();
	_time.set_format( _iso8601TimeFormat_ );
	_selectedTime = _time;
	return;
	M_EPILOG
}

}

}


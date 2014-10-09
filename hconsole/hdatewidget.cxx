/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatewidget.cxx - this file is integral part of `yaal' project.

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
#include "hdatewidget.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

namespace {

void center( HString& str_, int toLen_ ) {
	int len( static_cast<int>( str_.get_length() ) );
	if ( len < toLen_ ) {
		int diff( toLen_  - len );
		str_.insert( 0, diff / 2, ' ' );
		str_.append( diff / 2 + diff % 2, ' ' );
	}
}

}

HDateWidget::HDateWidget( HWindow* parent_, int row_, int column_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, 1, static_cast<int>( sizeof ( " 0000-00-00 " ) ) - 1, label_ ),
	_time( HTime::LOCAL, _iso8601DateFormat_ ), _infoTime( _time ), _mode( MODE::VIEW ) {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HDateWidget::~HDateWidget( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HDateWidget::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	if ( _focused ) {
		cons.curs_set( CURSOR::INVISIBLE );
	}
	draw_label();
	if ( _mode == MODE::VIEW ) {
		cons.mvprintf( _rowRaw, _columnRaw, " %s ", _time.string().c_str() );
	} else {
		int rowOffset( 0 );
		HString month( _time.to_string( "%B %Y" ) );
		center( month, 3 * HTime::DAYS_IN_WEEK + 1 );
		cons.mvprintf( _rowRaw + rowOffset, _columnRaw, month.c_str() );
		++ rowOffset;
		for ( int i( 0 ); i < HTime::DAYS_IN_WEEK; ++ i ) {
			cons.mvprintf( _rowRaw + rowOffset, _columnRaw + i * 3, " %s ", week_day_name( ( i + 1 ) % HTime::DAYS_IN_WEEK ) );
		}
		++ rowOffset;
		HTime firstDayOfMonth( _time.get_year(), _time.get_month(), 1 );
		int firstDayOfMonthInWeekIdx( firstDayOfMonth.get_day_of_week() );
		int weeksInMonth( ( _time.get_days_in_month() + firstDayOfMonthInWeekIdx + HTime::DAYS_IN_WEEK - 1 ) / HTime::DAYS_IN_WEEK );
		cons.set_attr( COLORS::ATTR_NORMAL );
		cons.mvprintf( _rowRaw + rowOffset + 5, _columnRaw, "%*c", 22, ' ' );
		set_attr_data();
		for ( int i( 0 ); i < weeksInMonth; ++ i ) {
			cons.mvprintf( _rowRaw + rowOffset + i, _columnRaw, "%*c", 22, ' ' );
		}
		for ( int i( 0 ), dim( _time.get_days_in_month() ); i < dim; ++ i ) {
			int row( ( i + firstDayOfMonthInWeekIdx ) / HTime::DAYS_IN_WEEK );
			int col( ( i + firstDayOfMonthInWeekIdx ) % HTime::DAYS_IN_WEEK );
			if ( i == ( _time.get_day() - 1 ) ) {
				set_attr_shortcut();
			}
			cons.mvprintf( _rowRaw + rowOffset + row, _columnRaw + col * 3 + 1, "%2d", i + 1 );
			if ( i == ( _time.get_day() - 1 ) ) {
				set_attr_data();
			}
		}
	}
	return;
	M_EPILOG
}

int HDateWidget::do_process_input( int code_ ) {
	M_PROLOG
	bool unknown( false );
	switch ( code_ ) {
		case ( KEY_CODES::DOWN ): {
			on_key_down();
		} break;
		case ( KEY_CODES::UP ): {
			on_key_up();
		} break;
		case ( KEY_CODES::LEFT ): {
			on_key_left();
		} break;
		case ( KEY_CODES::RIGHT ): {
			on_key_right();
		} break;
		default: {
			unknown = true;
		}
	}
	if ( !unknown ) {
		code_ = 0;
		schedule_repaint();
		_window->status_bar()->message( COLORS::FG_LIGHTGRAY, "%s", _varTmpBuffer.raw() );
	}
	return ( code_ );
	M_EPILOG
}

void HDateWidget::on_key_down( void ) {
	M_PROLOG
	if ( _mode == MODE::VIEW ) {
		_mode = MODE::EDIT;
	} else {
		_time.mod_day( HTime::DAYS_IN_WEEK );
	}
	return;
	M_EPILOG
}

void HDateWidget::on_key_up( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_day( - HTime::DAYS_IN_WEEK );
	}
	return;
	M_EPILOG
}
void HDateWidget::on_key_left( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_day( - 1 );
	}
	return;
	M_EPILOG
}
void HDateWidget::on_key_right( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_day( 1 );
	}
	return;
	M_EPILOG
}

bool HDateWidget::do_click( mouse::OMouse& ) {
	M_PROLOG
	return ( false );
	M_EPILOG
}

char const* HDateWidget::week_day_name( int day_ ) {
	M_PROLOG
	tm broken;
	broken.tm_wday = day_;
	static int const WEEK_DAY_NAME_ABBREV_SIZE( 8 );
	char weekDayAbbrevBuf[WEEK_DAY_NAME_ABBREV_SIZE];
	strftime( weekDayAbbrevBuf, WEEK_DAY_NAME_ABBREV_SIZE, "%a", &broken );
	weekDayAbbrevBuf[2] = 0;
	_varTmpBuffer = weekDayAbbrevBuf;
	return ( _varTmpBuffer.c_str() );
	M_EPILOG
}

HInfo const& HDateWidget::do_get_data( void ) const {
	return ( _infoTime );
}

void HDateWidget::do_set_data( HInfo const& data_ ) {
	M_PROLOG
	_time = data_.get_time();
	_time.set_format( _iso8601DateFormat_ );
	return;
	M_EPILOG
}

}

}


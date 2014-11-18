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

static int const CAL_EDIT_WIDTH = 22;
static int const CAL_VIEW_WIDTH = static_cast<int>( sizeof ( " 0000-00-00 " ) ) - 1;

}

HDateWidget::HDateWidget( HWindow* parent_, int row_, int column_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, 1, CAL_VIEW_WIDTH, label_ ),
	_time( HTime::LOCAL, _iso8601DateFormat_ ),
	_selectedTime( HTime::LOCAL, _iso8601DateFormat_ ),
	_infoTime( _selectedTime ), _mode( MODE::VIEW ) {
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
		_heightRaw = 1;
		_widthRaw = CAL_VIEW_WIDTH;
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
		int firstDayOfMonthInWeekIdx( get_first_day_of_month_in_week_index() );
		int weeksInMonth( ( _time.get_days_in_month() + firstDayOfMonthInWeekIdx + HTime::DAYS_IN_WEEK - 1 ) / HTime::DAYS_IN_WEEK );
		cons.set_attr( COLORS::ATTR_NORMAL );
		cons.mvprintf( _rowRaw + rowOffset + 5, _columnRaw, "%*c", CAL_EDIT_WIDTH, ' ' );
		set_attr_data();
		for ( int i( 0 ); i < weeksInMonth; ++ i ) {
			cons.mvprintf( _rowRaw + rowOffset + i, _columnRaw, "%*c", CAL_EDIT_WIDTH, ' ' );
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
		_heightRaw = weeksInMonth + 2;
		_widthRaw = CAL_EDIT_WIDTH;
	}
	return;
	M_EPILOG
}

int HDateWidget::get_first_day_of_month_in_week_index( void ) const {
	M_PROLOG
	HTime firstDayOfMonth( _time.get_year(), _time.get_month(), 1 );
	return ( firstDayOfMonth.get_day_of_week() );
	M_EPILOG
}

void HDateWidget::do_kill_focus( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		close_calendar( ACTION::CANCEL );
	}
	HWidget::do_kill_focus();
	return;
	M_EPILOG
}

void HDateWidget::close_calendar( ACTION::action_t action_ ) {
	M_PROLOG
	_mode = MODE::VIEW;
	if ( action_ == ACTION::APPLY ) {
		_selectedTime = _time;
	} else {
		_time = _selectedTime;
	}
	if ( _window ) {
		_window->schedule_repaint( true );
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
		case ( KEY_CODES::PAGE_DOWN ): {
			on_key_page_down();
		} break;
		case ( KEY_CODES::PAGE_UP ): {
			on_key_page_up();
		} break;
		case ( '\r' ): {
			on_key_enter();
		} break;
		case ( KEY_CODES::ESCAPE ): {
			on_key_enter();
			break;
		}
		default: {
			unknown = true;
		}
	}
	if ( !unknown ) {
		code_ = 0;
		schedule_repaint();
		if ( _window ) {
			_window->status_bar()->clear( COLORS::FG_LIGHTGRAY );
		}
	}
	return ( code_ );
	M_EPILOG
}

void HDateWidget::on_key_enter( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		close_calendar( ACTION::APPLY );
	} else {
		_mode = MODE::EDIT;
	}
	return;
	M_EPILOG
}

void HDateWidget::on_key_down( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_day( HTime::DAYS_IN_WEEK );
	} else {
		_mode = MODE::EDIT;
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
		_time.mod_day( -1 );
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

void HDateWidget::on_key_page_down( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_month( 1 );
	}
	return;
	M_EPILOG
}

void HDateWidget::on_key_page_up( void ) {
	M_PROLOG
	if ( _mode == MODE::EDIT ) {
		_time.mod_month( -1 );
	}
	return;
	M_EPILOG
}

bool HDateWidget::do_click( mouse::OMouse& mouse_ ) {
	M_PROLOG
	bool handled( HWidget::do_click( mouse_ ) );
	if ( ! handled ) {
		if ( _mode == MODE::EDIT ) {
			int row( ( mouse_._row - _rowRaw ) - 2 );
			int col( ( mouse_._column - _columnRaw ) / 3 );
			int day( ( HTime::DAYS_IN_WEEK * row + col - get_first_day_of_month_in_week_index() ) + 1 );
			if ( day == _time.get_day() ) {
				on_key_enter();
				handled = true;
			} else if ( ( day > 0 ) && ( day <= _time.get_days_in_month() ) ) {
				_time.mod_day( day - _time.get_day() );
				schedule_repaint();
				handled = true;
			}
		} else {
			_mode = MODE::EDIT;
			schedule_repaint();
			handled = true;
		}
	}
	return ( handled );
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
	_selectedTime = _time;
	return;
	M_EPILOG
}

HWidget::ptr_t HDateWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HWidgetAttributes attrs;
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	attrs.label_position( r._labelPosition ).label_decoration( r._labelDecoration );
	HWidget* edit( new HDateWidget( window_, r._row, r._column, r._label, attrs ) );
	apply_resources( edit->get_pointer(), node_ );
	return ( edit->get_pointer() );
	M_EPILOG
}

bool HDateWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) {
	M_PROLOG
	return ( true );
	M_EPILOG
}

bool HDateWidgetCreator::do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) {
	return ( false );
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "date",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new HDateWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


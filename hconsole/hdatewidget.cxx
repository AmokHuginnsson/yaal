/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatewidget.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

namespace {

void center( HString& str_, int toLen_ ) {
	int len( static_cast<int>( str_.get_length() ) );
	if ( len < toLen_ ) {
		int diff( toLen_  - len );
		str_.insert( 0, diff / 2, ' '_ycp );
		str_.append( diff / 2 + diff % 2, ' '_ycp );
	}
}

static int const CAL_EDIT_WIDTH = 22;
static int const CAL_EDIT_HEIGHT = 8;
static int const CAL_VIEW_WIDTH = static_cast<int>( sizeof ( " 0000-00-00 " ) ) - 1;

}

HDateWidget::HDateWidget( HWindow* parent_, int row_, int column_,
		yaal::hcore::HString const& label_,
		HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, 1, CAL_VIEW_WIDTH, label_ )
	, _time( HTime::TZ::LOCAL, _iso8601DateFormat_ )
	, _selectedTime( HTime::TZ::LOCAL, _iso8601DateFormat_ )
	, _infoTime( _selectedTime )
	, _mode( MODE::VIEW ) {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

HDateWidget::~HDateWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HDateWidget::do_paint( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	if ( _focused ) {
		cons.curs_set( CURSOR::INVISIBLE );
	}
	draw_label();
	if ( _mode == MODE::VIEW ) {
		cons.mvprintf( _rowRaw, _columnRaw, " %s ", _time.string() );
		_heightRaw = 1;
		_widthRaw = CAL_VIEW_WIDTH;
	} else {
		if ( ( _columnRaw + CAL_EDIT_WIDTH ) > cons.get_width() ) {
			_columnRaw = cons.get_width() - CAL_EDIT_WIDTH;
		}
		if ( ( _rowRaw + CAL_EDIT_HEIGHT ) > cons.get_height() ) {
			_rowRaw = cons.get_height() - CAL_EDIT_HEIGHT;
		}
		int rowOffset( 0 );
		HString month( _time.to_string( "%B %Y" ) );
		center( month, 3 * HTime::DAYS_IN_WEEK + 1 );
		cons.mvprintf( _rowRaw + rowOffset, _columnRaw, month );
		++ rowOffset;
		for ( int i( 0 ); i < HTime::DAYS_IN_WEEK; ++ i ) {
			cons.mvprintf( _rowRaw + rowOffset, _columnRaw + i * 3, " %s ", week_day_name( ( i + 1 ) % HTime::DAYS_IN_WEEK ) );
		}
		++ rowOffset;
		int firstDayOfMonthInWeekIdx( get_first_day_of_month_in_week_index() );
		int weeksInMonth( ( _time.get_days_in_month() + firstDayOfMonthInWeekIdx + HTime::DAYS_IN_WEEK - 1 ) / HTime::DAYS_IN_WEEK );
		cons.set_attr( COLOR::ATTR_DEFAULT );
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
	return ( static_cast<int>( firstDayOfMonth.get_day_of_week() ) );
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

void HDateWidget::close_calendar( ACTION action_ ) {
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
		case ( KEY_CODE::DOWN ): {
			on_key_down();
		} break;
		case ( KEY_CODE::UP ): {
			on_key_up();
		} break;
		case ( KEY_CODE::LEFT ): {
			on_key_left();
		} break;
		case ( KEY_CODE::RIGHT ): {
			on_key_right();
		} break;
		case ( KEY_CODE::PAGE_DOWN ): {
			on_key_page_down();
		} break;
		case ( KEY_CODE::PAGE_UP ): {
			on_key_page_up();
		} break;
		case ( '\r' ): {
			on_key_enter();
		} break;
		case ( KEY_CODE::ESCAPE ): {
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
			_window->status_bar()->clear( COLOR::FG_LIGHTGRAY );
		}
	}
	return code_;
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
	return handled;
	M_EPILOG
}

yaal::hcore::HString const& HDateWidget::week_day_name( int day_ ) {
	M_PROLOG
	tm broken;
	::memset( &broken, 0, sizeof ( broken ) );
	broken.tm_wday = day_;
	static int const WEEK_DAY_NAME_ABBREV_SIZE( 8 );
	char weekDayAbbrevBuf[WEEK_DAY_NAME_ABBREV_SIZE];
	strftime( weekDayAbbrevBuf, WEEK_DAY_NAME_ABBREV_SIZE, "%a", &broken );
	weekDayAbbrevBuf[2] = 0;
	_varTmpBuffer = weekDayAbbrevBuf;
	return ( _varTmpBuffer );
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
	HDateWidget* date( create_widget<HDateWidget>( window_, r._row, r._column, r._label, attrs ) );
	apply_resources( date->get_pointer(), node_ );
	return ( date->get_pointer() );
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
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HDateWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


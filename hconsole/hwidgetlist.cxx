/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwidgetlist.hxx"
#include "hstatusbarwidget.hxx"
#include "hcore/hlog.hxx"
#include "tools/collections.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::collections;

namespace yaal {

namespace hconsole {

HWidgetList::HWidgetList( cyclic_iterator& focused_ )
	: _list()
	, _focused( focused_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HWidgetList::next_enabled( char shortcut_ ) {
	M_PROLOG
	bool loop( true );
	HWidgetList::cyclic_iterator it( _focused );
	do {
		++ _focused;
		if ( dynamic_cast<HStatusBarWidget*>( &(*(*_focused))) )
			continue;
		loop = ! (*_focused)->set_focus( shortcut_ );
		if ( _focused == it )
			loop = false;
	} while ( loop );
	if ( _focused != it ) {
		(*it)->kill_focus();
		(*_focused)->set_focus( -1 );
	}
	return;
	M_EPILOG
}

void HWidgetList::add_widget( HWidget::ptr_t widget_ ) {
	M_PROLOG
	_list.push_back( widget_ );
	if ( _focused == _list.end() ) {
		_focused = cyclic_iterator( &_list, prev( _list.end() ) );
	}
	return;
	M_EPILOG
}

void HWidgetList::sync_focus( void ) {
	M_PROLOG
	if ( _focused != _list.end() ) {
		(*_focused)->enable( true );
		(*_focused)->set_focus();
	}
	return;
	M_EPILOG
}

void HWidgetList::refresh_all( bool force_ ) {
	M_PROLOG
	bool focusedNeedRepaint( (*_focused)->need_repaint() );
	for ( model_t::iterator it( _list.begin() ), end( _list.end() ); it != end; ++ it ) {
		if ( ( it != _focused.base() ) && ( force_ || (*it)->need_repaint() ) ) {
			(*it)->paint();
		}
	}
	if ( !! (*_focused) && ( force_ || focusedNeedRepaint ) ) {
		(*_focused)->paint();
	}
	return;
	M_EPILOG
}

void HWidgetList::update_all( void ) {
	M_PROLOG
	for ( model_t::iterator it( _list.begin() ), end( _list.end() ); it != end; ++ it ) {
		(*it)->update();
	}
	return;
	M_EPILOG
}

void HWidgetList::hit_test_all( mouse::OMouse& mouse_ ) {
	M_PROLOG
	if ( (*_focused)->hit_test( mouse_._row, mouse_._column ) ) {
		(*_focused)->click( mouse_ );
	} else {
		for ( HWidget::ptr_t& p : _list ) {
			if ( p->hit_test( mouse_._row, mouse_._column ) ) {
				p->click( mouse_ );
				break;
			}
		}
	}
	return;
	M_EPILOG
}

HWidget* HWidgetList::get_widget_by_no( int offset_ ) {
	M_PROLOG
	model_t::iterator it = _list.begin();
	while ( offset_ -- > 0 ) {
		++ it;
	}
	return ( &**it );
	M_EPILOG
}

void HWidgetList::pop_front( void ) {
	M_PROLOG
	_list.pop_front();
	return;
	M_EPILOG
}

void HWidgetList::select( HWidget const* widget_ ) {
	M_PROLOG
	if ( (*_focused) != widget_ ) {
		model_t::iterator it( _list.begin() );
		for ( model_t::iterator end( _list.end() ); it != end; ++ it ) {
			if ( (*it) == widget_ ) {
				_focused = cyclic_iterator( &_list, it );
				break;
			}
		}
		if ( it == _list.end() ) {
			M_THROW( "bogus object", reinterpret_cast<int long>( widget_ ) );
		}
	}
	return;
	M_EPILOG
}

void HWidgetList::select( HWidget::ptr_t const& widget_ ) {
	M_PROLOG
	select( widget_.raw() );
	return;
	M_EPILOG
}

void HWidgetList::exchange( int former_, int latter_ ) {
	M_PROLOG
	if ( former_ != latter_ ) {
		int first( min( former_, latter_ ) ); /* *FIXME* Original code had +1, I do not why at this moment. */
		int last( max( former_, latter_ ) );
		model_t::iterator itFirst( n_th( _list, first ) );
		model_t::iterator itLast( itFirst );
		advance( itLast, last - first );
		_list.exchange( itFirst, itLast );
	}
	return;
	M_EPILOG
}

void HWidgetList::reorder_widgets( widget_order_t const& widgetOrder_ ) {
	M_PROLOG
	for ( widget_order_t::value_type const& wp : widgetOrder_ ) {
		int realPos( 0 );
		bool found( false );
		for ( HWidget::ptr_t const& w : _list ) {
			if ( w->resource_id() == wp.second ) {
				found = true;
				break;
			}
			++ realPos;
		}
		if ( found && ( realPos != wp.first ) ) {
			exchange( realPos, wp.first );
		}
	}
	return;
	M_EPILOG
}

}

}


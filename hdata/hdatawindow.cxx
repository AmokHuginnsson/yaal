/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawindow.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatawindow.hxx"
#include "hcore/hcore.hxx"
#include "hcore/memory.hxx"
#include "hdatalistwidget.hxx"
#include "hdatatreewidget.hxx"
#include "hdataeditwidget.hxx"
#include "hdatastatusbarwidget.hxx"
#include "hdataprocess.hxx"
#include "hcore/hformat.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::dbwrapper;
using namespace yaal::hconsole;

namespace yaal {

namespace hdata {

HDataWindow::HDataWindow( HString const& title_, HDataProcess* owner_ )
	: HWindow( title_ ),
	_modified( false ), _documentMode( DOCUMENT::VIEW ), _mainWidget( NULL ),
	_syncStore( NULL ),
	_viewModeWidgets(), _editModeWidgets(), _owner( owner_ ),
	_crud( new ( memory::yaal ) HCRUDDescriptor( owner_->data_base() ) ),
	_mode( HCRUDDescriptor::MODE::SELECT ),
	_idColumnName(), _dictionaries() {
	M_PROLOG
	register_postprocess_handler( KEY<'n'>::command, NULL, call( &HDataWindow::handler_add_new, this, _1 ) );
	register_postprocess_handler( KEY<'e'>::command, NULL, call( &HDataWindow::handler_edit, this, _1 ) );
	register_postprocess_handler( KEY<'d'>::command, NULL, call( &HDataWindow::handler_delete, this, _1 ) );
	register_postprocess_handler( KEY<'w'>::command, NULL, call( &HDataWindow::handler_save, this, _1 ) );
	register_postprocess_handler( KEY<'r'>::ctrl, NULL, call( &HDataWindow::handler_requery, this, _1 ) );
	register_postprocess_handler( KEY_CODES::ESC, NULL, call( &HDataWindow::handler_cancel, this, _1 ) );
	return;
	M_EPILOG
}

HDataWindow::~HDataWindow( void ) {
	M_PROLOG
	_mainWidget = NULL;
	_syncStore = NULL;
	return;
	M_EPILOG
}

void HDataWindow::do_init( void ) {
	M_PROLOG
	for ( dictionaries_t::value_type& val : _dictionaries ) {
		val.second->load();
	}
	HWindow::do_init();
	_mainWidget->set_focus();
	if ( _mainWidget ) {
		_mainWidget->load();
		_mainWidget->process_input( KEY_CODES::HOME );
	}
	paint();
	return;
	M_EPILOG
}

HStatusBarWidget* HDataWindow::do_init_bar( char const* label_ ) {
	M_PROLOG
	return ( new ( memory::yaal ) HDataStatusBarWidget( this, label_ ) );
	M_EPILOG
}

void HDataWindow::set_mode( DOCUMENT::mode_t mode_ ) {
	M_PROLOG
	switch ( mode_ ) {
		case ( DOCUMENT::VIEW ):
		case ( DOCUMENT::EDIT ): {
			_documentMode = mode_;
			controls_t::iterator end = _editModeWidgets.end();
			for ( controls_t::iterator it = _editModeWidgets.begin(); it != end; ++ it )
				(*it)->enable( mode_ == DOCUMENT::EDIT );
			end = _viewModeWidgets.end();
			for ( controls_t::iterator it = _viewModeWidgets.begin(); it != end; ++ it )
				(*it)->enable( mode_ == DOCUMENT::VIEW );
			controls_t::iterator begin = mode_ == DOCUMENT::EDIT ? _editModeWidgets.begin() : _viewModeWidgets.begin();
			end = mode_ == DOCUMENT::EDIT ? _editModeWidgets.end() : _viewModeWidgets.end();
			if ( begin != end )
				(*begin)->set_focus();
		}
		break;
		default :
			M_THROW( "unknown window mode", mode_ );
		break;
	}
	return;
	M_EPILOG
}

void HDataWindow::sync( HRecordSet::iterator it ) {
	M_PROLOG
	M_ASSERT( _syncStore );
	M_ASSERT( _documentMode == DOCUMENT::VIEW );
	int long count = _syncStore->_item.get_value_count();
	for ( int i = 0; i < count; i ++ ) {
		_syncStore->_item[ i ].set_string( it[i] ? *it[ i ] : "" );
	}
	if ( _syncStore->_idColNo >= 0 )
		_syncStore->_item._id = lexical_cast<int>( *it[ _syncStore->_idColNo ] );
	M_EPILOG
}

void HDataWindow::sync( void ) {
	M_PROLOG
	M_ASSERT( _documentMode == DOCUMENT::EDIT );
	int i( 0 );
	for ( controls_t::iterator it( _editModeWidgets.begin() ), end( _editModeWidgets.end() ); it != end; ++ it, ++ i )
		(*_crud)[ i ] = (*it)->get_data().get_string();
	return;
	M_EPILOG
}

void HDataWindow::set_sync_store( ORowBuffer* rB_ ) {
	_syncStore = rB_;
	return;
}

bool HDataWindow::handler_add_new( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot add new rocord now." ) );
		return ( true );
	}
	_mode = HCRUDDescriptor::MODE::INSERT;
	if ( _mainWidget )
		_mainWidget->add_new();
	set_mode( DOCUMENT::EDIT );
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_edit( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot start editing of this record." ) );
		return ( true );
	}
	if ( ! _crud->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( true );
	}
	_mode = HCRUDDescriptor::MODE::UPDATE;
	set_mode( DOCUMENT::EDIT );
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_delete( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "You cannot delete this record." ) );
		return ( true );
	}
	if ( ! _crud->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "There is nothing to remove." ) );
		return ( true );
	}
	if ( _mainWidget ) {
		HString filter;
		filter.format( "id = %ld", _mainWidget->get_current_id() );
		_crud->set_filter( filter );
		_crud->execute( HCRUDDescriptor::MODE::DELETE );
		_mainWidget->load();
	}
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_save( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT ) {
		_statusBar->message( COLORS::FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( true );
	}
	if ( _mode == HCRUDDescriptor::MODE::UPDATE ) {
		HString filter;
		filter.format( "id = %ld", _mainWidget->get_current_id() );
		_crud->set_filter( filter );
	}
	sync();
	HRecordSet::ptr_t rs = _crud->execute( _mode );
	if ( rs->get_errno() )
		_statusBar->message( COLORS::FG_BRIGHTRED, "%s", rs->get_error() );
	else {
		_modified = false;
		set_mode( DOCUMENT::VIEW );
		_mainWidget->load();
	}
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_requery( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "Finish your current operation first." ) );
		return ( true );
	}
	set_mode( DOCUMENT::VIEW );
	_mainWidget->load();
	paint();
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_cancel( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT )
		return ( true );
	set_mode( DOCUMENT::VIEW );
	if ( ( _mode == HCRUDDescriptor::MODE::INSERT ) && _mainWidget )
		_mainWidget->cancel_new();
	_modified = false;
	_statusBar->paint();
	_statusBar->message( COLORS::FG_BRIGHTRED, _( "Dropping all changes." ) );
	return ( true );
	M_EPILOG
}

bool HDataWindow::is_modified( void ) const {
	return ( _modified );
}

void HDataWindow::set_modified( bool modified_ ) {
	M_PROLOG
	bool modified = _modified;
	_modified = modified_;
	if ( ! modified )
		_statusBar->paint();
	return;
	M_EPILOG
}

void HDataWindow::set_widget_role( yaal::hdata::HDataWidget* widget_, HDataWidget::ROLE::role_t role_ ) {
	M_PROLOG
	switch ( role_ ) {
		case ( HDataWidget::ROLE::MAIN ): {
			_mainWidget = widget_;
			_viewModeWidgets.push_back( widget_ );
			widget_->enable( true );
		}
		break;
		case ( HDataWidget::ROLE::DATA ):
			_editModeWidgets.push_back( widget_ );
		break;
		case ( HDataWidget::ROLE::FILTER ):
			widget_->enable( true );
			_viewModeWidgets.push_back( widget_ );
		break;
		default :
			M_THROW( "unknown resource purpose", role_ );
		break;
	}
	return;
	M_EPILOG
}

void HDataWindow::set_record_descriptor( yaal::hcore::HString const& table_,
		yaal::hcore::HString const& columns_,
		yaal::hcore::HString const& filter_,
		yaal::hcore::HString const& sort_,
		yaal::hcore::HString const& idCol_ ) {
	M_PROLOG
	_crud->set_table( table_ );
	_crud->set_columns( columns_ );
	_crud->set_filter( filter_ );
	_crud->set_sort( sort_ );
	_idColumnName = idCol_;
	_mainWidget->set_crud_descriptor( _crud );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HDataWindow::id_column_name( void ) const {
	return ( _idColumnName );
}

void HDataWindow::add_dictionary( yaal::hcore::HString const& name_,
		HDictionary::ptr_t const& dict_ ) {
	M_PROLOG
	M_ENSURE( _dictionaries.insert( make_pair( name_, dict_ ) ).second );
	return;
	M_EPILOG
}

HDictionary::ptr_t HDataWindow::get_dictionary( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( _dictionaries.at( name_ ) );
	M_EPILOG
}

hconsole::HWindow::ptr_t HDataWindowCreator::do_new_instance( hconsole::HTUIProcess* tui_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HDataProcess* dp( dynamic_cast<HDataProcess*>( tui_ ) );
	M_ENSURE( dp );
	HString name( xml::attr_val( node_, "title" ) );
	HWindow::ptr_t window( make_pointer<HDataWindow>( name, dp ) );
	HDataWindow* dw( dynamic_cast<HDataWindow*>( window.raw() ) );
	for ( yaal::tools::HXml::HConstNodeProxy const& n : node_ ) {
		HString nodeName( n.get_name() );
		if ( nodeName == "dicts" ) {
			for ( yaal::tools::HXml::HConstNodeProxy const& dict : n ) {
				HString dictName( xml::attr_val( dict, "name" ) );
				HString table( xml::attr_val( dict, "table" ) );
				HString id( xml::attr_val( dict, "id_column" ) );
				HString value( xml::attr_val( dict, "value_column" ) );
				HDictionary::ptr_t d( new HDictionary( dp->data_base(), table, id, value ) );
				dw->add_dictionary( dictName, d );
			}
		}
	}
	create_widgets( window, node_ );
	for ( yaal::tools::HXml::HConstNodeProxy const& n : node_ ) {
		HString nodeName( n.get_name() );
		if ( nodeName == "db" ) {
			HString table( xml::attr_val( n, "table" ) );
			HString columns( xml::attr_val( n, "column" ) );
			HString id( xml::attr_val( n, "id_column" ) );
			xml::value_t filter( xml::try_attr_val( n, "filter" ) );
			xml::value_t sort( xml::try_attr_val( n, "sort" ) );
			dw->set_record_descriptor( table, columns, filter ? *filter : "", sort ? *sort : "", id );
		}
	}
	return ( window );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWindowFactory::get_instance().register_window_creator( "datawindow", HWindowCreatorInterface::ptr_t( new HDataWindowCreator() ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


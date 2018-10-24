/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "hdatadatewidget.hxx"
#include "hdatacomboboxwidget.hxx"
#include "hdatastatusbarwidget.hxx"
#include "hdataprocess.hxx"
#include "hcore/hlog.hxx"
#include "tools/stringalgo.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::dbwrapper;
using namespace yaal::hconsole;

namespace yaal {

namespace hdata {

HDataWindow::HDataWindow( HString const& title_, HDataProcess* owner_ )
	: HWindow( title_ )
	, _modified( false )
	, _documentMode( DOCUMENT::VIEW )
	, _mainWidget( nullptr )
	, _viewModeWidgets()
	, _editModeWidgets()
	, _crud( new ( memory::yaal ) HCRUDDescriptor( owner_->data_base() ) )
	, _mode( HCRUDDescriptor::MODE::READ )
	, _columns()
	, _idColumnName()
	, _dictionaries() {
	M_PROLOG
	register_postprocess_handler( KEY<'n'>::command, nullptr, call( &HDataWindow::handler_add_new, this, _1 ) );
	register_postprocess_handler( KEY<'e'>::command, nullptr, call( &HDataWindow::handler_edit, this, _1 ) );
	register_postprocess_handler( KEY<'d'>::command, nullptr, call( &HDataWindow::handler_delete, this, _1 ) );
	register_postprocess_handler( KEY<'w'>::command, nullptr, call( &HDataWindow::handler_save, this, _1 ) );
	register_postprocess_handler( KEY<'r'>::ctrl, nullptr, call( &HDataWindow::handler_requery, this, _1 ) );
	register_postprocess_handler( KEY_CODE::ESCAPE, nullptr, call( &HDataWindow::handler_cancel, this, _1 ) );
	return;
	M_EPILOG
}

HDataWindow::~HDataWindow( void ) {
	M_PROLOG
	_mainWidget = nullptr;
	return;
	M_DESTRUCTOR_EPILOG
}

void HDataWindow::do_init( void ) {
	M_PROLOG
	HWindow::do_init();
	if ( _mainWidget ) {
		_mainWidget->set_focus();
		_mainWidget->load();
		_mainWidget->process_input( KEY_CODE::HOME );
	}
	paint();
	return;
	M_EPILOG
}

HStatusBarWidget* HDataWindow::do_init_bar( yaal::hcore::HString const& label_ ) {
	M_PROLOG
	return ( create_widget<HDataStatusBarWidget>( this, label_ ) );
	M_EPILOG
}

void HDataWindow::set_mode( DOCUMENT mode_ ) {
	M_PROLOG
	switch ( mode_ ) {
		case ( DOCUMENT::VIEW ):
		case ( DOCUMENT::EDIT ): {
			_documentMode = mode_;
			controls_t::iterator end = _editModeWidgets.end();
			for ( controls_t::iterator it = _editModeWidgets.begin(); it != end; ++ it ) {
				(*it)->enable( mode_ == DOCUMENT::EDIT );
			}
			end = _viewModeWidgets.end();
			for ( controls_t::iterator it = _viewModeWidgets.begin(); it != end; ++ it ) {
				(*it)->enable( mode_ == DOCUMENT::VIEW );
			}
			controls_t::iterator begin = mode_ == DOCUMENT::EDIT ? _editModeWidgets.begin() : _viewModeWidgets.begin();
			end = mode_ == DOCUMENT::EDIT ? _editModeWidgets.end() : _viewModeWidgets.end();
			if ( begin != end ) {
				(*begin)->set_focus();
			}
		} break;
		default: {
			M_THROW( "unknown window mode", mode_ );
		} break;
	}
	return;
	M_EPILOG
}

void HDataWindow::sync( void ) {
	M_PROLOG
	M_ASSERT( _documentMode == DOCUMENT::EDIT );
	int i( 0 );
	for ( controls_t::iterator it( _editModeWidgets.begin() ), end( _editModeWidgets.end() ); it != end; ++ it, ++ i ) {
		HWidget* w( *it );
		if ( dynamic_cast<HDateWidget*>( w ) ) {
			hdata::sync( (*_crud)[ i ], w->get_data(), TYPE::HTIME );
			log << "date: " << w->get_label() << ": [" << *(*_crud)[ i ] << "]" << endl;
		} else if ( dynamic_cast<HComboboxWidget*>( w ) ) {
			hdata::sync( (*_crud)[ i ], w->get_data(), TYPE::INT_LONG_LONG );
			log << "combo: " << w->get_label() << ": [" << *(*_crud)[ i ] << "]" << endl;
		} else if ( dynamic_cast<HListWidget*>( w ) ) {
			-- i;
		} else {
			hdata::sync( (*_crud)[ i ], w->get_data(), TYPE::HSTRING );
			log << "edit: " << w->get_label() << ": [" << *(*_crud)[ i ] << "]" << endl;
		}
	}
	return;
	M_EPILOG
}

bool HDataWindow::handler_add_new( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "You cannot add new record now." ) );
		return ( true );
	}
	_mode = HCRUDDescriptor::MODE::CREATE;
	if ( _mainWidget ) {
		_mainWidget->add_new();
	}
	set_mode( DOCUMENT::EDIT );
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_edit( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "You cannot start editing of this record." ) );
		return ( true );
	}
	if ( ! _crud->get_size() ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "There is nothing to edit." ) );
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
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "You cannot delete this record." ) );
		return ( true );
	}
	if ( ! _crud->get_size() ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "There is nothing to remove." ) );
		return ( true );
	}
	if ( _mainWidget ) {
		_crud->set_filter_value( _mainWidget->get_current_id() );
		_crud->execute( HCRUDDescriptor::MODE::DELETE );
		_mainWidget->load();
	}
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_save( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( true );
	}
	if ( ( _mode == HCRUDDescriptor::MODE::CREATE ) || ( _mode == HCRUDDescriptor::MODE::UPDATE ) ) {
		for ( HDataWidget* dw : _editModeWidgets ) {
			HDataEditWidget* dew( dynamic_cast<HDataEditWidget*>( dw ) );
			if ( dew && ! dew->is_valid() ) {
				_statusBar->message( COLOR::FG_BRIGHTRED, _( "Invalid value in: %s" ), dew->get_label() );
				return ( true );
			}
		}
	}
	int long id( _mainWidget->get_current_id() );
	_crud->set_columns( _columns );
	if ( _mode == HCRUDDescriptor::MODE::UPDATE ) {
		_crud->set_filter_value( to_string( id ) );
	}
	sync();
	HRecordSet::ptr_t rs = _crud->execute( _mode );
	if ( rs->get_errno() ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, rs->get_error() );
	} else {
		if ( _mode == HCRUDDescriptor::MODE::CREATE ) {
			id = rs->get_insert_id();
		}
		for ( HDataWidget* dw : _editModeWidgets ) {
			dw->save( id );
		}
		_modified = false;
		set_mode( DOCUMENT::VIEW );
		_mainWidget->load();
	}
	reload_record();
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_requery( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLOR::FG_BRIGHTRED, _( "Finish your current operation first." ) );
		return ( true );
	}
	set_mode( DOCUMENT::VIEW );
	for ( dictionaries_t::value_type& val : _dictionaries ) {
		val.second->load();
	}
	_mainWidget->load();
	paint();
	return ( true );
	M_EPILOG
}

bool HDataWindow::handler_cancel( hconsole::HEvent const& ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT ) {
		return ( true );
	}
	set_mode( DOCUMENT::VIEW );
	if ( ( _mode == HCRUDDescriptor::MODE::CREATE ) && _mainWidget ) {
		_mainWidget->cancel_new();
	}
	_modified = false;
	_statusBar->paint();
	_statusBar->message( COLOR::FG_BRIGHTRED, _( "Dropping all changes." ) );
	reload_record();
	return ( true );
	M_EPILOG
}

void HDataWindow::reload_record( void ) {
	M_PROLOG
	HDataListWidget* l( dynamic_cast<HDataListWidget*>( _mainWidget ) );
	if ( l ) {
		l->selection_change();
	}
	return;
	M_EPILOG
}

bool HDataWindow::on_sel_change( yaal::hconsole::HEvent const& ) {
	M_PROLOG
	int long currId( _mainWidget->get_current_id() );
	for ( HDataWidget* dw : _editModeWidgets ) {
		dw->load( currId );
	}
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
	if ( ! modified ) {
		_statusBar->paint();
	}
	return;
	M_EPILOG
}

void HDataWindow::set_widget_role( yaal::hdata::HDataWidget* widget_, HDataWidget::ROLE role_ ) {
	M_PROLOG
	switch ( role_ ) {
		case ( HDataWidget::ROLE::MAIN ): {
			_mainWidget = widget_;
			_viewModeWidgets.push_back( widget_ );
			widget_->enable( true );
		} break;
		case ( HDataWidget::ROLE::DATA ): {
			_editModeWidgets.push_back( widget_ );
		} break;
		case ( HDataWidget::ROLE::FILTER ): {
			widget_->enable( true );
			_viewModeWidgets.push_back( widget_ );
		} break;
		default : {
			M_THROW( "unknown resource purpose", role_ );
		} break;
	}
	return;
	M_EPILOG
}

void HDataWindow::set_record_descriptor( yaal::hcore::HString const& table_,
		yaal::hcore::HString const& columns_,
		yaal::hcore::HString const& idCol_,
		yaal::hcore::HString const& filter_,
		yaal::hcore::HString const& sort_,
		yaal::hcore::HString const& view_ ) {
	M_PROLOG
	_crud->set_table( table_ );
	_columns = string::split<HCRUDDescriptor::field_names_t>( columns_, "," );
	M_ENSURE( _columns.back().trim() == idCol_ );
	_columns.pop_back();
	_crud->set_columns( _columns );
	_crud->set_id_column( idCol_ );
	_crud->set_filter( idCol_ );
	_crud->set_filter( filter_, HCRUDDescriptor::MODE::READ, HCRUDDescriptor::OFilter::CONDITION::NONE );
	_crud->set_sort( sort_ );
	_idColumnName = idCol_;
	_mainWidget->set_crud_descriptor( _crud );
	_mainWidget->set_view_query( view_ );
	_mainWidget->register_event_listener( call( &HDataWindow::on_sel_change, this, _1 ) );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HDataWindow::id_column_name( void ) const {
	return ( _idColumnName );
}

int long HDataWindow::get_current_id( void ) const {
	M_PROLOG
	return ( _mainWidget ? _mainWidget->get_current_id() : -1 );
	M_EPILOG
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
	return ( window );
	M_EPILOG
}

bool HDataWindowCreator::do_apply_resources( hconsole::HTUIProcess* tui_, window_ptr_t window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HDataWindow* dw( dynamic_cast<HDataWindow*>( window_.raw() ) );
	HDataProcess* dp( dynamic_cast<HDataProcess*>( tui_ ) );
	M_ENSURE( dp );
	for ( yaal::tools::HXml::HConstNodeProxy const& n : node_ ) {
		HString nodeName( n.get_name() );
		if ( nodeName == "dicts" ) {
			for ( yaal::tools::HXml::HConstNodeProxy const& dict : n ) {
				HString dictName( xml::attr_val( dict, "name" ) );
				HString table( xml::attr_val( dict, "table" ) );
				HString idColumn( xml::attr_val( dict, "id_column" ) );
				HString value( xml::attr_val( dict, "value_column" ) );
				HDictionary::ptr_t d( new ( memory::yaal ) HDictionary( dp->data_base(), table, idColumn, value ) );
				dw->add_dictionary( dictName, d );
				d->load();
			}
		}
	}
	create_widgets( window_, node_ );
	for ( yaal::tools::HXml::HConstNodeProxy const& n : node_ ) {
		HString nodeName( n.get_name() );
		if ( nodeName == "db" ) {
			xml::value_t view( xml::try_node_val( n ) );
			HString table( xml::attr_val( n, "table" ) );
			HString columns( xml::attr_val( n, "column" ) );
			HString idColumn( xml::attr_val( n, "id_column" ) );
			xml::value_t filter( xml::try_attr_val( n, "filter" ) );
			xml::value_t sort( xml::try_attr_val( n, "sort" ) );
			dw->set_record_descriptor( table, columns, idColumn, filter ? *filter : "", sort ? *sort : "", view ? *view : "" );
		}
	}
	return ( true );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWindowFactory::get_instance().register_window_creator( "datawindow", HWindowCreatorInterface::ptr_t( new ( memory::yaal ) HDataWindowCreator() ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}


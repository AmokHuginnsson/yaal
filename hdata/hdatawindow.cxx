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
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataWindow::HDataWindow( char const* title_, HDataProcess* owner_,
		resources_t* dataControlInfo_ )
	: HWindow( title_ ),
	_modified( false ), _documentMode( DOCUMENT::VIEW ), _mainWidget( NULL ),
	_resourcesArray( dataControlInfo_ ), _syncStore( NULL ),
	_viewModeWidgets(), _editModeWidgets(), _owner( owner_ ),
	_dB( new ( memory::yaal ) HSQLDescriptor( owner_->data_base() ) ),
	_mode( HSQLDescriptor::MODE::SELECT ) {
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
	_resourcesArray = NULL;
	_syncStore = NULL;
	return;
	M_EPILOG
}

#define M_SETUP_STANDARD r._row, r._column, r._height, r._width, r._label

void HDataWindow::do_init( void ) {
	M_PROLOG
	char value[] = "";
	char const* mask = _maskDefault_;
	HDataWidget* dataWidget = NULL;
	OAttributes attributes;
	OAttributes* attr = &attributes;
	OEditControlResource editControlResource;
	OListControlResource listControlResource;
/* ecr stands for EditControlResource */
	OEditControlResource* ecr = &editControlResource;
/* LCR stands for ListControlResource */
	OListControlResource* lcr = &listControlResource;
	attributes._drawLabel = true;
	attributes._disabledAttribute = HWidget::DEFAULT_ATTRS;
	attributes._enabledAttribute = HWidget::DEFAULT_ATTRS;
	attributes._focusedAttribute = HWidget::DEFAULT_ATTRS;
	HWindow::do_init();
	for ( int i( 0 ), SIZE( static_cast<int>( _resourcesArray->size() ) ); i < SIZE; ++ i ) {
		OResource& r( (*_resourcesArray)[ i ] );
		if ( r._attributes )
			attr = r._attributes;
		switch ( r._type ) {
			case ( DATACONTROL_BITS::TYPE::EDIT ): {
				editControlResource._maxStringSize = 127;
				editControlResource._value = value;
				editControlResource._mask = mask;
				editControlResource._replace = false;
				editControlResource._multiLine = false;
				editControlResource._readOnly = false;
				editControlResource._rightAligned = false;
				editControlResource._password = false;
				editControlResource._maxHistoryLevel = 8;
				if ( r._typeSpecific ) {
					ecr = static_cast<OEditControlResource*>( r._typeSpecific );
				}

				dataWidget = new ( memory::yaal ) HDataEditWidget( this,
						M_SETUP_STANDARD,
						HEditWidgetAttributes()
						.max_string_size( ecr->_maxStringSize )
						.text( ecr->_value )
						.pattern( ecr->_mask )
						.replace( ecr->_replace )
						.multiline( ecr->_multiLine )
						.readonly( ecr->_readOnly )
						.rightaligned( ecr->_rightAligned )
						.password( ecr->_password )
						.max_history_level( ecr->_maxHistoryLevel ) );
			}
			break;
			case ( DATACONTROL_BITS::TYPE::LIST ): {
				listControlResource._checkable = false;
				listControlResource._sortable = true;
				listControlResource._searchable = true;
				listControlResource._drawHeader = true;
				if ( r._typeSpecific )
					lcr = static_cast<OListControlResource*>( r._typeSpecific );
				HDataListWidget* list( static_cast<HDataListWidget*>( dataWidget = new ( memory::yaal ) HDataListWidget( this, M_SETUP_STANDARD ) ) );
				list->set_flags( HListWidget::flag_t( lcr->_checkable ? HListWidget::FLAG::CHECKABLE : HListWidget::FLAG::NONE )
						| ( lcr->_sortable ? HListWidget::FLAG::SORTABLE : HListWidget::FLAG::NONE )
						| ( lcr->_editable ? HListWidget::FLAG::EDITABLE : HListWidget::FLAG::NONE )
						| ( lcr->_drawHeader ? HListWidget::FLAG::DRAW_HEADER : HListWidget::FLAG::NONE ),
						HListWidget::FLAG::ALL );
			}
			break;
			case ( DATACONTROL_BITS::TYPE::TREE ):
				dataWidget = new ( memory::yaal ) HDataTreeWidget( this, M_SETUP_STANDARD );
			break;
			case ( DATACONTROL_BITS::TYPE::COMBO ):
			break;
			case ( DATACONTROL_BITS::TYPE::DATE ):
			break;
			case ( DATACONTROL_BITS::TYPE::CHECK ):
			break;
			default :
			break;
		}
		if ( dataWidget ) {
			dataWidget->set_draw_label( attr->_drawLabel );
			dataWidget->set_attributes( attr->_disabledAttribute,
					attr->_enabledAttribute,
					attr->_focusedAttribute );
			dataWidget->set_label_position( r._labelStacked ? HWidget::LABEL::POSITION::STACKED : HWidget::LABEL::POSITION::SIDE_BY_SIDE );
			dataWidget->set_resource( &r );
		}
		switch ( r._role ) {
			case ( DATACONTROL_BITS::ROLE::MAIN ): {
				_dB->set_table( r._table );
				_dB->set_columns( r._columns );
				_dB->set_filter( r._filter );
				_dB->set_sort( r._sort );
				dataWidget->set_dbd( _dB );
				_mainWidget = dataWidget;
				_viewModeWidgets.push_back( dataWidget );
				dataWidget->enable( true );
			}
			break;
			case ( DATACONTROL_BITS::ROLE::DATA ):
				link( i, dataWidget );
				_editModeWidgets.push_back( dataWidget );
			break;
			case ( DATACONTROL_BITS::ROLE::FILTER ):
				dataWidget->enable( true );
				_viewModeWidgets.push_back( dataWidget );
			break;
			default :
				M_THROW( ( HFormat( "unknown resource purpose at %d" ) % i ).string(), r._role );
			break;
		}
	}
	_mainWidget->set_focus();
	if ( _mainWidget ) {
		_mainWidget->load();
		_mainWidget->process_input( KEY_CODES::HOME );
	}
	paint();
	return;
	M_EPILOG
}

HStatusBarWidget* HDataWindow::init_bar( char const* label_ ) {
	M_PROLOG
	return ( new ( memory::yaal ) HDataStatusBarWidget( this, label_ ) );
	M_EPILOG
}

void HDataWindow::link( int child_, HDataWidget* dataControl_ ) {
	M_PROLOG
	int parent = 0;
	char name [] = "";
	HDataListWidget* pDC = NULL; /* PDC stands for ParentDataControl */
/* CI stands for ColumnInfo */
	OColumnInfo columnInfo, * cI = &columnInfo;
	cI->_placement = HDataListWidget::HColumnInfo::ADD_AT_THE_END;
	cI->_name = name;
	cI->_width = 1; 				/* width is awlays proportional */
	cI->_align = HWidget::BITS::ALIGN::LEFT;
	cI->_type = TYPE::HSTRING;
	parent = (*_resourcesArray)[ child_ ]._parent;
	if ( (*_resourcesArray)[ parent ]._type == DATACONTROL_BITS::TYPE::LIST ) {
		pDC = dynamic_cast<HDataListWidget*>( _widgets.get_widget_by_no( parent + 1 /* 1 stands for offset caused by 'status bar' */ ) );
		if ( ! pDC )
			M_THROW( "wrong control resource order",
					parent );
		if ( (*_resourcesArray)[ child_ ]._columnInfo )
			cI = (*_resourcesArray)[ child_ ]._columnInfo;
		pDC->add_column( cI->_placement, cI->_name,
				cI->_width, cI->_align, cI->_type, dataControl_ );
	} else
		M_THROW( "unknown parent type", (*_resourcesArray)[ parent ]._type );
	return;
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
	for ( int i = 0; i < count; i ++ )
		_syncStore->_item[ i ].set_string( *it[ i ] );
	if ( _syncStore->_idColNo >= 0 )
		_syncStore->_item._id = lexical_cast<int>( *it[ _syncStore->_idColNo ] );
	M_EPILOG
}

void HDataWindow::sync( void ) {
	M_PROLOG
	M_ASSERT( _documentMode == DOCUMENT::EDIT );
	int i( 0 );
	for ( controls_t::iterator it( _editModeWidgets.begin() ), end( _editModeWidgets.end() ); it != end; ++ it, ++ i )
		(*_dB)[ i ] = (*it)->get().get_string();
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
	_mode = HSQLDescriptor::MODE::INSERT;
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
	if ( ! _dB->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( true );
	}
	_mode = HSQLDescriptor::MODE::UPDATE;
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
	if ( ! _dB->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "There is nothing to remove." ) );
		return ( true );
	}
	if ( _mainWidget ) {
		HString filter;
		filter.format( "id = %ld", _mainWidget->get_current_id() );
		_dB->set_filter( filter );
		_dB->execute( HSQLDescriptor::MODE::DELETE );
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
	if ( _mode == HSQLDescriptor::MODE::UPDATE ) {
		HString filter;
		filter.format( "id = %ld", _mainWidget->get_current_id() );
		_dB->set_filter( filter );
	}
	sync();
	HRecordSet::ptr_t rs = _dB->execute( _mode );
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
	if ( ( _mode == HSQLDescriptor::MODE::INSERT ) && _mainWidget )
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

}

}


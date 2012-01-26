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
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hdatawindow.hxx"
#include "hcore/hcore.hxx"
#include "hcore/memory.hxx"
#include "hdatalistcontrol.hxx"
#include "hdatatreecontrol.hxx"
#include "hdataeditcontrol.hxx"
#include "hdatastatusbarcontrol.hxx"
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
	_modified( false ), _documentMode( DOCUMENT::VIEW ), _mainControl( NULL ),
	_resourcesArray( dataControlInfo_ ), _syncStore( NULL ),
	_viewModeControls(), _editModeControls(), _owner( owner_ ),
	_dB( new ( memory::yaal ) HSQLDescriptor( owner_->data_base() ) ),
	_mode( HSQLDescriptor::MODE::SELECT ) {
	M_PROLOG
	register_postprocess_handler( KEY<'n'>::command, NULL,
			&HDataWindow::handler_add_new );
	register_postprocess_handler( KEY<'e'>::command, NULL,
			&HDataWindow::handler_edit );
	register_postprocess_handler( KEY<'d'>::command, NULL,
			&HDataWindow::handler_delete );
	register_postprocess_handler( KEY<'w'>::command, NULL,
			&HDataWindow::handler_save );
	register_postprocess_handler( KEY<'r'>::ctrl, NULL,
			&HDataWindow::handler_requery );
	register_postprocess_handler( KEY_CODES::ESC, NULL,
			&HDataWindow::handler_cancel );
	return;
	M_EPILOG
}

HDataWindow::~HDataWindow( void ) {
	M_PROLOG
	_mainControl = NULL;
	_resourcesArray = NULL;
	_syncStore = NULL;
	return;
	M_EPILOG
}

#define M_SETUP_STANDARD r._row, r._column, r._height, r._width, r._label

int HDataWindow::init( void ) {
	M_PROLOG
	char value[] = "";
	char const* mask = _maskDefault_;
	HDataControl* dataControl = NULL;
	OAttributes attributes;
	OAttributes* attr = &attributes;
	OEditControlResource editControlResource;
	OListControlResource listControlResource;
/* ecr stands for EditControlResource */
	OEditControlResource* ecr = &editControlResource;
/* LCR stands for ListControlResource */
	OListControlResource* lcr = &listControlResource;
	attributes._drawLabel = true;
	attributes._disabledAttribute = -1;
	attributes._enabledAttribute = -1;
	attributes._focusedAttribute = -1;
	HWindow::init();
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
				if ( r._typeSpecific )
					ecr = static_cast<OEditControlResource*>( r._typeSpecific );
				dataControl = new ( memory::yaal ) HDataEditControl( this,
						M_SETUP_STANDARD, ecr->_maxStringSize, ecr->_value,
						ecr->_mask, ecr->_replace,
						ecr->_multiLine, ecr->_readOnly,
						ecr->_rightAligned, ecr->_password,
						ecr->_maxHistoryLevel );
			}
			break;
			case ( DATACONTROL_BITS::TYPE::LIST ): {
				listControlResource._checkable = false;
				listControlResource._sortable = true;
				listControlResource._searchable = true;
				listControlResource._drawHeader = true;
				if ( r._typeSpecific )
					lcr = static_cast<OListControlResource*>( r._typeSpecific );
				HDataListControl* list( static_cast<HDataListControl*>( dataControl = new ( memory::yaal ) HDataListControl( this, M_SETUP_STANDARD ) ) );
				list->set_flags( HListControl::flag_t( lcr->_checkable ? HListControl::FLAG::CHECKABLE : HListControl::FLAG::NONE )
						| ( lcr->_sortable ? HListControl::FLAG::SORTABLE : HListControl::FLAG::NONE )
						| ( lcr->_editable ? HListControl::FLAG::EDITABLE : HListControl::FLAG::NONE )
						| ( lcr->_drawHeader ? HListControl::FLAG::DRAW_HEADER : HListControl::FLAG::NONE ),
						HListControl::FLAG::ALL );
			}
			break;
			case ( DATACONTROL_BITS::TYPE::TREE ):
				dataControl = new ( memory::yaal ) HDataTreeControl( this, M_SETUP_STANDARD );
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
		if ( dataControl ) {
			dataControl->set_draw_label( attr->_drawLabel );
			dataControl->set_attributes( attr->_disabledAttribute,
					attr->_enabledAttribute,
					attr->_focusedAttribute );
			dataControl->set_resource( &r );
		}
		switch ( r._role ) {
			case ( DATACONTROL_BITS::ROLE::MAIN ): {
				_dB->set_table( r._table );
				_dB->set_columns( r._columns );
				_dB->set_filter( r._filter );
				_dB->set_sort( r._sort );
				dataControl->set_dbd( _dB );
				_mainControl = dataControl;
				_viewModeControls.push_back( dataControl );
				dataControl->enable( true );
			}
			break;
			case ( DATACONTROL_BITS::ROLE::DATA ):
				link( i, dataControl );
				_editModeControls.push_back( dataControl );
			break;
			case ( DATACONTROL_BITS::ROLE::FILTER ):
				dataControl->enable( true );
				_viewModeControls.push_back( dataControl );
			break;
			default :
				M_THROW( ( HFormat( "unknown resource purpose at %d" ) % i ).string(), r._role );
			break;
		}
	}
	_mainControl->set_focus();
	if ( _mainControl ) {
		_mainControl->load();
		_mainControl->process_input( KEY_CODES::HOME );
	}
	refresh();
	return ( 0 );
	M_EPILOG
}

HStatusBarControl* HDataWindow::init_bar( char const* label_ ) {
	M_PROLOG
	return ( new ( memory::yaal ) HDataStatusBarControl( this, label_ ) );
	M_EPILOG
}

void HDataWindow::link( int child_, HDataControl* dataControl_ ) {
	M_PROLOG
	int parent = 0;
	char name [] = "";
	HDataListControl* pDC = NULL; /* PDC stands for ParentDataControl */
/* CI stands for ColumnInfo */
	OColumnInfo columnInfo, * cI = &columnInfo;
	cI->_placement = HDataListControl::HColumnInfo::ADD_AT_THE_END;
	cI->_name = name;
	cI->_width = 1; 				/* width is awlays proportional */
	cI->_align = HControl::BITS::ALIGN::LEFT;
	cI->_type = TYPE::HSTRING;
	parent = (*_resourcesArray)[ child_ ]._parent;
	if ( (*_resourcesArray)[ parent ]._type == DATACONTROL_BITS::TYPE::LIST ) {
		pDC = dynamic_cast<HDataListControl*>( _controls.get_control_by_no( parent + 1 /* 1 stands for offset caused by 'status bar' */ ) );
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
			controls_t::iterator end = _editModeControls.end();
			for ( controls_t::iterator it = _editModeControls.begin(); it != end; ++ it )
				(*it)->enable( mode_ == DOCUMENT::EDIT );
			end = _viewModeControls.end();
			for ( controls_t::iterator it = _viewModeControls.begin(); it != end; ++ it )
				(*it)->enable( mode_ == DOCUMENT::VIEW );
			controls_t::iterator begin = mode_ == DOCUMENT::EDIT ? _editModeControls.begin() : _viewModeControls.begin();
			end = mode_ == DOCUMENT::EDIT ? _editModeControls.end() : _viewModeControls.end();
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
		_syncStore->_item[ i ]( it[ i ]->raw() );
	if ( _syncStore->_idColNo >= 0 )
		_syncStore->_item._id = lexical_cast<int>( *it[ _syncStore->_idColNo ] );
	M_EPILOG
}

void HDataWindow::sync( void ) {
	M_PROLOG
	M_ASSERT( _documentMode == DOCUMENT::EDIT );
	int count = static_cast<int>( _editModeControls.size() );
	for ( int i = 0; i < count; i ++ )
		(*_dB)[ i ] = _editModeControls[ i ]->get().get<HString const&>();
	return;
	M_EPILOG
}

void HDataWindow::set_sync_store( ORowBuffer* rB_ ) {
	_syncStore = rB_;
	return;
}

int HDataWindow::handler_add_new( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot add new rocord now." ) );
		return ( 0 );
	}
	_mode = HSQLDescriptor::MODE::INSERT;
	if ( _mainControl )
		_mainControl->add_new();
	set_mode( DOCUMENT::EDIT );
	return ( 0 );
	M_EPILOG
}

int HDataWindow::handler_edit( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "You cannot start editing of this record." ) );
		return ( 0 );
	}
	if ( ! _dB->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_ ( "There is nothing to edit." ) );
		return ( 0 );
	}
	_mode = HSQLDescriptor::MODE::UPDATE;
	set_mode( DOCUMENT::EDIT );
	return ( 0 );
	M_EPILOG
}

int HDataWindow::handler_delete( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "You cannot delete this record." ) );
		return ( 0 );
	}
	if ( ! _dB->get_size() ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "There is nothing to remove." ) );
		return ( 0 );
	}
	if ( _mainControl ) {
		HString filter;
		filter.format( "id = %ld", _mainControl->get_current_id() );
		_dB->set_filter( filter );
		_dB->execute( HSQLDescriptor::MODE::DELETE );
		_mainControl->load();
	}
	return ( 0 );
	M_EPILOG
}

int HDataWindow::handler_save( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT ) {
		_statusBar->message( COLORS::FG_BRIGHTRED, _( "There is nothing to save." ) );
		return ( 0 );
	}
	if ( _mode == HSQLDescriptor::MODE::UPDATE ) {
		HString filter;
		filter.format( "id = %ld", _mainControl->get_current_id() );
		_dB->set_filter( filter );
	}
	sync();
	HRecordSet::ptr_t rs = _dB->execute( _mode );
	if ( rs->get_errno() )
		_statusBar->message( COLORS::FG_BRIGHTRED, "%s", rs->get_error() );
	else {
		_modified = false;
		set_mode( DOCUMENT::VIEW );
		_mainControl->load();
	}
	return ( 0 );
	M_EPILOG
}

int HDataWindow::handler_requery( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::VIEW ) {
		_statusBar->message( COLORS::FG_BRIGHTRED,
				_( "Finish your current operation first." ) );
		return ( 0 );
	}
	set_mode( DOCUMENT::VIEW );
	_mainControl->load();
	refresh();
	return ( 0 );
	M_EPILOG
}

int HDataWindow::handler_cancel( int, void const* ) {
	M_PROLOG
	if ( _documentMode != DOCUMENT::EDIT )
		return ( 0 );
	set_mode( DOCUMENT::VIEW );
	if ( ( _mode == HSQLDescriptor::MODE::INSERT ) && _mainControl )
		_mainControl->cancel_new();
	_modified = false;
	_statusBar->refresh();
	_statusBar->message( COLORS::FG_BRIGHTRED, _( "Dropping all changes." ) );
	return ( 0 );
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
		_statusBar->refresh();
	return;
	M_EPILOG
}

}

}


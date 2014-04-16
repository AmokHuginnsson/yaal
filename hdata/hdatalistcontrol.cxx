/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatalistcontrol.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::hconsole::list_control_helper;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataListControl::HDataListControl(
		HDataWindow* window_, int row_, int column_, int height_,
		int width_, char const* title_ )
								: HControl ( window_, row_, column_, height_,
										width_, title_ ),
								HSearchableControl( true ),
								HListControl( NULL, 0, 0, 0, 0, NULL ),
								HDataControl(), _dataControler( _controler ) {
	M_PROLOG
	return;
	M_EPILOG
}

HDataListControl::~HDataListControl ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HDataListControl::load( int long /*id_*/ ) {
	M_PROLOG
	_SQL->set_filter( "" );
	HDataWindow* parent = dynamic_cast<HDataWindow*>( _parent );
	M_ASSERT( parent );
	HRecordSet::ptr_t rs = _SQL->execute( HSQLDescriptor::MODE::SELECT );
	int idColNo = -1;
	int const colCount = rs->get_field_count();
	for ( int i = 0; i < colCount; ++ i )
		if ( ! ::strcmp( rs->get_column_name( i ), _resource->_id ) )
			idColNo = i;
	HDataWindow::ORowBuffer rb( idColNo, static_cast<int>( _header.size() ) );
	parent->set_sync_store( &rb );
	parent->status_bar()->init_progress( 0., "Collecting ..." );
	HListControler<>::model_ptr_t model = _dataControler->get_model();
	HListControler<>::model_t::iterator it = model->begin();
	int ctr( 0 );
	int size( static_cast<int>( _dataControler->size() ) );
	for ( HRecordSet::iterator row( rs->begin() ), end( rs->end() ); row != end; ++ row ) {
		parent->sync( row );
		parent->status_bar()->update_progress();
		if ( it != model->end() )
			{	
			(*it) = rb._item;
			++ it;
		} else
			model->push_back( rb._item );
		++ ctr;
	} while ( ctr ++ < size )
		_dataControler->remove_tail();
	reset();
	parent->set_sync_store();
	return;
	M_EPILOG
}

int long HDataListControl::get_current_id( void ) {
	M_PROLOG
	return ( _cursor->get_id() );
	M_EPILOG
}

void HDataListControl::add_new( void ) {
	M_PROLOG
	_dataControler->add_tail( HInfoItem( static_cast<int>( _header.size() ) ) );
	process_input( KEY_CODES::HOME );
	process_input( KEY_CODES::END );
	return;
	M_EPILOG
}

void HDataListControl::cancel_new( void ) {
	M_PROLOG
	_dataControler->remove_tail();
	if ( _dataControler->size() ) {
		process_input( KEY_CODES::HOME );
		process_input( KEY_CODES::END );
	}
	return;
	M_EPILOG
}

}

}


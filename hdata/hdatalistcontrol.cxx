/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hdatalistcontrol.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::hconsole::list_control_helper;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataListControl::HDataListControl( 
		HDataWindow* a_poWindow, int a_iRow, int a_iColumn, int a_iHeight,
		int a_iWidth, char const* a_pcTitle )
								: HControl ( a_poWindow, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcTitle ),
								HSearchableControl( true ),
								HListControl( NULL, 0, 0, 0, 0, NULL ),
								HDataControl(), f_oDataControler( f_oControler )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataListControl::~HDataListControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HDataListControl::load( int long /*a_iId*/ )
	{
	M_PROLOG
	f_oSQL->set_filter( "" );
	int l_iCount = 0, l_iCtr = 0, l_iSize = static_cast<int>( f_oDataControler->size() );
	HDataWindow* l_poParent = dynamic_cast<HDataWindow*>( f_poParent );
	M_ASSERT ( l_poParent );
	HRecordSet::ptr_t rs = f_oSQL->execute( HSQLDescriptor::MODE::SELECT );
	int idColNo = -1;
	int const colCount = rs->get_field_count();
	for ( int i = 0; i < colCount; ++ i )
		if ( ! ::strcmp( rs->get_column_name( i ), f_psResource->f_pcId ) )
			idColNo = i;
	HDataWindow::ORowBuffer rb( idColNo, static_cast<int>( f_oHeader.size() ) );
	l_poParent->set_sync_store( &rb );
	l_poParent->status_bar()->init_progress ( static_cast<double>( l_iCount ), "Collecting ..." );
	HListControler<>::model_ptr_t l_oModel = f_oDataControler->get_model();
	HListControler<>::model_t::iterator it = l_oModel->begin();
	for ( HRecordSet::iterator row = rs->begin(); row != rs->end(); ++ row )
		{
		l_poParent->sync( row );
		l_poParent->status_bar()->update_progress();
		if ( it != l_oModel->end() )
			{	
			(*it) = rb.f_oItem;
			++ it;
			}
		else
			l_oModel->push_back( rb.f_oItem );
		++ l_iCtr;
		}
	while ( l_iCtr ++ < l_iSize )
		f_oDataControler->remove_tail();
	reset();
	l_poParent->set_sync_store();
	return;
	M_EPILOG
	}

int long HDataListControl::get_current_id( void )
	{
	M_PROLOG
	return ( f_oCursor->get_id() );
	M_EPILOG
	}

void HDataListControl::add_new( void )
	{
	M_PROLOG
	f_oDataControler->add_tail( HItem( static_cast<int>( f_oHeader.size() ) ) );
	process_input( KEY_CODES::HOME );
	process_input( KEY_CODES::END );
	return;
	M_EPILOG
	}

void HDataListControl::cancel_new( void )
	{
	M_PROLOG
	f_oDataControler->remove_tail();
	if ( f_oDataControler->size() )
		{
		process_input( KEY_CODES::HOME );
		process_input( KEY_CODES::END );
		}
	return;
	M_EPILOG
	}

}

}


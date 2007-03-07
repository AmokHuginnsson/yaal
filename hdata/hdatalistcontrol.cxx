/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatalistcontrol.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hdatalistcontrol.h"
#include "hdatawindow.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataListControl::HDataListControl ( HRecordSet * a_poRecordSet,
		HDataWindow * a_poWindow, int a_iRow, int a_iColumn, int a_iHeight,
		int a_iWidth, char const * a_pcTitle )
								: HControl ( a_poWindow, a_iRow, a_iColumn, a_iHeight,
										a_iWidth, a_pcTitle ),
								HSearchableControl ( true ),
								HListControl ( NULL, 0, 0, 0, 0, NULL ),
								HDataControl ( a_poRecordSet ), f_oDataControler( f_oControler )
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

void HDataListControl::load ( int long /*a_iId*/ )
	{
	M_PROLOG
	int l_iCount = 0, l_iCtr = 0, l_iQuantity = f_oDataControler->size();
	HItem l_oItem ( f_oHeader.size() );
	HDataWindow * l_poParent = dynamic_cast<HDataWindow*> ( f_poParent );
	M_ASSERT ( l_poParent );
	l_poParent->set_sync_store( & l_oItem );
	if ( f_poRecordSet->is_open() )
		l_iCount = f_poRecordSet->requery();
	else
		l_iCount = f_poRecordSet->open();
	l_poParent->status_bar ( )->init_progress ( static_cast<double>( l_iCount ), "Collecting ..." );
	iterator_t it = f_oDataControler->begin();
	while ( ! f_poRecordSet->is_eof() )
		{
		l_poParent->status_bar()->update_progress();
		if ( it != f_oDataControler->end() )
			{	
			(*it) = l_oItem;
			++ it;
			}
		else
			f_oDataControler->add_tail( l_oItem );
		f_poRecordSet->move_next();
		}
	while ( l_iCtr ++ < l_iQuantity )
		f_oDataControler->remove_tail();
	reset();
	l_poParent->set_sync_store();
	return;
	M_EPILOG
	}

int long HDataListControl::get_current_id ( void )
	{
	M_PROLOG
	return ( (*f_oCursor).m_lId );
	M_EPILOG
	}

void HDataListControl::add_new ( void )
	{
	M_PROLOG
	f_oDataControler->add_tail ( );
	process_input ( KEY_CODES::D_HOME );
	process_input ( KEY_CODES::D_END );
	return;
	M_EPILOG
	}

void HDataListControl::cancel_new ( void )
	{
	M_PROLOG
	f_oDataControler->remove_tail();
	if ( f_oDataControler->size() )
		{
		process_input ( KEY_CODES::D_HOME );
		process_input ( KEY_CODES::D_END );
		}
	return;
	M_EPILOG
	}

}

}


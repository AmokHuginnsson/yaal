/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hitem.c - this file is integral part of `stdhapi' project.

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
M_CVSID ( "$CVSHeader$" );
#include "hitem.h"

HItem::HItem ( int a_iSize ) : HArray < HInfo > ( a_iSize )
	{
	M_PROLOG
	m_lId = 0;
	m_bChecked = false;
	return;
	M_EPILOG
	}

HItem::~HItem ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HItem::HItem ( const HItem & a_roItem, int ) : HArray < HInfo > ( a_roItem )
	{
	M_PROLOG
	m_lId = a_roItem.m_lId;
	m_bChecked = a_roItem.m_bChecked;
	return;
	M_EPILOG
	}

HItem & HItem::operator = ( const HItem & a_roItem )
	{
	M_PROLOG
	( * this ).HArray < HInfo >::operator = ( a_roItem );
	m_lId = a_roItem.m_lId;
	m_bChecked = a_roItem.m_bChecked;
	return ( * this );
	M_EPILOG
	}

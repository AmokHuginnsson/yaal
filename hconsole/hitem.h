/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hitem.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HITEM_H
#define __YAAL_HCONSOLE_HITEM_H

#line 31

#define D_VCSID_HCONSOLE_HITEM_H "$Id$"

#include "hcore/harray.h"
#include "hcore/hinfo.h"

namespace yaal
{

namespace hconsole
{

/*! \brief Basic data unit stored in HListControl and HTreeControl.
 */
template <typename tType = yaal::hcore::HInfo>
class HItem_t : public hcore::HArray<tType>
	{
public:
	int long	m_lId;
	bool	m_bChecked;
	HItem_t ( int = 0 );
	virtual ~HItem_t ( void );
	HItem_t ( HItem_t const &, int );
	HItem_t & operator = ( HItem_t const & );
	};

template <typename tType>
HItem_t<tType>::HItem_t ( int a_iSize ) : hcore::HArray<tType> ( a_iSize ),
															 m_lId ( 0 ), m_bChecked ( false )
	{
	return;
	}

template <typename tType>
HItem_t<tType>::~HItem_t ( void )
	{
	return;
	}

template <typename tType>
HItem_t<tType>::HItem_t ( HItem_t const & a_roItem, int ) : hcore::HArray<tType> ( a_roItem ),
	m_lId ( a_roItem.m_lId ), m_bChecked ( a_roItem.m_bChecked )
	{
	return;
	}

template <typename tType>
HItem_t<tType> & HItem_t<tType>::operator = ( HItem_t<tType> const & a_roItem )
	{
	M_PROLOG
	if ( this != & a_roItem )
		{
		(*this).hcore::HArray<tType>::operator = ( a_roItem );
		m_lId = a_roItem.m_lId;
		m_bChecked = a_roItem.m_bChecked;
		}
	return ( *this );
	M_EPILOG
	}

typedef HItem_t<> HItem;

}

}

#endif /* not __YAAL_HCONSOLE_HITEM_H */

/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hitem.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HITEM_HXX_INCLUDED
#define YAAL_HCONSOLE_HITEM_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hinfo.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Basic data unit stored in HListControl and HTreeControl.
 */
template <typename tType = yaal::hcore::HInfo>
class HItem_t : public hcore::HArray<tType> {
public:
	int long	m_lId;
	bool	m_bChecked;
	HItem_t ( int = 0 );
	virtual ~HItem_t ( void );
	HItem_t ( HItem_t const &, int );
	HItem_t & operator = ( HItem_t const & );
};

template <typename tType>
HItem_t<tType>::HItem_t ( int size_ ) : hcore::HArray<tType> ( size_ ),
															 m_lId ( 0 ), m_bChecked ( false ) {
	return;
}

template <typename tType>
HItem_t<tType>::~HItem_t ( void ) {
	return;
}

template <typename tType>
HItem_t<tType>::HItem_t ( HItem_t const & item_, int ) : hcore::HArray<tType> ( item_ ),
	m_lId ( item_.m_lId ), m_bChecked ( item_.m_bChecked ) {
	return;
}

template <typename tType>
HItem_t<tType> & HItem_t<tType>::operator = ( HItem_t<tType> const & item_ ) {
	M_PROLOG
	if ( this != & item_ ) {
		(*this).hcore::HArray<tType>::operator = ( item_ );
		m_lId = item_.m_lId;
		m_bChecked = item_.m_bChecked;
	}
	return ( *this );
	M_EPILOG
}

typedef HItem_t<> HItem;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HITEM_HXX_INCLUDED */


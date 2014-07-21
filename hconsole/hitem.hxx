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
#include "hconsole/hinfo.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Basic data unit stored in HListWidget and HTreeWidget.
 *
 * tType myst be subclass of HInfo.
 */
template <typename tType = HInfoMultiVal>
class HItem {
public:
	typedef tType value_type;
	typedef HItem<tType> this_type;
	typedef hcore::HArray<value_type> data_t;
private:
	data_t _data;
public:
	int long	_id;
	bool	_checked;
	HItem( int = 0 );
	virtual ~HItem ( void );
	HItem( HItem const& );
	HItem& operator = ( HItem const& );
	void swap( HItem& );
	HInfo& operator[]( int );
	HInfo const& operator[]( int ) const;
	int get_value_count( void ) const;
};

template <typename tType>
HItem<tType>::HItem( int size_ )
	: _data( size_ ), _id( 0 ), _checked( false ) {
	return;
}

template <typename tType>
HItem<tType>::~HItem ( void ) {
	return;
}

template <typename tType>
HItem<tType>::HItem( HItem const& item_ )
	: _data( item_._data ), _id ( item_._id ), _checked ( item_._checked ) {
	return;
}

template <typename tType>
HItem<tType>& HItem<tType>::operator = ( HItem const& item_ ) {
	M_PROLOG
	if ( this != &item_ ) {
		HItem tmp( item_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

template <typename tType>
void HItem<tType>::swap( HItem& item_ ) {
	if ( &item_ != this ) {
		using yaal::swap;
		swap( _data, item_._data );
		swap( _id, item_._id );
		swap( _checked, item_._checked );
	}
	return;
}

template <typename tType>
HInfo& HItem<tType>::operator[]( int idx_ ) {
	return ( _data[ idx_ ] );
}

template <typename tType>
HInfo const& HItem<tType>::operator[]( int idx_ ) const {
	return ( _data[ idx_ ] );
}

template <typename tType>
int HItem<tType>::get_value_count( void ) const {
	return ( static_cast<int>( _data.get_size() ) );
}

typedef HItem<> HInfoItem;

template <typename tType>
inline void swap( yaal::hconsole::HItem<tType>& a, yaal::hconsole::HItem<tType>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCONSOLE_HITEM_HXX_INCLUDED */


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashcontainer.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhashcontainer.hxx"
#include "algorithm.hxx"

namespace yaal {

namespace hcore {

int long const x_tag_g_pulPrimes[ 32 ] = {
	1,					3,					7,					13,					31,					61,					
	127,				251,				509,				1021,				2039,				4093,
	8191,				16381,			32749,			65521,			131071,			262139,
	524287,			1048573,		2097143,		4194301,		8388593,		16777213,
	33554393,		67108859,		134217689,	268435399,	536870909,	1073741789,
	2147483647,	0
}, * const _primes_ = x_tag_g_pulPrimes;

template<>
int long hash<HString>::operator () ( HString const& string_ ) const {
	char const* const buffer( string_.raw() );
	int long size( string_.get_length() );
	int long h( 0 );
	while ( size -- ) {
		h += buffer[ size ];
		h <<= 3;
	}
	return ( h );
}

template<typename T>
int long hash<T>::operator () ( T const& key_ ) const {
	return ( static_cast<int long>( key_ ) );
}

template struct hash<char>;
template struct hash<char signed>;
template struct hash<char unsigned>;
template struct hash<int short>;
template struct hash<int short unsigned>;
template struct hash<int>;
template struct hash<int unsigned>;
template struct hash<int long>;
template struct hash<int long unsigned>;
template struct hash<int long long>;
template struct hash<int long long unsigned>;

}

}


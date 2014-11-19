/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	stringalgo.cxx - this file is integral part of `yaal' project.

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
#include "stringalgo.hxx"
#include "hcore/hresource.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace string {

namespace {
int min3 ( int a, int b, int c ) {
	int ret = c;
	if ( a < ret )
		ret = a;
	if ( b < ret )
		ret = b;
	return ( ret );
}
int min ( int a, int b ) {
	return ( a < b ? a : b );
}
}

namespace distance {

int levenshtein_damerau( yaal::hcore::HString const& one_, yaal::hcore::HString const& two_, bool damerau_ ) {
	M_PROLOG
	int cost = 0;
	int indexOne = 0, indexTwo = 0;
	int lengthOne = static_cast<int>( one_.length() );
	int lengthTwo = static_cast<int>( two_.length() );
	int** distanceMatrix = NULL;
	if ( ! lengthTwo )
		return ( lengthOne );
	if ( ! lengthOne )
		return ( lengthTwo );
	lengthOne ++;
	lengthTwo ++;
	HResource<int*[]> distanceMatrixHolder( new ( memory::yaal ) int*[ lengthOne ] );
	HResource<int[]> distanceMatrixBuffer( new ( memory::yaal ) int[ lengthOne * lengthTwo ] );
	distanceMatrix = distanceMatrixHolder.raw();
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne )
		distanceMatrix[ indexOne ] = distanceMatrixBuffer.raw() + indexOne * lengthTwo;
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne )
		distanceMatrix[ indexOne ][ 0 ] = indexOne;
	for ( indexTwo = 0; indexTwo < lengthTwo; ++ indexTwo )
		distanceMatrix[ 0 ][ indexTwo ] = indexTwo;
	lengthTwo --;
	lengthOne --;
	/* real magic starts here */
	for ( indexOne = 0; indexOne < lengthOne; ++ indexOne ) {
		for ( indexTwo = 0; indexTwo < lengthTwo; ++ indexTwo ) {
			cost = 0;
			if ( one_ [ indexOne ] != two_ [ indexTwo ] )
				cost = 1;
			distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min3(
					distanceMatrix[ indexOne ][ indexTwo + 1 ] + 1,
					distanceMatrix[ indexOne + 1 ][ indexTwo ] + 1,
					distanceMatrix[ indexOne ][ indexTwo ] + cost );
			if ( damerau_ && ( indexOne > 0 ) && ( indexTwo > 0 )
					&& ( one_[ indexOne - 1 ] == two_[ indexTwo ] )
					&& ( one_[ indexOne ] == two_[ indexTwo - 1 ] ) )
				distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min(
						distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ],
						distanceMatrix[ indexOne - 1 ][ indexTwo - 1 ] + cost );
		}
	}
	return ( distanceMatrix[ lengthOne ][ lengthTwo ] );
	M_EPILOG
}

}

}

}

}


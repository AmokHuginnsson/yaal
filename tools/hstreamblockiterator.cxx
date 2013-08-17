/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreamblockiterator.cxx - this file is integral part of `yaal' project.

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
#include "hcore/algorithm.hxx"
#include "hstreamblockiterator.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {

static int long const BUFFER_SIZE = yaal::meta::power<2,20>::value;

}

HStreamBlockIterator::HStreamBlockIterator( HStreamInterface& stream_, int long size_ )
	: _buffer( BUFFER_SIZE ), _index( 0 ), _size( size_ ),
	_bufferOffset( 0 ), _bufferSize( 0 ), _stream( stream_ ) {
	M_ASSERT( _size > 0 );
	if ( _size > BUFFER_SIZE )
		M_THROW( "so large stream blocks are not supported", _size );
}

HStreamBlockIterator::~HStreamBlockIterator( void ) {
}

HStreamBlockIterator& HStreamBlockIterator::operator ++ ( void ) {
	M_PROLOG
	++ _index;
	return ( *this );
	M_EPILOG
}

HStreamBlockIterator::HBlock HStreamBlockIterator::operator* ( void ) {
	M_PROLOG
	int long firstOctet = _index * _size;
	if ( ( firstOctet >= ( _bufferOffset + _bufferSize ) ) && ( ( _bufferSize == BUFFER_SIZE ) || ! _bufferSize ) ) {
		_bufferOffset = firstOctet;
		_bufferSize = _stream.read( _buffer.raw(), BUFFER_SIZE );
	}
	return ( HBlock( ( _buffer.get<char>() + firstOctet ) - _bufferOffset, min( _bufferSize - ( firstOctet - _bufferOffset ), _size ) ) );
	M_EPILOG
}

}

}


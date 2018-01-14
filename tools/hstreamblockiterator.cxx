/* Read yaal/LICENSE.md file for copyright and licensing information. */

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


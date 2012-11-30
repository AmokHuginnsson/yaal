/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreamblockiterator.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED
#define YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Read stream block by block.
 */
class HStreamBlockIterator : private yaal::trait::HNonCopyable {
public:
	/*! \brief Single block of data.
	 */
	class HBlock {
		void* _start;
		int long _size;
	public:
		HBlock( void ) : _start( NULL ), _size( 0 ) {}
		HBlock( void* start_, int long size_ ) : _start( start_ ), _size( size_ ) {}
		HBlock( HBlock const& o ) : _start( o._start ), _size( o._size ) {}
		HBlock& operator = ( HBlock const& o ) {
			if ( &o != this ) {
				HBlock tmp( o );
				swap( tmp );
			}
			return ( *this );
		}
		void swap( HBlock& o ) {
			if ( &o != this ) {
				using yaal::swap;
				swap( _start, o._start );
				swap( _size, o._size );
			}
			return;
		}
		void* data( void ) const
			{ return ( _start ); }
		int long octets( void ) const
			{ return ( _size ); }
	};
private:
	typedef HStreamBlockIterator this_type;
	yaal::hcore::HChunk _buffer;
	int long _index; /*!< number of currently processed IO block */
	int long _size; /*!< requested size of IO block */
	int long _bufferOffset;
	int long _bufferSize;
	yaal::hcore::HStreamInterface& _stream;
public:
	/*! \brief Construct new block based stream reader.
	 *
	 * \param stream - data source stream.
	 * \param size - size of IO block.
	 */
	HStreamBlockIterator( yaal::hcore::HStreamInterface& stream, int long size );
	~HStreamBlockIterator( void );
	HStreamBlockIterator& operator ++ ( void );
	HBlock operator* ( void );
};

typedef yaal::hcore::HExceptionT<HStreamBlockIterator> HBitSourceException;

}

}

#endif /* #ifndef YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED */


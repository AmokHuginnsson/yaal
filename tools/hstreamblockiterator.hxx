/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
		HBlock( void ) : _start( nullptr ), _size( 0 ) {}
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


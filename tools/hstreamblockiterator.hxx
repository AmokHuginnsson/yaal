/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreamblockiterator.hxx - this file is integral part of hanalyser.h project.

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

#ifndef YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED
#define YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED

#include "hcore/base.hxx"
#include "hcore/hpool.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Read stream block by block.
 */
class HStreamBlockIterator : private yaal::HNonCopyable
	{
public:
	/*! \brief Single block of data.
	 */
	class HBlock
		{
		void* f_pvStart;
		int long f_lSize;
	public:
		HBlock( void ) : f_pvStart( NULL ), f_lSize( 0 ) {}
		HBlock( void* a_pvStart, int long const& a_lSize ) : f_pvStart( a_pvStart ), f_lSize( a_lSize ) {}
		HBlock( HBlock const& o ) : f_pvStart( o.f_pvStart ), f_lSize( o.f_lSize ) {}
		HBlock& operator = ( HBlock const& o )
			{
			if ( &o != this )
				{
				HBlock tmp( o );
				swap( tmp );
				}
			return ( *this );
			}
		void swap( HBlock& o )
			{
			if ( &o != this )
				{
				using yaal::swap;
				swap( f_pvStart, o.f_pvStart );
				swap( f_lSize, o.f_lSize );
				}
			return;
			}
		void* data( void ) const
			{ return ( f_pvStart ); }
		int long octets( void ) const
			{ return ( f_lSize ); }
		};
	typedef yaal::hcore::HPool<char> buffer_t;
private:
	typedef HStreamBlockIterator self_t;
	buffer_t f_oBuffer;
	int long f_lIndex; /*!< number of currently processed IO block */
	int long f_lSize; /*!< requested size of IO block */
	int long f_lBufferOffset;
	int long f_lBufferSize;
	yaal::hcore::HStreamInterface const& f_oStream;
public:
	/*! \brief Construct new block based stream reader.
	 *
	 * \param stream - data source stream.
	 * \param size - size of IO block.
	 */
	HStreamBlockIterator( yaal::hcore::HStreamInterface const& stream, int long size );
	~HStreamBlockIterator( void );
	HStreamBlockIterator& operator ++ ( void );
	HBlock operator* ( void );
	};

typedef yaal::hcore::HExceptionT<HStreamBlockIterator> HBitSourceException;

}

}

#endif /* not YAAL_TOOLS_HBITSOURCE_HXX_INCLUDED */


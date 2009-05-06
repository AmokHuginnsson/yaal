/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreaminterface.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef HSTREAMINTERFACE_HXX_INCLUDED
#define HSTREAMINTERFACE_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hpool.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hcore
{

class HStreamInterface;
HStreamInterface& endl( HStreamInterface& );
HStreamInterface& flush( HStreamInterface& );

/*! \brief Interface for stream based IO.
 */
class HStreamInterface
	{
	typedef HStreamInterface self_t;
public:
	/*! \brief Status of given IO stream.
	 */
	struct STATUS
		{
		typedef enum
			{
			OK,
			REPEAT,
			ERROR
			} code_t;
		int octets; /*!< number of octets read during last read operation */
		code_t code; /*!< return code for last read operation */
		STATUS( void ) : octets( 0 ), code( OK ) {}
		};
	typedef HPointer<HStreamInterface> ptr_t;
protected:
	typedef yaal::hcore::HPool<char> cache_t;
	cache_t f_oCache;
	int f_iOffset;
	STATUS f_sStatus;
public:
	HStreamInterface( void );
	virtual ~HStreamInterface( void );
	HStreamInterface& operator << ( HString const& );
	HStreamInterface& operator << ( char const* const& );
	HStreamInterface& operator << ( char const& );
	HStreamInterface& operator << ( int short const& );
	HStreamInterface& operator << ( int short unsigned const& );
	HStreamInterface& operator << ( int const& );
	HStreamInterface& operator << ( int unsigned const& );
	HStreamInterface& operator << ( int long const& );
	HStreamInterface& operator << ( int long unsigned const& );
	HStreamInterface& operator << ( double const& );
	HStreamInterface& operator << ( double long const& );
	HStreamInterface& operator << ( float const& );
	HStreamInterface& operator << ( void const* const& );
	HStreamInterface& operator << ( HStreamInterface& ( *const )( HStreamInterface& ) );
	/*! \brief Read data from stream until end of it or until delimiter is encountered.
	 *
	 * \param store - Store read date here.
	 * \param delim - Stop reading data at any character in this set.
	 * \param strip - Remove delimiting stop char from output buffer.
	 * \return Number of characters avctualy read.
	 */
	STATUS const& read_until( yaal::hcore::HString& store, char const* const delim = eols, bool strip = true );
	int long read( void* const, int long const& );
	int long write( void const* const, int long const& );
	static char const* const eols;
private:
	virtual int long do_write( void const* const, int long const& ) = 0;
	virtual int long do_read( void* const, int long const& ) = 0;
	virtual void do_flush( void ) const = 0;
	friend HStreamInterface& endl( HStreamInterface& );
	friend HStreamInterface& flush( HStreamInterface& );
	};

typedef HExceptionT<HStreamInterface> HStreamInterfaceException;

/*! \brief Iterator adaptor for streams.
 *
 * This helper may be useful in algorithms.
 *
 * \tparam stream_t - type of stream wrapped by this iterator.
 * \tparam delim_t - type of optional delimiter separating each iteration.
 */
template<typename stream_t, typename delim_t>
class HStreamIterator
	{
	stream_t& _stream;
	delim_t _delim;
public:
	explicit HStreamIterator( stream_t& stream, delim_t const& delim = delim_t() ) : _stream( stream ), _delim( delim ) {}
	HStreamIterator& operator* ( void )
		{ return ( *this ); }
	template<typename item_t>
	HStreamIterator& operator = ( item_t const& item )
		{
		_stream << item << _delim;
		return ( *this );
		}
	HStreamIterator& operator ++ ( void )
		{ return ( *this ); }
	};

template<typename stream_t, typename delim_t>
HStreamIterator<stream_t, delim_t> stream_iterator( stream_t& stream, delim_t delim )
	{ return ( HStreamIterator<stream_t, delim_t>( stream, delim ) ); }

template<typename stream_t>
HStreamIterator<stream_t, char const* const> stream_iterator( stream_t& stream )
	{ return ( HStreamIterator<stream_t, char const* const>( stream, "" ) ); }

}

}

#endif /* not HSTREAMINTERFACE_HXX_INCLUDED */


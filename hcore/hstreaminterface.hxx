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
HStreamInterface& dec( HStreamInterface& );
HStreamInterface& hex( HStreamInterface& );
HStreamInterface& oct( HStreamInterface& );

/*! \brief Interface for stream based IO.
 */
class HStreamInterface
	{
	typedef HStreamInterface self_t;
public:
	class HManipulator
		{
		int _value;
		typedef void ( HManipulator::* ACTION_t )( HStreamInterface& ) const;
		ACTION_t _action;
	public:
		HManipulator( int, ACTION_t );
		void operator()( HStreamInterface& ) const;
		void set_fill( HStreamInterface& ) const;
		void set_width( HStreamInterface& ) const;
		};
	typedef HPointer<HStreamInterface> ptr_t;
	struct BASES
		{
		typedef enum
			{
			DEC,
			HEX,
			OCT
			} enum_t;
		};
	typedef HStreamInterface& ( *manipulator_t )( HStreamInterface& );
protected:
	typedef yaal::hcore::HPool<char> cache_t;
	cache_t f_oCache; /*!< Read buffer. */
	int f_iOffset; /*!< Position of where continued read (another read_until invocation after interrupted one) shall store consecutive bytes. */
	HString f_oWordCache; /*!< Cache for operator >> () and operator << (). */
	int _fill; /*!< Fill character for output operations. */
	int _width; /*!< Next output operation width. */
	BASES::enum_t _base;
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
	HStreamInterface& operator << ( manipulator_t const& );
	HStreamInterface& operator << ( HManipulator const& );
	HStreamInterface& operator >> ( HString& );
	HStreamInterface& operator >> ( char& );
	HStreamInterface& operator >> ( int short& );
	HStreamInterface& operator >> ( int short unsigned& );
	HStreamInterface& operator >> ( int& );
	HStreamInterface& operator >> ( int unsigned& );
	HStreamInterface& operator >> ( int long& );
	HStreamInterface& operator >> ( int long unsigned& );
	HStreamInterface& operator >> ( double& );
	HStreamInterface& operator >> ( double long& );
	HStreamInterface& operator >> ( float& );
	HStreamInterface& operator >> ( void const*& );
	/*! \brief Read data from stream until end of it or until delimiter is encountered.
	 *
	 * \param store - Store read date here.
	 * \param delim - Stop reading data at any character in this set.
	 * \param strip - Remove delimiting stop char from output buffer.
	 * \return number of bytes read.
	 */
	int long read_until( yaal::hcore::HString& store, char const* const delim = eols, bool strip = true );
	/*! \brief Read data from stream until end of it or until delimiter is encountered or enough data has been acquired.
	 *
	 * \param store - Store read date here.
	 * \param maxcount - A maximum number of bytes to read.
	 * \param delim - Stop reading data at any character in this set.
	 * \param strip - Remove delimiting stop char from output buffer.
	 * \return number of bytes read.
	 */
	int long read_until_n( yaal::hcore::HString& store, int long const& maxcount, char const* const delim = eols, bool strip = true );
	int long read( void* const, int long const& );
	int long write( void const* const, int long const& );
	static char const* const eols;
	bool is_valid( void ) const;
	HStreamInterface& set_fill( int );
	HStreamInterface& set_width( int );
	HStreamInterface& set_base( BASES::enum_t );
private:
	bool read_word( void );
	int long reformat( void );
	virtual int long do_write( void const* const, int long const& ) = 0;
	virtual int long do_read( void* const, int long const& ) = 0;
	virtual void do_flush( void ) const = 0;
	virtual bool do_is_valid( void ) const = 0;
	friend HStreamInterface& endl( HStreamInterface& );
	friend HStreamInterface& flush( HStreamInterface& );
	};

HStreamInterface::HManipulator setfill( int );
HStreamInterface::HManipulator setw( int );

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


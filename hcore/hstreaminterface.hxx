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
#include "hcore/hchunk.hxx"
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
public:
	typedef HStreamInterface this_type;
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
	HChunk _cache; /*!< Read buffer. */
	int _offset; /*!< Position of where continued read (another read_until invocation after interrupted one) shall store consecutive bytes. */
	HString _wordCache; /*!< Cache for operator >> () and operator << (). */
	int _fill; /*!< Fill character for output operations. */
	int _width; /*!< Next output operation width. */
	BASES::enum_t _base;
public:
	HStreamInterface( void );
	virtual ~HStreamInterface( void );
	HStreamInterface& operator << ( HString const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( char const* const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( char const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int short const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int short unsigned const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int unsigned const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int long const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( int long unsigned const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( double const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( double long const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( float const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( void const* const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( manipulator_t const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator << ( HManipulator const& val_ )
		{ return ( do_output( val_ ) ); }
	HStreamInterface& operator >> ( HString&  val_)
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( char& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int short& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int short unsigned& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int unsigned& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int long& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( int long unsigned& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( double& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( double long& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( float& val_ )
		{ return ( do_input( val_ ) ); }
	HStreamInterface& operator >> ( void const*& val_ )
		{ return ( do_input( val_ ) ); }
	/*! \brief Read data from stream until end of it or until delimiter is encountered.
	 *
	 * \param store - Store read date here.
	 * \param delim - Stop reading data at any character in this set.
	 * \param strip - Remove delimiting stop char from output buffer.
	 * \return number of bytes read.
	 */
	int long read_until( yaal::hcore::HString& store, char const* const delim = eols, bool strip = true )
		{ return ( do_read_until( store, delim, strip ) ); }
	/*! \brief Read data from stream until end of it or until delimiter is encountered or enough data has been acquired.
	 *
	 * \param store - Store read date here.
	 * \param maxcount - A maximum number of bytes to read.
	 * \param delim - Stop reading data at any character in this set.
	 * \param strip - Remove delimiting stop char from output buffer.
	 * \return number of bytes read.
	 */
	int long read_until_n( yaal::hcore::HString& store, int long const& maxcount, char const* const delim = eols, bool strip = true )
		{ return ( do_read_until_n( store, maxcount, delim, strip ) ); }
	int long read( void* const, int long const& );
	int long write( void const* const, int long const& );
	M_YAAL_HCORE_PUBLIC_API static char const* const eols;
	bool is_valid( void ) const;
	void flush( void ) const;
	HStreamInterface& set_fill( int val_ )
		{ return ( do_set_fill( val_ ) ); }
	HStreamInterface& set_width( int val_ )
		{ return ( do_set_width( val_ ) ); }
	HStreamInterface& set_base( BASES::enum_t val_ )
		{ return ( do_set_base( val_ ) ); }
protected:
	virtual HStreamInterface& do_output( HString const& );
	virtual HStreamInterface& do_output( char const* const& );
	virtual HStreamInterface& do_output( char const& );
	virtual HStreamInterface& do_output( int short const& );
	virtual HStreamInterface& do_output( int short unsigned const& );
	virtual HStreamInterface& do_output( int const& );
	virtual HStreamInterface& do_output( int unsigned const& );
	virtual HStreamInterface& do_output( int long const& );
	virtual HStreamInterface& do_output( int long unsigned const& );
	virtual HStreamInterface& do_output( double const& );
	virtual HStreamInterface& do_output( double long const& );
	virtual HStreamInterface& do_output( float const& );
	virtual HStreamInterface& do_output( void const* const& );
	virtual HStreamInterface& do_output( manipulator_t const& );
	virtual HStreamInterface& do_output( HManipulator const& );
	virtual HStreamInterface& do_input( HString& );
	virtual HStreamInterface& do_input( char& );
	virtual HStreamInterface& do_input( int short& );
	virtual HStreamInterface& do_input( int short unsigned& );
	virtual HStreamInterface& do_input( int& );
	virtual HStreamInterface& do_input( int unsigned& );
	virtual HStreamInterface& do_input( int long& );
	virtual HStreamInterface& do_input( int long unsigned& );
	virtual HStreamInterface& do_input( double& );
	virtual HStreamInterface& do_input( double long& );
	virtual HStreamInterface& do_input( float& );
	virtual HStreamInterface& do_input( void const*& );
	virtual int long do_read_until( yaal::hcore::HString&, char const* const, bool );
	virtual int long do_read_until_n( yaal::hcore::HString&, int long const&, char const* const, bool );
	virtual HStreamInterface& do_set_fill( int );
	virtual HStreamInterface& do_set_width( int );
	virtual HStreamInterface& do_set_base( BASES::enum_t );
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


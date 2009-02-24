/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hformat.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HFORMAT_HXX_INCLUDED
#define YAAL_HCORE_HFORMAT_HXX_INCLUDED

#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief printf() like but type safe string formating.
 */
class HFormat
	{
	typedef HFormat self_t;
	class HFormatImpl;
	typedef HPointer<HFormatImpl> format_impl_ptr_t;
	format_impl_ptr_t _impl;
public:
	HFormat( char const* const = "" );
	HFormat( format_impl_ptr_t );
	HFormat( HFormat const& );
	HFormat& operator = ( HFormat const& );
	void swap( HFormat& );
	HFormat operator % ( char const& );
	HFormat operator % ( char unsigned const& );
	HFormat operator % ( int short const& );
	HFormat operator % ( int short unsigned const& );
	HFormat operator % ( int const& );
	HFormat operator % ( int unsigned const& );
	HFormat operator % ( int long const& );
	HFormat operator % ( int long unsigned const& );
	HFormat operator % ( float const& );
	HFormat operator % ( double const& );
	HFormat operator % ( double long const& );
	HFormat operator % ( char const* const& );
	HFormat operator % ( HString const& );
	HFormat operator % ( void const* const& );
	HString string( void ) const;
	HString format( void ) const;
	};

typedef HExceptionT<HFormat> HFormatException;

class HStreamFormatProxy
	{
	class HStreamFormatProxyImpl
		{
		HFormat _format;
		HStreamInterface& _stream;
		HStreamFormatProxyImpl( HStreamInterface& s, HFormat const& f ) : _format( f ), _stream( s ) {}
		friend class HStreamFormatProxy;
		};
	typedef HPointer<HStreamFormatProxyImpl> stream_format_proxy_impl_t;
	stream_format_proxy_impl_t _impl;
public:
	HStreamFormatProxy( HStreamInterface& s, HFormat const& f ) : _impl( new HStreamFormatProxyImpl( s, f ) ) {}
	template<typename tType>
	HStreamFormatProxy operator % ( tType const& v )
		{
		M_PROLOG
		_impl->_format % v;
		return ( *this );
		M_EPILOG
		}
	template<typename tType>
	HStreamInterface& operator << ( tType const& v )
		{
		M_PROLOG
		_impl->_stream << _impl->_format.string() << v;
		return ( _impl->_stream );
		M_EPILOG
		}
	};

HStreamFormatProxy operator << ( HStreamInterface&, HFormat const& );

}

inline void swap( yaal::hcore::HFormat& a, yaal::hcore::HFormat& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HFORMAT_HXX_INCLUDED */


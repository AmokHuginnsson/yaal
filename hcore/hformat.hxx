/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hformat.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hformat.hxx
 * \brief HFormat and HStreamFormatProxy classes declarations.
 */

#ifndef YAAL_HCORE_HFORMAT_HXX_INCLUDED
#define YAAL_HCORE_HFORMAT_HXX_INCLUDED 1

#include "hcore/memory.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace hcore {

/*! \brief printf() like but type safe string formating.
 */
class HFormat {
	typedef HFormat this_type;
	class HFormatImpl;
	typedef HPointer<HFormatImpl> format_impl_ptr_t;
	format_impl_ptr_t _impl;
public:
	HFormat( char const* const = "" );
	HFormat( format_impl_ptr_t );
	HFormat( HFormat const& );
	HFormat& operator = ( HFormat const& );
	void swap( HFormat& );
	HFormat operator % ( char );
	HFormat operator % ( char unsigned );
	HFormat operator % ( int short );
	HFormat operator % ( int short unsigned );
	HFormat operator % ( int );
	HFormat operator % ( int unsigned );
	HFormat operator % ( int long );
	HFormat operator % ( int long unsigned );
	HFormat operator % ( int long long );
	HFormat operator % ( int long long unsigned );
	HFormat operator % ( float );
	HFormat operator % ( double );
	HFormat operator % ( double long );
	HFormat operator % ( char const* );
	HFormat operator % ( HString const& );
	HFormat operator % ( void const* );
	HString string( void ) const;
	HString format( void ) const;
};

typedef HExceptionT<HFormat> HFormatException;

/*! \brief Proxy between HFormat and HStreamInterface classes.
 *
 * Example usage:
 * \code
 * cout << HFormat( "%.15Lf" ) % 3.141592653589793 << endl;
 * \endcode
 */
class HStreamFormatProxy {
	class HStreamFormatProxyImpl {
		HFormat _format;
		HStreamInterface& _stream;
		HStreamFormatProxyImpl( HStreamInterface& s, HFormat const& f ) : _format( f ), _stream( s ) {}
		friend class HStreamFormatProxy;
	};
	typedef HPointer<HStreamFormatProxyImpl> stream_format_proxy_impl_t;
	stream_format_proxy_impl_t _impl;
public:
	HStreamFormatProxy( HStreamInterface& s, HFormat const& f ) : _impl( new ( memory::yaal ) HStreamFormatProxyImpl( s, f ) ) {}
	template<typename tType>
	HStreamFormatProxy operator % ( tType const& v ) {
		M_PROLOG
		_impl->_format % v;
		return ( *this );
		M_EPILOG
	}
	template<typename tType>
	HStreamInterface& operator << ( tType const& v ) {
		M_PROLOG
		_impl->_stream << _impl->_format.string() << v;
		return ( _impl->_stream );
		M_EPILOG
	}
};

HString str( HFormat const& );

}

yaal::hcore::HStreamFormatProxy operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HFormat const& );

inline void swap( yaal::hcore::HFormat& a, yaal::hcore::HFormat& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HFORMAT_HXX_INCLUDED */


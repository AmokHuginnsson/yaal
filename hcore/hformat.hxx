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

/*! \brief printf() like but type safe string formatting.
 */
class HFormat {
	typedef HFormat this_type;
	class HFormatImpl;
	typedef HPointer<HFormatImpl> format_impl_ptr_t;
	format_impl_ptr_t _impl;
public:
	HFormat( HString const& = HString() );
	HFormat( HFormat const& );
	HFormat( HFormat&& );
	HFormat& operator = ( HFormat const& );
	HFormat& operator = ( HFormat&& );
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
	HFormat operator % ( code_point_t );
	HFormat operator % ( HString const& );
	HFormat operator % ( void const* );
	HString string( void ) const;
	HString format( void ) const;
private:
	HFormat( format_impl_ptr_t );
};

/*! \brief Simplify creation of printf like interface in library classes.
 */
class HFormatter {
private:
	mutable HFormat _format;
protected:
	HFormatter( void )
		: _format() {
	}
	virtual ~HFormatter( void ) {}
	template<typename... item_t>
	yaal::hcore::HString do_format( yaal::hcore::HString const& format_, item_t const&... item_ ) const {
		_format = HFormat( format_ );
		format_impl( item_... );
		return ( _format.string() );
	}
private:
	template<typename head_t, typename... item_t>
	void format_impl( head_t const& head_, item_t const&... item_ ) const {
		M_PROLOG
		_format % head_;
		format_impl( item_... );
		return;
		M_EPILOG
	}
	void format_impl( void ) const {
		M_PROLOG
		return;
		M_EPILOG
	}
public:
	template<typename head_t, typename... item_t>
	static void format_impl( HFormat& format_, head_t const& head_, item_t const&... item_ ) {
		M_PROLOG
		format_ % head_;
		format_impl( format_, item_... );
		return;
		M_EPILOG
	}
	static void format_impl( HFormat& ) {
		M_PROLOG
		return;
		M_EPILOG
	}
};

template<typename... item_t>
yaal::hcore::HString format( yaal::hcore::HString const& format_, item_t const&... item_ ) {
	M_PROLOG
	HFormat f( format_ );
	HFormatter::format_impl( f, item_... );
	return ( f.string() );
	M_EPILOG
}

typedef HExceptionT<HFormat> HFormatException;

HFormat operator "" _yf ( char const*, size_t );

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
		HStreamFormatProxyImpl( HStreamInterface& s, HFormat const& f )
			: _format( f )
			, _stream( s ) {
		}
		friend class HStreamFormatProxy;
	};
	typedef HPointer<HStreamFormatProxyImpl> stream_format_proxy_impl_t;
	stream_format_proxy_impl_t _impl;
public:
	HStreamFormatProxy( HStreamInterface& s, HFormat const& f )
		: _impl( new ( memory::yaal ) HStreamFormatProxyImpl( s, f ) ) {
	}
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

yaal::hcore::HStreamFormatProxy operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HFormat const& );

inline void swap( yaal::hcore::HFormat& a, yaal::hcore::HFormat& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HFORMAT_HXX_INCLUDED */


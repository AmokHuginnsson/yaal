/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hformat.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hformat.hxx"
#include "hlist.hxx"

namespace yaal
{

namespace hcore
{

class HFormat::HFormatImpl
	{
	struct CONVERSION
		{
		typedef enum
			{
			D_EMPTY,
			D_INT,
			D_FLOAT,
			D_DOUBLE,
			D_STRING,
			D_POINTER,
			D_CONSTANT
			} converion_t;
		};
	struct FLAG
		{
		typedef enum
			{
			D_NONE = 0,
			D_ALTERNATE = 1,
			D_ZERO_PADDED = 2,
			D_LEFT_ADJUST = 4,
			D_SPACE_PADDED = 8,
			D_SIGN_PREFIX = 16
			} flag_t;
		};
	struct OToken
		{
		CONVERSION::converion_t _conversion;
		FLAG::flag_t _flag;
		int _position;
		int _width;
		int _precision;
		HString _const;
		OToken( void ) : _conversion( CONVERSION::D_EMPTY ), _flag( FLAG::D_NONE ), _position( 0 ), _width( 0 ), _precision( 0 ), _const() {}
		};
	typedef HList<OToken> tokens_t;
	typedef HPointer<tokens_t> tokens_ptr_t;
	int _tokenIndex;
	HString _format;
	HString _string;
	HFormatImpl( char const* const = "" );
	HFormatImpl( HFormatImpl const& );
	HFormatImpl& operator = ( HFormatImpl const& );
	void swap( HFormatImpl& );
	OToken next_token( void );
	friend class HFormat;
	static bool has_conversion( HString const&, int const& );
	static OToken next_conversion( HString const&, int& );
	static bool has_constant( HString const&, int const& );
	static OToken next_constant( HString const&, int& );
	static bool has_position( HString const&, int const& );
	static int get_position( HString const&, int& );
	static bool has_flag( HString const&, int const& );
	static FLAG::flag_t get_flag( HString const&, int& );
	static bool has_width( HString const&, int const& );
	static int get_width( HString const&, int& );
	static bool has_precision( HString const&, int const& );
	static int get_precision( HString const&, int& );
	static CONVERSION::converion_t get_conversion( HString const&, int& );
	};

HFormat::HFormatImpl::HFormatImpl( char const* const fmt )
	: _tokenIndex( 0 ), _format( fmt ), _string()
	{
	}

HFormat::HFormatImpl::HFormatImpl( HFormatImpl const& fi )
	: _tokenIndex( fi._tokenIndex ), _format( fi._format ), _string( fi._string )
	{
	}

HFormat::HFormatImpl& HFormat::HFormatImpl::operator = ( HFormat::HFormatImpl const& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		HFormatImpl tmp( fi );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HFormat::HFormatImpl::swap( HFormat::HFormatImpl& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		using yaal::swap;
		swap( _tokenIndex, fi._tokenIndex );
		swap( _format, fi._format );
		swap( _string, fi._string );
		}
	return;
	M_EPILOG
	}

HFormat::HFormat( char const* const aFmt )
	: _impl()
	{
	M_PROLOG
	HString fmt( aFmt );
	HFormatImpl::tokens_ptr_t tokens( new HFormatImpl::tokens_t );
	HFormatImpl::OToken t;
	int idx = -1;
	do
		{
		t = HFormatImpl::OToken();
		if ( HFormatImpl::has_conversion( fmt, idx ) )
			t = HFormatImpl::next_conversion( fmt, idx );
		else if ( HFormatImpl::has_constant( fmt, idx ) )
			t = HFormatImpl::next_constant( fmt, idx );
		tokens->push_back( t );
		}
	while ( t._conversion != HFormatImpl::CONVERSION::D_EMPTY );
	M_EPILOG
	}

HFormat::HFormat( format_impl_ptr_t fi )
	: _impl( fi )
	{
	}

HFormat::HFormat( HFormat const& fi )
	: _impl( new HFormat::HFormatImpl( *(fi._impl) ) )
	{
	}

HFormat& HFormat::operator = ( HFormat const& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		HFormat tmp( fi );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HFormat::swap( HFormat& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		using yaal::swap;
		swap( _impl, fi._impl );
		}
	return;
	M_EPILOG
	}

HString HFormat::string( void ) const
	{
	return ( _impl->_string );
	}

HString HFormat::format( void ) const
	{
	return ( _impl->_format );
	}

HFormat HFormat::operator % ( int const& i )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	HFormatImpl::OToken t = _impl->next_token();
	M_ENSURE( t._conversion == HFormatImpl::CONVERSION::D_INT );
	_impl->_string += i;
	return ( _impl );
	M_EPILOG
	}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_token( void )
	{
	M_PROLOG
	return ( OToken() );
	M_EPILOG
	}

HStreamInterface& operator << ( HStreamInterface& stream, HFormat const& format )
	{
	M_PROLOG
	return ( stream << format.string() );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_conversion( HString const& s, int const& i )
	{
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] == '%' ) );
	M_EPILOG
	}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_conversion( HString const& s, int& i )
	{
	M_PROLOG
	OToken t;
	++ i;
	if ( has_position( s, i ) )
		t._position = get_position( s, i );
	if ( has_flag( s, i ) )
		t._flag = get_flag( s, i );
	if ( has_width( s, i ) )
		t._width = get_width( s, i );
	if ( has_precision( s, i ) )
		t._precision = get_precision( s, i );
	t._conversion = get_conversion( s, i );
	return ( t );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_constant( HString const& s, int const& i )
	{
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] != '%' ) );
	M_EPILOG
	}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_constant( HString const&, int& )
	{
	M_PROLOG
	OToken t;
	return ( t );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_flag( HString const& s, int const& i )
	{
	M_PROLOG
	HString flags = "#0- +";
	return ( ( i < s.get_length() ) && ( flags.find( s[ i ] ) >= 0 ) );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_precision( HString const& s, int const& i )
	{
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] == '.' ) );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_position( HString const& s, int const& i )
	{
	M_PROLOG
	if ( ( i + 1 ) < s.get_length() )
		{
		}
	return ( false );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_width( HString const&, int const& )
	{
	M_PROLOG
	return ( false );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_position( HString const&, int& )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_width( HString const&, int& )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_precision( HString const&, int& )
	{
	M_PROLOG
	return ( 0 );
	M_EPILOG
	}

HFormat::HFormatImpl::CONVERSION::converion_t HFormat::HFormatImpl::get_conversion( HString const&, int& )
	{
	M_PROLOG
	return ( CONVERSION::D_EMPTY );
	M_EPILOG
	}

HFormat::HFormatImpl::FLAG::flag_t HFormat::HFormatImpl::get_flag( HString const&, int& )
	{
	M_PROLOG
	return ( FLAG::D_NONE );
	M_EPILOG
	}

}

}


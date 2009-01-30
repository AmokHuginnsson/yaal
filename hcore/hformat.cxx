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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hformat.hxx"
#include "hlist.hxx"
#include "hvariant.hxx"

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
			D_EMPTY =       0x0000,
			D_INT =         0x0001,
			D_DOUBLE =      0x0002,
			D_STRING =      0x0004,
			D_CHAR =        0x0008,
			D_POINTER =     0x0010,
			D_CONSTANT =    0x0020,
			D_BYTE =        0x0040,
			D_SHORT =       0x0080,
			D_LONG =        0x0100,
			D_LONG_LONG =   0x0200,
			D_UNSIGNED =    0x0400,
			D_OCTAL =       0x0800,
			D_HEXADECIMAL = 0x1000
			} converion_t;
		};
	struct FLAG
		{
		typedef enum
			{
			D_NONE = 0,
			D_ALTERNATE = 1,
			D_ZERO_PADDED = 2,
			D_LEFT_ALIGNED = 4,
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
	typedef HVariant<bool, char, int short, int, int long, void*, double, double long, HString> format_arg_t;
	typedef HList<format_arg_t> args_t;
	int _tokenIndex;
	HString _format;
	HString _string;
	tokens_t _tokens;
	args_t _args;
	HFormatImpl( char const* const );
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
	: _tokenIndex( 0 ), _format( fmt ), _string(), _tokens(), _args()
	{
	}

HFormat::HFormatImpl::HFormatImpl( HFormatImpl const& fi )
	: _tokenIndex( fi._tokenIndex ), _format( fi._format ), _string( fi._string ), _tokens( fi._tokens ), _args( fi._args )
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
		swap( _tokens, fi._tokens );
		swap( _args, fi._args );
		}
	return;
	M_EPILOG
	}

HFormat::HFormat( char const* const aFmt )
	: _impl( new HFormatImpl( aFmt ) )
	{
	M_PROLOG
	HString fmt( aFmt );
	HFormatImpl::OToken t;
	int idx = -1;
	do
		{
		t = HFormatImpl::OToken();
		if ( HFormatImpl::has_conversion( fmt, idx ) )
			t = HFormatImpl::next_conversion( fmt, idx );
		else if ( HFormatImpl::has_constant( fmt, idx ) )
			t = HFormatImpl::next_constant( fmt, idx );
		_impl->_tokens.push_back( t );
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
	M_ENSURE( t._conversion & HFormatImpl::CONVERSION::D_INT );
	_impl->_string += i;
	_impl->_args.push_back( i );
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

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_constant( HString const& s, int& i )
	{
	M_PROLOG
	OToken t;
	int token = static_cast<int>( s.find( '%' ) );
	t._const = s.mid( i, token > 0 ? token - i : INT_MAX );
	i += static_cast<int>( t._const.get_length() );
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
	bool hasPosition = false;
	int long len = s.get_length();
	if ( ( i + 1 ) < len ) /* + 1 is for '$' character */
		{
		int k = i;
		while ( ( s[ k ] >= '0' ) && ( s[ k ] <= '9' ) && ( k < len ) )
			++ k;
		if ( ( k < len ) && ( s[ k ] == '$' ) )
			hasPosition = true;
		}
	return ( hasPosition );
	M_EPILOG
	}

bool HFormat::HFormatImpl::has_width( HString const& s, int const& i )
	{
	M_PROLOG
	return ( ( i < s.get_length() ) && ( ( s[ i ] == '*' ) || ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) ) ) );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_position( HString const& s, int& i )
	{
	M_PROLOG
	M_ENSURE( ( i + 1 ) < s.get_length() );
	int position = lexical_cast<int>( s.mid( i ) );
	while ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) )
		++ i;
	++ i; /* for '$' character. */
	return ( position );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_width( HString const& s, int& i )
	{
	M_PROLOG
	int width = 0;
	if ( s[ i ] == '*' )
		{
		++ i; /* skip '*' character. */
		if ( has_position( s, i ) )
			width = - ( get_position( s, i ) + 1 );
		else
			width = -1;
		}
	else
		{
		width = get_position( s, i );
		-- i; /* width is in place, no '$' character at the end of width. */
		}
	return ( width );
	M_EPILOG
	}

int HFormat::HFormatImpl::get_precision( HString const& s, int& i )
	{
	M_PROLOG
	++ i; /* skip '.' character */
	return ( get_width( s, i ) );
	M_EPILOG
	}

HFormat::HFormatImpl::CONVERSION::converion_t HFormat::HFormatImpl::get_conversion( HString const& s, int& i )
	{
	M_PROLOG
	CONVERSION::converion_t conversion = CONVERSION::D_EMPTY;
	M_ENSURE( i < s.get_length() );
	struct
		{
		char const* _label;
		CONVERSION::converion_t _converion;
		} length[] = { { "hh", CONVERSION::D_BYTE },
				{ "h", CONVERSION::D_SHORT },
				{ "ll", CONVERSION::D_LONG_LONG },
				{ "l", CONVERSION::D_LONG },
				{ "L", CONVERSION::D_LONG } };
	int lenMod = -1;
	for ( size_t k = 0; k < ( sizeof ( length ) / sizeof ( char const* ) ); ++ k )
		{
		if ( s.find( length[ k ]._label, i ) == i )
			{
			i += static_cast<int>( ::std::strlen( length[ k ]._label ) );
			lenMod = static_cast<int>( k );
			break;
			}
		}
	M_ENSURE( i < s.get_length() );
	switch ( s[ i ] )
		{
		case ( 'd' ):
		case ( 'i' ):
			conversion = CONVERSION::D_INT;
		break;
		case ( 'u' ):
			conversion = CONVERSION::D_INT | CONVERSION::D_UNSIGNED;
		break;
		case ( 'o' ):
			conversion = CONVERSION::D_INT | CONVERSION::D_OCTAL;
		break;
		case ( 'x' ):
		case ( 'X' ):
			conversion = CONVERSION::D_INT | CONVERSION::D_HEXADECIMAL;
		break;
		case ( 'f' ):
		case ( 'F' ):
		case ( 'e' ):
		case ( 'E' ):
		case ( 'g' ):
		case ( 'G' ):
			conversion = CONVERSION::D_DOUBLE;
		break;
		case ( 'c' ):
			conversion = CONVERSION::D_CHAR;
		break;
		case ( 's' ):
			conversion = CONVERSION::D_STRING;
		break;
		case ( 'p' ):
			conversion = CONVERSION::D_POINTER;
		break;
		case ( '%' ):
			{
			if ( s[ i - 1 ] == '%' )
				break;
			}
		default: M_THROW( "bad conversion", s[ i ]  ); break;
		}
	char const* const E_BAD_LEN_MOD = _( "bad length mod." );
	switch ( lenMod )
		{
		case ( 0 ):
		case ( 1 ):
		case ( 2 ):
		case ( 3 ):
			{
			if ( conversion & CONVERSION::D_INT )
				conversion |= length[ lenMod ]._converion;
			else
				M_THROW( E_BAD_LEN_MOD, lenMod );
			}
		case ( 4 ):
			{
			if ( conversion & CONVERSION::D_DOUBLE )
				conversion |= CONVERSION::D_LONG;
			else
				M_THROW( E_BAD_LEN_MOD, lenMod );
			}
		break;
		}
	++ i;
	return ( conversion );
	M_EPILOG
	}

HFormat::HFormatImpl::FLAG::flag_t HFormat::HFormatImpl::get_flag( HString const& s, int& i )
	{
	M_PROLOG
	int long len = s.get_length();
	FLAG::flag_t flag = FLAG::D_NONE;
	bool done = false;
	for ( ; ! done && ( i < len ); ++ i )
		{
		switch ( s[ i ] )
			{
			case ( '#' ): flag |= FLAG::D_ALTERNATE; break;
			case ( '0' ): flag |= FLAG::D_ZERO_PADDED; break;
			case ( '-' ): flag |= FLAG::D_LEFT_ALIGNED; break;
			case ( ' ' ): flag |= FLAG::D_SPACE_PADDED; break;
			case ( '+' ): flag |= FLAG::D_SIGN_PREFIX; break;
			default:
				done = true;
			break;
			}
		}
	return ( flag );
	M_EPILOG
	}

}

}


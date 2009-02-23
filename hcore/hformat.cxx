/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hformat.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hformat.hxx"
#include "hlist.hxx"
#include "hmap.hxx"
#include "hset.hxx"
#include "hvariant.hxx"
#include "iterator.hxx"

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
		OToken( void )
			: _conversion( CONVERSION::D_EMPTY ), _flag( FLAG::D_NONE ),
			_position( 0 ), _width( 0 ), _precision( 0 ), _const() {}
		};
	typedef HList<OToken> tokens_t;
	typedef HMap<int, OToken*> positions_t;
	typedef HPointer<positions_t> positions_ptr_t;
	typedef HVariant<bool, char, int short, int, int long, void const*, double, double long, HString> format_arg_t;
	typedef HMap<int, format_arg_t> args_t;
	typedef HPointer<args_t> args_ptr_t;
	int _tokenIndex;
	HString _format;
	mutable HString _buffer;
	mutable HString _string;
	tokens_t _tokens;
	positions_ptr_t _positions;
	tokens_t::const_iterator _token;
	args_ptr_t _args;
	HFormatImpl( char const* const );
	HFormatImpl( HFormatImpl const& );
	HFormatImpl& operator = ( HFormatImpl const& );
	void swap( HFormatImpl& );
	int next_token( CONVERSION::converion_t const& );
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
	: _tokenIndex( 0 ), _format( fmt ), _buffer(), _string(), _tokens(),
	_positions( new positions_t ), _token(), _args( new args_t )
	{
	}

HFormat::HFormatImpl::HFormatImpl( HFormatImpl const& fi )
	: _tokenIndex( fi._tokenIndex ), _format( fi._format ), _buffer( fi._buffer ),
	_string( fi._string ), _tokens( fi._tokens ), _positions( new positions_t ),
	_token( _tokens.begin() ), _args( new args_t )
	{
	advance( _token, distance( fi._tokens.begin(), fi._token ) );
	_positions->copy_from( *fi._positions );
	_args->copy_from( *fi._args );
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
		swap( _buffer, fi._buffer );
		swap( _string, fi._string );
		swap( _tokens, fi._tokens );
		swap( _token, fi._token );
		swap( _args, fi._args );
		}
	return;
	M_EPILOG
	}

namespace
{

class common
	{
	int long _count;
public:
	common( int long count = 0 ) : _count( count ) {}
	common& operator = ( int ) { return ( *this ); }
	common& operator* ( void ) { return ( *this ); }
	common& operator ++ ( void ) { ++ _count; return ( *this ); }
	int long operator()( void ) { return ( _count ); }
	void operator() ( int long count ) { _count = count; }
	} c;

template<typename iter1_t, typename iter2_t>
bool does_intersect( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	return ( set_intersection( it1, end1, it2, end2, c )() > 0 );
	}

}

HFormat::HFormat( char const* const aFmt )
	: _impl( new HFormatImpl( aFmt ) )
	{
	M_PROLOG
	HString fmt( aFmt );
	HFormatImpl::OToken t;
	int idx = 0;
	while ( true )
		{
		t = HFormatImpl::OToken();
		if ( HFormatImpl::has_conversion( fmt, idx ) )
			t = HFormatImpl::next_conversion( fmt, idx );
		else if ( HFormatImpl::has_constant( fmt, idx ) )
			t = HFormatImpl::next_constant( fmt, idx );
		if ( t._conversion != HFormatImpl::CONVERSION::D_EMPTY )
			_impl->_tokens.push_back( t );
		else
			break;
		}
	bool anyTokenHaveIndex = false;
	bool firstToken = true;
	typedef HSet<int> idx_t;
	idx_t idxs;
	idx_t widthIdxs;
	idx_t precIdxs;
	int pos = 0;
	for ( HFormatImpl::tokens_t::iterator it = _impl->_tokens.begin(), end = _impl->_tokens.end(); it != end; ++ it, ++ pos )
		{
		if ( it->_conversion == HFormatImpl::CONVERSION::D_CONSTANT )
			{
			-- pos;
			continue;
			}
		bool thisTokenHasIndex = ( it->_position > 0 ) || ( it->_width < -1 ) || ( it->_precision < -1 );
		M_ENSURE( ! thisTokenHasIndex
				|| ( thisTokenHasIndex && ( it->_position > 0 ) && ( it->_width != -1 ) && ( it->_precision != -1 ) ) );
		M_ENSURE( firstToken || ( anyTokenHaveIndex && thisTokenHasIndex ) || ( ! ( anyTokenHaveIndex || thisTokenHasIndex ) ) );
		if ( it->_position > 0 )
			idxs.insert( it->_position );
		else
			{
			it->_position = pos;
			++ pos;
			}
		if ( it->_width == -1 )
			{
			it->_width = ( - pos ) - 1;
			++ pos;
			}
		if ( it->_width < -1 )
			widthIdxs.insert( - ( it->_width + 1 ) );
		if ( it->_precision == -1 )
			{
			it->_precision = ( - pos ) - 1;
			++ pos;
			}
		if ( it->_precision < -1 )
			precIdxs.insert( - ( it->_precision + 1 ) );
		if ( firstToken )
			anyTokenHaveIndex = thisTokenHasIndex;
		firstToken = false;
		++ pos;
		}
	int last = 0;
	M_ENSURE( ! does_intersect( widthIdxs.begin(), widthIdxs.end(), precIdxs.begin(), precIdxs.end() ) );
	M_ENSURE( ! does_intersect( widthIdxs.begin(), widthIdxs.end(), idxs.begin(), idxs.end() ) );
	M_ENSURE( ! does_intersect( idxs.begin(), idxs.end(), precIdxs.begin(), precIdxs.end() ) );
	copy( widthIdxs.begin(), widthIdxs.end(), insert_iterator( idxs ) );
	copy( precIdxs.begin(), precIdxs.end(), insert_iterator( idxs ) );
	for ( idx_t::iterator it = idxs.begin(); it != idxs.end(); ++ it, ++ last )
		M_ENSURE( *it == ( last + 1 ) );
	_impl->_token = _impl->_tokens.begin();
	return;
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

namespace
{

template<typename T>
struct variant_shell
	{
	static T get( HFormat::HFormatImpl::args_t const& args, int const& idx )
		{
		HFormat::HFormatImpl::args_t::const_iterator it = args.find( idx );
		M_ASSERT( it != args.end() );
		return ( it->second.get<T>() );
		}
	};

}

HString HFormat::string( void ) const
	{
	HString fmt;
	for ( HFormatImpl::tokens_t::const_iterator it = _impl->_tokens.begin(), end = _impl->_tokens.end(); it != end; ++ it )
		{
		HFormatImpl::CONVERSION::converion_t conv = it->_conversion;
		if ( conv == HFormatImpl::CONVERSION::D_CONSTANT )
			_impl->_string += it->_const;
		else
			{
			fmt = "%";
			if ( it->_flag & HFormatImpl::FLAG::D_ALTERNATE )
				fmt += "#";
			if ( it->_flag & HFormatImpl::FLAG::D_ZERO_PADDED )
				fmt += "0";
			if ( it->_flag & HFormatImpl::FLAG::D_SPACE_PADDED )
				fmt += " ";
			if ( it->_flag & HFormatImpl::FLAG::D_SIGN_PREFIX )
				fmt += "+";
			if ( it->_flag & HFormatImpl::FLAG::D_LEFT_ALIGNED )
				fmt += "-";
			if ( it->_width > 0 )
				fmt += it->_width;
			else if ( it->_width < 0 )
				{
				M_ASSERT( it->_width < -1 );
				fmt += variant_shell<int>::get( *_impl->_args, - ( it->_width + 1 ) );
				}
			if ( it->_precision )
				fmt += ".";
			if ( it->_precision > 0 )
				fmt += it->_precision;
			else if ( it->_precision < 0 )
				{
				M_ASSERT( it->_precision < -1 );
				fmt += variant_shell<int>::get( *_impl->_args, - ( it->_precision + 1 ) );
				}
			if ( conv & HFormatImpl::CONVERSION::D_BYTE )
				fmt += "hh";
			if ( conv & HFormatImpl::CONVERSION::D_SHORT )
				fmt += "h";
			if ( conv & HFormatImpl::CONVERSION::D_LONG )
				{
				if ( conv & HFormatImpl::CONVERSION::D_INT )
					fmt += "l";
				else
					{
					M_ASSERT( conv & HFormatImpl::CONVERSION::D_DOUBLE );
					fmt += "L";
					}
				}
			if ( conv & HFormatImpl::CONVERSION::D_LONG_LONG )
				fmt += "ll";
			if ( conv & HFormatImpl::CONVERSION::D_OCTAL )
				fmt += "o";
			else if ( conv & HFormatImpl::CONVERSION::D_HEXADECIMAL )
				fmt += "x";
			else if ( conv & HFormatImpl::CONVERSION::D_UNSIGNED )
				fmt += "u";
			else if ( conv & HFormatImpl::CONVERSION::D_INT )
				fmt += "d";
			if ( conv & HFormatImpl::CONVERSION::D_DOUBLE )
				fmt += "f";
			if ( conv & HFormatImpl::CONVERSION::D_STRING )
				fmt += "s";
			if ( conv & HFormatImpl::CONVERSION::D_CHAR )
				fmt += "c";
			if ( conv & ( HFormatImpl::CONVERSION::D_LONG | HFormatImpl::CONVERSION::D_LONG_LONG ) )
				{
				if ( conv & HFormatImpl::CONVERSION::D_DOUBLE )
					_impl->_buffer.format( fmt.raw(), variant_shell<double long>::get( *_impl->_args, it->_position ) );
				else
					{
					M_ASSERT( conv & HFormatImpl::CONVERSION::D_INT );
					_impl->_buffer.format( fmt.raw(), variant_shell<int long>::get( *_impl->_args, it->_position ) );
					}
				}
			else
				{
				if ( conv & HFormatImpl::CONVERSION::D_INT )
					{
					if ( conv & HFormatImpl::CONVERSION::D_SHORT )
						_impl->_buffer.format( fmt.raw(), variant_shell<int short>::get( *_impl->_args, it->_position ) );
					else
						_impl->_buffer.format( fmt.raw(), variant_shell<int>::get( *_impl->_args, it->_position ) );
					}
				else if ( conv & HFormatImpl::CONVERSION::D_STRING )
					_impl->_buffer.format( fmt.raw(), variant_shell<HString>::get( *_impl->_args, it->_position ).raw() );
				else if ( conv & HFormatImpl::CONVERSION::D_POINTER )
					_impl->_buffer.format( fmt.raw(), variant_shell<void*>::get( *_impl->_args, it->_position ) );
				else if ( conv & HFormatImpl::CONVERSION::D_CHAR )
					_impl->_buffer.format( fmt.raw(), variant_shell<char>::get( *_impl->_args, it->_position ) );
				}
			_impl->_string += _impl->_buffer;
			}
		}
	return ( _impl->_string );
	}

HString HFormat::format( void ) const
	{
	return ( _impl->_format );
	}

HFormat HFormat::operator % ( int short const& is )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT | HFormatImpl::CONVERSION::D_SHORT );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( is ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( int short unsigned const& isu )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT | HFormatImpl::CONVERSION::D_SHORT | HFormatImpl::CONVERSION::D_UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( isu ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( int const& i )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( i ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( int unsigned const& iu )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT | HFormatImpl::CONVERSION::D_UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( iu ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( int long const& il )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT | HFormatImpl::CONVERSION::D_LONG );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( il ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( int long unsigned const& ilu )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_INT | HFormatImpl::CONVERSION::D_LONG | HFormatImpl::CONVERSION::D_UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( ilu ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( double const& d )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_DOUBLE );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( d ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( double long const& dl )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_DOUBLE | HFormatImpl::CONVERSION::D_LONG );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( dl ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( float const& f )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_DOUBLE );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( f ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( char const* const& s )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_STRING );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( s ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( void const* const& p )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_POINTER );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( p ) ) );
	return ( _impl );
	M_EPILOG
	}

HFormat HFormat::operator % ( HString const& s )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::D_STRING );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( s ) ) );
	return ( _impl );
	M_EPILOG
	}

int HFormat::HFormatImpl::next_token( HFormatImpl::CONVERSION::converion_t const& conv )
	{
	M_PROLOG
	M_ENSURE( _token != _tokens.end() );
	while ( _token->_conversion == CONVERSION::D_CONSTANT )
		{
		++ _token;
		M_ENSURE( _token != _tokens.end() );
		}
	M_ENSURE( conv == _token->_conversion );
	++ _token;
	int idx = _tokenIndex;
	++ _tokenIndex;
	return ( idx );
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
	int token = static_cast<int>( s.find( '%', i ) );
	t._const = s.mid( i, token > 0 ? token - i : INT_MAX );
	i += static_cast<int>( t._const.get_length() );
	t._conversion = CONVERSION::D_CONSTANT;
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
	struct OLength
		{
		char const* _label;
		CONVERSION::converion_t _converion;
		} length[] = { { "hh", CONVERSION::D_BYTE },
				{ "h", CONVERSION::D_SHORT },
				{ "ll", CONVERSION::D_LONG_LONG },
				{ "l", CONVERSION::D_LONG },
				{ "L", CONVERSION::D_LONG } };
	int lenMod = -1;
	for ( size_t k = 0; k < ( sizeof ( length ) / sizeof ( OLength ) ); ++ k )
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
	-- i;
	return ( flag );
	M_EPILOG
	}

}

}


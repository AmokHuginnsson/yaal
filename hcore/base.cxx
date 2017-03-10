/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  base.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>
#include <climits>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )

#include "hstring.hxx"
#include "hformat.hxx"
#include "hcore.hxx"
#include "safe_int.hxx"

using namespace yaal::hcore;

namespace yaal {

exit_flag_t _isKilled_{ false };

bool is_hexadecimal( char const* str_, int len_ ) {
	if ( ( len_ >= 4 ) && ( str_[ 0 ] == '-' ) ) { /* -0x0 */
		++ str_;
		-- len_;
	}
	return ( ( len_ >= 3 )
			&& ( str_[ 0 ] == '0' )
			&& ( ( str_[1] == 'x' ) || ( str_[1] == 'X' ) )
			&& ( ( ( str_[2] >= '0' ) && ( str_[2] <= '9' ) )
				|| ( ( str_[2] >= 'a' ) && ( str_[2] <= 'f' ) )
				|| ( ( str_[2] >= 'A' ) && ( str_[2] <= 'F' ) ) ) );
}

template<>
bool is_hexadecimal( HString const& str_ ) {
	return ( is_hexadecimal( str_.raw(), static_cast<int>( str_.get_length() ) ) );
}

bool is_binary( char const* str_, int len_ ) {
	if ( ( len_ >= 4 ) && ( str_[ 0 ] == '-' ) ) { /* -0b0 */
		++ str_;
		-- len_;
	}
	return ( ( len_ >= 3 )
			&& ( str_[ 0 ] == '0' )
			&& ( ( str_[1] == 'b' ) || ( str_[1] == 'B' ) )
			&& ( ( ( str_[2] >= '0' ) && ( str_[2] <= '1' ) ) ) );
}

template<>
bool is_binary( HString const& str_ ) {
	return ( is_binary( str_.raw(), static_cast<int>( str_.get_length() ) ) );
}

bool is_octal( char const* str_, int len_ ) {
	if ( ( len_ >= 3 ) && ( str_[ 0 ] == '-' ) ) { /* -07 */
		++ str_;
		-- len_;
	}
	bool octal( false );
	if ( str_[0] == '0' ) {
		++ str_;
		-- len_;
		if ( str_[0] == 'o' ) {
			++ str_;
			-- len_;
		}
		if ( ( len_ > 0 ) && ( str_[0] >= '0' ) && ( str_[0] <= '7' ) ) {
			octal = true;
		}
	}
	return ( octal );
}

template<>
bool is_octal( HString const& str_ ) {
	return ( is_octal( str_.raw(), static_cast<int>( str_.get_length() ) ) );
}

static int const MAX_VALID_BINARY_INTEGER_LENGTH = 130; /* 128 bits + '-' + terminator */
static int const MAX_VALID_OCTAL_INTEGER_LENGTH = 46;

namespace {

HPair<int, char const*> preparse_integer( HString const& str_, char* alternate_ ) {
	typedef LexicalCast this_type;
	/* how to choose correct base:
	 *
	 * 0     -> 10
	 * 0Ala  -> 10
	 * 0xOla -> 10
	 * 0xAla -> 16
	 * 0x    -> 10
	 * 0x10b -> 16
	 * xOla  -> 10
	 * xAla  -> 10
	 * 0Ala  -> 10
	 * 01Ala ->  8
	 * 01    ->  8
	 * 0110b ->  2
	 * 0110c ->  8
	 * 1010c -> 10
	 * 1010b ->  2
	 */
	int base( 10 );
	char const* str( str_.raw() );
	int long len( str_.get_length() );
	if ( is_hexadecimal( str_ ) ) {
		base = 16;
	} else if ( is_binary( str_ ) ) {
		base = 2;
		char const* src( str );
		str = alternate_;
		int offset( 0 );
		if ( src[0] == '-' ) {
			alternate_[0] = '-';
			++ offset;
			++ src;
			-- len;
		}
		len -= 2;
		src += 2;
		if ( len >= MAX_VALID_BINARY_INTEGER_LENGTH ) {
			M_THROW( "too many binary digits: " + str_, len );
		}
		::memcpy( alternate_ + offset, src, static_cast<size_t>( len ) );
		alternate_[offset + len] = 0;
	} else if ( is_octal( str_ ) ) {
		base = 8;
		char const* src( str );
		str = alternate_;
		int offset( 0 );
		if ( src[0] == '-' ) {
			alternate_[offset] = '-';
			++ offset;
			++ src;
			-- len;
		}
		alternate_[offset] = '0';
		++ offset;
		-- len;
		++ src;
		if ( ( src[0] == 'o' ) || ( src[0] == 'O' ) ) {
			++ src;
			-- len;
		}
		if ( len >= MAX_VALID_OCTAL_INTEGER_LENGTH ) {
			M_THROW( "too many binary digits: " + str_, len );
		}
		::memcpy( alternate_ + offset, src, static_cast<size_t>( len ) );
		alternate_[offset + len] = 0;
	} else {
		char const* src( str );
		if ( ( len >= 2 ) && ( src[0] == '-' ) ) { /* -0 */
			++ src;
			-- len;
		}
		if ( ! ( ( len > 0 ) && ( src[0] >= '0' ) && ( src[0] <= '9' ) ) ) {
			M_THROW( "not a number: " + str_, 0 );
		}
		if ( src[0] == '0' ) {
			str = alternate_;
			alternate_[0] = '0';
			alternate_[1] = 0;
		}
	}
	HPair<int, char const*> ret;
	ret.first = base;
	ret.second = str;
	return ( ret );
}

}

namespace hcore {
int long long unsigned stoull_impl( char const*, int*, int );
}

template<>
int long long unsigned lexical_cast( HString const& str_ ) {
	M_PROLOG
	char alternateForm[ MAX_VALID_BINARY_INTEGER_LENGTH ];
	HPair<int, char const*> preParsed( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoull_impl( preParsed.second, nullptr, preParsed.first ) );
	M_EPILOG
}

template<>
int long long unsigned lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<int long long unsigned, HString>( val ) );
	M_EPILOG
}

template<>
int long unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int long unsigned>( lexical_cast<int long long unsigned>( val ) ) );
	M_EPILOG
}

template<>
int long unsigned lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int long unsigned>( lexical_cast<int long long unsigned, HString>( val ) ) );
	M_EPILOG
}

template<>
int unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int unsigned>( lexical_cast<int long long unsigned>( val ) ) );
	M_EPILOG
}

template<>
int unsigned lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int unsigned>( lexical_cast<int long long unsigned, HString>( val ) ) );
	M_EPILOG
}

template<>
int short unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int short unsigned>( lexical_cast<int long long unsigned>( val ) ) );
	M_EPILOG
}

template<>
int short unsigned lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int short unsigned>( lexical_cast<int long long unsigned, HString>( val ) ) );
	M_EPILOG
}

namespace hcore {
int long long stoll_impl( char const*, int*, int );
}

template<>
int long long lexical_cast( HString const& str_ ) {
	M_PROLOG
	char alternateForm[ MAX_VALID_BINARY_INTEGER_LENGTH ];
	HPair<int, char const*> preParsed( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoll_impl( preParsed.second, nullptr, preParsed.first ) );
	M_EPILOG
}

template<>
int long long lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<int long long, HString>( val ) );
	M_EPILOG
}

template<>
int long lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int long>( lexical_cast<int long long>( val ) ) );
	M_EPILOG
}

template<>
int long lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int long>( lexical_cast<int long long, HString>( val ) ) );
	M_EPILOG
}

template<>
int lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int>( lexical_cast<int long long>( val ) ) );
	M_EPILOG
}

template<>
int lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int>( lexical_cast<int long long, HString>( val ) ) );
	M_EPILOG
}

template<>
int short lexical_cast( HString const& val ) {
	M_PROLOG
	return ( safe_int::cast<int short>( lexical_cast<int long long>( val ) ) );
	M_EPILOG
}

template<>
int short lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( safe_int::cast<int short>( lexical_cast<int long long, HString>( val ) ) );
	M_EPILOG
}

template<>
double long lexical_cast( HString const& val ) {
	M_PROLOG
	return ( hcore::strtold( val ) );
	M_EPILOG
}

template<>
double long lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<double long, HString>( val ) );
	M_EPILOG
}

template<>
double lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<double>( lexical_cast<double long>( val ) ) );
	M_EPILOG
}

template<>
double lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<double, HString>( val ) );
	M_EPILOG
}

template<>
float lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<float>( lexical_cast<double long>( val ) ) );
	M_EPILOG
}

template<>
float lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<float, HString>( val ) );
	M_EPILOG
}

template<>
bool lexical_cast( char const* const& value_ ) {
	M_PROLOG
	static HString message;
	bool bVal( false );
	if ( ! ::strcasecmp( value_, "true" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "false" ) ) {
		bVal = false;
	} else if ( ! ::strcasecmp( value_, "yes" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "no" ) ) {
		bVal = false;
	} else if ( ! ::strcasecmp( value_, "on" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "off" ) ) {
		bVal = false;
	} else if ( ! ::strcasecmp( value_, "enable" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "disable" ) ) {
		bVal = false;
	} else if ( ! ::strcasecmp( value_, "enabled" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "disabled" ) ) {
		bVal = false;
	} else if ( ! ::strcasecmp( value_, "1" ) ) {
		bVal = true;
	} else if ( ! ::strcasecmp( value_, "0" ) ) {
		bVal = false;
	} else {
		message = "not a boolean value: ";
		message += value_;
		typedef LexicalCast this_type;
		M_THROW( message, bVal );
	}
	return ( bVal );
	M_EPILOG
}

template<>
bool lexical_cast( HString const& value_ ) {
	M_PROLOG
	return ( lexical_cast<bool>( value_.raw() ) );
	M_EPILOG
}

template<>
char lexical_cast( HString const& val ) {
	M_PROLOG
	return ( val.is_empty() ? static_cast<char>( 0 ) : val[ 0 ] );
	M_EPILOG
}

template<>
char const* lexical_cast( HString const& val ) {
	M_PROLOG
	return ( val.raw() );
	M_EPILOG
}

template<>
HString lexical_cast( HString const& val ) {
	M_PROLOG
	return ( val );
	M_EPILOG
}

template<>
HString lexical_cast( char const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( char unsigned const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int short const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int short unsigned const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int unsigned const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int long const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int long unsigned const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int long long const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( int long long unsigned const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( float const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( double const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( double long const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( void* const& val_ ) {
	M_PROLOG
	return ( val_ );
	M_EPILOG
}

template<>
HString lexical_cast( bool const& val_ ) {
	M_PROLOG
	return ( val_ ? "true" : "false" );
	M_EPILOG
}

template<>
HString lexical_cast( HFormat const& f ) {
	M_PROLOG
	return ( f.string() );
	M_EPILOG
}

char const* error_message( int code_ ) {
	return ( ::strerror( code_ ) );
}

template<>
bool is_hexadecimal( char const* const& str_ ) {
	return ( is_hexadecimal<HString>( str_ ) );
}

template<>
bool is_binary( char const* const& str_ ) {
	return ( is_binary<HString>( str_ ) );
}

template<>
bool is_octal( char const* const& str_ ) {
	return ( is_octal<HString>( str_ ) );
}

template<>
bool is_hexadecimal( char* const& val ) {
	return ( is_hexadecimal<char const*>( val ) );
}

template<>
bool is_binary( char* const& val ) {
	return ( is_binary<char const*>( val ) );
}

template<>
bool is_octal( char* const& val ) {
	return ( is_octal<char const*>( val ) );
}

}


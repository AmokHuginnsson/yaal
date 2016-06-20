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

using namespace yaal::hcore;

namespace yaal {

exit_flag_t _isKilled_{ false };

bool is_hexadecimal( char const* str_, int len_ ) {
	if ( ( len_ >= 4 ) && ( str_[ 0 ] == '-' ) ) { /* -0x0 */
		++ str_;
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
	if ( ( len_ >= 3 ) && ( str_[ 0 ] == '-' ) ) { /* -0b */
		++ str_;
	}
	int binaryMark( static_cast<int>( strspn( str_, "01" ) ) );
	return ( ( binaryMark > 0 ) && ( ( str_[binaryMark] == 'b' ) || ( str_[binaryMark] == 'B' ) ) );
}

template<>
bool is_binary( HString const& str_ ) {
	return ( is_binary( str_.raw(), static_cast<int>( str_.get_length() ) ) );
}

bool is_octal( char const* str_, int len_ ) {
	bool octal( false );
	if ( ! is_binary( str_, len_ ) ) {
		int offset( 0 );
		if ( ( len_ >= 3 ) && ( str_[ 0 ] == '-' ) ) { /* -01 */
			++ offset, ++ str_;
		}
		octal = ( len_ >= 2 ) && ( str_[0] == '0' ) && ( str_[1] >= '0' ) && ( str_[1] <= '7' );
	}
	return ( octal );
}

template<>
bool is_octal( HString const& str_ ) {
	return ( is_octal( str_.raw(), static_cast<int>( str_.get_length() ) ) );
}

static int const MAX_VALID_INTEGER_LENGTH = 32;

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
	if ( is_hexadecimal( str_ ) ) {
		base = 16;
	} else if ( is_binary( str_ ) ) {
		base = 2;
	} else if ( is_octal( str_ ) ) {
		base = 8;
	} else {
		int long len( str_.get_length() );
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
	char alternateForm[ MAX_VALID_INTEGER_LENGTH ];
	HPair<int, char const*> preParsed( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoull_impl( preParsed.second, nullptr, preParsed.first ) );
	M_EPILOG
}

template<>
int long unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int long unsigned>( lexical_cast<int long long unsigned>( val ) ) );
	M_EPILOG
}

template<>
int unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int unsigned>( lexical_cast<int long unsigned>( val ) ) );
	M_EPILOG
}

template<>
int short unsigned lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int short unsigned>( lexical_cast<int long unsigned>( val ) ) );
	M_EPILOG
}

namespace hcore {
int long long stoll_impl( char const*, int*, int );
}

template<>
int long long lexical_cast( HString const& str_ ) {
	M_PROLOG
	char alternateForm[ MAX_VALID_INTEGER_LENGTH ];
	HPair<int, char const*> preParsed( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoll_impl( preParsed.second, nullptr, preParsed.first ) );
	M_EPILOG
}

template<>
int long lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int long>( lexical_cast<int long long>( val ) ) );
	M_EPILOG
}

template<>
int long lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( lexical_cast<int long, HString>( val ) );
	M_EPILOG
}

template<>
int long lexical_cast( char* const& val ) {
	M_PROLOG
	return ( lexical_cast<int long, HString>( val ) );
	M_EPILOG
}

template<>
int lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int>( lexical_cast<int long>( val ) ) );
	M_EPILOG
}

template<>
int short lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<int short>( lexical_cast<int long>( val ) ) );
	M_EPILOG
}

template<>
int lexical_cast( char const* const& val ) {
	M_PROLOG
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	M_EPILOG
}

template<>
int lexical_cast( char* const& val ) {
	M_PROLOG
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	M_EPILOG
}

template<>
double long lexical_cast( HString const& val ) {
	M_PROLOG
	return ( hcore::strtold( val ) );
	M_EPILOG
}

template<>
float lexical_cast( HString const& val ) {
	M_PROLOG
	return ( static_cast<float>( lexical_cast<double long>( val ) ) );
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
bool lexical_cast( char const* const& value_ ) {
	M_PROLOG
	static HString message;
	bool bVal = false;
	if ( ! ::strcasecmp( value_, "yes" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "no" ) )
		bVal = false;
	else if ( ! ::strcasecmp( value_, "true" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "false" ) )
		bVal = false;
	else if ( ! ::strcasecmp( value_, "on" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "off" ) )
		bVal = false;
	else if ( ! ::strcasecmp( value_, "enable" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "disable" ) )
		bVal = false;
	else if ( ! ::strcasecmp( value_, "1" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "0" ) )
		bVal = false;
	else {
		message = "not a boolean value: ";
		message += value_;
		typedef LexicalCast this_type;
		M_THROW( message, bVal );
	}
	return ( bVal );
	M_EPILOG
}

template<>
bool lexical_cast( char* const& value_ ) {
	M_PROLOG
	return ( lexical_cast<bool, char const*>( value_ ) );
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
HString lexical_cast( int short const& val_ ) {
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
HString lexical_cast( int long const& val_ ) {
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

}


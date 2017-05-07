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
#include "unicode.hxx"

using namespace yaal::hcore;

namespace yaal {

exit_flag_t _isKilled_{ false };

template<>
bool is_hexadecimal( HString const& str_ ) {
	return ( is_hexadecimal( str_.begin(), str_.end() ) );
}

template<>
bool is_binary( HString const& str_ ) {
	return ( is_binary( str_.begin(), str_.end() ) );
}

template<>
bool is_octal( HString const& str_ ) {
	return ( is_octal( str_.begin(), str_.end() ) );
}

/* Be ready for 128 bit integers. */
static int const MAX_VALID_BINARY_INTEGER_LENGTH = 64 * 2;
static int const MAX_VALID_OCTAL_INTEGER_LENGTH = 22 * 2;
static int const MAX_VALID_DECIMAL_INTEGER_LENGTH = 20 * 2;
static int const MAX_VALID_HEXADECIMAL_INTEGER_LENGTH = 16 * 2;

namespace {

int preparse_integer( HString const& str_, char* alternate_ ) {
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
	HString::const_iterator it( str_.begin() );
	int len( safe_int::cast<int>( str_.get_length() ) );
	while ( ( len > 0 ) && _whiteSpace_.has( static_cast<u32_t>( *it ) ) ) { /* *TODO* *FIXME* Remove static_cast after UCS in HString is implemented. */
		++ it;
		-- len;
	}
	if ( ( len > 0 ) && ( *it == '-' ) ) {
		*alternate_ = '-';
		++ alternate_;
		++ it;
		-- len;
	}
	int base( 10 );
	int skip( 0 );
	if ( is_hexadecimal( it, str_.end() ) ) {
		base = 16;
		skip = 2;
	} else if ( is_binary( it, str_.end() ) ) {
		base = 2;
		skip = 2;
	} else if ( is_octal( it, str_.end() ) ) {
		base = 8;
		M_ASSERT( *it == '0' );
		++ it;
		-- len;
		if ( ( len > 0 ) && ( ( *it == 'o' ) || ( *it == 'O' ) ) ) {
			++ skip;
		}
	} else {
		if ( ! ( ( len > 0 ) && ( *it >= '0' ) && ( *it <= '9' ) ) ) {
			M_THROW( "not a number: " + str_, 0 );
		}
		if ( *it == '0' ) {
			len = 1;
		}
	}
	while ( skip > 0 ) {
		++ it;
		-- len;
		-- skip;
	}
	int const maxDigits( base == 10 ? MAX_VALID_DECIMAL_INTEGER_LENGTH : ( base == 16 ? MAX_VALID_HEXADECIMAL_INTEGER_LENGTH : ( base == 8 ? MAX_VALID_OCTAL_INTEGER_LENGTH : MAX_VALID_BINARY_INTEGER_LENGTH ) ) );
	for ( int i( 0 ); ( i < len ) && ( static_cast<u32_t>( *it ) < unicode::MAX_1_BYTE_CODE_POINT ); ++ i, ++ it, ++ alternate_ ) { /* *TODO* *FIXME* Remove static_cast after UCS in HString is implemented. */
		if ( i > maxDigits ) {
			M_THROW( "too many "_ys.append( base == 10 ? "decimal" : ( base == 16 ) ? "hexadecimal" : ( base == 8 ? "octal" : "binary" ) ).append( " digits: " ).append( str_ ), len );
		}
		*alternate_ = static_cast<char>( *it );
	}
	*alternate_ = 0;
	return ( base );
}

}

namespace hcore {
int long long unsigned stoull_impl( char const*, int*, int );
}

template<>
int long long unsigned lexical_cast( HString const& str_ ) {
	M_PROLOG
	char alternateForm[ MAX_VALID_BINARY_INTEGER_LENGTH ];
	int base( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoull_impl( alternateForm, nullptr, base ) );
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
	int base( preparse_integer( str_, alternateForm ) );
	return ( hcore::stoll_impl( alternateForm, nullptr, base ) );
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
bool lexical_cast( HString const& value_ ) {
	M_PROLOG
	static HString message;
	bool bVal( false );
	if ( ! stricasecmp( value_, "true" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "false" ) ) {
		bVal = false;
	} else if ( ! stricasecmp( value_, "yes" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "no" ) ) {
		bVal = false;
	} else if ( ! stricasecmp( value_, "on" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "off" ) ) {
		bVal = false;
	} else if ( ! stricasecmp( value_, "enable" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "disable" ) ) {
		bVal = false;
	} else if ( ! stricasecmp( value_, "enabled" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "disabled" ) ) {
		bVal = false;
	} else if ( ! stricasecmp( value_, "1" ) ) {
		bVal = true;
	} else if ( ! stricasecmp( value_, "0" ) ) {
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
bool lexical_cast( char const* const& value_ ) {
	M_PROLOG
	return ( lexical_cast<bool>( HString( value_ ) ) );
	M_EPILOG
}

template<>
char lexical_cast( HString const& val ) {
	M_PROLOG
	if ( val.get_length() != 1 ) {
		M_THROW( "bad string size", val.get_length() );
	}
	return ( safe_int::cast<char>( val[ 0 ] ) );
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


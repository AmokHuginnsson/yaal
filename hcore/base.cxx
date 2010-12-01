/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	base.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )

#include "hstring.hxx"
#include "hformat.hxx"
#include "hcore.hxx"

using namespace yaal::hcore;

namespace yaal
{

template<>
int long unsigned lexical_cast( HString const& val )
	{
	int base = 10;
	if ( ( val.get_length() > 2 ) && ( val[ 1 ] == 'x' ) )
		base = 16;
	return ( ::strtoul( val.raw(), NULL, base ) );
	}

template<>
int unsigned lexical_cast( HString const& val )
	{
	return ( static_cast<int unsigned>( lexical_cast<int long unsigned>( val ) ) );
	}

template<>
int short unsigned lexical_cast( HString const& val )
	{
	return ( static_cast<int short unsigned>( lexical_cast<int long unsigned>( val ) ) );
	}

template<>
int long lexical_cast( HString const& val )
	{
	int base( 10 );
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
	if ( ( val.get_length() > 2 ) && ( val[ 1 ] == 'x' ) )
		base = 16;
	return ( ::strtol( val.raw(), NULL, base ) );
	}

template<>
int long lexical_cast( char const* const& val )
	{
	return ( lexical_cast<int long, HString>( val ) );
	}

template<>
int long lexical_cast( char* const& val )
	{
	return ( lexical_cast<int long, HString>( val ) );
	}

template<>
int lexical_cast( HString const& val )
	{
	return ( static_cast<int>( lexical_cast<int long>( val ) ) );
	}

template<>
int short lexical_cast( HString const& val )
	{
	return ( static_cast<int short>( lexical_cast<int long>( val ) ) );
	}

template<>
int lexical_cast( char const* const& val )
	{
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	}

template<>
int lexical_cast( char* const& val )
	{
	return ( static_cast<int>( lexical_cast<int long, HString>( val ) ) );
	}

template<>
double long lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( hcore::strtold( val ) );
	M_EPILOG
	}

template<>
float lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( static_cast<float>( lexical_cast<double long>( val ) ) );
	M_EPILOG
	}

template<>
double long lexical_cast( char const* const& val )
	{
	M_PROLOG
	return ( lexical_cast<double long, HString>( val ) );
	M_EPILOG
	}

template<>
double lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( static_cast<double>( lexical_cast<double long>( val ) ) );
	M_EPILOG
	}

template<>
double lexical_cast( char const* const& val )
	{
	M_PROLOG
	return ( lexical_cast<double, HString>( val ) );
	M_EPILOG
	}

template<>
bool lexical_cast( char const* const& value_ )
	{
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
	else if ( ! ::strcasecmp( value_, "1" ) )
		bVal = true;
	else if ( ! ::strcasecmp( value_, "0" ) )
		bVal = false;
	else
		{
		message = "bad value: ";
		message += value_;
		M_THROW( message, bVal );
		}
	return ( bVal );
	M_EPILOG
	}

template<>
bool lexical_cast( char* const& value_ )
	{
	M_PROLOG
	return ( lexical_cast<bool,char const*>( value_ ) );
	M_EPILOG
	}

template<>
bool lexical_cast( HString const& value_ )
	{
	M_PROLOG
	return ( lexical_cast<bool>( value_.raw() ) );
	M_EPILOG
	}

template<>
char lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( val.is_empty() ? static_cast<char>( 0 ) : val[ 0 ] );
	M_EPILOG
	}

template<>
char const* lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( val.raw() );
	M_EPILOG
	}

template<>
HString lexical_cast( HString const& val )
	{
	M_PROLOG
	return ( val );
	M_EPILOG
	}

template<>
HString lexical_cast( int const& val_ )
	{
	M_PROLOG
	return ( val_ );
	M_EPILOG
	}

template<>
HString lexical_cast( HFormat const& f )
	{
	M_PROLOG
	return ( f.string() );
	M_EPILOG
	}

char const* error_message( int code_ )
	{
	return ( ::strerror( code_ ) );
	}

}


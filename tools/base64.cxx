/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	base64.cxx - this file is integral part of hanalyser.h project.

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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "base64.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

char const n_pcBase64EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
u8_t n_pcBase64DecodeTable[ 256 ];

class HBase64DecodeTableInitializer
	{
public:
	HBase64DecodeTableInitializer( void )
		{
		::memset( n_pcBase64DecodeTable, 0, sizeof ( n_pcBase64DecodeTable ) );
		for ( size_t i = 0; i < sizeof ( n_pcBase64EncodeTable ); ++ i )
			n_pcBase64DecodeTable[ static_cast<int>( n_pcBase64EncodeTable[ i ] ) ] = static_cast<u8_t>( i );
		}
	} base64DecodeTableInit;

void do_buf_3_to_4( HString& out, u32_t const& in, int long pad = 0 )
	{
	char buf[ 5 ] = { 0, 0, 0, 0, 0 };
	buf[ 0 ] = n_pcBase64EncodeTable[ ( in >> 18 ) & 63 ];
	buf[ 1 ] = n_pcBase64EncodeTable[ ( in >> 12 ) & 63 ];
	buf[ 2 ] = n_pcBase64EncodeTable[ ( in >> 6 ) & 63 ];
	buf[ 3 ] = n_pcBase64EncodeTable[ in & 63 ];
	if ( pad )
		::memset( ( ( buf + sizeof ( buf ) ) - pad ) - 1, '=', pad );
	out += buf;
	return;
	}

void do_buf_4_to_3( HString& out, u32_t const& in )
	{
	char buf[ 4 ] = { 0, 0, 0, 0 };
	buf[ 0 ] = static_cast<char>( ( in >> 16 ) & 255 );
	buf[ 1 ] = static_cast<char>( ( in >> 8 ) & 255 );
	buf[ 2 ] = static_cast<char>( in & 255 );
	out += buf;
	return;
	}

yaal::hcore::HString base64::encode( yaal::hcore::HString const& message )
	{
	M_PROLOG
	int shifts[] = { 16, 8, 0 };
	static int const D_PREALLOCATE = 128;
	HString output( D_PREALLOCATE, true );
	u32_t coder = 0;
	char const* ptr = message.raw();
	int long length = message.get_length();
	for ( int long i = 0; i < length; ++ i )
		{
		int shift = shifts[ i % 3 ];
		coder |= ( ptr[ i ] << shift );
		if ( ! shift )
			{
			do_buf_3_to_4( output, coder );
			coder = 0;
			}
		}
	if ( coder )
		do_buf_3_to_4( output, coder, 3 - ( length % 3 ) );
	return ( output );
	M_EPILOG
	}

yaal::hcore::HString base64::decode( yaal::hcore::HString const& message )
	{
	M_PROLOG
	int shifts[] = { 18, 12, 6, 0 };
	static int const D_PREALLOCATE = 128;
	HString output( D_PREALLOCATE, true );
	u32_t coder = 0;
	char const* ptr = message.raw();
	int long length = message.get_length();
	for ( int long i = 0; ( i < length ) && ( ptr[ i ] != '=' ); ++ i )
		{
		char ch = ptr[ i ];
		M_ENSURE( ( ( ch >= 'A' ) && ( ch <= 'Z' ) )
				|| ( ( ch >= 'a' ) && ( ch <= 'z' ) )
				|| ( ( ch >= '0' ) && ( ch <= '9' ) )
				|| ( ch == '-' ) || ( ch == '_' ) );
		int shift = shifts[ i % 4 ];
		coder |= ( n_pcBase64DecodeTable[ static_cast<int>( ptr[ i ] ) ] << shift );
		if ( ! shift )
			{
			do_buf_4_to_3( output, coder );
			coder = 0;
			}
		}
	if ( coder )
		do_buf_4_to_3( output, coder );
	return ( output );
	M_EPILOG
	}

}

}


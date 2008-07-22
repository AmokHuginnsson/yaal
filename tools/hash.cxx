/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hash.cxx - this file is integral part of hanalyser.h project.

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
#include "hash.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace hash
{

yaal::hcore::HString md5( HBitSourceInterface const& bitSource )
	{
	static int const D_BLOCK_SIZE = 512;
	static int const D_MESSAGE_LENGTH_SIZE = 64;
	static int const D_SUPPLEMENT_SIZE = 1;
	static int const D_STATE_SIZE = 4;
	static u32_t const D_S0 = 0x67452301;
	static u32_t const D_S1 = 0xefcdab89;
	static u32_t const D_S2 = 0x98badcfe;
	static u32_t const D_S3 = 0x10325476;
	int long block = 0;
	HBitmap bmp( D_BLOCK_SIZE );
	int last = 0;
	int long total = 0;
	int long state[ D_STATE_SIZE ] = { D_S0, D_S1, D_S2, D_S3 };
	do
		{
		bmp = bitSource.get_nth_block( block, D_BLOCK_SIZE );
		last = bmp.get_size();
		total += last;
		if ( last < ( ( D_BLOCK_SIZE - D_MESSAGE_LENGTH_SIZE ) - D_SUPPLEMENT_SIZE ) )
			{
			}
		else if ( last < D_BLOCK_SIZE )
			{
			}
		else
			{
			}
		}
	while ( last == D_BLOCK_SIZE );
	HString result;
	result.format( "%x%x%x%x", state[ 0 ], state[ 1 ], state[ 2 ], state[ 3 ] );
	return ( result );
	}

}

}

}


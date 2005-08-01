/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	bit.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>
#include <stdio.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/xalloc.h"

namespace stdhapi
{

namespace tools
{

int long unsigned g_pulMaskBitSet [ ] = 
	{
	2147483648u, 1073741824, 536870912, 268435456, 134217728, 67108864, 33554432,
	16777216, 8388608, 4194304, 2097152, 1048576, 524288, 262144, 131072, 65536,
	32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1
	};
                                     
int long unsigned g_pulMaskBitClear [ ] =
	{
	2147483647, 3221225471u, 3758096383u, 4026531839u, 4160749567u, 4227858431u,
	4261412863u, 4278190079u, 4286578687u, 4290772991u, 4292870143u, 4293918719u,
	4294443007u, 4294705151u, 4294836223u, 4294901759u, 4294934527u, 4294950911u,
	4294959103u, 4294963199u, 4294965247u, 4294966271u, 4294966783u, 4294967039u,
	4294967167u, 4294967231u, 4294967263u, 4294967279u, 4294967287u, 4294967291u,
	4294967293u, 4294967294u
	};
                                       
unsigned char g_pucMaskBitSet [ ] = {128, 64, 32, 16, 8, 4, 2, 1};
unsigned char g_pucMaskBitClear [ ] = {127, 191, 223, 239, 247, 251, 253, 254};

int getbit ( void * a_pvAddress, int long unsigned a_ulNumber )
	{
	int short l_hState = 0, l_iOffset = 0;
	char * l_pcAddress = NULL;
	int long unsigned l_ulDword;
	l_pcAddress = static_cast < char * > ( a_pvAddress );
	l_ulDword = a_ulNumber >> 3;
	l_iOffset = static_cast < short > ( a_ulNumber ) & 7;
	l_hState = * ( l_pcAddress + l_ulDword ) & g_pucMaskBitSet [ l_iOffset ];
	if ( l_hState )
		l_hState = 1;
	return ( l_hState );
	}
	
void setbit( void * a_pvAddress, int long unsigned a_ulNumber, int a_iState )
	{
	int l_iOffset;
	unsigned char * l_pcAddress;
	int long unsigned l_ulDword;
	l_pcAddress = static_cast < unsigned char * > ( a_pvAddress );
	l_ulDword = a_ulNumber >> 3;
	l_iOffset = a_ulNumber & 7;
	if ( a_iState )
		* ( l_pcAddress + l_ulDword ) |= g_pucMaskBitSet [ l_iOffset ];
	else
		* ( l_pcAddress + l_ulDword ) &= g_pucMaskBitClear [ l_iOffset ];
	return ;
	}
	
void ror( unsigned char * a_pucTmpBuf, int a_iStart, int a_iLen, int a_iVal )
	{
	int l_i, l_iSize;
	unsigned char * l_pucTmp = 0;
	if ( ( a_iVal < 1 ) || ( a_iVal >= a_iLen ) || ( a_iStart < 0 ) || ( a_iLen < 1 ) )
		{
		perror( "e: fatal rolling a_iValues" );
		exit( 1 );
		}
	l_iSize = ( a_iLen + a_iStart ) / 8;
	if ( ( a_iLen + a_iStart ) % 8 )
		l_iSize++;
	l_pucTmp = xmalloc ( l_iSize, unsigned char );
	for ( l_i = 0; l_i < l_iSize; l_i++ )
		l_pucTmp[ l_i ] = a_pucTmpBuf[ l_i ];
	for ( l_i = 0; l_i < a_iLen; l_i++ )
		setbit( a_pucTmpBuf, a_iStart + l_i, getbit( l_pucTmp, a_iStart + ( l_i + a_iVal ) % a_iLen ) );
	xfree ( l_pucTmp );
	return ;
	}
	
void rol( unsigned char * a_pucTmpBuf, int a_iStart, int a_iLen, int a_iVal )
	{
	int l_i, l_iSize;
	unsigned char * l_pucTmp = 0;
	if ( ( a_iVal < 1 ) || ( a_iVal >= a_iLen ) || ( a_iStart < 0 ) || ( a_iLen < 1 ) )
		{
		perror( "e: fatal rolling values" );
		exit( 1 );
		}
	l_iSize = ( a_iLen + a_iStart ) / 8;
	if ( ( a_iLen + a_iStart ) % 8 )
		l_iSize++;
	l_pucTmp = xmalloc ( l_iSize, unsigned char );
	for ( l_i = 0; l_i < l_iSize; l_i++ )
		l_pucTmp[ l_i ] = a_pucTmpBuf[ l_i ];
	for ( l_i = 0; l_i < a_iLen; l_i++ )
		setbit( a_pucTmpBuf, a_iStart + ( l_i + a_iVal ) % a_iLen, getbit( l_pucTmp, a_iStart + l_i ) );
	xfree ( l_pucTmp );
	return ;
	}
	
}

}


/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashmap.cxx - this file is integral part of `yaal' project.

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

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hhashmap.hxx"

namespace yaal
{

namespace hcore
{

int long const x_tag_g_pulPrimes[ 32 ] =
	{
	1,					3,					7,					13,					31,					61,					
	127,				251,				509,				1021,				2039,				4093,
	8191,				16381,			32749,			65521,			131071,			262139,
	524287,			1048573,		2097143,		4194301,		8388593,		16777213,
	33554393,		67108859,		134217689,	268435399,	536870909,	1073741789,
	2147483647,	0
	}, * const _primes_ = x_tag_g_pulPrimes;

}

}


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrandomizer.c - this file is integral part of `stdhapi' project.

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

#define MULTIPLIER      0x015a4e35L
#define INCREMENT       1

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hrandomizer.h"

namespace stdhapi
{

namespace hcore
{

HRandomizer::HRandomizer( unsigned int a_uiSeed )
	{
	M_PROLOG
	f_lSeed = a_uiSeed;
	return ;
	M_EPILOG
	}
	
void HRandomizer::set( unsigned int a_uiVal )
	{
	M_PROLOG
	f_lSeed = a_uiVal;
	return ;
	M_EPILOG
	}
	
long int HRandomizer::get( void )
	{
	M_PROLOG
	return ( f_lSeed );
	M_EPILOG
	}
	
int HRandomizer::rnd( void )
	{
	M_PROLOG
	f_lSeed = MULTIPLIER * f_lSeed + INCREMENT;
	return ( ( int ) ( f_lSeed >> 16 ) & 0x7fff );
	M_EPILOG
	}

}

}


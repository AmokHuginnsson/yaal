/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.c - this file is integral part of `stdhapi' project.

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

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hinfo.h"

namespace stdhapi
{

namespace hcore
{

#define M_HINFO_PURGE( ) \
	f_eType ( D_NONE ), f_iInt ( 0 ), f_lLongInt ( 0 ), f_dDouble ( 0 ),\
	f_pvPointer ( NULL ), f_oString ( "" ), f_oTime ( )

HInfo::HInfo ( void )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HInfo::HInfo ( const int & a_riInt )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_INT;
	f_iInt = a_riInt;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const long int & a_rlLongInt )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_LONG_INT;
	f_lLongInt = a_rlLongInt;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const double & a_rdDouble )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_DOUBLE;
	f_dDouble = a_rdDouble;
	return;
	M_EPILOG
	}

HInfo::HInfo ( void * a_pvPointer )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_POINTER;
	f_pvPointer = a_pvPointer;
	return;
	M_EPILOG
	}

HInfo::HInfo ( char const * a_pcBuffer )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_HSTRING;
	f_oString = a_pcBuffer;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HString & a_roString )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_HSTRING;
	f_oString = a_roString;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HTime & a_roTime )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	f_eType = D_HTIME;
	f_oTime = a_roTime;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HInfo & a_roInfo, int )
	: M_HINFO_PURGE ( )
	{
	M_PROLOG
	( * this ) = a_roInfo;
	return;
	M_EPILOG
	}

HInfo::~HInfo ( void )
	{
	M_PROLOG
	purge ( );
	return;
	M_EPILOG
	}

void HInfo::purge ( void )
	{
	M_PROLOG
	f_eType = D_NONE;
	f_iInt = 0;
	f_lLongInt = 0;
	f_dDouble = 0;
	f_oString = "";
	f_oTime = HTime ( );
	f_pvPointer = NULL;
	return;
	M_EPILOG
	}

char & HInfo::operator [ ] ( int a_iIndex )
	{
	M_PROLOG
	return ( f_oString [ a_iIndex ] );
	M_EPILOG
	}

HInfo & HInfo::operator = ( const HInfo & a_roInfo )
	{
	M_PROLOG
	if ( this != & a_roInfo )
		{
		f_eType = a_roInfo.f_eType;
		f_iInt = a_roInfo.f_iInt;
		f_lLongInt = a_roInfo.f_lLongInt;
		f_dDouble = a_roInfo.f_dDouble;
		f_oString = a_roInfo.f_oString;
		f_oTime = a_roInfo.f_oTime;
		f_pvPointer = a_roInfo.f_pvPointer;
		}
	return ( * this );
	M_EPILOG
	}

HInfo & HInfo::operator ( ) ( const HInfo & a_roInfo )
	{
	M_PROLOG
	if ( a_roInfo.f_eType & D_INT )
		f_iInt = a_roInfo.f_iInt;
	if ( a_roInfo.f_eType & D_LONG_INT )
		f_lLongInt = a_roInfo.f_lLongInt;
	if ( a_roInfo.f_eType & D_DOUBLE )
		f_dDouble = a_roInfo.f_dDouble;
	if ( a_roInfo.f_eType & D_HSTRING )
		f_oString = a_roInfo.f_oString;
	if ( a_roInfo.f_eType & D_HTIME )
		f_oTime = a_roInfo.f_oTime;
	if ( a_roInfo.f_eType & D_POINTER )
		f_pvPointer = a_roInfo.f_pvPointer;
	f_eType |= a_roInfo.f_eType;
	return ( * this );
	M_EPILOG
	}

template < >
int HInfo::get < int > ( void ) const
	{
	M_PROLOG
	return ( f_iInt );
	M_EPILOG
	}

template < >
long int HInfo::get < long int > ( void ) const
	{
	M_PROLOG
	return ( f_lLongInt );
	M_EPILOG
	}

template < >
double HInfo::get < double > ( void ) const
	{
	M_PROLOG
	return ( f_dDouble );
	M_EPILOG
	}

template < >
char const * HInfo::get < char const * > ( void ) const
	{
	M_PROLOG
	return ( static_cast < char const * > ( f_oString ) );
	M_EPILOG
	}

template < >
const HString & HInfo::get < const HString & > ( void ) const
	{
	M_PROLOG
	return ( f_oString );
	M_EPILOG
	}

template < >
const HTime & HInfo::get < const HTime & > ( void ) const
	{
	M_PROLOG
	return ( f_oTime );
	M_EPILOG
	}

template < >
void * HInfo::get < void * > ( void ) const
	{
	M_PROLOG
	void * l_pvTmp = f_pvPointer;
	return ( l_pvTmp );
	M_EPILOG
	}

HInfo::operator bool ( void ) const
	{
	M_PROLOG
	return ( f_lLongInt || f_dDouble || f_oString );
	M_EPILOG
	}

}

}


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

#include "hinfo.h"

#include "hexception.h"

HInfo::HInfo ( void )
	{
	M_PROLOG
	purge ( );
	return;
	M_EPILOG
	}

HInfo::HInfo ( const int & a_riInt )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_INT;
	f_iInt = a_riInt;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const long int & a_rlLongInt )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_LONG_INT;
	f_lLongInt = a_rlLongInt;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const double & a_rdDouble )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_DOUBLE;
	f_dDouble = a_rdDouble;
	return;
	M_EPILOG
	}

HInfo::HInfo ( void * a_pvPointer )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_POINTER;
	f_pvPointer = a_pvPointer;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const char * a_pcBuffer )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_HSTRING;
	f_oString = a_pcBuffer;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HTime & a_roTime )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_HTIME;
	f_oTime = a_roTime;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HObject * a_poObject )
	{
	M_PROLOG
	purge ( );
	f_iType = D_TYPE_HOBJECT;
	( * this ) = a_poObject;
	return;
	M_EPILOG
	}

HInfo::HInfo ( const HInfo & a_roInfo, int )
	{
	M_PROLOG
	purge ( );
	( * this ) = a_roInfo;
	return;
	M_EPILOG
	}

HInfo::~HInfo ( void )
	{
	M_PROLOG
	if ( f_poObject )
		{
		delete f_poObject;
		f_poObject = NULL;
		}
	return;
	M_EPILOG
	}

void HInfo::purge ( void )
	{
	M_PROLOG
	f_iType = D_TYPE_NONE;
	f_iInt = 0;
	f_lLongInt = 0;
	f_dDouble = 0;
	f_oString = "";
	f_oTime = HTime ( );
	f_poObject = NULL;
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
	f_iType = a_roInfo.f_iType;
	f_iInt = a_roInfo.f_iInt;
	f_lLongInt = a_roInfo.f_lLongInt;
	f_dDouble = a_roInfo.f_dDouble;
	f_oString = a_roInfo.f_oString;
	f_oTime = a_roInfo.f_oTime;
	if ( a_roInfo.f_poObject )
		{
		if ( f_poObject )
			{
			delete f_poObject;
			f_poObject = NULL;
			}
		f_poObject = a_roInfo.f_poObject->clone ( );
		}
	f_pvPointer = a_roInfo.f_pvPointer;
	return ( * this );
	M_EPILOG
	}

int & HInfo::operator = ( const int & a_riInt )
	{
	M_PROLOG
	f_iType |= D_TYPE_INT;
	f_iInt = a_riInt;
	return ( f_iInt );
	M_EPILOG
	}

long int & HInfo::operator = ( const long int & a_rlLongInt )
	{
	M_PROLOG
	f_iType |= D_TYPE_LONG_INT;
	f_lLongInt = a_rlLongInt;
	return ( f_lLongInt );
	M_EPILOG
	}

double & HInfo::operator = ( const double & a_rdDouble )
	{
	M_PROLOG
	f_iType |= D_TYPE_DOUBLE;
	f_dDouble = a_rdDouble;
	return ( f_dDouble );
	M_EPILOG
	}

void * HInfo::operator = ( void * a_pvPointer )
	{
	M_PROLOG
	f_iType |= D_TYPE_POINTER;
	f_pvPointer = a_pvPointer;
	return ( f_pvPointer );
	M_EPILOG
	}

/*
HString & HInfo::operator = ( const HString & a_roString )
	{
	M_PROLOG
	f_iType |= D_TYPE_HSTRING;
	f_oString = a_roString;
	return ( f_oString );
	M_EPILOG
	}
*/
	
HString & HInfo::operator = ( const char * a_pcString )
	{
	M_PROLOG
	f_iType |= D_TYPE_HSTRING;
	f_oString = a_pcString;
	return ( f_oString );
	M_EPILOG
	}

HTime & HInfo::operator = ( const HTime & a_roTime )
	{
	M_PROLOG
	f_iType |= D_TYPE_HTIME;
	f_oTime = a_roTime;
	return ( f_oTime );
	M_EPILOG
	}

HObject * HInfo::operator = ( const HObject * a_poObject )
	{
	M_PROLOG
	if ( ! a_poObject )
		throw new HException ( __WHERE__, "bad casting", g_iErrNo );
	f_iType |= D_TYPE_HOBJECT;
	if ( f_poObject )
		{
		delete f_poObject;
		f_poObject = NULL;
		}
	f_poObject = a_poObject->clone ( );
	return ( f_poObject );
	M_EPILOG
	}

HInfo::operator int & ( void )
	{
	M_PROLOG
	return ( f_iInt );
	M_EPILOG
	}

HInfo::operator long int & ( void )
	{
	M_PROLOG
	return ( f_lLongInt );
	M_EPILOG
	}

HInfo::operator double & ( void )
	{
	M_PROLOG
	return ( f_dDouble );
	M_EPILOG
	}

HInfo::operator HString & ( void )
	{
	M_PROLOG
	return ( f_oString );
	M_EPILOG
	}

HInfo::operator HTime & ( void )
	{
	M_PROLOG
	return ( f_oTime );
	M_EPILOG
	}

HInfo::operator HObject & ( void )
	{
	M_PROLOG
	return ( * f_poObject );
	M_EPILOG
	}

HInfo::operator void * ( void )
	{
	M_PROLOG
	return ( f_pvPointer );
	M_EPILOG
	}

HInfo::operator bool ( void )
	{
	M_PROLOG
	return ( f_lLongInt || f_dDouble || f_oString );
	M_EPILOG
	}

/*
long int operator = ( long int & a_rlLongInt, HInfo & a_roInfo )
	{
	M_PROLOG
	a_rlLongInt = a_roInfo.f_lLongInt;
	return ( a_rlLongInt );
	M_EPILOG
	}

double operator = ( double & a_rdDouble, HInfo & a_roInfo )
	{
	M_PROLOG
	a_roInfo = a_roInfo.f_dDouble;
	return ( a_rdDouble );
	M_EPILOG
	}

HString operator = ( HString & a_roString, HInfo & a_roInfo )
	{
	M_PROLOG
	a_roString = a_roInfo.f_oString;
	return ( a_roString );
	M_EPILOG
	}
*/

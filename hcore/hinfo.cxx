/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.cxx - this file is integral part of `yaal' project.

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
     You can not even demand cost of the carrier(CD for example).
  5. You can not include it to any commercial enterprise(for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hinfo.h"

namespace yaal
{

namespace hcore
{

HInfo::HInfo( void )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HInfo::HInfo( int const& a_riInt )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_INT;
	f_iInt = a_riInt;
	return;
	M_EPILOG
	}

HInfo::HInfo( int long const& a_rlLongInt )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_LONG_INT;
	f_lLongInt = a_rlLongInt;
	return;
	M_EPILOG
	}

HInfo::HInfo ( double const& a_rdDouble )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_DOUBLE;
	f_dDouble = a_rdDouble;
	return;
	M_EPILOG
	}

HInfo::HInfo( void* const a_pvPointer )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_POINTER;
	f_pvPointer = a_pvPointer;
	return;
	M_EPILOG
	}

HInfo::HInfo( char const* const a_pcBuffer )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_HSTRING;
	f_oString = a_pcBuffer;
	return;
	M_EPILOG
	}

HInfo::HInfo( HString const& a_roString )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_HSTRING;
	f_oString = a_roString;
	return;
	M_EPILOG
	}

HInfo::HInfo( HTime const& a_roTime )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	f_eType = D_HTIME;
	f_oTime = a_roTime;
	return;
	M_EPILOG
	}

HInfo::HInfo( HInfo const& a_roInfo )
	: f_eType( D_VOID ), f_iInt( 0 ), f_lLongInt( 0 ), f_dDouble( 0 ), f_pvPointer( NULL ), f_oString( "" ), f_oTime( )
	{
	M_PROLOG
	( * this ) = a_roInfo;
	return;
	M_EPILOG
	}

HInfo::~HInfo( void )
	{
	M_PROLOG
	purge( );
	return;
	M_EPILOG
	}

void HInfo::purge( void )
	{
	M_PROLOG
	f_eType = D_VOID;
	f_iInt = 0;
	f_lLongInt = 0;
	f_dDouble = 0;
	f_oString = "";
	f_oTime = HTime( );
	f_pvPointer = NULL;
	return;
	M_EPILOG
	}

char& HInfo::operator [] ( int const a_iIndex )
	{
	M_PROLOG
	return ( f_oString [ a_iIndex ] );
	M_EPILOG
	}

HInfo& HInfo::operator = ( HInfo const& a_roInfo )
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

HInfo& HInfo::operator () ( HInfo const& a_roInfo )
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

template<>
int const HInfo::get<int>( void ) const
	{
	M_PROLOG
	return ( f_iInt );
	M_EPILOG
	}

template<>
int long const HInfo::get<int long>( void ) const
	{
	M_PROLOG
	return ( f_lLongInt );
	M_EPILOG
	}

template<>
double const HInfo::get<double>( void ) const
	{
	M_PROLOG
	return ( f_dDouble );
	M_EPILOG
	}

template<>
char const* const HInfo::get<char const* const>( void ) const
	{
	M_PROLOG
	return ( static_cast < char const * const > ( f_oString ) );
	M_EPILOG
	}

template<>
HString const& HInfo::get<HString const&>( void ) const
	{
	M_PROLOG
	return ( f_oString );
	M_EPILOG
	}

template<>
HTime const& HInfo::get<HTime const&>( void ) const
	{
	M_PROLOG
	return ( f_oTime );
	M_EPILOG
	}

template<>
void* const HInfo::get<void*>( void ) const
	{
	M_PROLOG
	void* l_pvTmp = f_pvPointer;
	return ( l_pvTmp );
	M_EPILOG
	}

HInfo::operator opcm_t const ( void ) const
	{
	M_PROLOG
	return ( f_lLongInt || f_dDouble || f_oString ? &OPointerConversion::f_iValid : NULL );
	M_EPILOG
	}

}

}


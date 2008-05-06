/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.cxx - this file is integral part of `yaal' project.

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

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hrecordset.h"
#include "db_driver_loader.h"

using namespace yaal::hcore;

#if 0

namespace yaal
{

namespace dbwrapper
{

char n_pcEMode [ ] = "record set is not in appropriate mode for operation";

HRecordSet::HRecordSet ( HDataBase * a_poDataBase )
	: f_pvCoreData ( NULL ), f_oSQL(), f_oVarTmpBuffer(), f_iIdFieldOffset ( - 1 ),
	f_iFieldCount ( 0 ), f_eMode ( D_CLOSED ), f_iCursorPosition ( 0 ), 
	f_iSetQuantity ( 0 ), f_oTable(), f_oColumns ( "*" ), f_oFilter(), 
	f_oSort(), f_oColumnNames(), f_oValues(), f_poDataBase ( a_poDataBase ), 
	m_oFilter(), m_oSort(), m_lId ( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HRecordSet::~HRecordSet ( void )
	{
	M_PROLOG
	if ( f_pvCoreData )
		dbwrapper::db_unquery ( f_pvCoreData );
	f_pvCoreData = NULL;
	f_poDataBase = NULL;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( void )
	{
	M_PROLOG 
	int l_iCtr = 0;
	if ( f_eMode == D_CLOSED )
		M_THROW ( n_pcEMode, f_eMode );
	for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
		sync ( l_iCtr, f_oValues [ l_iCtr ] );
	if ( f_iFieldCount > 0 )
		sync ( f_iIdFieldOffset, m_lId );
	return;
	M_EPILOG
	}

void HRecordSet::build_sql ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_oVarTmpBuffer = "";
	switch ( f_eMode )
		{
		case ( D_CLOSED ):
			{
			f_oSQL.format ( "SELECT %s FROM %s", static_cast < char const * > ( f_oColumns ),
					static_cast < char const * > ( f_oTable ) );
			if ( f_oFilter.is_empty() )
				f_oVarTmpBuffer = m_oFilter;
			else if ( m_oFilter.is_empty() )
				f_oVarTmpBuffer = f_oFilter;
			else
				f_oVarTmpBuffer.format ( "%s AND ( %s )", static_cast < char const * > ( f_oFilter ),
					static_cast < char const * > ( m_oFilter ) );
			if ( ! f_oVarTmpBuffer.is_empty() )
				f_oSQL += ( " WHERE " + f_oVarTmpBuffer );
			f_oVarTmpBuffer = "";
			if ( f_oSort.is_empty() )
				f_oVarTmpBuffer = m_oSort;
			else if ( m_oSort.is_empty() )
				f_oVarTmpBuffer = f_oSort;
			else
				f_oVarTmpBuffer.format ( "%s, %s", static_cast < char const * > ( m_oSort ),
					static_cast < char const * > ( f_oSort ) );
			if ( ! f_oVarTmpBuffer.is_empty() )
				f_oSQL += ( " ORDER BY " + f_oVarTmpBuffer );
			f_oSQL += ';';
			}
		break;
		case ( D_EDITING ):
			{
			f_oSQL = "UPDATE " + f_oTable + " SET ";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )
					continue;
				if ( ! f_oVarTmpBuffer.is_empty() )
					f_oVarTmpBuffer += ", ";
				f_oVarTmpBuffer += f_oColumnNames [ l_iCtr ] + " = '" + f_oValues [ l_iCtr ] + '\'';
				}
			f_oVarTmpBuffer += " WHERE id = ";
			f_oSQL += f_oVarTmpBuffer;
			f_oSQL += m_lId;
			f_oSQL += ';';
			}
		break;
		case ( D_ADDING ):
			{
			f_oSQL = "INSERT INTO " + f_oTable + " ( ";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )
					continue;
				if ( ! f_oVarTmpBuffer.is_empty() )
					f_oVarTmpBuffer += ", ";
				f_oVarTmpBuffer += f_oColumnNames [ l_iCtr ];
				}
			f_oVarTmpBuffer += " ) VALUES ( ";
			f_oSQL += f_oVarTmpBuffer;
			f_oVarTmpBuffer = "";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )
					continue;
				if ( ! f_oVarTmpBuffer.is_empty() )
					f_oVarTmpBuffer += ", ";
				f_oVarTmpBuffer += '\'' + f_oValues [ l_iCtr ] + '\'';
				}
			f_oVarTmpBuffer += " );";
			f_oSQL += f_oVarTmpBuffer;
			}
		break;
		default :
			M_THROW ( n_pcEMode, f_eMode );
		}
	return;
	M_EPILOG
	}

int long HRecordSet::open ( char const * a_pcQuery )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( f_eMode != D_CLOSED )
		M_THROW ( n_pcEMode, f_eMode );
	if ( a_pcQuery )
		f_oSQL = a_pcQuery;
	else
		build_sql();
	free();
	f_iCursorPosition = 0;
	f_iSetQuantity = f_poDataBase->query ( f_oSQL );
	if ( f_iSetQuantity < 0 )
		log ( LOG_TYPE::D_ERROR ) << "SQL error (query): " << f_poDataBase->get_error() << endl;
	f_pvCoreData = f_poDataBase->get_result();
	f_iFieldCount = dbwrapper::rs_fields_count ( f_pvCoreData );
	if ( f_iFieldCount < 0 )
		log ( LOG_TYPE::D_ERROR ) << "SQL error (fiels count): " << f_poDataBase->get_error() << endl;
	f_eMode = D_NORMAL;
	f_oColumnNames.clear();
	f_oValues.clear();
	for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
		{
		f_oColumnNames.add_tail() = dbwrapper::rs_column_name ( f_pvCoreData,
				l_iCtr );
		if ( f_oColumnNames.tail() == "id" )
			f_iIdFieldOffset = l_iCtr;
		f_oValues.add_tail();
		}
	if ( f_iSetQuantity > 0 )
		sync();
	return ( f_iSetQuantity );
	M_EPILOG
	}

void HRecordSet::close ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	free();
	f_eMode = D_CLOSED;
	M_EPILOG
	}

void HRecordSet::cancel ( void )
	{
	M_PROLOG
	if ( ( f_eMode != D_ADDING ) && ( f_eMode != D_EDITING ) )
		M_THROW ( n_pcEMode, f_eMode );
	f_eMode = D_NORMAL;
	return;
	M_EPILOG
	}

void HRecordSet::free ( void )
	{
	M_PROLOG
	if ( f_pvCoreData )
		dbwrapper::db_unquery ( f_pvCoreData );
	f_pvCoreData = NULL;
	M_EPILOG
	}

int long HRecordSet::requery ( char const * a_pcQuery )
	{
	M_PROLOG
	close();
	if ( a_pcQuery )
		f_oSQL = a_pcQuery;
	else
		build_sql();
	return ( open ( f_oSQL ) );
	M_EPILOG
	}

bool HRecordSet::is_open ( void ) const
	{
	return ( f_eMode != D_CLOSED );
	}

bool HRecordSet::is_eof ( void ) const
	{
	M_PROLOG
	return ( f_iCursorPosition >= f_iSetQuantity );
	M_EPILOG
	}

bool HRecordSet::is_bof ( void ) const
	{
	M_PROLOG
	return ( f_iCursorPosition < 0 );
	M_EPILOG
	}

void HRecordSet::move_next ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_iCursorPosition ++;
	if ( f_iCursorPosition > f_iSetQuantity )
		M_THROW ( "end of set reached", f_iCursorPosition );
	if ( f_iCursorPosition < f_iSetQuantity )
		sync();
	return;
	M_EPILOG
	}

void HRecordSet::move_previous ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_iCursorPosition --;
	if ( f_iCursorPosition < -1 )
		M_THROW ( "beginning of set reached",
				f_iCursorPosition );
	if ( f_iCursorPosition >= 0 )
		sync();
	return;
	M_EPILOG
	}

void HRecordSet::move_first ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_iCursorPosition = 0;
	sync();
	return;
	M_EPILOG
	}

void HRecordSet::move_last ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_iCursorPosition = f_iSetQuantity - 1;
	sync();
	return;
	M_EPILOG
	}

HString HRecordSet::get ( int a_iField )
	{
	M_PROLOG
	HString l_oTmp = dbwrapper::rs_get ( f_pvCoreData, f_iCursorPosition,
			a_iField );
	return ( l_oTmp );
	M_EPILOG
	}

void HRecordSet::add_new ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_eMode = D_ADDING;
	return;
	M_EPILOG
	}

void HRecordSet::edit ( void )
	{
	M_PROLOG
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_eMode = D_EDITING;
	return;
	M_EPILOG
	}

int long HRecordSet::update ( void )
	{
	M_PROLOG
	int long l_iRetVal = 0;
	if ( ( f_eMode != D_ADDING ) && ( f_eMode != D_EDITING ) )
		M_THROW ( n_pcEMode, f_eMode );
	sync();
	build_sql();
	f_poDataBase->query ( f_oSQL );
	if ( f_eMode == D_ADDING )
		l_iRetVal = f_poDataBase->insert_id();
	f_poDataBase->free_result();
	f_eMode = D_NORMAL;
	requery();
	return ( l_iRetVal );
	M_EPILOG
	}

void HRecordSet::remove ( void )
	{
	M_PROLOG
	int l_iCursorPosition = f_iCursorPosition;
	if ( f_eMode != D_NORMAL )
		M_THROW ( n_pcEMode, f_eMode );
	f_oSQL.format ( "DELETE FROM %s WHERE id = %ld;",
			static_cast < char const * > ( f_oTable ), m_lId );
	f_poDataBase->query ( f_oSQL );
	f_poDataBase->free_result();
	requery();
	if ( f_iSetQuantity > 0 )
		{
		if ( l_iCursorPosition >= f_iSetQuantity )
			f_iCursorPosition = f_iSetQuantity - 1;
		else
			f_iCursorPosition = l_iCursorPosition;
		}
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, char & a_rcChar )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty() )
			a_rcChar = l_oTmp [ 0 ];
		}
	else
		f_oValues [ a_iField ] = a_rcChar;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, short & a_rhShort )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty() )
			a_rhShort = static_cast < short > ( atoi ( l_oTmp ) );
		}
	else
		f_oValues [ a_iField ] = a_rhShort;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, int & a_riInt )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty() )
			a_riInt = atoi ( l_oTmp );
		}
	else
		f_oValues [ a_iField ] = a_riInt;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, int long & a_rlLongInt )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty() )
			a_rlLongInt = atol ( l_oTmp );
		}
	else
		f_oValues [ a_iField ] = a_rlLongInt;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, double & a_rdDouble )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty() )
			a_rdDouble = atof ( l_oTmp );
		}
	else
		f_oValues [ a_iField ] = a_rdDouble;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HString & a_roString )
	{
	M_PROLOG
	if ( f_eMode == D_NORMAL )
		a_roString = get ( a_iField );
	else
		f_oValues [ a_iField ] = a_roString;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HTime & a_roTime )
	{
	M_PROLOG
	HTime l_oTime;
	if ( f_eMode == D_NORMAL )
		a_roTime = static_cast < char const * > ( get ( a_iField ) );
	else
		{
		l_oTime = a_roTime;
		l_oTime.format ( "%F %T" );
		f_oValues [ a_iField ] = static_cast < char const * > ( l_oTime );
		}
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HInfo & a_roInfo )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_eMode == D_NORMAL )
		{
		l_oTmp = get ( a_iField );
		a_roInfo ( atoi ( l_oTmp ) );
		a_roInfo ( atol ( l_oTmp ) );
		a_roInfo ( atof ( l_oTmp ) );
		a_roInfo ( static_cast < char const * > ( l_oTmp ) );
		}
	else
		f_oValues [ a_iField ] = a_roInfo.get < HString const & >();
	return;
	M_EPILOG
	}

}

}

#endif


/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.c - this file is integral part of `stdhapi' project.

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

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hrecordset.h"
#include "db_driver_loader.h"

char g_pcEMode [ ] = "record set is not in appropriate mode for operation";
#define E_MODE g_pcEMode

HRecordSet::HRecordSet ( HDataBase * a_poDataBase )
	{
	M_PROLOG
	f_iFieldCount = 0;
	f_iMode = D_MODE_CLOSED;
	f_iSetQuantity = 0;
	f_iCursorPosition = 0;
	f_pvCoreData = NULL;
	f_poDataBase = a_poDataBase;
	f_oColumns = "*";
	f_iIdFieldOffset = -1;
	m_lId = 0;
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

HObject * HRecordSet::clone ( void ) const
	{
	M_PROLOG
	HRecordSet * l_poObject = NULL;
	l_poObject = new HRecordSet ( * this );
	if ( ! l_poObject )
		throw new HException ( __WHERE__, "new returned", ( int ) l_poObject );
	return ( l_poObject );
	M_EPILOG
	}

void HRecordSet::sync ( void )
	{
	M_PROLOG 
	int l_iCtr = 0;
	if ( f_iMode == D_MODE_CLOSED )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
		sync ( l_iCtr, f_oValues [ l_iCtr ] );
	sync ( f_iIdFieldOffset, m_lId );
	return;
	M_EPILOG
	}

void HRecordSet::build_sql ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	f_oBuffer = "";
	switch ( f_iMode )
		{
		case ( D_MODE_CLOSED ):
			{
			f_oSQL.format ( "SELECT %s FROM %s", ( const char * ) f_oColumns,
					( const char * ) f_oTable );
			if ( f_oFilter.is_empty ( ) )f_oBuffer = m_oFilter;
			else if ( m_oFilter.is_empty ( ) )f_oBuffer = f_oFilter;
			else f_oBuffer.format ( "%s AND ( %s )", ( const char * ) f_oFilter,
					( const char * ) m_oFilter );
			if ( ! f_oBuffer.is_empty ( ) )
				f_oSQL += ( " WHERE " + f_oBuffer );
			f_oBuffer = "";
			if ( f_oSort.is_empty ( ) )f_oBuffer = m_oSort;
			else if ( m_oSort.is_empty ( ) )f_oBuffer = f_oSort;
			else f_oBuffer.format ( "%s, %s", ( const char * ) m_oSort,
					( const char * ) f_oSort );
			if ( ! f_oBuffer.is_empty ( ) )
				f_oSQL += ( " ORDER BY " + f_oBuffer );
			f_oSQL += ';';
			break;
			}
		case ( D_MODE_EDITING ):
			{
			f_oSQL = "UPDATE " + f_oTable + " SET ";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )continue;
				if ( ! f_oBuffer.is_empty ( ) )f_oBuffer += ", ";
				f_oBuffer += f_oColumnNames [ l_iCtr ] + " = '" + f_oValues [ l_iCtr ] + '\'';
				}
			f_oBuffer += " WHERE id = ";
			f_oSQL += f_oBuffer;
			f_oSQL += m_lId;
			f_oSQL += ';';
			break;
			}
		case ( D_MODE_ADDING ):
			{
			f_oSQL = "INSERT INTO " + f_oTable + " ( ";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )continue;
				if ( ! f_oBuffer.is_empty ( ) )f_oBuffer += ", ";
				f_oBuffer += f_oColumnNames [ l_iCtr ];
				}
			f_oBuffer += " ) VALUES ( ";
			f_oSQL += f_oBuffer;
			f_oBuffer = "";
			for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
				{
				if ( l_iCtr == f_iIdFieldOffset )continue;
				if ( ! f_oBuffer.is_empty ( ) )f_oBuffer += ", ";
				f_oBuffer += '\'' + f_oValues [ l_iCtr ] + '\'';
				}
			f_oBuffer += " );";
			f_oSQL += f_oBuffer;
			break;
			}
		default :
			{
			throw new HException ( __WHERE__, E_MODE, f_iMode );
			break;
			}
		}
	return;
	M_EPILOG
	}

long int HRecordSet::open ( const char * a_pcQuery )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( f_iMode != D_MODE_CLOSED )throw new HException ( __WHERE__, E_MODE, f_iMode );
	if ( a_pcQuery )f_oSQL = a_pcQuery;
	else build_sql ( );
	free ( );
	f_iCursorPosition = 0;
	f_iSetQuantity = f_poDataBase->query ( f_oSQL );
	f_pvCoreData = f_poDataBase->get_result ( );
	f_iFieldCount = dbwrapper::rs_fields_count ( f_pvCoreData );
	f_iMode = D_MODE_NORMAL;
	f_oColumnNames.flush ( );
	f_oValues.flush ( );
	for ( l_iCtr = 0; l_iCtr < f_iFieldCount; l_iCtr ++ )
		{
		f_oColumnNames.add_tail ( ) = dbwrapper::rs_column_name ( f_pvCoreData,
				l_iCtr );
		if ( f_oColumnNames.tail ( ) == "id" )f_iIdFieldOffset = l_iCtr;
		f_oValues.add_tail ( );
		}
	if ( f_iSetQuantity > 0 )sync ( );
	return ( f_iSetQuantity );
	M_EPILOG
	}

void HRecordSet::close ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	free ( );
	f_iMode = D_MODE_CLOSED;
	M_EPILOG
	}

void HRecordSet::cancel ( void )
	{
	M_PROLOG
	if ( ( f_iMode != D_MODE_ADDING ) && ( f_iMode != D_MODE_EDITING ) )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iMode = D_MODE_NORMAL;
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

long int HRecordSet::requery ( const char * a_pcQuery )
	{
	M_PROLOG
	close ( );
	if ( a_pcQuery )f_oSQL = a_pcQuery;
	else build_sql ( );
	return ( open ( f_oSQL ) );
	M_EPILOG
	}

bool HRecordSet::is_open ( void )
	{
	return ( f_iMode != D_MODE_CLOSED );
	}

bool HRecordSet::is_eof ( void )
	{
	M_PROLOG
	return ( f_iCursorPosition >= f_iSetQuantity );
	M_EPILOG
	}

bool HRecordSet::is_bof ( void )
	{
	M_PROLOG
	return ( f_iCursorPosition < 0 );
	M_EPILOG
	}

void HRecordSet::move_next ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iCursorPosition ++;
	if ( f_iCursorPosition > f_iSetQuantity )
		throw new HException ( __WHERE__, "end of set reached", f_iCursorPosition );
	if ( f_iCursorPosition < f_iSetQuantity )sync ( );
	return;
	M_EPILOG
	}

void HRecordSet::move_previous ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iCursorPosition --;
	if ( f_iCursorPosition < -1 )
		throw new HException ( __WHERE__, "beginning of set reached",
				f_iCursorPosition );
	if ( f_iCursorPosition >= 0 )sync ( );
	return;
	M_EPILOG
	}

void HRecordSet::move_first ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iCursorPosition = 0;
	sync ( );
	return;
	M_EPILOG
	}

void HRecordSet::move_last ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iCursorPosition = f_iSetQuantity - 1;
	sync ( );
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
	if ( f_iMode != D_MODE_NORMAL )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iMode = D_MODE_ADDING;
	return;
	M_EPILOG
	}

void HRecordSet::edit ( void )
	{
	M_PROLOG
	if ( f_iMode != D_MODE_NORMAL )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_iMode = D_MODE_EDITING;
	return;
	M_EPILOG
	}

long int HRecordSet::update ( void )
	{
	M_PROLOG
	long int l_iRetVal = 0;
	if ( ( f_iMode != D_MODE_ADDING ) && ( f_iMode != D_MODE_EDITING ) )
		throw new HException ( __WHERE__, E_MODE, f_iMode );
	sync ( );
	build_sql ( );
	f_poDataBase->query ( f_oSQL );
	if ( f_iMode == D_MODE_ADDING )
		l_iRetVal = f_poDataBase->insert_id ( );
	f_poDataBase->free_result ( );
	f_iMode = D_MODE_NORMAL;
	requery ( );
	return ( l_iRetVal );
	M_EPILOG
	}

void HRecordSet::remove ( void )
	{
	M_PROLOG
	int l_iCursorPosition = f_iCursorPosition;
	if ( f_iMode != D_MODE_NORMAL )throw new HException ( __WHERE__, E_MODE, f_iMode );
	f_oSQL.format ( "DELETE FROM %s WHERE id = %ld;",
			( const char * ) f_oTable, m_lId );
	f_poDataBase->query ( f_oSQL );
	f_poDataBase->free_result ( );
	requery ( );
	if ( f_iSetQuantity > 0 )
		{
		if ( l_iCursorPosition >= f_iSetQuantity )
			f_iCursorPosition = f_iSetQuantity - 1;
		else f_iCursorPosition = l_iCursorPosition;
		}
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, char & a_rcChar )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty ( ) )a_rcChar = l_oTmp [ 0 ];
		}
	else f_oValues [ a_iField ] = a_rcChar;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, short & a_rhShort )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty ( ) )a_rhShort = atoi ( l_oTmp );
		}
	else f_oValues [ a_iField ] = a_rhShort;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, int & a_riInt )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty ( ) )a_riInt = atoi ( l_oTmp );
		}
	else f_oValues [ a_iField ] = a_riInt;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, long int & a_rlLongInt )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty ( ) )a_rlLongInt = atol ( l_oTmp );
		}
	else f_oValues [ a_iField ] = a_rlLongInt;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, double & a_rdDouble )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		if ( ! l_oTmp.is_empty ( ) )a_rdDouble = atof ( l_oTmp );
		}
	else f_oValues [ a_iField ] = a_rdDouble;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HString & a_roString )
	{
	M_PROLOG
	if ( f_iMode == D_MODE_NORMAL )a_roString = get ( a_iField );
	else f_oValues [ a_iField ] = a_roString;
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HTime & a_roTime )
	{
	M_PROLOG
	HTime l_oTime;
	if ( f_iMode == D_MODE_NORMAL )a_roTime = get ( a_iField );
	else
		{
		l_oTime = a_roTime;
		l_oTime.format ( "%F %T" );
		f_oValues [ a_iField ] = ( HString & ) l_oTime;
		}
	return;
	M_EPILOG
	}

void HRecordSet::sync ( int a_iField, HInfo & a_roInfo )
	{
	M_PROLOG
	HString l_oTmp;
	if ( f_iMode == D_MODE_NORMAL )
		{
		l_oTmp = get ( a_iField );
		a_roInfo = atoi ( l_oTmp );
		a_roInfo = atol ( l_oTmp );
		a_roInfo = atof ( l_oTmp );
		a_roInfo = l_oTmp;
		}
	else f_oValues [ a_iField ] = ( HString & ) a_roInfo;
	return;
	M_EPILOG
	}


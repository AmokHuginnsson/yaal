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
#include "hdatabase.h"
#include "db_driver_loader.h"

using namespace yaal::hcore;

namespace yaal
{

namespace dbwrapper
{

char n_pcEMode [ ] = "record set is not in appropriate mode for operation";

HRecordSet::HRecordSet( database_ptr_t a_oDataBase, void* a_pvReuslt )
	: f_oDataBase( a_oDataBase ), f_pvResult( a_pvReuslt )
	{
	M_PROLOG
	if ( get_size() < 0 )
		log( LOG_TYPE::D_ERROR ) << "SQL error (query): " << f_oDataBase->get_error() << endl;
	if ( get_field_count() < 0 )
		log( LOG_TYPE::D_ERROR ) << "SQL error (fiels count): " << f_oDataBase->get_error() << endl;
	return;
	M_EPILOG
	}

HRecordSet::~HRecordSet ( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

void HRecordSet::clear( void )
	{
	M_PROLOG
	if ( f_pvResult )
		dbwrapper::db_unquery( f_pvResult );
	f_pvResult = NULL;
	M_EPILOG
	}

bool HRecordSet::is_empty( void ) const
	{
	return ( ! get_size() );
	}

int HRecordSet::get_field_count( void ) const
	{
	return ( dbwrapper::rs_fields_count( f_pvResult ) );
	}

int HRecordSet::get_size( void ) const
	{
	return ( dbwrapper::dbrs_records_count( &*f_oDataBase, f_pvResult ) );
	}

char const* HRecordSet::get_column_name( int a_iColumn ) const
	{
	return ( dbwrapper::rs_column_name( f_pvResult, a_iColumn ) );
	}

int long HRecordSet::get_insert_id( void ) const
	{
	M_PROLOG
	if ( ! f_pvResult )
		M_THROW( "no result", errno );
	return ( dbwrapper::dbrs_id( &*f_oDataBase, f_pvResult ) );
	M_EPILOG
	}

char const* HRecordSet::get_error( void ) const
	{
	M_PROLOG
	return ( f_oDataBase->get_error() );
	M_EPILOG
	}

int HRecordSet::get_errno( void ) const
	{
	M_PROLOG
	return ( f_oDataBase->get_errno() );
	M_EPILOG
	}

HRecordSet::iterator HRecordSet::begin( void )
	{
	return ( iterator( this, 0 ) );
	}

HRecordSet::iterator HRecordSet::end( void )
	{
	return ( iterator( this, get_size() ) );
	}

HRecordSet::iterator HRecordSet::rbegin( void )
	{
	return ( iterator( this, get_size() - 1 ) );
	}

HRecordSet::iterator HRecordSet::rend( void )
	{
	return ( iterator( this, -1 ) );
	}

HSQLDescriptor::HSQLDescriptor( void )
	: f_eMode( MODE::D_SELECT ), f_oVarTmpBuffer(), f_oSQL(), f_oTable(),
	f_oColumns ( "*" ), f_oFilter(), f_oSort(), f_oFields(), f_iFieldCount( 0 ),
	f_iSetSize( 0 ), f_oValues(), f_oDataBase()
	{
	return;
	}

HSQLDescriptor::HSQLDescriptor( database_ptr_t a_oDataBase )
	: f_eMode( MODE::D_SELECT ), f_oVarTmpBuffer(), f_oSQL(), f_oTable(),
	f_oColumns ( "*" ), f_oFilter(), f_oSort(), f_oFields(), f_iFieldCount( 0 ),
	f_iSetSize( 0 ), f_oValues(), f_oDataBase( a_oDataBase )
	{
	return;
	}

HSQLDescriptor::~HSQLDescriptor( void )
	{
	}

HString const& HSQLDescriptor::build_sql( MODE::mode_t const& a_eMode )
	{
	M_PROLOG
	f_oVarTmpBuffer = "";
	switch ( a_eMode )
		{
		case ( MODE::D_SELECT ):
			{
			f_oSQL.format ( "SELECT %s FROM %s", static_cast < char const * > ( f_oColumns ),
					static_cast < char const * > ( f_oTable ) );
			if ( ! f_oFilter.is_empty() )
				f_oSQL += ( " WHERE " + f_oFilter );
			if ( ! f_oSort.is_empty() )
				f_oSQL += ( " ORDER BY " + f_oSort );
			f_oSQL += ';';
			}
		break;
		case ( MODE::D_UPDATE ):
			{
			f_oSQL = "UPDATE " + f_oTable + " SET ";
			M_ENSURE( f_oFields.get_size() == f_oValues.get_size() );
			int const size = f_oValues.get_size();
			for ( int i = 0; i < size; ++ i )
				{
				if ( i > 0 )
					f_oSQL += ", ";
				f_oSQL += f_oFields[ i ];
				f_oSQL += " = '";
				f_oSQL += f_oValues[ i ];
				f_oSQL += "'";
				}
			if ( ! f_oFilter.is_empty() )
				{
				f_oSQL += " WHERE ";
				f_oSQL += f_oFilter;
				}
			f_oSQL += ';';
			}
		break;
		case ( MODE::D_INSERT ):
			{
			f_oSQL = "INSERT INTO " + f_oTable + " ( ";
			M_ENSURE( f_oFields.get_size() == f_oValues.get_size() );
			int const size = f_oFields.get_size();
			for ( int i = 0; i < size; ++ i )
				{
				if ( i > 0 )
					f_oSQL += ", ";
				f_oSQL += f_oFields[ i ];
				}
			f_oSQL += " ) VALUES ( ";
			for ( int i = 0; i < size; ++ i )
				{
				if ( i > 0 )
					f_oSQL += ", ";
				f_oSQL += "'";
				f_oSQL += f_oFields[ i ];
				f_oSQL += "'";
				}
			f_oSQL += " );";
			}
		break;
		case ( MODE::D_DELETE ):
			{
			f_oSQL = "DELETE FROM ";
			f_oSQL += f_oTable;
			if ( ! f_oFilter.is_empty() )
				{
				f_oSQL += " WHERE ";
				f_oSQL += f_oFilter;
				}
			f_oSQL += ";";
			}
		break;
		default :
			M_THROW( n_pcEMode, a_eMode );
		}
	f_eMode = a_eMode;
	return ( f_oSQL );
	M_EPILOG
	}

void HSQLDescriptor::sync( int a_iField, HString& value )
	{
	M_PROLOG
	if ( f_eMode == MODE::D_SELECT )
		value = f_oValues[ a_iField ];
	else
		f_oValues[ a_iField ] = value;
	M_EPILOG
	}

void HSQLDescriptor::sync( int a_iField, int long& value )
	{
	M_PROLOG
	if ( f_eMode == MODE::D_SELECT )
		value = lexical_cast<int long>( f_oValues[ a_iField ] );
	else
		f_oValues[ a_iField ] = value;
	M_EPILOG
	}

void HSQLDescriptor::set_table( char const* const a_pcTable )
	{
	f_oTable = a_pcTable;
	}

void HSQLDescriptor::set_columns( char const* const a_pcColumns )
	{
	f_oColumns = a_pcColumns;
	}

void HSQLDescriptor::set_filter( char const* const a_pcFilter )
	{
	f_oFilter = a_pcFilter;
	}

void HSQLDescriptor::set_sort( char const* const a_pcSort )
	{
	f_oSort = a_pcSort;
	}

HString HSQLDescriptor::get_table( void ) const
	{
	return ( f_oTable );
	}

HString HSQLDescriptor::get_columns( void ) const
	{
	return ( f_oColumns );
	}

HString HSQLDescriptor::get_filter( void ) const
	{
	return ( f_oFilter );
	}

HString HSQLDescriptor::get_sort( void ) const
	{
	return ( f_oSort );
	}

HSQLDescriptor::MODE::mode_t HSQLDescriptor::get_mode( void ) const
	{
	return ( f_eMode );
	}

int HSQLDescriptor::get_size( void ) const
	{
	return ( f_iSetSize );
	}

HRecordSet::HIterator::HIterator( HRecordSet* a_poOwner, int const& a_iPosition )
	: f_poOwner( a_poOwner ), f_iCursorPosition( a_iPosition )
	{
	}

HRecordSet::HIterator::HIterator( HIterator const& it )
	: f_poOwner( it.f_poOwner ), f_iCursorPosition( it.f_iCursorPosition )
	{
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator = ( HIterator const& it )
	{
	if ( &it != this )
		{
		f_poOwner = it.f_poOwner;
		f_iCursorPosition = it.f_iCursorPosition;
		}
	return ( *this );
	}

bool HRecordSet::HIterator::operator == ( HIterator const& it ) const
	{
	M_ASSERT( it.f_poOwner == f_poOwner );
	return ( it.f_iCursorPosition == f_iCursorPosition );
	}

bool HRecordSet::HIterator::operator != ( HIterator const& it ) const
	{
	M_ASSERT( it.f_poOwner == f_poOwner );
	return ( it.f_iCursorPosition != f_iCursorPosition );
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator ++ ( void )
	{
	M_ASSERT( f_poOwner );
	++ f_iCursorPosition;
	return ( *this );
	}

HRecordSet::HIterator HRecordSet::HIterator::operator ++ ( int )
	{
	M_ASSERT( f_poOwner );
	HIterator it( *this );
	++ f_iCursorPosition;
	return ( it );
	}

yaal::hcore::HString HRecordSet::HIterator::operator[] ( int const& a_iField ) const
	{
	M_ASSERT( f_poOwner );
	return ( dbwrapper::rs_get( f_poOwner->f_pvResult, f_iCursorPosition,
			a_iField ) );
	}

HRecordSet::ptr_t HSQLDescriptor::execute( void )
	{
	M_PROLOG
	HRecordSet::ptr_t rs = f_oDataBase->query( f_oSQL );
	f_iFieldCount = rs->get_field_count();
	f_oFields = fields_t( f_iFieldCount );
	f_oValues = values_t( f_iFieldCount );
	for ( int l_iCtr = 0; l_iCtr < f_iFieldCount; ++ l_iCtr )
		f_oFields[ l_iCtr ] = rs->get_column_name( l_iCtr ); 
	f_iSetSize = rs->get_size();
	return ( rs );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( MODE::mode_t const& a_eMode )
	{
	M_PROLOG
	build_sql( a_eMode );
	return ( execute() );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( char const* const a_pcQuery )
	{
	M_PROLOG
	f_oSQL = a_pcQuery;
	return ( execute() );
	M_EPILOG
	}

void HSQLDescriptor::sync( HRecordSet::iterator const& it )
	{
	M_PROLOG
	for ( int l_iCtr = 0; l_iCtr < f_iFieldCount; ++ l_iCtr )
		f_oValues[ l_iCtr ] = it[ l_iCtr ];
	M_EPILOG
	}

HString& HSQLDescriptor::operator[]( int a_iColumn )
	{
	M_PROLOG
	M_ASSERT( ( a_iColumn >= 0 ) && ( a_iColumn <= f_iFieldCount ) );
	return ( f_oValues[ a_iColumn ] );
	M_EPILOG
	}

}

}


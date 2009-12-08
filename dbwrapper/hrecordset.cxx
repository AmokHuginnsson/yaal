/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrecordset.cxx - this file is integral part of `yaal' project.

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

#include <stdlib.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hlog.hxx"
#include "hrecordset.hxx"
#include "hdatabase.hxx"
#include "db_driver_loader.hxx"

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
		log( LOG_TYPE::ERROR ) << "SQL error (query): " << f_oDataBase->get_error() << endl;
	if ( get_field_count() < 0 )
		log( LOG_TYPE::ERROR ) << "SQL error (fiels count): " << f_oDataBase->get_error() << endl;
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
		(f_oDataBase->connector().db_unquery)( f_pvResult );
	f_pvResult = NULL;
	M_EPILOG
	}

bool HRecordSet::is_empty( void ) const
	{
	return ( ! get_size() );
	}

int HRecordSet::get_field_count( void ) const
	{
	return ( (f_oDataBase->connector().rs_fields_count)( f_pvResult ) );
	}

int long HRecordSet::get_size( void ) const
	{
	return ( (f_oDataBase->connector().dbrs_records_count)( &*f_oDataBase, f_pvResult ) );
	}

char const* HRecordSet::get_column_name( int a_iColumn ) const
	{
	return ( (f_oDataBase->connector().rs_column_name)( f_pvResult, a_iColumn ) );
	}

int long HRecordSet::get_insert_id( void ) const
	{
	M_PROLOG
	if ( ! f_pvResult )
		M_THROW( "no result", errno );
	return ( (f_oDataBase->connector().dbrs_id)( &*f_oDataBase, f_pvResult ) );
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
	: f_eMode( MODE::SELECT ), f_oVarTmpBuffer(), f_oSQL(), f_oTable(),
	f_oColumns ( "*" ), f_oFilter(), f_oSort(), f_oFields(), f_iFieldCount( 0 ),
	f_lSetSize( 0 ), f_oValues(), f_oDataBase(), f_oMutated()
	{
	return;
	}

HSQLDescriptor::HSQLDescriptor( database_ptr_t a_oDataBase )
	: f_eMode( MODE::SELECT ), f_oVarTmpBuffer(), f_oSQL(), f_oTable(),
	f_oColumns ( "*" ), f_oFilter(), f_oSort(), f_oFields(), f_iFieldCount( 0 ),
	f_lSetSize( 0 ), f_oValues(), f_oDataBase( a_oDataBase ), f_oMutated()
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
		case ( MODE::SELECT ):
			{
			f_oSQL.format( "SELECT %s FROM %s", f_oColumns.raw(),
					f_oTable.raw() );
			if ( ! f_oFilter.is_empty() )
				f_oSQL += ( " WHERE " + f_oFilter );
			if ( ! f_oSort.is_empty() )
				f_oSQL += ( " ORDER BY " + f_oSort );
			f_oSQL += ';';
			}
		break;
		case ( MODE::UPDATE ):
			{
			f_oSQL = "UPDATE " + f_oTable + " SET ";
			M_ENSURE( f_oFields.get_size() == f_oValues.get_size() );
			int long const size = f_oValues.get_size();
			bool hasField = false;
			for ( int i = 0; i < size; ++ i )
				{
				if ( f_oMutated[ i ] )
					{
					if ( hasField )
						f_oSQL += ", ";
					hasField = true;
					f_oSQL += f_oFields[ i ];
					f_oSQL += " = '";
					f_oSQL += f_oValues[ i ];
					f_oSQL += "'";
					}
				}
			if ( ! f_oFilter.is_empty() )
				{
				f_oSQL += " WHERE ";
				f_oSQL += f_oFilter;
				}
			f_oSQL += ';';
			}
		break;
		case ( MODE::INSERT ):
			{
			f_oSQL = "INSERT INTO " + f_oTable + " ( ";
			M_ENSURE( f_oFields.get_size() == f_oValues.get_size() );
			int long const size = f_oFields.get_size();
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
		case ( MODE::DELETE ):
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
	if ( f_eMode == MODE::SELECT )
		value = f_oValues[ a_iField ];
	else
		f_oValues[ a_iField ] = value;
	M_EPILOG
	}

void HSQLDescriptor::sync( int a_iField, int long& value )
	{
	M_PROLOG
	if ( f_eMode == MODE::SELECT )
		value = lexical_cast<int long>( f_oValues[ a_iField ] );
	else
		f_oValues[ a_iField ] = value;
	M_EPILOG
	}

void HSQLDescriptor::set_table( yaal::hcore::HString const& a_oTable )
	{
	M_PROLOG
	f_oTable = a_oTable;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_columns( yaal::hcore::HString const& a_oColumns )
	{
	M_PROLOG
	f_oColumns = a_oColumns;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_filter( yaal::hcore::HString const& a_oFilter )
	{
	M_PROLOG
	f_oFilter = a_oFilter;
	return;
	M_EPILOG
	}

void HSQLDescriptor::set_sort( yaal::hcore::HString const& a_oSort )
	{
	M_PROLOG
	f_oSort = a_oSort;
	return;
	M_EPILOG
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

int long HSQLDescriptor::get_size( void ) const
	{
	return ( f_lSetSize );
	}

HRecordSet::HIterator::HIterator( HRecordSet* a_poOwner, int long const& a_iPosition )
	: f_poOwner( a_poOwner ), f_lCursorPosition( a_iPosition )
	{
	}

HRecordSet::HIterator::HIterator( HIterator const& it )
	: f_poOwner( it.f_poOwner ), f_lCursorPosition( it.f_lCursorPosition )
	{
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator = ( HIterator const& it )
	{
	if ( &it != this )
		{
		f_poOwner = it.f_poOwner;
		f_lCursorPosition = it.f_lCursorPosition;
		}
	return ( *this );
	}

bool HRecordSet::HIterator::operator == ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it.f_poOwner == f_poOwner );
	return ( it.f_lCursorPosition == f_lCursorPosition );
	M_EPILOG
	}

bool HRecordSet::HIterator::operator != ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it.f_poOwner == f_poOwner );
	return ( it.f_lCursorPosition != f_lCursorPosition );
	M_EPILOG
	}

HRecordSet::HIterator& HRecordSet::HIterator::operator ++ ( void )
	{
	M_PROLOG
	M_ASSERT( f_poOwner );
	++ f_lCursorPosition;
	return ( *this );
	M_EPILOG
	}

HRecordSet::HIterator HRecordSet::HIterator::operator ++ ( int )
	{
	M_PROLOG
	M_ASSERT( f_poOwner );
	HIterator it( *this );
	++ f_lCursorPosition;
	return ( it );
	M_EPILOG
	}

yaal::hcore::HString HRecordSet::HIterator::operator[] ( int const& a_iField ) const
	{
	M_PROLOG
	M_ASSERT( f_poOwner );
	return ( (f_poOwner->f_oDataBase->connector().rs_get)( f_poOwner->f_pvResult,
				f_lCursorPosition, a_iField ) );
	M_EPILOG
	}

HRecordSet::ptr_t HSQLDescriptor::execute( void )
	{
	M_PROLOG
	HRecordSet::ptr_t rs = f_oDataBase->query( f_oSQL );
	f_iFieldCount = rs->get_field_count();
	if ( f_oFields.get_size() != f_iFieldCount )
		{
		f_oFields = fields_t( f_iFieldCount );
		f_oValues = values_t( f_iFieldCount );
		f_oMutated = mutated_t( f_iFieldCount );
		}
	for ( int l_iCtr = 0; l_iCtr < f_iFieldCount; ++ l_iCtr )
		{
		f_oFields[ l_iCtr ] = rs->get_column_name( l_iCtr ); 
		f_oMutated[ l_iCtr ] = false;
		}
	f_lSetSize = rs->get_size();
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
	f_oMutated[ a_iColumn ] = true;
	return ( f_oValues[ a_iColumn ] );
	M_EPILOG
	}

}

}


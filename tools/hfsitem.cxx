/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfsitem.cxx - this file is integral part of `yaal' project.

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

#include <dirent.h>
#include <sys/stat.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hfsitem.hxx"
#include "hcore/xalloc.hxx"

#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HFSItem::HFSItem( HString const& a_oRoot )
	: f_iNameLen( static_cast<int>( a_oRoot.get_length() ) ), f_oPath( a_oRoot )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HFSItem::~HFSItem( void )
	{
	}

bool HFSItem::is_directory() const
	{
	M_PROLOG
	struct stat l_sStat;
	do_stat( &l_sStat );
	return ( S_ISDIR( l_sStat.st_mode ) );
	M_EPILOG
	}

bool HFSItem::is_file() const
	{
	M_PROLOG
	struct stat l_sStat;
	do_stat( &l_sStat );
	return ( S_ISREG( l_sStat.st_mode ) );
	M_EPILOG
	}

bool HFSItem::is_executable() const
	{
	M_PROLOG
	struct stat l_sStat;
	do_stat( &l_sStat );
	return ( ( l_sStat.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) ) ? true : false );
	M_EPILOG
	}

void HFSItem::do_stat( void* buf ) const
	{
	M_PROLOG
	M_ENSURE( ( ::stat( f_oPath.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) || ( ::lstat( f_oPath.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) );
	return;
	M_EPILOG
	}

HString HFSItem::get_path( void ) const
	{
	return ( f_oPath );
	}

HString HFSItem::get_name( void ) const
	{
	M_PROLOG
	return ( f_oPath.right( f_iNameLen ) );
	M_EPILOG
	}

void HFSItem::set_path( HString const& a_oPath, int a_iNameLen )
	{
	M_PROLOG
	f_iNameLen = a_iNameLen;
	f_oPath = a_oPath;
	return;
	M_EPILOG
	}

bool HFSItem::operator ! ( void ) const
	{
	struct stat dummy;
	return ( ! ( ( ::stat( f_oPath.raw(),
						&dummy ) == 0 ) || ( ::lstat( f_oPath.raw(),
							&dummy ) == 0 ) ) );
	}

void HFSItem::swap( HFSItem& o )
	{
	if ( &o != this )
		{
		using yaal::swap;
		swap( f_iNameLen, o.f_iNameLen );
		swap( f_oPath, o.f_oPath );
		}
	return;
	}

HFSItem::HIterator HFSItem::begin( void )
	{
	M_PROLOG
	M_ENSURE( is_directory() );
	return ( HIterator( f_oPath ) );
	M_EPILOG
	}

HFSItem::HIterator HFSItem::end( void )
	{
	M_PROLOG
	return ( HIterator( "" ) );
	M_EPILOG
	}

HFSItem::HIterator::HIterator( HString const& a_oPath ) : f_oPath( a_oPath ), f_pvDir( NULL ), f_oDirEnt(), f_oItem( "" )
	{
	M_PROLOG
	if ( ! f_oPath.is_empty() )
		{
		f_pvDir = ::opendir( f_oPath.raw() );
		f_oDirEnt = HChunk::ptr_t( new HChunk( chunk_size<dirent>( 1 ) ) );
		operator ++();
		}
	return;
	M_EPILOG
	}

HFSItem::HIterator::HIterator( HIterator const& a_oIt ) : f_oPath( a_oIt.f_oPath ), f_pvDir( NULL ), f_oDirEnt(), f_oItem( "" )
	{
	M_PROLOG
	if ( a_oIt.f_pvDir )
		{
		f_pvDir = ::opendir( f_oPath.raw() );
		seekdir( static_cast<DIR*>( f_pvDir ), telldir( static_cast<DIR*>( a_oIt.f_pvDir ) ) );
		if ( !! a_oIt.f_oDirEnt )
			{
			f_oDirEnt = HChunk::ptr_t( new HChunk( chunk_size<dirent>( 1 ) ) );
			::memcpy( f_oDirEnt->get<void>(), a_oIt.f_oDirEnt->get<void>(), sizeof ( dirent ) );
			}
		f_oItem.set_path( a_oIt.f_oItem.f_oPath, a_oIt.f_oItem.f_iNameLen );
		}
	return;
	M_EPILOG
	}

HFSItem::HIterator::~HIterator( void )
	{
	M_PROLOG
	cleanup();
	return;
	M_EPILOG
	}

HFSItem::HIterator& HFSItem::HIterator::operator = ( HFSItem::HIterator const& a_oFSItemIterator )
	{
	M_PROLOG
	if ( &a_oFSItemIterator != this )
		{
		HIterator tmp( a_oFSItemIterator );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HFSItem::HIterator::swap( HFSItem::HIterator& o )
	{
	M_PROLOG
	if ( &o != this )
		{
		using yaal::swap;
		swap( f_oPath, o.f_oPath );
		swap( f_pvDir, o.f_pvDir );
		swap( f_oDirEnt, o.f_oDirEnt );
		swap( f_oItem, o.f_oItem );
		}
	M_EPILOG
	}

bool HFSItem::HIterator::operator == ( HIterator const& it ) const
	{
	M_PROLOG
	bool same = false;
	if ( f_oPath == it.f_oPath )
		{
		if ( ! ( f_pvDir || it.f_pvDir ) )
			same = true;
		else if ( f_pvDir && it.f_pvDir )
			same = ( f_oDirEnt->get<dirent>()->d_ino == it.f_oDirEnt->get<dirent>()->d_ino );
		}
	return ( same );
	M_EPILOG
	}

bool HFSItem::HIterator::operator != ( HIterator const& it ) const
	{
	return ( ! operator == ( it ) );
	}

HFSItem::HIterator& HFSItem::HIterator::operator++( void )
	{
	M_ASSERT( f_pvDir );
	dirent* result = NULL;
	readdir_r( static_cast<DIR*>( f_pvDir ), f_oDirEnt->get<dirent>(), &result );
	if ( ! result )
		cleanup();
	return ( *this );
	}

void HFSItem::HIterator::update( void )
	{
	M_PROLOG
	dirent* ent = f_oDirEnt->get<dirent>();
	f_oItem.set_path( f_oPath + "/" + ent->d_name, static_cast<int>( ::strlen( ent->d_name ) ) );
	return;
	M_EPILOG
	}

void HFSItem::HIterator::cleanup( void )
	{
	if ( f_pvDir )
		closedir( static_cast<DIR*>( f_pvDir ) );
	f_pvDir = NULL;
	f_oPath = "";
	return;
	}

HFSItem const& HFSItem::HIterator::operator* ( void )
	{
	M_PROLOG
	M_ASSERT( f_pvDir );
	update();
	return ( f_oItem );
	M_EPILOG
	}

HFSItem const* HFSItem::HIterator::operator->( void )
	{
	M_PROLOG
	M_ASSERT( f_pvDir );
	update();
	return ( &f_oItem );
	M_EPILOG
	}

}

}


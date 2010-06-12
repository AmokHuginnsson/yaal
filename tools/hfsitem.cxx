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

#include <cstring>
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

HFSItem::HFSItem( HString const& root_ )
	: _nameLen( static_cast<int>( root_.get_length() ) ), _path( root_ )
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
	struct stat stat;
	do_stat( &stat );
	return ( S_ISDIR( stat.st_mode ) );
	M_EPILOG
	}

bool HFSItem::is_file() const
	{
	M_PROLOG
	struct stat stat;
	do_stat( &stat );
	return ( S_ISREG( stat.st_mode ) );
	M_EPILOG
	}

bool HFSItem::is_executable() const
	{
	M_PROLOG
	struct stat stat;
	do_stat( &stat );
	return ( ( stat.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) ) ? true : false );
	M_EPILOG
	}

void HFSItem::do_stat( void* buf ) const
	{
	M_PROLOG
	M_ENSURE( ( ::stat( _path.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) || ( ::lstat( _path.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) );
	return;
	M_EPILOG
	}

HString HFSItem::get_path( void ) const
	{
	return ( _path );
	}

HString HFSItem::get_name( void ) const
	{
	M_PROLOG
	return ( _path.right( _nameLen ) );
	M_EPILOG
	}

void HFSItem::set_path( HString const& path_, int nameLen_ )
	{
	M_PROLOG
	_nameLen = nameLen_;
	_path = path_;
	return;
	M_EPILOG
	}

bool HFSItem::operator ! ( void ) const
	{
	struct stat dummy;
	return ( ! ( ( ::stat( _path.raw(),
						&dummy ) == 0 ) || ( ::lstat( _path.raw(),
							&dummy ) == 0 ) ) );
	}

void HFSItem::swap( HFSItem& o )
	{
	if ( &o != this )
		{
		using yaal::swap;
		swap( _nameLen, o._nameLen );
		swap( _path, o._path );
		}
	return;
	}

HFSItem::HIterator HFSItem::begin( void )
	{
	M_PROLOG
	M_ENSURE( is_directory() );
	return ( HIterator( _path ) );
	M_EPILOG
	}

HFSItem::HIterator HFSItem::end( void )
	{
	M_PROLOG
	return ( HIterator( "" ) );
	M_EPILOG
	}

HFSItem::HIterator::HIterator( HString const& path_ ) : _path( path_ ), _dir( NULL ), _dirEnt(), _item( "" )
	{
	M_PROLOG
	if ( ! _path.is_empty() )
		{
		_dir = ::opendir( _path.raw() );
		_dirEnt = HChunk::ptr_t( new HChunk( chunk_size<dirent>( 1 ) ) );
		operator ++();
		}
	return;
	M_EPILOG
	}

HFSItem::HIterator::HIterator( HIterator const& it_ ) : _path( it_._path ), _dir( NULL ), _dirEnt(), _item( "" )
	{
	M_PROLOG
	if ( it_._dir )
		{
		_dir = ::opendir( _path.raw() );
		seekdir( static_cast<DIR*>( _dir ), telldir( static_cast<DIR*>( it_._dir ) ) );
		if ( !! it_._dirEnt )
			{
			_dirEnt = HChunk::ptr_t( new HChunk( chunk_size<dirent>( 1 ) ) );
			::memcpy( _dirEnt->get<void>(), it_._dirEnt->get<void>(), sizeof ( dirent ) );
			}
		_item.set_path( it_._item._path, it_._item._nameLen );
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

HFSItem::HIterator& HFSItem::HIterator::operator = ( HFSItem::HIterator const& fSItemIterator_ )
	{
	M_PROLOG
	if ( &fSItemIterator_ != this )
		{
		HIterator tmp( fSItemIterator_ );
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
		swap( _path, o._path );
		swap( _dir, o._dir );
		swap( _dirEnt, o._dirEnt );
		swap( _item, o._item );
		}
	M_EPILOG
	}

bool HFSItem::HIterator::operator == ( HIterator const& it ) const
	{
	M_PROLOG
	bool same = false;
	if ( _path == it._path )
		{
		if ( ! ( _dir || it._dir ) )
			same = true;
		else if ( _dir && it._dir )
			same = ( _dirEnt->get<dirent>()->d_ino == it._dirEnt->get<dirent>()->d_ino );
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
	M_ASSERT( _dir );
	dirent* result = NULL;
	readdir_r( static_cast<DIR*>( _dir ), _dirEnt->get<dirent>(), &result );
	if ( ! result )
		cleanup();
	return ( *this );
	}

void HFSItem::HIterator::update( void )
	{
	M_PROLOG
	dirent* ent = _dirEnt->get<dirent>();
	_item.set_path( _path + "/" + ent->d_name, static_cast<int>( ::strlen( ent->d_name ) ) );
	return;
	M_EPILOG
	}

void HFSItem::HIterator::cleanup( void )
	{
	if ( _dir )
		closedir( static_cast<DIR*>( _dir ) );
	_dir = NULL;
	_path = "";
	return;
	}

HFSItem const& HFSItem::HIterator::operator* ( void )
	{
	M_PROLOG
	M_ASSERT( _dir );
	update();
	return ( _item );
	M_EPILOG
	}

HFSItem const* HFSItem::HIterator::operator->( void )
	{
	M_PROLOG
	M_ASSERT( _dir );
	update();
	return ( &_item );
	M_EPILOG
	}

}

}


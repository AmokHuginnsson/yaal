/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfsitem.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hfsitem.hxx"
#include "hcore/memory.hxx"
#include "hcore/system.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HFSItem::HFSItem( HString const& root_ )
	: _nameLen( static_cast<int>( root_.get_length() ) ), _path( root_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HFSItem::~HFSItem( void ) {
}

bool HFSItem::is_directory( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( S_ISDIR( s.st_mode ) );
	M_EPILOG
}

bool HFSItem::is_file( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( S_ISREG( s.st_mode ) );
	M_EPILOG
}

bool HFSItem::is_executable( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( ( s.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) ) ? true : false );
	M_EPILOG
}

int long HFSItem::get_size( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( static_cast<int long>( s.st_size ) );
	M_EPILOG
}

int long HFSItem::size( void ) const {
	M_PROLOG
	return ( get_size() );
	M_EPILOG
}

int HFSItem::get_permissions( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( s.st_mode );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::modified( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( s.st_mtime );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::created( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( s.st_ctime );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::accessed( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( s.st_atime );
	M_EPILOG
}

void HFSItem::do_stat( void* buf ) const {
	M_PROLOG
	M_ENSURE( ( ::stat( _path.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) || ( ::lstat( _path.raw(),
					static_cast<struct stat*>( buf ) ) == 0 ) );
	return;
	M_EPILOG
}

HString HFSItem::get_path( void ) const {
	return ( _path );
}

HString HFSItem::get_name( void ) const {
	M_PROLOG
	return ( _path.right( _nameLen ) );
	M_EPILOG
}

yaal::hcore::HString HFSItem::get_user( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( system::get_user_name( s.st_uid ) );
	M_EPILOG
}

yaal::hcore::HString HFSItem::get_group( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( system::get_group_name( s.st_gid ) );
	M_EPILOG
}

void HFSItem::set_path( HString const& path_, int nameLen_ ) {
	M_PROLOG
	_nameLen = nameLen_;
	_path = path_;
	return;
	M_EPILOG
}

bool HFSItem::operator ! ( void ) const {
	struct stat dummy;
	return ( ! ( ( ::stat( _path.raw(),
						&dummy ) == 0 ) || ( ::lstat( _path.raw(),
							&dummy ) == 0 ) ) );
}

void HFSItem::swap( HFSItem& o ) {
	if ( &o != this ) {
		using yaal::swap;
		swap( _nameLen, o._nameLen );
		swap( _path, o._path );
	}
	return;
}

HFSItem::HIterator HFSItem::begin( void ) {
	M_PROLOG
	M_ENSURE( is_directory() );
	return ( HIterator( _path ) );
	M_EPILOG
}

HFSItem::HIterator HFSItem::end( void ) {
	M_PROLOG
	return ( HIterator( "" ) );
	M_EPILOG
}

static int const DIRENT_SIZE_BASE = sizeof ( dirent ) - sizeof ( static_cast<dirent*>( NULL )->d_name ) + 1;

inline int dirent_size( char const* const path_ ) {
	int const nameMax( static_cast<int>( pathconf( path_, _PC_NAME_MAX ) ) );
#ifndef NAME_MAX
#define NAME_MAX _XOPEN_NAME_MAX
#endif /* #ifndef NAME_MAX */
	int const DIRENT_SIZE( DIRENT_SIZE_BASE + ( nameMax > 0 ? nameMax : NAME_MAX ) );
	return ( DIRENT_SIZE );
}

HFSItem::HIterator::HIterator( HString const& path_ ) : _path( path_ ), _dir( NULL ), _dirEnt(), _item( "" ) {
	M_PROLOG
	if ( ! _path.is_empty() ) {
		_dir = ::opendir( _path.raw() );
		_dirEnt = make_pointer<HChunk>( dirent_size( _path.raw() ) );
		operator ++();
	}
	return;
	M_EPILOG
}

HFSItem::HIterator::HIterator( HIterator const& it_ ) : _path( it_._path ), _dir( NULL ), _dirEnt(), _item( "" ) {
	M_PROLOG
	if ( it_._dir ) {
		_dir = ::opendir( _path.raw() );
		seekdir( static_cast<DIR*>( _dir ), telldir( static_cast<DIR*>( it_._dir ) ) );
		if ( !! it_._dirEnt ) {
			int const DIRENT_SIZE( dirent_size( _path.raw() ) );
			_dirEnt = make_pointer<HChunk>( DIRENT_SIZE );
			::memcpy( _dirEnt->get<void>(), it_._dirEnt->get<void>(), DIRENT_SIZE );
		}
		_item.set_path( it_._item._path, it_._item._nameLen );
	}
	return;
	M_EPILOG
}

HFSItem::HIterator::~HIterator( void ) {
	M_PROLOG
	cleanup();
	return;
	M_DESTRUCTOR_EPILOG
}

HFSItem::HIterator& HFSItem::HIterator::operator = ( HFSItem::HIterator const& fSItemIterator_ ) {
	M_PROLOG
	if ( &fSItemIterator_ != this ) {
		HIterator tmp( fSItemIterator_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

void HFSItem::HIterator::swap( HFSItem::HIterator& o ) {
	M_PROLOG
	if ( &o != this ) {
		using yaal::swap;
		swap( _path, o._path );
		swap( _dir, o._dir );
		swap( _dirEnt, o._dirEnt );
		swap( _item, o._item );
	}
	M_EPILOG
}

bool HFSItem::HIterator::operator == ( HIterator const& it ) const {
	M_PROLOG
	bool same = false;
	if ( _path == it._path ) {
		if ( ! ( _dir || it._dir ) )
			same = true;
		else if ( _dir && it._dir )
			same = ( _dirEnt->get<dirent>()->d_ino == it._dirEnt->get<dirent>()->d_ino );
	}
	return ( same );
	M_EPILOG
}

bool HFSItem::HIterator::operator != ( HIterator const& it ) const {
	return ( ! operator == ( it ) );
}

HFSItem::HIterator& HFSItem::HIterator::operator++( void ) {
	M_ASSERT( _dir );
	dirent* result( NULL );
	do {
		readdir_r( static_cast<DIR*>( _dir ), _dirEnt->get<dirent>(), &result );
	} while ( result && ( ! ( ::memcmp( result->d_name, ".\0", 2 ) && ::memcmp( result->d_name, "..\0", 3 ) ) ) );
	if ( ! result )
		cleanup();
	return ( *this );
}

void HFSItem::HIterator::update( void ) {
	M_PROLOG
	dirent* ent( _dirEnt->get<dirent>() );
	_item.set_path( _path + "/" + ent->d_name, static_cast<int>( ::strlen( ent->d_name ) ) );
	return;
	M_EPILOG
}

void HFSItem::HIterator::cleanup( void ) {
	if ( _dir )
		closedir( static_cast<DIR*>( _dir ) );
	_dir = NULL;
	_path = "";
	return;
}

HFSItem const& HFSItem::HIterator::operator* ( void ) {
	M_PROLOG
	M_ASSERT( _dir );
	update();
	return ( _item );
	M_EPILOG
}

HFSItem const* HFSItem::HIterator::operator->( void ) {
	M_PROLOG
	M_ASSERT( _dir );
	update();
	return ( &_item );
	M_EPILOG
}

HString HFSItem::error_message( int errno_ ) const {
	return ( _path + ": " + strerror( errno_ ) );
}

}

}


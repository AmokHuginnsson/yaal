/* Read yaal/LICENSE.md file for copyright and licensing information. */

#define YAAL_USES_STAT 1

#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hfsitem.hxx"
#include "hcore/memory.hxx"
#include "hcore/hcore.hxx"
#include "hcore/system.hxx"

#undef YAAL_USES_STAT

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {
bool const initHFSItemException __attribute__((used)) = HFSItemException::decode_errno();
}

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

u64_t HFSItem::id( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( static_cast<u64_t>( s.st_ino ) );
	M_EPILOG
}

int HFSItem::get_permissions( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	static u32_t const FILE_MODE_BITS( 07777 );
	return ( static_cast<int>( s.st_mode & FILE_MODE_BITS ) );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::modified( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( unix_epoch_to_yaal_epoch( s.st_mtime ) );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::status_changed( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( unix_epoch_to_yaal_epoch( s.st_ctime ) );
	M_EPILOG
}

yaal::hcore::HTime HFSItem::accessed( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( unix_epoch_to_yaal_epoch( s.st_atime ) );
	M_EPILOG
}

void HFSItem::do_stat( void* buf ) const {
	M_PROLOG
	bytes_t bytes( string_to_bytes( _path ) );
	M_ENSURE( ( ::stat( bytes.data(),
					static_cast<struct stat*>( buf ) ) == 0 ) || ( ::lstat( bytes.data(),
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

yaal::hcore::system::user_id_t HFSItem::get_uid( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( static_cast<yaal::hcore::system::user_id_t>( s.st_uid ) );
	M_EPILOG
}

yaal::hcore::system::user_id_t HFSItem::get_gid( void ) const {
	M_PROLOG
	struct stat s;
	do_stat( &s );
	return ( static_cast<yaal::hcore::system::user_id_t>( s.st_gid ) );
	M_EPILOG
}

yaal::hcore::HString HFSItem::get_user( void ) const {
	M_PROLOG
	return ( system::get_user_name( get_uid() ) );
	M_EPILOG
}

yaal::hcore::HString HFSItem::get_group( void ) const {
	M_PROLOG
	return ( system::get_group_name( get_gid() ) );
	M_EPILOG
}

void HFSItem::set_path( HString const& path_, int nameLen_ ) {
	M_PROLOG
	_nameLen = nameLen_;
	_path = path_;
	return;
	M_EPILOG
}

HFSItem::operator bool ( void ) const {
	struct stat dummy;
	bytes_t bytes( string_to_bytes( _path ) );
	return (
		( ::stat( bytes.data(), &dummy ) == 0 )
		|| ( ::lstat( bytes.data(), &dummy ) == 0 )
	);
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
	if ( ! is_directory() ) {
		throw HFSItemException( "is_directory failed: "_ys.append( _path ), ENOTDIR );
	}
	return ( HIterator( _path ) );
	M_EPILOG
}

HFSItem::HIterator HFSItem::end( void ) {
	M_PROLOG
	return ( HIterator( "" ) );
	M_EPILOG
}

HFSItem::HIterator::HIterator( HString const& path_ )
	: _path( path_ )
	, _dir( nullptr )
	, _pos( -1 )
	, _inode( 0 )
	, _name()
	, _item( "" ) {
	M_PROLOG
	if ( ! _path.is_empty() ) {
		bytes_t bytes( string_to_bytes( _path ) );
		_dir = ::opendir( bytes.data() );
		if ( _dir == nullptr ) {
			throw HFSItemException( "opendir failed: "_ys.append( _path ) );
		}
		operator ++();
	}
	return;
	M_EPILOG
}

HFSItem::HIterator::HIterator( HIterator const& it_ )
	: _path( it_._path )
	, _dir( nullptr )
	, _pos( -1 )
	, _inode( it_._inode )
	, _name( it_._name )
	, _item( "" ) {
	M_PROLOG
	if ( it_._dir ) {
		bytes_t bytes( string_to_bytes( _path ) );
		_dir = ::opendir( bytes.data() );
		if ( _dir == nullptr ) {
			throw HFSItemException( "opendir failed: "_ys.append( _path ) );
		}
		while ( _pos < it_._pos ) {
			operator ++();
			if ( _inode == it_._inode ) {
				break;
			}
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
		swap( _pos, o._pos );
		swap( _inode, o._inode );
		swap( _name, o._name );
		swap( _item, o._item );
	}
	M_EPILOG
}

bool HFSItem::HIterator::operator == ( HIterator const& it ) const {
	M_PROLOG
	bool same = false;
	if ( _path == it._path ) {
		if ( ! ( _dir || it._dir ) ) {
			same = true;
		} else if ( _dir && it._dir ) {
			same = ( _inode == it._inode );
		}
	}
	return ( same );
	M_EPILOG
}

bool HFSItem::HIterator::operator != ( HIterator const& it ) const {
	return ( ! operator == ( it ) );
}

HFSItem::HIterator& HFSItem::HIterator::operator++( void ) {
	M_ASSERT( _dir );
	dirent* result( nullptr );
	do {
		result = readdir( static_cast<DIR*>( _dir ) );
	} while ( result && ( ! ( ::memcmp( result->d_name, ".\0", 2 ) && ::memcmp( result->d_name, "..\0", 3 ) ) ) );
	++ _pos;
	if ( result ) {
		_inode = result->d_ino;
		bytes_to_string( _name, result->d_name );
	} else {
		cleanup();
	}
	return ( *this );
}

void HFSItem::HIterator::update( void ) {
	M_PROLOG
	_item.set_path( _path + "/" + _name, static_cast<int>( _name.get_length() ) );
	return;
	M_EPILOG
}

void HFSItem::HIterator::cleanup( void ) {
	if ( _dir )
		closedir( static_cast<DIR*>( _dir ) );
	_dir = nullptr;
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


/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	filesystem.cxx - this file is integral part of `yaal' project.

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
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "filesystem.hxx"
#include "stringalgo.hxx"
#include "hfsitem.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace filesystem {

typedef FileSystem this_type;

namespace {

void do_stat( struct stat* s, path_t const& path_ ) {
	M_PROLOG
	::memset( s, 0, sizeof ( *s ) );
	M_ENSURE( ( ::stat( path_.raw(), s ) == 0 ) || ( ::lstat( path_.raw(), s ) == 0 ) );
	return;
	M_EPILOG
}

}

path_t normalize_path( path_t const& path_ ) {
	M_PROLOG
	HString path;
	bool sep( false );
	for ( char c : path_ ) {
		if ( sep && ( c == '/' ) ) {
			continue;
		}
		sep = ( c == '/' );
		path.push_back( c );
	}
	return ( path );
	M_EPILOG
}

bool exists( path_t const& path_ ) {
	int err( ::access( path_.c_str(), F_OK ) );
	if ( ( err != 0 ) && ( errno != ENOENT ) ) {
		throw HFileSystemException( to_string( "Failed to determine `" ).append( path_ ).append( "'s onthological status." ) );
	}
	return ( err == 0 );
}

bool is_directory( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_ );
	return ( S_ISDIR( s.st_mode ) );
	M_EPILOG
}

bool is_symbolic_link( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_ );
	return ( S_ISLNK( s.st_mode ) );
	M_EPILOG
}

void remove( path_t const& path_ ) {
	int err( ::unlink( path_.c_str() ) );
	if ( ( err != 0 ) && ( errno != ENOENT ) ) {
		throw HFileSystemException( to_string( "Failed to remove: `" ).append( path_ ).append( "'" ) );
	}
	return;
}

void rename( path_t const& old_, path_t const& new_ ) {
	int err( ::rename( old_.c_str(), new_.c_str() ) );
	if ( err != 0 ) {
		throw HFileSystemException( to_string( "Failed to rename: `" ).append( old_ ).append( "' to `" ).append( new_ ).append( "'" ) );
	}
	return;
}

void create_directory( path_t const& path_, u32_t mode_, DIRECTORY_MODIFICATION directoryModification_ ) {
	M_PROLOG
	HString path( normalize_path( path_ ) );
	M_ENSURE( ! path.is_empty() );
	if ( directoryModification_ == DIRECTORY_MODIFICATION::EXACT ) {
		int err( ::mkdir( path_.c_str(), mode_ ) );
		if ( ( err != 0 ) && ( ( errno != EEXIST ) || ! is_directory( path_ ) ) ) {
			throw HFileSystemException( to_string( "Failed to create directory `" ).append( path_ ).append( "'" ) );
		}
	} else {
		typedef HArray<yaal::hcore::HString> file_names_t;
		file_names_t fileNames( string::split<file_names_t>( path_, "/", HTokenizer::SKIP_EMPTY ) );
		path.assign( path[0] == '/' ? "/" : "" );
		for ( yaal::hcore::HString const fn : fileNames ) {
			path.append( fn );
			create_directory( path, mode_, DIRECTORY_MODIFICATION::EXACT );
			path.append( "/" );
		}
	}
	return;
	M_EPILOG
}

void remove_directory( path_t const& path_, DIRECTORY_MODIFICATION directoryModification_ ) {
	if ( directoryModification_ == DIRECTORY_MODIFICATION::EXACT ) {
		int err( ::rmdir( path_.c_str() ) );
		if ( ( err != 0 ) && ( ( errno != ENOENT ) || is_directory( path_ ) ) ) {
			throw HFileSystemException( to_string( "Failed to remove directory `" ).append( path_ ).append( "'" ) );
		}
	} else {
	}
}

find_result find( yaal::hcore::HString const& in, yaal::hcore::HRegex const& pattern_,
		int minDepth_, int maxDepth_, FILE_TYPE::enum_t fileType_ ) {
	find_result result;
	HFSItem p( in );
	if ( !! p ) {
		if ( p.is_file() ) {
			if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( in ) ) {
				result.push_back( in );
			}
		} else if ( p.is_directory() ) {
			for ( HFSItem::HIterator it( p.begin() ), end( p.end() ); it != end; ++ it ) {
				if ( it->is_file() ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
				} else if ( it->is_directory() ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::DIRECTORY ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
					if ( maxDepth_ > 0 ) {
						find_result sub( find( it->get_path(), pattern_, minDepth_ > 0 ? minDepth_ - 1 : 0, maxDepth_ - 1 ) );
						result.insert( result.end(), sub.begin(), sub.end() );
					}
				}
			}
		}
	}
	return ( result );
}

}

}

}


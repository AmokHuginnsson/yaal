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

#define YAAL_USES_STAT 1

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "filesystem.hxx"
#include "stringalgo.hxx"
#include "hfsitem.hxx"
#include "hcore/hcall.hxx"

#undef YAAL_USES_STAT

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace filesystem {

namespace path {

path_t const ROOT = "/";
path_t const CURRENT = ".";
path_t const PARENT = "..";
path_t const SEPARATOR_STR = "/";

}

typedef FileSystem this_type;

namespace {
bool const initHFileSystemException __attribute__((used)) = HFileSystemException::decode_errno();
}

namespace {

bool do_stat( struct stat* s, path_t const& path_ ) {
	M_PROLOG
	::memset( s, 0, sizeof ( *s ) );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	bool success( ( ::stat( path_.raw(), s ) == 0 ) || ( ::lstat( path_.raw(), s ) == 0 ) );
	if ( ! success && ( errno != ENOENT ) ) {
		throw HFileSystemException( to_string( "Cannot acquire metadata for `" ).append( path_ ).append( "'" ) );
	}
	return ( success );
	M_EPILOG
}

}

path_t current_working_directory( void ) {
	char* cwd( ::getcwd( nullptr, 0 ) );
	path_t p( cwd );
	::free( cwd );
	return ( p );
}

path_t normalize_path( path_t const& path_ ) {
	M_PROLOG
	HString path( path_ );
	bool absolute( ! path.is_empty() && path[0] == '/' );
	int origLen( -1 );
	while ( origLen != static_cast<int>( path.get_length() ) ) {
		origLen = static_cast<int>( path.get_length() );
		path.replace( "/./", path::SEPARATOR_STR );
		path.replace( "//", path::SEPARATOR_STR );
	}
	int pos( 0 );
	int back( 0 );
	while ( ( back = static_cast<int>( path.find( "/../", pos ) ) ) != HString::npos ) {
		pos = back + 1;
		int prev( static_cast<int>( path.find_last( path::SEPARATOR, back > 0 ? back - 1 : 0 ) ) );
		if ( prev != HString::npos ) {
			if ( ! ( ( ( back - prev ) == 3 ) && ( path[prev + 1] == '.' ) && ( path[prev + 2] == '.' ) ) ) {
				path.erase( prev + 1, back - prev + 3 );
				pos = prev;
			}
		} else if ( ! path.is_empty() && ( path.find( "../" ) != 0 ) && ( path.find( "./" ) != 0 ) ) {
			path.erase( pos = 0, back + 3 + ( absolute ? 0 : 1 ) );
		}
	}
	if ( path.find( "/..", path.get_length() - 3 ) != HString::npos ) {
		int prev( static_cast<int>( path.find_last( path::SEPARATOR, path.get_length() - 4 ) ) );
		if ( prev != HString::npos ) {
			if ( ! ( ( ( back - prev ) == 3 ) && ( path[prev + 1] == '.' ) && ( path[prev + 2] == '.' ) ) ) {
				path.erase( prev ? prev : 1 );
			}
		} else if ( ! path.is_empty() && ( path.find( "../" ) != 0 ) && ( path.find( "./" ) != 0 ) ) {
			path.erase( absolute ? 1 : 0 );
		}
	}
	if ( path.find( "./" ) == 0 ) {
		path.erase( 0, 2 );
	}
	if ( ( ( path.get_length() > 1 ) || ! ( absolute || path.is_empty() ) ) && ( path[path.get_length() - 1] == path::SEPARATOR ) ) {
		path.erase( path.get_length() - 1 );
	}
	if ( path.find( "/.", path.get_length() - 2 ) != HString::npos ) {
		if ( path.get_length() != 2 ) {
			path.erase( path.get_length() - 2 );
		} else {
			path.erase( absolute ? 1 : 0 );
		}
	} else if ( path == "/.." ) {
		path.erase( absolute ? 1 : 0 );
	} else if ( path == path::CURRENT ) {
		path.clear();
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
	return ( do_stat( &s, path_ ) && S_ISDIR( s.st_mode ) );
	M_EPILOG
}

bool is_symbolic_link( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	return ( do_stat( &s, path_ ) && S_ISLNK( s.st_mode ) );
	M_EPILOG
}

bool is_regular_file( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	return ( do_stat( &s, path_ ) && S_ISREG( s.st_mode ) );
	M_EPILOG
}

bool is_other( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	return ( do_stat( &s, path_ ) && ! ( S_ISREG( s.st_mode ) || S_ISDIR( s.st_mode ) || S_ISLNK( s.st_mode ) ) );
	M_EPILOG
}

i64_t file_size( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	return ( do_stat( &s, path_ ) ? static_cast<i64_t>( s.st_size ) : -1 );
	M_EPILOG
}

path_t dirname( path_t const& path_ ) {
	M_PROLOG
	path_t dname( path_ );
	bool degenerated( false );
	if ( dname.is_empty() ) {
		degenerated = true;
	}	else if ( dname != path::ROOT ) {
		dname.trim_right( path::SEPARATOR_STR.raw() );
		if ( ! dname.is_empty() ) {
			int long delimPos( dname.find_last( path::SEPARATOR ) );
			if ( delimPos != HString::npos ) {
				dname.erase( delimPos > 0 ? delimPos : 1 );
			} else {
				degenerated = true;
			}
		} else {
			M_ASSERT( !"bad code path"[0] );
		}
	}
	if ( degenerated ) {
		dname.assign( "." );
	}
	return ( dname );
	M_EPILOG
}

path_t basename( path_t const& path_ ) {
	M_PROLOG
	path_t bname( path_ );
	if ( ( bname != path::ROOT )
		&& ( bname != path::CURRENT )
		&& ( bname != path::PARENT ) ) {
		bname.trim_right( path::SEPARATOR_STR.raw() );
		int long delimPos( bname.find_last( path::SEPARATOR ) );
		if ( delimPos != HString::npos ) {
			bname.shift_left( delimPos + 1 );
		}
	}
	return ( bname );
	M_EPILOG
}

path_t readlink( path_t const& path_ ) {
	M_PROLOG
	HString path;
	int len( 0 );
	static int const MIN_BUF_SIZE( 8 );
	int alloc( MIN_BUF_SIZE );
	HChunk buffer;
	do {
		alloc <<= 1;
		buffer.realloc( alloc, HChunk::STRATEGY::EXACT );
		len = static_cast<int>( ::readlink( path_.raw(), buffer.get<char>(), static_cast<size_t>( alloc ) ) );
	} while ( len >= alloc );
	if ( len < 0 ) {
		throw HFileSystemException( "readlink failed: `"_ys.append( path_ ).append( "'" ) );
	}
	path.assign( buffer.get<char>(), len );
	return ( path );
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
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	if ( directoryModification_ == DIRECTORY_MODIFICATION::EXACT ) {
		int err( ::mkdir( path.c_str(), static_cast<mode_t>( mode_ ) ) );
		if ( ( err != 0 ) && ( ( errno != EEXIST ) || ! is_directory( path ) ) ) {
			throw HFileSystemException( to_string( "Failed to create directory `" ).append( path ).append( "'" ) );
		}
	} else {
		typedef HArray<yaal::hcore::HString> file_names_t;
		file_names_t fileNames( string::split<file_names_t>( path, "/", HTokenizer::SKIP_EMPTY ) );
		path.assign( path[0] == '/' ? "/" : "" );
		bool real( false );
		for ( yaal::hcore::HString const fn : fileNames ) {
			if ( fn != ".." ) {
				real = true;
			}
			path.append( fn );
			if ( real ) {
				create_directory( path, mode_, DIRECTORY_MODIFICATION::EXACT );
			}
			path.append( "/" );
		}
	}
	return;
	M_EPILOG
}

void chmod( path_t const& path_, u32_t mode_ ) {
	M_PROLOG
	if ( ::chmod( path_.raw(), static_cast<mode_t>( mode_ ) ) < 0 ) {
		throw HFileSystemException( "chmod failed: `"_ys.append( path_ ).append( "'" ) );
	}
	return;
	M_EPILOG
}

void chdir( path_t const& path_ ) {
	M_PROLOG
	if ( ::chdir( path_.raw() ) < 0 ) {
		throw HFileSystemException( "chdir failed: `"_ys.append( path_ ).append( "'" ) );
	}
	return;
	M_EPILOG
}

void remove_directory( path_t const& path_, DIRECTORY_MODIFICATION directoryModification_ ) {
	HString path( normalize_path( path_ ) );
	M_ENSURE( ! path.is_empty() );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	if ( directoryModification_ == DIRECTORY_MODIFICATION::EXACT ) {
		int err( ::rmdir( path.c_str() ) );
		if ( ( err != 0 ) && ( ( errno != ENOENT ) || is_directory( path ) ) ) {
			throw HFileSystemException( to_string( "Failed to remove directory `" ).append( path ).append( "'" ) );
		}
	} else {
		find_result fr( find( path, ".*" ) );
		find_result::iterator lim(
			yaal::stable_partition( fr.begin(), fr.end(),
				[]( path_t const& p ) {
					return ( ! is_directory( p ) );
				}
			)
		);
		for_each( fr.begin(), lim, ptr_fun( &remove ) );
		for_each( fr.rbegin(), HReverseIterator<find_result::iterator>( lim ), call( &remove_directory, _1, DIRECTORY_MODIFICATION::EXACT ) );
		remove_directory( path_, DIRECTORY_MODIFICATION::EXACT );
	}
}

find_result find( yaal::hcore::HString const& in, yaal::hcore::HString const& pattern_,
		int minDepth_, int maxDepth_, FILE_TYPE::enum_t fileType_ ) {
	HRegex regex( pattern_ );
	return ( find( in, regex, minDepth_, maxDepth_, fileType_ ) );
}

find_result find( yaal::hcore::HString const& in, yaal::hcore::HRegex const& pattern_,
		int minDepth_, int maxDepth_, FILE_TYPE::enum_t fileType_ ) {
	find_result result;
	HFSItem p( in );
	if ( !! p ) {
		if ( is_directory( p.get_path() ) ) {
			for ( HFSItem::HIterator it( p.begin() ), end( p.end() ); it != end; ++ it ) {
				if ( is_directory( it->get_path() ) ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::DIRECTORY ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
					if ( maxDepth_ > 0 ) {
						find_result sub( find( it->get_path(), pattern_, minDepth_ > 0 ? minDepth_ - 1 : 0, maxDepth_ - 1 ) );
						result.insert( result.end(), sub.begin(), sub.end() );
					}
				} else {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
				}
			}
		} else {
			if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( in ) ) {
				result.emplace_back( in );
			}
		}
	}
	return ( result );
}

}

}

}


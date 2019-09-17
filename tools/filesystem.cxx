/* Read yaal/LICENSE.md file for copyright and licensing information. */

#define YAAL_USES_STAT 1

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

#include "config.hxx"

#ifdef HAVE_UTIME_H
#include <utime.h>
#elif defined( HAVE_SYS_UTIME_H )
#include <sys/utime.h>
#else
#error No utime.h found on this system.
#endif

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "filesystem.hxx"
#include "stringalgo.hxx"
#include "hfsitem.hxx"
#include "hcore/hqueue.hxx"
#include "hcore/hcall.hxx"

#undef YAAL_USES_STAT

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::string;

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

void do_stat( struct stat* s, path_t const& path_, bool resolve_ = true ) {
	M_PROLOG
	::memset( s, 0, sizeof ( *s ) );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	HUTF8String utf8( path_ );
	if ( ! ( ( resolve_ && ( ::stat( utf8.c_str(), s ) == 0 ) ) || ( ::lstat( utf8.c_str(), s ) == 0 ) ) ) {
		throw HFileSystemException( to_string( "Cannot acquire metadata for `" ).append( path_ ).append( "'" ) );
	}
	return;
	M_EPILOG
}

}

path_t current_working_directory( void ) {
	char* cwd( ::getcwd( nullptr, 0 ) );
	path_t p( cwd );
	::free( cwd );
	return ( p );
}

bool is_absolute( path_t const& path_ ) {
	M_PROLOG
	return ( ! path_.is_empty() && ( path_.front() == path::ROOT.front() ) );
	M_EPILOG
}

path_t normalize_path( path_t const& path_ ) {
	M_PROLOG
	HString path( path_ );
#ifdef __MSVCXX__
	path.replace( "\\", "/" );
#endif /* #ifdef __MSVCXX__ */
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
	HUTF8String utf8( path_ );
	int err( ::access( utf8.c_str(), F_OK ) );
	if ( ( err != 0 ) && ( errno != ENOENT ) ) {
		throw HFileSystemException( to_string( "Failed to determine `" ).append( path_ ).append( "'s ontological status." ) );
	}
	return ( err == 0 );
}

FILE_TYPE file_type( path_t const& path_, bool resolve_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_, resolve_ );
	FILE_TYPE ft( FILE_TYPE::REGULAR );
	if ( S_ISDIR( s.st_mode ) ) {
		ft = FILE_TYPE::DIRECTORY;
	} else if ( S_ISLNK( s.st_mode ) ) {
		ft = FILE_TYPE::SYMBOLIC_LINK;
	} else if ( S_ISSOCK( s.st_mode ) ) {
		ft = FILE_TYPE::SOCKET;
	} else if ( S_ISFIFO( s.st_mode ) ) {
		ft = FILE_TYPE::FIFO;
	} else if ( S_ISCHR( s.st_mode ) ) {
		ft = FILE_TYPE::CHARACTER_DEVICE;
	} else if ( S_ISBLK( s.st_mode ) ) {
		ft = FILE_TYPE::BLOCK_DEVICE;
	}
	return ( ft );
	M_EPILOG
}

char const* file_type_name( FILE_TYPE fileType_ ) {
	char const* name( "unknown" );
	switch ( fileType_ ) {
		case ( FILE_TYPE::REGULAR ):          name = "regular";   break;
		case ( FILE_TYPE::DIRECTORY ):        name = "directory"; break;
		case ( FILE_TYPE::SYMBOLIC_LINK ):    name = "symlink";   break;
		case ( FILE_TYPE::SOCKET ):           name = "socket";    break;
		case ( FILE_TYPE::FIFO ):             name = "fifo";      break;
		case ( FILE_TYPE::CHARACTER_DEVICE ): name = "chardev";   break;
		case ( FILE_TYPE::BLOCK_DEVICE ):     name = "blockdev";  break;
	}
	return ( name );
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
	do_stat( &s, path_, false );
	return ( S_ISLNK( s.st_mode ) );
	M_EPILOG
}

bool is_regular_file( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_ );
	return ( S_ISREG( s.st_mode ) );
	M_EPILOG
}

bool is_other( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_ );
	return ( ! ( S_ISREG( s.st_mode ) || S_ISDIR( s.st_mode ) || S_ISLNK( s.st_mode ) ) );
	M_EPILOG
}

i64_t file_size( path_t const& path_ ) {
	M_PROLOG
	struct stat s;
	do_stat( &s, path_ );
	return ( static_cast<i64_t>( s.st_size ) );
	M_EPILOG
}

path_t dirname( path_t const& path_ ) {
	M_PROLOG
	path_t dname( path_ );
#ifdef __MSVCXX__
	dname.replace( "\\", "/" );
#endif /* #ifdef __MSVCXX__ */
	bool degenerated( false );
	if ( dname.is_empty() ) {
		degenerated = true;
	}	else if ( dname != path::ROOT ) {
		dname.trim_right( path::SEPARATOR_STR );
		M_ASSERT( ! dname.is_empty() );
		int long delimPos( dname.find_last( path::SEPARATOR ) );
		if ( delimPos != HString::npos ) {
			dname.erase( delimPos > 0 ? delimPos : 1 );
		} else {
			degenerated = true;
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
#ifdef __MSVCXX__
	bname.replace( "\\", "/" );
#endif /* #ifdef __MSVCXX__ */
	if ( ( bname != path::ROOT )
		&& ( bname != path::CURRENT )
		&& ( bname != path::PARENT ) ) {
		bname.trim_right( path::SEPARATOR_STR );
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
		HUTF8String utf8( path_ );
		len = static_cast<int>( ::readlink( utf8.c_str(), buffer.get<char>(), static_cast<size_t>( alloc ) ) );
	} while ( len >= alloc );
	if ( len < 0 ) {
		throw HFileSystemException( "readlink failed: `"_ys.append( path_ ).append( "'" ) );
	}
	path.assign( buffer.get<char>(), len );
	return ( path );
	M_EPILOG
}

void remove( path_t const& path_ ) {
	HUTF8String utf8( path_ );
	int err( ::unlink( utf8.c_str() ) );
	if ( ( err != 0 ) && ( errno != ENOENT ) ) {
		throw HFileSystemException( to_string( "Failed to remove: `" ).append( path_ ).append( "'" ) );
	}
	return;
}

void rename( path_t const& old_, path_t const& new_ ) {
	HUTF8String oldUtf8( old_ );
	HUTF8String newUtf8( new_ );
	int err( ::rename( oldUtf8.c_str(), newUtf8.c_str() ) );
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
		HUTF8String utf8( path );
		int err( ::mkdir( utf8.c_str(), static_cast<mode_t>( mode_ ) ) );
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
	HUTF8String utf8( path_ );
	if ( ::chmod( utf8.c_str(), static_cast<mode_t>( mode_ ) ) < 0 ) {
		throw HFileSystemException( "chmod failed: `"_ys.append( path_ ).append( "'" ) );
	}
	return;
	M_EPILOG
}

void chdir( path_t const& path_ ) {
	M_PROLOG
	HUTF8String utf8( path_ );
	if ( ::chdir( utf8.c_str() ) < 0 ) {
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
		HUTF8String utf8( path );
		int err( ::rmdir( utf8.c_str() ) );
		if ( ( err != 0 ) && ( ( errno != ENOENT ) || is_directory( path ) ) ) {
			throw HFileSystemException( to_string( "Failed to remove directory `" ).append( path ).append( "'" ) );
		}
	} else {
		paths_t fr( find( path, ".*" ) );
		paths_t::iterator lim(
			yaal::stable_partition( fr.begin(), fr.end(),
				[]( path_t const& p ) {
					return ( ! is_directory( p ) );
				}
			)
		);
		for_each( fr.begin(), lim, ptr_fun( &remove ) );
		for_each( fr.rbegin(), HReverseIterator<paths_t::iterator>( lim ), call( &remove_directory, _1, DIRECTORY_MODIFICATION::EXACT ) );
		remove_directory( path_, DIRECTORY_MODIFICATION::EXACT );
	}
}

paths_t find( path_t const& in, yaal::hcore::HString const& pattern_,
		int minDepth_, int maxDepth_, FIND_TYPE::enum_t fileType_ ) {
	HRegex regex( pattern_ );
	return ( find( in, regex, minDepth_, maxDepth_, fileType_ ) );
}

paths_t find( path_t const& in, yaal::hcore::HRegex const& pattern_,
		int minDepth_, int maxDepth_, FIND_TYPE::enum_t fileType_ ) {
	paths_t result;
	HFSItem p( in );
	if ( !! p ) {
		if ( is_directory( p.get_path() ) ) {
			for ( HFSItem::HIterator it( p.begin() ), end( p.end() ); it != end; ++ it ) {
				if ( is_directory( it->get_path() ) ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FIND_TYPE::DIRECTORY ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
					if ( maxDepth_ > 0 ) {
						paths_t sub( find( it->get_path(), pattern_, minDepth_ > 0 ? minDepth_ - 1 : 0, maxDepth_ - 1, fileType_ ) );
						result.insert( result.end(), sub.begin(), sub.end() );
					}
				} else {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FIND_TYPE::REGULAR_FILE ) && pattern_.matches( it->get_path() ) ) {
						result.emplace_back( it->get_path() );
					}
				}
			}
		} else {
			if ( ( minDepth_ == 0 ) && ( fileType_ & FIND_TYPE::REGULAR_FILE ) && pattern_.matches( in ) ) {
				result.emplace_back( in );
			}
		}
	}
	return ( result );
}

HString glob_to_re( yaal::hcore::HString const& globStr_ ) {
	HString globRE( "^" );
	bool escaped( false );
	for ( code_point_t c : globStr_ ) {
		if ( escaped || ( ( c != '\\' ) && ( c != '*' ) && ( c != '?' ) ) ) {
			globRE.push_back( '['_ycp );
			globRE.push_back( c );
			globRE.push_back( ']'_ycp );
			escaped = false;
		} else if ( c == '\\' ) {
			escaped = true;
		} else if ( c == '*' ) {
			globRE.append( ".*" );
		} else {
			globRE.push_back( '.'_ycp );
		}
	}
	globRE.push_back( '$'_ycp );
	return ( globRE );
}

namespace {

struct OScan {
	filesystem::path_t _path;
	int _part;
	OScan( filesystem::path_t const& path_, int part_ )
		: _path( path_ )
		, _part( part_ ) {
	}
	OScan( OScan const& ) = default;
	OScan( OScan&& ) = default;
};

}

yaal::tools::filesystem::paths_t glob( path_t const& path_ ) {
	HString globChars( "*?" );
	if ( path_.find_one_of( globChars ) == HString::npos ) {
		return ( paths_t( { path_ } ) );
	}
	paths_t result;
	tokens_t pathParts( split<>( filesystem::normalize_path( path_ ), filesystem::path::SEPARATOR ) );
	typedef HQueue<OScan> scan_t;
	scan_t scan;
	scan.emplace( "", 0 );
	if ( pathParts.front().is_empty() ) {
		pathParts.erase( pathParts.begin() );
		scan.back()._path.assign( "/" );
	}
	HString trialPath;
	while ( ! scan.is_empty() ) {
		OScan trial( scan.front() );
		scan.pop();
		HString const& p( pathParts[trial._part] );
		trialPath.assign( ! trial._path.is_empty() ? trial._path : "./" );
		if ( ! trial._path.is_empty() && ( trial._path.back() != filesystem::path::SEPARATOR ) ) {
			trial._path.append( filesystem::path::SEPARATOR );
		}
		if ( p.find_one_of( globChars ) != HString::npos ) {
			HRegex globRE( glob_to_re( p ) );
			HFSItem dir( trialPath );
			if ( ! dir ) {
				continue;
			}
			++ trial._part;
			HString name;
			HString newMatch;
			for ( HFSItem const& f : dir ) {
				name.assign( f.get_name() );
				if ( ! globRE.matches( name ) || ( ( trial._part < pathParts.get_size() ) && ! f.is_directory() ) ) {
					continue;
				}
				newMatch.assign( trial._path ).append( name );
				if ( trial._part < pathParts.get_size() ) {
					scan.emplace( newMatch, trial._part );
				} else {
					result.push_back( newMatch );
				}
			}
		} else {
			trial._path.append( p );
			++ trial._part;
			if ( trial._part < pathParts.get_size() ) {
				scan.emplace( trial );
			} else {
				result.push_back( trial._path );
			}
		}
	}
	sort( result.begin(), result.end() );
	return ( result );
}

void update_times( path_t const& path_, yaal::hcore::HTime const& modTime_, yaal::hcore::HTime const& accessTime_ ) {
	HUTF8String utf8( path_ );
	utimbuf rawTimes = {
		static_cast<time_t>( yaal_epoch_to_unix_epoch( accessTime_.raw() ) ),
		static_cast<time_t>( yaal_epoch_to_unix_epoch( modTime_.raw() ) )
	};
	if ( ::utime( utf8.c_str(), &rawTimes ) != 0 ) {
		throw HFileSystemException( to_string( "Failed to update times for: `" ).append( path_ ).append( "'" ) );
	}
}

}

}

}


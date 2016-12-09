/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  tools/filesystem.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/filesystem.hxx
 * \brief Declarations of file system handling functions.
 */

#ifndef YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED
#define YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hregex.hxx"

namespace yaal {

namespace tools {

namespace filesystem {

enum class FILE_TYPE {
	REGULAR,
	DIRECTORY,
	SYMBOLIC_LINK,
	SOCKET,
	FIFO,
	CHARACTER_DEVICE,
	BLOCK_DEVICE
};

struct FileSystem {};
typedef yaal::hcore::HExceptionT<FileSystem> HFileSystemException;
typedef yaal::hcore::HString path_t;

path_t normalize_path( path_t const& );

path_t current_working_directory( void );
bool exists( path_t const& );
FILE_TYPE file_type( path_t const&, bool = false );
char const* file_type_name( FILE_TYPE );
bool is_directory( path_t const& );
bool is_symbolic_link( path_t const& );
bool is_regular_file( path_t const& );
bool is_other( path_t const& );
i64_t file_size( path_t const& );
void remove( path_t const& );
void rename( path_t const&, path_t const& );
path_t dirname( path_t const& );
path_t basename( path_t const& );
path_t readlink( path_t const& );
void chmod( path_t const&, u32_t );
void chdir( path_t const& );

namespace path {

extern path_t const ROOT;
extern path_t const CURRENT;
extern path_t const PARENT;
static char const SEPARATOR = '/';

}

enum class DIRECTORY_MODIFICATION {
	EXACT,
	RECURSIVE
};

void create_directory( path_t const&, u32_t, DIRECTORY_MODIFICATION = DIRECTORY_MODIFICATION::EXACT );
void remove_directory( path_t const&, DIRECTORY_MODIFICATION = DIRECTORY_MODIFICATION::EXACT );

typedef yaal::hcore::HArray<path_t> find_result;

struct FIND_TYPE {
	typedef enum {
		REGULAR_FILE = 1,
		DIRECTORY = 2,
		ALL = REGULAR_FILE | DIRECTORY
	} enum_t;
};

find_result find( yaal::hcore::HString const&, yaal::hcore::HString const&,
		int = 0, int = meta::max_signed<int>::value, FIND_TYPE::enum_t = FIND_TYPE::ALL );
find_result find( yaal::hcore::HString const&, yaal::hcore::HRegex const&,
		int = 0, int = meta::max_signed<int>::value, FIND_TYPE::enum_t = FIND_TYPE::ALL );

}

}

}

#endif /* #ifndef YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED */

